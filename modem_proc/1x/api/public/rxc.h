#ifndef RXC_H
#define RXC_H
/*===========================================================================

                R X C D M A   S U B T A S K    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for the main
  task to interface with the RXC subtask.

  Copyright (c) 1990 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxc.h_v   1.29   29 Oct 2002 10:54:06   baon  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/rxc.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/09   jtm     Removed unnecessary includes.
09/14/09   adw     Changes to support modem data header CMI refactoring.
08/31/09   adw     Removed unnecessary customer.h include.
08/28/09   adw     Removed unnecessary inclusion of private mdrrlp.h header.
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
08/05/09   jtm     Lint fixes.
05/20/09   adw     Removed unnecessary includes.
04/08/09   adw     Added temporary CMI-lite featurization.
04/02/09   jtm     Removed featurization.
03/19/09   jtm     Updated included headers.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_MUX.05.00.04
07/17/07   vlc     Moved rxc_get_chan_tc_demod_config_param outside
                   FEATURE_IS2000_REL_A_CC.
01/09/07   vlc     Moved rxc_chan_demod_config_param_type outside
                   FEATURE_IS2000_REL_A_CC.  This type is used by SRCH
                   outside the REL_A feature.
06/09/06   trc     Remove T_MSM6700 featurization
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
04/13/05   bn/fq   Moved all the common defines and structures to rxcmc.h
12/07/04   ejv     Added rxc_tc_chan_id_type, rxc_sch_info_type,
                   rxc_tc_chan_info_type, and prototype for
                   rxc_get_chan_tc_demod_config_param.
11/09/05   vlc     Added support for Rel D Fast Call Set Up Bypass 2 Good Frame
                   feature.
09/28/04   sr      Merged in changes from 6550 2009 build
08/09/04   bn/kwo  Added new interfaces to expose chan mod setup parameters
05/26/04   vlc     Added RXC_FOR_PDCH to rxc_dsch_type.
05/07/04   vlc     Implemented new RLP interface for EV-DV.  The new interface
                   replaces the current interface for 1x channels as well.
                   Also, modified RLP logging extensively to work with new
                   interface.
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
03/03/04   sr      Changes for DV plus included changes to MSMSHARED tip
01/13/04   jrp     Moved descramble bypass variables in RXC_TCI_F within
                   FEATURE_IS2000_REL_C_DV.
11/04/03   bn      Defined FCCCH deint task in first slot and BCCH deint task in second slot
10/09/03   bn      Corrected the FCCCH data type in rxc_fccch_info_type
04/18/03   bn      Removed JCDMA Features in FER so that it can apply to DIAG
03/20/03   bn,sr   Added NCCCH_STATE to support simultaneously monitor BCCH and FCCCH
02/10/03   fc      Added page_match in rxc_wake_type.
11/19/02   hxw     Added rxc_gps_cmd_type support for gpsOne.
10/29/02   bn      Added new fields in the rxc_chan_config_cmd_type and chan_info
                   to support BC BCCH
08/13/02   sr      Removed rxc_idle_chan_config(). New SRCH-RXC interface for
                   Fast RAHO replaces this.
07/11/02   bn      Modified function prototype to support new QIB logic
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up
                   (feature FEATURE_IS2000_REL_A_VP2.)
04/19/02   jrp     Removed fch_incl from rxc_get_setpt_cmd_type.
04/01/02   lad     Added members needed for BCCH/FCCCH frame info logging.
03/27/02   ems     Modified sizes of ser and energy fields, featurized with
                   MSMHW_EXTENDED_ENERGY_MATRIX.
02/27/02   lad     Merged IS2000 Rel A Common Channels:
                   Added rxc_idle_chan_config().
                   Added rxc_idle_activate().
                   Assigned members of rxc_chan_type to the values of
                   caix_chan_type to eliminate need for translation.  Updated
                   rxc_idle_cmd_type to contain chan_id.  Added 'length' and
                   'frame' fields to rxc_dec_type.
                   Added the complete BCCH/FCCCH cmd interfaces & unused code.
02/25/02   bn      Added support for  MUX SVD signaling
01/21/02   bgc     Merged data changes for disabling MDR.
01/03/02   bgc     Added SCH featurization to turn off for MSM6000.
12/10/01   hrk     Renamed RXC_FULL_FRAME to RXC_GOOD_FRAME.
11/13/01   bn      Added the enhance DCCH DTX detection and channel supervision logics
08/22/01   bn      Support DCCH in FTM mode
06/15/01   snn     Added extern for new functions for DCCH UI display.
06/15/01   day     Mainlined FEATURE_FFPC, FEATURE_IS95B, FEATURE_RXC_ACTION_TIME_PROCESSING,
                   FEATURE_IS2000
                   Deleted FEATURE_FFPC_OLD, FEATURE_SPECIAL_MDR
06/11/01   fc      Removed #ifdef for rxc_pc_fade_type.
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
06/01/01   snn     Modified the rxc_get_sch_info to return SCH RC. Added
                   prototype for rxc_get_fch_rate_set()
05/11/01   jq      Added RXC_NO_FRAME.
05/03/01   lcc     Changed rxc_returned_type to include "included" field for
                   rev_pwr_cntl_delay.  Also made some minor restructuring.
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
03/20/01   ych     Merged JCDMA Features
03/08/01   va      Added rxc_get_setpt_cmd_type data structure.
03/07/01   va      ASdded p_rev_in_use in the TCI_F and RETURNED_F interface.
02/01/01   tc      Updated RXC_RIF_HHO_OR_CFS_F to RXC_RIFHHO_CFS_GPS_F.
01/29/01   tc      Merged GPS support.
11/29/00   jq      Moved defination of rxc_wake_type.slot out of BROADCAST_SMS.
10/04/00   jq      Added FTM FCH and SCH loopback feature
09/06/00   bgc     Added FEATURE_FACTORY_TESTMODE.
07/19/00   hrk     Moved member rev_pwr_cntl_delay in rxc_tci_cmd_type out of
                   FEATURE_FFPC.
07/17/00   va      Added support for rev pwr cntl delay.
07/14/00   rm      Added suuport for multiframe interleaving.
06/05/00   fc      Added rxc_olr_cmd_type.
06/02/00   lad     Changed order of rxc_frame_type enum for logging.
05/31/00   fc      Added command RXC_OLR_F.
05/26/00   jq      Added support for the new rxc design to support BCSMS
05/18/00   et      added support for dtx in sch
04/24/00   hrk     Added new transaction processing support.
04/12/00   hrk     Added prototype for rxc_send_rlp_frames_log().
04/10/00   dgy     Removed FEATURE_IS2000_QPCH.
04/07/00   va      Added RXC_RF_TUNE_F (HHO fix from PP)
03/27/00   sr      Changed the type of scc_energy[] from word to int2
03/24/00   bgc     Added enable_skip_tci in tci_cmd_type for PLT.
03/24/00   na      Added prototype for rxc_init_sch_mux().
03/16/00   na      Included prototypes for rxc_get_sch_info().
02/02/00   hrk     Merged with version GV40401 rxc.h from ASW archive:
           dna     Added timeout and is_hho as payload to the
                   RXC_RIF_HHO_OR_CFS_F command.
           aaj     Added FEATURE_DETECT_SUP_MUTING for MDR sup muting detection
01/31/00   rm      Clean up TEMP_HACKS_FOR_TIGER.
01/24/00   rm      Added in support for ACTION TIME processing.
12/17/99   va      Added transaction_num to rxc_pwr_ctrl_cmd_type,
                   Removed return_from_hho from rxc_returned_type interface.
12/13/99   hrk     Introduced new RXC-FFPC interface. Older implementation
                   of FFPC is featurized to FEATURE_FFPC_OLD
12/01/99   hrk     SOme FFPC parameters expanded to quadword range.
11/22/99   va      Expanded the following MC-RXC interfaces for the IS2000
                   support:
                   RXC_TCI_F
                   RXC_SO_F
                   RXC_RETURNED_F
                   RXC_HO_F (new)
                   RXC_PWR_CTRL_F (new)
                   RXC_FPC_SUBCHAN_GAIN_F (new)
                   The old RXC_HO_F has been replaced by RXC_HHO_BEGIN_F.
11/09/99   rm      Added decode type as part of RXC_SCH command to distinguish
                   between Convolutional / Turbo coding.
11/04/99   lcc     Corrected FFPC featurization in txc_tci_cmd_type.
                   Merged from GD4021:
           kmp     Mainlained the FEATURE_ENHANCED_STANDBY_II feature by
                   removing the #ifdef around the rxc_load_pc_isr() prototype.
           ry      Added extern declaration for rxc_flag_good_frame()
10/27/99   ajn     Added QPCH support
10/21/99   rm      Added new command interface RXC_SCCH_F to support MDR
                   for MSM5000.
10/07/99   lcc     Merged from PLT 2.0:
           jcw     Added support for F3 message control for PLT demod
           hrk     Added support for Outer Loop on F-FCH
           rm      Added new Interface for SCH.
09/17/99   lcc     Merged in new interface definitions for IS2000 and MSM5000
                   support from PLT archive:
                   1. Fixed problem in tci struct when building for MSM3000
                   2. Removed T_MSM5000PLT.
                   3. Made changes in TCI command to add RC parameter for
                      IS2000. Also  changes dec_tc_type to dec_fch_type.
08/26/99   aaj     Merged Panther Branch 3.1 to Mainline
08/25/99   aaj     Added new definition for sup channel SER threshold for RS2
08/09/99   ry      Combined FEATURE_IS95B_HARD_HO and FEATURE_IS95B_CFS into
                   FEATURE_IS95B_MAHHO
07/09/99   ry      Added RXC_RIF_HHO_OR_CFS_F and RXC_RETURNED_F
06/17/99   kmp     Merged in the one change listed below.
06/07/99   ks      Added rxc_pc_fade_type enum for various fade types.
04/06/99   aaj     Removed functions prototypes for RXDSP clock changes.
                   Added function prototype for rxc_voc_xchg()
02/26/99   ejv     Added prototype for rxc_load_pc_isr, part of FEATURE_
                   ENHANCED_STANDBY_II.
02/03/99   aaj     Added protoype declarations for RXDSP clk change routines
01/15/99   aaj     Merged MDR branch to Panther mainline
11/20/98   aaj     Added support for Medium (High) data rate sup channels
11/03/98   abh     Changes for frame no. correction after frame offset change.
09/17/98   bgc     Added rxc_set_frame_offset().
08/31/98   pms     Added Tan's Pascal calling convention code for ROM savings
09/06/96   gb      Added dec_date rate determination field to dec buffer type.
04/09/96   rdh     Added funct to return whether or not we're in Paging slot.
03/06/96   jca     Added RXC_PWR_REP_DELAY_F to rxc_cmd_name_type.
12/12/95   jca     Changed (there is no) rx_cmd_q to rxc_cmd_q.
08/10/95   gb      Added New Markov Service Options and made all service
                     options CAI_SO_*.
07/26/95   rah     Added rxc_reset_voc_init call.
07/13/95   gb      Changed mux option initialization.
06/14/95   gb      Added support for Rate set 2 service options.
08/09/94   jca     Added support for IS96/IS96A dual-capability vocoder.
03/24/94   gb      Added called interface to set isr.
06/24/92   jai     First Revision
10/10/90   rrd     created file

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

//common
#include "comdef.h"

//1X
#include "dec5000.h"
#include "cai.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef struct
{
  dword frame_cnt;  /* Total frame count */
  dword ser1;   /* Sum of all 192 bit frame symbol error counts */
  dword ser2;   /* Sum of all 96 bit frame symbol error counts */
  dword ser4;   /* Sum of all 48 bit frame symbol error counts */
  dword ser8;   /* Sum of all 24 bit frame symbol error counts */
} rxc_ser_type;

extern cai_data_rate_type rxc_rpt_voc_rate(void);
  /* report current vocoder frame rate */

extern void rxc_ser_reset( void );
  /* reset ser statistics */

extern void rxc_ser_report ( rxc_ser_type * );
  /* report on the collected ser statistics */

/*========================================================================

FUNCTION RXC_GET_SCH_INFO

DESCRIPTION This function returns current configuration information for
            the SCHs. This is used by the UI in the data display.

            PARAMETERS:
            sch_rc       - gives the Radio configuration of the R-SCH (used
                           only if RS2 is defined )
            sch_rate_ptr - points to the current SCH rate.
            double_ptr   - is TRUE when double sized PDUs are being used
                           is FALSE when single sized PDUs are being used

DEPENDENCIES None

RETURN VALUE TRUE:  If SCH are assigned.
             FALSE: If no SCH are assigned.

SIDE EFFECTS None

=========================================================================*/
boolean rxc_get_sch_info
(
  dec_rc_type*       sch_rc,         /* ptr to SCH RC */
  dec_sch_rate_type* sch_rate_ptr,
  boolean* double_ptr
);


/*========================================================================

FUNCTION RXC_GET_FCH_RATE_SET

DESCRIPTION This function returns current Rate set used by FCH.
            This is used by the UI in the data display.

            PARAMETERS:
            None

DEPENDENCIES None

RETURN VALUE

SIDE EFFECTS None

=========================================================================*/
dec_rate_set_type rxc_get_fch_rate_set(void );

/*========================================================================

FUNCTION RXC_GET_DCCH_RATE_SET

DESCRIPTION This function returns current Rate set used by DCCH.
            This is used by the UI in the data display.

            PARAMETERS:
            None

DEPENDENCIES None

RETURN VALUE
            Rates set of the DCCH.

SIDE EFFECTS None

=========================================================================*/
extern dec_rate_set_type rxc_get_dcch_rate_set(void );

/*===========================================================================

FUNCTION RXC_DCCH_IS_ON

DESCRIPTION
  Returns if DCCH is ON.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If DCCH is On
  FALSE: If DCCH is Off

SIDE EFFECTS
  None
===========================================================================*/
extern boolean rxc_dcch_is_on(void );

/*========================================================================

FUNCTION RXC_INIT_SCH_MUX

DESCRIPTION This function initializes the array that will be used to hold
            the SCH Mux frames for RLP

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_init_sch_mux( void );


#endif /* RXC_H */
