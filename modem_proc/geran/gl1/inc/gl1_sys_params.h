#ifndef SYS_PARAMS_H
#define SYS_PARAMS_H
/*===========================================================================

                G S M   L 1   S Y S T E M   P A R A M E T E R S

DESCRIPTION
   This header file contain the GSM L1 specification parameters
   contained in GSM04.08 or 11.10.

REFERENCES

Copyright (c) 2002     -2013 Qualcomm Technologies, Inc.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gl1_sys_params.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/14   pjr     CR752564 Merge missing changes for CR711506 (FR19667)
17/12/13   ab      CR598397 DSDA:Protect G Monitors getting desence when W in connected mode.
14/10/13   pa      CR539505:Bolt: Added support for Dynamic Power Scheduling
04/10/13   nm      CR552803 tighten power scan threshold from -107dBm to -102dBm for background power scan only
29/07/13   sk      CR519663 Partial QBTA bringup changes
26/11/12   zf      CR417719: Optimization of Ncell acquisition
03/01/13   jj      CR415161  Modify the ghost cell check to discard a channel that  
                             cant possibly be the spillover from an adjacent GSM channel
19/11/12   ip/zf   CR341843: Opt16: Re-use SCH timing for re-acquiring Ncell.
16/08/12   zf      CR390228: Featurizing Opt19.
03/07/12   zf      CR341842: Opt19: Increase power monitor number to 5 per PCH frame.
21/01/10   pg      Revert GENESIS 9K modem back to do 7 power scans per frame 
14/12/09   ws      For GENESIS 9K modem only do 1 power scan per frame
24/07/09   ws      CR 180476 - Improve performance of Dedicated AGC
03/07/09   ip      Remove Poseidon changes from bring up
24/06/09   ws      CR 184611 Changed featurisation for UOOS to allow 7 power mons
                   per frame when UOOS is disabled
03/06/09   ip      SCMM bring up changes for Posiedon
04/01/08   og      Reduce the number of measurements per carrier from three to
                   two (reselection and power up) when FEATURE_GSM_L1_OOS_SEARCH_TIME_ENH
                   is enabled.
13/08/07   ws      Introduced FEATURE_GSM_L1_OOS_SEARCH_TIME_ENH to featurise
                   7 power monitor's per frame.
03/08/07   agv     CR123023 - Combined fix for AFC/TT/AGC. Changes slew from
                   +/- 24 dB per 52 frames to +/- 48 dB per 52 frames.
19/07/07   og      UOOS changes, reduce number of measured samples.CR119147.
02/09/05   bm      Optimized values for power scan
02/05/05   bm      Changed SYS_PARAM_MAX_FRAMES_FOR_FCCH_DETECTION from 23 to 12
06/07/04   gw      Changed SYS_PARAM_NUM_MEASUREMENTS_PER_CARRIER_RESELECTION
                   from 1 to 3.
06/02/04   gw      Added SYS_PARAM_NUM_MEASUREMENTS_PER_CARRIER_RESELECTION
                   and removed some obsolete parameters.
05/20/04   gfr     Mainlined FEATURE_COMMON_PWR_HANDLER
01/12/04   gfr     Support for 6 monitors per frame.
11/07/03   gw      Added SYS_PARAM_Tav_TICKS_MIN to control minimum power scan
                   duration.  Also increased (from 3 to 4)
                   SYS_PARAM_NUM_MEASUREMENTS_PER_FRAME_PSCAN.
08/15/03   gw      Changed slew rate limits from 7dB per 52 frames to 24dB per
                   52 frames.
07/27/03   bk      Changed SYS_PARAM_ADJACENT_CELL_PWR_RX_LVL_DELTA_dB_x16 from
                   -16 to 13 dB since power scans use HW instead of FW filter.
06/12/03   gw      Increased number of monitors per frame in Idle Mode from
                   2 to 4 to support early paging decode.  Added more constants
                   for SCE.
03/13/03   gw      Changed SYS_PARAM_MIN_ACCEPTABLE_RX_LVL_dBm_x16 from
                   -110 dBm to -107 dBm.
12/09/02   mk      Changed SYS_PARAM_ADJACENT_CELL_PWR_RX_LVL_DELTA_dB_x16
                   from -13 to -16dB.
10/15/02   JC      Modified several parameters to accomodate multi-band
                   power scan (EGSM/DCS).
10/02/02   dp      Added SYS_PARAM_ADJACENT_CELL_PWR_RX_LVL_DELTA_dB_x16
09/04/02   plm     Fixed another bad comment delimiter to fix compiler warning.
08/23/02   plm     Fixed bad comment delimiter to fix compiler warning.
08/13/02   dp      Changed acq parameters for parallel decodes
07/25/02   dp      Added SYS_PARAM_MAX_NCELL_RX_LVL_DROP_dB_x16
04/29/02   JC      Changed SYS_PARAM_NUM_ATTEMPTS_PER_MEASURE_PERIOD from
                   1 (debugging val) to 4 (for 4 gain ranges of radio). Also,
                   changed SYS_PARAM_MIN_ACCEPTABLE_RX_LVL_dBm_x16 from
                   -120 (rxlev testing) to -110 dBm per systems.
04/23/02   JC      Changed SYS_PARAM_Tav_TICKS to 650 (3 secs) for PGSM.
04/21/02   dp      Added SYS_PARAM_MIN_ACCEPTABLE_NCELL_RX_LVL_dBm_x16
04/17/02   gw      Changed SYS_PARAM_NUM_MEASUREMENTS_PER_FRAME_PSCAN from
                   1 to 3 and changed
                   SYS_PARAM_NUM_MEASUREMENTS_PER_FRAME_NCELL_PWR_MON from
                   1 to 2.
03/27/02   JC      Added some NCELL system parameters.
02/13/02   JC      Initial version

===========================================================================*/

#include "geran_variation.h"
#include "comdef.h"

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_Tav_TICKS_MIN                                  */
/* Description:     See GSM 5.08 sece                                        */
/*                  Minimum duration of power scan. Depending on the number  */
/*                  of ARFCNs to measure the power scan may take longer      */
/*                  than this.  The units are GSM frames.                    */
/* Suggested Value: 650 (=> 3s min                                           */
/* Notes:           1000ms/4.615ms =  217                                    */
/*                  3000ms/4.615ms =  650                                    */
/*                  5000ms/4.615ms = 1083                                    */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_Tav_TICKS_MIN      650
#define SYS_PARAM_Tav_TICKS_MIN_OPT      217

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_NUM_MEASUREMENTS_PER_CARRIER                   */
/* Description:     See GSM.05.08, Measurements for normal cell selection.   */
/*                  This is the number of measurement samples per RF carrier */
/*                  over which the receive level average is computed.        */
/* Suggested Value: 5 (min. per GSM.05.08)                                   */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_NUM_MEASUREMENTS_PER_CARRIER   5


#define SYS_PARAM_NUM_MEASUREMENTS_PER_CARRIER_OPT   3


/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_NUM_MEASUREMENTS_PER_CARRIER_RESELECTION       */
/* Description:     This is the number of measurement samples per RF carrier */
/*                  over which the receive level average is computed for     */
/*                  reselection power scans.                                 */
/* Suggested Value: 3                                                        */
/* ------------------------------------------------------------------------- */


#define SYS_PARAM_NUM_MEASUREMENTS_PER_CARRIER_RESELECTION  3


/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_NUM_ATTEMPTS_PER_MEASURE_PERIOD                */
/* Description:     This is the number of measurement attempts that are made */
/*                  per measurement period for a particular carrier. The     */
/*                  measurement period is the interval between samples that  */
/*                  are used to compute the receive level average.           */
/*         i.e., SYS_PARAM_Tav_TICKS/SYS_PARAM_NUM_MEASUREMENTS_PER_CARRIER-1) */
/* Suggested Value: 4 (to support the 4 gain ranges)                         */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_NUM_ATTEMPTS_PER_MEASURE_PERIOD 1

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_NUM_MEASUREMENTS_PER_FRAME_PSCAN               */
/* Description:     Number of power measurements to make during a TDMA frame */
/*                  for initial power scan.                                  */
/*                  The hardware (mdsp) MIPS limits can be violated if more  */
/*                  than 4 are selected; this may yield unpredictable        */
/*                  results.                                                 */
/* Suggested Value: 4                                                        */
/* ------------------------------------------------------------------------- */
#ifdef FEATURE_GSM_DYNMC_PWR_MEAS
#define SYS_PARAM_NUM_MEASUREMENTS_PER_FRAME_PSCAN     (12)
#else
#define SYS_PARAM_NUM_MEASUREMENTS_PER_FRAME_PSCAN     (7)
#endif /*FEATURE_GSM_DYNMC_PWR_MEAS*/
/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_INIT_EXP_RX_PWR_LVL_dBm                        */
/* Description:     The initial guess of the power at the antenna for the    */
/*                  initial power up scan.                                   */
/* Suggested Value: -70 dBm, Center of gain range2.                          */
/*                  If there is power on a channel it is most likely in this */
/*                  range.  Lets start here, and if there is no energy on a  */
/*                  channel, it will result in a lower power measurement     */
/*                  anyways.                                                 */
/*                  This parameter is specified in 1/16 dB units.            */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_INIT_EXP_PWR_RX_LVL_dBm_x16     (-70*16)

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_ADJACENT_CELL_PWR_RX_LVL_DELTA_dB_x16          */
/* Description:     We weed out adjacent cells in the power scan list        */
/*                  which are weaker than the carrier by this threshold      */
/* Suggested Value: -13dB.  Will need to be tweaked after the new channel    */
/*                  filter is applied to power scan                          */
#define SYS_PARAM_ADJACENT_CELL_PWR_RX_LVL_DELTA_dB_x16  (-17*16)

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_POSITIVE_AGC_SLEW_RATE_dBx16                   */
/* Description:     The AGC slew rate controls the variation of gain over    */
/*                  time (represented in frames).                            */
/* Suggested Value: +80 dBm in 52 frames. If the Rx signal power has changed  */
/*                  by more than +80 dB in 52 frames.                         */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_POSITIVE_AGC_SLEW_RATE_dBx16  ((80*16)/52)


/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_NEGATIVE_AGC_SLEW_RATE_dBx16                   */
/* Description:     The AGC slew rate controls the variation of gain over    */
/*                  time (represented in frames).                            */
/* Suggested Value: -80 dBm in 52 frames. If the Rx signal power has changed  */
/*                  by more than -80 dB in 52 frames.                         */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_NEGATIVE_AGC_SLEW_RATE_dBx16  ((-80*16)/52)

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_RX_LVL_dBm_x16                             */
/* Description:     This parameter is used to bound the sorting of power     */
/*                  measure results. It is used as one of the parameters to  */
/*                  bin power results.                                       */
/* Suggested Value: -10 dBm.                                                 */
/*                  This parameter is specified in 1/16 dB units.            */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_RX_LVL_dBm_x16             (-10*16)

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MIN_ACCEPTABLE_RX_LVL_dBm_x16                  */
/* Description:     This parameter is used to filter out attempting to Rx    */
/*                  FCCH/SCH bursts on ARFCNs whose signal levels are below  */
/*                  this power threshold.                                    */
/* Suggested Value: -107 dBm.                                                */
/*                  This parameter is specified in 1/16 dB units.            */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MIN_ACCEPTABLE_RX_LVL_dBm_x16  (-107*16) /* -107 dBm */
#define SYS_PARAM_MIN_ACCEPTABLE_PLMN_dBm_x16    (-102*16) /* -102 dBm */

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_NCELL_RX_LVL_DROP_dB_x16                   */
/* Description:     This parameter is used to trigger SCH decodes when the   */
/*                  rx level drop between consecutive measurements on a cell */
/*                  changes by this factor                                   */
/* Suggested Value: -10dB                                                    */
/*                  This parameter is specified in 1/16 dB units.            */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_NCELL_RX_LVL_DROP_dB_x16  (-10*16) /* - 10dB */

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_FRAMES_FOR_FCCH_DETECTION                  */
/* Description:     The timeout value in frames for detecting an FCCH tone   */
/*                  during acquisition.                                      */
/* Suggested Value: 32 frames, to be able to go through at least 2 fades     */
/*                  that are 10 to 11 frames apart.                          */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_FRAMES_FOR_FCCH_DETECTION 12

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_FRAMES_FOR_SCH_DECODING                    */
/* Description:     The timeout value in frames for decoding an SCH burst.   */
/* Suggested Value: 153 frames, to allow for the AFC to converge.            */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_FRAMES_FOR_SCH_DECODING 33

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_ATTEMPTS_FOR_SCH_DECODING                  */
/* Description:     The max attempts for decoding an SCH burst.              */
/* Suggested Value: 1 for normal acquisition.                                 */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_ATTEMPTS_FOR_SCH_DECODING 2

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_FRAMES_FOR_FCCH_DETECTION_QTA              */
/* Description:     The timeout value in frames for detecting an FCCH tone   */
/*                  during acquisition in QTA mode.                          */
/* Suggested Value: 31 frames, to be able to go through at least 2 fades     */
/*                  that are 10 to 11 frames apart.                          */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_FRAMES_FOR_FCCH_DETECTION_QTA 12

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_FRAMES_FOR_SCH_DECODING_QTA                */
/* Description:     The timeout value in frames for decoding an SCH burst.   */
/* Suggested Value: 32 frames, to allow for the AFC to converge.             */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_FRAMES_FOR_SCH_DECODING_QTA 23

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_ATTEMPTS_FOR_SCH_DECODING_QTA              */
/* Description:     The max attempts for decoding an SCH burst.              */
/* Suggested Value: 1 for normal acquisition.                                 */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_ATTEMPTS_FOR_SCH_DECODING_QTA 2

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_FRAMES_FOR_FCCH_DETECTION_KNOWN_SCH_TIMING */
/* Description:     The timeout value in frames for detecting an FCCH tone   */
/*                  during acquisition.                                      */
/* Suggested Value: 2 frames, when SCH timing is known.                      */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_FRAMES_FOR_FCCH_DETECTION_KNOWN_SCH_TIMING 2

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_FRAMES_FOR_SCH_DECODING_KNOWN_SCH_TIMING   */
/* Description:     The timeout value in frames for decoding an SCH burst.   */
/* Suggested Value: 3 frames, when SCH timing is known.                      */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_FRAMES_FOR_SCH_DECODING_KNOWN_SCH_TIMING 3

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_ATTEMPTS_FOR_SCH_DECODING_KNOWN_SCH_TIMING */
/* Description:     The max attempts for decoding an SCH burst.              */
/* Suggested Value: 1 when SCH timing is known.                              */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_ATTEMPTS_FOR_SCH_DECODING_KNOWN_SCH_TIMING 1

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*        N E I G H B O R   C E L L   S Y S T E M   P A R A M E T R S        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MIN_ACCEPTABLE_NCELL_RX_LVL_dBm_x16            */
/* Description:     This parameter is used to filter out attempting decode   */
/*                  Ncell FCCH/SCH bursts on ARFCNs whose signal levels are  */
/*                  below. Eventually this might be made the same as         */
/*                   SYS_PARAM_MIN_ACCEPTABLE_RX_LVL_dBm_x16                 */
/* Suggested Value: -112 dBm.                                                */
/*                  This parameter is specified in 1/16 dB units.            */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MIN_ACCEPTABLE_NCELL_RX_LVL_dBm_x16  (-110*16)  /* -112 dBm */


/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_NUM_MEASUREMENTS_PER_FRAME_NCELL_PWR_MON       */
/* Description:     Number of power measurements to make during a TDMA frame */
/*                  for IDLE mode monitoring during PCH block.               */
/*                  The hardware (mdsp) MIPS limits can be violated if more  */
/*                  than 2 are selected; this may yield unpredictable        */
/*                  results.                                                 */
/* Suggested Value: 2 (MON done during normal PCH RX burst )                 */
/*                  4 if early paging decode is supported                    */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_NUM_MEASUREMENTS_PER_FRAME_NCELL_PWR_MON     5

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_NUM_MEAS_PER_FRAME_NCELL_PWR_MON_REDUCED       */
/* Description:     Number of power measurements to make during a TDMA frame */
/*                  for IDLE mode monitoring during PCH block.               */
/*                  IN DSDA , when G is a Victim in IDLE , W in connected    */
/*                  state , the power monitors coudl get desence             */
/*                  Thus perform reduce power mon limiting desecne           */
/* Suggested Value: 1                                                        */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_NUM_MEAS_PER_FRAME_NCELL_PWR_MON_REDUCED     1

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_NUM_MEASUREMENTS_PER_FRAME_NCELL_PWR_SCAN      */

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_FRAMES_FOR_NCELL_FCCH_DETECTION            */
/* Description:     The timeout value in frames for detecting an FCCH tone   */
/*                  during neighbor cell combined acquisition.               */
/* Suggested Value: 12 frames, to be able to see at leat on complete FCB.    */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_FRAMES_FOR_NCELL_FCCH_DETECTION          12

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_FRAMES_FOR_NCELL_SCH_DECODING              */
/* Description:     The timeout value in frames for decoding an SCH burst    */
/*                  during neighbor cell combined acquisition.               */
/* Suggested Value: 15 frames, 12 to get FCB plus 3 for SCH (conservative).  */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_FRAMES_FOR_SCH_NCELL_DECODING            15

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_MAX_ATTEMPTS_FOR_NCELL_SCH_DECODING            */
/* Description:     The max attempts for decoding an SCH burst               */
/*                  during neighbor cell combined acquisition.               */
/* Suggested Value: 1.                                                       */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_MAX_ATTEMPTS_FOR_NCELL_SCH_DECODING          1

/*****************************************************************************/
/* Constants used in gl1_msg_dtx_detection_algorithm                         */
/*****************************************************************************/

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_DTX_DET_RXQUAL_SUB_THRESHOLD                   */
/* Description:     Minimum RXQUAL_SUB (SACCH frames quality) required for   */
/*                  dtx detection algorithm to detect DTX.                   */
/* Suggested Value: 2.                                                       */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_DTX_DET_RXQUAL_SUB_THRESHOLD       2

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_DTX_DET_RXQUAL_FULL_THRESHOLD                  */
/* Description:     Maximum RXQUAL_SUB (SACCH  + TCH frames quality)         */
/*                  allowed for dtx detection algorithm to detect DTX.       */
/* Suggested Value: 4.                                                       */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_DTX_DET_RXQUAL_FULL_THRESHOLD      4

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_DTX_DET_RXLEV_SUB_THRESHOLD                    */
/* Description:     Minimum RXLEV_SUB (SACCH frames level) required for      */
/*                  dtx detection algorithm to detect DTX.                   */
/* Suggested Value: 5.                                                       */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_DTX_DET_RXLEV_SUB_THRESHOLD        5

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_DTX_DET_COUNTER_THRESHOLD                      */
/* Description:     Minimum number of SACCH cycles (480 ms) the conditions   */
/*                  have to be met before the algorithm declares             */
/*                  DTX detected.                                            */
/* Suggested Value: 1 SACCH report Cycle (480 ms).                           */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_DTX_DET_COUNTER_THRESHOLD          1


/*****************************************************************************/
/* Constants used in gl1_msg_amr_pd_algorithm                                */
/*****************************************************************************/

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_AMR_PD_RXQUAL_SUB_THRESHOLD                    */
/* Description:     Minimum RXQUAL_SUB (SACCH frames quality) required to    */
/*                  enable Partial Decod.                                    */
/* Suggested Value: 4.                                                       */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_AMR_PD_RXQUAL_SUB_THRESHOLD       4

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_AMR_PD_RXQUAL_FULL_THRESHOLD                   */
/* Description:     Minimum RXQUAL_SUB (SACCH + TCH frames quality)          */
/*                  required to enable Partial Decod.                        */
/* Suggested Value: 4.                                                       */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_AMR_PD_RXQUAL_FULL_THRESHOLD      4

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_AMR_PD_HARD_RXQUAL_THRESHOLD                   */
/* Description:     Minimum hard rxqual required to  enable Partial Decode.  */
/* Suggested Value: 32243.                                                   */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_AMR_PD_HARD_RXQUAL_THRESHOLD      32243

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_AMR_PD_RXLEV_SUB_THRESHOLD                     */
/* Description:     Minimum RXLEV_SUB (SACCH frames level) required to       */
/*                  enable Partial Decode.                                   */
/* Suggested Value: 5.                                                       */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_AMR_PD_RXLEV_SUB_THRESHOLD        5

/* ------------------------------------------------------------------------- */
/* Item Name:       SYS_PARAM_AMR_PD_COUNTER_THRESHOLD                       */
/* Description:     Minimum number of SACCH cycles (480 ms) the conditions   */
/*                  have to be met before the algorithm can enabe            */
/*                  Partial Decode.                                          */
/* Suggested Value: 2 SACCH report Cycle (960 ms).                           */
/* ------------------------------------------------------------------------- */
#define SYS_PARAM_AMR_PD_COUNTER_THRESHOLD          2

#endif /* SYS_PARAMS_H */
