#ifndef GL1_MSG_G_H
#define GL1_MSG_G_H
/*========================================================================
                      GSM GPRS NPL1 MSG LAYER HEADER FILE
DESCRIPTION
   This file defines the interfaces to the GSM/GPRS NPL1 MSG LAYER. The GSM/GPRS
   NPL1 message layer abstracts all Non-Portable Layer 1 concepts. The message
   layer mainly deals processing that spans over multi-frames or blocks.

Copyright (c) 2001-2015 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_msg_g.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
06/06/16     km      CR1017936 FR 35777: Enable Samsung 'Big Data' feature on 8917 - GERAN GL1
08/06/16   hd       CR1023028 Use a new parameter to gl1_msg_snr_to_bep() indicate TCH channel mode
19/10/15   br       CR926522 GL1 to Pass the frequency offset to rotator to correct in GBTA.
09/10/15   akh      CR912421 FN/QS not correct for active ttg, sleep ttg resp delayed by 1s
16/04/15   ak       CR820145 Problems with empty BA list during ded mode and with HMSC tests
05/06/15   cs       CR808438 Ensure any pending TT requests are sent before cell
                    information is updated
30/03/15   nm       CR765891 Allowing TA for TDS during PS RACH
19/03/15   jk       CR810476:VAMOS2 BFI gating algorithm should be applied only for TCH loop back A mode
31/03/15   am       CR814147 Don't skip acq abort in find_bcch state
16/12/14   jk       CR753423:LIF for Idle,voice and data
04/02/15   am       CR790448 Re-set meas_aborted_while_retrieving
12/01/14   ja       CR775188 GL1 changes to receive EPD preference
02/12/14   cja      CR760267 For X2G SCH decode structure make sch_data an array and add metrics
14/10/14   br       CR732289 Change in TCH/HS bfi algorithm to consider prev hard rx_qual
17/11/14   cs       CR701894 Enhanced Page Detection Support
04/11/14   df       CR734490 Rework the wake up recovery mechanism
19/08/14   pjr      CR711506 Support for AMR Partial Decode and DTX detection FR19667
03/10/14   aga      CR732455 Changes required to tune away framework for TH 1.0 single sim build
13/08/13   dg       CR485683: In dedicated & packet transfer mode,Reject tone if its offset
                    is less than starting of FCCH window
23/09/14   cja      CR728745 Always clear IR mem when enable IR
17/09/14   sjv      CR724558 For PM, TRM band-reg and release at frame level granularity for perf. optimization
09/09/14   jk       CR697308:GL1 changes to support HLin LLin sawless feature
12/09/14   nm       CR723570 GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
25/07/14   hd       CR699946 Pass correct channel type to the function l1_log_burst_metrics() for FR/HR
20/05/14   sk       CR659786 Ensure yielded monitor not considered for averaging
07/08/14   cja      CR643648 Desense for channel adjacent to spur
05/08/14   df       CR536059 Allow Asynchronous sleep for all idle activities
10/07/14   jk       CR688587:GL1 changes to support LIF feature
09/07/14   pjr      CR685199 Added extra check to ensure WTR hop is not triggered after ACCESS MODE is aborted
27/05/14   sp       CR670592: Update Rxlev reported from FCCH results
20/06/14   pjr      DATA+MMS feature - USF utilisation calculation
08/05/14   cgc      CR642356 fix prototype for tch_rfacch_indicator() added to header.
17/04/14   jk       CR649527:handling frequency hopping and DTX with Shifted SACCH for all cases
08/04/14   gk       CR641603 Ignore yielded power monitor results
04/04/14    cah     CR643764 VSTMR multi-sim restruct and FEATURE_MSIM_VSTMR removal
02/01/14   jk       CR595130: GL1 changes to support VAMOS-II
22/01/13   ws       CR602047 - Allow MVS/GHDI to operate on SUB2 if VS driver is not present
19/12/13   jk       CR503359 - GL1 changes to support Rx diversity
05/12/13   am       CR581771 Ensure frequency correction to re-selection cell is always applied
19/11/13   ssh/ip   CR547956 DSDS/DSDA GL1 timeline Opt (parallelize the RF config. with GL1 warm-up)
27/11/13   cja      CR521061 Remove unused RF API and mainline FEATURE_GSM_RFA_TASK.
21/11/13   cja      CR455527 Mainline or remove features to tidy up code.
07/08/13   jk       CR490807:Support for shifted SACCH
11/10/13   js       CR556573 Invaild releasing of VS buffers
01/10/13   sk       CR552259 CR sync from 1.0/1.1
16/09/13   cs       CR503042 Updated XO RGS handling for Triton
28/08/13   cja      CR515629 Check schedule OK for handover TT
23/08/13   sk       CR533467 Partial QBTA bringup on 2.0 in DSDS
02/08/13   cs       CR522369 Include residual XO drift handling in CR509747
16/07/13   zc       CR470197 Replace memcpy with memscpy
29/07/13   sk       CR519663 Partial QBTA bringup changes
12/07/13   cs       CR509747 Apply frequency drift to TRM_DENIED SUB(s)
11/07/13   nm       CR510055 - trm_suspension when pwr meas aborted in Retrieving state
20/06/13   cs       Major Triton TSTS Syncup
24/05/13   cs       TSTS Updates
16/05/13   ab       CR487396 - Support EGPRS IR Reduced Window in GFW
28/05/13   ap       CR461268: DSDS XO management + async time drift problem
20/05/13   npt      CR417466 - Ensure that opt3 is not kicking in
                    if change to GSM only mode is being processed
16/05/13   pjr      CR487402 removed vs_msg_chan_type, vs_msg_voc_type and
                    added gl1_msg_is_l1_vocoder_control_enabled_local
29/04/13   ws       CR481359 enable GTA support
03/04/13   pjr      CR470940 moved VS interface prototype to gl1_hw_vs_g.h
07/03/13   dv       CR461080: Introduction of new GSM VS interface
14/02/13   pjr      CR452272 added support to set multisim mode at activation time
04/01/13   og       CR427574. Make sure the PDTCH handler is active before scheduling
                    G2T searches.
11/12/12   cja      CR423646 Store initial acq XO value for use by MPH_SELECT_SPECIFIC_BCCH_REQ
07/12/12   cja      CR429173 Remove Dime compiler warnings
22/06/12   pg       CR367679: Modify TCH/FS & TCH/HS BFI algs to use pre-mimo SNR
21/06/12   cja      CR372395 Clear cipher enabled flag on panic reset
31/05/12   ab       CR 366880 RX and TX Time Power averaging Feature for DSDA as part of SAR Req.
25/05/12   pg       Add support for RF Task
08/06/12   cja      Fix compiler warning
24/04/12   npt      CR354790 - Remove old sleep debug (under DEBUG_GL1_SLEEP) as it is not needed
                    after FEATURE_GL1_SLEEP_DEBUG_STRUCT is introduced
26/03/12   jj       CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33
13/03/12   ws       CR335057 eSACCH buffer flush and restore after inter-cell handovers
08/03/12   ip       CR326699 - Code changed for wake up recovery mechanism
06/03/12   pg       CR319120: Ensure latest DTX setting gets applied after vocoder init
24/03/11   cja      CR280412 For H/O failure remove time tracking on failed cell
29/06/11   nt       DSDS CR289454: ASYNC should ignore timing adjustments for CS and PS connections
14/06/11   cja      CR264690 Acquisition frequency capture range larger
22/03/11   ap       CR279430 DSDS Timing value wraps casusing frame misalignment
10/11/10   ws       CR 254488 - Use higher timeslots for AFC in packet transfer mode
                    if lowest timesot has poor SNR
24/03/11   cja      CR280412 For H/O failure remove time tracking on failed cell
04/01/11   dv       Featurise fix for CR265055
03/12/10   dv       CR265055 NAS/GL1 get out of sync due to codec change due to
                    HO occurring before call connect
09/12/10   cs       CR264682 Add support for asynch dsp completion interrupt on DSDS
14/01/11   ab       Resolve Klocwork Buffer Overflow errors
09/12/10   cs       CR264682 Add support for asynch dsp completion interrupt
07/12/10   ab       For HSMC , WS reduction to 256 and need not be half as 320.
                    Thus Physical address space for MS CLASS 10,12,33 is same=256
30/11/10   ab       Support EGPRS WS REDUCTION during allocation of dynamic memory in start
                    gsm mode request when FEATURE_GSM_EGPRS_IR_OPT/PER TBF is defined.
14/09/10   og       CR255453 Setup the serving cell arfcn and band before WCDMA ID search.
21/07/10   cs       CR246418 Allow disabling of SAIC F/W processing in idle
23/07/10   ip       CR245474 Added Sleep debug code
11/05/10   cja      CR237386 For BEP use burst snr, not pre-mimo for high SNR.
27/04/10   cs       CR236460 Latest AFC\XO handling updates
10-04-01   tjw      Add customer.h and comdef.h to all SU API files
06/04/10   cja      Setting of mdsp feature enable to match DSP functionalty.
16/03/10   og       Further g2x and x2g updates.
08/02/10   cja      Update XO
01/02/10   cs       Fix previous checkin featurisation for MOB builds
20/01/10   cs       CR221032 21.3.1 failure with SAIC enabled as AFC/TT not updating
03/07/09   sjw      Support Extended BCCH parameter for l1_compute_tc()
06/06/09   ab       Feature FEATURE_GSM_EGPRS_IR_OPT for LCU
26/05/09   cs       CR167848 fix for expanded afc_freq I/F to mDSP
29/05/09   ws       FEATURE_GSM_MDSP_AEQ phase 1 for all GMSK bursts
09/04/09   og       Seperate the dedicated and transfer SCH scheduling decisions
                    CR174438.
30/03/09   ws       Added FEATURE_GSM_GPRS_QDSP6 and FEATURE_GSM_GPRS_POSEIDON
                    to replace FEATURE_GSM_GPRS_MUSTANG
12/03/09   og       FEATURE_XO support.
26/02/09   cs       Add initial XO support
30/01/09   ip       CR 167135 Added sleep debug code
15/01/09   nt       CR167218 - changes to restore old rot and pdm values after HO fails
07/01/09   nt       Added prototype for gl1_msg_get_saic_state
08/12/08   cs       Add new gl1_hw_debug.h include from gl1_hw.h
03/12/08   ws       Added FEATURE_GSM_DYNAMIC_SAIC
14/11/08   ws       CR 152636 - increase MAX_TCH_FS_OCTETS to 34 since endianess has
                    changed on 8K. Also changed on other targets for consitency.
28/10/08   agv      CR161919 - Fix for UL Transmit Slots not reset at end of
                    E/GPRS TBF for GSM call.
01/10/08   cs       Fix the packet length for AMR_WB to be 38 octets
04/09/08   dv       CR153510: Fix for Manual NW search failure
28/08/08   cs       Implement the Fast Rx Sweep functionality for FTM
27/06/08   nt       CR147984 - REV2:- Final changes for TCXO reset
16/06/08   nt       CR147984:- Changes for TCXO reset.
12/05/08   dv       Added FEATURE_GSM_AMR_WB - L1 component
27/03/08   ap       CR 139993 Changed prototype gl1_msg_get_rxqual_full to take an argument
14/03/08   ip       Added PSHO chnages in feature FEATURE_GPRS_PS_HANDOVER
15/01/08   ws       Added suport for ESIAC logging and SAIC AFC in PTM
14/01/08   cs       Allow for separate IQ logging and saving
14/12/07   ws       merged LCU Bringup and demo changes from 6260 branch
09/11/07   nf       Added FEATURE_GSM_R_SACCH
06/11/07   ip       CR115620 Removed use of FEATURE_INTER_RAT_HO_OPT around
                    gl1_msg_disable_vocoder(), required for Worldphone targets
22/10/07   ws       Add support for ESAIC NV 2508 item control
17/09/07   agv      Added I/Q logging to code base.
31/08/07   agv      CR127468 - Fix for SAIC TT handling in Dedicated.
31/08/07   agv      CR127469 - Temp fix for Timing Alignment for Handover.
08/07/07   ws       CR - 124046 Open loopback mode C when we need to send FACCH
                    and add invalid channel type
03/08/07   agv      CR123023 - Combined fix for AFC/TT/AGC.
06/07/07   og       Allow W to inform G about the frequency offset of the G cell
                    in W2G non-blind handover scenarios. CR122524.
12/06/07   og       Upgrade the FEATURE_GSM_L1_HFREQ_ERR_NCELL_SCH_DECODE to
                    include dedicated TCH NCELL SCH attempts.
23/05/07   agv      CR118309 - Large RSSI variation initial fix.
20/04/07   cs       CR117035 Report HR DTX correctly for SACCH Measurement reports
08/12/06   og       Fix pch block decode failures due to interruption by BPLMN
                    search. Fixes CR107369.
09/11/06   og       Fix to CR92174, handle IDLE NCELL SCH / BCCH decode at high frequency
                    error conditions.
12/10/06   og       Merge fix to CR101667 / SR860781. Ensure the correct freq err
                    is applied before attenpting to decode BCCH after pscan.
21/09/06   og       Reset only the cs part of lower layers when connecting DTM
                    assignment in packet transfer (EDTM route).
31/07/06   agv      CR85810 NB AB TSC, and TA fix.
06/04/06   agv      Feature SAIC MIMO elements added to the burst metrics.
26/01/06    og      Additions to allow TA to be included in NCELL ACQ offset
                    calculations.
11/22/05   gfr      Change tx scheduled and discarded callbacks to take
                    structures instead of just message data pointers.  Return
                    the timeslot and offset_to_sob for scheduled transmits.
11/10/05   gfr      Updated prototype for gl1_msg_rx_ncell_fcch
11/07/05   gfr      Fix prototype for gl1_msg_cfg_fcch_sch_list
10/02/05   gfr      Added prototype for gl1_msg_abort_rx_tch_amr_fr
09/27/05   gfr      Add gl1_msg_rach_type to distinguish between normal,
                    handover and extended RACHs.
08/31/05   gfr      Replace gl1_msg_metrics_rpt with gl1_defs_metrics_rpt
08/30/05   gfr      QLint cleanup
08/26/05   gfr      Added prototypes for gl1_msg_abort_pbcch and gl1_msg_abort_pccch
08/21/05   gfr      Changed prototype for gl1_msg_set_band_group
08/19/05   gfr      Added gl1_msg_set_band_group
08/17/05   gfr      Added gl1_msg_init_frame_tick_info and gl1_msg_inc_rach_frame_cnt
07/29/05   gfr      DTM support
06/08/05   gfr      Added gl1_msg_inc_dl_ptcch_frame_count
04/21/05   gfr      Removed gl1_msg_conditional_abort_dl_ptcch
04/14/05   gfr      Removed obselete store/retrieve_tx_lev, changed ta to uint8
03/29/05   dp       Inter-RAT audio optimizations
03/16/05   gfr      Added loopback_tn to the srb loopback interface
03/15/05   dp       Added support for enhanced measurement reporting
03/10/05   yh       Removed FEATURE_GSM_PLT for BEP functions.
03/07/05   gfr      Add gl1_msg_IR_set_tfi
02/05/05   bm       BG PLMN search optimizations
02/02/05   gfr      Removed gl1_msg_IR_config
01/24/05   pp       Added channel type GL1_MSG_ONSET
01/14/05   gfr      Added SNR to BEP conversion function (gl1_msg_snr_to_bep)
01/12/05   gfr      Added prototypes for packet mode TT and AFC
01/05/05   dp       Merged audio optimizations to mainline
12/07/04   yhong    Lint cleanup
11/30/04   gfr      Support for BEP processing for enhanced measurement reporting
11/01/04   yhong    Changed NCell Abort API to take a parameter
08/18/03   gsc      AMR BFI Algorithm changes.
08/12/04   gsc      Added support for Loopback I.
08/09/04   gfr      Mainlined FEATURE_GSM_GPRS_L1_SERV_CELL_MEAS
07/22/04   gfr      EGPRS support.
06/12/04   gfr      Removed gl1_msg_cch_ft_update prototype, added ARFCN and
                    offset_to_sob to SCH decode report for logging.
06/25/04   gsc      Cleaned up function prototype for gl1_msg_compute_bfi_amr_fr.
                    Added function prototype for gl1_msg_compute_bfi_amr_hr.
06/21/04   dp       Added prototype for gl1_msg_cap_reported_power_level
05/20/04   gfr      Mainlined FEATURE_COMMON_PWR_HANDLER
05/06/04   gfr      Combine phase and freq value for acq results.
04/29/04   gfr      Increase the size of the TCH data buffer.
03/31/04   dp       Fix to pass in flag to indicate if FACCH available in CSD
03/31/04   dp       Fix to factor hard RXQUAL into BFI computation
03/18/04   gsc      Some more Lint cleanup.
03/05/04   gfr      Lint cleanup.
02/26/04   gfr      Removed some obselete APIs.
02/17/04   gfr      GSM rotators support.
01/28/04   gw       Added prototype for gl1_msg_SNR_to_dBx16().
11/25/03   gsc      Corrected gl1_msg_is_channel_mode_changed() declaration to
                    remove compiler warning.
11/11/03   dp       Added function prototypes for gl1_msg_channel_mode_changed
                    and gl1_msg_is_channel_mode_changed
11/05/03    gfr     Support for quad-band.
10/22/03   gsc      Added function prototypes to support vocoder control by
                    L1 for AMR calls.
10/21/03   gsc      Added SMS CB support.
10/16/03   gfr      Fix gl1_msg_adjust_time_stamp to take negative offsets.
10/03/03   dp       Added function prototypes to support vocoder control by
                    L1
08/29/03   jc       Removed ypf functions no longer needed by PLT.
08/11/03   gsc      Added c_over_i to gl1_msg_metrics_rpt for AMR.
07/25/03   gfr      Compiler warning cleanup.
07/14/03   gsc      Added mods to metrics_cb for AMR
07/08/03   bk       Added support for partial tones
07/01/03   gsc      Prototype change for gl1_msg_tx_tch_amr_fr and
                    gl1_msg_tx_tch_amr_hr to support AMR loopbacks
06/23/03   gfr      Compiler warning cleanup.
05/21/03   gw       Added fields to gl1_msg_metrics_rpt to support early
                    paging decode.
05/13/03   gfr      Common power handler support.
05/07/03   gsc      Added AMR support
04/14/03   dp       Added prototype for gl1_msg_vocoder_released and
                    gl1_msg_is_vocoder_initialized
04/14/03   ATM      Added dtx_used logic: CR28812
03/25/03   jc/drl   Added prototype for gl1_msg_abort_power_measure
03/21/03   gsc      Added num_bursts parameter of type uint8 to gl1_msg_get_snr.
                    Updated prototype for gl1_msg_compute_bfi_ufi.
03/12/03   pjr      Added prototype for GPRS function gpl1_gprs_calc_rxqual_lev.
02/26/03   dp       Added half rate support featurized under
                    FEATURE_GSM_HALF_RATE
02/24/03   gfr      Lint cleanup.
01/31/03   gfr      Move rxlev handler prototype to gl1_msgi.h.
01/15/03   gfr      Added first_rx_tn to the rxlev handler.
01/13/03   mk       Added support for BFI algorithm.
01/02/03   gw       Added another parameter to gl1_msg_cfg_dl_ccch(), to
                    support getting PCH block from mDSP one frame earlier.
11/22/02   mk       Added multiband AFC support
11/07/02   gfr      Moved internal GPRS functions to gl1_msgi.h
11/03/02   tb       Added ability to skip first frame of block for INTERRAT.
10/27/02   JC       Added acquisition AGC support.
10/01/02   ATM      Added MVS blanking support.
10/10/02   gr       Changed GPRS TX interface to accept TXLEV vs. power index.
09/25/02   ATM      Added MVS support from ymc.
09/18/02   gr       Added conditional abort for PTCCH to support TBF suspension.
09/09/02   gr       Moved max message size constants to gl1_defs.h
09/06/02   gr       Changed GPRS rxlev handler to take a timestamp for AGC
09/06/02   gr       Added GPRS DL abort functionality.
08/29/02   JC       Chaned TX interfaces to accept TXLEV vs. power index
                    to support multi-band operation and because it provides
                    cleaner abstraction. Also, modified gl1_msg_cfg_ul_rach()
                    and gl1_msg_tx_rach() interfaces.
08/13/02   dp       Interface changes for parallel decodes
08/23/02   plm      Added void argument to gl1_msg_reset_tch_fifos() function
                    declaration to eliminate compiler warnings.
08/13/02   dp       Added prototype for gl1_msg_reset_tch_fifos
07/11/02   gw       Removed field power_dBm_x16 from reports.  All reports
                    returning rx power levels now have a field called
                    pwr_dBm_x16.
06/28/02   bk       Added abort DLPTCCH  and changed the signature for NCELL
                    FCCH and NCELL BCCHto support GPRS.
06/28/02   thh      Featurize this whole module under FEATURE_GSM.  This is
                    required to allow dualmode build to make single mode
                    (WCDMA) executable.
06/14/02   dp       Changed tx aborts for TCH, FACCH and SDCCH to return
                    boolean when abort is complete
06/06/02   mk       Support for aborts on all channels.
06/04/02   bbd      Changed FEATURE_GPLT to FEATURE_GSM_PLT
                    Changed FEATURE_GPRS to FEATURE_GSM_GPRS
05/21/02   gw       Modified power measure definitions to support Idle Mode
                    pscan.
05/17/02   dp       Changed gl1_msg_vocoder_rx interface to pass in TAF flag
05/14/02   mk       Updated RXQUAL reporting.
05/10/02   JC       Modified gl1_msg_cfg_power_measure() API to support
                    IDLE monitor/ and serving power scan.
05/08/02   JC       Added offset_to_sob field in tch_facch_decode_rpt and
                    gl1_msg_metrics_rpt so that the burst and message
                    metrics can be correlated in a clean fashion for logging.
04/24/02   JC       Added an API to adjust the local time stamp when in DRX
                    so that we don't upset the slew rate fcn of AGC.
04/19/02   JC       Added rx_gain_range to gl1_msg_metrics_rpt to enhance
                    logging capabilities.  Also, added arfcn to burst
                    metrics report for logging.
04/16/02   bk       Added abort APIs for PRACH and PDTCH adn to set med. acc mode
03/29/02   mk       Updated afc/tt.
03/29/02   dp       Merged Gabe's BFI interface changes.  Changed gl1_msg_rx_abort_sacch
                    interface to return boolean
03/27/02   JC       Modified gl1_msg_cfg_power_measure() API so that it
                    can support serving and NCELL power measures.
03/26/02   dp       Added gl1_msg_ncell_sch_pos_in_acq_window()
03/20/02   JC       Added AGC support to gl1_msg_do_power_meas().
03/20/02   dp       Changed txlevs to tx power index
03/15/02   mk       Added support for direct frequency correction on DCHs.
03/14/02   bk       Changed the signature of GPRS monitor functions
03/06/02   dp       Support to frequency hop access bursts
03/05/02   JC       Added power_dBm_x16 to gl1_msg_metrics_rpt.
02/28/02   mk       Added initial support for RXQUAL_FULL and RXQUAL_SUB.
02/15/02   mk       Updated afc/tt.
02/14/02   gw       Replaced gl1_msg_reset_sync_rx_seq_nums with more general
                    gl1_msg_reset, plus other changes to support AGC.
02/13/02   jc       Added prototype to support DTX.
02/09/02   bk       Added AGC related GPRS i/f changes and moved gprs meas struct here
02/07/02   gw       Added some prototypes.
01/14/01   JC       Added types for power measure and AGC.  Modified several
                    acquisition interfaces to accomodate new AGC design.
                    Removed timestamp parm from gl1_msg_frame_tick_notification.
02/01/02   mk       Updated to support the latest afc/tt algorithms.
01/23/02   bk       Added a bsic param for PACCH and changed tsc to bsic for
                    PTCCH channel.
01/16/01   mk       Added support for afc/tt.
01/14/02   sg       Added power measurement support
12/18/01   dp       Changed acquisition offset returned by DSP to int32 to handle
                    negative offsets.
12/13/01   gw       Changes to support message/frame layer acquisition.
12/12/01   gw       Changed interface for getting debug (logging) data.
                    Added support for returning the YPF word returned by the
                    mDSP with decoded data.  To use, FEATURE_GPLT must be
                    defined.  Moved prototype for gl1_msg_reset_sync_rx_seq_nums
                    from gl1_hw.h
12/07/01   mk       Added support for sacch26 abort.
11/9/01    bk       Added GPRS support
10/31/01   gw/dp    Added support for logging debug info.  Added support for
                    returning metrics in dedicated mode.  Added reserved byte
                    to decode reports with odd number of data octets to ensure
                    word alignment at DSP
10/31/01   dp       Modified decode report structs to contain a hdr of type
                    gl1_defs_rx_hdr_type
10/24/01   dp       Added MAX_FCCH_OCTETS
10/16/01   gw       Changes to support reporting RSSI in dBm.  Added some more
                    comments.
10/10/01   gw       Added support for power measurements and Ncell SCH and
                    BCCH reception.
09/27/01   gw/jc    Added support for passing the channel type into
                    gl1_msg_rx_ccch and returning it in the callback. Added
                    RACH support.
09/25/01   mk/gw    Added CCCH/BCCH and syncrx metrics support.
08/31/01   jc       Initial version.
===========================================================================*/


/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gl1_defs.h"
#include "gl1_defs_g.h"
#include "gl1_sys_algo.h"
#include "sys.h"
#include "sys_v.h"
#include "gl1_hw.h"
#include "gl1_hw_g.h"
#include "gl1_hw_debug.h"
#include "gl1_msg.h"
#include "l1_log_api.h"
#include "mdsp_dtm.h"

#ifdef FEATURE_MVS
#include "mvs.h"
#endif

#include "geran_multi_sim.h"

/*========================================================================== */
/* Check gl1_msg.h FEATURE assumptions */
/*========================================================================== */

#ifndef FEATURE_GSM_AMR         /* ON */
  #pragma warning ("FEATURE_GSM_AMR assumed defined but isn't")
#endif

#ifndef FEATURE_MVS             /* ON */
  #pragma warning ("FEATURE_MVS assumed defined but isn't")
#endif


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define GSM_HO_TIMING_ADJ
#define GL1_MSG_NUM_ABORT_MSG  5

#define BFI_SNR_THRESHOLD_SAIC_FR  362  /* 1.5dB in Q8 (for TCH/FS) */
#define BFI_SNR_THRESHOLD_SAIC_HR  362  /* 1.5dB in Q8 (for TCH/HS) */

#define MAX_FACCH_OCTETS         23
#define MAX_SACCH_OCTETS         23
#define MAX_SDCCH_OCTETS         23
#define MAX_BCCH_OCTETS          23
#define MAX_CCCH_OCTETS          23

#define MAX_TCH_HS_OCTETS        16
#define MAX_TCH_HS_OCTETS_LEN    14

/* Max SCH Octets is 4 */
#define MAX_SCH_OCTETS            GL1_MAX_SCH_OCTETS

#ifdef FEATURE_GSM_AMR
#define MAX_TCH_OCTETS_AMR        37
#define MAX_AMR_HDR_LEN           4
#define MAX_FACCH_OCTETS_AMR      25

#ifdef FEATURE_GSM_AMR_WB
/*32 for 12.65k WB code + 4 for AMR header info */
#define MAX_TCH_OCTETS_AMR_WB     MAX_TCH_OCTETS_AMR
#endif

#define MAX_TCH_HS_OCTETS_AMR     20

#define MAX_RATSCCH_OCTETS        10
#define MAX_SID_UPDATE_OCTETS     10
#define MAX_SID_FIRST_OCTETS      4
#endif /* FEATURE_GSM_AMR */



#define MAX_CSTCH_OCTETS          37
#define MAX_TCH_FS_OCTETS         34

/* Define the maximum size of a TCH message, add 1 where necessary to make
   the size word aligned */
#if defined(FEATURE_GSM_AMR)
  #define MAX_TCH_OCTETS (MAX_TCH_OCTETS_AMR + 1)
  #define CHECK_MAX_TCH_OCTETS_MVS MAX_TCH_OCTETS
#else
  #define CHECK_MAX_TCH_OCTETS_MVS MAX_TCH_FS_OCTETS
  #if defined(FEATURE_DATA_GCSD)
    #define MAX_TCH_OCTETS (MAX_CSTCH_OCTETS + 1)
  #else
    #define MAX_TCH_OCTETS (MAX_TCH_FS_OCTETS + 1)
  #endif
#endif

/* Check the size of the buffer declared in the API */
#if CHECK_MAX_TCH_OCTETS_MVS > MAX_TCH_OCTETS_MVS
#error CHECK_MAX_TCH_OCTETS_MVS > MAX_TCH_OCTETS_MVS
#endif

#define MAX_FCCH_OCTETS     14

#define GL1_ACQ_HDR "GL1_XO_ACQ: "

/* Below this the mDSP fine_freq is invalid */
#define GL1_MSG_AFC_VALID_SNR_THRESH (322)

#define RX_POWER_FLOOR               (-32767)

/* VS interface */
 #define GAS_ID_IS_MULTIMODE_SUB(gas_id)     (( (gas_id < GERAN_ACCESS_STRATUM_ID_2) ? TRUE:FALSE ) )
 #define GAS_ID_IS_GSM_ONLY_MODE_SUB(gas_id) (( (gas_id >= GERAN_ACCESS_STRATUM_ID_2) ? TRUE:FALSE ) )
 #define OTHER_GSM_ONLY_SUB_INACTIVE(gas_id) ((l1_tsk_buffer[gas_id].l1_state == L1_INACTIVE_MODE)? TRUE:FALSE)

/*Frame Duration during which PM will be active*/
#define ACTIVE_PM_FRAMES      2
/*==============================================================================
                    GSM and GPRS Data Structures
==============================================================================*/

 /* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* enum describing the various Downlink GPRS logical channels */
typedef enum
{
  GL1_MSG_PBCCH,
  Gl1_MSG_NCELL_PBCCH,
  GL1_MSG_PCCCH,
  GL1_MSG_DL_PTCCH,
  GL1_MSG_DL_PACCH_PDTCH
} gl1_msg_dl_chn_type;


/* enum describing the various Uplink GPRS logical channels */
typedef enum
{
  GL1_MSG_PRACH,
  GL1_MSG_UL_PTCCH,
  GL1_MSG_UL_PACCH_PDTCH
} gl1_msg_ul_chn_type;

typedef enum
{
  ASYNC_ACTIVITY_SUSPENSION,
  ASYNC_ACTIVITY_GBTA
}async_ccch_type_t;

#ifdef FEATURE_GSM_AMR
/* enum for AMR frame type definition */
typedef enum
{
  GL1_MSG_AMR_SPEECH_GOOD,          /* Good speech frame              */
  GL1_MSG_AMR_SPEECH_DEGRADED,      /* Degraded speech frame          */
  GL1_MSG_AMR_ONSET,                /* Onset speech frame             */
  GL1_MSG_AMR_SPEECH_BAD,           /* Corrupt speech frame (bad CRC) */
  GL1_MSG_AMR_SID_FIRST,            /* First silence descriptor       */
  GL1_MSG_AMR_SID_UPDATE,           /* Comfort noise frame            */
  GL1_MSG_AMR_SID_BAD,              /* Corrupt SID frame (bad CRC)    */
  GL1_MSG_AMR_NO_DATA,              /* Nothing to transmit            */
  GL1_MSG_AMR_FRAME_TYPE_MAX,
  GL1_MSG_AMR_FRAME_TYPE_UNDEF = GL1_MSG_AMR_FRAME_TYPE_MAX  /* undefined */
} gl1_msg_amr_frame_type;


/* AMR frame mode (frame rate) definitions */
typedef enum
{
  GL1_MSG_AMR_MODE_0475,    /* 4.75 kbit /s */
  GL1_MSG_AMR_MODE_0515,    /* 5.15 kbit /s */
  GL1_MSG_AMR_MODE_0590,    /* 5.90 kbit /s */
  GL1_MSG_AMR_MODE_0670,    /* 6.70 kbit /s */
  GL1_MSG_AMR_MODE_0740,    /* 7.40 kbit /s */
  GL1_MSG_AMR_MODE_0795,    /* 7.95 kbit /s */
  GL1_MSG_AMR_MODE_1020,    /* 10.2 kbit /s */
  GL1_MSG_AMR_MODE_1220,    /* 12.2 kbit /s */
#ifdef FEATURE_GSM_AMR_WB
  GL1_MSG_AMR_WB_MODE_0660,   /* 6.60 kbit /s */
  GL1_MSG_AMR_WB_MODE_0885,   /* 8.85 kbit /s */
  GL1_MSG_AMR_WB_MODE_1265,   /* 12.65 kbit/s */
#endif
  GL1_MSG_AMR_MODE_MAX,
  GL1_MSG_AMR_MODE_UNDEF = GL1_MSG_AMR_MODE_MAX  /* undefined */
} gl1_msg_amr_mode_type;


typedef struct
{
  gl1_msg_amr_frame_type frame;
  gl1_msg_amr_mode_type mode;
} gl1_msg_amr_ul_frame_info_type;


typedef struct
{
  gl1_msg_amr_frame_type frame;
  gl1_msg_amr_mode_type mode;
} gl1_msg_amr_dl_frame_info_type;


typedef union
{
  gl1_msg_amr_ul_frame_info_type ul_info;
  gl1_msg_amr_dl_frame_info_type dl_info;
} gl1_msg_amr_frame_info_type;

#endif /* FEATURE_GSM_AMR */


/* Access burst type */
typedef enum
{
   GL1_MSG_RACH_NORMAL,
   GL1_MSG_RACH_HANDOVER,
   GL1_MSG_RACH_EXTENDED
} gl1_msg_rach_type;


/* Frame Tick Notification information */
typedef struct
{
   boolean next_frame_is_dl_block_start;
   boolean next_frame_is_ul_block_start;
   boolean next_frame_is_dl_pdch_block_start;
   boolean next_frame_is_ul_pdch_block_start;
   boolean next_frame_is_dl_ptcch_sacch_block_start;
   boolean next_frame_is_ul_ptcch_sacch_block_start;
   boolean next_frame_is_acq_ncell_block_start;
   boolean next_frame_is_pwr_meas_block_start;
   boolean next_frame_is_int_meas_block_start;
} gl1_msg_frame_tick_type;


/* Downlink information passed back, when the message layer is aborted */
typedef struct
{
  uint8               *msg_data;
  uint8               *metrics_info;
  uint8               *log_info;
  gl1_msg_dl_chn_type  chn_type;
} gl1_msg_rx_aborted_type;


/*==============================================================================
                    GSM Data Structures
==============================================================================*/

typedef uint16 gl1_msg_tag_type;

/***********************************************************************/

/* Decoded Data reports passed back on up to portable layer 1 */

typedef struct
{
   gl1_defs_rx_hdr_struct hdr;
   uint8                  data[MAX_TCH_OCTETS];
   uint16                 offset_to_sob; /* start of burst offset,in frames */
} tch_facch_decode_rpt;



typedef struct
{
   gl1_defs_rx_hdr_struct hdr;
   uint8               data[MAX_SACCH_OCTETS];

  /* This reserved byte is added because the number of data
      octets is odd.   Since the DSP handles everything in words,
      we need to pad the odd byte to a word with an extra byte
      before the DSP fills in the results.  Otherwise a byte from
      the adjacent memory location would get overwritten
   */
   uint8                reserved;
   uint16               offset_to_sob; /* start of burst offset,in frames */
} cch_decode_rpt;


typedef struct
{
    gl1_msg_tag_type    tag;
    boolean             own_paging_block;
    ARFCN_T             arfcn;
    gl1_defs_rx_hdr_struct hdr;
    uint8               data[MAX_CCCH_OCTETS];

    /* This reserved byte is added because the number of data
       octets is odd.   Since the DSP handles everything in words,
       we need to pad the odd byte to a word with an extra byte
       before the DSP fills in the results.  Otherwise a byte from
       the adjacent memory location would get overwritten
    */
    uint8                reserved;
    uint16               offset_to_sob; /* start of burst offset,in frames */
    boolean              extbcch; /* whether the data was read from Extended BCCH */
}  ccch_decode_rpt;


typedef struct
{
    gl1_defs_rx_hdr_struct  hdr;
    uint8                   data[MAX_SCH_OCTETS];
    ARFCN_T                 arfcn;
    uint16                  offset_to_sob; /* start of burst offset,in frames */
} sch_decode_rpt;


typedef struct
{
   int32   tone_offset;
   int32   start_delay;
   boolean tone_detected;
   int32   coarse_freq;
   int32   fine_freq;
   int32   afc_freq;
}  fcch_decode_rpt;


typedef struct
{
   boolean success;
   uint16  id;
   ARFCN_T arfcn;              /* channel on which we acquired   */
   dBx16_T pwr_dBm_x16;        /* Rx level of signal we acquired */
   int32   time_offset;
   int32   coarse_freq;
   int32   fine_freq;
   int32   afc_freq;
   uint16  snr;
   uint8   sch_data[MAX_SCH_OCTETS];
   uint32  rssi;
#ifdef GERAN_L1_HLLL_LNASTATE
   uint16  jdetReading;
#endif /*GERAN_L1_HLLL_LNASTATE*/
} acq_decode_rpt;


typedef union
{
   ccch_decode_rpt         ccch;
   cch_decode_rpt          cch;
   tch_facch_decode_rpt    tch_facch;
   sch_decode_rpt          sch;
   fcch_decode_rpt         fcch;
   acq_decode_rpt          acq;
} gl1_msg_decode_rpt;

/* Report pointers for rx callbacks */
typedef struct
{
   gl1_msg_decode_rpt      *data;
   gl1_defs_metrics_rpt    *metrics[GL1_DEFS_FRAMES_IN_BLK_PERIOD];
} gl1_msg_rx_rpt;


typedef struct
{
    /* Results of power measure */
   uint32                       num_pwr_meas;
   dBx16_T                      *pwr_dBm_x16;
   boolean                      stopped;

    /* Passed in parameters returned to caller */
   uint32                       num_arfcns;
   ARFCN_T                      *arfcns;
   sys_algo_agc_T               **agc_ptrs;
   boolean                      *yielded;
} gl1_msg_pwr_meas_result_type;


/* Structure used to pass back information on scheduled tx data */
typedef struct
{
   gl1_defs_tn_type   tn;
   gl1_defs_tn_type   camp_tn;
   uint8              offset_to_sob;
   uint8              *msg_data;
} gl1_msg_tx_scheduled_data_type;


/* Structure used to pass back information on discarded tx data */
typedef struct
{
   uint8              offset_to_sob;
   uint8              *msg_data;
} gl1_msg_tx_discarded_data_type;

/* Structure used to pass back information on usf utilisation */
typedef struct
{
  uint8               num_scheduled_blocks;
  uint8               num_blanked_blocks;
  uint32              num_consecutive_blanked_blocks;
} gl1_msg_usf_utilisation_data_type;

#ifdef FEATURE_GSM_DTM
/* Which dedicated mode AFC/TT functions to use - for DTM */
typedef enum
{
   AFC_TT_MODE_DCH,
   AFC_TT_MODE_PDCH
} gl1_msg_afc_tt_mode;
#endif

#ifdef FEATURE_VAMOS_II
typedef enum
{
  WAIT_AFTER_BLOCK_START,
  WAIT_AFTER_BURST2,
  NO_WAIT
} gl1_vamos2_tch_wait_type;

#define SPEECH_V1_VAMOS2 193
#define SPEECH_V2_VAMOS2 194
#define SPEECH_V3_VAMOS2 195
#define SPEECH_V5_VAMOS2 197

#endif /*FEATURE_VAMOS_II*/

/* States of the RACCH state machine */
typedef enum
{
   RACH_STATE_NULL,
   RACH_STATE_WAITING_FOR_BLOCK_START,
   RACH_STATE_TRANSMITTING
} rach_state_type;


/***********************************************************************/

extern uint16 gl1_get_ncell_start_acq_offset(gas_id_t gas_id);

/* Callback function definitions */

typedef void (*gl1_msg_tx_data_callback)(gas_id_t gas_id);

typedef void (*gl1_msg_rx_data_callback)(gl1_msg_decode_rpt *rpt, gas_id_t gas_id);

typedef void (*gl1_msg_rx_metrics_callback_tch)(gl1_defs_metrics_rpt *rpt[],
                                                uint16 chan, gas_id_t gas_id);

typedef void (*gl1_msg_rx_metrics_callback)(gl1_defs_metrics_rpt *rpt[], gas_id_t gas_id);

typedef void (*gl1_msg_tx_rach_callback)(uint16 content, gas_id_t gas_id);

typedef void (*gl1_msg_pwr_meas_cb_type)(gl1_msg_pwr_meas_result_type *rpt, gas_id_t gas_id);

typedef void (*gl1_msg_pwr_meas_abort_cb_type)(gas_id_t gas_id);

typedef void (*gl1_msg_rx_log_callback)(void);

typedef void (*gl1_msg_tx_abort_callback)(gas_id_t gas_id);

typedef void (*gl1_msg_rx_abort_callback)(gas_id_t gas_id);


typedef struct
{
    gl1_msg_tx_data_callback    data_cb;
    void                        (*metrics_cb)(void);
    void                        (*log_cb)(void);
    gl1_msg_tx_abort_callback   abort_cb;
} gl1_msg_tx_cb_type;


typedef struct
{
    gl1_msg_rx_data_callback    data_cb;
    gl1_msg_rx_metrics_callback_tch tch_metrics_cb;
    gl1_msg_rx_metrics_callback metrics_cb;
    gl1_msg_rx_log_callback     log_cb;
    gl1_msg_rx_abort_callback   abort_cb;
} gl1_msg_rx_cb_type;


typedef struct
{
    gl1_msg_tx_rach_callback    data_cb;
    gl1_msg_tx_abort_callback   abort_cb;
} gl1_msg_rach_cb_type;



/***********************************************************************/

typedef enum
{
    GL1_MSG_FS,                 /* Full Rate Speech */
    GL1_MSG_HS,                 /* Half Rate Speech */
    GL1_MSG_EFS,                /* Enhanced Full Rate Speech */
    GL1_MSG_AMR                 /* Adaptive Rate Speech */
} gl1_msg_voc_type;

/************************************************************
 *
 *   Enum to be used in ncell abort parameter.
 *   DELAYED_ABORT is used if the callback is required in a
 *   few frames after the abort is issued. There is a 2 frame
 *   spacing that should not have any NPL1, RF activity.
 *   IMMEDIATE_ABORT is used when in dedicated mode and
 *   packet transfer and it should have the callback returned
 *   right away.
 *
 ************************************************************/
typedef enum
{
    DELAYED_ABORT,
    IMMEDIATE_ABORT,
    GRACEFUL_ABORT
} gl1_msg_ncell_abort_type;



/*==============================================================================
                    GPRS Data Structures
==============================================================================*/


/* Monitoring data types */

/* structure specifying the signal info for RXLEV and interference meas */
typedef struct
{
   ARFCN_T               arfcn;
   sys_algo_agc_T        *agc_ptr;
} gl1_msg_meas_signal_type;


/* Transmitted data notification callback */
typedef void (*gl1_msg_scheduled_data_callback )
(
   gl1_msg_tx_scheduled_data_type msgs_txd[ GL1_DEFS_MAX_UL_MSGS ],
   uint8 num_msg_txd,gas_id_t gas_id

);

/* Discarded data notification callback */
typedef void (*gl1_msg_discarded_data_callback )
(
   gl1_msg_tx_discarded_data_type msgs_discarded[ GL1_DEFS_MAX_UL_MSGS ],
   uint8 num_msg_discarded,gas_id_t gas_id
);

/* USF utilisation notification callback */
typedef void (*gl1_msg_usf_utilisation_callback )
(
   gl1_msg_usf_utilisation_data_type usf_utilisation,
   gas_id_t gas_id
);

/* Interference measure callback */
typedef void (*gl1_msg_int_meas_cb_type )
(
   gl1_defs_int_meas_result_type *int_meas_results,
   gas_id_t gas_id
);


/*------------------------------------------------------------------------------
   structure containing the tx and discarded notification callbacks used by
   Uplink channels
----------------------------------------------------------------------------- */
typedef struct
{
   gl1_msg_scheduled_data_callback  scheduled_callback;
   gl1_msg_discarded_data_callback  discarded_callback;
   gl1_msg_usf_utilisation_callback usf_utilisation_callback;
} gl1_msg_tx_pkt_cb_type;

/* Information on Uplink PACCH/PDTCH signal */
typedef struct
{
  gl1_defs_tn_type tn;
  int16            TXLEV;
} gl1_msg_ul_pacch_pdtch_signal_type;



extern gsm_voc_packet_type gl1_rx_pkt[NUM_GERAN_DATA_SPACES];
extern gsm_voc_packet_type gl1_tx_pkt[NUM_GERAN_DATA_SPACES];

/* =======================================================================
**                   FUNCTIONS COMMON TO GSM AND GPRS
** ======================================================================= */

/*=============================================================================
FUNCTION gl1_msg_set_num_assigned_tx_slots

DESCRIPTION
   Sets the number of assigned uplink slots.  This is used for determining
   the power backoff for multi-slot uplink.  Note that "assigned" is used
   meaning the number of slots in the PUA, whereas "allocated" refers to
   the number of USFs allocated in dynamic allocation.

============================================================================= */
void gl1_msg_set_num_assigned_tx_slots (uint8 num_assigned_tx_slots,gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_acq_tcxo_and_rotator_init

DESCRIPTION This is the setup for the PDM, and Rotator so Ageing is used.


DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_acq_tcxo_and_rotator_init( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_bfi_mod_2

DESCRIPTION
   This triggers bfi AGC updates every 8 frames in dedicated 26.

===========================================================================*/
boolean gl1_msg_bfi_mod_2( gas_id_t gas_id  );

extern mdsp_rf_desense_chan_type gl1_msg_is_arfcn_xo_desense( ARFCN_T arfcn,gas_id_t gas_id );



#if  defined ( FEATURE_GSM_LOG_IQ_SAMPLES )
/*===========================================================================

FUNCTION gl1_msg_trigger_iq

DESCRIPTION
   This triggers IQ Samples if gl1_trigger_iq_state is set true.

===========================================================================*/
extern boolean gl1_msg_trigger_iq( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_set_iq_state

DESCRIPTION
   This sets/resets gl1_trigger_iq_stateto trigger IQ Samples or not.

===========================================================================*/
extern void gl1_msg_set_iq_state( boolean state,gas_id_t gas_id);
#endif /* FEATURE_GSM_LOG_IQ_SAMPLES */

#ifdef GSM_HO_TIMING_ADJ
/*===========================================================================

FUNCTION gl1_msg_tt_ho_adjust

DESCRIPTION Temp workaround for timing error at Handover.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_tt_ho_adjust( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_tt_ho_bursts

DESCRIPTION Temp workaround for timing error at Handover.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 gl1_msg_tt_ho_bursts(gas_id_t gas_id );

#endif


/*===========================================================================

FUNCTION gl1_msg_set_ho_pwr

DESCRIPTION
   This signals the Msg Layer the Handover AGC power to use.

===========================================================================*/
void gl1_msg_set_ho_pwr( int16 agc_pwr,gas_id_t gas_id  );

/*===========================================================================

FUNCTION gl1_msg_rx_tch_hr_sub_chan

DESCRIPTION
  This function allows PL1 via MSG Layer to signal GSM Half Rate Sub Channel

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_rx_tch_hr_sub_chan(uint8 hr_sub_chan, gas_id_t gas_id);

/*=============================================================================
FUNCTION gl1_get_dtx_used_this_period

DESCRIPTION
   This function returns if DTX was used during the current SACCH reporting
   period then resets itself.

DEPENDENCIES
   This expects the SACCH reporting mechanism in L1 to call this each time.

RETURN VALUE
   boolean - value of dtx_used

SIDE EFFECTS
   dtx_used is reset for next reporting period
============================================================================= */
extern boolean gl1_get_dtx_used_this_period( gas_id_t gas_id  );

/*=============================================================================
FUNCTION gl1_re/set_dtx_used_flag

DESCRIPTION
   This function re/set the reporting flag gl1_dtx_used.

DEPENDENCIES
   This mechanism is for use when previous reporting period didn't occur.
   (examples: start/end of call, handover).

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_reset_dtx_used_flag( gas_id_t gas_id  );
extern void gl1_set_dtx_used_flag( gas_id_t gas_id  );

/*=============================================================================
FUNCTION gl1_msg_enable_l1_vocoder_control

DESCRIPTION
   This function enables L1's control of the vocoder

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msg_enable_l1_vocoder_control( gas_id_t gas_id );

/*=============================================================================
FUNCTION gl1_msg_disable_l1_vocoder_control

DESCRIPTION
   This function disables L1's control of the vocoder

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msg_disable_l1_vocoder_control( gas_id_t gas_id );

/*=============================================================================
FUNCTION gl1_msg_is_l1_vocoder_control_enabled_local

DESCRIPTION
   This function returns true if L1 has control on the vocoder

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS
   None
============================================================================= */
extern boolean gl1_msg_is_l1_vocoder_control_enabled_local( gas_id_t gas_id );


/*===========================================================================

FUNCTION gl1_msg_get_tch_loopback

DESCRIPTION
  This function gets the current TCH loopback in operation. If GL1_DEFS_NO_LOOPBACK
  returns then no loopback is currently active

DEPENDENCIES
  None

RETURN VALUE
  current loop_back type

SIDE EFFECTS
  None
===========================================================================*/
extern gl1_defs_loopback_type gl1_msg_get_tch_loopback(gas_id_t gas_id);


/*=============================================================================
FUNCTION gl1_msg_enable_vocoder

DESCRIPTION
   This function invokes ghdi functions to enable the vocoder

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msg_enable_vocoder (gl1_msg_chan_type chan, gl1_msg_voc_type voc_type, gas_id_t gas_id);

/*=============================================================================
FUNCTION gl1_msg_disable_vocoder

DESCRIPTION
   This function invokes mvs functions to disable the vocoder

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msg_disable_vocoder ( gas_id_t gas_id );

#ifdef FEATURE_INTER_RAT_HO_OPT
/*=============================================================================
FUNCTION gl1_msg_acquire_vocoder

DESCRIPTION
   This function invokes mvs functions to acquire the vocoder

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msg_acquire_vocoder ( gas_id_t gas_id );

/*=============================================================================
FUNCTION gl1_msg_release_wcdma_vocoder

DESCRIPTION
   This function invokes mvs functions to disable the WCDMA vocoder

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msg_release_wcdma_vocoder ( gas_id_t gas_id );

/*=============================================================================
FUNCTION gl1_msg_is_vocoder_acquired

DESCRIPTION
   This function is used by NAS to indicate the vocoder has been
   acquired but not enabled yet

=============================================================================*/
boolean gl1_msg_is_vocoder_acquired( gas_id_t gas_id );
#endif


/*=============================================================================
FUNCTION gl1_msg_is_vocoder_initialized

DESCRIPTION
   This function is used by PL1 to determine if the vocoder is
   initialized or not
=============================================================================*/
boolean gl1_msg_is_vocoder_initialized( gas_id_t gas_id );

/*=============================================================================
FUNCTION gl1_msg_is_vocoder_ready

DESCRIPTION
   This function is used by PL1 to determine if the vocoder is
   ready to transmit
============================================================================= */
boolean gl1_msg_is_vocoder_ready( gas_id_t gas_id );

/*=============================================================================
FUNCTION gl1_msg_init_vocoder

DESCRIPTION
   This function disable the vocoder

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_init_vocoder (gl1_msg_chan_type chan, gl1_msg_voc_type voc_type, gas_id_t gas_id);

/*=============================================================================
FUNCTION gl1_msg_stop_vocoder_blanking

DESCRIPTION
   This helper function is used to indicate we should stop performing
   vocoder data blanking because the vocoder on the phone is now ready

============================================================================= */
void gl1_msg_stop_vocoder_blanking( gas_id_t gas_id );

/*===========================================================================
FUNCTION  gl1_msg_init_vocoder_amr_fr_hr

DESCRIPTION


DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void gl1_msg_init_vocoder_amr_fr_hr (gl1_msg_chan_type chan,gas_id_t gas_id);

/*=============================================================================
FUNCTION gl1_msg_channel_mode_change

DESCRIPTION
   This function informs NPL1 if there was a channel change or not
============================================================================= */
void gl1_msg_channel_mode_change( boolean mode_change );

/*=============================================================================
FUNCTION gl1_msg_is_channel_mode_changed

DESCRIPTION
   This function lets PL1 know if there was a channel mode change or not
============================================================================= */
boolean gl1_msg_is_channel_mode_changed( void );

/*=============================================================================
FUNCTION gl1_msg_init

DESCRIPTION
   This function initializes the message layers. Initialization includes
   the following:
   - Initialization of all internal variables
   - Initialization of the frame layer
   The message layer is now ready to handle channel operations and monitoring.


DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_init( gas_id_t gas_id );

/*=============================================================================
FUNCTION gl1_msg_terminate

DESCRIPTION
   Terminates all message layer processing.


DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_terminate( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_init_frame_tick_info

DESCRIPTION
  Initializes the frame tick info structure passed in.  Used to allow
  upper layer to be oblivious to all block start flags except the ones
  it is setting to TRUE.

===========================================================================*/
void gl1_msg_init_frame_tick_info (gl1_msg_frame_tick_type *ftn);

/*===========================================================================

FUNCTION gl1_msg_frame_tick_notification

DESCRIPTION
   Used to notify NPL1 message layer of frame, block, PTCCH and 26/52
   multi-frame idle frame, 51 multi-frame idle frame and SACCH frame boundaries.
   For correct operation of the NPL1 drivers it is imperative that this
   function be called every frame tick. All frame layer processing is done in
   this function.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_frame_tick_notification
(
   const gl1_msg_frame_tick_type *frame_tick_info,gas_id_t gas_id
);

/*===========================================================================

FUNCTION gl1_msg_store_ta

DESCRIPTION
  This function stores the timing advance as provided by the base
  station.  The timing advance changes infrequently about every 104 frames.


DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_store_ta ( uint8 ta,gas_id_t gas_id );


/* =======================================================================
**                  GSM FUNCTIONS
** ======================================================================= */

/*===========================================================================

FUNCTION gl1_msg_fr_vocoder_rx  ,  gl1_msg_fr_vocoder_tx
         gl1_msg_hr_vocoder_rx  ,  gl1_msg_hr_vocoder_tx

DESCRIPTION
  This function is used to transfer data to or from the vocoder.  These
  functions may be called by the task level.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_fr_vocoder_rx (uint8 *data, boolean bfi, boolean taf,
                                   gas_id_t gas_id);
extern void gl1_msg_hr_vocoder_rx (
                                     uint8 *data,
                                     boolean bfi,
                                     boolean ufi,
                                     uint8 sid,
                                     boolean taf,
                                     gas_id_t gas_id);

extern boolean gl1_msg_hr_vocoder_tx (uint8 *data, uint8 datalength, uint8 *packet_available,
                                      gas_id_t gas_id);

extern boolean gl1_msg_fr_vocoder_tx (uint8 *data, uint8 datalength, uint8 *packet_available,
                                      gas_id_t gas_id);


#ifdef FEATURE_DATA_GCSD
/*===========================================================================

FUNCTION gl1_msg_cs_data_rx  ,  gl1_msg_cs_data_tx

DESCRIPTION
  This function is used to transfer data to or from the GSM circuit
  switched data handler.  These functions may be called by the task level.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_cs_data_rx
(
  tch_facch_decode_rpt *curr_rpt,
  tch_facch_decode_rpt **next_rpt_buf,
  int32 status, gas_id_t gas_id
);


extern void gl1_msg_cs_data_tx (uint8 **data, gas_id_t gas_id);

#endif

/*===========================================================================

FUNCTION gl1_msg_tx_tch_fr,   gl1_msg_tx_facch_fr
         gl1_msg_tx_tch_hr,   gl1_msg_tx_facch_hr

DESCRIPTION
  This function is used to transmit a message on the TCH/FACCH on the next
  block boundary.  The message contents should span 4 frames ie 1 block.
  This function may be called at task level but should be called in the block
  before the data is to transmitted.   If data to be transmitted is for
  frame #4,5,6,7, then this function should be completed before start of
  frame #3, ie called in frames 0,1,2.

DEPENDENCIES
  gl1_msg_store_tx_TSC(), gl1_msg_cfg_ul_tch() and gl1_msg_store_ta(),
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_tx_tch_fr ( ARFCN_T ARFCN[4], int16 TXLEV[4], uint8 *data,
#ifdef FEATURE_VAMOS_II
                                gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                                gas_id_t gas_id);

extern void gl1_msg_tx_facch_fr ( ARFCN_T ARFCN[4], int16 TXLEV[4], uint8 *data,
#ifdef FEATURE_VAMOS_II
                                  gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                                  gas_id_t gas_id );
extern void gl1_msg_tx_tch_hr ( ARFCN_T ARFCN[4], int16 TXLEV[4], uint8 *data,
#ifdef FEATURE_VAMOS_II
                                gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                                gas_id_t gas_id);

extern void gl1_msg_tx_facch_hr ( ARFCN_T ARFCN[4], int16 TXLEV[4], uint8 *data,
                                  boolean middle_facch_block,
#ifdef FEATURE_VAMOS_II
                                  boolean middle_facch_block_vamos2,
                                  gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                                  gas_id_t gas_id );

#ifdef FEATURE_DATA_GCSD
extern void gl1_msg_tx_cstch (
                        ARFCN_T ARFCN[4],
                        int16 TXLEV[4],
                        uint8 *cs_data,
                        uint8 *facch_data,
                        boolean facch_available,
                        gas_id_t gas_id);
#endif

/*===========================================================================

FUNCTION gl1_msg_rx_tch_facch_fr
         gl1_msg_rx_tch_facch_hr

DESCRIPTION
  This function is used to receive a message on either the TCH or the FACCH.
  The message contents should span 4 frames ie 1 block.
  This function may be called at task level but should be called in the block
  before the data is to transmitted.   If data to be received is for
  frame #4,5,6,7, then this function should be completed before start of
  frame #3, ie called in frames 0,1,2.

DEPENDENCIES
  gl1_msg_store_rx_TSC(), gl1_msg_cfg_dl_tch_facch() ,
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_rx_tch_facch_fr (
                            ARFCN_T              ARFCN[4],
                            sys_algo_agc_T       *agc_ptr,
#ifdef FEATURE_GSM_RX_DIVERSITY
                            sys_algo_agc_T       *agc_ptr_divrx,
#endif
                            gl1_msg_rx_rpt       *rpt,
#ifdef FEATURE_VAMOS_II
                            gl1_vamos2_tch_wait_type  vamos2_fr_tch_wait_state,
#endif
                            gas_id_t             gas_id);

extern void gl1_msg_rx_tch_facch_hr (
                            ARFCN_T               ARFCN[4],
                            sys_algo_agc_T       *agc_ptr,
#ifdef FEATURE_GSM_RX_DIVERSITY
                            sys_algo_agc_T       *agc_ptr_divrx,
#endif
                            gl1_msg_rx_rpt       *rpt,
                            boolean               facch_only,
#ifdef FEATURE_VAMOS_II
                            gl1_vamos2_tch_wait_type  vamos2_hr_tch_wait_state,
#endif
                            gas_id_t              gas_id );

#ifdef FEATURE_DATA_GCSD
extern void gl1_msg_rx_cstch_facch (
                            ARFCN_T               ARFCN[4],
                            sys_algo_agc_T        *agc_ptr,
#ifdef FEATURE_GSM_RX_DIVERSITY
                            sys_algo_agc_T        *agc_ptr_divrx,
#endif
                            tch_facch_decode_rpt  **cs_rpt,
                            gl1_msg_rx_rpt        *facch_rpt,
                            gas_id_t              gas_id);

#endif

/*===========================================================================

FUNCTION gl1_msg_tx_sacch

DESCRIPTION
  This function is used to transmit a message on the SACCH on the next sacch
  block boundary.  The message contents should span 104 frames ie 1 block.
  This function may be called at task level but should be called in the block
  before the data is to transmitted.

DEPENDENCIES
  gl1_msg_store_tx_TSC(), gl1_msg_cfg_ul_tch() and gl1_msg_store_ta(),
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_tx_sacch ( ARFCN_T ARFCN[4], int16  TXLEV[4], uint8  *data,gas_id_t gas_id );

extern void gl1_msg_tx_sacch51 ( ARFCN_T ARFCN[4], int16  TXLEV[4], uint8  *data,gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_rx_sacch

DESCRIPTION
  This function is used to receive a message on the SACCH on the next sacch
  block boundary.  The message contents should span 104 frames ie 1 block.
  This function may be called at task level but should be called in the block
  before the data is to transmitted.

DEPENDENCIES
  gl1_msg_store_tx_TSC(), gl1_msg_cfg_ul_tch() and gl1_msg_store_ta(),
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_rx_sacch (
                        ARFCN_T         ARFCN[4],
                        sys_algo_agc_T  *agc_ptr,
#ifdef FEATURE_GSM_RX_DIVERSITY
                        sys_algo_agc_T  *agc_ptr_divrx,
#endif
                        gl1_msg_rx_rpt  *rpt,
                        boolean         rsacch_active,
                        gas_id_t        gas_id);


extern void gl1_msg_rx_sacch51 (
                        ARFCN_T         ARFCN[4],
                        sys_algo_agc_T  *agc_ptr,
#ifdef FEATURE_GSM_RX_DIVERSITY
                        sys_algo_agc_T  *agc_ptr_divrx,
#endif
                        gl1_msg_rx_rpt *rpt,
                        gas_id_t       gas_id);

/*===========================================================================

FUNCTION gl1_msg_tx_sdcch

DESCRIPTION
  This function is used to transmit a message on the SDCCH on the next sdcch
  block boundary.  The message contents should span 104 frames ie 1 block.
  This function may be called at task level but should be called in the block
  before the data is to transmitted.

DEPENDENCIES
  gl1_msg_store_tx_TSC(), gl1_msg_cfg_ul_sdcch() and gl1_msg_store_ta(),
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_tx_sdcch ( ARFCN_T ARFCN[4], int16  TXLEV[4], uint8  *data ,gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_rx_sdcch

DESCRIPTION
  This function is used to receive a message on the SDCCH on the next sdcch
  block boundary.  The message contents should span 104 frames ie 1 block.
  This function may be called at task level but should be called in the block
  before the data is to transmitted.

DEPENDENCIES
  gl1_msg_store_tx_TSC(), gl1_msg_cfg_ul_sdcch() and gl1_msg_store_ta(),
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_rx_sdcch (
                        ARFCN_T         ARFCN[4],
                        sys_algo_agc_T  *agc_ptr,
#ifdef FEATURE_GSM_RX_DIVERSITY
                        sys_algo_agc_T  *agc_ptr_divrx,
#endif
                        gl1_msg_rx_rpt *rpt,
                        gas_id_t        gas_id);

/*===========================================================================

FUNCTION gl1_msg_rx_ccch

DESCRIPTION
  This function is used to receive a message on the CCCH and BCCH on the
  next ccch block boundary. This function may be called at task level but
  should be called in the block before the data is to transmitted.

DEPENDENCIES
  gl1_msg_cfg_dl_ccch() must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_rx_ccch
(
   ARFCN_T           ARFCN[GL1_DEFS_FRAMES_IN_BLK_PERIOD],
   sys_algo_agc_T    *agc_ptr,
#ifdef FEATURE_GSM_RX_DIVERSITY_IDLE
   sys_algo_agc_T    *agc_ptr_divrx_ccch, // added CCCH temporarily for diferentiating on the other agc pointers.
#endif /*FEATURE_GSM_RX_DIVERSITY_IDLE*/
   uint8             tag,
   boolean           own_paging_block,
   gl1_msg_chan_type chan,
   gl1_msg_rx_rpt    *rpt,
   gas_id_t           gas_id
);

extern void gl1_msg_rx_cbch
(
   ARFCN_T           ARFCN[GL1_DEFS_FRAMES_IN_BLK_PERIOD],
   sys_algo_agc_T    *agc_ptr,
   int16             timing_offset,
   gl1_msg_rx_rpt    *rpt,
   gas_id_t          gas_id
);

boolean gl1_msg_ccch_is_rx_on_next_frame( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_rx_ncell_bcch

DESCRIPTION
  This function is used to receive a message on the BCCH of a neighbor cell.
  The message is received on the next bcch block boundary on the neighbor
  cell.

DEPENDENCIES
  gl1_msg_cfg_ncell_bcch() must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_rx_ncell_bcch
(
   ARFCN_T         ARFCN[GL1_DEFS_FRAMES_IN_BLK_PERIOD],
   sys_algo_agc_T *agc_ptr,
   uint16          timing_offset,
   gl1_msg_rx_rpt *rpt,
   gl1_msg_chan_type   msg_chan_type,
   async_ccch_type_t   async_ccch_type,
   int32               freq_error,
   gas_id_t        gas_id
);

/*===========================================================================

FUNCTION gl1_msg_rx_ncell_sch

DESCRIPTION
  This function is used to receive an SB  on the SCH of a neighbor cell.

DEPENDENCIES
  gl1_msg_cfg_ncell_sch() must have been called at some time prior.

===========================================================================*/
extern void gl1_msg_rx_ncell_sch
(
   ARFCN_T          ARFCN,
   sys_algo_agc_T  *agc_ptr,
#ifdef FEATURE_GSM_RX_DIVERSITY
   sys_algo_agc_T  *agc_ptr_divrx,
#endif
   uint16           timing_offset,
   int32            freq_error,
   gl1_msg_rx_rpt  *rpt,
   gas_id_t           gas_id
);

/*===========================================================================

FUNCTION gl1_msg_rx_ncell_fcch

DESCRIPTION
  This function is used to receive the FCCH on a neighbor cell.

DEPENDENCIES
  gl1_msg_cfg_ncell_fcch() must have been called at some time prior.

===========================================================================*/
void gl1_msg_rx_ncell_fcch
(
   const gl1_defs_tn_struct* first_tn_ptr,
   const gl1_defs_tn_struct* last_tn_ptr,
   ARFCN_T                   ARFCN,
   sys_algo_agc_T*           agc_ptr,
   gl1_msg_rx_rpt*           rpt,
   gas_id_t                  gas_id
);

/*===========================================================================

FUNCTION gl1_msg_store_cbch_TSC

DESCRIPTION
  This function stores the received training sequence for the CB channel.

===========================================================================*/
extern void gl1_msg_store_cbch_TSC ( uint8 TSC,gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_cfg_ul_tch  ,  gl1_msg_cfg_dl_tch_facch

DESCRIPTION
  This function is used to configure the TSC and callbacks for the TCH.  The
  callbacks will be invoked by the frame tick handler as required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_cfg_ul_tch (
                        uint8 TSC,
                        gl1_msg_tx_cb_type *cb,
                        gas_id_t gas_id );

extern void gl1_msg_cfg_ul_facch (
                        uint8 TSC,
                        gl1_msg_tx_cb_type *cb,
                        gas_id_t gas_id );

extern void gl1_msg_cfg_dl_tch_facch (
                        uint8 TSC,
                        gl1_msg_rx_cb_type *cb,
                        gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_cfg_ncell_bcch

DESCRIPTION
  This function is used to configure the TSC and callbacks for the neighbor
  cell BCCH. The callbacks will be invoked by the frame tick handler as
  required.

===========================================================================*/
extern void gl1_msg_cfg_ncell_bcch
(
   uint8 TSC,
   boolean DRX,
   gl1_msg_rx_cb_type *cb,
   gas_id_t gas_id
);

/*===========================================================================

FUNCTION gl1_msg_cfg_ncell_sch

DESCRIPTION
  This function is used to configure the callbacks for the neighbor
  cell SCH. The callbacks will be invoked by the frame tick handler as
  required.

===========================================================================*/
extern void gl1_msg_cfg_ncell_sch ( gl1_msg_rx_cb_type *cb,gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_cfg_ncell_fcch

DESCRIPTION
  This function is used to configure the callbacks for the neighbor
  cell FCCH. The callbacks will be invoked by the frame tick handler as
  required.

===========================================================================*/
void gl1_msg_cfg_ncell_fcch ( gl1_msg_rx_cb_type *cb,gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_cfg_ul_sacch  ,  gl1_msg_cfg_dl_sacch

DESCRIPTION
  This function is used to configure the TSC and callbacks for the SACCH.  The
  callbacks will be invoked by the frame tick handler as required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_cfg_ul_sacch (
                        uint8 TSC,
                        gl1_msg_tx_cb_type *cb,
                        gas_id_t gas_id );

extern void gl1_msg_cfg_dl_sacch (
                        uint8 TSC,
                        gl1_msg_rx_cb_type *cb,
                        gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_cfg_ul_sdcch  ,  gl1_msg_cfg_dl_sdcch

DESCRIPTION
  This function is used to configure the TSC and callbacks for the SDCCH.  The
  callbacks will be invoked by the frame tick handler as required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_cfg_ul_sdcch (
                        uint8 TSC,
                        gl1_msg_tx_cb_type *cb,gas_id_t gas_id );

extern void gl1_msg_cfg_dl_sdcch (
                        uint8 TSC,
                        gl1_msg_rx_cb_type *cb,gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_cfg_dl_ccch

DESCRIPTION
  This function is used to configure the TSC and callbacks for the CCCH and
  BCCH. The callbacks will be invoked by the frame tick handler as required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_cfg_ccch
(
   uint8 TSC,
   boolean DRX,
   gl1_msg_rx_cb_type *cb,
   gas_id_t gas_id
);

extern void gl1_msg_cfg_cbch
(
   uint8 TSC,
   boolean DRX,
   gl1_msg_rx_cb_type *cb,
   gas_id_t gas_id
);

/*===========================================================================

FUNCTION  gl1_msg_init_for_tch

DESCRIPTION
  This function should be called to perform any initialization that need to
  be done when entering the traffic channel from idle, or rach states.
  The search window size is modified as is the burst size.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_msg_init_tch (gl1_msg_chan_type chan, gl1_msg_voc_type voc_type, gas_id_t gas_id);

/*===========================================================================

FUNCTION  gl1_msg_terminate_tch

DESCRIPTION
  This function should be called to perform any actions that need to
  be done when exiting the traffic channel.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_msg_terminate_tch (gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_align_vfr

DESCRIPTION
  This function sets the hardware such that on the next frame tick, the
  Vocoder Frame Alignment should occur.  This function should be called
  every time the gstmr is reprogrammed to realign the VFR again.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_align_vfr( boolean real_vfr, gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_cfg_ul_rach

DESCRIPTION
  This function is used to configure the parameters that are used with the
  RACH burst.  The callback function will be invoked by the handler when
  the RACH burst has been transmitted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_cfg_ul_rach
(
  uint8                ab_tsc, /* Access Burst TSC */
  uint8                BSIC,
  gl1_msg_rach_type    type,
  gl1_msg_rach_cb_type *cb,
  gas_id_t             gas_id
);

/*===========================================================================

FUNCTION gl1_msg_tx_rach

DESCRIPTION
  This function is used to transmit a message on the SDCCH on the next sdcch
  block boundary.  The message contents should span 104 frames ie 1 block.
  This function may be called at task level but should be called in the block
  before the data is to transmitted.

DEPENDENCIES
  gl1_msg_cfg_ul_rach() must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_tx_rach
(
   ARFCN_T  ARFCN,
   int16    TXLEV,
   uint16   delay,          /* rach frames to delay, nonzero */
   uint16   random_content,
   gas_id_t gas_id
);

boolean gl1_msg_is_rach_tx_on_next_frame( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_inc_rach_frame_cnt

DESCRIPTION
  This function is used to increment the count of rach frames.

===========================================================================*/
void gl1_msg_inc_rach_frame_cnt (gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_abort_rach

DESCRIPTION
  This function is used to terminate any RACH bursts pending.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_abort_rach ( gas_id_t gas_id );
extern boolean gl1_msg_abort_tx_tch_facch_fr( gas_id_t gas_id );
extern boolean gl1_msg_abort_tx_tch_facch_hr( gas_id_t gas_id );
extern boolean gl1_msg_abort_rx_tch_hr( gas_id_t gas_id );
extern boolean gl1_msg_abort_tx_facch_fr( gas_id_t gas_id );
extern void gl1_msg_abort_tx_sacch( gas_id_t gas_id );
extern void gl1_msg_abort_tx_sacch51( gas_id_t gas_id );
extern boolean gl1_msg_abort_tx_sdcch( gas_id_t gas_id );

extern void gl1_msg_abort_rx_sacch( gas_id_t gas_id );
extern void gl1_msg_abort_rx_sacch51( gas_id_t gas_id );
extern void gl1_msg_abort_rx_ccch ( gas_id_t gas_id );
extern void gl1_msg_abort_rx_cbch( gas_id_t gas_id );

extern boolean gl1_msg_abort_rx_tch_fr( gas_id_t gas_id );
extern void gl1_msg_abort_rx_sdcch( gas_id_t gas_id );

/*===========================================================================

FUNCTION  gl1_msg_abort_rx_ncell_bcch

DESCRIPTION
  This function aborts the currently ongoing ncell BCCH rx and invokes the
  registered abort callback when done

DEPENDENCIES
  None

PARAMETERS
  Callback to

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_msg_abort_rx_ncell_bcch(gl1_msg_ncell_abort_type type,gas_id_t gas_id);

/*===========================================================================

FUNCTION  gl1_msg_abort_rx_ncell_fcch

DESCRIPTION
  This function aborts the currently ongoing ncell FCCH rx and invokes the
  registered abort callback when done

DEPENDENCIES
  None

PARAMETERS
  Callback to

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_msg_abort_rx_ncell_fcch(gl1_msg_ncell_abort_type type,gas_id_t gas_id);

/*===========================================================================

FUNCTION  gl1_msg_abort_rx_ncell_sch

DESCRIPTION
  This function aborts the currently ongoing ncell SCH rx and invokes the
  registered abort callback when done

DEPENDENCIES
  None

PARAMETERS
  Callback to

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_msg_abort_rx_ncell_sch(gl1_msg_ncell_abort_type type,gas_id_t gas_id);

/*===========================================================================

FUNCTION  gl1_msg_abort_rx_ncell_fcch_sch

DESCRIPTION
  This function aborts the currently ongoing ncell FCCH/SCH rx and invokes the
  registered abort callback when done

DEPENDENCIES
  None

PARAMETERS
  Callback to

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_msg_abort_rx_ncell_fcch_sch(gl1_msg_ncell_abort_type type, gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_store_ta,   gl1_msg_retrieve_ta

DESCRIPTION
  This function stores/retrieves the timing advance as provided by the base
  station.  The timing advance changes infrequently about every 104 frames.


DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 gl1_msg_retrieve_ta ( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_cfg_pwr_meas

DESCRIPTION
  This function is used to configure the callback for power measurements.
  The callback will be invoked by the frame tick handler as required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean gl1_msg_cfg_pwr_meas
(
   gl1_msg_pwr_meas_cb_type pwr_meas_callback,
   gl1_msg_pwr_meas_abort_cb_type pwr_meas_abort_callback,
   gas_id_t gas_id
);

/*===========================================================================

FUNCTION gl1_msg_do_pwr_meas

DESCRIPTION
  This function starts power measurements over multiple frames.  The first
  measurements are made in the frame following the call.  Pointers to all
  the arrays passed in are returned in the rpt provided to the callback
  function.

  pwr_meas_data  - a pointer to a structure containing the arfcn/agc info
  monitors_per_frame - the number of measurements that should be made each
      frame - one to four
  max_frames_to_measure  - the max number of frames to perform measures on,
      note that results may arrive in up to max_frames_to_measure +
      <2 frame latency> frames.  Pass 0 for no limit.
  pwr_meas_results_info - pointer to the report that will be filled up and passed
      to the callback.  The caller should fill in the pwr_dBm_x16 pointer if
      they are interested in the individual measurements.

DEPENDENCIES
  gl1_msg_cfg_pwr_meas must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_do_pwr_meas
(
   ARFCN_T                       *arfcns,
   sys_algo_agc_T                **agc_ptrs,
   uint32                        num_arfcns,
   uint32                        max_meas_per_frame,
   uint32                        max_frames_to_measure,
   gl1_msg_pwr_meas_result_type  *pwr_meas_results,
   gas_id_t                      gas_id
);


/*===========================================================================

FUNCTION gl1_msg_suspend_pwr_meas

DESCRIPTION
  This functions temporarily suspends power measurements.  The measurements that
  are suspended are scheduled later and are not thrown away but the total number
  of measurements are truncated

DEPENDENCIES
  gl1_msg_cfg_pwr_meas must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_suspend_pwr_meas ( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_resume_pwr_meas

DESCRIPTION
  This functions resumes the suspended power measurements.

DEPENDENCIES
  gl1_msg_cfg_pwr_meas must have been called at some time prior

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void gl1_msg_resume_pwr_meas ( gas_id_t gas_id );


/*===========================================================================

FUNCTION gl1_msg_stop_pwr_meas

DESCRIPTION
  This functions stop currently scheduled measurements.  Any pending
  data will be retrieved from the mdsp and then the data callback made.

DEPENDENCIES
  gl1_msg_cfg_pwr_meas must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_stop_pwr_meas( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_abort_pwr_meas

DESCRIPTION
  This functions aborts currently scheduled power measurements.
  Note if interference measurements are in progress they will also need to
  be aborted.

===========================================================================*/
extern void gl1_msg_abort_pwr_meas( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_pwr_meas_aborted_while_retrieving

DESCRIPTION
  This functions returns whether the last Pwr meas abort was done while it was in
  SM_PWR_MEAS_RETRIEVING state.

  DEPENDENCIES
    gl1_msg_abort_pwr_meas must have been called at some time prior.

  RETURN VALUE
    TRUE - if the last Pwr meas abort was done while it was in
               SM_PWR_MEAS_RETRIEVING state.
    FALSE - if not

  SIDE EFFECTS
    None

===========================================================================*/
extern boolean gl1_msg_pwr_meas_aborted_while_retrieving( gas_id_t gas_id );

extern void gl1_msg_pwr_reset_meas_aborted_while_retrieving( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_cfg_rx_fcch_sch

DESCRIPTION
  This function is used to configure the callbacks for acquisition.

  Note that no metrics are returned for acquisition.  The metrics
  callback is ignored.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_cfg_fcch_sch ( gl1_msg_rx_cb_type *cb ,gas_id_t gas_id);

extern void gl1_msg_cfg_fcch_sch_list (gl1_msg_rx_cb_type *cb, uint32 list_pos,gas_id_t gas_id);

/* This function is used to allow calling client write access to the
 * values of the rotator or PDM.
 */
extern void gl1_msg_set_afc_info( int32 afc_freq ,gas_id_t gas_id);

/* This function is used to allow calling client access to the
 * values of the rotator and PDM.
 */
extern void gl1_msg_get_afc_info( int32 *word1_ptr,gas_id_t gas_id );

/* This function is used to allow FCCH/SCH combined acquistion in
 * in IDLE mode with AFC ON/OFF using rotator/PDM.
 */
extern void  gl1_msg_cfg_ncell_fcch_sch( gl1_msg_rx_cb_type*  cb,
                                         boolean              afc_on,
                                         gas_id_t             gas_id );

/*===========================================================================

FUNCTION gl1_msg_rx_fcch_sch

DESCRIPTION
  Acquire synchronization to a cell by receiving the FCCH followed by the SCH.

  ARFCN              - the ARFCN to attempt acquisition on
  signal_strength    - the expext rx signal level
  start_offset       - the qbit offset of the start of the acquisition window
  fcch_frames        - the maximum number of frames to search for an FCB
  sch_frames         - the maximum number of frames to try to decode an SB
                       once an FCB has been detected
  sch_attempts       - the maximum number of attempts to try to decode an SB
                       once an FCB has been detected
  do_AFC             - indicates whether the results from the FCb should be
                       used to adjust the frequency reference
  do_AGC             - enables/disables AGC during acquisition
  early_callback     - switch to acq_stopped state from acq_wait_stop in the same frame
  rpt                - reports for passing to callbacks

DEPENDENCIES
  gl1_msg_cfg_fcch_sch() must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_rx_fcch_sch(
                      ARFCN_T            ARFCN,
                      sys_algo_agc_T     *agc_ptr,
                      uint16             start_offset,
                      uint8              fcch_frames,
                      uint8              sch_frames,
                      uint8              sch_attempts,
                      boolean            do_AFC,
                      boolean            do_AGC,
                      boolean            early_callback,
                      gl1_msg_rx_rpt    *rpt,
                      gas_id_t           gas_id   );

/*===========================================================================

FUNCTION gl1_msg_correct_timing

DESCRIPTION
  This function is used to change the MS timing.  This is done by advancing
  or delaying the GSTMR.

  Negative values of timing_error will advance the GSTMR, positive values
  will delay the GSTMR.  The GSTMR count is changed by adjusting the
  terminal count for the current frame.  Large negative values should not
  be used since they may represent a time that has already passed!  The
  behaviour is when this occurs is undefined.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_correct_timing( int32 timing_error ,gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_reset

DESCRIPTION
  Calls hw reset function to allow fifos in the drivers to be reset.
  This function should be called after any message layer abort functions
  have been invoked, before starting new message operations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_reset( gas_id_t gas_id );

#ifdef  FEATURE_GSM_DTM
#ifdef  FEATURE_GSM_EDTM

void  gl1_msg_reset_cs_only( gas_id_t gas_id );

#endif
#endif

/*===========================================================================

FUNCTION gl1_msg_enable_dtx   , gl1_msg_disable_dtx

DESCRIPTION
  This function enables/disables Discontinous Transmission (dtx).
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_enable_dtx( gas_id_t gas_id );
void gl1_msg_disable_dtx( gas_id_t gas_id );



/*=============================================================================
FUNCTION gl1_msg_set_cs_timeslot

DESCRIPTION
   This function sets the timeslot to be used for the CS channels (SACCH,
   SDCCH, TCH)

============================================================================= */
void gl1_msg_set_cs_timeslot (gl1_defs_tn_type tn, gas_id_t gas_id);



/*=============================================================================
FUNCTION gl1_msg_get_cs_timeslot

DESCRIPTION
   This function gets the timeslot in use for the CS channels (SACCH,
   SDCCH, TCH)

============================================================================= */
gl1_defs_tn_type gl1_msg_get_cs_timeslot (gas_id_t gas_id);



/*===========================================================================

FUNCTION  gl1_msg_enable_ciphering,   gl1_msg_disable_ciphering

DESCRIPTION
  This function enables/disables ciphering on both the uplink and downlink
  by writing to the GSAC_CONFIG_x_RW registers and then telling the mdsp
  to turn on/off ciphering.

  key - pointer to cipher key

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gl1_msg_enable_ciphering ( uint8                *key,
                                gl1_defs_cipher_mode mode,
                                gas_id_t             gas_id);

void gl1_msg_disable_ciphering ( gas_id_t gas_id );
void gl1_msg_clear_ciphering_enabled ( gas_id_t gas_id );
boolean gl1_msg_is_ciphering_enabled( gas_id_t gas_id);

void gl1_msg_acq_afc_init( ARFCN_T arfcn, gas_id_t gas_id);
#ifdef FEATURE_GSM_DTM
void gl1_msg_set_afc_tt_mode( gl1_msg_afc_tt_mode mode,gas_id_t gas_id );
#endif

uint16 gl1_msg_saic_tt_threshold_snr( gl1_defs_metrics_rpt *m );
uint16 gl1_msg_saic_afc_threshold_snr( gl1_defs_metrics_rpt *m );

uint16 gl1_msg_gprs_saic_tt_threshold_snr( gl1_defs_rx_pkt_metrics_type *m );
uint16 gl1_msg_gprs_saic_afc_threshold_snr( gl1_defs_rx_pkt_metrics_type *m );


void gl1_msg_dch_afc_tt_init( ARFCN_T arfcn,gas_id_t gas_id );
void gl1_msg_dch_tt_update(int16 timing_offset, uint16 snr,
                           boolean saic_used, gas_id_t gas_id);

gl1_defs_modulation_type gl1_msg_snr_threshold(boolean saic_used);
void gl1_msg_dch_afc_update(int16 freq_err, uint16 snr, boolean saic_used,gas_id_t gas_id);
void gl1_msg_dch_tt_adjust( gas_id_t gas_id );
void gl1_msg_dch_afc_adjust( gas_id_t gas_id );

void gl1_msg_pdch_afc_tt_init( ARFCN_T arfcn ,gas_id_t gas_id);
void gl1_msg_pdch_tt_update(int16 timing_offset, uint16 snr, gl1_defs_modulation_type mod,gas_id_t gas_id);
void gl1_msg_pdch_afc_update( uint8                          num_timeslots,
                              gl1_defs_rx_pkt_metrics_type*  metrics_ptr[GL1_DEFS_MAX_DL_MSGS ],
                              gas_id_t gas_id
                            );
void gl1_msg_pdch_tt_adjust( gas_id_t gas_id );
void gl1_msg_pdch_afc_adjust( gas_id_t gas_id );

void gl1_msg_cch_afc_tt_init( ARFCN_T arfcn,gas_id_t gas_id);
void gl1_msg_cch_tt_update(int16 timing_offset, uint16 snr, boolean saic_used,gas_id_t gas_id);
void gl1_msg_cch_tt_adjust( gas_id_t gas_id );
void gl1_msg_cch_afc_adjust( gas_id_t gas_id );
void gl1_msg_cch_afc_update(int16 freq_err, uint16 snr, boolean saic_used,gas_id_t gas_id);

void gl1_msg_update_sacch_arfcns( ARFCN_T ARFCN[4], gas_id_t gas_id );

void gl1_msg_compute_bfi( boolean crc_pass,
#ifdef FEATURE_VAMOS_II
                          boolean vamos_paired,
                          gl1_defs_loopback_type loopback_type,
#endif
                          uint16 curr_rx_qual_s,
                          uint16 prev_rx_qual_s,
                          uint16 curr_rx_qual_h,
                          uint16 prev_bfi,
                          uint16 snr,
                          uint16 *new_bfi
                        );

#ifdef FEATURE_GSM_AMR
void gl1_msg_compute_bfi_amr_fr( boolean crc_pass,
#ifdef FEATURE_VAMOS_II
                                 boolean vamos_paired,
                                 gl1_defs_loopback_type loopback_type,
#endif
                                 uint16 curr_rx_qual_s,
                                 uint16 snr,
                                 gl1_msg_amr_mode_type dl_codec_mode,
                                 uint16 *new_bfi
                                 );
#endif

uint16 gl1_msg_get_snr( gl1_defs_metrics_rpt *rpt[], uint8 num_bursts );

/*
 * RXQUAL_FULL, RXQUAL_SUB
 */
#ifdef FEATURE_GSM_DYNAMIC_SAIC
uint32 gl1_msg_get_hard_rxqual_full_avg(gas_id_t gas_id);
#endif

void  gl1_msg_rxqual_init( gas_id_t gas_id );
void  gl1_msg_rxqual_full_update( uint16 dsp_rxqual, gas_id_t gas_id );
void  gl1_msg_rxqual_sub_update( uint16 dsp_rxqual, gas_id_t gas_id );
uint8 gl1_msg_get_rxqual_full( boolean *meas_report_valid,gas_id_t gas_id);
uint8 gl1_msg_get_rxqual_sub( gas_id_t gas_id );

uint8 gpl1_gprs_calc_rxqual_lev( uint32 rxqual_sum, uint16 block_cnt,gas_id_t gas_id );

extern void gl1_msg_compute_bfi_ufi( boolean crc_pass,
#ifdef FEATURE_VAMOS_II
                                     boolean vamos_paired,
                                     gl1_defs_loopback_type loopback_type,
#endif
                                     uint16 curr_rx_qual,
                                     uint16 prev_rx_qual,uint16 prev_bfi,uint16 *new_bfi,
                                     uint16 *new_ufi,
                                     uint16 snr,
                                     uint16 curr_rx_qual_h,
                                     uint16 prev_rx_qual_h);

extern void gl1_msg_compute_bfi_amr_hr( boolean crc_pass,
#ifdef FEATURE_VAMOS_II
                                        boolean vamos_paired,
                                        gl1_defs_loopback_type loopback_type,
#endif
                                        uint16 *new_bfi,
                                        uint16 snr,
                                        gl1_msg_amr_mode_type dl_codec_mode,
                                        uint16 curr_rx_qual_h);

extern uint8 gl1_msg_compute_sid( uint16 diff_c1,uint16 diff_c1c2,uint8 mode);

/*===========================================================================

FUNCTION gl1_msg_tch_snr_to_bep

DESCRIPTION
  Converts an SNR value for TCH to BEP.

===========================================================================*/
uint16 gl1_msg_tch_snr_to_bep
(
   gl1_defs_metrics_rpt *metrics_ptr,
   gas_id_t gas_id
);


/*===========================================================================

FUNCTION gl1_msg_snr_to_bep

DESCRIPTION
  Converts an SNR value for a given modulation scheme in the BEP.

===========================================================================*/
uint16 gl1_msg_snr_to_bep
(
   uint16 snr,
   gl1_defs_modulation_type mod,
   boolean is_tch,
   gas_id_t gas_id
);


/*===========================================================================

FUNCTION  gl1_msg_calc_mean_bep_gmsk

DESCRIPTION
  Takes a mean GMSK BEP value in Q16 format and returns the MEAN_BEP
  index.

===========================================================================*/
uint8  gl1_msg_calc_mean_bep_gmsk (uint32 mean_bep);

/*===========================================================================

FUNCTION  gl1_msg_calc_aeq_8psk_mean_bep

DESCRIPTION
  Takes BEP values for 1 block in Q16 format and calculates the mean BEP over
  the block (4 bursts). Every 4 blocks, the mean BEP over these blocks is copied
  into a larger mean BEP buffer holding mean BEP per 4 block resust over the
  last 24 blocks. The mean BEP per 4 blocks and 24 blockss is then used to determine
  if AEQ PH2 should be used for the next Rx block.

===========================================================================*/
void gl1_msg_calc_aeq_8psk_mean_bep (uint16  *src_bep_ptr,gas_id_t gas_id);

/*===========================================================================

FUNCTION  gl1_msg_init_aeq_8psk_mean_bep

DESCRIPTION
  Initialises AEQ 8PSK mean BEP buffers and buffer indices.

===========================================================================*/
void gl1_msg_init_aeq_8psk_mean_bep(gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_bep_init

DESCRIPTION
     Initializes mean and CV BEP totals

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_bep_init( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_mean_bep_update

DESCRIPTION
     Accumulates passed in mean BEP

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_mean_bep_update ( uint32 mean_bep, gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_cv_bep_update

DESCRIPTION
     Accumulates passed in CV BEP

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_cv_bep_update ( uint32 cv_bep, gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_get_mean_bep

DESCRIPTION
     Computes and returns the mean BEP ( total / num blocks )

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 gl1_msg_get_mean_bep( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_get_cv_beo

DESCRIPTION
     Computes and returns the CV BEP ( total / num blocks )

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 gl1_msg_get_cv_bep( gas_id_t gas_id  );


/*===========================================================================

FUNCTION  gl1_msg_calc_mean_bep_8psk

DESCRIPTION
  Takes a mean 8PSK BEP value in Q16 format and returns the
  MEAN_BEP index

===========================================================================*/
uint8  gl1_msg_calc_mean_bep_8psk (uint32 mean_bep);


/*===========================================================================

FUNCTION  gl1_msg_calc_cv_bep

DESCRIPTION
  Takes a BEP variance value in Q16 and returns the CV_BEP index

===========================================================================*/
uint8  gl1_msg_calc_cv_bep (uint32 cv_bep);



/*===========================================================================

FUNCTION  gl1_msg_process_bep

DESCRIPTION
  Takes raw BEP values and returns the mean and coefficient of variance
  in Q16 format.

===========================================================================*/
void gl1_msg_process_bep
(
   const uint16 bep[GL1_DEFS_FRAMES_IN_BLK_PERIOD],
   uint32 *mean_bep,
   uint32 *cv_bep
);


/*===========================================================================

FUNCTION  gl1_msg_set_band_group

DESCRIPTION
  Informs the NPL1 of which band group we will be operating in.
  Currently only used to inform clock regime which high clock speed to
  use since certain speeds cause RF interference in certain bands.

===========================================================================*/
extern void gl1_msg_set_band_group (sys_band_mask_type band_mask);

/*===========================================================================

FUNCTION  set_afc_band

DESCRIPTION
  Sets the band for AFC, and informs the HW layer of the new center
  frequency.

===========================================================================*/
extern void set_afc_band( ARFCN_T arfcn, boolean dedicated_mode,gas_id_t gas_id );

/*===========================================================================

FUNCTION  gl1_msg_ncell_sch_pos_in_acq_window

DESCRIPTION
  This function determines if the passed in SCH offset lies within the
  TCH surround cell acquisition window

DEPENDENCIES
  None

PARAMETERS
  SCH offset in quarter symbols

RETURN VALUE
   -1 if offset lies before start of acq window
    0 if offset lies within acq window
   +1 if offset lies after the end of the acq window

SIDE EFFECTS
  None

===========================================================================*/
int16 gl1_msg_ncell_sch_pos_in_acq_window( int16 sch_offset,
                                           const gl1_defs_tn_struct* first_rx_tn_ptr,
                                           const gl1_defs_tn_struct* last_tx_tn_ptr,
                                           gas_id_t                  gas_id);

/* This is to be used specifically for TCH operation */
int16 gl1_msg_ncell_sch_pos_in_acq_window_dedic( int16 sch_offset,
                                                 const gl1_defs_tn_struct* first_rx_tn_ptr,
                                                 const gl1_defs_tn_struct* last_tx_tn_ptr,
                                                 gas_id_t                  gas_id);

/*=============================================================================
FUNCTION  gl1_msg_adjust_time_stamp

DESCRIPTION
   This function provides an interface to add an offset to our local
   copy of time stamp.  This is necessary to support DRX operation.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_adjust_time_stamp (int32 offset,gas_id_t gas_id);

/*=============================================================================
FUNCTION  gl1_hw_msg_inc_fn

DESCRIPTION

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_hw_msg_inc_fn ( int32 num,gas_id_t gas_id);

/*=============================================================================
FUNCTION  gl1_msg_reset_tch_fifos

DESCRIPTION
   This function resets the rx and tx fifo counters which store the
   rx and tx burst parameters

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_reset_tch_fifos( void );


/* =======================================================================
**                          GPRS FUNCTIONS
** ======================================================================= */

/* =======================================================================
**                   FUNCTIONS RELATED TO PCCCH
** ======================================================================= */
/*=============================================================================
FUNCTION gl1_msg_cfg_pccch

DESCRIPTION
   PBCCH is configured and ready to receive data and metrics on the specified
   timeslot.


DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_cfg_pccch
(
    gl1_defs_tn_type              tn,
    uint8                         tsc,
    const gl1_defs_rx_pkt_cb_type *rx_msg_cb_info
);

/*=============================================================================
FUNCTION gl1_msg_rx_pccch

DESCRIPTION
   Schedule a receive and decode of a PCCCH block of data at the specified
   frequencies. This function also causes the metrics and data callback handlers
   to be invoked every frame in the block.

   The pointers to the buffers for decoded data, metrics and log will
   subsequently be filled by NPL1 and the corresponding callback will be invoked
   to notify that the data/information has been copied on to the buffer.

   This function is to be called anywhere in the first 3 frames of the previous
   block. The first received burst is scheduled at the start of the current
   block boundary.

DEPENDENCIES
   The channel must be configured.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_rx_pccch
(
    const ARFCN_T                   arfcn[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ],
    sys_algo_agc_T                  *agc_ptr,
    const gl1_defs_rx_pkt_mem_type  *rx_data_info
);


/* =======================================================================
**                   FUNCTIONS RELATED TO PBCCH
** ======================================================================= */
/*=============================================================================
FUNCTION gl1_msg_cfg_pbcch

DESCRIPTION
   PBCCH is configured and ready to receive data and metrics on the specified
   timeslot.


DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_cfg_pbcch
(
    gl1_defs_tn_type              tn,
    uint8                         tsc,
    const gl1_defs_rx_pkt_cb_type *rx_msg_cb_info
);

/*=============================================================================
FUNCTION gl1_msg_rx_pbcch

DESCRIPTION
   Schedule a receive and decode of a PBCCH block of data at the specified
   frequencies. This function also causes the metrics and data callback handlers
   to be invoked every frame in the block.

   The pointers to the buffers for decoded data, metrics and log will
   subsequently be filled by NPL1 and the corresponding callback will be invoked
   to notify that the data/information has been copied on to the buffer.

   This function is to be called anywhere in the first 3 frames of the previous
   block. The first received burst is scheduled at the start of the current
   block boundary.

DEPENDENCIES
   The channel must be configured.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_rx_pbcch
(
    const ARFCN_T                  arfcn[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ],
    sys_algo_agc_T                 *agc_ptr,
    const gl1_defs_rx_pkt_mem_type *rx_data_info
);



/* =======================================================================
**                   FUNCTIONS RELATED TO DL PTCCH
** ======================================================================= */

/*=============================================================================
FUNCTION gl1_msg_cfg_dl_ptcch

DESCRIPTION
   PTCCH is configured and ready to receive data and metrics on the specified
   timeslot.

DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_cfg_dl_ptcch
(
   gl1_defs_tn_type              tn,
   uint8                         bsic,
   const gl1_defs_rx_pkt_cb_type *rx_msg_cb_info,
   gas_id_t                      gas_id
);


/*=============================================================================
FUNCTION gl1_msg_update_dl_ptcch_tn

DESCRIPTION
   Updates the current PTCCH timeslot - used if the timeslot timing has
   changed in between bursts.

DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_update_dl_ptcch_tn
(
  gl1_defs_tn_type tn,gas_id_t gas_id
);


/*=============================================================================
FUNCTION gl1_msg_inc_dl_ptcch_frame_count

DESCRIPTION
   Increments the current PTCCH frame count, called if frame tick slamming
   has resulted in missing a frame between PTCCH bursts.

============================================================================= */
void gl1_msg_inc_dl_ptcch_frame_count (gas_id_t gas_id);


/*=============================================================================
FUNCTION gl1_msg_rx_ptcch

DESCRIPTION
   Schedule a receive and decode of a PTCCH block of data at the specified
   frequencies. This function also causes the metrics and data callback handlers
   to be invoked every frame.

   The pointers to the buffers for decoded data, metrics and log will
   subsequently be filled by NPL1 and the corresponding callback will be invoked
   to notify that the data/information has been copied on to the buffer.

   This function is to be called anywhere in the first 3 frames of the previous
   block (block with 4 frames). The first received burst is scheduled to be
   received in the frame following the end of the previous block boundary. NPL1
   then schedules the remaining 3 bursts 26 frames apart.


DEPENDENCIES
   The channel must be configured.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_rx_ptcch
(
    const ARFCN_T                  arfcn[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ],
    sys_algo_agc_T                 *agc_ptr,
    const gl1_defs_rx_pkt_mem_type *rx_data_info,
    gas_id_t                       gas_id
);


/*===========================================================================

FUNCTION gl1_msg_abort_dl_ptcch

DESCRIPTION
  Aborts a PTCCH receive in progress.
  The DL PTCCH channel is aborted when the function returns.
  Subsequent metrics or data callback will not be invoked.

===========================================================================*/
void gl1_msg_abort_dl_ptcch ( gas_id_t  gas_id );



/* =======================================================================
**                   FUNCTIONS RELATED TO DL PACCH and PDTCH
** ======================================================================= */

/*=============================================================================
FUNCTION gl1_msg_cfg_dl_pacch_pdtch

DESCRIPTION
   PACCH and PDTCH are configured and ready to receive data and metrics.

   Note that enabling USF granularity flag causes the USF to be monitored only
   for the first block, after that, for the next 3 blocks the USF are not
   monitored.

   The timeslots on which USFs were decoded on the first block are considered
   allocated for the next 3 blocks.

DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_cfg_dl_pacch_pdtch
(
    gl1_defs_rx_pkt_alloc_type     alloc_info[ GL1_DEFS_MAX_ASSIGNED_DL_TS ],
    uint8                          num_alloc_tn,
    uint8                          tsc,
    boolean                        usf_gran_set,
    const gl1_defs_rx_pkt_cb_type  *rx_msg_cb_info,
    gas_id_t                       gas_id
);


/*=============================================================================
FUNCTION gl1_msg_rx_pacch_pdtch

DESCRIPTION
   Schedule a receive and decode of a PACCH or PDTCH block of data at the
   specified frequencies. This function also causes the metrics and data
   callback handlers to be invoked every frame in the block.

   The pointers to the buffers for decoded data, metrics and log will
   subsequently be filled by NPL1 and the corresponding callback will be invoked
   to notify that the data/information has been copied on to the buffer.

   This function is to be called anywhere in the first 3 frames of the previous
   block. The first received burst is scheduled at the start of the current
   block boundary.


DEPENDENCIES
   The channel must be configured.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_rx_pacch_pdtch
(
    gl1_defs_tn_type                tn,
    const ARFCN_T                   arfcn[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ],
    sys_algo_agc_T                  *agc_ptr,
    const gl1_defs_rx_pkt_mem_type  *rx_data_info,
    gas_id_t                         gas_id
);

/*=============================================================================
FUNCTION gl1_msg_abort_dl_pacch_pdtch

DESCRIPTION
   Immediately bborts the processing of the DL PACCH PDTCH. Some of the drivers
   are left in a incorrect state. gl1_msg_reset() needs to be invoked following
   this function.
   If this function is invoked in the task context make sure that this function
  has INTERRUPTS LOCKED.

DEPENDENCIES
   The channel must be configured.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_abort_dl_pacch_pdtch( gas_id_t gas_id );



/* =======================================================================
**                   FUNCTIONS RELATED TO PRACH
** ======================================================================= */

/*=============================================================================
FUNCTION gl1_msg_cfg_prach

DESCRIPTION
   PRACH is configured and ready to transmit data. Note: To avoid confusion the
   tsc defined here is not the NB TSC, but the AB Sync Seq Bits as per
   3GPP 45.002, Clause 5.2.7.


DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_cfg_prach
(
    gl1_defs_tn_type               tn,
    int16                          TXLEV,
    gl1_defs_coding_type           coding_scheme,
    uint8                          ab_tsc, /* Access Burst TSC */
    uint8                          bsic,
    boolean                        usf_enabled,
    const gl1_msg_tx_pkt_cb_type  *tx_msg_cb_info
);

/*=============================================================================
FUNCTION gl1_msg_tx_prach

DESCRIPTION
   Write PRACH data and schedule an encode and transmit of the data at the
   configured power and frequency. The PRACH data is transmitted as a single
   access burst.

   This function is to be called anywhere in frame 2 or 3 of the previous block.
   The first transmitted burst is scheduled at the specified offset from the
   start of the transmitting block boundary.

   Note that if this function fails due to the mDSP message buffers being full
   then the discarded message callback is invoked.


DEPENDENCIES
   The channel must be configured.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_tx_prach
(
    uint8    *msg_data,
    ARFCN_T  arfcn
);

/*=============================================================================
FUNCTION gl1_msg_update_prach_params

DESCRIPTION
   This function may be invoked when the frequency has to be changed after data
   has been submitted.

   For example in dynamic or extended dynamic allocation, data maybe submitted
   to NPL1. However, if USF==FREE is not decoded then the data will remain in
   NPl1 (if not discarded to make space for a higher priority message) until the
   USF = FREE is decoded. Due to frequency hopping, the frequency may be have to
    be changed periodically and applied to the submitted data.

   The frequency takes effect at the next block boundary.


DEPENDENCIES
   PRACH data must be submitted.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_update_prach_params
(
    ARFCN_T arfcn
);

/*=============================================================================
FUNCTION gl1_msg_abort_prach

DESCRIPTION
   This function may be invoked to abort ongoing prach transmission.

   This function is applicable mainly to abort dynamic prach processing. In
   case a USF is not detected the NPL1 will continously issue transmit commands
   to the mDSP whenever the ul block start flag is set.

   Invoking this function will result in resetting the PRACH state machine to
   the last configured state and the discarded callback will be invoked
   with a pointer to the previously submitted untransmitted payload.

   This function may be invoked in any frame in the multiframe.

DEPENDENCIES
   PRACH data must be submitted.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_abort_prach( void );


/* =======================================================================
**                   FUNCTIONS RELATED TO UL PTCCH
** ======================================================================= */

/*=============================================================================
FUNCTION gl1_msg_cfg_ul_ptcch

DESCRIPTION
   PTCCH UL is configured and ready to transmit data.


DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */

void gl1_msg_cfg_ul_ptcch
(
    gl1_defs_tn_type              tn,
    gl1_defs_coding_type          coding_scheme,
    uint8                         bsic,
    const gl1_msg_tx_pkt_cb_type *tx_msg_cb_info,
    gas_id_t                      gas_id
);

/*=============================================================================
FUNCTION gl1_msg_tx_ptcch

DESCRIPTION
   Write PTCCH data and schedule an encode and transmit of the data at the
   configured power and frequency. The PTCCH data is transmitted as a single
   access burst.

   This function is to be called anywhere in frame 2 or 3 of the previous block.
   The transmitted burst is scheduled in the frame following the end of the
   previous block boundary.

   Note that if this function fails due to the mDSP message buffers being full
   then the discarded message callback is invoked.


DEPENDENCIES
   The channel must be configured.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_tx_ptcch
(
    uint8    *msg_data,
    int16    TXLEV,
    ARFCN_T  arfcn,
    gas_id_t gas_id
);



/* =======================================================================
**                   FUNCTIONS RELATED TO UL PACCH and PDTCH
** ======================================================================= */

/*=============================================================================
FUNCTION gl1_msg_cfg_ul_pacch_pdtch

DESCRIPTION
   PACCH and PDTCH are configured and ready to transmit data.


DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_cfg_ul_pacch_pdtch
(
    uint8                         tsc,
    uint8                         ab_tsc, /* Access Burst TSC */
    uint8                         bsic,
    const gl1_msg_tx_pkt_cb_type *tx_msg_cb_info,
    gas_id_t                      gas_id
);


/*=============================================================================
FUNCTION gl1_msg_tx_pacch_pdtch_fixed

DESCRIPTION
   Write PACCH or PDTCH data and schedule an encode and transmit of the data.

   This function is to be called anywhere in frame 2 or 3 of the previous block.
   The first transmitted burst is scheduled at the start of the current block
   boundary.

   Note that if this function fails due to the mDSP message buffers being full
   then the discarded message callback is invoked.

DEPENDENCIES
   The channel must be configured.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_tx_pacch_pdtch_fixed
(
   gl1_defs_tx_pkt_data_type          *msg,
   gl1_msg_ul_pacch_pdtch_signal_type *signal_info,
   ARFCN_T                            arfcn[GL1_DEFS_FRAMES_IN_BLK_PERIOD],
   gl1_defs_coding_type               coding_scheme,
   gl1_defs_fixed_block_type          block_type,
   gas_id_t                           gas_id
);

#ifdef FEATURE_GPRS_PS_HANDOVER
void gl1_msg_tx_psho_pacch_pdtch_fixed
(
   gl1_defs_tx_pkt_data_type          *msg,
   gl1_msg_ul_pacch_pdtch_signal_type *signal_info,
   ARFCN_T                            arfcn[GL1_DEFS_FRAMES_IN_BLK_PERIOD],
   gl1_defs_coding_type               coding_scheme,
   gas_id_t                           gas_id
);
#endif

/*=============================================================================
FUNCTION gl1_msg_tx_pacch_pdtch_loopback

DESCRIPTION
   Schedule a loopback transmit.

   This function is to be called anywhere in frame 2 or 3 of the previous block.
   The first transmitted burst is scheduled at the start of the current block
   boundary.

DEPENDENCIES
   The channel must be configured.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_tx_pacch_pdtch_loopback
(
   gl1_msg_ul_pacch_pdtch_signal_type *signal_info,
   ARFCN_T                            arfcn[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ],
   gl1_defs_tn_type                   tn_to_loopback,
   gas_id_t                           gas_id
);


/*=============================================================================
FUNCTION gl1_msg_tx_pacch_pdtch_dynamic

DESCRIPTION
   Write PACCH or PDTCH data and schedule an encode and transmit of the data.

   This function is to be called anywhere in frame 2 or 3 of the previous block.
   The first transmitted burst is scheduled in the block following the one in
   which a USF has been decoded.

   The submitted data will be transmitted on the block following the one on
   which a USF has been decoded. The transmission of the data is indicated by
   the invoking of the 'transmission of data notification' callback.

   Note that if this function fails due to the mDSP message buffers being full
   then the discarded message callback is invoked.


DEPENDENCIES
   The channel must be configured.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_tx_pacch_pdtch_dynamic
(
   gl1_defs_tx_pkt_data_type  *msg,
   gl1_defs_coding_type       coding_scheme,
   geran_pdu_priority_t       priority,
   gas_id_t                   gas_id
);


/*=============================================================================
FUNCTION gl1_msg_set_pacch_pdtch_dynamic_signal_info

DESCRIPTION
   This function is to be invoked following the submission of USF specific
   messages or when there are un-transmitted USF specific messages from previous
   blocks.
   In the case of dynamic or extended dynamic allocation, the submitted messages
   are not tied to any particular timeslot. Hence the power and frequency for
   all the assigned timeslots must be submitted every block until all the
   submitted USF messages have been transmitted. This function allows the
   updating of the frequency and power information for the next four frames.

   The frequencies and powers take effect two frames following the frame in
   which this function was called.


DEPENDENCIES
   PACCH or PDTCH data must be submitted.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_set_pacch_pdtch_dynamic_signal_info
(
    gl1_msg_ul_pacch_pdtch_signal_type
                                signal_info[ GL1_DEFS_MAX_ASSIGNED_UL_TS ],
    uint8                       num_tn,
    ARFCN_T                     arfcn[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ],
    gas_id_t                           gas_id
);


/*=============================================================================
FUNCTION gl1_msg_abort_ul_pacch_pdtch

DESCRIPTION
   This function may be invoked to abort ongoing PACCH/PDTCH transmissions.

   This function is applicable mainly to abort dynamic PACCH/PDTCH processing.
   In USFs are not detected the NPL1 will continously issue transmit commands
   to the mDSP whenever the ul block start flag is set.

   Invoking this function will result in resetting the PACCH/PDTCH state machine
   to the last configured state and the discarded callback will be invoked
   with pointers to the previously submitted untransmitted payloads.

   This function may be invoked frame 2 or frame 3 of a radio block.

DEPENDENCIES
   PACCH/PDTCH data must be submitted.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_abort_ul_pacch_pdtch( gas_id_t gas_id  );


/* =======================================================================
**                   FUNCTIONS RELATED TO MONITORING
** ======================================================================= */

/*=============================================================================
FUNCTION gl1_msg_cfg_int_meas

DESCRIPTION
   Configures the message layer to retrieve interference measurements. The
   message layer is now ready to be commanded to retrieve interference
   measurements

DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_cfg_int_meas
(
   gl1_msg_int_meas_cb_type results_callback,
   gas_id_t gas_id
);

/*=============================================================================
FUNCTION gl1_msg_do_int_meas

DESCRIPTION
   This function is to be called one frame prior to which interference
   measurements are to be taken. The measurement results are copied into the
   results buffer and the callback is invoked when all the specified
   measurements are taken

DEPENDENCIES
   The message layer must be configured for interference measurements.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_do_int_meas
(
   gl1_msg_meas_signal_type           signal_info,
   const gl1_defs_tn_type             tn[ GL1_DEFS_MAX_ASSIGNED_DL_TS ],
   uint8                              num_tn,
   const gl1_defs_int_meas_result_type *int_results_info,
   gas_id_t                           gas_id
);

/*=============================================================================
FUNCTION gl1_msg_int_retrieving_results

DESCRIPTION
	 This function returns TRUE if i/f measurements results are waiting to be
	 retrieved

DEPENDENCIES
	 None

RETURN VALUE
	 Returns TRUE if i/f results are to be retrieved

SIDE EFFECTS
	 None
============================================================================= */
boolean gl1_msg_int_retrieving_results(gas_id_t gas_id);


/*=============================================================================
FUNCTION gl1_msg_set_medium_access_mode

DESCRIPTION
   This function sets the medium access mode to FIXED, DYNAMIC or EXT DYNAMIC
   ALLOCATION.

============================================================================= */
void gl1_msg_set_medium_access_mode
(
   gl1_defs_access_mode_type mode,
   gas_id_t gas_id
);

/*=============================================================================
FUNCTION gl1_msg_is_aeq_egprs__enabled

DESCRIPTION
   determines if AEQ has been disabled via NV item

RETURNS
  TRUE if AEQ is disabled FALSE otherwise
============================================================================= */
boolean gl1_msg_is_aeq_egprs_enabled(gas_id_t gas_id);

/*=============================================================================
FUNCTION gl1_msg_IR_enable

DESCRIPTION
   This function enables or disables incremental redundancy.

============================================================================= */
void gl1_msg_IR_enable (boolean enable,gas_id_t gas_id);

boolean gl1_msg_is_IR_enabled (gas_id_t gas_id);

/*=============================================================================
FUNCTION gl1_msg_IR_clear_memory

DESCRIPTION
   Clears a range of incremental redundancy memory.

============================================================================= */
void gl1_msg_IR_clear_memory (uint16 starting_bsn, uint16 ending_bsn,gas_id_t gas_id);


/*=============================================================================
FUNCTION gl1_msg_IR_set_active_window

DESCRIPTION
   Sets the active window for incremental redundancy.

============================================================================= */
void gl1_msg_IR_set_active_window (uint16 starting_bsn, uint16 ending_bsn,gas_id_t gas_id);


/*=============================================================================
FUNCTION gl1_msg_IR_set_tfi

DESCRIPTION
   Sets the TFI for incremental redundancy.

============================================================================= */
void gl1_msg_IR_set_tfi (uint8 tfi,gas_id_t gas_id);

/*=============================================================================
FUNCTION gl1_msg_set_egprs_mode

DESCRIPTION
   This function enables or disables EGPRS mode.

============================================================================= */
void gl1_msg_set_egprs_mode (boolean egprs_mode, boolean reg_clk,gas_id_t gas_id);

#ifdef FEATURE_GSM_EGPRS_IR_WS_REDUCTION
/*===========================================================================

FUNCTION gl1_msg_get_ir_oper_win_size

DESCRIPTION
  return the size of phyical memory space allocated for edge IR
===========================================================================*/
uint16 gl1_msg_get_ir_oper_win_size(gas_id_t gas_id);
#endif /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */





#ifdef FEATURE_GSM_AMR
/* =======================================================================
**                  GSM FUNCTIONS FOR AMR
** ======================================================================= */
/*===========================================================================
FUNCTION gl1_msg_vocoder_rx_amr_fr  ,  gl1_msg_vocoder_tx_amr_fr

DESCRIPTION
  These function are used to transfer data to or from the vocoder.  These
  functions may be called by the task level.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_vocoder_rx_amr_fr(uint8 *data,
                      gl1_msg_amr_frame_info_type *frame,gas_id_t gas_id);


extern void gl1_msg_vocoder_tx_amr_fr(uint8 *data,
                    uint8 datalength,
                      gl1_msg_amr_frame_info_type *frame,gas_id_t gas_id);

/*===========================================================================
FUNCTION gl1_msg_enable_vocoder_amr_fr_hr,  gl1_msg_disable)vocoder_amr_fr_hr

DESCRIPTION
  These function are used to enable/disable vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_enable_vocoder_amr_fr_hr(gl1_msg_chan_type chan,gas_id_t gas_id);
extern void gl1_msg_disable_vocoder_amr_fr_hr(gl1_msg_chan_type chan,gas_id_t gas_id);

/*===========================================================================
FUNCTION gl1_msg_tx_tch_amr_fr,   gl1_msg_tx_facch_amr_fr

DESCRIPTION
  These functions are used to transmit a message on the TCH/FACCH on the next
  block boundary.  The message contents should span 4 frames ie 1 block.
  This function may be called at task level but should be called in the block
  before the data is to transmitted.   If data to be transmitted is for
  frame #4,5,6,7, then this function should be completed before start of
  frame #3, ie called in frames 0,1,2.

DEPENDENCIES
  gl1_msg_store_tx_TSC(), gl1_msg_cfg_ul_tch() and gl1_msg_store_ta(),
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_tx_tch_amr_fr( ARFCN_T ARFCN[4],  int16 TXLEV[4],
                                   uint8 *data,  boolean onset,
                                   uint8 ul_cmi, uint8 dl_cmi,
                                   uint8 cmr, uint8 cmc,
                                   gl1_msg_amr_mode_type curr_codec_mode,
                                   uint8 phase,
#ifdef FEATURE_VAMOS_II
                                   gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                                   gas_id_t gas_id);


extern void gl1_msg_tx_facch_amr_fr(ARFCN_T ARFCN[4], int16 TXLEV[4],
                    uint8 *data, boolean onset,
                    uint8 cmi, uint8 cmr,
                    uint8 phase,
#ifdef FEATURE_VAMOS_II
                    gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                    gas_id_t gas_id);

/*===========================================================================
FUNCTION gl1_msg_tx_ratscch_amr_fr, gl1_msg_tx_sid_first_amr_fr
         gl1_msg_tx_sid_update_amr_fr, gl1_msg_tx_saved_sid_update_amr_fr

DESCRIPTION
  These functions are used to transmit a message on the RATSCCH/SID_FIRST/
  SID_UPDATE on the next block boundary.  The message contents should span 4
  frames ie 1 block. This function may be called at task level but should be
  called in the block before the data is to transmitted. If data to be
  transmitted is for frame #4,5,6,7, then this function should be completed
  before start of frame #3, ie called in frames 0,1,2.

DEPENDENCIES
  gl1_msg_store_tx_TSC(), gl1_msg_cfg_ul_tch() and gl1_msg_store_ta(),
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_tx_ratscch_amr_fr(ARFCN_T ARFCN[4], int16 TXLEV[4],
                      uint8 *data, boolean onset,
                      uint8 cmi, uint8 cmr,
                      uint8 phase,
#ifdef FEATURE_VAMOS_II
                      gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                      gas_id_t gas_id);


extern void gl1_msg_tx_sid_first_amr_fr(ARFCN_T ARFCN[4], int16 TXLEV[4],
                    uint8 *data,
                    uint8 cmi,
                    uint8 cmr, uint8 phase,
#ifdef FEATURE_VAMOS_II
                    gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                    gas_id_t gas_id);


extern void gl1_msg_tx_sid_update_amr_fr(ARFCN_T ARFCN[4], int16 TXLEV[4],
                     uint8 *data,
                     uint8 cmi, uint8 cmr,
                     uint8 phase,
#ifdef FEATURE_VAMOS_II
                     gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                     gas_id_t gas_id);


extern void gl1_msg_tx_saved_sid_update_amr_fr(ARFCN_T ARFCN[4], int16 TXLEV[4],
                           uint8 cmi,
                           uint8 cmr, uint8 phase,
#ifdef FEATURE_VAMOS_II
                           gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                           gas_id_t gas_id);

/*===========================================================================
FUNCTION  gl1_msg_tx_save_sid_update_amr_fr_hr

DESCRIPTION
  This function is used to save a sid_update frame in case it is being
  pre-empted by a FACCH or a RATSCCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_save_sid_update_amr_fr_hr(uint8 *data,gas_id_t gas_id);


/*===========================================================================
FUNCTION gl1_msg_rx_tch_facch_amr_fr

DESCRIPTION
  This function is used to receive a message on either the TCH or the FACCH.
  The message contents should span 4 frames ie 1 block.
  This function may be called at task level but should be called in the block
  before the data is to transmitted.   If data to be received is for
  frame #4,5,6,7, then this function should be completed before start of
  frame #3, ie called in frames 0,1,2.

DEPENDENCIES
  gl1_msg_store_rx_TSC(), gl1_msg_cfg_dl_tch_facch() ,
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_rx_tch_facch_amr_fr(ARFCN_T ARFCN[4],
                    sys_algo_agc_T *agc_ptr,
#ifdef FEATURE_GSM_RX_DIVERSITY
                    sys_algo_agc_T  *agc_ptr_divrx,
#endif
                    gl1_msg_rx_rpt *rpt,
                    uint8 phase,
#ifdef FEATURE_VAMOS_II
                    gl1_vamos2_tch_wait_type vamos2_tch_wait_state,
#endif
                    gas_id_t gas_id );


/*===========================================================================

FUNCTION  gl1_msg_init_tch_amr

DESCRIPTION
  This function should be called to perform any initialization that need to
  be done when entering the traffic channel from idle, or rach states.
  The search window size is modified as is the burst size.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_msg_init_tch_amr (gl1_msg_chan_type chan, gl1_msg_voc_type voc_type,gas_id_t gas_id);

/*===========================================================================

FUNCTION  gl1_msg_terminate_tch_amr_fr_hr

DESCRIPTION
  This function should be called to perform any actions that need to
  be done when exiting the traffic channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_msg_terminate_tch_amr_fr_hr(gas_id_t gas_id);

/*===========================================================================
FUNCTION gl1_msg_abort_tx_tch_amr_fr

DESCRIPTION
  These functions are used to terminate any TCH/FACCH/RATSCCH/SID_FIRST/
  SID_UPDATE bursts pending.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean gl1_msg_abort_tx_tch_amr_fr(gas_id_t gas_id);


/*===========================================================================
FUNCTION gl1_msg_abort_rx_tch_amr_fr

DESCRIPTION
   This function is used to abort TCH/FACCH.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
extern boolean gl1_msg_abort_rx_tch_amr_fr(gas_id_t gas_id);


/*===========================================================================
FUNCTION gl1_msg_set_dl_acs

DESCRIPTION
  These function give the current D/L ACS to the MDSP.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_set_dl_acs( gl1_msg_amr_mode_type * codec_set,gas_id_t gas_id);


/*===========================================================================
FUNCTION gl1_msg_cfg_ul_tch_amr_fr_hr,  gl1_msg_cfg_ul_facch_amr_fr_hr
         gl1_msg_cfg_ul_ratscch_amr_fr_hr, gl1_msg_cfg_ul_sid_first_amr_fr_hr
         gl1_msg_cfg_ul_sid_update_amr_fr_hr,
         gl1_msg_cfg_dl_tch_facch_amr_fr_hr

DESCRIPTION
  These functions are used to configure the TSC and callbacks for the TCH,
  FACCH, RATSCCH, SID_FIRST, SID_UPDATE.  The callbacks will be invoked by
  the frame tick handler as required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_cfg_ul_tch_amr_fr_hr(uint8 TSC, gl1_msg_tx_cb_type *cb,gas_id_t gas_id);

extern void gl1_msg_cfg_ul_facch_amr_fr_hr(uint8 TSC, gl1_msg_tx_cb_type *cb,gas_id_t gas_id);

extern void gl1_msg_cfg_ul_ratscch_amr_fr_hr(uint8 TSC, gl1_msg_tx_cb_type *cb,gas_id_t gas_id);

extern void gl1_msg_cfg_ul_sid_first_amr_fr_hr(uint8 TSC, gl1_msg_tx_cb_type *cb,gas_id_t gas_id);

extern void gl1_msg_cfg_ul_sid_update_amr_fr_hr(uint8 TSC, gl1_msg_tx_cb_type *cb,gas_id_t gas_id);

extern void gl1_msg_cfg_dl_tch_facch_amr_fr_hr(uint8 TSC, gl1_msg_rx_cb_type *cb,gas_id_t gas_id);


/* =======================================================================
**                  GSM FUNCTIONS FOR AMR HR
** ======================================================================= */
/*===========================================================================
FUNCTION gl1_msg_vocoder_rx_amr_hr  ,  gl1_msg_vocoder_tx_amr_hr

DESCRIPTION
  These function are used to transfer data to or from the vocoder.  These
  functions may be called by the task level.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_vocoder_rx_amr_hr(uint8 *data,
                      gl1_msg_amr_frame_info_type *frame,gas_id_t gas_id);

extern void gl1_msg_vocoder_tx_amr_hr(uint8 *data,
                    uint8 datalength,
                      gl1_msg_amr_frame_info_type *frame,gas_id_t gas_id);

/*===========================================================================
FUNCTION gl1_msg_tx_tch_amr_hr,   gl1_msg_tx_facch_amr_hr

DESCRIPTION
  These functions are used to transmit a message on the TCH/FACCH on the next
  block boundary.

DEPENDENCIES
  gl1_msg_store_tx_TSC(), gl1_msg_cfg_ul_tch() and gl1_msg_store_ta(),
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_tx_tch_amr_hr( ARFCN_T ARFCN[4],  int16 TXLEV[4],
                                   uint8 *data,  boolean onset,
                                   uint8 ul_cmi, uint8 dl_cmi,
                                   uint8 cmr, uint8 cmc,
                                   gl1_msg_amr_mode_type curr_codec_mode,
                                   uint8 phase,
                                   boolean sid_first_inh,
                                   boolean sid_update_inh,
#ifdef FEATURE_VAMOS_II
                                   gl1_vamos2_tch_wait_type vamos2_hr_tch_wait_state,
#endif
                                   gas_id_t gas_id);

extern void gl1_msg_tx_facch_amr_hr(ARFCN_T ARFCN[4], int16 TXLEV[4],
                    uint8 *data, boolean onset,
                    uint8 cmi, uint8 cmr,
                    uint8 phase, boolean middle_facch_block,
                    boolean sid_first_inh,
                    boolean sid_update_inh,
#ifdef FEATURE_VAMOS_II
                    boolean amr_middle_facch_block_vamos2,
                    gl1_vamos2_tch_wait_type vamos2_hr_tch_wait_state,
#endif
                    gas_id_t gas_id
                    );

/*===========================================================================
FUNCTION gl1_msg_tx_ratscch_amr_hr, gl1_msg_tx_sid_first_amr_hr
         gl1_msg_tx_sid_update_amr_hr, gl1_msg_tx_saved_sid_update_amr_hr

DESCRIPTION
  These functions are used to transmit a message on the RATSCCH/SID_FIRST/
  SID_UPDATE on the next block boundary.

DEPENDENCIES
  gl1_msg_store_tx_TSC(), gl1_msg_cfg_ul_tch() and gl1_msg_store_ta(),
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_tx_ratscch_amr_hr(ARFCN_T ARFCN[4], int16 TXLEV[4],
                      uint8 *data, boolean onset,
                      uint8 cmi, uint8 cmr,
                      uint8 phase,
                      boolean sacch_in_middle,
                      boolean sid_first_inh,
                      boolean sid_update_inh,
#ifdef FEATURE_VAMOS_II
                      boolean amr_middle_facch_block_vamos2,
                      gl1_vamos2_tch_wait_type vamos2_hr_tch_wait_state,
#endif
                      gas_id_t gas_id
                     );

extern void gl1_msg_tx_sid_first_amr_hr(ARFCN_T ARFCN[4], int16 TXLEV[4],
                    uint8 *data,
                    uint8 cmi,
                    uint8 cmr, uint8 phase,
                    boolean sacch_in_middle,
#ifdef FEATURE_VAMOS_II
                    boolean amr_middle_facch_block_vamos2,
                    gl1_vamos2_tch_wait_type vamos2_hr_tch_wait_state,
#endif
                    gas_id_t gas_id
                    );

extern void gl1_msg_tx_sid_update_amr_hr(ARFCN_T ARFCN[4], int16 TXLEV[4],
                     uint8 *data,
                     uint8 cmi, uint8 cmr,
                     uint8 phase,
                     boolean sacch_in_middle,
#ifdef FEATURE_VAMOS_II
                     boolean amr_middle_facch_block_vamos2,
                     gl1_vamos2_tch_wait_type vamos2_hr_tch_wait_state,
#endif
                     gas_id_t gas_id
                     );

extern void gl1_msg_tx_saved_sid_update_amr_hr(ARFCN_T ARFCN[4], int16 TXLEV[4],
                           uint8 cmi,
                           uint8 cmr,
                           uint8 phase,
                           boolean sacch_in_middle,
#ifdef FEATURE_VAMOS_II
                           boolean amr_middle_facch_block_vamos2,
                           gl1_vamos2_tch_wait_type vamos2_hr_tch_wait_state,
#endif
                           gas_id_t gas_id
                           );

/*===========================================================================
FUNCTION gl1_msg_rx_tch_facch_amr_fr

DESCRIPTION
  This function is used to receive a message on either the TCH or the FACCH.

DEPENDENCIES
  gl1_msg_store_rx_TSC(), gl1_msg_cfg_dl_tch_facch() ,
  must have been called at some time prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_rx_tch_facch_amr_hr(ARFCN_T ARFCN[4], sys_algo_agc_T *agc_ptr,
#ifdef FEATURE_GSM_RX_DIVERSITY
                    sys_algo_agc_T   *agc_ptr_divrx,
#endif
                    gl1_msg_rx_rpt *rpt, boolean facch_only,
                    uint8 phase,
#ifdef FEATURE_VAMOS_II
                    gl1_vamos2_tch_wait_type  vamos2_hr_tch_wait_state,
#endif
                    gas_id_t gas_id);

/*===========================================================================
FUNCTION gl1_msg_abort_tx_tch_amr_hr, gl1_msg_abort_rx_tch_amr_hr

DESCRIPTION
  These functions are used to terminate any TCH/FACCH/RATSCCH/SID_FIRST/
  SID_UPDATE bursts pending.

===========================================================================*/
extern boolean gl1_msg_abort_tx_tch_amr_hr(gas_id_t gas_id);

extern boolean gl1_msg_abort_rx_tch_amr_hr(gas_id_t gas_id);


#endif  /* FEATURE_GSM_AMR */

/*===========================================================================
FUNCTION gl1_msg_SNR_to_dBx16

DESCRIPTION
 Converts the raw SNR returned in metrics reports to dBx16.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern dBx16_T gl1_msg_SNR_to_dBx16( uint16 SNR );

/*===========================================================================

FUNCTION  gl1_msg_algs_linear_to_dBx16

DESCRIPTION
  This function converts a linear value into a dBx16 value.  The return
  value is 16*10*log_10( x ).

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
extern dBx16_T gl1_msg_algs_linear_to_dBx16( uint32 x );

/*===========================================================================

FUNCTION gl1_msg_cap_reported_power_level

DESCRIPTION
  This function maps the ordered TXLEV to the reported TXLEV
  based on the power class.  Also TXLEV values that are mapped
  to the same outout power are capped to the smallest value
  in the range.   This only affect the reported TXLEV and not
  the tranmsitted power

DEPENDENCIES
  txlev - input, as specified in 3GPP specs (05 series)

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
int16 gl1_msg_cap_reported_power_level
(
  ARFCN_T  arfcn,
  int16    txlev
);

/*===========================================================================
FUNCTION gl1_msg_get_current_voc_type

DESCRIPTION
  Function used to return the current vocoder type to PL1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern gl1_msg_voc_type gl1_msg_get_current_voc_type ( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_saic_in_burst

DESCRIPTION
  This function checks if SAIC was used in the burst. From the mDSP SAIC
  code bit 14 is used, and if it's set to '0' then SAIC was not used, and
  '1' if SAIC is used.

DEPENDENCIES
  None

RETURN VALUE
  Boolean     TRUE if SAIC is used.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean gl1_msg_saic_in_burst(uint16   saic_misc_word);

/*===========================================================================

FUNCTION gl1_msg_reset_hard_rxqual_ave

DESCRIPTION
  Will reset the hard_rx_qual averaging params for a timer restart.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_reset_hard_rxqual_ave( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_is_saic_disable_in_idle_valid

DESCRIPTION
  Called to decide if we can disbale SAIC in idle at the start of a PCH receive
  sequence. Will avoid switching SAIC status mid receive. Penalty is we will
  lose a bad block but should get gain from SAIC on next one and until timer
  timesout as timer is then restarted.

DEPENDENCIES
  None

RETURN VALUE
  Boolean to indicate if we can disable SAIC in idle

SIDE EFFECTS
  None
===========================================================================*/
extern boolean gl1_msg_is_saic_disable_in_idle_valid( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_update_hard_rxqual_ave

DESCRIPTION
  Called each PCH/BCCH receive to update the stored hard_rx_qual vals
  that are later used to find the average when the timer expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void    gl1_msg_update_hard_rxqual_ave( uint16 current_hard_rx_qual,gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_set/get_saic_disable_in_idle

DESCRIPTION
  Set/Get the SAIC disable status flag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_set_saic_disable_in_idle( boolean  saic_disable_in_idle,
                                              gas_id_t gas_id );

extern boolean gl1_msg_get_saic_disable_in_idle( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msgi_reset_acq_afc

DESCRIPTION
  This function is used to program the rotator/pdm using the value
  passed by the caller. It also generates an afc adjustment log packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void gl1_msgi_reset_acq_afc( int32 afc_freq ,gas_id_t gas_id);

extern void gl1_msg_reset_pdm( int16 frequency_offset, ARFCN_T arfcn ,gas_id_t gas_id);

#ifdef FEATURE_GSM_MDSP_ESAIC
/*===========================================================================

FUNCTION gl1_msg_clear_esaic_iir_filter

DESCRIPTION
  Tells the mDSP to Clear it's IIR filter for use in applying the ESAIC on
  Downlink channels.

  Currently needs to be called For Downlink DTX and Handover scenarios

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_clear_esaic_iir_filter(boolean inter_handover, boolean result,
                                    gas_id_t gas_id_t);

#endif

/*===========================================================================

FUNCTION gl1_msg_acch_get_nv_flag

DESCRIPTION
  Return the xACCH NV flags.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern uint16 gl1_msg_acch_get_nv_flag (gas_id_t gas_id);

/*===========================================================================

FUNCTION  gl1_msg_check_list_req_pass_num

DESCRIPTION
  This function looks at the retry_with_reset passed in from RR. This is the provided
  by RR as an indication that bcch_decode_list_req has been re-requested as L1 has
  indicated low confidence on the last pass.

DEPENDENCIES

PARAMETERS
  boolean retry_with_reset
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_msg_check_list_req_pass_num(boolean retry_with_reset,gas_id_t gas_id);

/*===========================================================================

FUNCTION  gl1_msg_get_pdm_confidence_factor

DESCRIPTION
  This function looks at the value on the PDM to decide if the value looks
  like a normal value. If it is outside the CONFIDENCE_THRESHOLD's then this
  returns FALSE, or confidence is low verdict.

DEPENDENCIES

PARAMETERS
  None
RETURN VALUE
  boolean FALSE=confidence is low
          TRUE=confidence is high
SIDE EFFECTS
  None

===========================================================================*/
extern boolean gl1_msg_get_pdm_confidence_factor (gas_id_t gas_id);


#ifdef FEATURE_GSM_DYNAMIC_SAIC
/*===========================================================================

FUNCTION gl1_msg_get_saic_state

DESCRIPTION
  Informs Pl1 if SAIC is currently enalbed or disabled

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE = Enabled, FALSE  = disabled
SIDE EFFECTS
  None
===========================================================================*/
boolean gl1_msg_get_saic_state(gas_id_t gas_id);
#endif /*FEATURE_GSM_DYNAMIC_SAIC*/

/*=============================================================================
FUNCTION gl1_msg_get_afc_params

DESCRIPTION
   Gets the current values loaded up in the rotator and the PDM, and returns
   them in the pointers passed into the function.

============================================================================= */
extern void gl1_msg_get_afc_params( int32 *rotator_value_store, int32 *freq_value_store,gas_id_t gas_id );

/*=============================================================================
FUNCTION gl1_msg_set_afc_params

DESCRIPTION
   Sets up the PDM and the rotator to the values passed in.

============================================================================= */
extern void gl1_msg_set_afc_params( int32 rotator_value_store, int32 freq_value_store, gas_id_t gas_id);

extern void gl1_msg_algs_get_arfcn( uint16*      arfcn_ptr,
                                    sys_band_T*  band_ptr,
                                    gas_id_t     gas_id );


/*=============================================================================
FUNCTION  gl1_msg_get_all_subs_in_idle_mode

DESCRIPTION This function returns true if all the subs have G tech active (idle)

DEPENDENCIES

RETURN VALUE True if all the subs are in G idle mode

SIDE EFFECTS
   None
============================================================================= */
extern boolean gl1_msg_get_all_subs_in_idle_mode(void);

/*=============================================================================
FUNCTION  gl1_msg_get_multi_sim_mode

DESCRIPTION This function returns true if multisim mode has been set to
            DSDS or DSDA, false otherwise

DEPENDENCIES

RETURN VALUE This function returns true if multisim mode has been set to
             DSDS or DSDA, false otherwise

SIDE EFFECTS
   None
============================================================================= */
extern boolean gl1_msg_get_multi_sim_mode(void);

/*=============================================================================
FUNCTION  gl1_msg_set_multi_sim_sys_mode

DESCRIPTION Sets gl1_msg_multi_sim_mode value. This function should be called
            when GSM is activated and deactivated.

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msg_set_multi_sim_sys_mode(sys_modem_device_mode_e_type multi_sim_mode);

/*=============================================================================
FUNCTION  gl1_msg_get_multi_sim_sys_mode

DESCRIPTION Returns the mode of operation set when GSM was activated

DEPENDENCIES

RETURN VALUE Returns the mode of operation set when GSM was activated

SIDE EFFECTS
   None
============================================================================= */
extern sys_modem_device_mode_e_type gl1_msg_get_multi_sim_sys_mode(void);

/*=============================================================================
FUNCTION  gl1_msg_get_multi_sim_standby_mode

DESCRIPTION This function returns TRUE if multisim standby mode has been set to
            DSDS or TSTS, FALSE otherwise

DEPENDENCIES

RETURN VALUE This function returns TRUE if multisim standby mode has been set to
             DSDS or TSTS, FALSE otherwise

SIDE EFFECTS
   None
============================================================================= */
extern boolean gl1_msg_get_multi_sim_standby_mode( void );

/*===========================================================================

FUNCTION gl1_msg_ncell_pch_low_monitor_active

DESCRIPTION
  Inform lower layers if the reduced power monitor state is active as this
  is used as a gating criteria for the asynch interrupt processing.

DEPENDENCIES
  None

RETURN
  Whether reduced ncell monitors are active

SIDE EFFECTS
  None
===========================================================================*/
extern boolean gl1_msg_ncell_pch_low_monitor_active( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_get_dtm_expected_power

DESCRIPTION
  Get latest expected_power stored in gl1_hw_dtm_params

DEPENDENCIES
  None

RETURN VALUE
  uint16
SIDE EFFECTS
  None
===========================================================================*/
extern int32 gl1_msg_get_dtm_expected_power(gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_get_dtm_band

DESCRIPTION
  Get current dtm band stored in gl1_hw_dtm_params

DEPENDENCIES
  None

RETURN VALUE
  uint16
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 gl1_msg_get_dtm_band(gas_id_t gas_id);

extern void gl1_msg_clear_async_drift_timing_error( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_get_expected_power

DESCRIPTION
  Get current dtm ARFCN stored in gl1_hw_dtm_params

DEPENDENCIES
  None

RETURN VALUE
  uint16
SIDE EFFECTS
  None
===========================================================================*/
extern uint16 gl1_msg_get_dtm_arfcn(gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_set_mdsp_asynch_int

DESCRIPTION Set the enable trigger to drivers for the asynch interrupt

RETURN VALUE None
===========================================================================*/
extern void gl1_msg_set_mdsp_asynch_int( boolean asynch_int_enable,gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_is_asynch_int_possible

DESCRIPTION
  Inform lower layers if the asynch interrupt processing is allowed based
  on upper later conditions.

DEPENDENCIES
  None

RETURN
  Whether asynch interrupt processing is allowed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean gl1_msg_is_asynch_int_possible( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_get_rat_change_active

DESCRIPTION
  Inform lower layers if change to GSM only mode is being processed

DEPENDENCIES
  None

RETURN
  Whether change to GSM only mode is being processed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean gl1_msg_get_rat_change_active( gas_id_t gas_id );

/*===========================================================================
FUNCTION gl1_msg_set_tt_failed_ho
DESCRIPTION Force removal of any TT of failed cell after failed handover.
DEPENDENCIES None
RETURN VALUE None
===========================================================================*/
extern void gl1_msg_set_tt_failed_ho( gas_id_t gas_id );

/*===========================================================================
FUNCTION gl1_msg_reset_ho_tt_adjust
DESCRIPTION Reset ho_tt_adjust so can track any adjusts on new cell in case of handover failure
DEPENDENCIES None
RETURN VALUE None
===========================================================================*/
extern void gl1_msg_reset_ho_tt_adjust( gas_id_t gas_id );

/*===========================================================================
FUNCTION gl1_msg_get_stored_dtx_indicator
DESCRIPTION Access function to allow access to stored_dtx_indicator variable
DEPENDENCIES None
RETURN VALUE boolean        stored_dtx_indicator
===========================================================================*/
extern boolean gl1_msg_get_stored_dtx_indicator( gas_id_t gas_id );

extern boolean gl1_msg_ncell_pch_low_monitor_active( gas_id_t gas_id );

extern boolean gl1_msg_pdch_block_active( gas_id_t gas_id  );

/*===========================================================================
FUNCTION gl1_msg_tch_loopback
DESCRIPTION
  This function tells the mdsp to put the TCH into loopback A,B or C or no
  loopback.
===========================================================================*/
extern void gl1_msg_tch_loopback( gl1_defs_loopback_type loopback,gas_id_t gas_id );

/*===========================================================================
FUNCTION gl1_msg_utils_binary_search
DESCRIPTION
  This function performs a binary search.
===========================================================================*/
int16 gl1_msg_utils_binary_search(const uint32 data[],
                                  uint32 input,
                                  uint16 sizeof_data);
/*===========================================================================
FUNCTION gl1_msg_core_ten_log_of_power_val
DESCRIPTION
  This function computes the 10log10(power).  It implemets this by
  linearly interpolating a log LUT.  The output of the LUT is a 10log10 value.
  The LUT provided a maximum error of 1/16 dB.
===========================================================================*/
boolean gl1_msg_ten_log_of_power_val(uint32 power, dBx16_T *dBx16_result_ptr);

/*===========================================================================
FUNCTION gl1_msg_compute_rx_power
DESCRIPTION
  This function computes and validates the Rx power at the antenna.  It
  accomplishes this by using the following formula:

              10log(rssi) - Grx

  where Grx represents the entire Rx chain gain from the antenna input to
  baseband.

  NOTE: Grx = G(digital) + G(rf)

  The design of this function assumes that rf_set_rx_gain_data() was
  previously invoked;  the gain value that is required for the Rx power
  computation is established there.

  rssi_value    - input,  (I^2 + Q^2) value
  dBm_x16_value - output, absolute power result
  gain_range    - output, gain range used in deriving the dBm_x16_value
                  (only provided for informational purposes - logging)

===========================================================================*/
rf_measurement_T gl1_msg_compute_rx_power (uint32          rssi_value,
                                           dBx16_T         *dBm_x16_value,
                                           rf_gain_range_T *gain_range,
                                           rf_gain_T       *stored_gain);

/*===========================================================================

FUNCTION gl1_msg_acq_complete

DESCRIPTION
  Inform lower layers if cell acquisition completed.

DEPENDENCIES
  None

RETURN
  Whether cell acquisition is complete.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean gl1_msg_acq_complete( gas_id_t gas_id );

extern boolean gl1_msg_asynch_int_dbg( gas_id_t gas_id );
extern boolean gl1_msg_is_asynch_sleep_enabled( gas_id_t gas_id );
extern boolean gl1_msg_is_sleep_opt3_enabled( gas_id_t gas_id );

extern boolean gl1_msg_is_sbd_enabled( gas_id_t gas_id );

extern boolean gl1_msg_is_epd_allowed( gas_id_t gas_id );

extern boolean gl1_msg_is_early_decode_disabled( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_update_xo_rgs

DESCRIPTION
  This is used to set the XO RGS correctly for the chosen SUB.
  Typically used only during select specific to ensure the XO
  manager has the most recent value for this SUB so when lock
  is released and regained the appropriate latest G SUB value is
  available.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_update_xo_rgs( boolean is_idle, boolean state_info, gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_msg_set_dedicated_gas_id

DESCRIPTION
 Sets the gas-id to use for dedicated mode operation when both subscriptions use
 legacy MVS/GHDI Audio interface

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_set_dedicated_gas_id(gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_get_rach_state

DESCRIPTION
 Returns the state of the RACH state machine

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern rach_state_type gl1_msg_get_rach_state(gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_get_rach_delay

DESCRIPTION
 Returns the rach_delay

DEPENDENCIES
  None

RETURN VALUE
  rach_delay

SIDE EFFECTS
  None
===========================================================================*/
extern int16 gl1_msg_get_rach_delay(gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_set_rach_delay

DESCRIPTION
 sets the rach_delay

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_msg_set_rach_delay(int16 rach_delay, gas_id_t gas_id);


/*===========================================================================
FUNCTION gl1_msg_recovery_update_ccch_params

DESCRIPTION
  Function used as part of the recovery mechanism when wakeup takes to0 long or it is delayed,
  to update CCCH parameters and send them to FW

===========================================================================*/
void gl1_msg_recovery_update_ccch_params(gas_id_t gas_id );

boolean l1_handle_rfacch(tch_facch_decode_rpt *rpt, gl1_msg_chan_type chan, gas_id_t gas_id);

void tch_rfacch_indicator(uint16 rfacch_start_of_blk_fn, gas_id_t gas_id);

#ifdef FEATURE_GSM_AMR
void amr_tch_rfacch_indicator(uint16 rfacch_start_of_blk_fn, gas_id_t gas_id);
#endif

#ifdef DEBUG_FEATURE_GSM_R_FACCH
#error code not present
#endif

#ifdef FEATURE_GSM_AMR_PD_DTX_DETECT
/*===========================================================================

FUNCTION gl1_msg_dtx_detection_algorithm

DESCRIPTION
  This function implement the DTX detection algorithm. This algorithm is
  called at every SACCH report.
  To detect DL DTX the algorithm checks that the RXQUAL_SUB (SACCH frames)
  is good (ie, below DTX_DET_RXQUAL_SUB_THRESHOLD) and the RXQUAL_FULL
  (SACCH + TCH) is degraded (ie above DTX_DET_RXQUAL_FULL_THRESHOLD).

  The above condition has to be true for at least DTX_DET_COUNTER_THRESHOLD
  SACCH cycles before declaring DTX detected.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_dtx_detection_algorithm(byte rxqual_full, byte rxqual_sub, byte rxlev_sub, gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_dtx_detection_get_status

DESCRIPTION
  This function TRUE if dtx is detected

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean gl1_msg_dtx_detection_get_status(gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_amr_pd_update_info

DESCRIPTION
  This function implement the algorithm to determine if AMR partial decode
  should be enabled

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_amr_pd_algorithm(byte rxqual_sub, byte rxqual_full, byte rxlev_sub, gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_amr_pd_update_hard_rxqual

DESCRIPTION
  This function is used to keep track of the minimum hard rxqual reported
  during the SACCH report period, when amr_pd_algorithm is executed min_rxqual
  is reset to its maximum possible value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_amr_pd_update_hard_rxqual(uint32 hard_rxqual, gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_amr_pd_cancel_partial_decode

DESCRIPTION
  This function is used to cancel partial decode. It is usually called
  when the burst metrics are received from firmware

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gl1_msg_amr_pd_cancel_partial_decode(boolean cancel, gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_amr_pd_get_status

DESCRIPTION
  This function TRUE if amr partial decode is enabled

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean gl1_msg_amr_pd_get_status(gas_id_t gas_id);
#endif /* FEATURE_GSM_AMR_PD_DTX_DETECT */

/*===========================================================================

FUNCTION gl1_msg_meas_in_progress

DESCRIPTION
 Returns True if meas is in progress

DEPENDENCIES
  None

RETURN VALUE
  Returns True if meas is in progress

SIDE EFFECTS
  None
===========================================================================*/
boolean gl1_msg_meas_in_progress(gas_id_t gas_id);

#ifdef FEATURE_QSH_DUMP
void gl1_msg_qsh_get_gfw_data_dump(uint8 *dump_data_ptr1,uint8 *dump_data_ptr2,uint8  *gfw_result);
#endif /* FEATURE_QSH_DUMP */

#endif /* GL1_MSG_H */

