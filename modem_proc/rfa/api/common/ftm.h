#ifndef FTM_H
#define FTM_H
/*===========================================================================

           F T M    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the FTM Services module.

REFERENCES
  None

Copyright (c) 2002 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/ftm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   cv      Adding LTE B66 support
05/19/15   sd      FTM iq capture support include dl power param  
01/13/15   bar     Added embedded support for Sensitivity/CNR calculation
12/29/14   bar     Added FTM interface for FTM_RF_IQ_CAPTURE_ANALYZE_SAMPLES() Command.
12/12/14   stw     Add Multiple Scell Support in NS mode
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
11/19/14   ck      Add a FTM feature to set pa bias NV items via diag
11/13/14   ars     Added FTM API to enable/disable DRx chain
10/14/14   jmf     [selftest] Implement FBRx Noise Floor Cal
10/13/14   al      Add ftm_tdscdma_set_devices_params_type
10/09/14   dbz     [selftest] Change the selftest measurement return packet size
10/07/14   dbz     [selftest] Add spectrum flatness measurement feature
09/30/14   jyu     Add ftm_tdscdma_alt_path_oride_params_type
09/19/14   jmf     [SelfTest] Add definitions for VSWR v2 Meas
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/01/14   ec      Add LNA ranges for HL mode on SAWless targets
07/29/14   pk      Reverting 3 carrier HDR RxAGC read support
07/17/14   ms      TDS Sawless RX support
07/10/14   cd/cri  Merge Pin Cal
07/08/14   pk      Implementation of HDR FTM module
07/07/14   zhw     Compiler fix for non-CDMA configuration
05/13/14   kab     Added RF-ML1 interface and support for NS CA
05/13/14   kab     Added interface changes for NS CA
04/28/14   jps     Support for TDS port switching
03/25/14   bsh     Ported CL: 5493095
02/04/14   jmf     Adding XPT mode FTM enums
01/28/14   npi     Support for configuring/apply/reset LTE dedicated notches
01/10/14   npi     Support to disable an SCELL
09/17/13   ck      Revert change in 407183
09/09/13   sbm     new data structure to hold multiple LTE paramters.
09/06/13   ars     Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
09/03/13   ck      Ported CR 407183 to mod type in reduced signaling handover
08/12/13   bsh     Added new field to receive the FTM fixed vector command 
07/30/13   gvn     Change name of stg parameter used in RSB cal
07/23/13   sdu     Included a structure type rfa_rf_lte_asm_update_params_s in payload
07/30/13   aka     FBRX IQ 32 bit format and diag limit change
07/23/13   ych     Add enum ftm_tdscdma_band_type
07/19/13   ac      DBDC bringup change to add the device for 2nd car
07/15/12   ars     Added FTM interface for self-test APIs
07/10/13   aro     Migrated Subsys ID and Mode ID
06/26/13   gvn     SCELL Support for RSB Cal
06/19/13   pk      Added missing FTM Mode IDs
06/01/13   nsh     Support Get Multi Synth Status in Dime
05/25/13   qzh     Porting TDSCDMA IQ capture from NIKEL to DIME
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/20/13   al      Added enum for FTM TDS B40_B
05/14/13   jl      Remove duplicate FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG
05/13/13   jl      Port IQ capture code
05/13/13   gh      Correct DBDC 3C and 4C FTM mode ID
05/07/13   ck      Change return type of ftm_gsm_ber_cleanup
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
04/22/13   ck      Add structures and macros to support GSM IQ capture
03/14/13   tks     Added boolean variable to control stg/tone input
03/14/13   sar     Fixed compiler errors in APQ target. 
03/07/13   bsh     B28/B29 support 
03/05/13   kai     Added enum ftm_wcdma_band_type
02/26/13   tks     Added support for rsb and tx lo leakage calibration 
02/15/13   tws     Back out previous change.
02/15/13   tws     Activate/deactive FTM mode in FTM task context via command.
02/06/13   ck      Add reserved FTM mode ID for customer
02/04/13   ndb/bmg Added FTM_SET_TUNER_TUNE_CODE_OVERRIDE
01/29/13   ec      Add framework for Non Signalling on multiple RF Devices
01/30/13   hdz     Added ftm_xpt_config_rsp_pkt_type
01/30/13   ka      Added secondary carrier logical devices
01/30/13   hdz     Added ftm_xpt_config_payload_type
01/22/13   pl      Implement code review comments
01/20/13   pl      FTM LTE-CA IRAT support
01/17/13   Saul    COMMON. XPT IQ Capture KW Fix
01/16/13   cd      KW fixes to allow for variable payload in load DPD
12/27/12   cd      Updated Tx override packet interface for invalid value
                   handling
12/21/12   cd      Fixed Tx override fields to be signed in order to allow
                   for invalid value programming
12/13/12   Saul    Common. Tx agc override intf. Delay now int32.
12/13/12   Saul    Common. Changed XPT capture, dpd, tx agc override intf.
11/29/12   tws     Add FTM GSM C2 ID.
11/28/12   nrk     Multi_synth_lock support is added
11/20/12   gvn     Featurize LTE for Triton
11/15/12   ka      Added support for multi-carrier Tx
11/09/12   gvn     LTE B14, B23, B26, B42 merge from Nikel
11/08/12   cd      FTM packet type for xPT Tx AGC override
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
10/30/12   spa     Added CDMA chain 3 enum: FTM_1X_C3_C for div on SV path
10/25/12   Saul    Warning fix.
10/25/12   cd/Saul Updated FTM DPD table override command type and added 
                   response packet type
                   - Made XPT IQ capture common.
08/16/12   ars/hdz Modifed FTM frequency adjust logic to apply correction using XO manager API - CDMA
07/24/12   hdz     Added ftm_tx_pwr_limit_type to support set tx pwr limit for c2k
07/21/12   pl      Adding FTM_LTE_CAR1_C and FTM_LTE_CAR1_RX_2_C
07/18/12   jps     Updates to TD-SCDMA burst TX/RX mode data types
07/16/12   jps     Added initial support for TD-SCDMA burst TX/RX mode
07/12/12   pl      Moved FTM command processing to FTM task
                   removed obsolete RFA_RF_LTE_FTM messages
07/12/12   pl      Add FTM LTE CA support
05/22/12   aca     Added FTM command to extract RX AGC data
05/16/12   sa      Added FTM_DATA_INVALID and FTM_BAD_PARAM to ftm_error_type
05/02/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API - LTE
04/24/12   yzw     Added FTM command FTM_LTE_SET_FREQ_ADJUST
04/18/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API - GSM
04/23/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API - WCDMA
04/16/12   jf      Add LTE B6 and B10
03/16/12   php     Added LTE NS support
03/07/12   cri     Added initial DPD support
03/08/12   hdz     Added tx_freq_adjust to ftm_rf_factory_data
02/29/12   yzw     Added LTE FTM command FTM_LTE_SET_LNA_STATE_DBM
02/07/12   can     Added FTM set modulation command.
02/03/12   ems     Added support for LTE RFM mode switch
01/20/12   php     Added support for B34 and B39
12/22/11   sbm     Added LTE band 9 support.
12/05/11   sbm     Added XGP band (B41)
12/08/11   aki     Added FTM_RFCA_MAL_C
10/20/11   adk     Added support for SV diversity.
10/17/11   ars     Added FTM_RF_GET_RX_LEVEL_DBM command
10/17/11   ars     Added FTM_SET_PA_STATE and FTM_SET_TX_POWER_DBM FTM Layer
09/22/11   jyu     Added LTE B12 support
09/14/11   can     Merge MDM9K tips to NikeL.
09/09/11   shb     Use different macro for # of hdet reads (128) vs returns(32)
09/09/11    dw     Fix WCDMA Cal crash
09/07/11   shb     Increased number of HDET reads in linearizer to 128
08/29/11   sr      KW fixes.
08/22/11    gh     Added FTM_RECEIVE_CHAIN_INVALID entry for error checking
08/05/11   whc     Added B25 Support
08/03/11   aro     Added support for FTM command to set OL Tx Power
07/29/11   hy      Resolved TD-SCDMA mode id conflict.
07/11/11   hy      Changed TD-SCDMA FTM Mode ID to resolve conflict.
07/11/11   hy      Including TD-SCDMA support.
06/23/11   pl      Remove obsolete interface : set_rx_on
05/24/11   bmg     CDMA 1x SV chain updates
05/09/11   sar     Removed ftmicap.h for cmi-4 cleanup.
04/28/11   sar     CMI-4 Cleanup.
04/20/11   pl      Remove obsolete message structures
04/11/11   sty     Renamed ftm_secondary_chain_control_enum_type to
                   ftm_sec_chain_ctl_enum_type
03/31/11   sar     Remove #def's to comply with CMI-4.
03/24/11   aro     Removed unused functions
03/07/11   jyu     Added Level 1&2 RF FTM IRAT test
03/03/11   tks     Added support for WCDMA Band XIX.
02/24/11   sty     Added C++ protection
02/18/11   aro     Added FTM_RFM_C SubSystem
02/16/11   aro     Added FTM_DB_RF_HDR enum
02/16/11   adk     FTM_PHONE_MODE_CDMA_BC4 not needed. Use FTM_PHONE_MODE_CDMA_1800.
02/15/11   adk     Added support for BC4 (KPCS).
02/15/11   aro     Removed unused functions
02/09/11   aca     FTM dac setpoint value override support
02/08/11   aca     FTM modulation change support
02/07/11   av      FTM IRAT 3G-2G support: (comment addition)
01/19/10    dw     variable name change for RxLM
01/14/11   can     Initial B18 bring-up on J2 HW.
01/11/11   can     Initial B5 bring-up on Viking A1 HW.
01/07/11   vss     Added LTE B8 Support
01/05/10    dw     Initial RxLM support
12/21/10   whc     FTM Command for SMPS Override
12/15/10   ems     WCDMA Non-signaling Power Control Override
11/11/10   jyu     Fixed lint errors
11/08/10   Saul    Fixed FTM-Online switching crash due to mdsp enable/disable latency.
10/30/10   mkv     Added support for LTE Get Rx Level
10/29/10   mkv     Added support for LTE Reset and Get Sync BLER API
10/26/10   mkv     Added LTE NS Tx AGC override
10/24/10   mkv     Added LTE NS UL Grant reconfiguration
15/10/10   mkv     Support LTE Composite Calibration
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1
10/01/10   whc     Added txagc oride types to ftm_lte_payload_type
09/28/10   qma     Added LTE B3/B20 support
09/27/10   whc     Added FTM command IDs for all LTE bands
08/05/10   anr     Added support for Enhanced Therm API in 1x mode
07/27/10   pl      Remove measurement FTM command handler to Measurement module
07/14/10   qma     Swap ID of FTM_LTE_RX_2_C and FTM_SEQ_C
07/09/10   aki     Merging RFNV FTM changes from dev/rfnv_dev
07/06/10   tnt     Add LTE B4 support
07/01/10   mkv     Adding support LTE LPM HDET Mode.
06/28/10   kma     Added SV module in FTM mode
06/15/10   tnt     Add B11 support
06/07/10   pl      Added FTM support for LTE IM2 CAL
06/04/10   pl      Adding x->L IRAT FTM command
05/12/10   ac      B11 bringup on j8220
03/16/10   pl      Merge FTM_LTE_NS_START_DL and FTM_LTE_NS_START_UL to become FTM_LTE_NS_START_DP
03/04/10   ad      Added support for dynamic IQ capture in INO cont. for WCDMA
02/18/10   pl      Added Set Secondary Chain for LTE
02/18/10   pl      Added GET_LNA_OFFSET for LTE
01/19/10   ka      Added support for multi-carrier ICI calibration.
12/21/09   jps     Updated ftm_meas_extended_pkt_type
11/21/09   mkv     Updating command id to be uint16 instead of uint32 in
                   ftm_lte_payload_type for consistency across technologies.
11/18/09   pl      Remove FEATURE_LTE around FTM_LTE_NS_C and FTM_LTE_C
11/18/09   sar     Removed unused FTM MODE_ID's.
11/11/09   aro     Reverting back the PACK changes
11/11/09   ka      Added channel array for DC-HSPA.
11/11/09   aro     Changed ftm_data_ptr and data_rsp_ptr to PACK pointers
10/23/09   pl      Added  FTM LTE NS UL data support
10/22/09   ka      Added support for 6th LNA state.
10/15/09   hp      Added FTM_GNSS_C to FTM Command Codes enum
10/13/09   pl/mkv  Changed FTM LTE command header type to remove unused field
10/07/09   ra      add support for version 2 hdet[16 bit]
10/06/09   jps     Added FTM command code for IRAT
10/06/09   mkv     Added LTE Calibration structure definitions to rf_lte_payload_type.
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
09/13/09   vb      ICI calibration support
09/14/09   sr      Added back FTM_FMRDS_C and assigned new value to FTM_SVDO_C
09/09/09   aro     Removed DateTime and Author + Extra Space Removal
09/01/09   tnt     Add FTM_INO_C to FTM command code
08/11/09   mkv     Merge MODEM_LTE_DEV into rfa2
08/11/09   bn      FTM WCDMA release 7 waveform feature
07/07/09   dyc     Add FTM_FMRDS_C to FTM cmd codes for FM support.
06/30/09   pl      Adding ACQ_REQ payload
06/15/09   pl      Merged with SCMM code base
06/12/09   pl      Removing unneccessary field in ftm_hdr_type
05/14/09   mkv     Modified LTE FTM packet definition nomenclature and updated comments.
05/01/09   pl      Added LTE specific signals, CMD code and packet type
05/01/09   pl      Stripped down version of ftm.h
03/30/09   aro     Initial SVDO Support : Added new FTM command FTM_SVDO_C
12/05/08   ahg     Modified FTM_NV_CMD_SIG, FTM_WLAN_SIG, FTM_EVT_Q_SIG values.
                   They were same as TASK_OFFLINE_SIG, TASK_STOP_SIG,TASK_START_SIG
09/30/08   aak/vm  Added Support for Band Class Sub Block Implementation
03/21/08   ad      Create runtime configurability for rfrx_enable_chip_reset().
02/06/08   ad      Added ftm_db_to_rf_mode() to remove compiler warnings
01/21/08   ad      Support for ftm_enable_chip_reset()
11/29/07   sar     Removed featurization around definition FTM_IQ_RECORD_BLOCK_SIZE.
11/19/07   adk     Merged changes from FTM tips.
11/09/07   ans     Removed un-used typedefs.
11/08/07   vm      Added support for FTM API supporting CDMA Waveform,
                   CW Waveform and Offset CW Waveform for Tx
11/08/07   ans     Added BC10 support for FTM.
09/17/07   vm/adk  Added function prototype ftm_current_mode_is_rf_cal().

08/01/07    kg     featurization changes for EDGE only support
06/25/07   ad      Added support for single ended BER
06/12/07   tm      Updated ftm_event_type to make it reuseable
05/11/07   dp      Add support for UMTS single ended ber
02/05/07   ycl     Enable FTM_IQ_RECORD_BLOCK_SIZE for all targets.
12/11/06   dbc     Support for MFLO Non-Signaling mode is now under FTM_MF_NS_C
12/07/06   bt      Added support or MBP (multi broadcast platform).
11/17/06   jfc     Moved ftm_rx_action_type to ftm_common.h
10/17/06   ycl     Modifications for initial MSM7600 build.
10/02/06   dbc     Added support for FLO demod. in FTM mode. Featurized by
                   FTM_HAS_MFLO and FTM_MFLO_SUPPORTS_FLO_DECODE
09/26/06   hkk     Added support for BC15, with featurization
07/06/06   anb     Added support to blow fuses via FTM commands.
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
03/28/06   rsr     Changed ftm_gsm_extended_parameters to be of type uint8,
                   This variable is a place holder for a buffer. Defining it
                   as void* implies it can be used as a pointer... when, in fact,
                   diag returns the first element of the allocated buffer in this
                   variable. If it is defined as a pointer, we could erraneously
                   dereference the first element in the buffer.
02/21/06  tws      Change WCDMA_1800 to BC3. Add support for BC4 and BC9.
01/16/06   rmd     Implemented BC11 and BC14 code review results. Including:
                   * Mainline BC11 and BC14 code.
12/19/05   dp      Add WCDMA 2nd chain command code.  Open up
                   ftm_receive_chain_handle_type enumeration for WCDMA targets.
11/01/05   rmd     Updated the definition for FTM_PHONE_MODE_CDMA_BC14 and
                   added the definition for FTM_PHONE_MODE_CDMA_BC11.
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
09/12/05   hkk     Changes for TX/RX sweep cal
05/02/05   dbc     Added inital support for MFLO.
03/22/05   ra      Removed FTM_HAS_UMTS wrapper from ftm_get_mode() to allow all system mode access.
                   Also, deleted ftm_set_mode api located in this file(there is a 1x specific ftm_set_mode which has different
                   functionality and is not part of this change). It set the global variable ftm_mode however this is to
                   be done in ftm_activate_protocol and ftm_deactivate_protocol.
12/03/04   bmg     Added 1x calibration v2 API - merged 03/21/05
03/11/05   ka      Added support for gsm extended commands.
10/31/04   ra      Removed reserved FTM_HWTC command codes
10/19/04   dp      Added FTM_WCDMA_BER_C command code.
10/15/04   ka      Removed unused code.
10/06/04   lp      Added FTM_CAMERA_C command code.
10/05/04   ka      Added mode ID for Customer Extensions.
10/05/04   ka      Added support for FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL.
10/01/04   ra      Renamed FTM_GET_RF_MODE to FTM_GET_RF_DB_STATE.
10/01/04   ra      Renamed FTM_SET_RF_MODE to FTM_SET_RF_DB_STATE.
10/01/04   wd      changed ftm header fields to support rsp_pkt_size.
10/01/04   wd      Added the right diagpkt header structure that defines the
                   subsys cmd id as a word as it should.
10/01/04   et      moved ftm_get_rf_mode prototype outside of FTM_HAS_UMTS
09/17/04   ra      removed FTM_LEDS... there are defined in ftm_task.h
09/08/04   ra      1X merge with UMTS
09/07/04   lcl     Added get IM2 with suppression.
09/03/04   dp      Added access functions for ftm mode variables
08/23/04   xw      Renamed FEATURE_FTM_BT to FEATURE_BT.
08/22/04   ra      Merge 1X and UMTS
08/23/04   xw      Renamed FEATURE_FTM_BT to FEATURE_BT.
8/12/04    ka      Added support for WCDMA 1800 band.
10/02/03   ka      Added support for I2C.
10/02/03   ka      Updated msg macros to remove legacy ids.
10/02/03   ka      Modified includes to fix compilation with FTM_USE_ASYNC_PKT
10/02/03   ka      Added support for variable length response packet.
06/26/03   lcl     Added LED to ftm_task
05/23/03   xw      Removed #define FEATURE_WCDMA_IM2_CAL
02/25/03   xw      Modified ftm_msg_type to support async ftm diag
12/10/02   xw      Added ftm_rf_mode_type
08/25/02   sb      Initial revision

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"              /* Target specific definitions            */
#include "comdef.h"              /* Definitions for byte, word, etc.       */
#include "diagpkt.h"
#include "ftm_subsys_id.h"
#include "ftm_mode_id.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "queue.h"
#include "rex.h"
#ifdef FEATURE_GNSS_ONLY_NO_WWAN
#include "sys.h"
#endif
#include "prot.h"
#include "msg.h"
#include "rfcom.h"
#include "ftm_gsm.h"
#include "ftm_mgr_msg.h"

#ifdef FEATURE_LTE
#include "rflte_msg.h"
#endif
#include "rfm_cdma_carrier_types.h"
/*===========================================================================

                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/

/* FTM signals */
#define FTM_CMD_Q_SIG          0x0001    /* A command is available in the queue. */
#define FTM_RPT_TIMER_SIG      0x0002    /* Used to signal a watchdog report.    */
#define FTM_HS_CMD_SIG         0x0004    /* This signal is used by the test      */
#define FTM_ACK_SIG            0x0008    /* State change ack signal              */
#define FTM_CMD_RSP_SIG        0x0010    /* Command response signal              */
#define FTM_CLK_WAIT_SIG       0X0020    /* Used generic for ftm_clk_rex_wait    */
#define FTM_SIG_WAIT_SIG       0X0040    /* Used generic for ftm_sig_rex_wait    */
#define FTM_ASYNC_PKT_SIG      0x0080    /* Used to process asyn packets         */
#define FTM_MDSP_ENABLE_SIG    0x0100    /* Signal for msdsp enable              */
#define FTM_ENABLE_PROT_SIG    0x0200    /* FTM enable protocol signal           */
#define FTM_MC_READY_SIG       0x0400    /* MC FTM ready siganl                  */
#define FTM_BT_SIG             0x0800    /* BT FTM siganl                        */
#define FTM_SEARCH_SIG         0x1000
#define FTM_NV_CMD_SIG         0x10000
#define FTM_EVT_Q_SIG          0x40000    /* Event signal                         */
#define FTM_MDSP_DISABLE_SIG   0x8000    /* Signal for mdsp disable */
#define FTM_NUM_CMDS           5

#define FTM_NUM_HDET_TO_RETURN 32
#define FTM_NUM_HDET_TO_READ   128
#define FTM_IQ_RECORD_BLOCK_SIZE   256

/*! Used to indicate that the link manager buffer is invalid */
#define FTM_INVALID_LM_BUFFER 65535

/*===========================================================================

                                  MACROS

===========================================================================*/
#define IS_FTM_IN_TEST_MODE() ( ftm_get_rf_db_state() !=  FTM_DB_RF_IDLE )

/* Macros for Self-Verification*/
#define FTM_FBRX_MEASURE_TX_POWER     0x00000001
#define FTM_FBRX_MEASURE_EVM          0x00000002
#define FTM_FBRX_MEASURE_ACLR         0x00000004
#define FTM_FBRX_SAVE_TX_IQ_SAMPLES   0x00000008
#define FTM_FBRX_SAVE_RX_IQ_SAMPLES   0x00000010
#define FTM_FBRX_MEAS_VSWR            0x00000020
#define FTM_FBRX_MEAS_VSWR_V2         0x00000040
#define FTM_FBRX_MEAS_FLATNESS        0x00000080

/* Macros for FTM IQ Capture analysis */
#define FTM_IQ_CAPTURE_MEASURE_ESTIMATED_SENSITIVITY  0x00000001
#define FTM_IQ_CAPTURE_MEASURE_CTON                   0x10000000
/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/


typedef enum {
    AMSS_MODE,
    DMSS_MODE = AMSS_MODE,
    FTM_MODE
} ftm_mode_type;

extern ftm_mode_type ftm_mode;

enum {
    FTM_HWTC          = 10,
    FTM               = 11,

    FTM_MAX
};

typedef enum {
    FTM_XPT_CONFIG_NOPT = -1,
    FTM_XPT_CONFIG_APT = 0,
    FTM_XPT_CONFIG_EPT = 1,
    FTM_XPT_CONFIG_ET  = 2,
    FTM_XPT_CONFIG_FBRX= 3,
} ftm_xpt_config_type;

typedef enum {
    FTM_RF_TECH_CDMA,                 /* RF is in CDMA tech     */
    FTM_RF_TECH_WCDMA,                /* RF is in WCDMA tech    */
    FTM_RF_TECH_GSM,                  /* RF is in GSM tech      */
    FTM_RF_TECH_LTE,                  /* RF is in LTE tech      */
    FTM_RF_TECH_TDSCDMA,              /* RF is in TDSCDMA tech  */
    FTM_RF_TECH_UNKNOWN
} ftm_rf_technology_type;

typedef enum {
    FTM_RECEIVE_CHAIN_0,
    FTM_RECEIVE_CHAIN_1,
    FTM_RECEIVE_CHAIN_2,
    FTM_RECEIVE_CHAIN_3,
    FTM_RECEIVE_CHAIN_INVALID
} ftm_receive_chain_handle_type;

typedef enum {
    US_PCS_BAND,
    US_CELLULAR_BAND
} digital_band_type;

typedef  enum {
    DISABLE_SECONDARY_CHAIN,
    RECEIVE_DIVERSITY,
    OFF_FREQUENCY_SEARCH
} ftm_sec_chain_ctl_enum_type;

typedef enum {
    FTM_PRIMARY_CHAIN_ONLY,
    FTM_SECONDARY_CHAIN_ONLY,
    FTM_PRIMARY_AND_SECONDARY
} ftm_second_chain_test_call_type;

typedef  enum {
    /*The synthesizer is valid for radio configuration, but it is
      currently disabled*/
    FTM_SYNTH_STATUS_DISABLED, 
    /*The synthesizer has been unlocked after at least one tuning attempt. 
      After this state is set, it will not be changed until reset by the 
      command FTM_RESET_MULTI_SYNTH_STATE*/
    FTM_SYNTH_STATUS_UNLOCKED, 
    /*The synthesizer is valid for radio configuration, but had been found
      locked in all valid queries*/
    FTM_SYNTH_STATUS_LOCKED,
    /*The synthesizer status has not been updated since the 
      FTM_RESET_MULTI_SYNTH_STATE command was called*/
    FTM_SYNTH_STATUS_UNDETERMINED,
    /*The synthesizer status is not applicable to the currently selected 
      carrier and receiver mode*/
    FTM_SYNTH_STATUS_NOT_APPLICABLE,
    /*Default value to indicate that status has not been  filled.*/
    FTM_SYNTH_STATUS_INVALID
} ftm_multi_synth_lock_status_enum_type;

typedef enum {
    FTM_SUCCESS,
    FTM_FAILURE,
    FTM_CMD_DOES_NOT_APPLY_TO_TARGET,
    FTM_RF_FREQ_ADJUST_ERROR_TECHNOLOGY_NOT_SUPPORTED,
    FTM_DATA_INVALID,
    FTM_BAD_PARAM,
    FTM_ERROR_SIZE=0xFFFF
} ftm_error_type;

/* Tuner override status type as per the ICD document  */
typedef enum {
    FTM_TUNER_OVERRIDE_TUNE_CODE_SUCCESS,
    FTM_TUNER_OVERRIDE_TUNER_NOT_PRESENT,
    FTM_TUNER_OVERRIDE_TUNE_CODE_FAILURE,
    FTM_TUNER_OVERRIDE_ERROR=0xFFFFFFFF
} ftm_tuner_override_status_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Generic Header used for all FTM Task Commands. */
typedef struct {
    q_link_type    link;               /* Queue link */
    q_type         *done_q_ptr;        /* Queue to place this cmd on when done */
} ftm_hdr_type;

/* This is the Message which brings all Command packets under one name.  */
typedef struct {
    ftm_hdr_type  hdr;
    uint16        req_pkt_len;    /* temp req packet length */
    uint16        rsp_pkt_len;    /* temp rsp packet length */
    void          *ftm_data_ptr;  /* pointer to requesting ftm or hwtc data */
    void          *data_rsp_ptr;  /* pointer to memory allocated for respond packet */
} ftm_msg_type;


typedef enum {
    FTM_EVENT_DIAG,
    FTM_EVENT_ADSP,
    FTM_EVENT_CAMERA
} ftm_event_category;


typedef struct {
    ftm_hdr_type           hdr;
    ftm_event_category     evt_cat;
    boolean                reuse;
    boolean                queued;
    void                   *evt_data_ptr;
} ftm_event_type;


typedef struct {
    uint16                 ref_ct;           /* helps cleanup */
    uint16                 req_pkt_len;      /* req packet length */
    void                   *ftm_data_ptr;    /* pointer to data */
    int                    max_alloc;
} ftm_diag_event_type;



typedef struct {
    ftm_diag_event_type                   ftm_diag_event;
    diagpkt_subsys_delayed_rsp_id_type    delay_rsp_id;
} ftm_diag_128_event_type;


typedef struct {
    ftm_diag_event_type                   ftm_diag_event;
    uint16                                ftm_subsys_code;
} ftm_diag_125_event_type;

/* Enums for supported LTE bandwidths */
typedef enum {
    FTM_BW_LTE_1P4MHz,
    FTM_BW_LTE_3MHz,
    FTM_BW_LTE_5MHz,
    FTM_BW_LTE_10MHz,
    FTM_BW_LTE_15MHz,
    FTM_BW_LTE_20MHz,
    FTM_NUM_LTE_BWS,
    FTM_BW_LTE_INVALID
} ftm_lte_bw_type;

/* Enums for supported LTE bandwidths */
typedef enum {
    FTM_LTE_PUSCH = 0,
    FTM_LTE_PUCCH,
    FTM_LTE_UpPTS,
    FTM_LTE_SRS,
    FTM_LTE_CW_WAVEFORM,
    FTM_LTE_WAVEFORM_INVALID
} ftm_lte_waveform_type;

typedef enum {
    FTM_PA_R0,
    FTM_PA_R1,
    FTM_PA_R2,
    FTM_PA_R3,

    FTM_PA_RMAX
} ftm_pa_range_type;

typedef enum {
    FTM_RX_LIN_STATE_LOW,
    FTM_RX_LIN_STATE_HIGH
} ftm_rx_lin_state_type;

/*! @brief Bus resource enum

*/
typedef enum
{
  FTM_BUS_RESOURCE_SSBI,    /*!< SSBI write or read  */
  FTM_BUS_RESOURCE_RFFE,    /*!< GRFC write or read  */
  FTM_BUS_RESOURCE_GRFC,    /*!< GRFC write */
  FTM_BUS_RESOURCE_NUM,
  FTM_BUS_RESOURCE_MAX=FTM_BUS_RESOURCE_NUM
} ftm_bus_resource_type;
	
typedef struct
{
  rf_burst_type burst_type;
  ftm_bus_resource_type resource_type;
  uint16 address;
  uint8 slave_id;
  uint8 channel;
  uint8 half_rate;
  uint8 rd_delay;
  boolean extended_cmd;
}ftm_read_device_status;


typedef enum {
    FTM_LNA_RANGE_0,
    FTM_LNA_RANGE_1,
    FTM_LNA_RANGE_2,
    FTM_LNA_RANGE_3,
    FTM_LNA_RANGE_4,
    FTM_LNA_RANGE_5,
    FTM_LNA_RANGE_MAX,  /* Max for non-SAWless targets */
    /* Following gain states are for use on SAWless targets in V4 cal.
       MSB is set in V4 cal tools to set HL mode on SAWless target.   */
    FTM_SAWLESS_HL_LNA_RANGE_0 = 0x80, 
    FTM_SAWLESS_HL_LNA_RANGE_1,
    FTM_SAWLESS_HL_LNA_RANGE_2,
    FTM_SAWLESS_HL_LNA_RANGE_3,
    FTM_SAWLESS_HL_LNA_RANGE_4,
    FTM_SAWLESS_HL_LNA_RANGE_5,
    FTM_SAWLESS_HL_LNA_RANGE_MAX
} ftm_lna_range_type;

/* For SAWless targets, we use the MSB of lna state to set high or low lin mode - assumes uint8 format */
#define FTM_GSM_MASK_OFF_LNA_STATE_MSB(lna_state) (lna_state & 0x7F)
#define FTM_GSM_CHECK_LNA_STATE_MSB(lna_state) ((lna_state & 0x80) ? TRUE : FALSE )


typedef enum{
    FTM_RSB_IQ_CAP_FAILURE   = -4, 
    FTM_RSB_LM_CON_FAILURE   = -3,
    FTM_TXLO_IQ_CAP_FAILURE  = -2,
    FTM_TXLO_LM_CON_FAILURE  = -1, 
    FTM_UNSUPPORTED_BAND     = 0, 
    FTM_INT_CAL_SUCCESS      = 1,
} ftm_internal_cal_status_type; 

/* Enums for WCDMA FTM band */
typedef enum
{
  WCDMA_FTM_BAND_1  = 1,
  WCDMA_FTM_BAND_2  = 2,
  WCDMA_FTM_BAND_3  = 3,
  WCDMA_FTM_BAND_4  = 4,
  WCDMA_FTM_BAND_5  = 5,
  WCDMA_FTM_BAND_8  = 8,
  WCDMA_FTM_BAND_9  = 9,
  WCDMA_FTM_BAND_11 = 11,
  WCDMA_FTM_BAND_19 = 19,
} ftm_wcdma_band_type;

/* Enums for TDSCDMA FTM band */
typedef enum
{
  TDSCDMA_FTM_BAND_B34  = 34,
  TDSCDMA_FTM_BAND_B39  = 39,
  TDSCDMA_FTM_BAND_B40  = 40,
} ftm_tdscdma_band_type;

typedef PACK(struct)
{
    word id;
    int2 val;
} ftm_id_val_type;

typedef PACK(struct)
{
    byte byte0;
    byte byte1;
    byte byte2;
} ftm_byte_type;

typedef enum {
    CW_OFFSET_ZER0_KHZ = 0,
    CW_OFFSET_19_2_KHZ = 1
}ftm_tx_waveform_attrib_type;

typedef enum {
    CDMA_WAVEFORM = 0,
    CW_WAVEFORM = 1
}ftm_tx_waveform_type;

typedef PACK(struct)
{
    ftm_tx_waveform_type ftm_tx_waveform;
    ftm_tx_waveform_attrib_type ftm_tx_waveform_attrib;
}ftm_tx_waveform_data_type;

typedef enum {
    FTM_PDM_TX_AGC_ADJ = 2,
    FTM_PDM_TRK_LO_ADJ = 4,

    FTM_PDM_MAX_NUM
} ftm_pdm_id_type;

typedef PACK(struct)
{
    byte  byte0;
    byte  byte1;
    byte  byte2;
    int16 intv;
} ftm_mix_type;

typedef PACK(struct)
{
    ftm_error_type error_code;
} ftm_ici_result_type;

typedef PACK(struct)
{
  uint8 mode;
  uint8 carrier;
  ftm_ici_result_type result;
} ftm_ici_type;

typedef PACK(struct)
{
    ftm_error_type error_code;
    uint16         a;
    uint16         b;
} ftm_iqm_result_type;


/* the real definition of the diagpkt_subsys_header_type*/
typedef PACK(struct)
{
    diagpkt_cmd_code_type         cmd_code;
    diagpkt_subsys_id_type        subsys_id;
    diagpkt_subsys_cmd_code_type  subsys_cmd_code;
} ftm_diagpkt_subsys_header_type;

/*
Definition for subsys_cmd_code_type.
*/
typedef uint8 cmd_code_type;

/*! 
Definition for  subsys_id_type
*/
typedef uint8 subsys_id_type;

/*! 
Definition for  subsys_cmd_code_type
*/
typedef uint16 subsys_cmd_code_type;

/* 	The real definition of the ftm_subsys_header_type*/
typedef PACK(struct)
{
    cmd_code_type         cmd_code;
    subsys_id_type        subsys_id;
    subsys_cmd_code_type  subsys_cmd_code;
}ftm_subsys_header_type;



/* the real definition of the ftm_diagpkt_subsys_hdr_type_v2 */
typedef PACK(struct)
{
    uint8 command_code;
    uint8 subsys_id;
    uint16 subsys_cmd_code;
    uint32 status;
    uint16 delayed_rsp_id;
    uint16 rsp_cnt; /* 0, means one response and 1, means two responses */
}
ftm_diagpkt_subsys_hdr_type_v2;


typedef PACK(struct)
{
    ftm_diagpkt_subsys_header_type    diag_header; /* same as diagpkt_subsys_header_type */
    byte data[1];
} ftm_subsys_cmd_packet_type;


typedef PACK(struct)
{
    ftm_diagpkt_subsys_hdr_type_v2 header;
    byte data[1];
} ftm_subsys_rsp_packet_type;


/* FTM PKT Header */
typedef PACK(struct)
{
    word cmd_id;            /* command id (required) */
    word cmd_data_len;      /* request pkt data length, excluding the diag and ftm headers
                               (optional, set to 0 if not used)
                            */
    word cmd_rsp_pkt_size;  /* rsp pkt size, size of response pkt if different then req pkt
                               (optional, set to 0 if not used)
                            */
} ftm_cmd_header_type;

typedef PACK(struct)
{
    uint16 chain_mask;
    uint8 enable;      
}ftm_config_deconfig_rx_chain_type;

typedef PACK(struct)
{
    uint8 bc;
    uint8 bd;
    int8  dhs;
    int8  dec;
    int8  ded;
    uint8 bed2;
    uint8 mu;
    uint8 num_dpdch;
    boolean hsdpcch_en;
    uint8 modulation_scheme;
    int16 reserved0;
    int16 reserved1;
} ftm_config_eul_data_type;

typedef PACK(struct)
{
    uint8 enable_flag;
    PACK(union)
    {
        int16 tx_power_dbm_1x;
        int16 tx_power_dbm_wcdma;
    }tx_power;
}ftm_set_tx_power_dbm_data_type;

typedef PACK(struct)
{
    PACK(union)
    {
        int16 rx_level_agc_1x;
	/* Tool does not support passing all the carriers RxAGC, 
	   so it is defined as array of 1. 
	   It will be changed to RFM_CDMA_CARRIER_NUM + 1 in future */
	int16 rx_level_agc_hdr[1];
        int16 rx_level_agc_wcdma;
        int16 rx_level_agc_tdscdma;
    }rx_level_agc;

    PACK(union)
    {
        int16 rx_level_dbm_1x;
	/* Tool does not support passing all the carriers RxAGC, 
	   so it is defined as array of 1. 
	   It will be changed to RFM_CDMA_CARRIER_NUM + 1 in future */
        int16 rx_level_dbm_hdr[1];
        int16 rx_level_dbm_wcdma;
        int16 rx_level_dbm_tdscdma;
    }rx_level_dbm;

}ftm_get_rx_level_dbm_data_type;

#define FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER 16

typedef PACK(struct)
{
    /* Selects the receiver chain for which Rx level needs to be measured*/
    uint16 select_rx_chain;      
}ftm_get_multiple_rx_level_dbm_data_type;

typedef enum{
  FTM_RX_LEVEL_DBM_TEST_SUCCESS,
  FTM_RX_LEVEL_DBM_TEST_GENERAL_FAILURE,
  FTM_RX_LEVEL_DBM_TEST_ATLEAST_ONE_RECEIVER_BAD_STATE,  
  FTM_RX_LEVEL_DBM_TEST_INVALID_RX_CHAIN,  
  FTM_RX_LEVEL_DBM_TEST_MAX_ERROR = 0xFFFF
}ftm_rx_level_dbm_error_code_type;

typedef PACK(struct)
{
  diagpkt_cmd_code_type            cmd_code;
  diagpkt_subsys_id_type           subsys_id;
  diagpkt_subsys_cmd_code_type     subsys_cmd_code;
  word                             ftm_rf_cmd;
  /* Selects the receiver chain for which Rx level needs to be measured*/
  uint16                           select_rx_chain;
  /* Error code */
  ftm_rx_level_dbm_error_code_type ftm_error_code;        
  /* Number of bytes in the response packet that will hold the result for Rx 
  level measurement */
  uint16                           payload_size;          
  /* Number of bytes in the results field is variable and is defined by "payload_size".
  The results will follow the sequence of bits enabled in "select_rx_chain" field
  starting with the least significant bit. */
  word                             results[32];          
}ftm_get_multiple_rx_level_dbm_rsp_pkt_type;

typedef PACK(struct)
{
    PACK(union)
    {
        int32  user_freq_adjust_ppb_cdma;
        int32  user_freq_adjust_ppb_gsm;
        int32  user_freq_adjust_ppb_wcdma;
        int32  user_freq_adjust_ppb_tdscdma;
    }user_freq_adjust_ppb;

    PACK(union)
    {
        uint32 center_frequency_hz_cdma;
        uint32 center_frequency_hz_gsm;
        uint32 center_frequency_hz_wcdma;
        uint32 center_frequency_hz_tdscdma;
    }center_frequency_hz;

    uint8  enable_xo_freq_adjust;

    PACK(union)
    {
        int32 total_freq_adjust_hz_cdma;
        int32 total_freq_adjust_hz_gsm;
        int32 total_freq_adjust_hz_wcdma;
        int32 total_freq_adjust_hz_tdscdma;
    }total_freq_adjust_hz;
}ftm_set_freq_adjust_type;

typedef PACK(struct)
{
    int16 txagc;
    boolean parange_override;
    uint8 parange;
} ftm_wcdma_set_txagc_data_type;

typedef PACK(struct)
{
    int16 start;
    int16 stop;
    int16 step;
    uint16 delay;
    boolean parange_override;
    uint8 parange;
} ftm_wcdma_txagc_sweep_data_type;

typedef PACK(struct)
{
boolean override_enable;
uint32 tpc_pattern;
}ftm_wcdma_tpc_control_data_type;

typedef PACK(struct)
{
    int8 index;
    uint32 mask;
} ftm_block_info_type;

typedef PACK(struct)
{
  uint32  testbus_sel;
  uint16  log_client_id;
  uint32  buf_size;
  ftm_ici_result_type result;
} ftm_config_iq_log_type;

typedef PACK(struct)
{
    int16 txagc;
    boolean parange_override;
    uint8 parange;
} ftm_tdscdma_set_txagc_data_type;

typedef PACK(struct)
{
    int16 start;
    int16 stop;
    int16 step;
    uint16 delay;
    boolean parange_override;
    uint8 parange;
} ftm_tdscdma_txagc_sweep_data_type;

typedef PACK(struct)
{
boolean override_enable;
uint32 tpc_pattern;
}ftm_tdscdma_tpc_control_data_type;

/*-----------------------------------------------------------------------*/
/* This structure is used to support overriding Alternate Path Index  */
typedef PACK(struct)
{
  /*! Defines the Master Logical Device Index */
  uint8 master_dev_id;  
  /*! Defines the Master Logical Device Alt Path Index */
  uint8 master_dev_alt_path_idx;
  /*! Defines the Slave Logical Device Index */
  uint8 slave_dev_id;
  /*! Defines the Slave Logical Device Alt Path Index */
  uint8 slave_dev_alt_path_idx;
} ftm_tdscdma_alt_path_oride_params_type;

/* This structure is used to support dynamially setting devices */
typedef PACK(struct)
{
  /*! Defines the Primary device */
  rfcom_device_enum_type pri_device;  
  /*! Defines the Secondary device */
  rfcom_device_enum_type sec_device;
  /*! Defines the Tx device */
  rfcom_device_enum_type tx_device;
} ftm_tdscdma_set_devices_params_type;

typedef PACK(struct)
{
  uint8 num_slots;
  uint8 mod_type;
  int16 ts1_pwrlvl;
  int16 ts2_pwrlvl;
  int16 ts3_pwrlvl;
  int16 ts4_pwrlvl;
  int16 ts5_pwrlvl;
  uint16 reserved1;
  uint16 reserved2;
  uint16 reserved3;
  uint16 reserved4;
  uint16 error_code;
}ftm_tdscdma_burst_tx_params_data_type;

typedef PACK(struct)
{
  boolean on_off;
  uint16 error_code;
}ftm_tdscdma_burst_tx_on_off_data_type;

typedef PACK(struct)
{
  int16 exp_dl_pwr;
  uint8 enable;
  uint16 reserved1;
  uint16 reserved2;
  uint16 error_code;
}ftm_tdscdma_burst_rx_params_data_type;

typedef PACK(struct)
{
    boolean status;
	ftm_multi_synth_lock_status_enum_type rx_pll_state;
	ftm_multi_synth_lock_status_enum_type tx_pll_state;

}ftm_tdscdma_multi_synth_state;

typedef enum
{
  TDS_PORT_A = 0,/* this is the default setting */
  TDS_PORT_B = 1,/* this is the port B setting */
  TDS_PORT_MAX = 2,/* this is the Max allowed setting */
} ftm_tdscdma_port_state_type;

typedef struct
{
  ftm_tdscdma_port_state_type asm_port_state; /* The port state needs to be preconfigured */
  uint16 channel; /* channel to identify the band */
  uint16 reserved1;
  uint16 reserved2;
  uint16 reserved3;
  uint16 reserved4;
  uint16 reserved5;
  uint16 error_code;
} ftm_tdscdma_asm_update_params_data_type;


/*! @brief This following structure is used for
 *         FTM to manage RxLM buffers.
 */
typedef struct
{
  uint32 buf_idx; /* RxLM buffer index */
  boolean is_buf_allocated; /* Flag to indicate if buffer has been allocated */
} ftm_lm_buf_type;

typedef PACK(struct)
{
  boolean pred_enable;
  uint16 amam_tbl[256];
  int16 ampm_tbl[256];
  uint16 iq_gain;
  uint16 env_scale;
}ftm_dpd_tbls_type;

/* Tuner V0 tunecode Config data structure as per the ICD document */
typedef PACK(struct)
{
  uint8 version;
  PACK(union)
  {
    PACK(struct)
    {
      uint8 c1_tune_code;
      uint8 c2_tune_code;
      uint8 switch_config;
    }version_0;
  }tunecode_config;
}ftm_tunecode_config_data_type;

/* Tuner override data structure as per the ICD document */
typedef PACK(struct)
{
  uint8 override_flag;  
  ftm_tunecode_config_data_type config_data;
  ftm_tuner_override_status_type result;
  uint16 nv_data;
  uint32 tuner_id;
}ftm_tuner_tunecode_override_data_type;

typedef PACK(struct)
{
  boolean tx_pwr_limit_override;
  int16 tx_pwr_limit_dbm;
}ftm_tx_pwr_limit_type;

typedef PACK(struct)
{
  boolean status_flag;
  ftm_multi_synth_lock_status_enum_type rx_synth_status;
  ftm_multi_synth_lock_status_enum_type tx_synth_status;
}ftm_get_multi_synth_lock_data_type;

/* *************************************************************/
/* ***************FTM IQ capture structures and macros*****************/
/* *************************************************************/

#define FTM_IQ_8_BIT_FORMAT 0
#define FTM_IQ_16_BIT_FORMAT 1
#define FTM_IQ_32_BIT_FORMAT 2


#define FTM_IQ_SAMPLE_NUM_OF_BYTE 2 // number of bytes of each I or Q sample.

#define FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG 3072 // The max. number of bytes that be tranfered through Diag in GET SAMPLES cmd of FTM IQ capture. The value is obtained through experiments.

typedef enum{
  FTM_IQ_SUCCESS,
  FTM_IQ_GENERAL_FAILURE,
  FTM_IQ_IQ_SAMPLES_UNAVAILABLE_ON_RFM_DEV,
  FTM_IQ_IQ_SAMPLES_UNAVAILABLE,
  FTM_IQ_ILLEGAL_COMBIMATION,
  FTM_IQ_SAMPLES_INVALID_FORMAT,
  FTM_IQ_SAMPLES_MEM_ALLOCATION_FAILURE,
  FTM_IQ_PROCESSING_ERROR,
  FTM_IQ_MAX_ERROR = 0xFFFF
}ftm_iq_capture_error_code_type;

typedef enum{
  FTM_GSM_SUCCESS,
  FTM_GSM_GENERAL_FAILURE,
}ftm_gsm_error_code_type;


// This structure is used to extract data and update for GET CONFIG command.
typedef PACK(struct)
{
  ftm_iq_capture_error_code_type ftm_error_code; // Error type
  uint16 max_sample_size; // Max number of continous samples that can be acquired 
  uint16 sample_data_format;  // Sample data format i.e. is it 8-bit or 16-bit
  uint32 sampling_freq; // Sampling frequency at which samples are obtained
  uint32 max_diag_sample_size; // The maximum number of samples that can be passed through Diag.
}ftm_iq_capture_get_config_type;


// This structure is used to extract data and update for ACQUIRE command.
typedef PACK(struct)
{
  ftm_iq_capture_error_code_type ftm_error_code; // Error type
  uint32 sample_size; // Number of samples to be acquired
  byte carrier_id;  // Carrier whose samples are to acquired , Note: Antenna chain will be determined by sub sys cmd code in the COMMAND.
}ftm_iq_capture_acquire_samples_type;


// This structure is used to extract data and update for GET SAMPLES command.
typedef PACK(struct)
{
  ftm_iq_capture_error_code_type ftm_error_code; // Error type
  uint32 sample_offset;  //  Offset (in unit of IQ pairs) at which samples are collected
  uint32 sample_size;    // Number of samples (IQ-pairs) that are to be collected
} ftm_iq_capture_get_samples_type;

// This structure is used to extract data and update for ANALYZE SAMPLES command.
typedef PACK(struct)
{
  uint16 select_rx_chain;
  uint32 analysis_id;  //  Its a 32bit bit-field which is used to select various analyses that needs to be performed on the
                       //  Rx IQ samples. Multiple analyses can be selected by activating the bit that corresponds to the 
                       //  measurement.
  uint8 num_averages;
  uint32 sample_size;
   uint32 dl_pwr;     // Reserved for future use.
} ftm_iq_capture_analyze_samples_type;

#define FTM_IQ_CAPTURE_GET_SAMPLE_HDR_SIZE 16
#define FTM_IQ_CAPTURE_ANALYZE_SAMPLE_HDR_SIZE 25
#define FTM_IQ_CAPTURE_ANALYZE_SAMPLES_MAX_PAYLOAD_SIZE 20


// List of commands used for FTM IQ capture. New commands should be added to the list.
typedef PACK(union)
{

  ftm_iq_capture_get_config_type ftm_iq_capture_get_config; // GET CONFIG command
 
  ftm_iq_capture_acquire_samples_type ftm_iq_capture_acquire_samples; // ACQUIRE command

  ftm_iq_capture_get_samples_type ftm_iq_capture_get_samples;   // GET SAMPLES command

  ftm_iq_capture_analyze_samples_type ftm_iq_capture_analyze_samples;   // ANALYZE SAMPLES command
  
}ftm_iq_capture_cmd_type;

typedef PACK(struct)
{
 diagpkt_cmd_code_type         cmd_code;
 diagpkt_subsys_id_type        subsys_id;
 diagpkt_subsys_cmd_code_type  subsys_cmd_code;
 word ftm_rf_cmd;
 ftm_iq_capture_error_code_type ftm_error_code; // Error type
 uint32 sample_offset;	//	Offset at which samples are collected
 uint32 sample_size;	// Number of samples that are to be collected
 union
 {
   int8 sample_buffer_8_bit[FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG];  
   int16 sample_buffer_16_bit[(FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)]; 
   int32 sample_buffer_32_bit[(FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/4)]; 
 }sample_buffer; // Samples that are returned, one of the above element is used based on SS data format.
}ftm_iq_capture_get_samples_rsp_pkt_type;

typedef PACK(struct)
{
 diagpkt_cmd_code_type         cmd_code;
 diagpkt_subsys_id_type        subsys_id;
 diagpkt_subsys_cmd_code_type  subsys_cmd_code;
 word ftm_rf_cmd;
 ftm_iq_capture_error_code_type ftm_error_code; // Error type
 uint16 select_rx_chain;
 uint32 analysis_id;  // Its a 32bit bit-field which is used to select various analyses that needs to be performed on the
                      // Rx IQ samples. Multiple analyses can be selected by activating the bit that corresponds to the 
                      // measurement.
 uint8 num_averages;
 uint32 sample_size;
 uint32 reserved;     // Reserved for future use.
 uint16 payload_size; // Number of bytes in the analysis_result field of the response packet.
 int8   analysis_result[FTM_IQ_CAPTURE_ANALYZE_SAMPLES_MAX_PAYLOAD_SIZE];
}ftm_iq_capture_analyze_samples_rsp_pkt_type;

/* *************************************************************/
/* ***************FTM IQ capture structures and macros*****************/
/* *************************************************************/

/* *************************************************************/
/* *********Structures for FTM RF FBRx test commands ***********/
/* *************************************************************/

typedef enum{
  FTM_FBRX_TEST_SUCCESS,
  FTM_FBRX_TEST_GENERAL_FAILURE,
  FTM_FBRX_TEST_MEM_ALLOCATION_FAILURE,
  FTM_FBRX_TEST_EXCEEDS_PAYLOAD_SIZE_LIMIT,
  FTM_FBRX_TEST_IQ_SAMPLES_EXCEEDS_CAPTURE_LIMIT,
  FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE_ON_RFM_DEV,
  FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE,
  FTM_FBRX_TEST_ILLEGAL_COMBIMATION,
  FTM_FBRX_TEST_MAX_ERROR = 0xFFFF
}ftm_fbrx_test_error_code_type;

typedef enum{
  FTM_FBRX_SAMPLE_SIZE_1024 = 0,
  FTM_FBRX_SAMPLE_SIZE_2048,
  FTM_FBRX_SAMPLE_SIZE_3072,
  FTM_FBRX_SAMPLE_SIZE_4096,
  FTM_FBRX_SAMPLE_SIZE_5120,
  FTM_FBRX_SAMPLE_SIZE_6144,
  FTM_FBRX_SAMPLE_SIZE_7168,
  FTM_FBRX_SAMPLE_SIZE_8192,
  FTM_FBRX_SAMPLE_SIZE_9216,
  FTM_FBRX_SAMPLE_SIZE_UNKNOWN
}ftm_fbrx_iq_sample_size;

typedef enum{
  FTM_FBRX_IQ_SAMPLE_TX,
  FTM_FBRX_IQ_SAMPLE_RX,
  FTM_FBRX_IQ_SAMPLE_UNKNOWN
}ftm_fbrx_iq_sample_source;

// This structure is used to extract data and update for GET CONFIG command.
typedef PACK(struct)
{
  /* Capture multiple IQ captures. The results will be averaged over the number of IQ captures.*/
  uint16 num_averages;          
  /* Delay in nano seconds after FBRx chain is configured/enabled and before capturing IQ samples.*/
  uint32 capture_offset;        
  /* Selects the parameters to be measured for the current UL config and power level.*/
  uint32 tx_measurement_config; 
}ftm_rf_fbrx_meas_tx_params_type;

// This structure is used to extract data and update for FTM_RF_MEAS_FBRX_NOISE command.
typedef PACK(struct)
{
  ftm_mode_id_type curr_tech_band; // FTM PHONE MODE
  uint8 device;  // Tx device to use
  uint8 num_captures_per_meas; // number of capture to average on
  uint8 num_lte_bw;   // Number of LTE BW Types to Cal for
  uint16 tx_chan[4]; // Channel for up to 4 possible LTE BW types
  ftm_lte_bw_type lte_bw[4];  // Up to 4 possible LTE BW Types
}ftm_rf_fbrx_meas_noise_params_type;

// This structure is used to extract data and update for FBRx IQ Capture trigger command
typedef PACK(struct)
{    
  /* Number of samples to be captured in multiple of 1024 (Max = 9216) */
  uint8 sample_size;  
  /* Specifies the path to be used as FBRx*/
  uint8 fbrx_chain;   
  /* Reserved for future use */
  uint8 reserved[5];
}ftm_rf_fbrx_iq_capture_trigger_type;

// This structure is used to extract data and update for FBRx IQ Capture get samples command
typedef PACK(struct)
{  
  /* Reserved for future use*/
  uint16 ftm_error_code; 
  /* Indicates if the samples returned through sample buffer are Tx or Rx samples.*/
  uint8 sample_source;   
  /* The offset of the first sample to return in the sample buffer*/
  uint32 sample_offset;  
  /* Number of I and Q sample pairs to be returned in the sample buffer field.*/
  uint32 sample_size;    
}ftm_rf_fbrx_iq_capture_get_samples_type;

// List of commands used for FTM RF FBRx tests. Any new commands should be added to this list.
typedef PACK(union)
{
  /* FTM Meas Tx Params  */
  ftm_rf_fbrx_meas_tx_params_type ftm_rf_fbrx_meas_tx_params;                
  /* FTM FBRx IQ capture trigger*/
  ftm_rf_fbrx_iq_capture_trigger_type ftm_rf_fbrx_iq_capture_trigger;        
  /* FTM FRRx IQ capture get samples */
  ftm_rf_fbrx_iq_capture_get_samples_type ftm_rf_fbrx_iq_capture_get_samples; 
  /* FTM FRRx Noise Cal */
  ftm_rf_fbrx_meas_noise_params_type ftm_rf_fbrx_meas_noise_params;
}ftm_rf_fbrx_test_cmd_type;

#define FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE 47
#define FTM_RF_MEAS_TX_PARAMS_HDR_SIZE 20
#define FTM_RF_MEAS_NOISE_PARAMS_PKT_SIZE 71

typedef PACK(struct)
{
  diagpkt_cmd_code_type         cmd_code;
  diagpkt_subsys_id_type        subsys_id;
  diagpkt_subsys_cmd_code_type  subsys_cmd_code;
  word                          ftm_rf_cmd;
  /* Error type*/
  ftm_fbrx_test_error_code_type ftm_error_code;        
  /* Capture multiple IQ captures. The results will be averaged over the number of IQ captures.*/
  uint16                        num_averages;          
  /* Delay in micro seconds after FBRx chain is configured/enabled and before capturing IQ samples */
  uint32                        capture_offset;        
  /* Selects the parameters to be measured for the current UL config and power level.*/
  uint32                        tx_measurement_config; 
  /* Number of bytes in the result field of the response packet.*/
  uint16                        payload_size;          
  int8                          result[FTM_RF_MEAS_TX_PARAMS_MAX_PAYLOAD_SIZE];
}ftm_rf_fbrx_meas_tx_params_rsp_pkt_type;

/* Structure for FBRx Noise power per ACLR bin */
typedef PACK(struct)
{
  uint16 utra1_neg;
  uint16 utra1_pos;
  uint16 utra2_neg;
  uint16 utra2_pos;
  uint16 eutra_neg;
  uint16 eutra_pos;
} ftm_rf_fbrx_noise_binsum_type;

/* Response Packet structure for FTM_RF_MEAS_FBRX_NOISE */
typedef PACK(struct)
{
  diagpkt_cmd_code_type         cmd_code;
  diagpkt_subsys_id_type        subsys_id;
  diagpkt_subsys_cmd_code_type  subsys_cmd_code;
  word                          ftm_rf_cmd;
  /* Error type*/
  ftm_fbrx_test_error_code_type ftm_error_code;
  /* Tech Band Combination at which Cal is performed */
  ftm_mode_id_type curr_tech_band;
  /* Tx Device for which cal is performed */
  uint8 device;
  /* Number of LTE BW Types (up to 4) for which cal was performed */
  uint8 num_lte_bw;
  /* Tx Channels (up to 4) for which cal was performed */
  uint16 tx_chan[4];
  /* LTE BW Types (up to 4) for which cal was performed */
  ftm_lte_bw_type lte_bw[4];
  /* Cal'ed noise results in binsum form per ACLR bin */
  ftm_rf_fbrx_noise_binsum_type fbrx_meas_noise_binsum[4];
} ftm_rf_fbrx_meas_noise_params_rsp_pkt_type;

typedef PACK(struct)
{
  diagpkt_cmd_code_type         cmd_code;
  diagpkt_subsys_id_type        subsys_id;
  diagpkt_subsys_cmd_code_type  subsys_cmd_code;
  word                          ftm_rf_cmd;
  /* Error type*/
  ftm_fbrx_test_error_code_type ftm_error_code;         
  /* Specifies the data format for Tx samples*/
  uint8                         sample_data_format_tx;   
  /* Specifies the data format for Rx samples*/
  uint8                         sample_data_format_rx;   
  /* Number of samples to be captured in multiple of 1024 (Max = 9216)*/
  uint8                         sample_size;             
  /* Specifies the path to be used as FBRx*/
  uint8                         fbrx_chain;              
  /* Sampling frequency in Hz that was used to capture Tx and Rx samples*/ 
  uint32                        sampling_frequency;      
  /* Max. number of IQ sample pairs that the FTM IQ capture software can use in a diagnostic response packet.*/
  uint32                        max_diag_sample_size_tx; 
  /* Max. number of IQ sample pairs that the FTM IQ capture software can use in a diagnostic response packet.*/
  uint32                        max_diag_sample_size_rx; 
}ftm_rf_fbrx_iq_capture_trigger_rsp_pkt_type;

#define FTM_RF_FBRX_IQ_CAPTURE_GET_SAMPLES_HDR_SIZE 17

typedef PACK(struct)
{
  diagpkt_cmd_code_type         cmd_code;
  diagpkt_subsys_id_type        subsys_id;
  diagpkt_subsys_cmd_code_type  subsys_cmd_code;
  word                          ftm_rf_cmd;
  /* Error type*/
  ftm_fbrx_test_error_code_type ftm_error_code;          
  /* Indicates if the samples returned through sample buffer are Tx or Rx samples.*/
  uint8                         sample_source;           
  /* The offset of the first sample to return in the sample buffer*/
  uint32                        sample_offset;           
  /* Number of I and Q sample pairs to be returned in the sample buffer field.*/
  uint32                        sample_size;             
  union
  {
    int8 sample_buffer_8_bit[FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG];  
    int16 sample_buffer_16_bit[(FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)]; 
    int32 sample_buffer_32_bit[(FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/4)]; 
  }sample_buffer; // Samples that are returned, one of the above element is used based on sample data format for Tx or Rx samples
}ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt_type;

typedef PACK(struct)
{
  uint16 state;
  uint32 multi_carrier_idx;
}ftm_set_multi_tx_type;

typedef PACK(struct)
{
    word ftm_rf_cmd;
    /*  Rf sub-command enums from host */

    PACK(union)
    {
        word                    gen_w;      /* Generic word                      */
        byte                    gen_b;      /* Generic byte                      */
        dword                   gen_d;      /* Generic dword                     */
        word                    chan;       /* For tuning to a specific channel  */
		uint8                   num_band_chan; /* For HDR it passes the number of carriers to be tunned to */
        uint16                  chan_array[RFCOM_MAX_CARRIERS];
        boolean                 on_off;     /* For anything with an on_off state */
        ftm_mode_id_type        mode;       /* For Setting Phone operation mode  */
        ftm_pa_range_type       range;      /* Set the LNA/PA range              */
        ftm_sec_chain_ctl_enum_type second_chain; /* set second chain to: Disabled, Rx Diversity, OFS */
        ftm_id_val_type         id_val;     /* For all indexed values            */
        ftm_byte_type           byte_struct;
        gsm_option_type         gsm_struct;
        ftm_mix_type            mix_struct;
        byte                    hdet_array[FTM_NUM_HDET_TO_RETURN]; /* ftm hdet block request/response type */
        uint16                  hdet_array_16_bit[FTM_NUM_HDET_TO_RETURN]; /* support 12-bit DAC */
        ftm_tx_waveform_data_type ftm_tx_waveform_data;
        ftm_ici_type            ici;
        ftm_iqm_result_type     iqm_result;
        ftm_config_eul_data_type ftm_config_eul_data;
        ftm_wcdma_set_txagc_data_type ftm_wcdma_set_txagc_data;
        ftm_wcdma_txagc_sweep_data_type ftm_wcdma_txagc_sweep_data;
        ftm_wcdma_tpc_control_data_type  ftm_wcdma_tpc_control_data;
        ftm_config_iq_log_type   ftm_config_iq_log;
        ftm_set_tx_power_dbm_data_type tx_power_dbm_data;
        ftm_get_rx_level_dbm_data_type rx_level_dbm_data;
        ftm_block_info_type ftm_block_info; /* For Calibrating the BC or Sub-block when RF_HAS_SUB_BLOCK_SUPPORT*/
        ftm_set_freq_adjust_type ftm_set_freq_adjust;
        ftm_dpd_tbls_type ftm_dpd_tbls;
        ftm_tdscdma_set_txagc_data_type ftm_tdscdma_set_txagc_data;
        ftm_tdscdma_txagc_sweep_data_type ftm_tdscdma_txagc_sweep_data;
        ftm_tdscdma_tpc_control_data_type  ftm_tdscdma_tpc_control_data;
        ftm_tdscdma_burst_tx_params_data_type ftm_tdscdma_burst_tx_params;
        ftm_tdscdma_burst_tx_on_off_data_type ftm_tdscdma_burst_tx_on_off;
        ftm_tdscdma_burst_rx_params_data_type ftm_tdscdma_burst_rx_params;
        ftm_tdscdma_multi_synth_state multi_synth;
        ftm_tdscdma_asm_update_params_data_type ftm_tdscdma_asm_update_params; 
        ftm_tdscdma_alt_path_oride_params_type  ftm_tdscdma_alt_path_oride_params;
        int16 tx_freq_adjust;
        ftm_tx_pwr_limit_type tx_pwr_limit_data;
        ftm_set_multi_tx_type multi_tx_data;
	ftm_tuner_tunecode_override_data_type ftm_tuner_tunecode_override_data;
        ftm_get_multi_synth_lock_data_type ftm_get_multi_synth_lock_rsp;
	ftm_iq_capture_cmd_type ftm_iq_capture_cmd;
        ftm_rf_fbrx_test_cmd_type ftm_rf_fbrx_test_cmd;
        int16 ftm_tdscdma_iq_cap_device;
        ftm_get_multiple_rx_level_dbm_data_type get_multiple_rx_level_dbm_data;
        ftm_rx_lin_state_type rx_lin_state;
        ftm_config_deconfig_rx_chain_type ftm_config_deconfig_rx_chain;
        ftm_read_device_status device_status;
        rf_burst_type device_status_read_burst_type;
    }  ftm_rf_factory_data;               /* union of RF commands data types   */
} ftm_rf_factory_parms;

typedef PACK(struct)
{
    diagpkt_subsys_header_type header;
    ftm_rf_factory_parms rf_params;
}  ftm_pkt_type;

typedef PACK(struct)
{
    int16 data[FTM_IQ_RECORD_BLOCK_SIZE];
    word recordIndex;
} ftm_iq_data_type;

/* FTM Composite PKT Header */
typedef PACK(struct)
{
    ftm_diagpkt_subsys_header_type  diag_hdr;
    ftm_cmd_header_type             ftm_hdr;
} ftm_composite_cmd_header_type;

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    void                         *ftm_gsm_ber_params;

} ftm_gsm_ber_pkt_type;

extern boolean ftm_gsm_ber_cleanup( rfm_device_enum_type device);
extern void ftm_gsm_ber_init( void);

typedef PACK(struct)
{
    ftm_diagpkt_subsys_header_type   diag_hdr;
    ftm_cmd_header_type              ftm_hdr;
    /* This is a start of a buffer of variable length. Using a byte as a
    ** placeholder for the buffer.
    */
    uint8                             ftm_gsm_extended_params[4];
} ftm_gsm_extended_pkt_type;

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    byte                         data[1];

} ftm_wcdma_ber_pkt_type;

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    byte                         data[1];

} ftm_tdscdma_ber_pkt_type;



extern void ftm_wcdma_ber_cleanup( void);
extern void ftm_wcdma_ber_init( void);
extern void ftm_wcdma_ber_process_msgs( void);
extern void ftm_wcdma_ber_process_int_msgs( void);

typedef PACK(struct)
{
    ftm_diagpkt_subsys_header_type   diag_hdr;
    ftm_cmd_header_type              ftm_hdr;
    void                             *ftm_extended_params;
} ftm_common_pkt_type;

typedef PACK(struct)
{
    ftm_subsys_header_type	subsys_hdr;
    ftm_cmd_header_type         ftm_hdr;
    prot_act_e_type 	        act_reason;
    prot_trans_type             trans_id;
    rex_tcb_type  	        *caller_tcb_ptr;
    rex_sigs_type		task_nv_wait_sig;	
} ftm_activate_pkt_type;

typedef PACK(struct)
{
    ftm_subsys_header_type	subsys_hdr;
    ftm_cmd_header_type         ftm_hdr;
    prot_deact_e_type 	        deact_reason;
    prot_trans_type             trans_id;
    rex_tcb_type  	        *caller_tcb_ptr;
    rex_sigs_type		task_nv_wait_sig;	
} ftm_deactivate_pkt_type;



typedef struct {
    unsigned int hdet_accum;        // 16Q8
    unsigned int hdet_set_point;    // 8Q0
    unsigned int hdet_tracking_pdm; // 32Q23
} hdet_tracking_params_type;

typedef PACK(struct)
{
    ftm_diagpkt_subsys_header_type   diag_hdr;
    ftm_cmd_header_type              ftm_hdr;
    /* The ftm meas parameters should be serving cell, serving channel, meas cell and meas channel */
    /* For get size, the ftm meas parameters is serv mode, meas mode from rfm_mode_enum_type*/
    uint16                             ftm_meas_extended_params[20];
} ftm_meas_extended_pkt_type;

typedef PACK(struct)
{
    uint8  notch_index;   /* Index into the notch buffer */
    uint32 notch_freq_hz; /* Notch freq in Hz */
    uint8  notch_depth;   /* Notch Depth */
    uint8  device;        /* RF device (should map to RFCOM definitions) */
} ftm_set_msm_notch_filter_data_s;

#ifdef FEATURE_LTE
/*! @brief This following structure is used as
 *         the payload of a LTE NS CMD packet from
 *         Diag.
 *
 *  @param cmd_id, the specific LTE NS command
 */
typedef PACK(struct)
{
    uint16 cmd_id;
    union {
        /*LTE Calibration*/
        /* This is the payload information for FTM_SET_MODE*/
        uint16 mode;
        /* This is the payload information for FTM_SET_CHAN*/
        rfcom_lte_earfcn_type tx_chan;
        /* This is the payload information for FTM_SET_PA_RANGE*/
        uint8 paRange;
        /* This is the payload information for FTM_SET_TX_GAIN_INDEX*/
        uint16 gain_index_id;
        /* This is the payload information for FTM_SET_LNA_RANGE*/
        uint8 lna_state;
        /* This is the payload information for FTM_SET_TX_BW*/
        uint8 tx_bw;
        /* This is the payload information for FTM_SET_RX_BW*/
        uint8 rx_bw;
        /* This is the payload information for FTM_GET_HDET_FROM_TX_SWEEP_CAL*/
        uint16 hdet_index;
        /*This is the payload information for FTM_GET_ADC_VAL*/
        uint16 adc_chan;
        /* This is the payload information for FTM_SET_LTE_WAVEFORM*/
        rfa_rf_lte_ftm_set_tx_waveform_data_s waveform_config;
        /* This is the payload information for FTM_LTE_DO_RX_IQ_MISMATCH_CAL*/
        rfa_rf_lte_ftm_set_rx_iq_mismatch_coeft_data_s rx_iq_mismatch_data;
        /* This is the payload information for FTM_CONFIGURE_TX_SWEEP_CAL*/
        rfa_rf_lte_ftm_configure_tx_sweep_data_s config_tx_sweep;
        /* this is the payload inforamtion for FTM_GET_LNA_OFFSET */
        rfa_rf_lte_ftm_get_lna_offset_data_s     get_lna_offset;
        /* this is the payload inforamtion for FTM_SET_SECONDARY_CHAIN */
        uint16     sec_flag;
        /* this is the payload inforamtion for FTM_LTE_IM2 */
        uint16     band;
        /* this the the payload information for enable LPM HDET*/
        uint8      enable_hdet_lpm;
        /* this is the payload information for the txagc output power override*/
        int32  oride_txagc_output_power_level;
        /* this is the payload information for the txagc digital backoff override*/
        int32  oride_txagc_backoff;
        /* this is the payload information for the smps pdm override value*/
        int16  oride_smps_pdm_val_payload;
        /* this is the payload information for the smps pdm override enable*/
        boolean oride_smps_pdm_enable_payload;
        /* This is the payload information for FTM_LTE_DAC_SETPOINT*/
        rfa_rf_lte_ftm_dac_setpoint_cmd_data_s rflte_ftm_dac_setpoint_data;
        /*LTE NS*/
        /* This is the payload information for FTM_LTE_NS_ACQ_REQ*/
        ftm_lte_ns_acq_req_data_type acq_req_data;
        /* This is the payload information for FTM_LTE_NS_START_DP_REQ*/
        ftm_lte_ns_dp_req_data_type dp_req_data;
        /* This is the payload information for FTM_LTE_NS_CONFIG_UL_WAVEFORM*/
        ftm_lte_ns_config_ul_waveform_req_data_type ul_waveform_req_data;
        /* This is the payload information for FTM_LTE_NS_CONFIG_UL_POWER*/
        ftm_lte_ns_config_ul_power_req_data_type ul_power_req_data;
        /* This is the payload information for FTM_LTE_NS_GET_DL_LEVEL*/
        ftm_lte_ns_get_dl_level_req_data_type dl_level_req_data;
        /* This is the payload information for FTM_LTE_NS_HANDOVER*/
        ftm_lte_ns_handover_req_data_type handover_req_data;
        /* This is the payload information for FTM_LTE_NS_ENABLE_SCELL*/
        ftm_lte_ns_enable_scell_req_data_type enable_scell_req_data;
        /* This is the payload information for FTM_LTE_NS_ENABLE_MULTI_SCELL*/
        ftm_lte_ns_enable_multi_scell_req_data_type enable_multi_scell_req_data;		
        /* This is the payload information for FTM_LTE_NS_DISABLE_MULTI_SCELL*/
        ftm_lte_ns_disable_multi_scell_req_data_type disable_multi_scell_req_data;				
        /* This is the payload informatino for FTM_SET_TX_POWER_DBM*/
        rfa_rf_lte_ftm_set_tx_power_dbm_cmd_data_s ftm_lte_set_tx_power_dbm_data;
        rfa_rf_lte_ftm_set_net_sig_value_cmd_data_s ftm_lte_set_net_sig_value_data;
        /* This is the payload informatino for FTM_SET_PA_STATE*/
        uint8 pa_state;
        /* This is the payload information for FTM_LTE_CHAN_MODULATION*/
        rfa_rf_lte_ftm_ul_modulation_type rflte_ftm_chan_modulation;
        /* This is the payload information for RFA_RF_LTE_FTM_MSG_ID_GET_RX_AGC_DATA*/
        uint8 requested_chain_bit_mask;
        /*This is the payload information for FTM_SET_LNA_STATE_DBM*/
        int16 expected_rx_pwr;
        /* This is the payload information for FTM_LTE_NS_TDD_CONFIG*/
        ftm_lte_ns_tdd_config_req_data_type tdd_config_req_data;
        /*This is the payload information for FTM_LTE_SET_FREQ_ADJUST*/
        rfa_rf_lte_ftm_set_freq_adjust_cmd_data_s rfa_rf_lte_ftm_set_freq_adjust_data;
        /*This is the payload information for FTM_LTE_SET_FREQ_ADJUST*/
        int32 tx_freq_adj;
        /* This is the payload for FTM_LTE_SET_CARRIER_ID */
        uint8 carrier_idx;
        /* This is the payload for FTM_SET_ASM_PORT */
        rfa_rf_lte_asm_update_params_s rfa_rf_lte_asm_update_params_data; 
        /* This is the payload commands associated with FBRx testing. */
        ftm_rf_fbrx_test_cmd_type ftm_rf_fbrx_test_cmd;
        /*This is the payload information for FTM_LTE_USE_FIXED_VECTOR for Tx */
        boolean use_fixed_encoder_input_ul; 
        /* This is the payload for FTM_GET_MULTIPLE_RX_LEVEL_DBM */
        ftm_get_multiple_rx_level_dbm_data_type get_multiple_rx_level_dbm_data;
        /* This is the payload for FTM_LTE_DISABLE_SCELL */
        uint32 carrier_idx_to_disable;
        /* This is the payload for FTM_SET_MSM_NOTCH_FILTER */
        ftm_set_msm_notch_filter_data_s notch_params;
        /* This is the payload for FTM_APPLY_MSM_NOTCH_FILTER_SETTINGS
         * and FTM_RESET_MSM_NOTCH_FILTER */
        uint8 device_to_apply_notch_settings;

        /* This is the payload for FTM_SET_PATH_SEL_OVERRIDE_INDEX */
        rfa_rf_lte_path_sel_override_params_s rfa_rf_lte_path_sel_override_params_data;   
        
        /* Payload for Config Rx chain*/
        rfa_rf_lte_config_rx_chain_params_s ftm_config_rx_chain_input_params;
        /* Payload for FTM_LTE_CONFIG_DECONFIG_RX_CHAIN*/
        ftm_config_deconfig_rx_chain_type ftm_config_deconfig_rx_chain;
        rfa_rf_lte_ftm_alt_path_oride_params_s rfa_rf_lte_alt_path_override_params_data;
    };
} ftm_lte_payload_type;


typedef PACK(struct)
{
    uint16 cmd_id;
    uint8 version_num;
    uint16     band;
    uint8 tx_bw;
    uint8 rx_bw;
    uint16 tx_chan;
    rfa_rf_lte_ftm_set_tx_waveform_data_s waveform_config;
    rfa_rf_lte_ftm_ul_modulation_type rflte_ftm_chan_modulation;
    rfa_rf_lte_ftm_set_net_sig_value_cmd_data_s ftm_lte_set_net_sig_value_data;
    uint8 reserved_1;
    uint8 reserved_2;
} ftm_lte_config_payload_type;

typedef PACK(struct)
{
    ftm_diagpkt_subsys_header_type  subsys_hdr;
    ftm_lte_config_payload_type payload;
} ftm_lte_config_pkt_type;

/*! @brief This following structure is used
 *         deliever a LTE FTM CMD packet from
 *         Diag.
 *
 *  @detail
 *  The payload packet will include the ftm_diagpkt_subsys_header_type, the ftm_cmd_header_type
 *  and a void pointer to the payload.
 *
 *  @param subsys_hdr, contains the subsystem ID and the subsystem command ID
 *                     In this case it subsystem will be FTM and subsystem command
 *                     is either FTM_LTE_C or FTM_LTE_NS_C
 *  @param cmd_id, the specific LTE NS command
 */
typedef PACK(struct)
{
    ftm_diagpkt_subsys_header_type  subsys_hdr;
    ftm_lte_payload_type            payload;
} ftm_lte_pkt_type;

#endif /*FEATURE_LTE*/

typedef PACK(struct)
{
  uint8               remote_tx_on;
  uint16              remote_rf_mode;
  int16               remote_tx_power;
  uint8               remote_curr_band;
  uint16              remote_rf_chan;
}ftm_sar_power_limit_control_type;

typedef PACK(struct)
{
    ftm_diagpkt_subsys_header_type      diag_hdr;
    ftm_cmd_header_type                 ftm_hdr;
    ftm_sar_power_limit_control_type    ftm_sv_payload;
} ftm_sv_pkt_type;

typedef PACK(struct)
{
  uint8                                 device;
  uint8                                 device_fb;
  uint8                                 xpt_capture_type;
  uint16                                num_samples;
  uint8                                 first_trig;
  uint8                                 last_trig;
  uint8                                 do_processing;
  uint8                                 do_leakage_cal;
  uint8  debug_state;
  uint8  iq_file_name_len;
  uint8  dpd_file_name_len;
  uint8  payload_start[];
}ftm_iq_capture_type;

typedef PACK(struct)
{
  uint8                                 device;
  uint8                                 device_fb;
  uint16                                num_samples;
  uint8                                 first_trig;
  uint8                                 last_trig;
  uint8  debug_state;
  uint8  iq_file_name_len;
  uint8  results_file_name_len;
  uint8  payload_start[];
}ftm_iq_xpt_delay_capture_type;


typedef PACK(struct)
{
  uint8                                 device;
  uint8                                 device_fb;
  uint16                                num_samples;
  uint8                                 first_trig;
  uint8                                 last_trig;
  uint8  debug_state;
  uint8  target_compression;
  uint8  iq_file_name_len;
  uint8  payload_start[];
}ftm_iq_xpt_pin_capture_type;


typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
} ftm_iq_capture_rsp_pkt_type;

typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
  int32   et_path_delay;
} ftm_iq_xpt_delay_capture_rsp_pkt_type;

typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
  uint8   target_compression_index;
  uint8   max_compression;
} ftm_iq_xpt_pin_capture_rsp_pkt_type;

typedef PACK(struct)
{
  uint8                                 device;
  uint8   xpt_type;
  uint8   action_type;
  uint16  num_amam;
  uint16  num_ampm;
  uint16  num_epdt;
  uint16  dpd_payload[];
}ftm_dpd_override_type;

typedef PACK(struct)
{
  uint8   primary_device;
  uint8   feedback_device;
  uint8   xpt_mode;
} ftm_xpt_config_payload_type;

typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
} ftm_xpt_config_rsp_pkt_type;

typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
  uint8   amam_bank_idx;
  uint16  amam_bank_offset;
  uint8   ampm_bank_idx;
  uint16  ampm_bank_offset;
  uint8   epdt_bank_idx;
  uint16  epdt_bank_offset;
}ftm_dpd_override_rsp_pkt_type;

typedef PACK(struct)
{
  uint8                                 device;
  int8   xpt_mode;
  uint8  iq_gain_action_type;
  int32  iq_gain;
  uint8  env_scale_action_type;
  int32  env_scale; 
  int16  rgi;
  int16  smps_bias;
  int8   pa_state;
  int16  pa_curr;
  uint8  delay_action_type;
  int32  delay;
} ftm_xpt_txagc_override_type;

typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
} ftm_xpt_txagc_override_rsp_type;

typedef PACK(struct)
{
  ftm_mode_id_type curr_band;
  boolean override_stg; 
  uint8 device;
  uint8 div_device;
  boolean exit_required;
} ftm_rsb_txlo_pkt_type; 

typedef PACK(struct)
{
  ftm_mode_id_type curr_band;
  boolean override_stg; 
  uint8 tx_device;
  boolean exit_required;
} ftm_fbrx_rsb_cal_pkt_type;

typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
  ftm_mode_id_type curr_band;
  ftm_internal_cal_status_type cal_status; 
} ftm_rsb_txlo_rsp_type; 

/*===========================================================================

                     EXTERNALIZED DATA DECLARATIONS
===========================================================================*/



/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION FTM_DEACTIVATE_PROTOCOL

DESCRIPTION
    The function is responsible for release any resource used by FTM mode.
 (for example, deregister isr handler from gstmr_handlers[],etc). It will be called by the task that needs 
  to de-activate the FTM mode 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_deactivate_protocol( rex_tcb_type * caller_tcb_ptr,
                              rex_sigs_type task_nv_wait_sig,
                              void(* task_wait_func_ptr)(rex_sigs_type),
                              prot_deact_e_type deact_reason,
                              prot_trans_type trans_id );

/*===========================================================================

FUNCTION FTM_DEACTIVATE_PROTOCOL_CMD

DESCRIPTION
    The function processes the FTM_deactivate CMD and sets the MMOC deact_done signal

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_deactivate_protocol_cmd( rex_tcb_type *caller_tcb_ptr, 
                                  rex_sigs_type task_nv_wait_sig, 
                                  prot_deact_e_type deact_reason, 
                                  prot_trans_type trans_id);



/*===========================================================================

FUNCTION FTM_ACTIVATE_PROTOCOL

DESCRIPTION
   The function is responsible for getting MB ready to FTM mode.
   It will be called by the task that needs to activate the FTM mode 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_activate_protocol( rex_tcb_type * caller_tcb_ptr,
                            rex_sigs_type task_nv_wait_sig,
                            void(* task_wait_func_ptr)(rex_sigs_type),
                            prot_act_e_type act_reason,
                            prot_trans_type trans_id );

/*===========================================================================

FUNCTION FTM_ACTIVATE_PROTOCOL_CMD

DESCRIPTION
    The function processes the FTM_activate CMD and sets the MMOC act_done signal

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_activate_protocol_cmd( rex_tcb_type *caller_tcb_ptr, 
                                rex_sigs_type task_nv_wait_sig, 
                                prot_act_e_type act_reason, 
                                prot_trans_type trans_id );

/*===========================================================================

FUNCTION FTM_ACTIVATE_PROTOCOL_OFFTARGET_TEST

DESCRIPTION
   The function is responsible for getting MB ready to FTM mode.
   It will be called only during off target testing to activate 
   the FTM mode 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_activate_protocol_offtarget_test
(
  prot_act_e_type act_reason
);


/*===========================================================================

FUNCTION FTM_CMD

DESCRIPTION
   This function is used to send command to the ftm task.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_cmd(ftm_msg_type *msg_ptr);

/*===========================================================================

FUNCTION FTM_EVENT

DESCRIPTION
   This function is used to send events to the ftm task.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_event(ftm_event_type *evt_ptr);


/*===========================================================================

FUNCTION FTM_GET_MODE

DESCRIPTION


DEPENDENCIES
     None.

RETURN VALUE
     None.

SIDE EFFECTS
     None.

===========================================================================*/
ftm_mode_type  ftm_get_mode( void);


/*===========================================================================

FUNCTION FTM_SET_RF_SB_STATE

DESCRIPTION


DEPENDENCIES
     None.

RETURN VALUE
     None.

SIDE EFFECTS
     None.

===========================================================================*/
void ftm_set_rf_db_state( ftm_rf_mode_type  mode );

/*===========================================================================

FUNCTION FTM_GET_RF_TECHNOLOGY

DESCRIPTION Returms the current technology (1x, GSM, WCDMA) in which the phone is operating


DEPENDENCIES
     None.

RETURN VALUE
     None.

SIDE EFFECTS
     None.

===========================================================================*/
ftm_rf_technology_type ftm_get_rf_technology( void );
/*===========================================================================

FUNCTION FTM_GET_RF_DB_STATE

DESCRIPTION


DEPENDENCIES
     None.

RETURN VALUE
     None.

SIDE EFFECTS
     None.

===========================================================================*/
ftm_rf_mode_type  ftm_get_rf_db_state( void );


/*===========================================================================

FUNCTION FTM_CURRENT_MODE_IS_RF_CAL

DESCRIPTION
   Determines if current ftm Mode is RF Cal Mode or not.

DEPENDENCIES
   None.

RETURN VALUE
   returns TRUE if FTM Mode is RF Cal Mode and FALSE Otherwise

SIDE EFFECTS
   None

===========================================================================*/
boolean ftm_current_mode_is_rf_cal(void);

/*===========================================================================
FUNCTION   ftm_enable_chip_reset

DESCRIPTION
  It enables RF chip reset when the UE does a mode change from ftm to online.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void ftm_enable_chip_reset(boolean enable);

/*===========================================================================

FUNCTION FTM_DB_TO_RF_MODE

DESCRIPTION
  This function converts the ftm_rf_mode_type to ftm_mode_id_type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
ftm_mode_id_type ftm_db_to_rf_mode( ftm_rf_mode_type rfmode );

/*===========================================================================

FUNCTION FTM_DB_TO_RF_MODE

DESCRIPTION
  This function converts the ftm_rf_mode_type to ftm_mode_id_type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
ftm_rf_technology_type ftm_phone_mode_to_rf_tech_type 
( 
  ftm_mode_id_type phone_mode 
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_H */
