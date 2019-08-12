#ifndef FTM_RF_CMD_H
#define FTM_RF_CMD_H
/*===========================================================================

           F T M  R F  C M D    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the FTM Services module.

REFERENCES
  None

Copyright (c) 2003 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_rf_cmd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/13   aro     Migrated FTM common commands
06/29/13   hm     Add new FTM command for tuner self cal: "FTM_DO_TUNER_SELF_CAL"
07/02/13   zg      Added TDSCDMA_TX_SELF_CAL commands.
06/19/13   kb      [XPT] Added FTM Tx waveform switch command.
06/07/13   aca     FTM Tuner RL cmd
05/20/13   jmf     Add xPT IQ Capture ET Path Delay FTM cmd
05/01/13   aki     Added FTM_RX_WAIT_DBM_V2
04/22/13   ck      add ftm id type to support GSM IQ capture
04/17/13   aka     added predist_override cmd
03/28/13   ems     Added support for NV items larger than 4K  
03/28/13   aka     GSM frame sweep feature changes.
03/21/13   yzw     Add new FTM cmd FTM_GET_RFFE_DEVICE_INFO to query RFFE device information {mfg_id, prd_id, rev_id}
03/13/13   aca     Tuner characterization
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch
03/04/13   aro     Command to perform XPT cal config
02/26/13   sr      Added new FTM_DO_GPIO_CMD cmd to control GPIO and GRFC.
02/26/13   tks     Added support for FTM_DO_RSB_TXLO_CAL 
02/15/13   tws     Back out previous change.
02/17/13   tws     New command added for FTM_ACTIVATE and FTM_DEACTIVATE_MODE
02/04/13   ndb/bmg Added FTM_SET_TUNER_TUNE_CODE_OVERRIDE support 
01/30/13   hdz     Added FTM_XPT_CONFIG
12/12/12   aka     Moved FTM_GSM_GET_ENV_GAIN_NV_DATA to PC cmds section
11/28/12   nrk     Added commands to get and reset the mutli synth state
11/19/12   aka     Add FTM_GSM_GET_ENV_GAIN_NV_DATA cmd to return NV value
11/15/12   ka      Initial support for DCHSUPA
11/14/12   dw      Add FTM cmd ID to set PA quiescent current
11/15/12   dj      Added FTM_XO_CALIBRATION
11/08/12   cd      Added common dispath for FTM_SET_XPT_TXAGC_OVERRIDE
11/07/12   aro     Migrated Radio Setup operation to FTM Task
11/06/12   kai     Moved all FTM commands before FTM_DIAG_CMD_ID_MAX
10/16/12   ka      Added FTM_MEAS_WRITE_WTOW
09/21/12   kai     Added FTM_PROGRAM_QFE_SCENARIO
09/14/12   jfc     Add FTM_DO_RFFE_CMD 
07/24/12   hdz     Added FTM_SET_TX_PWR_LIMIT
07/21/12   pl      Remove FTM_LTE_SET_CARRIER_ID
07/11/12   jps     Added FTM RF mode commands for TD-SCDMA 
07/10/12   kai     Add support for Qtuner FTM command
07/06/12   pl      Added FTM command for setting LTE Carrier index for CA support
05/22/12   aca     Added FTM command to extract RX AGC data
05/16/12   sa      Added FTM_GET_ENH_XO_THERM Command Support
05/09/12   aki     Mainlined FTM_RX_WAIT_DBM
05/03/12   id      Added command for Tx Internal Device Cal
04/10/12   id      Additional Tx Internal Device Cal support
04/05/12   vb      Added command FTM_DO_DAC_CAL to perform DAC cal only.
03/22/12   id      Added Tx Internal Device Cal support
03/16/12   php     Added LTE NS support
03/07/12   cri     Added initial DPD support
03/06/12   kai     Added API for setting/getting Tx datapath IQ gain
02/28/12   sb      Added changes to support disabling Idle frame in FTM mode
02/29/12   yzw    Added LTE FTM command FTM_LTE_SET_LNA_STATE_DBM
02/16/12   pv/ars  Added FTM Err Fatal Command to crash the phone when received.
02/14/12   swb     Added API for FTM Tx Frequency Adjustment
01/26/12   ems     Added command for LTE exit mode to be run outside of ISR context
01/16/12   aro     Added generic command to query THERM
11/29/11   aki     Added FTM_RX_WAIT_DBM
11/29/11   sb      Added functionality for GSM IQ CAPTURE
11/21/11  tnt      Support for Ant Tuner Overwrite cmd thru FTM
11/17/11   sb      Added FTM function to update frequency error
11/17/11   hy      Added TDSCDMA V2/V3 Cal
11/17/11   av      Added FTM_GSM_SETUP_RX_BURST_CALVER and FTM_GSM_RSSI_RESULT_CALVER.
10/17/11   ars     Added FTM_RF_GET_RX_LEVEL_DBM command
09/25/11    vb     Added command for all the self calibration routines of
                   RF Devices & MSM
09/14/11   can     Merge MDM9K tips to NikeL.
08/19/11    gh     Added command for CalV3 for WCDMA
08/03/11   aro     Added support for FTM command to set OL Tx Power
07/11/11   hy      Including TD-SCDMA support.
07/07/11   vrb     Ported in KV Cal V2 command ID 254
07/06/11   aro     Changed the command ID numebr for Cal Sweep
06/25/11   aro     Added support to fetch CalV3 results
06/13/11   aro     Added internal command to Set Mode, Tx On, and Set
                   Lowest Tx Pwr
05/16/11   vb      Added commands for XO Cal V2
05/06/11   aro     Renamed linearizer sweep command to calibration Sweep
02/21/11   ggs     Enable ENV_DC calibration
04/06/11   aro     Added command for Tx Linearizer Calibration
03/29/11   aro     Added commands to disable Tx and LogCommit for CompCal under
                   FTM Task Context
03/28/11   aro     Added FTM_INT_SET_CHAN command
03/14/11   pl      Adding Sleep and Wakeup Command for LTE FTM
02/09/11   aca     FTM dac setpoint value override support
02/08/11   aca     FTM modulation change support
02/07/11   av      Corrected the FTM IRAT api cmd numbers. 3G-2G api number were reserved long time ago.
01/10/11   av      Added commands for PGA backoff and restore
12/15/10   ems     WCDMA Non-signaling Power Control Override
11/26/10   ggs     Added command for enhanced EDGE predistortion calibration
11/02/10   jyu     Added FTM_MEAS_XTOY_TEST_Y and FTM_MEAS_XTOY_TEST_X ID back
10/30/10   mkv     Added support for LTE Get Rx Level
10/29/10   mkv     Added support for LTE Reset and Get Sync BLER API
10/26/10   mkv     Added LTE NS Tx AGC override
10/24/10   mkv     Added LTE NS UL Grant reconfiguration
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1
08/28/10   whc     Added txagc oride ftm_id's
07/27/10   pl      Added support for L2DO test
07/14/10   kma     Resolved command ID conflict
07/01/10   aki     Merging RFNV FTM changes from dev/rfnv_dev
07/01/10   mkv     Adding support LTE LPM HDET Mode.
06/28/10   kma     Added SV FTM command for SV testing
06/10/10   kguo    Added support for FTM_MEAS_LTOW_TEST_WCDMA
06/04/10   pl      Adding x->L IRAT FTM command
06/07/10   pl      Added FTM support for LTE IM2 CAL
05/06/10   pl      Added support for FTM_LTE_NS_IS_CONN
04/14/10   adk     Merged branch scmm_mdm9k_merge_final
04/05/10   rsr     Added support for multislot RX.
03/16/10   pl      Merged FTM_LTE_NS_START_DL and FTM_LTE_NS_START_UL to become FTM_LTE_NS_START_DP
03/04/10   ad      Added support for dynamic IQ capture in INO cont. for WCDMA
01/19/10   ka      Added support for multi-carrier ICI calibration.
12/01/09   ckl     Added command ID for W2W test.
11/22/09   mkv     Removed PA ON command.
11/11/09   ka      Added command IDs for Dual Carrier.
10/24/09   id      Addded FTM_DO_ENH_XO_FT_COLLECT_SAMPLE
10/21/09   mkv     Added TX/RX IQ-mismatch commands
10/15/09   hp      Added FTM GNSS Extended Command ID, FTM_GNSS_EXTERNAL_LNA
10/09/09   pl      Adding FTM LTE Non-signaling command codes
10/07/09   ra      Enable hdet Tx Sweep version 2
10/06/09   jps     Added FTM cmd IDs for IRAT
10/06/09   mkv/pl  Added new LTE commands
09/29/09   bmg     Added FTM_SET_PA_STATE and FTM_SET_PA_SMPS_LEVEL commands
09/22/09   rr/bmg  Added FTM_PA_SMPS command
09/13/09   vb      Added commands for ICI calibration
09/14/09   sr      Mainlined code
09/09/09   aro     Changed Internal Command Name
                   + Removed DateTime and Author + Extra Space Removal
08/11/09   mkv     Merge MODEM_LTE_DEV into rfa2
07/09/09   aro     Added Internal Commands to Trigger TxSweep and Composite Cal.
06/15/09   pl      merging with SCMM
05/12/09   mkv     Adding LTE RF Calibration command definitions
02/09/09   anr     Added commands for enhanced XO calibration.
09/30/08   aak/vm  Added Support for Band Class Sub Block Implementation
04/16/08   jfc     Fix missing comment terminator
04/07/08   bmg     Added FTM_SELECT_RX_ANTENNA
04/12/08    kg     Reserving commands for LCU
04/01/08    kg     Reserving FTM_GET_AGC_RESOLUTION_TX and FTM_GET_AGC_RESOLUTION_RX for LCU
12/14/07    ch     Added FTM_GET_THERM
12/06/07   adk     Updated the value of enum FTM_MULTIMODE_HANDOVER
11/16/07   adk     Merged changes from FTM tips.
11/09/07   anr     Added FTM_SET_CALIBRATION_STATE
11/08/07   vm      Added support for FTM API supporting CDMA Waveform,
                   CW Waveform and Offset CW Waveform for Tx
10/26/07   rsr     Added FTM_SET_SMPS_PDM_TBL to set SMPS pdm table
06/13/07   jfc     Added FTM_TX_CS_SWEEP, FTM_TX_4xFMOD_SWEEP & FTM_GSM_DETECTOR_CAL
05/16/07   anr     Added FTM_DO_XO_DC_CAL and FTM_DO_XO_FT_CURVE_CAL
05/09/07   lcl     Added FTM_TX_CS_SWEEP
12/08/06   rsr     Added FTM_GSM_DETECTOR_CAL
10/27/06   jfc     Added FTM_MULTIMODE_HANDOVER for 7600
04/07/06   lcl     Added FTM_DO_DC_CAL for WCDMA interband.
03/28/06   rsr     Added FTM_RX_GAIN_RANGE_CAL
03/15/06   rsr     Added FTM_TX_RSB_DC_SWEEP
02/07/06    ka     Added FTM_DO_INTERNAL_DEVICE_CAL.
12/22/05    ka     Added FTM_SET_TX_SWEEP_STEP_SIZE.
11/17/05    ka     Added FTM_LOAD_RF_NV.
09/12/05   hkk     Changes for TX/RX sweep cal - Added id 130
08/25/05   dbc     Removed MFLO commands. They are now in ftm_mf_dispatch.h.
08/23/05   jfb     Changed cmd's to reflect current MFLO FTM API document
08/09/05   dbc     Added FTM_MF_AGC_ENABLE and FTM_MF_AGC_DISABLE
07/09/05   dbc     Added some more commands for MFLO
06/22/05   dbc     Added some more commands for MFLO
05/02/05   dbc     Added commands for MFLO.
03/20/05   bmg     Added FTM_SET_RF_POWER_MODE API.
03/11/05   ka      Added FTM_GSM_POLAR_TX_SWEEP_CAL.
03/04/05   rsr     Added support for OPLL transmit and RTR6250 RSB calibration
10/05/04   ka      Added support for FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL.
09/07/04   lcl     Added get IM2 with suppression subcommand.
08/19/04   ra      Merged UMTS with 1x targets. Removed Amps support
08/20/04    ka     Added command to configure tx sweep cal.
04/15/03    xw     Initial revision

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "target.h"              /* Target specific definitions            */
#include "comdef.h"              /* Definitions for byte, word, etc.       */
#include "ftm_command_id.h"

/* <EJECT> */
/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


#endif /* FTM_RF_CMD_H */
