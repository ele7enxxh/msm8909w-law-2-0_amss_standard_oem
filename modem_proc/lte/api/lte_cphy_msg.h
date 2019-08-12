/*!
  @file   lte_cphy_msg.h

  @brief  Defines the CPHY SAPs used to interface with L1.

  @detail

  This file describes the interface from upper layers to L1.  This interface
  is expected to be used principally by RRC, although there is a portion
  of this interface to be used by MAC as well.

  This interface is generated from
  $Header:

  @mainpage
  This documentation describes the L1 module.  The interface to L1 is
  broken down into @ref cphy_sap and @ref phy_sap.  L1 also exposes a
  test-only interface which exposes additional functionality to be used in
  PLT in @ref cphy_test_sap


*/
/*===========================================================================

  Copyright (c) 2006 - 2012 QUALCOMM Technologies Incorporated. All Rights 
  Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly 
  reserved. By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_cphy_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/17/15   ymu     L2 API requirement for dummy SR
06/30/15   ymu     GM handling for dummy SR request to check connectivity after LTA
10/14/15   ymu     FR29101 Shink SR for multisim scenario
03/10/15   el      CR791280: UL SPS event is populated with proper reason and SFN
06/14/15   mg      CR832574: EMBMS DPD two client implementation
12/15/14   vc     Pass tx tune info to MAC for rach restart decision at DTA end.
06/12/14   ru      CR665154: SRLTE: Fast retransmission of UL RLC packets lost 
                   due to HARQ failures in Tuneaway
07/23/13   qih     Added interface for EPHR support
06/19/13   mc      Added interface for SGLTE support
12/03/12   st      CR 402042: Added a new message to send from MAC to Ml1
11/30/12   mc      L2G Single Radio Voice Call Continuity (SRVCC) support
11/03/12   kp      Added interface changes for Rel-10 CSI-RS.
08/17/12   kp      Added report_add_neigh_meas_r10 to CA meas interface
09/05/12   tmai    Added support for rel 10 quantityCfg change 
07/12/11   tmai    Maintain W cell timing during L2W resel
08/17/12   kp      Updated CA interface with latest spec changes
08/09/12   st      CR 385047: Enhanced the TA_CMD api with a new flag 
07/23/12   kp      Added interface change for Rel-10 Carrier Aggregation feature.
07/17/12   st      MAC-Ml1 interface change for Rel 10 CA Scell MAC CE
12/08/11   tmai    W2L time transfer interface change
09/06/11   anm     Added eMBMS related interface
07/25/11   mm      Added definitions for ABORT_IRAT_CGI_REQ
06/28/11   amit    Added field scan_needed to lte_cphy_band_scan_req_params_s
06/13/11   mm      Added threshold definitions for IRAT CGI reading
06/01/11   vatsac  Added EARFCN to cphy_bplmn_cell_cnf_s
05/27/11   vatsac  Added unusable systems to band scan request
05/25/11   tjc     Added PRS commands
05/18/11   fhu     changed BPLMN CELL_CNF payload.
03/11/11   nl      reverted change for number of EUTRA layers supported.
03/07/11   mm      Changes for supporting CMAS
03/07/11   nl      modified the number of layers to the minimum number for EUTRA as 
                   specified in per 36.133 sec 4.2.2.9 and 8.1.2.1.1.1.
03/05/11   fhu     added LTE_CPHY_RFCHAIN_CNF for response
02/28/11   nl      modified the number of layers to the minimum number for EUTRA as 
                   specified in per 36.133 sec 4.2.2.9.
02/24/11   mm      Added definitions for IRAT SON/ANR
02/10/11   fhu     Added support for DS to have AT commands to pass down 
                    RX Chain requests 
12/06/10   tjc     Add CDMA system time to STOP_REQ
12/03/10   bxwu    Added rel9 fields for cell_select_cnf_s
11/30/10   fhu     Added Rel9 fields for BPLMN
09/14/10   mm      Added Rel9 fields for RSRQ based cell reselection
09/13/10   anm     Extended number of max SI msgs from 10 to 32 as per spec
09/01/10   mm      Added RLF timers and constants to dedicated config
08/17/10   mm      Added reason fields in OOS and RLF indications
08/08/10   ub      Increased num cancidates in band scan cnf to 50
06/29/10   da      Added comment to reflect that q_rxlevmin_offset will be passed
                   to ML1 only in bplmn_cell_req during HPLMN search.
06/28/10   fhu     Piggy back serving cell measurements in PLMN_STOP_CNF
06/22/10   mm      Added definitions for Release 9 mandatory features
06/22/10   ub      Changed max systems to scan in sys scan req to 10
06/14/10   fhu     Piggy back serving cell measurements in PLMN_COMPLTE_IND
06/01/10   mm      Added definitions for LTE_CPHY_SERV_CELL_MEAS_REQ
05/20/10   mm      Added p_max to common mobility request parameters
05/17/10   dk      Added MAX SR expired field in SR CNF to MAC
05/13/10   mm      Defined the structures for GERAN and CDMA measResults
05/12/10   dk      Added MAC DRX Command msg i/f from MAC->ML1
05/12/10   da      Msg definition for NMR info req\cnf
05/07/10   tjc     Added reverse CDMA time transfer support
05/05/10   mm      Added presence_antenna_port1 field to EUTRA measObj config,
                   defined a default value maro for search window size
04/28/10   mm      Added changes related to GSM connected mode measurements
04/28/10   mm      Added new fields for supporting CGI reporting
04/27/10   jyw     Updated with cdma2000 measurement quantity config.
04/28/10   svk     Readded the CDMA measurement object.
04/21/10   svk     Added WCDMA connected mode structure types
04/25/10   mm      Added new fields for supporting CGI reporting
04/24/10   ub      Updated num systems per band reported to RRC to 10
04/19/10   vp      Added gap support to conn_meas_cfg.
04/21/10   jyw     Updated with cdma2000 measurement objects.
04/07/10   ub      Increased num blacklist cells in cphy_acq_req to 20
04/05/10   amit    Added phy_cell_id and freq to lte_cphy_tdd_cfg_cnf_s
03/31/10   fhu     Changed the data type of bandwidth of BPLMN black list
03/22/10   vp      Changed LTE_CPHY_MEAS_MAX_NUM_FREQ to 1 for on-target PLT only
03/17/10   nl      Changed datatype of p_max to be int8
03/17/10   vp      Changed LTE_CPHY_MEAS_MAX_NUM_FREQ to 1 under PLT feature
03/17/10   tjc     Added GPS timetag request message
03/16/10   fhu     Replaced lte_l1_tdd_subframe_assign_e by
                     lte_l1_tdd_ul_dl_cfg_index_e
02/16/10   sureshs Added Frequency Barred to lte_cphy_resel_cancel_cause_e
02/11/10   pa      Removed deprecated interface for cell barring
02/27/10   fhu     Added bandwidth info in lte_cphy_msg_bplmn_cell_cnf_s
01/25/10   fhu     Changed bandwidth type in data structure lte_cphy_bplmn_chan_s
12/08/09   sk      Added acquisition type and cell id to lte_cphy_acq_req_s
01/08/10   sah     Reorganize Message Ids so they reside in a different
                   shareable interface file.
12/20/09   svk     Added WCDMA idle-mode information to the cph_idle_meas_config_req
12/08/09   sk      Added acquisition type and cell id to lte_cphy_acq_req_s
10/19/09   sah     Added GSM idle-mode measurement types.
11/04/09   sk      Added cdma cell info structure in reselection indication
                   to RRC
10/30/09   tjc     Added support for CDMA layers
10/29/09   sureshs Added IRAT resel failure to lte_cphy_resel_cancel_cause_e
10/26/09   fhu     Adding BPLMN support messages
10/26/09   sah     Remove FW interface dependancy.
10/21/09   mm      Added CDMA system time and eHRPD / 1X reselection parameter
                   types to lte_cphy_idle_meas_config_req
10/14/09   anm     Updated cdma sys time type from 32 bit to 64 bit
10/06/09   anm     Added suspend/resume msgs
08/10/09   sk      Added March spec changes for Meas
08/06/09   mm      Added and featurized the changes for March ASN.1 migration
07/15/09   anm     Removed obsolete mib mask and release mask
                   from dedicated cfg req
05/28/09   mm      Added UE Timers and constants as a parameter in
                   lte_cphy_common_cfg_req_param_s, required for RLF detection/
                   recovery.
04/27/09   fh      Changed lte_cphy_idle_meas_cfg_info_s:
                     removed: cfg_bitmask
                     added:
                       requested_cfg_bitmask
                       disabled_cfg_bitmask
04/27/09   pa      Reverted changes to add types to bitmask enums
04/27/09   anm     Added enable flag in PHR and MBSFN cfg
04/22/09   pa      Added types to enums for autogen functionality
04/15/09   sureshs Added one more reselection failure reason for CSG
03/13/09   ub      Updates after initial idle bringup on qtf
03/06/09   bn      Used a consistent field name prach_index_mask
03/06/09   bn      Added in preamble index mask to RACH MAC<->L1 APIs
03/04/09   gsc     Added MSG3 Transmission Indication (ML1->MAC)
02/27/09   dk      Added in the type for delta_tf_list_pucch
                   for PUCCH power control
02-27-09   anm     Separated MIB and SIB1 bit masks, added last sfn field in sib
                   req, updated num of si msgs value
02/27/09   bn      Added in the receiving time in the RAR Params MSG
02/26/09   bn      Added in initial_preamble_power in start RACH API
02/16/09   ask     Added connected-mode intra-freq measurement config.
02/15/09   da      Added RLF IND UMID
02/13/09   pa      Added enums for controlling partial roaming restrictions
01/29/09   ask     Added inter-freq idle-mode measurement config
01/23/09   gsc     Added support for closed loop RACH.
01/13/09   sureshs More Resel Cancel Req changes
01/09/09   sureshs Cell Bar Req and Resel Cancel Req changes
12/18/08   dineshd Added the CanCelConnReq (lte_cphy_cancel_conn_req_s) with
                   UMID LTE_CPHY_CANCEL_CONN_REQ
12/17/08   ub      Commented out union temporarily for building
12/16/08   ub      Added cell reselection and OOS interface structures & enums
12/15/08   dk      Added prach_frequency_offset to lte_cphy_prach_setup_param_s
10/20/08   sureshs Added S criteria params to cell select req on behalf of
                   Scott; removed paging params from the req
10/15/08   am      Added UMID LTE_CPHY_MOD_PRD_BND_IND
10/15/08   sah     Add types from MAC configuration for HARQ and DRX
10/14/08   hj      Use new band definitions from sys.h
10/14/08   sureshs Changes for using new LTE band enums
10/08/08   am      Added LTE_CPHY_MIB_IND
09/26/08   gsc     Changed trbllk_size to unit32 in pkt_build_ind.
09/23/08   sah     Update organization and doxygen comments
08/28/08   sah     Split config request into multiple configurations
08/22/08   bn      Added in flag to indicate new rach cfg or remove rach cfg
                   Added in the RNTI type in lte_cphy_ul_pkt_build_ind_s
08/14/08   bn      Renamed PDCCH_ORDER_IND to PDCCH_ORDER
08/12/08   bn      Added msgr hdr to lte_cphy_ul_pkt_build_ind_s.
08/11/08   bn      Finished up the lte_cphy_rach_cfg_req_s.
07/25/08   jyw     Moved l1 types into lte_l1_types.h and editorial changes.
07/22/08   anm     Update acq request phy struct & added stop cnf struct
02/27/08   sah     Initial release

===========================================================================*/
#ifndef LTE_CPHY_MSG_H
#define LTE_CPHY_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

//#include "msgr_umid.h"
#include <msgr_lte.h>
#include <msgr.h>
#include "lte.h"
#include "lte_cphy_msg_ids.h"
#include "lte_l1_types.h"
#include <sys.h>
#include <sys_v.h>
#include "trm.h"
#include <lte_irat_types.h>



/*===========================================================================

                      DEFINITIONS

===========================================================================*/
/*! @brief Maximum size of the black list = (4 + 16)
           4 cells are those blacklisted by RRC in previous acq attempts
           16 cells is the max blacklist passed from another RAT on IRAT to
              LTE idle mode mobility
 */
#define LTE_CPHY_MAX_ACQ_BLACKLISTED_CELLS      20


/*! @brief Fill in a correct value here */
#define LTE_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS   1


/*! @brief Number of band scan candidates to be returned */
#define LTE_CPHY_BANDSCAN_NUM_CANDIDATES_FDD    30

/*! @brief Number of band scan candidates to be returned for TDD */
#define LTE_CPHY_BANDSCAN_NUM_CANDIDATES        30

/*! @brief Number of system scan candidates to be returned */
#define LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN 10

/*! @brief Max number of PCI sent in Acq Req*/
#define LTE_CPHY_ACQ_MAX_PCI                    32

/*! @brief Maximum number of RNTIs in the PDCCH setup interface */
#define LTE_CPHY_MAX_NUM_PDCCH_RNTIS            LTE_MAX_NUM_RNTI_TYPES

/*! @brief Maximum number of SI messages that can be specified in a SIB
    schedule
*/
#define LTE_CPHY_MAX_SI_MSGS                    32

/*! @brief uint8 infinity value */
#define LTE_CPHY_UINT8_INFINITY                 0xFF

/*! @brief int8 minus infinity value */
#define LTE_CPHY_INT8_NEG_INFINITY              -128

/*! @brief Maximum number of MBSFN frame allocations */
#define LTE_CPHY_MAX_MBSFN_ALLOCATIONS            8

/*! @brief Maximum number of bands passed into for BPLMN band scan */
#define LTE_CPHY_BPLMN_MAX_BANDS                 64

/*! @brief Maximum number of frequency passed into for BPLMN frequency scan */
#define LTE_CPHY_BPLMN_MAX_FREQS                 64

/*! @brief special number to indicate "unlimited" time for a BPLMN search */
#define LTE_CPHY_BPLMN_UNLIMITED_TIME           0xFFFFFFFF

/*! @brief default search window size. In connected mode, if network configures
  a CDMA measObj without search window size, this default value is assumed */
#define LTE_CPHY_CONN_MODE_SEARCH_WIN_SIZE_NOT_PRESENT 0xFF

/*! @brief Maximum number of neigbor cells to report for NMR */
#define LTE_CPHY_MAX_CELL_FOR_NMR_REPORT               6

/*! @brief Maximum number of GSM freq for which timing info is sent in suspend_cnf */
#define LTE_CPHY_GSM_TIME_INFO_MAX_FREQ              8

/*! @brief Maximum number of freq for which timing info is sent in suspend_cnf */
#define LTE_CPHY_WCDMA_TIME_INFO_MAX_FREQ              7

/*! @brief Maximum number of cells per freq for which timing info is sent in 
suspend_cnf */
#define LTE_CPHY_WCDMA_TIME_INFO_MAX_CELLS_PER_FREQ    8

/*! @brief Invalid WCDMA RSCP value indicating RSCP is not reported */
#define LTE_CPHY_WCDMA_RSCP_UNREPORTED_VALUE           -128

/*! @brief Invalid WCDMA ECNO value indicating ECNO is not reported */
#define LTE_CPHY_WCDMA_ECNO_UNREPORTED_VALUE           -1

/*! @brief Maximum number of freq for which timing info is sent in suspend_cnf */
#define LTE_CPHY_WCDMA_TIME_INFO_MAX_FREQ              7

/*! @brief Maximum number of cells per freq for which timing info is sent in 
suspend_cnf */
#define LTE_CPHY_TDSCDMA_TIME_INFO_MAX_CELLS_PER_FREQ    8

/*! @brief Invalid TDSCDMA RSCP value indicating RSCP is not reported */
#define LTE_CPHY_TDSCDMA_RSCP_UNREPORTED_VALUE           -128

/*! @brief Invalid TDSCDMA ECNO value indicating ECNO is not reported */
#define LTE_CPHY_TDSCDMA_ECNO_UNREPORTED_VALUE           -1

/*! @brief Maximum number of freq for which timing info is sent in suspend_cnf */
#define LTE_CPHY_TDSCDMA_TIME_INFO_MAX_FREQ              7

/*! @brief Maximum MCCH config passed to ML1. Even though only 8 MCCHs could be active 
  at any time, 8 each could be configured and released at the same time */
#define LTE_CPHY_MAX_MCCH_CFG              16

/*! @brief current band + Max_additional frequency bands*/
#define LTE_CPHY_MAX_OVERLAP_BANDS         9 
/*! @brief Maximum number of SRS cfg for DCI fortmat 4*/
#define MAX_SRS_CFG_DCI_FORMAT4 3

/*! @brief Maximum number of PUCCH AN List for fortmat 3*/
#define MAX_PUCCH_AN_LIST 4

/*! @brief Maximum number of PUCCH AN List for channelSelection*/
#define MAX_PUCCH_AN_CHANNEL_SELECTION 2

#define LTE_CPHY_MAX_EPHR_PAYLOAD_LEN 19

/*! @brief Maximum number of CRS Assistance Info cell reports*/
#define LTE_CPHY_MAX_CRS_ASSIST_INFO_CELL_REPORT  8

#define LTE_CPHY_MAX_TUNEAWAY_CC  1

/* Default TRM Timeout value for Initial CPHY_RESUME_REQ */
#define LTE_CPHY_TRM_TIMEOUT_VALUE_DEFAULT 500

/* TRM Timeout value if Initial CPHY_RESUME_REQ fails*/
#define LTE_CPHY_TRM_TIMEOUT_VALUE_RESUME_FAIL 5000

/*! @brief
  Maximum number of neighbour cells referenced with a measurement object
  Defined in 36.331 Section 6.4
*/
#define LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS 32

#define LTE_CPHY_EMBMS_MAX_PRIORITY_CFG 2

#define LTE_CPHY_EMBMS_PRIORITY_ABORT_SUBFN 0xF

/*===========================================================================

                      TYPEDEF USED IN THE CPHY INTERFACE

===========================================================================*/
/******************************************************************************
                BAND SCAN & SYSTEM SCAN structures
*******************************************************************************/
/*! @brief
    Fields defining an LTE system
*/
typedef struct
{
  /*! System center frequency */
  lte_earfcn_t          earfcn;

  /*! System band 0, 1..14, 33..40 */
  sys_sband_lte_e_type  band;
  
  /*! System bandwidth */
  lte_bandwidth_e       bandwidth;
} lte_cphy_system_s;

/*! @brief
  Scan results structure
*/
typedef struct
{
  /*! Contains the band, freq and bandwidth */
  lte_cphy_system_s  system;

  /*! Energy metric in units of dBm/100KHz */
  int16              energy;
} lte_cphy_scan_result_s;

/*! @brief
  Parameters for the unusable systems in band scan request
*/
typedef struct
{
  /*! the number of systems in the list that ML1 should not search on */
  uint8                         num_systems;

  /*! the list of EARFCNs that ML1 should ignore during band scans */
  lte_cphy_system_s             system[LTE_CPHY_BANDSCAN_NUM_CANDIDATES_FDD];

}lte_cphy_band_scan_req_unusable_systems_s;

/*! @brief Defines bandwidth masks.
*/
typedef uint16 lte_cphy_bw_mask_t;

/*! @brief
  Parameters for the band scan request
*/
typedef struct
{
  /*! Number of bands to scan 1.. MAX_SUPPORTED_BANDS(TBD) */
  uint8                 num_bands;

  /*! List of bands to scan*/
  sys_sband_lte_e_type  band[LTE_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS];

  /*! the bandwidth masks for each band in the above list */
  lte_cphy_bw_mask_t    bw_mask[LTE_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS];

  /*! Flag to indicate if an actual scan is needed */
  boolean               scan_needed;

  /*! timeout value for the procedure (in seconds) */
  uint32                t_timeout;

  /*! Unusable systems */
  lte_cphy_band_scan_req_unusable_systems_s unusable_systems;

}lte_cphy_band_scan_req_params_s;

/*! @brief
  Parameters for the system scan request
*/
typedef struct
{
  /*! Whether ML1 should use early abort threshold if available; threshold to be
     configured at compile time and provided by systems team */
  boolean            use_early_abort;

  /*! The minimum number of cells ML1 should return despite early abort
      threshold criterion being satisfied */
  uint8              min_num_systems_to_scan;

  /*! Number of systems to scan 1..10 */
  uint8              num_systems;

  /*! timeout value for the procedure (in seconds) */
  uint32             t_timeout;

  /*! List of systems to scan */
  lte_cphy_system_s  system[LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];

} lte_cphy_system_scan_req_params_s;

/*! @brief
  Band scan results structure; one per band
*/
typedef struct
{
  /*! System band */
  sys_sband_lte_e_type    band;

  /*! Number of candidates found 0..1000 */
  uint16                  num_candidates;

   /*! List of candidates found in band, sorted according to energy (highest first)*/
  lte_cphy_scan_result_s  candidates[LTE_CPHY_BANDSCAN_NUM_CANDIDATES];
} lte_cphy_band_scan_result_s;

/*! @brief
  Parameters for the band scan cnf
*/
typedef struct
{
  /*! Number of band results; matches number of bands requested
      1.. LTE_BANDSCAN_MAX_SUPORTED_BANDS */
  uint8                        num_bands;

  /*! List of scan results, one for each band requested; order matches
      requested list of bands struct */
  lte_cphy_band_scan_result_s  band[LTE_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS];
} lte_cphy_band_scan_cnf_params_s;

/*! @brief
  Parameters for system scan cnf
*/
typedef struct
{
  /*! Number of systems scanned; 0 to num requested */
  uint8                   num_systems;

  /*! List of results, sorted according to energy (highest first) */
  lte_cphy_scan_result_s  system[LTE_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];
} lte_cphy_system_scan_cnf_params_s;

/******************************************************************************
                              PDSCH config common
*******************************************************************************/

/*! @brief
    PDSCH config common params  (ref: 36.331, 6.3.2)
*/
typedef struct {

  /*! The reference signal power - Range is -60 - 50
  */
  int8 ref_signal_pwr;

  /*! Provides information about the offset between Type A and Type B
      PDSCH resource elements
  */
  lte_l1_pb_e pb;


} lte_cphy_pdsch_cfg_common_param_s;


/******************************************************************************
                              PDSCH config dedicated
*******************************************************************************/
/*! @brief
    PDSCH config dedicated params (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*! P_A provides information about the exact power setting of the PDSCH
      transmission. The offset between PDSCH and RS EPRE
  */
  lte_l1_pa_e pa;
} lte_cphy_pdsch_cfg_dedicated_param_s;


/******************************************************************************
                              PUCCH config common
*******************************************************************************/
/*! @brief
    PUCCH config common params  (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*! PUCCH parameter delta shift Range: 1-3 */
  lte_l1_pucch_delta_shift_e delta_shift;

  /*! see RAN 1 spec - Range: 0-63 */
  uint8 n_rb_cqi;

  /*! see 36.211 5.4 - Range: 0-7 */
  uint8 n_cs_an;

  /*! Offset used to calculate ACK/NAK resource from PDCCH CCE - Range: 0-2047*/
  uint16 n1_pucch_an;
} lte_cphy_pucch_cfg_common_param_s;

/******************************************************************************
                              PUSCH config
*******************************************************************************/


/*! @brief
    PUSCH config params (ref: 36.331, 6.3.2)
*/
typedef struct {

  /************************** PUSCH config basic ******************************/

  /*! Parameter N_SB */
  lte_l1_pusch_n_sb_e n_sb;

  /*! Hoppping mode */
  lte_l1_slot_hopping_e hopping_mode;

  /*! PUSCH hopping offset Parameter */
  uint8 hopping_offset;

  /*! Boolean to enable 64 QAM */
  boolean enable_64_qam;

  /********************** UL refernce signal PUSCH ****************************/

  /*! Provides information if group hopping is enabled or disabled */
  boolean group_hopping_enabled;

  /*! Provides information about the assigned PUSCH group */
  lte_cphy_pusch_grp_pusch_assign_t group_assignment_pusch;

  /*! Provided information if sequence hopping is enabled or disabled */
  boolean seq_hopping_enabled;

  /*! Provides information about the cyclic shift to use in the cell -
      valid range 0-7
  */
  uint8 cyclic_shift;
} lte_cphy_pusch_cfg_common_param_s;


/*! @brief
  PRACH static parameters
*/
typedef struct
{
  /*! Random Access Preamble timing for preamble format Range: 0-15
      Defined in 36.211 Table 5.7.1-2
      Defined in 36.311 Section 6.3.2 */
  uint8 prach_cfg;

  /*! Cyclic Shifts for various preamble formats
      Defined in 36.211 Table 5.7.2-2 and Table 5.7.2-3
      0-15 for Preamble Format 0-3;
      0-6 for Preamble Format 4 [TDD] */
  uint8 cs_zone_length;

  /*! Logical Root Zadoff-Chu Sequence number for various preamble formats
      Defined in 36.211 Table 5.7.2-4 and Table 5.7.2-5
      0-837 for Preamble Format 0-3;
      0-137 for Preamble Format 4 [TDD] */
  uint16 log_root_seq_index;

  /*! Controls grouping of preamble sequences in two sets, to differentiate
      a high speed UE from normal.
      Defined in 36.331 Section 6.3.2 */
  boolean high_speed_flag;

  /*! Maximum number of HARQ re-transmissions from UE for RACH message3
      Defined in TBD */
  uint8 max_retx_msg3;

  /*! Window size for reception of RACH message2.
      Defined in 36.331 Section 6.3.2 - GenericRandomAccessParams */
  uint8 ra_rsp_win_size;

  /*! PRACH Frequency Offset for MSG1
      Defined in 36.331 Section 6.3.2 */
  uint8 prach_frequency_offset;
} lte_cphy_prach_setup_param_s;

/******************************************************************************
                      UL Sounding RS config common
*******************************************************************************/


/*! @brief
    SRS(Sounding Reference Signal) common config params(ref: 36.331, 6.3.2)
*/
typedef struct {
  /*! Flag to indicate if SRS cfg is enabled.  If set to FASLE, ML1 should discontinue
      using previous values and deconfigure SRS common cfg
  */
  boolean enable;

 /*! The available SRS bandwidths within the cell. Actual configuration depends
      on UL bandwidth.
  */
  lte_l1_srs_bw_e srs_bw_cfg;

  /*! Provides information in which sub frames SRS may be transmitted (by any
      UE) within the cell.
  */
  lte_l1_srs_subframe_e srs_subframe_config;

  /*! Provides the configuration on simultaneous transmission of Ack/Nack
      and SRS. In case of non-simultaneous transmission, SRS is dropped.
      In case of simultaneous transmission, shortened Ack/Nack format is
      used.
  */
  boolean sim_ack_nak_srs;

  /*! SRS Max Up Pts - TDD only parameter */
  boolean srx_max_up_pts;

} lte_cphy_srs_cfg_common_param_s;

/******************************************************************************
                            UL power control common
*******************************************************************************/

/*! @brief
    UL power control common config params (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*!p0_nominal value for PUSCH. Range: -126..24 in dBm */

  int16 p0_nominal_pusch;

  /*! Alpha is a 3-bit cell specific parameter provided by higher layers */
  lte_l1_ul_power_ctrl_alpha_e alpha;

  /*! p0_nominal value for PUCCH Range: -127..-96. unit dBm */
  lte_l1_ul_power_ctrl_p0_nom_pucch_t p0_nominal_pucch;

  /*! A table of Delta_TF values, one for each PUCCH format. Two bits
      (four possible values) per format except for a reference format.
  */
  lte_l1_delta_f_list_pucch_s delta_tf_list_pucch;

  /*! Parameter delta preamble msg 3*/
  int8 delta_preamble_msg3;
} lte_cphy_ul_power_ctrl_common_param_s;

/******************************************************************************
                            DRX information
*******************************************************************************/

/*! @brief
    Idle DRX information
*/
typedef struct
{
  /*! UE paging cycle in milli-seconds */
  uint16 paging_cycle;

  /*! nb value - used to derive the number of paging groups */
  lte_l1_nb_e nb;

  /*! UE identity (IMSI) mod 1024 */
  lte_l1_drx_ue_id_t ue_id;

  /*! Mod per coefficient from sib2*/
  uint16 mod_per_coeff;

} lte_cphy_idle_drx_param_s;


/*! @brief
    DRX Cycle and Drx start offset
*/
typedef struct
{
  /*! Drx Cycle */
  lte_l1_drx_long_cycle_e long_drx_cycle;

  /*! Drx Start Offset */
  uint32 offset;

} lte_l1_drx_long_cycle_start_offset_s;

/*! @brief
    Connected mode DRX dedicated information
*/
typedef struct
{
  /*! DRX Enable flag - Indicates whether to enable or disable Dedicate mode DRX */
  boolean drx_enable;

  /*! On Duration Timer [36.321]. Value in number of PDCCH sub-frames.
    Default value is [FFS]. Value psf1 corresponds to 1 PDCCH subframe,
    psf2 corresponds to 2 PDCCH sub-frames and so on */
  lte_l1_on_duration_timer_e on_duration_timer;

  /*! : DRX Inactivity Timer [36.321]. Value in number of PDCCH sub-frames.
    Default value is [FFS]. Value psf1 corresponds to 1 PDCCH subframe,
    psf2 corresponds to 2 PDCCH sub-frames and so on. */
  lte_l1_drx_inactivity_timer_e inactivity_timer;

  /*! DRX Retransmission Timer [36.321]. Value in number of PDCCH sub-frames. */
  lte_l1_drx_retx_timer_e retx_timer;

  /*! Long DRX Cycle and DRX Start Offset [36.321]. Long DRX Cycle Vvalue in
    number of sub-frames. Value sf10 corresponds to 10 sub-frames, sf20 corresponds
    to 20 subframes and so on. If shortDRX-Cycle is configured, the value shall be
    a multiple of the shortDRX-Cycle value. DRX Start Offset value in integer.
    In TDD, this can point to a DL or UL sub-frame.. */
  lte_l1_drx_long_cycle_start_offset_s long_drx_cycle_start_offset;

   /*! Short cycle DRX Enable flag - Indicates whether to enable or disable
    short cycle DRX */
  boolean short_drx_cycle_enable;

  /*! Short DRX cycle in [36.321] */
  lte_l1_drx_short_cycle_e short_drx_cycle;

  /*! DRX Short Cycle Timer [36.321]. Value in multiples of shortDRX-Cycle.
    Default value is [FFS]. A value of 1 corresponds to shortDRX-Cycle, a value
    of 2 corresponds to 2 * shortDRX-Cycle and so on. */
  uint32 short_drx_cycle_timer;

} lte_cphy_drx_cfg_dedicated_param_s;


/******************************************************************************

                            Cell Select config - S-criteria information
*******************************************************************************/
/*! @brief
  SIB1 S-criteria information
*/
typedef struct {
  /*! Minimum Rx level to camp on cell (-70..-22) * 2

   @todo update values based upon discussion within l1
   */
  lte_l1_q_rxlevmin_t q_rxlevmin;

  /*! Range: 1..8 when included in LTE_CPHY_BPLMN_CELL_REQ during HPLMN search.
      Set to 0 in all other cases.

    Offset to the signalled Qrxlevmin taken into account in the Srxlev
    evaluation as a result of a periodic search for a higher priority
    PLMN while camped normally in a VPLMN
   */
  lte_l1_q_rxlevmin_offset_t q_rxlevmin_offset;

  /*! Maximum value for output power that the UE should use */
  int8 p_max;

  /*! Boolean to indicate if Cell selection info for Release 9 is present;
    For Rel 8 this boolean will always be set to FALSE. For Rel 9 the field will
    always be set to TRUE and either signaled values or default values will be passed */
  boolean cell_select_info_rel9_present;

  /*! Cell selection info Release 9 */
  lte_l1_cell_select_info_rel9_s cell_select_info_rel9;

} lte_cphy_cell_select_cfg_s;


/******************************************************************************
                        PUCCH config dedicated
*******************************************************************************/

/*! @brief
    twoAntennaPortActivatedPUCCH-Format3-r10 (ref: 36.331, 6.3.2)
*/
typedef struct 
{
  /*enable/disable flag*/
  boolean enable;
  
  /*!Parameter:  for antenna port P0 and for antenna port P1 respectively, see TS 36.213 [23, 10.1]. */
  uint8 num_an_list;
  uint16 n3_pucch_an_list_p1[MAX_PUCCH_AN_LIST];
}lte_cphy_two_at_port_act_pucch_fm3_param_s;


/*! @brief
    PUCCH config dedicated params (ref: 36.331, 6.3.2)
*/
typedef struct 
{
  /*valid is set only then the content of structure is valid*/
  boolean valid;
  
  /*!Parameter:  for antenna port P0 and for antenna port P1 respectively, see TS 36.213 [23, 10.1]. */
  uint8 num_an_list;
  uint16 n3_pucch_an_list[MAX_PUCCH_AN_LIST];

  /*two_nntenna_port_act_pucch_fm3 info */
  lte_cphy_two_at_port_act_pucch_fm3_param_s two_nntenna_port_act_pucch_fm3_cfg;

}lte_cphy_pucch_rel10_format3_param_s;

/*! @brief
    N1PUCCH-AN-CS-r10 (ref: 36.331, 6.3.2)
*/
typedef struct 
{
  /*! Parameter:   for PUCCH format 1b with channel selection, see TS 36.213 [23, 10.1.2.2.1, 10.1.3.2.1].*/
  uint8 num_cs_list;
  uint16 cs_list[MAX_PUCCH_AN_LIST];
}lte_cphy_n1_pucch_rel10_cs_param_s;

/*! @brief
    channelSelection-r10  (ref: 36.331, 6.3.2)
*/
typedef struct 
{
  /*enable/disable flag*/
  boolean enable;
  
  /*! Parameter:   for PUCCH format 1b with channel selection, see TS 36.213 [23, 10.1.2.2.1, 10.1.3.2.1].*/
  uint8 num_an_cs_list;
  lte_cphy_n1_pucch_rel10_cs_param_s n1_pucch_an_cs_list[MAX_PUCCH_AN_CHANNEL_SELECTION];
}lte_cphy_pucch_rel10_channel_selection_s;


/*!
  @brief
  An enum describing  pucch rel10 format type
  */
typedef enum
{
  LTE_CPHY_PUCCH_CFG_MASK_BIT_FORMAT3_CFG,
  LTE_CPHY_PUCCH_CFG_MASK_BIT_CHANNEL_SELECT_CFG
}lte_cphy_pucch_rel10_format_type_e;

/*! @brief
    PUCCH config dedicated params (ref: 36.331, 6.3.2)
*/
typedef struct 
{
  /*valid is set only then the content of structure is valid*/
  boolean valid;
  
  /*describ type of pucch format used*/
  lte_cphy_pucch_rel10_format_type_e  pucch_format_type;

  /*Valid only if pucch_format_type = LTE_CPHY_PUCCH_CFG_MASK_BIT_FORMAT3_CFG*/
  lte_cphy_pucch_rel10_format3_param_s pucch_rel10_format3_cfg;

  /*Valid only if pucch_format_type = LTE_CPHY_PUCCH_CFG_MASK_BIT_CHANNEL_SELECT_CFG*/
  lte_cphy_pucch_rel10_channel_selection_s pucch_rel10_cs_cfg;

}lte_cphy_pucch_rel10_format_s;

/*! @brief
    PUCCH config dedicated params (ref: 36.331, 6.3.2)
*/
typedef struct 
{
  lte_cphy_pucch_rel10_format_s pucch_rel10_format_cfg;

  /*! simultaneous_pucch_pusch_tx parameter, ref: 36.213[23,  10.1.2.1]*/
  boolean two_antenna_port_act_pucch_format1a1b;
  
  /*! simultaneous_pucch_pusch_tx parameter, ref: 36.213[23, 10.1]*/
  boolean simultaneous_pucch_pusch_tx;

  /*! n1_pucch_an_rep_p1 parameter, ref: 36.213[23, 10.1]*/
  boolean n1_pucch_an_rep_p1_present;
  uint16 n1_pucch_an_rep_p1;
}lte_cphy_pucch_cfg_dedicated_rel10_param_s;


/*! @brief
    PUCCH config dedicated params (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*! Enable/disable field */
  boolean enable;

  /*! Parameter indicates whether ACK/NACK repetition is enable or disabled */
  boolean ack_nak_repetition_enable;

  /*! Ack Nack repetition factor */
  lte_l1_ack_nak_repetition_factor_e ack_nak_repetition_factor;

  /*! TDD ACK/NAK feedback mode enable or disable */
  boolean tdd_ack_nak_feedback_mode_enable;

  /*! n1_pucch_an_rep parameter, ref: 36.213[23, 10.1]*/
  uint16 n1_pucch_an_rep;

  /*! Indicates one of the two TDD ACK/NACK feedback modes used,
      see TS 36.213 [23, 7.3]. Bundling corresponds to use of ACK/NACK bundling
      whereas, multiplexing corresponds to ACK/NACK multiplexing.
      The same value applies to both ACK/NACK feedback modes on PUCCH as well
      as on PUSCH. This field is only applicable for TDD.
  */
  lte_l1_tdd_ack_nak_feedback_mode_e tdd_ack_nak_feedback_mode;

  /*!Indicate rel10_pucch_cfg is valid or not*/
  boolean rel10_pucch_cfg_valid;
  /*! Parameters required to configure dedicated Rel-10 PUCCH cfg information*/
  lte_cphy_pucch_cfg_dedicated_rel10_param_s rel10_pucch_cfg;

} lte_cphy_pucch_cfg_dedicated_param_s;



/******************************************************************************
                        PUSCH config dedicated
*******************************************************************************/
/*! @brief
    Rel-10 betaOffsetMC-r10 PUSCH config dedicated params (ref: 36.331, 6.3.2)
*/
  
typedef struct {
  /*! Valid/invalid for the content of structure*/
  boolean valid;

  /*! Parameter delta offset ACK index MC.  see TS 36.213 [23, Table 8.6.3-1]*/
  uint8 delta_offset_ack_index_mc;
  
  /*! Parameter delta offset RI index MC.  see TS 36.213 [23, Table 8.6.3-1]*/
  uint8 delta_offset_ri_index_mc;

  /*! Parameter delta offset CQI index MC.  see TS 36.213 [23, Table 8.6.3-1]*/
  uint8 delta_offset_cqi_index_mc;

}lte_cphy_pusch_rel10_beta_offset_mc_param_s;


/*! @brief
    Rel-10 PUSCH config dedicated params (ref: 36.331, 6.3.2)
*/
  
typedef struct {
  /*! Parameter delta offset ACK index MC.  see TS 36.213 [23, Table 8.6.3-1].*/
  lte_cphy_pusch_rel10_beta_offset_mc_param_s beta_offset_mc;

  /*! Parameter: Disable-sequence-group-hopping, see TS 36.211 [21, 5.5.1.3].*/
  boolean group_hopping_disabled;

  /*!Parameter: Activate-DMRS-with OCC, see TS 36.211 [21, 5.5.2.1]..*/
  boolean dmrs_with_occ_activated;
}lte_cphy_pusch_rel10_cfg_dedicated_param_s;

/*! @brief
    PUSCH config dedicated params (ref: 36.331, 6.3.2)
*/

typedef struct {

  /*! Parameter delta offset ACK index.  see TS 36.213 [23, Table 8.6.3-1]*/
  uint8 delta_offset_ack_index;

  /*! Parameter delta offset RI index.  see TS 36.213 [23, Table 8.6.3-1]*/
  uint8 delta_offset_ri_index;

  /*! Parameter delta offset CQI index.  see TS 36.213 [23, Table 8.6.3-1]*/
  uint8 delta_offset_cqi_index;

  /*! Indicate if Rel-10 PUSCH cfg valid or not*/
  boolean rel10_pusch_cfg_valid;
  lte_cphy_pusch_rel10_cfg_dedicated_param_s rel10_pusch_cfg;
}lte_cphy_pusch_cfg_dedicated_param_s;

/*! @brief
  The uplink transport channel configurations
  */
typedef struct
{
  /*! The maximum number of harq retransmissions before the buffer is aborted */
  lte_l1_max_harq_tx_e max_harq_tx;
  /*! Whether tti bundling is enabled */
  boolean tti_bundling;
} lte_cphy_ulsch_cfg_dedicated_param_s;


/*! @brief
  The uplink Power Headroom configurations
  */
typedef struct
{
  /*! flag to enable or disable PHR cfg */
  boolean enable;

  /*! Periodic PHR Timer */
  lte_l1_periodic_phr_timer_e           periodic_phr_timer;

  /*! Prohibit PHR Timer */
  lte_l1_prohibit_phr_timer_e           prohibit_phr_timer;

  /*! DL Pathloss change */
  lte_l1_dl_pathloss_change_e           dl_pathloss_change;

  boolean extended_phr_indicated; /*!< extendedPHR Indicates if power headroom shall be reported using the 
   Extended Power Headroom Report MAC control element defined in TS 36.321 [6] (value TRUE) or the Power Headroom Report MAC control element
   defined in TS 36.321 [6] (value FALSE).*/
} lte_cphy_phr_cfg_dedicated_param_s;
/******************************************************************************
                            UL power control dedicated
*******************************************************************************/

/*! @brief
    UL power control dedicated params (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*! flag to enable or disable R-10 UL power control dedicated cfg */
  boolean enable;

  /*! pSRS-AP offset 0..15 */
  lte_l1_ul_power_ctrl_psrs_offsetAp_t p_srs_offset_Ap; 
} lte_cphy_ul_power_ctrl_dedicated_r10_param_s;


/*! @brief
    UL power control dedicated params (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*! P0_UE value for PUSCH. Range: -8..7 in dB - Field applicable for non
      persistent scheduling only
  */
  //int8 p0_nominal_pusch;

  lte_l1_ul_power_ctrl_p0_ue_t p0_ue_pusch;

  /*! Specifies whether or not mcs is enabled */
  lte_l1_ul_power_ctrl_delta_mcs_enable_e delta_mcs_enabled;

  /*! Provides information if accumulation of PUSCH TPC commands is enabled
      or disabled
  */
  boolean accum_enabled;

  /*! P0_UE value for PUCCH. Range: -8..7 in dB */
  lte_l1_ul_power_ctrl_p0_ue_t p0_ue_pucch;

  /*! pSRS offset 0..15 */
  lte_l1_ul_power_ctrl_psrs_offset_t p_srs_offset;

  /*! Filter coefficient used to determine the filtered RSRP which is used in
    pathloss computation */
  lte_l1_ul_power_ctrl_filter_coefficient_e filter_coefficient;

  lte_cphy_ul_power_ctrl_dedicated_r10_param_s r10_cfg;

} lte_cphy_ul_power_ctrl_dedicated_param_s;

/******************************************************************************
                      UL Sounding RS config dedicated
*******************************************************************************/

/*! @brief
    UL sounding RS dedicated config params  (ref: 36.331, 6.3.2)
*/

typedef struct {
    /*! Enable field */
    boolean enable;

    /*Valid an1 ,    an2 ,    an4 */
   lte_l1_antenna_info_ports_count_e srs_antenna_port;
} lte_cphy_srs_cfg_rel10_ant_param_s;


typedef struct {
  /*! Provides information on AntennaPort */
  lte_l1_antenna_info_ports_count_e srs_antenna_port;

  /*! A maximum of four bandwidths. Less than four in more narrow system
      bandwidths
  */
  lte_l1_srs_bw_e srs_bw;

  /*! freqDomainPositionAp-r10           INTEGER   (0 .  . 23 )  */
  uint8 freq_domain_position_ap;

  /*!    transmissionCombAp-r10                  INTEGER   (0 .  . 1 ) */
  uint8 tx_comb_ap;


  /*! Provides information (n-SRS) used to calculate cyclic shift for the SRS
      transmission from the UE.
  */
  lte_l1_srs_cyclic_shift_e cyclic_shift;
  } lte_cphy_srs_cfg_ap_param_s;

typedef struct {
  /*! Enable field */
  boolean enable;

  /*! Provides information SRS cfg for DCI format 0*/
  lte_cphy_srs_cfg_ap_param_s srs_cfg_dci_format0;

  /*! Provides information SRS cfg for DCI format 1a2b2c*/
  lte_cphy_srs_cfg_ap_param_s srs_cfg_dci_format_1a2b2c;

  } lte_cphy_srs_cfg_activate_ap_param_s;


typedef struct {
  /*! Enable field */
  boolean enable;

  /*! SRS Configuration Index. Range: 0-1023*/
  uint16 srs_cfg_index;
  
  /*! Provides information SRS cfg for DCI format 4
  */
  uint8 num_srs_cfg_dci_format4;
  lte_cphy_srs_cfg_ap_param_s srs_cfg_dci_format4[MAX_SRS_CFG_DCI_FORMAT4];
  
  /*! Provides information SRS cfg for DCI format 0, 1a, 2b, 2c*/
  lte_cphy_srs_cfg_activate_ap_param_s srs_cfg_dci_format_01a2b2c;

} lte_cphy_srs_cfg_dedicated_aperiodic_param_s;

typedef struct {
  /*! Enable field */
  boolean enable;

  /*! A maximum of four bandwidths. Less than four in more narrow system
      bandwidths
  */
  lte_l1_srs_bw_e srs_bw;

  /*! Parameter SRS hopping bandwith */
  lte_l1_srs_hopping_bw_e srs_hbw;

  /*! Frequency domain position.  Range: 0-23 */
  uint8 freq_domain_pos;

  /*! Provides information about the duration of SRS transmission
      (FALSE = once or TRUE = indefinite )
     FFS may disappear
  */
  boolean duration;


  /*! SRS Configuration Index. Range: 0-1023*/
  uint16 srs_cfg_index;

  /*! Provides information about the transmission comb (0 or 1) of the SRS
      transmission from the UE
  */
  uint8 tx_comb;

  /*! Provides information (n-SRS) used to calculate cyclic shift for the SRS
      transmission from the UE.
  */
  lte_l1_srs_cyclic_shift_e cyclic_shift;

} lte_cphy_srs_periodic_cfg_dedicated_param_s;

/*!
  @brief
  An enum describing the bit positions in the
  lte_cphy_srs_cfg_dedicated_param_s
  */
enum
{
  LTE_CPHY_DEDICATED_SRS_PERIODIC_CFG_MASK_BIT                 = 0x0001,
  LTE_CPHY_DEDICATED_SRS_R10_APERIODIC_CFG_MASK_BIT            = 0x0002,
  LTE_CPHY_DEDICATED_SRS_R10_ANT_PORT_CFG_MASK_BIT             = 0x0004,
} ;

typedef struct {

  uint8 srs_cfg_mask;

  /*! Legacy SRS periodic cfg parameter i.e.  when  srs_cfg_mask has 
  LTE_CPHY_DEDICATED_SRS_PERIODIC_CFG_MASK_BIT====
  */
  lte_cphy_srs_periodic_cfg_dedicated_param_s srs_periodic_cfg_ded;

  /*! Rel-10 SRS cfg parameter i.e.  when  srs_cfg_mask has 
  LTE_CPHY_DEDICATED_SRS_R10_APERIODIC_CFG_MASK_BIT ====
  */

  /*! Parameters required to configure UL Aperiodic Sounding Reference Signal*/
  lte_cphy_srs_cfg_dedicated_aperiodic_param_s r10_aperiodic_srs_cfg_ded;

  /*! Rel-10 SRS ANT cfg parameter i.e.  when  srs_cfg_mask has 
  LTE_CPHY_DEDICATED_SRS_R10_ANT_PORT_CFG_MASK_BIT ====
  */

  /*Valid an1 ,    an2 ,    an4 */
  lte_cphy_srs_cfg_rel10_ant_param_s r10_srs_antenna_port;

} lte_cphy_srs_cfg_dedicated_param_s;


/*! @brief
  CQI reporting parameters
*/
typedef struct
{
  /*! Enable/disable field */
  boolean enable;

  /*! CQI reporting mode aperiodic enable / disable flag */
  boolean cqi_reporting_mode_aperiodic_enable;

  /*! CQI reporting mode aperiodic */
  lte_l1_cqi_reporting_mode_aperiodic_e cqi_reporting_mode_aperiodic;

  /*! Provides information about the nominal measurement offset (in dB)
      between the PDSCH and RS EPRE that the UE should assume when
      computing CQI.  Range: 1-6  */
  int8 nom_pdsch_rs_epre_offset;

  /*! Flag to indicate if reporting periodic info present or not */
  boolean cqi_reporting_periodic_present;

  /*! CQI reporting periodic info.  Only valid if present field is set to TRUE */
  lte_l1_cqi_reporting_periodic_s cqi_reporting_periodic;

  /*! Flag to indicate if CQI release 9 parameters are present or not */
  boolean cqi_rel9_param_present;

  /*! CQI release 9 parameters */
  lte_l1_cqi_rel9_param_s cqi_rel9_param;

  /*! Flag to indicate if CQI release 10 parameters are present or not */
  boolean cqi_rel10_param_present;

  /*! CQI release 10 parameters */
  lte_l1_cqi_rel10_param_s cqi_rel10_param;
} lte_cphy_cqi_reporting_param_s;

/******************************************************************************
                      Antenna info dedicated config
*******************************************************************************/
/*! @brief
    Antenna info dedicated config params  (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*! Information about the antenna transmission mode for PDSCH */
  lte_l1_dl_tx_mode_e transmission_mode;

  /*! A bitmask of the codebook restriction.  The number of bits used
    in the restriction varies with the transmission mode.  The bit mask
    will be placed in the lower order bits.

    36.331 Section 6.3.2

    The number of bits are as follows:

    n2TxAntenna-tm3 : 2
    n4TxAntenna-tm3 : 4
    n2TxAntenna-tm4 : 6
    n4TxAntenna-tm4 : 64
    n2TxAntenna-tm5 : 4
    n4TxAntenna-tm5 : 16
    n2TxAntenna-tm6 : 4
    n4TxAntenna-tm6 : 16

  */
  uint64 codebook_subset_restriction;

  // For TM9 8 CSI RS ports, mask can be up to 109 bits, add one more word
  // without having to change existing code. This is the most significant long word
  // of the two bit mask long words (64 bit long word)
  uint64 codebook_subset_restriction_lw;

  /*! Field to indicated if UE transmit antenna selection is disabled or enabled */
  boolean tx_antenna_selection_enabled;

  /*! Info on whether UE transmit antenna selection control is closed-loop or open-loop*/
  lte_l1_tx_antenna_selection_e tx_antenna_selection_ctrl;

} lte_cphy_antenna_cfg_dedicated_param_s;

/*! @brief
  TPC config parameters for power control information found on the
  pdcch.
test_*/
typedef struct
{
  /*! Whether this type of power control is enabled */
  boolean enable;

  /*! The rnti on which to decode power control commands */
  lte_rnti_val_t tpc_rnti;

  /*! Provides the index of the UE within the TPC-PDCCH for channel control */
  lte_l1_tpc_index_t tpc_index;

} lte_cphy_tpc_pdcch_cfg_s;

typedef enum
{
  LTE_CPHY_SR_RETX_NUM_DEFAULT = 0,
  LTE_CPHY_SR_RETX_NUM_1 = 1,            
  LTE_CPHY_SR_RETX_NUM_3 = 3
}lte_cphy_sr_retx_num_e;

/*! @brief
  Scheduling request parameters
*/
typedef struct
{

  /*! flag to enable or disable scheduling req */
  boolean enable;

  /*! The pucch resource the scheduling request is to be sent on. Range: 0-2047*/
  uint16 sr_pucch_resource_index;

  /*! The scheduling request Config Index - Range: 0-157
    Values 156 and 157 are not applicable for Release 8 */
  uint8 sr_configuration_index;

  /*! Parameter DSR TRANS MAX */
  lte_l1_dsr_trans_max_e dsr_trans_max;

  /*! Parameter DSR TRANS MAX after LTA */
  uint8 dsr_trans_max_after_lta;

  /*Max retx times SR, this is optional from MAC only for LTA scenario, if it is set, ML1 need to follow MAC pattern*/
  lte_cphy_sr_retx_num_e sr_retx_num_lta;

  /*! Boolean to indicate if SR Prohibit timer is present */
  boolean sr_prohibit_timer_present;

  /*! SR Prohibit timer - Range 0 to 7, Value 0 means no timer for SR transmission,
    1 means one SR period, Value 2 means 2 * SR period and so on */
  uint8 sr_prohibit_timer;

} lte_cphy_scheduling_req_cfg_type;

/*! @brief
  Semi Persistent Scheduling Configuration Info
*/
typedef struct
{
  /*! TRUE indicates to use SPS RNTI indicated by sps_crnti field.  FALSE
     indicates to discontinue using previous SPS RNTI */
  boolean use_sps_crnti;

  /*! SPS RNTI */
  lte_rnti_val_t sps_crnti;

  /*! SPS configuration for DL */
  lte_l1_sps_cfg_dl_s sps_cfg_dl;

  /*! SPS configuration for UL */
  lte_l1_sps_cfg_ul_s sps_cfg_ul;

}lte_cphy_sps_cfg_s;

/*! @brief
  UE related configuration parameters
*/
typedef struct
{
  /*! UE CRNTI */
  lte_rnti_val_t crnti;
} lte_cphy_ue_related_cfg_s;

/*! @brief
  Parameter struct for cell info in L1M acquisition confirmation
*/
typedef struct
{
  /*! Detected cell ID (0 - 503) */
  lte_phy_cell_id_t                     phy_cell_id;

  /*!  */
  lte_l1_mib_info_param_s               mib;

  /*! Detected cyclic prefix */
  lte_l1_cp_e                           cp;
} lte_cphy_acq_cell_info_s;

/*! @brief
  Parameter struct for cell info describing frequency in bandwith
*/
typedef struct
{

  /*! The cell id this configuration applies to or the source cell in case of
      handover */
  lte_phy_cell_id_t phy_cell_id;

  /*! The frequency the configuration applies to or the source cell in case of handover */
  lte_earfcn_t dl_cell_frequency;

}lte_cphy_cell_info_s;

typedef struct
{
  /*! The band the cell resides on */
  lte_l1_gsm_band_e band;
  /*! The ARFCN of the cell */
  lte_l1_gsm_arfcn_t arfcn;
  /*! The BSIC ID of the cell to be barred */
  lte_l1_gsm_cell_id_s cell_id;
  /*! The averaged RSSI value for the cell in dB */
  lte_l1_gsm_rssi_t rssi;
  /*! The sch timing of this cell */
  lte_l1_gsm_timing_s sch_timing;

} lte_cphy_gsm_cell_info_s;

/*! @brief
  Parameter struct for cdma cell info
*/
typedef struct
{
  /*! Band class to which the cell belongs */
  sys_band_class_e_type band_class;

  /*! Channel to which the cell belongs */
  sys_channel_num_type  channel;

  /*! Pn offset */
  sys_pilot_pn_type     cell_id;

  /*! Pilot strength */
  uint8                 pilot_strength;

}lte_cphy_cdma_cell_info_s;

/*! @brief
  Maximum number of CDMA neighbor cells per layer
  Defined in 36.331 Section 6.3
*/
#define LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS_PER_LAYER 32

/*! @brief
 Parameter struct for cdma layer info
*/
typedef struct
{
  /*! Band class to which the cell belongs */
  sys_band_class_e_type band_class;

  /*! Channel to which the cell belongs */
  sys_channel_num_type  channel;

  /*! Number of cells */
  uint8                 num_cells;

  /*! Pn offset(s) */
  sys_pilot_pn_type     cell_id[LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS_PER_LAYER];
} lte_cphy_cdma_layer_info_s;

/*! @brief
  WCDMA cell timing info (per cell)
*/
typedef struct
{
  /*! Scrambling code of the resel candidate cell */
  uint16                     psc;
  /*! PN position In units of chipx8 Range 0...38400 */
  uint32                     pn_pos;
  /*! Srxlev or Squal value of the wcdma cell used for ranking during resel
  when more than one W cell of equal priority are suitable for reselection*/
  int16                      rank;
  /*! TRUE means "sttd_indicator" value is present. FALSE otherwise. */
  boolean                    sttd_indicator_present;
  /*! sttd_indicator - valid only if sttd_indicator_present is TRUE */
  boolean                    sttd_indicator;
}lte_cphy_wcdma_cell_time_info_s;

typedef struct
{
  /*! Frequency */
  lte_uarfcn_t freq;
  /*! WCDMA cell and timing info*/
  lte_cphy_wcdma_cell_time_info_s cell_info;
}lte_cphy_wcdma_cell_info_s;

/*! @brief
  WCDMA cell timing info (per freq)
*/
typedef struct
{
  /*! Frequency */
  lte_uarfcn_t                    freq;
  uint8                           num_cells;
  /*! WCDMA timing info per cell */
  lte_cphy_wcdma_cell_time_info_s cell_time_info[LTE_CPHY_WCDMA_TIME_INFO_MAX_CELLS_PER_FREQ];
}lte_cphy_wcdma_freq_time_info_s;

/*! @brief
  WCDMA cell timing info (all cells)
*/
typedef struct
{
  uint8                           num_freq;
  /*! WCDMA timing info per freq */
  lte_cphy_wcdma_freq_time_info_s freq_time_info[LTE_CPHY_WCDMA_TIME_INFO_MAX_FREQ];
}lte_cphy_wcdma_time_info_s;


/*! @brief
  GSM cell/arfcn timing info 
*/
typedef struct
{

  /*! GSM Cell id */
  lte_l1_gsm_cell_id_s        bsic;

  /*! GSM ARFCN */
  lte_l1_gsm_complete_arfcn_s arfcn;

  /*! Timing of the GSM BSIC */
  lte_l1_gsm_timing_s timing;

  /*! Frequency offset */
  int16 freq_offset;

  /*! GSM cell power */
  lte_l1_gsm_rssi_t rssi_x16;
}lte_cphy_gsm_freq_time_info_s;

/*! @brief
  GSM cell timing info (all cells)
*/
typedef struct
{
  uint8                           num_arfcn;
  /*! WCDMA timing info per freq */
  lte_cphy_gsm_freq_time_info_s freq_time_info[LTE_CPHY_GSM_TIME_INFO_MAX_FREQ];
}lte_cphy_gsm_time_info_s;


typedef struct
{
  /*! Scrambling code of the resel candidate cell */
  uint8        psc;
  /*! Frequency */
  lte_uarfcn_t freq;

}lte_cphy_tdscdma_cell_info_s;

/*! @brief
  TDSCDMA cell timing info (per cell)
*/
typedef struct
{
  /*! TDS cell id */
  uint8                       cpid;
  /*! RSCP value of the cell */
  int16                       rscp;
}lte_cphy_tdscdma_cell_time_info_s;

/*! @brief
  TDSCDMA frequency timing info
*/
typedef struct
{
  /*! Frequency */
  lte_uarfcn_t                    freq;
  /*! number of JDS cells for interference cancellation */
  uint8                           num_of_jds_cells;
  /*! current LNA state */
  uint8                           lna_state;
  /*! ts0 start position from step0 result */
  uint16                          ts0_position;
  /*! TDSCDMA cell list */
  lte_cphy_tdscdma_cell_time_info_s cell_time_info[LTE_CPHY_TDSCDMA_TIME_INFO_MAX_CELLS_PER_FREQ];
}lte_cphy_tdscdma_freq_time_info_s;

/*! @brief
  TDSCDMA cell timing info (all cells)
*/
typedef struct
{
  uint8                           num_freq;
  /*! TDSCDMA timing info per freq */
  lte_cphy_tdscdma_freq_time_info_s freq_time_info[LTE_CPHY_TDSCDMA_TIME_INFO_MAX_FREQ];
}lte_cphy_tdscdma_time_info_s;

/*! @brief
  Parameter struct for cell info describing frequency in bandwith
*/
typedef struct
{
  /*! Boolean to indicate if ul_cell_frequency is present in ul_freq_cfg */
  boolean ul_cell_freq_present;
  lte_earfcn_t ul_cell_frequency;
  lte_bandwidth_e ul_bandwidth;
  boolean add_spectrum_emission_present;
  /* additionalSpectrumEmission Range: 0-31 */
  uint8 add_spectrum_emission;
}lte_cphy_ul_freq_cfg_s;

/*! @brief
  Target cell info
*/
typedef struct
{

  /*! The cell id of the target cell */
  lte_phy_cell_id_t target_cell_id;

  /*! The frequency of the target cell */
  lte_earfcn_t dl_cell_frequency;

  /*! The downlink bandwidth of the target cell */
  lte_bandwidth_e dl_bandwidth;

  /*! The irat target cell info */
  lte_irat_target_cell_info_s x2l_ho_info;
} lte_cphy_ho_target_cell_info_s;

/******************************************************************************
                          Antenna info common config
*******************************************************************************/

/*! @brief
    Antenna info common config params (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*! Number of cell specific antenna ports */
  lte_l1_antenna_info_ports_count_e antenna_ports_count;
} lte_cphy_antenna_cfg_common_param_s;

/******************************************************************************
                              TDD config
*******************************************************************************/

/*! @brief
    TDD config params (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*! Sub frame assignment */
  lte_l1_tdd_ul_dl_cfg_index_e subframe_assign;

  /*! Special subframe patterns */
  lte_l1_tdd_special_subframe_pattern_e  special_sub_fn_patterns;
} lte_cphy_tdd_cfg_param_s;


/******************************************************************************
                              MBSFN subframe config
*******************************************************************************/

/*! @brief The mbsfn configuration
  */
typedef struct
{
  /*! flag to enable or disable MBSFN cfg in LL. Once MBSFN main config is disabled
      other MBSFN area related configs will be erased from L1 as well */
  boolean enable;

  /* Number of MBSFN frame allocations */
  uint8 num_mbms_allocations;

  /* MBSFN frame allocation info */
  lte_l1_mbsfn_allocation_s mbsfn_alloacation[LTE_CPHY_MAX_MBSFN_ALLOCATIONS];
} lte_cphy_mbsfn_cfg_s;


/******************************************************************************
                        MBSFN area MCCH schedule config
*******************************************************************************/
/*! @brief The mbsfn area contol schedule info (SIB13)
  */
typedef struct
{
  /*! Cfg action */
  lte_l1_config_action_e                    cfg_action;

  /*! Monitor pattern */
  lte_cphy_mbsfn_area_monitor_pattern_e     monitor_pattern;

  /*! MBSFN area id: 0 - 255 */
  lte_mbsfn_area_id_t                       area_id;

  /*! Cell info where this MBSFN area is present. If cell info not same as the serving cell,
    prioritize the PMCH decodes of MBSFN areas in the serving cell, over this area */
  lte_cphy_cell_info_s                      cell_info;

  /*! Non-MBSFN region length */
  lte_l1_non_mbsfn_region_length_e          non_mbsfn_symbols;
  
  /*! MCCH config change bit position in PDCCH DCI 1C (MBMS-RNTI)
      for this MBSFN area. 0 (LSbit) - 7
      Based on MBMS-RNTI PDCCH bit mask, MBSFN area monitored by UE
      and corresponding mcch_change_bit_pos, higher layers would
      indicate L1 accordingly to acquire any change in MBSFN area
      configuration.
  */    
  uint8                                     notification_ind_bit;

  /*! MCCH schedule config */
  lte_l1_mbsfn_mcch_cfg_s                   mcch_schdl_cfg;

} lte_cphy_mbsfn_area_mcch_schdl_cfg_s;

/******************************************************************************
                        MBSFN notification config
*******************************************************************************/
/*! @brief The mbsfn area MCCH change notification schedule config
  */
typedef struct
{
  /*! Config action */
  lte_l1_config_action_e                    cfg_action;

  /*! Actual change notification repetition period common for all MCCHs
      that are configured = shortest modification period/ notificationRepetitionCoeff.
      The 'shortest modificaton period' corresponds with the lowest value of
      mcch-ModificationPeriod of all MCCHs
  */
  lte_l1_mbsfn_notification_period_e        period;

  /*! Notification repetition coefficient */
  lte_l1_mbsfn_notification_coeff_e         coeff;

  /*! Notification offset: Indicates, together with the notificationRepetitionCoeff,
      the radio frames in which the MCCH information change notification is scheduled i.e.
      the MCCH information change notification is scheduled in radio frames for which:
      SFN mod notification repetition period = notificationOffset (0 - 10)
  */
  uint8                                     offset;

  /*! Subframe index: Indicates the subframe used to transmit MCCH change notifications on PDCCH.
      FDD: Value 1, 2, 3, 4, 5 and 6 correspond with subframe #1, #2, #3 #6, #7, and #8 respectively.
      TDD: Value 1, 2, 3, 4, and 5 correspond with subframe #3, #4, #7, #8, and #9  respectively.
  */
  uint8                                     subfn_idx;

} lte_cphy_mbsfn_notification_cfg_s;

/******************************************************************************
        MBSFN Area control info schedule and notification config (SIB13)
*******************************************************************************/

/*! @brief The mbsfn Areas info schedule configuration
  */
typedef struct
{
  /*! Number of MBSFN areas valid info schedule config */
  uint8                                 num_mbsfn_areas;

  /*! MBSFN area MCCH schedule info */
  lte_cphy_mbsfn_area_mcch_schdl_cfg_s  mbsfn_area[LTE_CPHY_MAX_MCCH_CFG];

  /*! MBMS notification config to monitor MCCH change for MBSFN areas */
  lte_cphy_mbsfn_notification_cfg_s     notification_cfg;
  
} lte_cphy_mbsfn_areas_mcch_schdl_cfg_s;

/******************************************************************************
                        MBSFN Area config
*******************************************************************************/

/*! @brief The mbsfn area particular PMCH configuration
  */
typedef struct
{
  /*! flag to indicate action on allocated PMCH
  */
  lte_l1_config_action_e                        cfg_action;

  /*! Monitor pattern */
  lte_cphy_mbsfn_area_monitor_pattern_e         monitor_pattern;
  
  /*! PMCH config */
  lte_l1_mbsfn_pmch_cfg_s                       cfg;
  
} lte_cphy_mbsfn_pmch_cfg_s;

/*! @brief The mbsfn area configuration
  */
typedef struct
{
  /*! Config action for PMCH schdls per MBSFN area "mbsfn_area" */
  lte_l1_config_action_e                        cfg_action;

  /*! MBSFN area id: 0 - 255 */
  lte_mbsfn_area_id_t                           area_id;

  /*! Number of common MBSFN frame allocations for this MBSFN area */
  uint8                                         num_mbms_subfn_allocations;

  /*! Common MBSFN frame allocation info for this MBSFN area */
  lte_l1_mbsfn_allocation_s                     subfn_allocation[LTE_CPHY_MAX_MBSFN_ALLOCATIONS];

  /*! Common MBSFN frame allocation period for this MBSFN area:
      Indicates the period during which resources corresponding with field commonSF-Alloc
      (subfn_allocation) are divided between the (P)MCH that are configured for this MBSFN area.
      The subframe allocation patterns, as defined by commonSF-Alloc, repeat continously
      during this period for which SFN mod commonSF-AllocPeriod = 0.
  */
  lte_l1_mbsfn_common_sf_allocation_period_e    subfn_allocation_period;

  /*! Number of valid PMCH info */
  uint8                                         num_pmch;

  /*! PMCHs config for this MBSFN area */
  lte_cphy_mbsfn_pmch_cfg_s                     pmch[LTE_L1_MAX_PMCH_PER_MBSFN_AREA];
  
} lte_cphy_mbsfn_area_cfg_s;


/******************************************************************************
        Multiple MBSFN areas PMCH allocation configuration for MBMS content
*******************************************************************************/

/*! @brief The mbsfn configuration
  */
typedef struct
{  
  /*! Number of MBSFN areas valid info schedule config */
  uint8                                 num_mbsfn_areas;

  /*! MBSFN area config */
  lte_cphy_mbsfn_area_cfg_s             mbsfn_area[LTE_EMBMS_MAX_MBSFN_AREA];
} lte_cphy_mbsfn_areas_cfg_s;

/******************************************************************************
                 MCCH change notification Indication
*******************************************************************************/

/*! @brief MCCH change notification indication 
    36.213.930; section 11.2 
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type       msgr_hdr;

  /*! DL earfcn of scell  */
  lte_earfcn_t               dl_earfcn;

  /*! Physical cell id of scell  */
  lte_phy_cell_id_t          phy_cell_id;

  /*! PDCCH bit mask; 8 bit */
  uint8                      mask;

  /*! OTA SFN when sending this change notification */
  lte_l1_sfn_t               sfn;

} lte_cphy_mcch_change_notification_ind_s;

/******************************************************************************
                 PMCH's MTCH (MBMS content) scheduling inforamtion
*******************************************************************************/

/*! @brief MCH scheduling inforamtion 
    36.321.930; section 6.1.3.7 
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type        msgr_hdr;

  /*! MBSFN area's PMCH MSI config */
  lte_l1_mbsfn_msi_cfg_s      msi_cfg;

} lte_cphy_msi_req_s;


/******************************************************************************
                              UE Timers and Constants Parameters
*******************************************************************************/

/*! @brief UE Timers and Constants parameters
  */
typedef struct
{

  /* Timer to detect radio link failure. Started when n310 out-of-sync
    indications received. Stopped when n311 in-sync indications received. */
  lte_cphy_t310_e t310;

  /* Maximum Number of out-of-sync indications to detect physical layer problems */
  lte_cphy_n310_e n310;

  /* Maximum Number of in-sync indications to detect recovery from physical layer problems */
  lte_cphy_n311_e n311;

} lte_cphy_tmr_and_const_param_s;

/*! @brief Dedicated RLF Timers and Constants
  */
typedef struct
{

  /* If dedicated RLF timers and constants are enabled */
  boolean enable;

  /* Dedicated timers and constants */
  lte_cphy_tmr_and_const_param_s tmr_and_const;

} lte_cphy_rlf_tmr_and_const_dedicated_param_s;

/*! @brief
    Idle DRX information needed for CSI-RS from SIB2
*/
typedef struct
{
  /*! UE paging cycle in milli-seconds */
  uint16 paging_cycle;

  /*! nb value - used to derive the number of paging groups */
  lte_l1_nb_e nb;

} lte_cphy_pcch_cfg_common_param_s;

/*! @brief
        PRACH Cfg from SIB2
*/
typedef struct
{
  /*! HST (High speed train) flag from SIB2*/
  boolean high_speed_flag;
} lte_cphy_prach_cfg_common_param_s;


/*!
  @brief
  An enum describing the bit positions in the lte_cphy_common_cfg_mask_t
  */
enum
{
  LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG = 0x01,
  LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG = 0x02,
  LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG = 0x04,
  LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG = 0x08,
  LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG = 0x10,
  LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL = 0x20,
  LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG = 0x40,
  LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH = 0x80,
  LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG = 0x100,
  LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG = 0x200,
  LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM = 0x400,
  //! @todo TDD: DELETE OBSOLETE
  LTE_CPHY_COMMON_CFG_MASK_BIT_TDD_CFG = 0x0800,
  LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_AREAS_MCCH_SCHDL_CFG = 0x1000,
  LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_AREA_CFG = 0x2000,
  LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG = 0x4000,
  LTE_CPHY_COMMON_CFG_MASK_BIT_PRACH_CFG = 0x8000,
  LTE_CPHY_COMMON_CFG_CURRENT_VALID_MASK_BITS = 0xFFFF
};


/*! @brief
  Channel config mask to enable the channel configuration changes. Only used
  in CPHY interface.

  @see lte_cphy_common_cfg_mask_bit_e for a description of the bits
       in this mask.
*/
typedef uint16 lte_cphy_common_cfg_mask_t;

/*!
  @brief The parameters for the l1 Scell common config request.
  */
typedef struct
{
  /*! Cell   characteristics dl-Bandwidth-r10  ENUMERATED   {n6 ,    n15 ,    n25 ,    n50 ,    n75 ,    n100}*/
  lte_bandwidth_e dl_bandwidth;

  /*! antennaPortsCount  ENUMERATED   {an1 ,    an2 ,    an4 ,    spare1} */
  lte_cphy_antenna_cfg_common_param_s antenna_ports;

  /*! MBSFN-SubframeConfigList  Defines the subframes that are reserved for MBSFN in downlink. */
  lte_cphy_mbsfn_cfg_s mbsfn_subframe_cfg;
  
  /*! phich-Config*/
  lte_l1_phich_cfg_param_s phich_cfg;

  /*! Parameters required to configure PDSCH physical channel */
  lte_cphy_pdsch_cfg_common_param_s pdsch_cfg_common;

  /*! Parameters which indicates tdd_cfg is valid*/
  boolean tdd_cfg_valid;

  /*! Parameters required to configure TDD Config */
  lte_cphy_tdd_cfg_param_s tdd_cfg; 
} lte_cphy_common_scell_dl_cfg_s;

/*!
  @brief The parameters for the l1 Scell common UL config request.
  */
typedef struct
{
  /*! flag to enable or disable Scell common UL config */
  boolean enable;


  /*! flag which indicate if  ul_cell_frequency is presnt or not*/
  boolean ul_cell_frequency_present;

  /*! The frequency the configuration applies to or the source cell in case of handover */
  lte_earfcn_t ul_cell_frequency;
  
  /*! flag which indicate if  ul_bandwidth is presnt or not*/
  boolean ul_bandwidth_present;

  /*! Cell   characteristics dl-Bandwidth-r10  ENUMERATED   {n6 ,    n15 ,    n25 ,    n50 ,    n75 ,    n100}*/
  lte_bandwidth_e ul_bandwidth;

  /*! AdditionalSpectrumEmission   ::=   INTEGER   (1 .  . 32 ) */
  uint8 add_spectrum_emission_scell;

  /*! P-Max   ::=   INTEGER   (-30 .  . 33 )  */
  int8 p_max;


} lte_cphy_common_scell_ul_cfg_s;


/*!
  @brief The parameters for the l1 Scell common config request.

  */
typedef struct
{
  lte_cphy_common_scell_dl_cfg_s scell_dl_cfg;
  lte_cphy_common_scell_ul_cfg_s scell_ul_cfg;
} lte_cphy_common_scell_cfg_param_s;


/*!
  @brief The parameters for the l1 common config request.

  This sets up the common parameters received in the SIBs.  This command
  can be used any time after a cell has been acquired.
  */
typedef struct
{
  /*! Mask of the commands to be configured (or setup)*/
  lte_cphy_common_cfg_mask_t cfg_mask;
  /*! Parameters required to ul frequency cfg info */
  lte_cphy_ul_freq_cfg_s ul_freq_cfg;
  /*! Parameters required to configure PDSCH physical channel */
  lte_cphy_pdsch_cfg_common_param_s pdsch_cfg_common;
  /*! Parameters required to configure PUCCH physical channel */
  lte_cphy_pucch_cfg_common_param_s pucch_cfg_common;
  /*! Parameters required to configure PUSCH physical channel */
  lte_cphy_pusch_cfg_common_param_s pusch_cfg_common;
  /*! Parameters required to configure UL Sounding Reference Signal */
  lte_cphy_srs_cfg_common_param_s srs_cfg_common;
  /*! Parameters required to configure UL Power Control information */
  lte_cphy_ul_power_ctrl_common_param_s ul_power_ctrl_common;
  /*! Parameters required to configure UL prefix cyclic length */
  lte_cphy_ul_cyclic_prefix_length_e ul_cyclic_prefix_length;
  /*! Parameters required to configure MBSFN subframes */
  lte_cphy_mbsfn_cfg_s mbsfn_cfg;
  /*! Cell selection params: required for S-criteria check */
  lte_cphy_cell_select_cfg_s cell_selection_cfg;
  /*! UE timers and constant params: required for RLF detection/recovery */
  lte_cphy_tmr_and_const_param_s tmr_and_const_param;
  //! @todo TDD: DELETE OBSOLETE
  /*! Parameters required to configure TDD Config */
  lte_cphy_tdd_cfg_param_s tdd_cfg;

  /* ML1 needs to know all PMCH (MTCH/MCCH) related config to determine 
     PRS conflicting subframes with PMCH even we are not monitoring any */
  /*! Parameters required to acquire MBMS control information */
  lte_cphy_mbsfn_areas_mcch_schdl_cfg_s    *mbsfn_areas_mcch_schdl_cfg_ptr;
  /*! Parameters required to find allocated PMCHs  */
  lte_cphy_mbsfn_areas_cfg_s               *mbsfn_areas_cfg_ptr;
  /*! Parameters required to configure Rel-10 CSI-RS from SIB2*/
  lte_cphy_pcch_cfg_common_param_s       pcch_cfg_common;
  /*! PRACH Parameters from SIB2*/
  lte_cphy_prach_cfg_common_param_s      prach_cfg_common;
} lte_cphy_common_cfg_req_param_s;

/*!
  @brief
  An enum describing the bit positions in the
  lte_cphy_common_mobility_cfg_mask_t
  */
enum
{
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_FREQ_CFG = 0x0001,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PDSCH_CFG = 0x0002,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PUCCH_CFG = 0x0004,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PUSCH_CFG = 0x0008,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_SRS_CFG = 0x0010,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_POWER_CTRL = 0x0020,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PHICH_CFG = 0x0040,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_ANTENNA_CFG = 0x0080,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH = 0x100,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_TDD_CFG = 0x0200,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_P_MAX = 0x0400,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_NCELL_CFG = 0x0800,
  LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PRACH_CFG = 0x8000,
  /*! UPDATE the value below whenever any of the above bitmasks change */
  LTE_CPHY_COMMON_MOBILITY_CFG_CURRENT_VALID_MASK_BITS = 0x0FFF
};

/*! @brief
  Channel config mask to enable the channel configuration changes. Only used
  in CPHY interface.

  @see lte_cphy_common_mobility_cfg_mask_bit_e for a description of the bits
       in this mask.
*/
typedef uint16 lte_cphy_common_mobility_cfg_mask_t;

/*!
  @brief The common mobility parameters passed upon handover.

  This is a union of the parameters detected and found on the PBCH and
  the ones found on the SIB.  Any parameters not passed in this request
  will default to the ones on the current serving cell.

  */
typedef struct
{
  /*! Mask of the commands to be configured (or setup)*/
  lte_cphy_common_mobility_cfg_mask_t cfg_mask;
  /*! Target Cell uplink information */
  lte_cphy_ul_freq_cfg_s ul_freq_cfg;
  /*! Parameters required to configure PDSCH physical channel */
  lte_cphy_pdsch_cfg_common_param_s pdsch_cfg_common;
  /*! Parameters required to configure PUCCH physical channel */
  lte_cphy_pucch_cfg_common_param_s pucch_cfg_common;
  /*! Parameters required to configure PUSCH physical channel */
  lte_cphy_pusch_cfg_common_param_s pusch_cfg_common;
  /*! Parameters required to configure UL Sounding Reference Signal */
  lte_cphy_srs_cfg_common_param_s srs_cfg_common;
  /*! Parameters required to configure UL Power Control information */
  lte_cphy_ul_power_ctrl_common_param_s ul_power_ctrl_common;
  /*! Parameters required to configure PHICH */
  lte_l1_phich_cfg_param_s phich_cfg;
  /*! Parameters required to configure common antenna info*/
  lte_cphy_antenna_cfg_common_param_s antenna_cfg_common;
  /*! Parameters required to configure UL prefix cyclic length */
  lte_cphy_ul_cyclic_prefix_length_e ul_cyclic_prefix_length;
  /*! Parameters required to configure TDD Config */
  lte_cphy_tdd_cfg_param_s tdd_cfg;
  /*! P_max value required for Uplink transmissions, maximum value for output
    power that the UE should use */
  int8 p_max;
  /*! neighCellConfig : Provides information related to MBSFN and TDD UL/DL configuration
   of neighbour cells of this frequency  */
  uint8 neighbourCellConfig;
  /*! PRACH Parameters */
  lte_cphy_prach_cfg_common_param_s      prach_cfg_common;

} lte_cphy_common_mobility_req_param_s;


/*!
  @brief
  An enum describing the bit positions in the
  lte_cphy_dedicated_cfg_mask_t
  */
enum
{
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_PDSCH_CFG = 0x0001,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUCCH_CFG = 0x0002,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUSCH_CFG = 0x0004,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_SRS_CFG = 0x0008,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_UL_POWER_CTRL = 0x0010,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_CQI_CFG = 0x0020,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_ULSCH_CFG = 0x0040,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_DRX_CFG   = 0x0080,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_ANTENNA_CFG = 0x0100,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_TPC_PDCCH_CFG_PUSCH = 0x0200,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_TPC_PDCCH_CFG_PUCCH = 0x0400,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_SCHEDULING_REQ = 0x0800,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_UE_RELATED_CFG = 0x1000,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_SPS_CFG = 0x2000,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_PHR_CFG = 0x4000,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_TMR_AND_CONST_PARAM = 0x8000,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_SCELL_CFG = 0x10000,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_PCELL_REL10_CFG = 0x20000,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_CSI_RS_CFG = 0x40000,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_SUBFR_PATTERN_PCELL = 0x80000,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_NEIGH_CELL_CRS_CFG = 0x100000,
  LTE_CPHY_DEDICATED_CFG_MASK_BIT_SUBFR_PATTERN_INTRA_NCELL = 0x200000,
  /*! UPDATE the value below whenever any of the above bitmasks change */
  LTE_CPHY_DEDICATED_CFG_CURRENT_VALID_MASK_BITS = 0x3FFFFF
} ;

/*! @brief
  Channel config mask to enable the channel configuration changes. Only used
  in CPHY interface.

  @see lte_cphy_common_mobility_cfg_mask_bit_e for a description of the bits
       in this mask.
*/
typedef uint32 lte_cphy_dedicated_cfg_mask_t;

typedef struct
{
  /*! Field to indicated if CSR RS disabled or enabled */
  boolean csi_rs_enabled;

  /*Below info valid only if csi_rs_enabled is TRUE*/
   /*antennaPortsCount-r10           ENUMERATED   {an1 ,    an2 ,    an4 ,    an8}*/
  lte_l1_antenna_info_ports_count_e antenna_port;
  /* resourceConfig-r10                  INTEGER   (0 .  . 31 )*/
  uint8 resource_config;
  /*subframeConfig-r10               INTEGER   (0 .  . 154 )*/
  uint8 subframeconfig;
  /*p-C-r10                              INTEGER   (-8 .  . 15 )*/
  int8 p_c;
} lte_cphy_dedicated_csi_rs_s;

typedef struct
{
  /*! Field to indicated if zero_tx_power_resource disabled or enabled */
  boolean zero_tx_power_resource_enabled;

  /*Valid only if zero_tx_power_resource_enabled is TRUE*/
  /*zeroTxPowerResourceConfigList-r10    BIT   STRING   (SIZE   (16 )  )*/
  uint16 zero_tx_power_resource_config_list;

  /*zeroTxPowerSubframeConfig-r10        INTEGER   (0 .  . 154 )*/
  uint8 zero_tx_subfm_config_list;
} lte_cphy_dedicated_zero_tx_power_csi_s;

/*!
  @brief
  An enum describing the bit positions in the
  add_mod_cfg_mask
  */
enum
{
  LTE_CPHY_DEDICATED_CSI_RS_CFG_MASK_BIT_CSI_RS_CFG             = 0x0001,
  LTE_CPHY_DEDICATED_CSI_RS_CFG_MASK_BIT_ZERO_PW_CSI_CFG        = 0x0002
} ;

/*! @brief
    Parameters specific to a CSI-RS cfg  (ref: 36.331, 6.3.2)
*/
typedef struct
{
  /*cfg mask which will indicate if csi_rs/zero_tx_power_csi added or modified*/
  uint8 add_mod_cfg_mask;

  /*CSI-RS cfg*/
  lte_cphy_dedicated_csi_rs_s csi_rs;

  /*ZeroPowerCSI-RS cfg*/
  lte_cphy_dedicated_zero_tx_power_csi_s zero_tx_power_csi;
} lte_cphy_dedicated_csi_rs_config_s;

/*! @brief
    Parameters specific to a Scell/Pcell  (ref: 36.331, 6.2.2)
*/
typedef struct
{
  /*transmission_mode_enabled is TRUE only then transmission_mode is valid*/
  boolean transmission_mode_enabled;

  /*! Information about the antenna transmission mode for UL, only TM1 & TM2 are valid as per v10.5 36.331 */
  lte_l1_dl_tx_mode_e transmission_mode;

  /*RRC only sets this to TRUE when NW includes I.E fourAntennaPortActivated-r10 otherwise it will be set to FALSE*/
  boolean four_antenna_port_activated;
} lte_cphy_ul_antenna_info_rel10_cfg_s;

/*!
  @brief The parameters for additionalSpectrumEmissionPCell
  */
typedef struct
{
  /*! flag to enable or disable Scell common UL config */
  boolean enable;

  /*! The UE requirements related to IE AdditionalSpectrumEmissionPCell are defined in TS 36.101 [42]. 
  EUTRAN does not configure AdditionalSpectrumEmissionPCell if there are no other serving cells configured.
  E-UTRAN does not configure the field in case of contiguous intra-band carrier aggregation. */
  uint8 add_sp_emission;

} lte_cphy_pcell_add_sp_emission_cfg_s;


/*!
  @brief
  An enum describing the bit positions in the
  pcell_cfg_mask
  */
enum
{
  LTE_CPHY_DEDICATED_PCELL_UL_CFG_MASK_BIT_ANTENNA_CFG             = 0x0001,
  LTE_CPHY_DEDICATED_PCELL_MASK_BIT_ADD_SP_EMISSION_CFG            = 0x0002
} ;

/*! @brief
    Parameters specific to a Pcell  (ref: 36.331, 6.2.2)
*/
typedef struct
{
  /*!when set to TRUE for a given serving cell, the UE shall use the carrier indicator field value from the detected 
  PDCCH with uplink DCI format to determine the serving cell for the corresponding PUSCH transmission. 
  this will enable reading  Carrier indicator 0 or 3 bits from DCI formats see 36.212 5.3.3.1 &  36.213 8.0*/
  boolean cif_presence;

  boolean sr_pucch_resource_idx_p1_valid;
  uint16 sr_pucch_resource_idx_p1;

  uint8 pcell_cfg_mask;
  
  /*! Information about the antenna*/
  lte_cphy_ul_antenna_info_rel10_cfg_s ul_antenna_cfg;

  /*! Parameters required to configure AdditionalSpectrumEmissionsPcell information. */
  lte_cphy_pcell_add_sp_emission_cfg_s pcell_add_sp_em;
} lte_cphy_dedicated_pcell_rel10_cfg_s;

  
/*!
  @brief
  An enum for Cross carrier  scheduling type
 */
typedef enum
{
  /*No cross carrier  scheduling  */
  LTE_CPHY_SCELL_NO_CC_SCHED,
  /*Cross carrier  scheduling  */
  LTE_CPHY_SCELL_CC_SCHED
} lte_cphy_scell_cc_sched_type_e;

/*!
  @brief
  NO Cross carrier scheduling params
  */
typedef struct
{
  boolean cif_presence;
} lte_cphy_scell_no_cc_sched_cfg_s;

/*!
  @brief
   Cross carrier scheduling params
  */
typedef struct
{
  /*SCellIndex-r10   ::=   INTEGER   (0 .  . 7 ), Note:- Value 0 means Pcell*/
  uint8 scell_id;

  /*SCellIndex-r10   ::=   INTEGER   (1 .  . 4 ) */
  uint8 pdsch_start;

}lte_cphy_scell_cc_sched_cfg_s;

/*!
  @brief
   Cross carrier scheduling params
  */
typedef struct
{
  /*! Parameters specify  cross carrier  scheduling eanbled or not */
  lte_cphy_scell_cc_sched_type_e cc_sched_type;

  /*! Parameters specify param for NO cross carrier scheduling */
  lte_cphy_scell_no_cc_sched_cfg_s no_cc_sched_cfg;
  
  /*! Parameters specify param for cross carrier scheduling */
  lte_cphy_scell_cc_sched_cfg_s  cc_sched_cfg;
} lte_cphy_dedicated_scell_cc_sched_cfg_s;

/*!
  @brief
  An enum describing the bit positions in the
  scell_dl_cfg_mask
  */
enum
{
  LTE_CPHY_DEDICATED_SCELL_DL_CFG_MASK_BIT_ANTENNA_CFG             = 0x0001,
  LTE_CPHY_DEDICATED_SCELL_DL_CFG_MASK_BIT_PDSCH_CFG               = 0x0002,
  LTE_CPHY_DEDICATED_SCELL_DL_CFG_MASK_BIT_CSI_RS_CFG              = 0x0004,
  LTE_CPHY_DEDICATED_SCELL_DL_CFG_MASK_BIT_CROSS_CARRIER_SCHED_CFG = 0x0008
} ;

/*! @brief
    Dedicated params  for Scell(ref: 36.331)
*/
typedef struct
{
  uint8 scell_dl_cfg_mask;

  /*! Parameters required to configure dedicated Antenna information. */
  lte_cphy_antenna_cfg_dedicated_param_s antenna_cfg_dedicated;

  /*! Parameters required to configure crossCarrierScheduling */
  lte_cphy_dedicated_scell_cc_sched_cfg_s  scell_cc_sched_cfg;
  
  /*! Parameters required to configure dedicated csi_rs information. */
  lte_cphy_dedicated_csi_rs_config_s csi_rs_cfg_dedicated;

  /*! Parameters required to configure PDSCH physical channel */
  lte_cphy_pdsch_cfg_dedicated_param_s pdsch_cfg_dedicated;
} lte_cphy_dedicated_scell_dl_cfg_s;

/*! @brief
   CRS-AssistanceInfo-r11
*/
typedef struct
{
  /*! Physical cell id */
  lte_phy_cell_id_t cell_id;

  /*! antennaPortsCount */
  lte_l1_antenna_info_ports_count_e antenna_ports_count;

  /*! MBSFN-SubframeConfigList*/
  lte_cphy_mbsfn_cfg_s mbsfn_subframe_cfg;

}lte_cphy_crs_assistance_param_s;


/*! @brief
   Neighbor Cell CRS Assistance Info
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;

  /*! Number of CRS assistance Info cell reports */
  uint8 num_crs_rpt;

  /*! CRS assistance Info */
  lte_cphy_crs_assistance_param_s crs_assistance_info[LTE_CPHY_MAX_CRS_ASSIST_INFO_CELL_REPORT];
  
}lte_cphy_neigh_cell_crs_cfg_param_s;



/*!
  @brief
  An enum describing pathlossReferenceLinking
  */
typedef enum
{
  LTE_CPHY_P_CELL ,
  LTE_CPHY_S_CELL
} lte_l1_path_loss_reference_linking_e;

/*! @brief
    UL power control dedicated params  for Scell(ref: 36.331)
*/
typedef struct {
  /*! P0_UE value for PUSCH. Range: -8..7 in dB - Field applicable for non
      persistent scheduling only
  */

  lte_l1_ul_power_ctrl_p0_ue_t p0_ue_pusch;

  /*! Specifies whether or not mcs is enabled */
  lte_l1_ul_power_ctrl_delta_mcs_enable_e delta_mcs_enabled;

  /*! Provides information if accumulation of PUSCH TPC commands is enabled
      or disabled
  */
  boolean accum_enabled;

  /*! pSRS offset 0..15 */
  lte_l1_ul_power_ctrl_psrs_offset_t p_srs_offset;

  /*L1 to look at p_srs_offset_Ap only when p_srs_offset_Ap_present is set to TRUE*/
  boolean p_srs_offset_Ap_present;
  /*! pSRS offset 0..15 */
  lte_l1_ul_power_ctrl_psrs_offsetAp_t p_srs_offset_Ap;

  /*! Filter coefficient used to determine the filtered RSRP which is used in
    pathloss computation, DEFAULT   fc4  */
  lte_l1_ul_power_ctrl_filter_coefficient_e filter_coefficient;

  /*! path loss Reference Linking*/
  lte_l1_path_loss_reference_linking_e path_loss_reference_linking;

} lte_cphy_ul_power_ctrl_dedicated_scell_param_s;


typedef struct
{
  /*RRC only sets this to TRUE when NW includes I.E groupHoppingDisabled-r10  otherwise it will be set to FALSE*/
  boolean  group_hopping_disabled;
  /*RRC only sets this to TRUE when NW includes I.E dmrs-WithOCC-Activated-r10  otherwise it will be set to FALSE*/
  boolean  dmrs_withOCC_activated;
} lte_cphy_dedicated_scell_pusch_config_s;



/*!
  @brief
  An enum describing the bit positions in the
  scell_dl_cfg_mask
  */
enum
{
  LTE_CPHY_DEDICATED_SCELL_UL_CFG_MASK_BIT_ANTENNA_CFG             = 0x0001,
  LTE_CPHY_DEDICATED_SCELL_UL_CFG_MASK_BIT_PUSCH_CFG               = 0x0002,
  LTE_CPHY_DEDICATED_SCELL_UL_CFG_MASK_BIT_PWD_CTRL_CFG            = 0x0004,
  LTE_CPHY_DEDICATED_SCELL_UL_CFG_MASK_BIT_CQI_CFG                 = 0x0008,
  LTE_CPHY_DEDICATED_SCELL_UL_CFG_MASK_BIT_SRS_CFG                 = 0x0010
} ;

/*! @brief
    Parameters specific to a Scell  (ref: 36.331, 6.2.2)
*/
typedef struct
{
  uint8 scell_ul_cfg_mask;
  
  /*antenna_info_valid is TRUE only then transmission_mode is valid*/
  boolean antenna_info_valid;
  /*! Information about the antenna*/
  lte_cphy_ul_antenna_info_rel10_cfg_s scell_ul_antenna_cfg;

  /*!cphy dedicated scell pusch config*/
  lte_cphy_dedicated_scell_pusch_config_s cphy_pusch_cfg;
 
  /*! Ul ul_power ctrl scell*/
  lte_cphy_ul_power_ctrl_dedicated_scell_param_s ul_power_ctrl_scell;
  
  /*! Scell CQI reporting param*/
  lte_cphy_cqi_reporting_param_s cqi_reporting_cfg;

  /*! Parameters required to configure UL Sounding Reference Signal */
  lte_cphy_srs_cfg_dedicated_param_s srs_cfg_dedicated;

  /*! Parameters required to configure UL Sounding Reference Signal aperiodic*/
  lte_cphy_srs_cfg_dedicated_aperiodic_param_s srs_aperiodic_ded_cfg;

} lte_cphy_dedicated_scell_ul_cfg_s;

/*! @brief
    Parameters specific to a Scell  (ref: 36.331, 6.2.2)
*/
typedef struct
{
  /*! Parameters required to configure Scell */
  lte_cphy_dedicated_scell_ul_cfg_s ul_cfg;

  /*! Parameters required to configure Scell */
  lte_cphy_dedicated_scell_dl_cfg_s dl_cfg;

} lte_cphy_dedicated_scell_add_reconfig_s;

/*!
  @brief
  An enum describing the bit positions in the
  scell_cfg_bitmask
  */
enum
{
  /*! @brief  scell Identification info cell_id, earfcn included  */
  LTE_CPHY_SCELL_IDENTIFICATION_ADD_MOD_INFO_INCLUDED  =  0x1,
  /*! @brief  scell common info  included  */
  LTE_CPHY_SCELL_COMMON_ADD_MOD_INFO_INCLUDED          =  0x2,
  /*! @brief  scell dedicated info included  */
  LTE_CPHY_SCELL_DEDICATED_ADD_MOD_INFO_INCLUDED       =  0x4
} ;

/*! @brief
    Parameters specific to a Scell (ref: 36.331, 6.2.2)
*/
typedef struct
{
  /*SCellIndex-r10   ::=   INTEGER   (1 .  . 7 )*/
  uint8 scell_id;

  /*scell_info_add_mod_bitmask will tell which of below config included*/
  uint8 scell_info_add_mod_bitmask;

  /*! scell Identification info cell_id, earfcn, valid only if LTE_CPHY_SCELL_IDENTIFICATION_INFO_INCLUDED
  is included in scell_info_add_mod_bitmask, RRC will set this at Scell addition, for Scell modification it may not be set L1 can continue using previous info*/
  lte_cphy_cell_info_s scell_id_info;

  /*! scell common cfg, valid only if LTE_CPHY_SCELL_COMMON_INFO_INCLUDED
  is included in scell_info_add_mod_bitmask, 
  RRC will set this at Scell addition, 
  for Scell modification it may not be set L1 can continue using previous info */
  lte_cphy_common_scell_cfg_param_s scell_common_cfg;

  /*! scell dedicated cfg , valid only if LTE_CPHY_SCELL_DEDICATED_INFO_INCLUDED
  is included in scell_info_add_mod_bitmask
  RRC will set this at Scell addition, for Scell modification 
  */
  lte_cphy_dedicated_scell_add_reconfig_s scell_dedicated_cfg;
} lte_cphy_scell_cfg_param_per_scell_s;

/*! @brief 
    Enumeration defines Scell Deactivation Timer in period in radio frames
    sCellDeactivationTimer
    SCell deactivation timer in TS 36.321 [6]. Value in number of radio frames. Value rf4 corresponds to 4 radio frames, 
    value rf8 corresponds to 8 radio frames and so on. E-UTRAN only configures the field if the UE is configured with 
    one or more SCells. If the field is absent, the UE shall delete any existing value for this field and assume the value 
    to be set to infinity. The same value applies for each SCell (although the associated functionality is performed 
    independently for each SCell). 
*/
typedef enum
{
  LTE_CPHY_SCELL_DEACTIVATION_TIMER_2RF = 2,
  LTE_CPHY_SCELL_DEACTIVATION_TIMER_4RF = 4,
  LTE_CPHY_SCELL_DEACTIVATION_TIMER_8RF = 8,
  LTE_CPHY_SCELL_DEACTIVATION_TIMER_16RF = 16,
  LTE_CPHY_SCELL_DEACTIVATION_TIMER_32RF = 32,
  LTE_CPHY_SCELL_DEACTIVATION_TIMER_64RF = 64,
  LTE_CPHY_SCELL_DEACTIVATION_TIMER_128RF = 128,
  LTE_CPHY_SCELL_DEACTIVATION_TIMER_INFINITY = 256
} lte_cphy_scell_deact_timer_e;

/*! @brief
    Parameters required to configure Scell's for Carrier Aggregation feature (ref: 36.331, 6.2.2)
*/
typedef struct
{
  /*! Bitmask which indicates which Scell config have been deleted SCellIndex-r10   ::=   INTEGER   (1 .  . 7 ).
  This has to be interpreted as below
  bit number:-<MSB> 8(unused)  7  6  5  4  3  2  1 <LSB>
  scell_id                                    7.....................1      
  For example scell_id_delete_list_bitmask = 0x3, means scell_id = 1 & 2 needs to be released*/
  uint32 scell_id_delete_list_bitmask;
  /*! Indicates number of entries in scell_add_mod_list valid*/
  uint8 num_scell_add_mod_list;
  /*! Parameters required to configure Scell's for Carrier Aggregation feature*/
  lte_cphy_scell_cfg_param_per_scell_s scell_cfg_list[LTE_CPHY_MAX_S_CELL];

  lte_cphy_scell_deact_timer_e scell_deact_timer;/*!<Scell Deactivation Timer in period in radio frames */
} lte_cphy_scell_cfg_param_s;

/*! @brief Cphy Measurement subframe pattern for neighbor
*/
typedef struct
{
  boolean enabled;

  /*! Time domain resource restriction pattern applicable to neighbour cell 
      RSRP and RSRQ measurements on the carrier frequency indicated by carrierFreq*/
  lte_l1_subfm_pattern_cfg_param_s meas_subfm_pattern_neigh;

  /*! List of cells for which measSubframePattern-Neigh is applied*/
  uint8 num_cells_list;
  lte_phy_cell_id_and_range_s meas_subfm_cell_list[LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS];

} lte_cphy_meas_subfm_patter_neigh_s;


/*!
  @brief The parameters for the dedicated configuration request.

  The common parameters must be configured prior to receiving these
  parameters.
  */
typedef struct
{
  /*! Mask of the commands to be configured (or setup)*/
  lte_cphy_dedicated_cfg_mask_t cfg_mask;
  /*! Parameters required to configure PDSCH physical channel */
  lte_cphy_pdsch_cfg_dedicated_param_s pdsch_cfg_dedicated;
  /*! Parameters required to configure PUCCH physical channel */
  lte_cphy_pucch_cfg_dedicated_param_s pucch_cfg_dedicated;
  /*! Parameters required to configure PUSCH physical channel */
  lte_cphy_pusch_cfg_dedicated_param_s pusch_cfg_dedicated;
  /*! Parameters required to configure UL Sounding Reference Signal */
  lte_cphy_srs_cfg_dedicated_param_s srs_cfg_dedicated;
  /*! Parameters required to configure UL Power Control information */
  lte_cphy_ul_power_ctrl_dedicated_param_s ul_power_ctrl_dedicated;
  /*! Parameters required to configure CQI reporting  */
  lte_cphy_cqi_reporting_param_s cqi_cfg;
  /*! Parameters required to configure the uplink shared channel */
  lte_cphy_ulsch_cfg_dedicated_param_s    ulsch_cfg;
  /*! The parameters for connected-mode drx parameters */
  lte_cphy_drx_cfg_dedicated_param_s      con_drx_cfg;
  /*! Parameters required to configure dedicated Antenna information. */
  lte_cphy_antenna_cfg_dedicated_param_s antenna_cfg_dedicated;
  /*! Parameters required to configure TPC for PUCCH  */
  lte_cphy_tpc_pdcch_cfg_s tpc_pdcch_cfg_pucch;
  /*! Parameters required to configure TPC for PUSCH  */
  lte_cphy_tpc_pdcch_cfg_s tpc_pdcch_cfg_pusch;
  /*! Parameters required to configure scheduling request parameters  */
  lte_cphy_scheduling_req_cfg_type scheduling_req_cfg;
  /*! Parameters required to configure UE related configuration */
  lte_cphy_ue_related_cfg_s ue_related_cfg;
  /*! Parameters required to configure Semi Persistent Scheduling info  */
  lte_cphy_sps_cfg_s sps_cfg;
  /*! Parameters required to configure Uplink Power headroom  */
  lte_cphy_phr_cfg_dedicated_param_s phr_cfg;
  /*! Dedicated RLF timers and constant params: used for RLF detection/recovery */
  lte_cphy_rlf_tmr_and_const_dedicated_param_s rlf_tmr_and_const_param;
  /*! Parameters required to configure Scell's for Carrier Aggregation feature */
  lte_cphy_scell_cfg_param_s scell_config;
  /*! Parameters required to configure dedicated pcell rel-10 information. */
  lte_cphy_dedicated_pcell_rel10_cfg_s pcell_rel10_cfg;
  /*! Parameters required to configure dedicated csi_rs information. */
  lte_cphy_dedicated_csi_rs_config_s csi_rs_cfg;
  /*! Time domain resource restriction pattern for the serving cell measurements 
      (RSRP, RSRQ and the radio link monitoring),*/
  lte_l1_subfm_pattern_cfg_param_s meas_subfm_pattern_pcell;
  /*! Time domain resource restriction pattern applicable to intra-freq neighbour cell 
      RSRP and RSRQ measurements */
  lte_cphy_meas_subfm_patter_neigh_s meas_subfm_pattern_intra_neigh;
  /*! Neighbor Cell CRS Assistance Info Configuration*/
  lte_cphy_neigh_cell_crs_cfg_param_s neigh_cell_crs_cfg;
} lte_cphy_dedicated_cfg_req_param_s;

/*!
  @brief
  An enum describing the bit positions in the lte_cphy_sib_mask_t

  CHANGE SIB MASKS IN LL1/intf/intf_dl_si.h INTERFACE FILE IF NEED TO CHANGE HERE
  */
enum
{
  LTE_CPHY_SIB_MASK_BIT_MIB=0x1,
  LTE_CPHY_SIB_MASK_BIT_SIB1=0x2,
  LTE_CPHY_SIB_MASK_BIT_SIB2=0x4,
  LTE_CPHY_SIB_MASK_BIT_SIB3=0x8,
  LTE_CPHY_SIB_MASK_BIT_SIB4=0x10,
  LTE_CPHY_SIB_MASK_BIT_SIB5=0x20,
  LTE_CPHY_SIB_MASK_BIT_SIB6=0x40,
  LTE_CPHY_SIB_MASK_BIT_SIB7=0x80,
  LTE_CPHY_SIB_MASK_BIT_SIB8=0x100,
  LTE_CPHY_SIB_MASK_BIT_SIB9=0x200,
  LTE_CPHY_SIB_MASK_BIT_SIB10=0x400,
  LTE_CPHY_SIB_MASK_BIT_SIB11=0x800,
  LTE_CPHY_SIB_MASK_BIT_SIB12=0x1000,
  LTE_CPHY_SIB_MASK_BIT_SIB13=0x2000,
  LTE_CPHY_SIB_MASK_BIT_SIB14=0x4000,
  LTE_CPHY_SIB_MASK_BIT_SIB15=0x8000,
  LTE_CPHY_SIB_MASK_BIT_SIB16=0x10000,
  LTE_CPHY_SIB_MASK_VALID_BITS_MASK = 0x1FFFF
};

/*! @brief Bitmask that defines System Information Block numbers.

  @see lte_cphy_sib_mask_bit_e for the bit positions in this mask
 */
typedef uint32 lte_cphy_sib_mask_t;

/*! @brief SI message reception window length in ms; used in SIB Sched Req
*/
typedef enum
{
  /** 1 ms */
  LTE_CPHY_SI_WINDOW_LENGTH_MS_1,
  /** 2 ms */
  LTE_CPHY_SI_WINDOW_LENGTH_MS_2,
  /** 5 ms */
  LTE_CPHY_SI_WINDOW_LENGTH_MS_5,
  /** 10 ms */
  LTE_CPHY_SI_WINDOW_LENGTH_MS_10,
  /** 15 ms */
  LTE_CPHY_SI_WINDOW_LENGTH_MS_15,
  /** 20 ms */
  LTE_CPHY_SI_WINDOW_LENGTH_MS_20,
  /** 40 ms */
  LTE_CPHY_SI_WINDOW_LENGTH_MS_40,
  LTE_CPHY_SI_WINDOW_LENGTH_MS_MAX
}lte_cphy_si_window_length_e;


typedef enum
{
  /** 8 radio frames */
  LTE_CPHY_SI_PERIODICITY_RF_8,
  /** 16 radio frames */
  LTE_CPHY_SI_PERIODICITY_RF_16,
  /** 32 radio frames */
  LTE_CPHY_SI_PERIODICITY_RF_32,
  /** 64 radio frames */
  LTE_CPHY_SI_PERIODICITY_RF_64,
  /** 128 radio frames */
  LTE_CPHY_SI_PERIODICITY_RF_128,
  /** 256 radio frames */
  LTE_CPHY_SI_PERIODICITY_RF_256,
  /** 512 radio frames */
  LTE_CPHY_SI_PERIODICITY_RF_512,
  LTE_CPHY_SI_PERIODICITY_RF_MAX
}lte_cphy_si_periodicity_e;

/*! @brief SIB scheduling information; used in SIB Sched Req;
   one per SI message
*/
typedef struct
{
  /** Periodicity of the SI message in radio frames */
  lte_cphy_si_periodicity_e si_periodicity;

  /** Bitmask to indicate the SIBs carried in this SI message. Eg.
  *  if this SI message carries SIB1 and SIB3, bits 1 and 3 will
  *  be set. This bitmask is similar in nature to the "SIB needed"
  *  bitmasks in the SIB Sched Req hence a direct AND
  *  operation of the two fields would determine the need to
  *  decode this SI message */
  lte_cphy_sib_mask_t sib_map_bitmask;
}lte_cphy_si_msg_info_s;

/*! @brief
  Configuration for applying partial roaming restriction (36.304 5.2.4.4).
*/
typedef enum
{
  /*! Default case with no exceptions for restrictions */
  LTE_CPHY_PARTIAL_ROAMING_ALLOW_NONE,

  /*! Allow target frequency during an acq_req after a cell redirection
      from connected mode. */
  LTE_CPHY_PARTIAL_ROAMING_ALLOW_TARGET_FREQ,

  /*! Allow all frequencies during 'any cell selection' */
  LTE_CPHY_PARTIAL_ROAMING_ALLOW_ALL,

  /*! Invalid max value */
  LTE_CPHY_PARTIAL_ROAMING_MAX

} lte_cphy_partial_roaming_restriction_e;



/******************************************************************************
                Cell Reselection indication and cancel requests
*******************************************************************************/

/*! @brief
  CDMA reselection type enum
*/
typedef enum
{
  /*! Normal neighbor reselection */
  LTE_CPHY_CDMA_RESEL_NORMAL,
  /*! CDMA reverse time transfer reselection */
  LTE_CPHY_CDMA_RESEL_REVERSE_TIME_TRANSFER,

} lte_cphy_cdma_resel_type_e;

/*! @brief
 CDMA reselection info union
*/
typedef union
{
  /*! CDMA cell info for normal reselection */
  lte_cphy_cdma_cell_info_s  cell;

  /*! CDMA layer info for reverse time transfer */
  lte_cphy_cdma_layer_info_s layer;

} lte_cphy_cdma_resel_info_u;

/*! @brief
  CDMA reselection info structure
*/
typedef struct
{
  /*! Reselection type */
  lte_cphy_cdma_resel_type_e type;

  /*! Union of cell and layer info for the different reselection types */
  lte_cphy_cdma_resel_info_u info;

} lte_cphy_cdma_resel_info_s;

/*! @brief
  Union of RAT specific cell info
*/
typedef union
{
  /*! LTE specific cell info */
  lte_cphy_cell_info_s  lte_cell_info;

  /*! CDMA (1X or DO) specific cell info */
  lte_cphy_cdma_resel_info_s cdma_resel_info;

  /*! GSM specific cell info */
  lte_cphy_gsm_cell_info_s gsm_cell_info;

  /*! WCDMA specific cell info */
  lte_cphy_wcdma_cell_info_s wcdma_cell_info;

    /*! TDSCDMA specific cell info */
  lte_cphy_tdscdma_cell_info_s tdscdma_cell_info;

  /*! TBD: More params needed like RAT specific target cell info,
      PLMN related etc. (if any)
  */
} lte_cphy_rat_specific_cell_info_u;

/*! @brief
  Cell reselection target cell structure
 */
typedef struct
{
  /*! Priority of the cell */
  lte_l1_resel_priority_t priority;

  /*! RAT type of target cell */
  lte_rat_e    rat_type;

  /*! RAT specific cell info */
  lte_cphy_rat_specific_cell_info_u  rat_cell_info;

} lte_cphy_resel_target_cell_info_s;

/*! @brief
  Parameter struct for cdma channel bar info
*/
typedef struct
{
  /*! RAT type of barred channel */
  lte_rat_e    rat_type;

  /*! CDMA/HDR channel */
  sys_channel_type      channel;

    /*! barred time in seconds*/
  uint64                t_barred;

}lte_cphy_cdma_channel_bar_info_s;

/*! @brief
  Structure to bar 1x/DO channels
 */
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  uint16                     num_bar_channels;

  lte_cphy_cdma_channel_bar_info_s  bar_list[LTE_RRC_MAX_1X_DO_AVOID_CHANNELS];

} lte_cphy_cdma_channel_bar_req_s;

/*! @brief Ref Time Structure
*/
typedef struct
{
  /*! The time in RTC ticks */
  uint64            curr_rtc;

  /*! The SFN at which indication is sent */
  lte_l1_sfn_t      curr_sfn;

  /*! The SubFn count at which indication was sent*/
  lte_l1_subfn_t    curr_subfn;

}lte_ref_time_s;

/*! @brief
  Cell reselection cancel cause.
 */
typedef enum
{
  LTE_CPHY_RESEL_CANCEL_CAUSE_NONE,
  LTE_CPHY_RESEL_CANCEL_CAUSE_S_FAILURE,
  LTE_CPHY_RESEL_CANCEL_CAUSE_SIB_READ_FAILURE,
  LTE_CPHY_RESEL_CANCEL_CAUSE_SIB_READ_TIMEOUT,
  LTE_CPHY_RESEL_CANCEL_CAUSE_DL_WEAK_IND,
  LTE_CPHY_RESEL_CANCEL_CAUSE_CELL_BARRED,
  LTE_CPHY_RESEL_CANCEL_CAUSE_FREQ_BARRED,
  LTE_CPHY_RESEL_CANCEL_CAUSE_NON_EQ_PLMN,
  LTE_CPHY_RESEL_CANCEL_CAUSE_FORBIDDEN_TA,
  LTE_CPHY_RESEL_CANCEL_CAUSE_ABORTED,
  LTE_CPHY_RESEL_CANCEL_CAUSE_FORBIDDEN_CSG_ID,
  LTE_CPHY_RESEL_CANCEL_CAUSE_IRAT_RESEL_FAILURE,
  LTE_CPHY_RESEL_CANCEL_CAUSE_IRAT_GSM_BSIC_MISMATCH,
  LTE_CPHY_RESEL_CANCEL_CAUSE_NOT_CSG,
  LTE_CPHY_RESEL_CANCEL_CAUSE_BAND_CHANGE,
  LTE_CPHY_RESEL_CANCEL_CAUSE_MAX
} lte_cphy_resel_cancel_cause_e;

/*! @brief Enumeration for bar causes
*/
typedef enum
{
  LTE_CPHY_CELL_BAR_UNKNOWN,
  LTE_CPHY_CELL_BAR_SIB_READ_FAILURE,
} lte_cphy_cell_bar_cause_e;

/*! @brief Enumeration for Unbar causes
*/
typedef enum
{
  LTE_CPHY_CELL_UNBAR_ALL,
  LTE_CPHY_CELL_UNBAR_FORBIDDEN_TA,
  LTE_CPHY_CELL_UNBAR_FORBIDDEN_CSG_ID,
  LTE_CPHY_CELL_UNBAR_PLMN_MISMATCH,
} lte_cphy_cell_unbar_cause_e;


/******************************************************************************
                Idle-mode Measurement Configuration
*******************************************************************************/

/*! @brief
  Maximum number of intra-freq whitelisted cells
  Defined in 36.331 Section 6.4
*/
#define LTE_CPHY_MEAS_MAX_CELL_INTRA 16

/*! @brief
  Maximum number of intra-freq blacklisted cells
  Defined in 36.331 Section 6.4
*/
#define LTE_CPHY_MEAS_MAX_CELL_BLACK 16

/*! @brief
  Maximum number of inter-freq whitelisted cells
  Defined in 36.331 Section 6.4
*/
#define LTE_CPHY_MEAS_MAX_CELL_INTER 16

/*! @brief
  Maximum number of EUTRA Carrier Frequencies
  Defined in 36.331 Section 6.4
*/
#define LTE_CPHY_MEAS_MAX_NUM_FREQ 8

/*! @brief
  Maximum allowed Carrier Frequencies (including serving freq) across all RATs
  Defined in 36.133 Section 4.2.2.9 and 8.1.2.1.1
*/
#define LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS 13

/*! @brief
  Maximum allowed Carrier Frequencies (including serving freq) across all RATs
  in Connected mode 
  Serving freq including Scells as well
*/
#define LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS_CONN 13


/*! @brief
TDS Freq per Layer
  */
#define LTE_CPHY_MEAS_NUM_TDSCDMA_FREQ_PER_LAYER 3

/*! @brief
Maximum number of TDS Meas layers in Active list
  */
#define LTE_CPHY_MEAS_MAX_ACTIVE_TDSCDMA_MEAS_LAYERS 2

/*! @brief
Maximum number of TDSCDMA Freqs (in Active list)
1TDS meas layer count = 
  (LTE_CPHY_MEAS_NUM_TDSCDMA_FREQ_PER_LAYER) freq layer count
  */
#define LTE_CPHY_MEAS_MAX_TDSCDMA_FREQS (LTE_CPHY_MEAS_NUM_TDSCDMA_FREQ_PER_LAYER * LTE_CPHY_MEAS_MAX_ACTIVE_TDSCDMA_MEAS_LAYERS)

/*! @brief
Maximum number of TDSCDMA Freqs (in Active list) when L (non serving) and G Meas Objs are not present
*/
#define LTE_CPHY_MEAS_MAX_TDS_FREQS_WO_L_G 12

/*! @brief
Maximum number of All RAT's neighbor frequencies
1 Inter freq meas layer count = 1 freq layer count
1 WCDMA meas layer count = 1 freq layer count
1 TDS meas layer count = LTE_CPHY_MEAS_NUM_TDSCDMA_FREQ_PER_LAYER freq layer count
1 CDMA meas layer count = 1 freq layer count
1 GSM freq grp layer count = 1 freq layer count
  */
#define LTE_CPHY_MEAS_ALL_RAT_NEIGH_FREQS 22


/*! @brief
  Maximum number of CDMA band classes
  Defined in 36.331 Section 6.3
*/
#define LTE_CPHY_MEAS_MAX_CDMA_BAND_CLASS 32

/*! @brief
  Maximum number of 1X neighbor cells
  Defined in 36.331 Section 6.3
*/
#define LTE_CPHY_MEAS_MAX_CDMA_1X_NEIGH_CELLS_REL9 40

/*! @brief
  Maximum number of CDMA neighbor cells possible in Rel 9
  Defined in 36.331 Section 6.3
*/
#define LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS_REL9 40

/*! @brief
  Maximum CDMA neighbor cells in Rel 8
  Defined in 36.331 Section 6.3
*/
#define LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS 32

/*! @brief Maximum number of GERAN frequency groups to be measured in
    idle mode.

  Defined in 36.331 Section 6.4
  */
#define LTE_CPHY_MEAS_MAX_GERAN_FREQ_GROUPS 16

/*! @brief Maximum number of GERAN frequencies in a frequency group

  Defined in 36.331 Section 6.4
  */
#define LTE_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP 32

/*! @brief Maximum number of GERAN frequencies supported by ML1.

  This is based upon the minimum required number of frequencies defined
  in 36.133 Section 4.2.2.9.

  */
#define LTE_CPHY_MEAS_MAX_GERAN_FREQ 32

/*! @brief Maximum number of Inter Freq L Meas Objs
  */
#define LTE_CPHY_MEAS_MAX_LTE_INTER_FREQ_MEAS_OBJ 8

/*! @brief Maximum number of LTE Layers in NPG in Connected
  */
#define LTE_CPHY_MEAS_MAX_LTE_LAYERS_NPG_CONN 5

/*! @brief Maximum number of LTE Layers in NPG in Idle
  */
#define LTE_CPHY_MEAS_MAX_LTE_LAYERS_NPG_IDLE 3

/*! @brief Maximum number of TDS Meas Objs
  */
#define LTE_CPHY_MEAS_MAX_TDS_MEAS_OBJ 16

/*! @brief
Maximum number of UTRA frequencies in the neighbour
list
  */
#define LTE_CPHY_MEAS_MAX_WCDMA_FREQ 16

/*! @brief
Maximum number of TDD frequencies in the neighbour
list
  */
#define LTE_CPHY_MEAS_MAX_TDSCDMA_FREQ 16

/*! Number of WCDMA frequencies supported for CSG */
#define LTE_CPHY_MEAS_NUM_WCDMA_CSG_FREQS 3 

/*! Number of LTE cells per frequency supported for CSG */
#define LTE_CPHY_MEAS_NUM_WCDMA_CSG_CELLS 16

/*! Number of LTE frequencies supported for CSG */
#define LTE_CPHY_MEAS_NUM_LTE_CSG_FREQS 3 

/*! Number of WCDMA cells per frequency supported for CSG */
#define LTE_CPHY_MEAS_NUM_LTE_CSG_CELLS 16

/*!
  @brief
  An enum describing the bit positions in the lte_cphy_idle_meas_cfg_mask_t
  */
enum
{
  LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON  = 0x01,
  LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ  = 0x02,
  LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ    = 0x04,
  LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ    = 0x08,
  LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA          = 0x10,
  LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM           = 0x20,
  LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA          = 0x40,
  LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA       = 0x80,
  LTE_CPHY_IDLE_MEAS_CFG_MASK_WCSG          = 0x100,
  LTE_CPHY_IDLE_MEAS_CFG_MASK_LCSG          = 0x200,
  LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS = 0x400,
};

/*! @brief
  A bitmask that includes all RATs
*/
#define LTE_CPHY_IDLE_MEAS_CFG_MASK_ALL_RAT \
 (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |  \
  LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |  \
  LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA      |  \
  LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM        |  \
  LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA       |  \
  LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA    |  \
  LTE_CPHY_IDLE_MEAS_CFG_MASK_WCSG       |  \
  LTE_CPHY_IDLE_MEAS_CFG_MASK_LCSG          \
  )

/*! @brief
  Idle measurements config  mask to enable the idle meas config. Only used
  in CPHY interface.
*/
typedef uint16 lte_cphy_idle_meas_cfg_mask_t;

/*! @brief Mobility state parameters for deciding to enter medium/high mobility
*/
typedef struct
{
  /*! Eval duration to enter mobility states (in sec) */
  uint8 t_eval;

  /*! Additional eval duration to enter normal mobility state (in sec) */
  uint8 t_hyst_normal;

  /*! Num cell changes to enter medium mobility state */
  uint8 n_cell_change_medium;

  /*! Num cell changes to enter high mobility state */
  uint8 n_cell_change_high;

} lte_cphy_meas_mobility_state_config_param_s;


/*! @brief Mobility state parameters for medium/high mobility
*/
typedef struct
{
  /*! If FALSE, then mobility state management is disabled, ignore rest of parameters */
  boolean is_valid;

  /*! Mobility State config params */
  lte_cphy_meas_mobility_state_config_param_s config_params;

  /*! Hysteresis correction applied in medium mobility state (in +/- dB) */
  int8 q_hyst_medium;

  /*! Hysteresis correction applied in high mobility state (in +/- dB) */
  int8 q_hyst_high;

} lte_cphy_meas_mobility_state_param_s;


/*! @brief Measurement configuration common to intra-freq, inter-freq
           and inter-rat reselection procedures
*/
typedef struct
{
  /*! Hysteresis for cell reselection (in dB) */
  uint8 q_hyst;

  /*! Mobility state parameters */
  lte_cphy_meas_mobility_state_param_s mobility_state_params;

  /*! Set to true to indicate neighbours have same reference signals */
  boolean same_ref_signals_in_neighbour;

} lte_cphy_meas_cell_resel_common_s;

/*! @brief Serving frequency reselection info applicable for Rel 9
*/
typedef struct
{
  /*! s-NonIntraSearchQ (0..31) threshold applicable only for Rel9. If not signaled
    default 0 will be passed */
  lte_l1_srch_meas_threshold_t s_non_intra_searchQ_r9;

  /*! if threshServingLowQ-r9 is passed */
  boolean thresh_serving_lowQ_r9_present;

  /*! threshServingLowQ-r9 (0..31) */
  lte_l1_resel_threshold_t thresh_serving_lowQ_r9;

} lte_cphy_meas_serving_freq_resel_info_rel9_s;


/*! @brief Parameters to used for serving freq when evaluating reselection
           candidates
*/
typedef struct
{
  /*! s-NonIntraSearch (0..62); For Rel 9, this field takes the value of the OTA field
    s-NonIntraSearchP if it is signaled. If not signaled, it will take the value of the
    Rel 8 OTA field s-NonIntraSearch. If neither of the OTA fields s-NonIntraSearchP or
    s-NonIntraSearch are signaled, default value LTE_CPHY_UINT8_INFINITY is used */
  lte_l1_srch_meas_threshold_t s_non_intra_search;

  /*! threshServingLow */
  lte_l1_resel_threshold_t thresh_serving_low;

  /*! Priority for serving freq */
  lte_l1_resel_priority_t  cell_resel_priority;

  /*! if serving_freq_resel_info_r9 is passed; For Rel 8 this boolean will always
    be set to FALSE. For Rel 9 the field will always be set to TRUE and either
    signaled values or default values will be passed */
  boolean serving_freq_resel_info_r9_present;

  /*! Serving frequency reselection info for Rel 9*/
  lte_cphy_meas_serving_freq_resel_info_rel9_s serving_freq_resel_info_r9;

} lte_cphy_meas_serving_freq_resel_info_s;


/*! @brief Enumeration of signalled bandwidth used for measurements
*/
typedef enum
{
    LTE_CPHY_MEAS_BANDWIDTH_6,       /*!<  6  RBs */
    LTE_CPHY_MEAS_BANDWIDTH_15,      /*!< 15  RBs */
    LTE_CPHY_MEAS_BANDWIDTH_25,      /*!< 25  RBs */
    LTE_CPHY_MEAS_BANDWIDTH_50,      /*!< 50  RBs */
    LTE_CPHY_MEAS_BANDWIDTH_75,      /*!< 76  RBs */
    LTE_CPHY_MEAS_BANDWIDTH_100,     /*!< 100 RBs */
    LTE_CPHY_MEAS_BANDWIDTH_DL       /*!< Same downlink bandwidth as in MIB */

} lte_cphy_meas_bandwidth_e;


/*! @brief Enumeration of timing-related scaling factors
*/
typedef enum
{
    LTE_CPHY_MEAS_TIME_SF_0DOT25,   /*!< 0.25 */
    LTE_CPHY_MEAS_TIME_SF_0DOT5,    /*!< 0.50 */
    LTE_CPHY_MEAS_TIME_SF_0DOT75,   /*!< 0.75 */
    LTE_CPHY_MEAS_TIME_SF_1DOT0     /*!< 1.00 */

} lte_cphy_meas_time_sf_e;


/*! @brief Parameters for whitelisted cells for intra/inter frequency meas
*/
typedef struct
{
  lte_phy_cell_id_t pci;    /*!< Physical cell id */
  int8 q_offset;            /*!< Cell-specific offset (in +/- dB) */

} lte_cphy_meas_eutra_cell_s;


/*! @brief Parameters for blacklisted cells for intra/inter frequency meas
*/
typedef struct
{
  lte_phy_cell_id_and_range_s pci_range;    /*!< Range of PCIs */

} lte_cphy_meas_eutra_black_cell_s;

/*! @brief Parameters for blacklisted EARFCN PCIs
*/
typedef struct
{
  lte_earfcn_t earfcn; /*!< EARFCN */
  lte_phy_cell_id_and_range_s pci_range;    /*!< Range of PCIs */

} lte_cphy_meas_eutra_freq_pci_range_s;

/*! @brief Rel 9 parameters for intra freq reselection
*/
typedef struct
{
  /*! q_QualMin_r9 (-34..-3) applicable only for Rel9. If not signaled, default
    LTE_CPHY_INT8_NEG_INFINITY will be passed */
  int8 q_qual_min_r9;

  /*! s-IntraSearchQ (0..31) threshold applicable only for Rel9. If not signaled, default
    0 dB will be passed */
  lte_l1_srch_meas_threshold_t s_intra_searchQ_r9;

} lte_cphy_meas_intra_freq_resel_info_rel9_s;

/*! @brief Intra-freq cell reselection measurement configuration parameters
*/
typedef struct
{
  /*! Brief description of this member */
  lte_l1_q_rxlevmin_t q_rxlevmin;

  /*! Maximum value for output power that the UE should use */
  int8 p_max;

  /*! Threshold current cell meas must fall below to consider intra freq for reselection;
    For Rel 9, this field takes the value of the OTA field s-IntraSearchP (0..62) if it is signaled,
    else if not signaled, the field takes the value of the Rel 8 OTA field s-IntraSearch. If neither of
    s-IntraSearchP or s-IntraSearch are signaled, default value LTE_CPHY_UINT8_INFINITY is used */
  lte_l1_srch_meas_threshold_t s_intra_search;

  /*! Bandwidth to restrict measurements on */
  lte_cphy_meas_bandwidth_e meas_bandwidth;

  /*! Indicates whether all the neighbouring cells use Antenna Port 1. When set to TRUE,
      the UE may assume that at least two cell-specific antenna ports are used in all
      neighbouring cells.*/
  boolean antenna_port_1;

  /*! neighCellConfig : Provides information related to MBSFN and TDD UL/DL configuration
      of neighbour cells of this frequency
      00: Not all neighbour cells have the same MBSFN subframe allocation as serving cell
      10: The MBSFN subframe allocations of all neighbour cells are identical to or subsets
          of that in the serving cell
      01: No MBSFN subframes are present in all neighbour cells
      11: Different UL/DL allocation in neighbouring cells for TDD compared to the serving cell
      For TDD, 00, 10 and 01 are only used for same UL/DL allocation in neighbouring cells
      compared to the serving cell.
  */
  uint8 neighbourCellConfig;

  /*! t-ReselectionEUTRAN */
  lte_l1_treselection_t t_resel_eutran;

  /*! t-ReselectionEUTRAN-SF-Medium */
  lte_cphy_meas_time_sf_e t_resel_eutran_medium;

  /*! t-ReselectionEUTRAN-SF-High */
  lte_cphy_meas_time_sf_e t_resel_eutran_high;

  /*! Cell-specific offsets */
  uint8 whitelist_num_cells;
  lte_cphy_meas_eutra_cell_s whitelist[LTE_CPHY_MEAS_MAX_CELL_INTRA];

  /*! Blacklisted cells */
  uint8 blacklist_num_cells;
  lte_cphy_meas_eutra_black_cell_s blacklist[LTE_CPHY_MEAS_MAX_CELL_BLACK];

  /*! CSG PCI range */
  lte_phy_cell_id_and_range_s csg_pci_range;

  /*! If intra_freq_resel_rel9_param are passed; For Rel 8 this boolean will always
    be set to FALSE. For Rel 9 the field will always be set to TRUE and either
    signaled values or default values will be passed */
  boolean intra_freq_resel_rel9_param_present;

  /*! Rel9 parameters for intra-freq reselection */
  lte_cphy_meas_intra_freq_resel_info_rel9_s intra_freq_resel_rel9_param;

} lte_cphy_meas_intra_freq_resel_info_s;

/*! @brief Cell reselection thresholds for rel 9
*/
typedef struct
{
  /*! Squal threshold (0..31) of an evaluated cell must be greater than these values to
  be considered for reselection to lower priority frequencies / RATs */
  lte_l1_resel_threshold_t threshX_lowQ;

  /*! Squal threshold (0..31) of an evaluated cell must be greater than these values to
  be considered for reselection to higher priority frequencies / RATs */
  lte_l1_resel_threshold_t threshX_highQ;

} lte_cphy_meas_resel_thresh_rel9_s;

/*! @brief Inter-freq cell reselection parameters for rel 9
*/
typedef struct
{
  /*! q_QualMin_r9 (-34..-3) applicable only for Rel9. If not signaled, default
    LTE_CPHY_INT8_NEG_INFINITY will be passed */
  int8 q_qual_min_r9;

  /*! if Squal thresholds are passed */
  boolean thresh_rel9_present;

  /*! Squal threshold of an evaluated cell must be greater than these values to
  be considered for reselection. If threshServingLowQ is signaled in SIB3, the below
  thresholds will be signaled */
  lte_cphy_meas_resel_thresh_rel9_s thresh_rel9;

} lte_cphy_meas_inter_freq_resel_info_rel9_s;

/*! @brief Inter-freq cell reselection measurement configuration parameters
*/
typedef struct
{
  /*! E-UTRA DL carrrier frequency to measure on */
  lte_earfcn_t dl_carrier_freq;

  /*! Minimum required Rx RSRP level */
  lte_l1_q_rxlevmin_t q_rxlevmin;

  /*! Maximum value for output power that the UE should use */
  int8 p_max;

  /*! Cell reselection timer */
  lte_l1_treselection_t t_resel_eutran;

  /*! t-ReselectionEUTRAN-SF-Medium */
  lte_cphy_meas_time_sf_e t_resel_eutran_medium;

  /*! t-ReselectionEUTRAN-SF-High */
  lte_cphy_meas_time_sf_e t_resel_eutran_high;

  /*! Srxlev-value of an evaluated cell must be greater than these values to
  be considered for reselection */
  lte_l1_resel_threshold_t threshX_low;
  lte_l1_resel_threshold_t threshX_high;

  /*! Bandwidth to restrict measurements on */
  lte_cphy_meas_bandwidth_e meas_bandwidth;

  /*! Cell reselection priority */
  lte_l1_resel_priority_t cell_resel_priority;

  /*! Indicates whether all the neighbouring cells use Antenna Port 1. When set to TRUE,
      the UE may assume that at least two cell-specific antenna ports are used in all
      neighbouring cells.*/
  boolean antenna_port_1;

  /*! neighCellConfig : Provides information related to MBSFN and TDD UL/DL configuration
      of neighbour cells of this frequency
      00: Not all neighbour cells have the same MBSFN subframe allocation as serving cell
      10: The MBSFN subframe allocations of all neighbour cells are identical to or subsets
          of that in the serving cell
      01: No MBSFN subframes are present in all neighbour cells
      11: Different UL/DL allocation in neighbouring cells for TDD compared to the serving cell
      For TDD, 00, 10 and 01 are only used for same UL/DL allocation in neighbouring cells
      compared to the serving cell.
  */
  uint8 neighbourCellConfig;

  /*! Freq-specific offset */
  int8 q_offset_freq;

  /*! Cell-specific offsets */
  uint8 whitelist_num_cells;        /*!< 0 indicates parameter is absent */
  lte_cphy_meas_eutra_cell_s whitelist[LTE_CPHY_MEAS_MAX_CELL_INTER];

  /*! Blacklisted cells */
  uint8 blacklist_num_cells;        /*!< 0 indicates parameter is absent */
  lte_cphy_meas_eutra_black_cell_s blacklist[LTE_CPHY_MEAS_MAX_CELL_BLACK];

  /*! if Inter-freq reselection reselection param for rel 9 are passed;
    For Rel 8 this boolean will always be set to FALSE. For Rel 9 the field will
    always be set to TRUE and either signaled values or default values will be passed */
  boolean inter_freq_resel_info_rel9_present;

  /*! Inter-freq reselection param for rel 9 */
  lte_cphy_meas_inter_freq_resel_info_rel9_s inter_freq_resel_param_rel9;

} lte_cphy_meas_inter_freq_resel_freq_info_s;

/*! @brief Inter-freq cell reselection list
*/
typedef struct
{
  /*! Number of configured measurement frequencies */
  uint8 num_freq;

  /*! Number of L neighbours in NPG list
      Say if NPG List is 2 and total freqs are 8
      Then first 2 entires in Freq list is for NPG List and remaining
      6 are in RPG list*/
  uint8 num_npg_list_ngbr_freq;

  /*! List of frequency specific measurement configuration */
  lte_cphy_meas_inter_freq_resel_freq_info_s freq_list[LTE_CPHY_MEAS_MAX_NUM_FREQ];

} lte_cphy_meas_inter_freq_resel_info_s;

/*! @brief GERAN frequency group.

  This defines a group of GERAN frequencies with associated priority and
  reselection parameters.  This GERAN frequency group is analogous to
  a frequency in another RAT.

  Defined in 36.331 Section 6.4
  */
typedef struct
{
  /*! The priority of this frequency group */
  lte_l1_resel_priority_t  cell_resel_priority;

  /*! Reselection threshold for high priority layers.
    */
  lte_l1_resel_threshold_t thresh_gsm_high;
  /*! Reselection threshold for low priority layers.
    */
  lte_l1_resel_threshold_t thresh_gsm_low;

  /*! Parameter Q_rxlevmin as defined in 45.008.

    This is the actual Q_rxlev_min value in Dbm.

    Needs to be converted from the IE value defined in 3GPP 36.331
    Section 6.3.1
    */
  int8 q_rxlev_min;

  /*! Maximum allowed transmission power for GERAN on an uplink carrier
    frequency as defined in 45.008.

    Value in dBm. Applicable for the neighbouring GERAN cells on this carrier
    frequency.

    Range 0..39

    */
  int8 p_max;

  /*! GSM band flag identifying ARFCNs as specified in 3GPP 45.005.

    Allows UE to differentiate between GSM DCS 1800 and PCS 1900 frequencies
    which have overlapping ARFCN values.

    This flag does not specify the band for all ARFCNs in this group, but
    only serves as a differentiator for overlapping ARFCN values.  Must
    be set to either LTE_L1_GSM_BAND_DCS_1800 or LTE_L1_GSM_BAND_PCS_1900
    and is only relevant to ARFCN values which fall in either of those bands.
   */
  lte_l1_gsm_band_e gsm_band;

  /*!  Bitmask specifying whether a neighbour with a particular
    Network Color Code is to be reported.  Bit "n" set to 1 means
    that a neighbour with NCC "n" should be included in the report.

    This flag is synonymous with a blacklist in other RATs
   */
  uint8 ncc_permitted;

} lte_cphy_meas_irat_gsm_freq_group_s;


/*! @brief GERAN Frequencies.

  This flattened structure of GSM frequencies is used to minimize space.

  */
typedef struct
{
  /*! The frequency group that this frequency is associated with.
     This is the index into the frequency group array in the GERAN configuration
   */
  uint16 freq_group_index;

  /*! Array of gsm frequencies to measure. */
  lte_l1_gsm_arfcn_t freq;

} lte_cphy_meas_irat_gsm_freq_s;

/*! 
  @brief UE mode types
*/
typedef enum
{
  /*! UE mode is CSFB */
  LTE_CPHY_UE_MODE_DEFAULT,

  /*! UE mode is SGLTE */
  LTE_CPHY_UE_MODE_SGLTE,

  /*! UE mode is 1xSrlte */
  LTE_CPHY_UE_MODE_1XSRLTE,

  /*! UE mode is SVLTE */
  LTE_CPHY_UE_MODE_SVLTE,

  /*! UE mode is SGLTE but GSM is inactive during VOLTE 
      ML1 must map this as default, and RRC as SGLTE */
  LTE_CPHY_UE_MODE_SGLTE_INACTIVE

} lte_cphy_ue_mode_type_e;

/*==========================================================================
  Coex DSDA conflict enum
===========================================================================*/
typedef enum
{
  /*! No conflict need for this device/ue*/
  LTE_ML1_COEX_DSDA_NO_CONFLICT_NEEDED = 0,
  /*! TX_backoff is needed. It's SGLTE or SGLTE+G*/
  LTE_ML1_COEX_DSDA_TX_BACKOFF_NEEDED,
  /*! Blanking is needed. It's NON SGLTE or SGLTE+G with DSDA*/
  LTE_ML1_COEX_DSDA_BLANKING_NEEDED
}lte_ml1_coex_dsda_conflict_type_e;
/*! @brief Idle Mode GERAN measurement configuration structure.

  This structure defines the idle-mode reselection and measurement
  parameters to be used by ML1.  Any new configuration will be applied
  at the next paging occasion.

  */
typedef struct
{

  /*! UE mode to configure */
  lte_cphy_ue_mode_type_e ue_mode;

  /*! T_Reselection timer dictating how long a given cell's receive power
    exceeds a reselection threshold prior to reselection.

    See 3GPP 36.304 5.2.4.5 for usage details.
    */
  uint8 t_reselection;

  /*! The speed scaling factor to be applied while in medium mobility
    state as defined in 3GPP 36.304 5.2.4.5
    */
  lte_cphy_meas_time_sf_e sf_medium;
  /*! The speed scaling factor to be applied while in high mobility
    state as defined in 3GPP 36.304 5.2.4.5
    */
  lte_cphy_meas_time_sf_e sf_high;

  /*! The maximum ue transmit power as defined as MS_TXPWR_MAX_CCH  in
    3GPP 45.008.

    Value in dBm.
    */
  int8 max_ue_gsm_transmit_power;

  /*! The number of GSM frequency groups corresponding to the number of
    frequency groups specified in the freq_groups array. */
  uint8 num_frequency_groups;

  /*! Array of frequency groups */
  lte_cphy_meas_irat_gsm_freq_group_s freq_groups[LTE_CPHY_MEAS_MAX_GERAN_FREQ_GROUPS];

  /*! The number of frequencies being configured.
    Range 0 .. LTE_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP */
  uint8 num_freqs;

  /*! The GSM frequencies to be configured in L1 */
  lte_cphy_meas_irat_gsm_freq_s freqs[LTE_CPHY_MEAS_MAX_GERAN_FREQ];
} lte_cphy_meas_irat_gsm_info_s;

/*! @brief typedef for CDMA system time type: uint64
*/
typedef uint64 lte_cphy_irat_cdma_system_time_t;

/*! @brief
  Structure with the CDMA system time type: uint64
*/
typedef struct
{
  /*! CDMA system time */
  lte_cphy_irat_cdma_system_time_t system_time;

} lte_cphy_irat_cdma_system_time_s;

/*! @brief enumeration to identify the CDMA system time type
*/
typedef enum
{
  LTE_CPHY_IRAT_CDMA_SYS_TIME_SYNCHRONOUS_TYPE, /*!<  Synchronous system time */
  LTE_CPHY_IRAT_CDMA_SYS_TIME_ASYNCHRONOUS_TYPE /*!<  Asynchronous system time */

} lte_cphy_irat_cdma_system_time_type_e;

/*! @brief enumeration to identify the dayLightSavingTime type
*/
typedef enum
{
  LTE_CPHY_DAYLIGHT_NONE,     /*!< NO adjustment needed*/
  LTE_CPHY_DAYLIGHT_PLUS_ONE, /*!< +1 hour adjustment*/
  LTE_CPHY_DAYLIGHT_PLUS_TWO, /*!< +2 hour adjustment*/
  LTE_CPHY_DAYLIGHT_RESERVED  /*!< reserved */

}lte_cphy_utc_dayLightSaving_type_e;



/*! @brief enumeration to identify BPLMN request type
*/
typedef enum
{
  /*! UE active on LTE, PLMN Search in LTE */
  LTE_CPHY_BPLMN_TYPE_LTE_LTE,

  /*! UE active on IRAT, PLMN Search in LTE */
  LTE_CPHY_BPLMN_TYPE_IRAT_LTE,

  /*! UE active on LTE, PLMN Search in IRAT */
  LTE_CPHY_BPLMN_TYPE_LTE_IRAT
} lte_cphy_bplmn_type_e;

/*! @brief enumeration to indicate if the cell is barred,
           the frequency is barred, or can be regularily
           used
*/
typedef enum
{
  LTE_CPHY_BPLMN_NOT_BARRED,
  LTE_CPHY_BPLMN_CELL_BARRED,
  LTE_CPHY_BPLMN_FREQ_BARRED
} lte_cphy_bplmn_cell_barred_type_e;

/*! @brief enumeration to BPLMN complete type */
typedef enum
{
  LTE_CPHY_BPLMN_PARTIAL_DONE,
  LTE_CPHY_BPLMN_ALL_DONE,
  LTE_CPHY_BPLMN_FAIL
} lte_cphy_bplmn_complete_type_e;

/*! @brief enumeration to BPLMN priority type */
typedef enum
{
  LTE_CPHY_BPLMN_PRIORITY_HIGH, /* The default value; BPLMN search should be prioritised
                                   over other procedures */
  LTE_CPHY_BPLMN_PRIORITY_LOW /* Other procedures can be prioritised over BPLMN search */
} lte_cphy_bplmn_priority_e;

/*! @brief enum of CDMA system time status states */
typedef enum
{
  LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_SIB8,
  LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_RTT,
} lte_cphy_irat_cdma_system_time_source_type_e;

/*! @brief union of synchronous and asynchronous system time
*/
typedef union
{
  /*! Synchronous system time */
  lte_cphy_irat_cdma_system_time_s cdma_synchronous_system_time;

  /*! Asynchronous system time */
  lte_cphy_irat_cdma_system_time_s cdma_asynchronous_system_time;

} lte_cphy_cdma_system_time_u;


/*! @brief CDMA system time info structure
*/
typedef struct
{
  /*! CDMA system time type - synchronous or asynchronous, indicates if CDMA2000
    and EUTRA networks are in sync, i.e there is no drift between their timing */
  lte_cphy_irat_cdma_system_time_type_e cdma_system_time_type;

  /*! CDMA system time */
  lte_cphy_cdma_system_time_u cdma_system_time;

  /*! SFN boundary at or after the ending boundary of the SI-window in which
    SIB8 is transmitted */
  lte_l1_sfn_t sib8_sfn_boundary;

} lte_cphy_irat_cdma_sib8_system_time_info_s;

/*! @brief UTC time info structure
*/
typedef struct
{
  /*! UTC system time indicating the integer count of 10 ms units
   since 00:00:00 on 1 January, 1900*/
  uint64 utc_system_time;

  /*! SFN boundary at or after the ending boundary of the SI-window in which
    SIB16 is transmitted */
  lte_l1_sfn_t sfn_boundary;

  /*! If UTC dayLightSavingTime is enabled - TRUE
  ELSE - ML1 should delete any values configured*/
  boolean utc_dayLightSavingTime_enabled;

  /*! UTC dayLightSavingTime Application*/
  lte_cphy_utc_dayLightSaving_type_e dayLightSavingTime;

  /*! If UTC leapSeconds is enabled - TRUE
  ELSE - ML1 should delete any values configured*/
  boolean utc_leapSeconds_enabled;

  /*! UTC leapSeconds in units of seconds */
  int16 leapSeconds;

  /*! If UTC localTimeOffset is enabled - TRUE
  ELSE - ML1 should delete any values configured*/
  boolean utc_localTimeOffset_enabled;

  /*! UTC localTimeOffset in units of seconds */
  int16 localTimeOffset;

} lte_cphy_utc_time_info_s;

/*! LTE STMR value - 22 bit value */
typedef uint32 lte_stmr_type;

typedef struct
{
  /*! LTE STMR value - 22 bit value */
  lte_stmr_type                    lte_ostmr;
  /*! CDMA system time in CDMA chip 1.2288MHZ - 64 bit value */
  lte_cphy_irat_cdma_system_time_t cdma_sys_time;
}lte_stmr_cdma_rtc_pair;

/*! @brief Reverse CDMA system time status enum */
typedef enum
{
  LTE_CPHY_IRAT_CDMA_REVERSE_SYSTEM_TIME_SUCCESS,
  LTE_CPHY_IRAT_CDMA_REVERSE_SYSTEM_TIME_FAILURE,

} lte_cphy_irat_cdma_reverse_system_time_status_e;

/*! @brief CDMA system time info structure
*/
typedef struct
{
  /*! Reverse time transfer status */
  lte_cphy_irat_cdma_reverse_system_time_status_e status;

  /*! RAT type */
  lte_rat_e rat_type;

  /*! CDMA time information */
  lte_stmr_cdma_rtc_pair cdma_time_info;

  /*! CDMA layer info */
  sys_channel_type channel;

} lte_cphy_irat_cdma_rtt_system_time_info_s;

/*! @brief CDMA system time info union
*/
typedef union
{
  /*! SIB8 provided CDMA system time */
  lte_cphy_irat_cdma_sib8_system_time_info_s sib8;

  /*! Reverse time transfer provided CDMA system time */
  lte_cphy_irat_cdma_rtt_system_time_info_s rtt;

} lte_cphy_irat_cdma_system_time_info_u;

/*! @brief CDMA2000 band class info structure
*/
typedef struct
{
  /*! CDMA2000 band class, as per 36.331 section 6.3.4, the valid band classes
    are bc0 to bc17 */
  sys_band_class_e_type band_class;

  /*! Srxlev-value of an evaluated cell must be greater than these values to
  be considered for reselection */
  lte_l1_resel_threshold_t threshX_low;
  lte_l1_resel_threshold_t threshX_high;

  /*! Cell reselection priority */
  lte_l1_resel_priority_t cell_resel_priority;

} lte_cphy_meas_irat_cdma_band_class_info_s;

/*! @brief CDMA2000 band channel info structure
*/
typedef struct
{
  /*! CDMA2000 band class, as per 36.331 section 6.3.4, the valid band classes
    are bc0 to bc17 */
  sys_band_class_e_type band_class;

  /*! CDMA channel num */
  sys_channel_num_type channel;

  /*! CDMA Cell id of the neighbor cell */
  sys_pilot_pn_type cell_id;

} lte_cphy_meas_irat_cdma_neigh_cell_info_s;

/*! @brief CDMA Reselection info structure
*/
typedef struct
{

  /*! Add num of band classes in the list below */
  uint8 band_class_list_size;

  /*! List of CDMA band classes */
  lte_cphy_meas_irat_cdma_band_class_info_s band_class_list[LTE_CPHY_MEAS_MAX_CDMA_BAND_CLASS];

  /*! num of CDMA neighbor cell info elements in the list below */
  uint8 neigh_cell_info_list_size;

  /*! List of CDMA neighbor cells */
  lte_cphy_meas_irat_cdma_neigh_cell_info_s   neigh_cell_info_list[LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS_REL9];

  /*! Cell reselection timer */
  lte_l1_treselection_t t_resel_cdma;

  /*! t-ReselectionCDMA-SF-Medium, default value is LTE_CPHY_MEAS_TIME_SF_1DOT0
    when not signaled. There is no scaling for medium / high speeds when using
    the default value */
  lte_cphy_meas_time_sf_e t_resel_cdma_medium;

  /*! t-ReselectionCDMA-SF-High, default value is LTE_CPHY_MEAS_TIME_SF_1DOT0
    when not signaled. There is no scaling for medium / high speeds when using
    the default value */
  lte_cphy_meas_time_sf_e t_resel_cdma_high;

} lte_cphy_meas_irat_cdma_resel_param_s;

/*! @brief CDMA info structure for eHRPD and 1x
*/
typedef struct
{
  /*! If CDMA search_window_size is enabled */
  boolean irat_cdma_search_window_size_enabled;

  /*! CDMA search_window_size */
  uint8 irat_cdma_search_window_size;

  /*! If eHRPD reselection is enabled */
  boolean irat_cdma_ehrpd_resel_enabled;

  /*! eHRPD reselection parameters */
  lte_cphy_meas_irat_cdma_resel_param_s irat_cdma_ehrpd_resel_param;

  /*! If 1x reselection is enabled */
  boolean irat_cdma_1x_resel_enabled;

  /*! 1x reselection parameters */
  lte_cphy_meas_irat_cdma_resel_param_s irat_cdma_1x_resel_param;

} lte_cphy_meas_irat_cdma_info_s;

/*! @brief
Idle mode inter-rat per WCDMA frequency information
*/
typedef struct
{
  /*! UTRAN frequency 0..16383 */
  lte_uarfcn_t freq;

  /*! Cell reselection priority 0..7*/
  lte_l1_resel_priority_t  cell_resel_priority;

  /*! Reselection threshold*/
  lte_l1_resel_threshold_t thresh_Xhigh;

  /*! Reselection threshold*/
  lte_l1_resel_threshold_t thresh_Xlow;

  /*! if Squal thresholds are passed; For Rel 8 this boolean will always be set
    to FALSE. For Rel 9 the field will be set to TRUE if threshX-Q-r9 is siganled */
  boolean thresh_rel9_present;

  /*! Squal threshold of an evaluated cell must be greater than these values to
  be considered for reselection. If threshServingLowQ is signaled in SIB3, the below
  thresholds will be signaled */
  lte_cphy_meas_resel_thresh_rel9_s thresh_rel9;

  /*!Actual value = IE value * 2+1. Specifies the minimum required Rx level
  in the cell expressedin dBm. Minimum required RX level in the cell.
  Defined in 25.304  Values in the range -60..-13*/
  int8 q_rx_lev_min_actual;

  /*! The maximum allowed transmission power on the (uplink) carrier
  frequency in dBm. Defined in 25.304. Values in the range -50..33 */
  int8 p_max;

  /*! Minimum required quality level in the cell (dB). Defined in 25.304
  Values in the range -24..0*/
  int8 q_qual_min;

}lte_cphy_meas_wcdma_freq_list_s;


/*! @brief
Idle mode inter rat WCDMA measurement config paramaeters
*/
typedef struct
{
  /*! Cell reselection timer value. This is time during which
  if the new cell is better ranked than the serving cell prior
  to cell reselection evaluation
  */
  lte_l1_treselection_t   t_resel;

  /*! When medium mobility state is detected
  t-Reselection-UTRA should be multiplied by SF-Medium and rounded off
  to the nearest second. Sec 5.2.4.3.1 36.304*/
  lte_cphy_meas_time_sf_e sf_medium;

  /*! When high mobility state is detected
  t-Reselection-UTRA should be multiplied by SF-high and rounded off
  to the nearest second. Sec 5.2.4.3.1 36.304*/
  lte_cphy_meas_time_sf_e sf_high;

  /*! Number of UTRA neighbours Can Range from 0...16 */
  uint8 num_ngbr_freq;

  /*!Information pertaining to each utra fdd frequency needed for the
  measurements*/
  lte_cphy_meas_wcdma_freq_list_s  fdd_freq_list[LTE_CPHY_MEAS_MAX_WCDMA_FREQ];
}lte_cphy_meas_irat_wcdma_resel_info_s;

/*! @brief
Idle mode inter-rat per TDSCDMA frequency information
*/
typedef struct
{
  /*! UTRAN frequency 0..16383 */
  lte_uarfcn_t freq;

  /*! Cell reselection priority 0..7*/
  lte_l1_resel_priority_t  cell_resel_priority;

  /*! Reselection threshold*/
  lte_l1_resel_threshold_t thresh_Xhigh;

  /*! Reselection threshold*/
  lte_l1_resel_threshold_t thresh_Xlow;

  /*!Actual value = IE value * 2+1. Specifies the minimum required Rx level
  in the cell expressedin dBm. Minimum required RX level in the cell.
  Defined in 25.304  Values in the range -60..-13*/
  int8 q_rx_lev_min_actual;

  /*! The maximum allowed transmission power on the (uplink) carrier
  frequency in dBm. Defined in 25.304. Values in the range -50..33 */
  int8 p_max;

  /*! Boolean to indicate if UARFCN is from TDS ACQ DB or not*/
  boolean freq_in_tds_acq_db;

}lte_cphy_meas_tdscdma_freq_list_s;


/*! @brief
Idle mode inter rat TDSCDMA measurement config paramaeters
*/
typedef struct
{
  /*! Cell reselection timer value. This is time during which
  if the new cell is better ranked than the serving cell prior
  to cell reselection evaluation
  */
  lte_l1_treselection_t   t_resel;

  /*! When medium mobility state is detected
  t-Reselection-UTRA should be multiplied by SF-Medium and rounded off
  to the nearest second. Sec 5.2.4.3.1 36.304*/
  lte_cphy_meas_time_sf_e sf_medium;

  /*! When high mobility state is detected
  t-Reselection-UTRA should be multiplied by SF-high and rounded off
  to the nearest second. Sec 5.2.4.3.1 36.304*/
  lte_cphy_meas_time_sf_e sf_high;

  /*! Number of UTRA neighbours Can Range from 0...16 */
  uint8 num_ngbr_freq;

  /*! Number of UTRA neighbours in active list
         For TDS supported network, the range can be 1 to 6
           So Active list ngbr entries will be tdd_freq_list[0] .. tdd_freq_list[5]
           and remaining ngbr entires will be dormant list
         For Non TDS supported network, num_active_list_ngbr_freq = 
           num_ngbr_freq (i.e. no dormant list)*/
  uint8 num_active_list_ngbr_freq;

  /*!Information pertaining to each utra tdd frequency needed for the
  measurements*/
  lte_cphy_meas_tdscdma_freq_list_s  tdd_freq_list[LTE_CPHY_MEAS_MAX_TDSCDMA_FREQ];
}lte_cphy_meas_irat_tdscdma_resel_info_s;


/*! WCDMA cell information */
typedef struct
{
  /*! Scrambling code of the resel candidate cell */
  uint16                     psc;

  /*! Whether timing and measurements are present.
    if FALSE, only the PSC is valid */
  boolean                    timing_present;

  /*! PN position In units of chipx1 Range 0...38400 */
  uint32                     pn_pos;

  /*! 2x the ecno for the cell */
  int16                      ecno_2x;
  /*! The RSCP for the cell */
  int16                      rscp;

  /*!Actual value = IE value * 2+1. Specifies the minimum required Rx level
  in the cell expressedin dBm. Minimum required RX level in the cell.
  Defined in 25.304  Values in the range -60..-13*/
  int8 q_rx_lev_min_actual;

  /*! The maximum allowed transmission power on the (uplink) carrier
  frequency in dBm. Defined in 25.304. Values in the range -50..33 */
  int8 p_max;

  /*! Minimum required quality level in the cell (dB). Defined in 25.304
  Values in the range -24..0*/
  int8 q_qual_min;

} lte_cphy_meas_csg_w_cell_info_s;

/*! @brief
  WCDMA CSG information
 */
typedef struct
{
  /*! Frequency */
  lte_uarfcn_t uarfcn;

  /*! Number of cells's */
  uint8 num_cells;

  /*! List of PSCs for CSG cells */
  lte_cphy_meas_csg_w_cell_info_s cell[LTE_CPHY_MEAS_NUM_WCDMA_CSG_CELLS];
}lte_cphy_meas_csg_wcdma_freq_info_s;

/*!
  @brief
  Idle meas config CSG info

  */
typedef struct
{
  /*! Number of frequencies in freq array */
  uint8 num_w_freq;

  lte_cphy_meas_csg_wcdma_freq_info_s wcdma_freq_info[LTE_CPHY_MEAS_NUM_WCDMA_CSG_FREQS];

} lte_cphy_meas_csg_info_s;


/*! @brief
  LTE CSG information
 */
typedef struct
{
  /*! Frequency */
  lte_earfcn_t earfcn;

  /*! Number of cells's */
  uint8 num_cells;

  /*! List of PCIs for CSG cells */
  lte_phy_cell_id_t pci[LTE_CPHY_MEAS_NUM_LTE_CSG_CELLS];

  /*! CSG PCI range */
  lte_phy_cell_id_and_range_s csg_pci_range;

}lte_cphy_meas_csg_lte_freq_info_s;

/*!
  @brief
  Idle meas config LTE CSG info

  */
typedef struct
{
  /*! Number of frequencies in freq array */
  uint8 num_l_freq;

  lte_cphy_meas_csg_lte_freq_info_s lte_freq_info[LTE_CPHY_MEAS_NUM_LTE_CSG_FREQS];

} lte_cphy_meas_lte_csg_info_s;

/*! @brief
  Idle mode measurement config parameters.
*/
typedef struct
{
  /*! Bitmask to indicate which meas config has been requested in the current message,
      bit masks defined above, see LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON */
  lte_cphy_idle_meas_cfg_mask_t requested_cfg_bitmask;

  /* ! Bitmask to indicate which meas configs have been disbaled,
     meas config bitmask that has been disabled should not be included in the
     requested_cfg_bitmask */
  lte_cphy_idle_meas_cfg_mask_t disabled_cfg_bitmask;

  /*! Common reselection configuration */
  lte_cphy_meas_cell_resel_common_s *resel_common_info_ptr;

  /*! Reselection configuration for serving freq */
  lte_cphy_meas_serving_freq_resel_info_s *serving_freq_info_ptr;

  /*! Configuration for intra-freq measurements */
  lte_cphy_meas_intra_freq_resel_info_s *intra_freq_info_ptr;

  /*! Configuration for inter-freq measurements */
  lte_cphy_meas_inter_freq_resel_info_s *inter_freq_info_ptr;

  /*! Configuration for cdma measurements */
  lte_cphy_meas_irat_cdma_info_s *irat_cdma_info_ptr;

  /*! Configuration for GSM measurements */
  lte_cphy_meas_irat_gsm_info_s *irat_gsm_info_ptr;

  /*! Configuration for the inter-rat UTRA-FDD measurements */
  lte_cphy_meas_irat_wcdma_resel_info_s *irat_wcdma_info_ptr;

  /*! Configuration for the inter-rat UTRA-TDD measurements */
  lte_cphy_meas_irat_tdscdma_resel_info_s *irat_tdscdma_info_ptr;

  /*! Configuration for the inter-rat UTRA CSGs */
  lte_cphy_meas_csg_info_s *csg_info_ptr;

  /*! Configuration for the LTE CSGs */
  lte_cphy_meas_lte_csg_info_s *lte_csg_info_ptr;

  /*! Boolean to disable idle search/meas periodicity */
  boolean skip_idle_srch_meas;

} lte_cphy_idle_meas_cfg_info_s;


/******************************************************************************
                Connected-mode Measurement Configuration
*******************************************************************************/

/*! @brief
  Maximum number of measurement object ids
  Defined in 36.331 Section 6.4
*/
#define LTE_CPHY_MEAS_MAX_MEAS_OBJ_ID 32

/*! @brief
  Maximum number of measurement ids
  Defined in 36.331 Section 6.4
*/
#define LTE_CPHY_MEAS_MAX_MEAS_ID 32

/*! @brief
  Maximum number of measurement reporting config ids
  Defined in 36.331 Section 6.4
*/
#define LTE_CPHY_MEAS_MAX_REPORT_CFG_ID 32

/*! @brief Measurement ID type. Range 1..32
*/
typedef uint8 lte_cphy_meas_id_t;

/*! @brief Measurement object ID type. Range 1..32
*/
typedef uint8 lte_cphy_meas_obj_id_t;

/*! @brief Reporting configuration ID type. Range 1..32
*/
typedef uint8 lte_cphy_report_cfg_id_t;


/*! @brief Measurement ID config (links meas object to report config)
*/
typedef struct
{
  /*! Measurement object ID. Range 1..32 */
  lte_cphy_meas_obj_id_t meas_obj_id;

  /*! Reporting config ID. Range 1..32 */
  lte_cphy_report_cfg_id_t report_cfg_id;

} lte_cphy_meas_id_s;


/*! @brief Measurement configuration for a whitelisted EUTRA cell
*/
typedef struct
{
  /*! Physical cell identity */
  lte_phy_cell_id_t pci;

  /*! Cell specific offset (in +/- dB). Range -24..24 */
  int16 cell_offset;

} lte_cphy_meas_eutra_neigh_cell_s;


/*! @brief List of whitelisted EUTRA neighbour cells
*/
typedef struct
{
  /*! Indicates which elements of cell_info list are active. A '1' indicates
      active; e.g. bit0 = 1 indicates element 0 active. */
  uint32 active_bitmask;

  /*! List of whitelisted cells (with their cell-specific offsets) */
  lte_cphy_meas_eutra_neigh_cell_s cell_info[LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS];

} lte_cphy_meas_eutra_neigh_cell_list_s;


/*! @brief Measurement configuration for a blacklisted EUTRA cell
*/
typedef struct
{
  /*! Physical cell identity or range */
  lte_phy_cell_id_and_range_s pci_range;

} lte_cphy_meas_eutra_blacklisted_cell_s;


/*! @brief List of blacklisted EUTRA neighbour cells
*/
typedef struct
{
  /*! Indicates which elements of cell_info list are active. A '1' indicates
      active; e.g. bit0 = 1 indicates element 0 active. */
  uint32 active_bitmask;

  /*! List of blacklisted cells */
  lte_cphy_meas_eutra_blacklisted_cell_s cell_info[LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS];

} lte_cphy_meas_eutra_blacklisted_cell_list_s;


/*! @brief Measurement object for EUTRA frequencies
*/
typedef struct
{
  /*! Measurement carrier freq */
  lte_earfcn_t carrier_freq;

  /*! Bandwidth to restrict measurements on */
  lte_cphy_meas_bandwidth_e meas_bandwidth;

  /*! Flag indicates if all the neighboring cells use antenna port 1 */
  boolean presence_antenna_port1;

  /*! neighCellConfig : Provides information related to MBSFN and TDD UL/DL configuration
      of neighbour cells of this frequency
      00: Not all neighbour cells have the same MBSFN subframe allocation as serving cell
      10: The MBSFN subframe allocations of all neighbour cells are identical to or subsets
          of that in the serving cell
      01: No MBSFN subframes are present in all neighbour cells
      11: Different UL/DL allocation in neighbouring cells for TDD compared to the serving cell
      For TDD, 00, 10 and 01 are only used for same UL/DL allocation in neighbouring cells
      compared to the serving cell.
  */
  uint8 neighbourCellConfig;

  /*! Freq specific offset (in +/- dB). Range -24..24 */
  int16 freq_offset;

  /*! Whitelisted cells */
  lte_cphy_meas_eutra_neigh_cell_list_s neigh_cells;

  /*! Blacklisted cells */
  lte_cphy_meas_eutra_blacklisted_cell_list_s blacklisted_cells;

  /*! Flag to indicate if reportCGI cell is included in the measObj */
  boolean report_cgi_cell_present;

  /*! Cell for which CGI needs to be reported */
  lte_phy_cell_id_t report_cgi_cell_id;

  /*!Parameter: Tmeasure_scc See TS 36.133 [16, 8.3.3]. The parameter is used only when an SCell is
    configured on the frequency indicated by the measObject and is in deactivated state, but the field may 
    also be signalled when an SCell is not configured. Value 160 corresponds to 160 sub-frames, 256 
    corresponds to 256 sub-frames and so on.*/
  boolean meas_cycle_scell_present;
  uint32 measCycleSCell;

  /*! Flag to indicate if gaps are needed for this measObj based on CA band combo*/
  boolean gaps_needed;

  /*! Time domain resource restriction pattern applicable to neighbour cell 
      RSRP and RSRQ measurements on the carrier frequency indicated by carrierFreq, 
      It is applicable only for intra-frequency measurement */
  lte_cphy_meas_subfm_patter_neigh_s meas_subfm_pattern_cfg_neigh;

  /*! Flag to indicate if the Meas Obj is part of NPG list*/
  boolean is_part_of_npg;
  
} lte_cphy_meas_obj_eutra_s;


/*! @brief cdma2000 type enum
*/
typedef enum
{
  /*! CDMA 2000 1xRTT */
  LTE_CPHY_CDMA_OBJTYPE_1X = 0,

  /*! CDMA 2000 HRPD */
  LTE_CPHY_CDMA_OBJTYPE_HRPD,

  /*! MAX number of the CDMA 2000 type */
  LTE_CPHY_CDMA_MAX_NUM_OBJTYPES
} lte_cphy_meas_cdma_type_e;

/*! @brief List of active cdma2000 neighbour cells
*/
typedef struct
{
  /*! Indicates which elements of cell_info list are active. A '1' indicates
      active; e.g. bit0 = 1 indicates element 0 active. */
  uint32 active_bitmask;

  /*! List of pilot pn of neighbor cells */
  sys_pilot_pn_type phy_cell_id[LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS];
} lte_cphy_meas_cdma_nbr_cell_list_s;


/*! @brief Measurement object for HRPD frequencies
*/
typedef struct
{
  /*! CDMA 2000 type */
  lte_cphy_meas_cdma_type_e  cdma_type;

  /*! Measurement carrier freq include band and channel number */
  sys_channel_type  channel;

  /*! CDMA search_window_size */
  uint8 search_window_size;

  /*! Freq specific offset (in +/- dB). Range -15..15 */
  int16 freq_offset;

  /*! To remove cell list */
  lte_cphy_meas_cdma_nbr_cell_list_s nbr_cells;

  /*! Flag indicating if the cgi cell id is included */
  boolean cgi_cell_included;

  /*! Cell for which CGI needs to be reported */
  lte_phy_cell_id_t report_cgi_cell_id;
} lte_cphy_meas_obj_cdma_s;

/*! @brief GERAN Phy Cell id structure
*/
typedef struct
{
  /* PhyCellIDGERAN Network Colour Code */
  uint8 ncc;

  /* PhyCellIDGERAN BaseStation Colour Code */
  uint8 bcc;

} lte_cphy_geran_phy_cell_id_s;


/*! @brief Measurement object for GERAN frequency groups
*/
typedef struct
{
  /*! UE mode to configure */
  lte_cphy_ue_mode_type_e ue_mode; 

  /*! The number of frequencies in freqs */
  uint32 num_freqs;

  /*! Array of gsm frequencies to measure. */
  lte_l1_gsm_arfcn_t freqs[LTE_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP];

  /*! GSM band flag identifying ARFCNs as specified in 3GPP 45.005.

    Allows UE to differentiate between GSM DCS 1800 and PCS 1900 frequencies
    which have overlapping ARFCN values.

    This flag does not specify the band for all ARFCNs in this group, but
    only serves as a differentiator for overlapping ARFCN values.  Must
    be set to either LTE_L1_GSM_BAND_DCS_1800 or LTE_L1_GSM_BAND_PCS_1900
    and is only relevant to ARFCN values which fall in either of those bands.
   */
  lte_l1_gsm_band_e gsm_band;

  /*! The Q-offset for this measurement report.  This offset
    is to be applied to the measurements prior to reporting */
  int16 q_offset;

  /*!  Bitmask specifying whether a neighbour with a particular
    Network Color Code is to be reported.  Bit "n" set to 1 means
    that a neighbour with NCC "n" should be included in the report.

    This flag is synonymous with a blacklist in other RATs
   */
  uint8 ncc_permitted;

  /*! Flag to indicate if reportCGI cell is included in the measObj */
  boolean report_cgi_cell_present;

  /* PhyCellIDGERAN of the CGI cell */
  lte_cphy_geran_phy_cell_id_s cgi_phy_cell_id_geran;

} lte_cphy_meas_obj_gsm_s;

typedef enum
{
  LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD = 0,
    LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD,
  LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_MAX
}lte_cphy_meas_obj_wcdma_type_e;

/*! @brief Measurement object for WCDMA frequency
*/
typedef struct
{
  /* Whether all information below corresponds to
  fdd mode or tdd mode 
    If W supported, then Mode will be set to FDD
    If T supported then Mode will be set to TDD
    if Both supported then Mode will be determined based on camped PLMN*/
  lte_cphy_meas_obj_wcdma_type_e wcdma_mode;

  /*! wcdma frequency to measure. */
  lte_uarfcn_t  freq;

  /*! The Q-offset for this measurement report.  This offset
    is to be applied to the measurements prior to reporting
    default - 0
    range -15..15
  */
  int16 q_offset;

  /*! Bitmask which indicates which cell indices are active. Bit 0
      corresponds to cell index 1, ..., bit 31 corresponds to cell index 32.
      implicitly it will be determined which ones were deleted
  */
  uint32 active_psc_bitmask;

  /*! cell info, mapping of the cell index & physical cell id
  Only the active cell indices will have valid psc in the range 0..511
  */

  uint16  psc[LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS];

  /*! Flag to indicate if reportCGI cell is included in the measObj */
  boolean report_cgi_cell_present;

  /*! Physical cell id of the cell for which to report the cgi*/
  uint16  psc_for_cgi_report;

  /*! Boolean to indicate whether frequency is in TDS ACQ DB or not*/
  boolean freq_in_acq_db;
}lte_cphy_meas_obj_wcdma_s;

/*! @brief Enumeration of types of measurement objects
*/
typedef enum
{
  /*! EUTRA */
  LTE_CPHY_MEAS_OBJ_TYPE_EUTRA,

  /*! WCDMA */
  LTE_CPHY_MEAS_OBJ_TYPE_WCDMA,

  /*! GERAN */
  LTE_CPHY_MEAS_OBJ_TYPE_GERAN,

  /*! CDMA */
  LTE_CPHY_MEAS_OBJ_TYPE_CDMA,

} lte_cphy_meas_obj_type_e;

/*! @brief Union of different types of measurement objects */
typedef union
{
  /*! EUTRA measurement object */
  lte_cphy_meas_obj_eutra_s eutra;

  /*! WCDMA measurement object*/
  lte_cphy_meas_obj_wcdma_s wcdma;
  
  /*! GSM Measurement Object */
  lte_cphy_meas_obj_gsm_s   gsm;


  /*! HRPD Measurement Object */
  lte_cphy_meas_obj_cdma_s  cdma;

} lte_cphy_meas_obj_info_u;


/*! @brief Measurement Object
*/
typedef struct
{
  /*! Type of measurement object */
  lte_cphy_meas_obj_type_e type;

  /*! Union of different types of measurement objects */
  lte_cphy_meas_obj_info_u        obj_info;
} lte_cphy_meas_obj_s;



/*! @brief Enumeration of different trigger types for report configs
*/
typedef enum
{
  /*! a1 */
  LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A1,

  /*! a2 */
  LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A2,

  /*! a3 */
  LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A3,

  /*! a4 */
  LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A4,

  /*! a5 */
  LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A5,

  /*! a6 */
  LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A6,

  /*! b1 */
  LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_B1,

  /*! b2 */
  LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_B2,

  /*! periodic */
  LTE_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC,

} lte_cphy_meas_report_cfg_trig_type_e;


/*! @brief Enumeration of inter-rat measurement threshold types
*/
typedef enum
{
  /*! CDMA */
  LTE_CPHY_MEAS_IRAT_THRESH_TYPE_CDMA,

  /*! GERAN */
  LTE_CPHY_MEAS_IRAT_THRESH_TYPE_GERAN,

  /*! WCDMA RSCP */
  LTE_CPHY_MEAS_IRAT_THRESH_TYPE_WCDMA_RSCP,

  /*! WCDMA EcNO */
  LTE_CPHY_MEAS_IRAT_THRESH_TYPE_WCDMA_ECNO,

} lte_cphy_meas_irat_thresh_type_e;


/*! @brief Defines a measurement threshold for IRAT measurements
*/
typedef struct
{
  /*! Threshold type */
  lte_cphy_meas_irat_thresh_type_e type;

  /*! Measurement threshold.
      For CDMA2000/GERAN threshold, range 0..63
      For WCDMA RSCP threshold, range -5..91
      WCDMA EcNO threshold, range 0..49
  */
  int16 thresh;

} lte_cphy_meas_irat_thresh_s;


/*! @brief Defines a measurement threshold for EUTRA
           For RSRP threshold, range 0..97
           For RSRQ threshold, range 0..34
*/
typedef int16 lte_cphy_meas_eutra_thresh_t;


/*! @brief Enumeration of different types for periodical report configs
*/
typedef enum
{
  /*! Report strongest cell */
  LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL,

  /*! SON */
  LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_SON,

  /*! Report cgi */
  LTE_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI,

} lte_cphy_meas_report_cfg_periodical_type_e;


/*! @brief Report config for periodical reports
*/
typedef struct
{
  /*! Purpose of this periodical report */
  lte_cphy_meas_report_cfg_periodical_type_e purpose;

} lte_cphy_meas_report_cfg_periodical_s;


/*! @brief Trigger config for a1 triggers
*/
typedef struct
{
  /*! Threshold for event a1 */
  lte_cphy_meas_eutra_thresh_t thresh;

} lte_cphy_meas_report_cfg_trig_a1_s;


/*! @brief Trigger config for a2 triggers
*/
typedef struct
{
  /*! Threshold for event a2 */
  lte_cphy_meas_eutra_thresh_t thresh;

} lte_cphy_meas_report_cfg_trig_a2_s;


/*! @brief Trigger config for a3 triggers
*/
typedef struct
{
  /*! Event offset - Range: -30..30 */
  int16 offset;

  /*! Send report on exit, if TRUE */
  boolean report_on_leave;

} lte_cphy_meas_report_cfg_trig_a3_s;


/*! @brief Trigger config for a4 triggers
*/
typedef struct
{
  /*! Threshold for event a4 */
  lte_cphy_meas_eutra_thresh_t thresh;

} lte_cphy_meas_report_cfg_trig_a4_s;


/*! @brief Trigger config for a5 triggers
*/
typedef struct
{
  /*! Threshold1 for event a5 */
  lte_cphy_meas_eutra_thresh_t thresh1;

  /*! Threshold2 for event a5 */
  lte_cphy_meas_eutra_thresh_t thresh2;

} lte_cphy_meas_report_cfg_trig_a5_s;


/*! @brief Trigger config for a6 triggers
*/
typedef struct
{
  /*! Event offset - Range: -30..30 */
  int16 offset;

  /*! Send report on exit, if TRUE */
  boolean report_on_leave;

} lte_cphy_meas_report_cfg_trig_a6_s;


/*! @brief Trigger config for b1 triggers
*/
typedef struct
{
  /*! Threshold for event b1 */
  lte_cphy_meas_irat_thresh_s thresh;

} lte_cphy_meas_report_cfg_trig_b1_s;


/*! @brief Trigger config for b2 triggers
*/
typedef struct
{
  /*! Threshold1 for event b2 */
  lte_cphy_meas_eutra_thresh_t thresh1;

  /*! Threshold2 for event b2 */
  lte_cphy_meas_irat_thresh_s thresh2;

} lte_cphy_meas_report_cfg_trig_b2_s;


/*! @brief Union of trigger configs
*/
typedef union
{
  /*! A1 */
  lte_cphy_meas_report_cfg_trig_a1_s a1;

  /*! A2 */
  lte_cphy_meas_report_cfg_trig_a2_s a2;

  /*! A3 */
  lte_cphy_meas_report_cfg_trig_a3_s a3;

  /*! A4 */
  lte_cphy_meas_report_cfg_trig_a4_s a4;

  /*! A5 */
  lte_cphy_meas_report_cfg_trig_a5_s a5;

  /*! A6 */
  lte_cphy_meas_report_cfg_trig_a6_s a6;

  /*! B1 */
  lte_cphy_meas_report_cfg_trig_b1_s b1;

  /*! B2 */
  lte_cphy_meas_report_cfg_trig_b2_s b2;

  /*! Periodic report */
  lte_cphy_meas_report_cfg_periodical_s periodical;

} lte_cphy_meas_report_cfg_trig_u;



/*! @brief Enumeration of trigger quantity types of EUTRA report configs
*/
typedef enum
{
  /*! RSRP */
  LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP,

  /*! RSRQ */
  LTE_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ,

} lte_cphy_meas_report_cfg_eutra_trig_qty_e;


/*! @brief Enumeration of report quantity types of EUTRA report configs
*/
typedef enum
{
  /*! Same as trigger quantity */
  LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_SAME,

  /*! Both RSRP and RSRQ */
  LTE_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH,

} lte_cphy_meas_report_cfg_eutra_report_qty_e;



/*! @brief Trigger config to be used as part of the measurement report configuration
*/
typedef struct
{
  /*! Trigger type */
  lte_cphy_meas_report_cfg_trig_type_e type;

  /*! Trigger parameters based on trigger type */
  lte_cphy_meas_report_cfg_trig_u data;

} lte_cphy_meas_report_cfg_trig_cfg_s;



/*! @brief Measurement report Config
*/
typedef struct
{
  /*! Report interval to use (for both periodical and event based reports), in ms */
  uint32 report_int;

  /*! Trigger config */
  lte_cphy_meas_report_cfg_trig_cfg_s trig_cfg;

  /*! Trigger quantity type for EUTRA reports */
  lte_cphy_meas_report_cfg_eutra_trig_qty_e trig_qty;

  /*! Report quantity type for EUTRA reports */
  lte_cphy_meas_report_cfg_eutra_report_qty_e report_qty;

  /*! Time to trigger (in ms) */
  uint16 time_to_trig;

  /*! Max number of cells to include in a report, range 1..8 */
  uint8 max_cells;

  /*! Num of reports to send. Should be set to LTE_CPHY_MEAS_REPORT_AMT_INFINITY 
  if infinity */
  uint8 report_amt;

  /*! Hysteresis, range 0..30 */
  uint16 hyst;

  /*! If ue-RxTxTimeDiffPeriodical is configured or not; Should be configured 
    only for reporting on the serving cell. If TRUE, trig_qty, report_qty
    and max_cells are NOT Applicable */
  boolean ue_rxtx_time_diff_periodical_enabled;

  /*! If UE is configured to read SIB1 in connected mode using A-gaps or not; 
      Valid only if trigger type is set to Periodical and purpose is set to
      reportCGI */
  boolean si_request_for_ho;

  /*! Boolean which control if UE need to report neighbouring cell */
  boolean report_add_neigh_meas_r10;
  
  /*! Boolean which indicates presence of 'includeLocationInfo-r10' for this report cfg*/
  boolean location_info_enabled;

} lte_cphy_meas_report_cfg_s;


/*! @brief Maximum number of neighbor cells that can be included in a
    connected-mode measurement report
*/
#define LTE_CPHY_MAX_CELL_REPORT  8

/*! @brief RSRP dBm value mapped to range 0..97 as defined in 36.133; negative
    value indicates invalid, not known, or not present
*/
typedef int8 lte_cphy_conn_meas_rsrp_t;

/*! @brief RSRQ dB value mapped to range 0..34 as defined in 36.133; negative
    value indicates invalid, not known, or not present
*/
typedef int8 lte_cphy_conn_meas_rsrq_t;

typedef enum
{
  LTE_CPHY_STOP_REQ_DEFAULT                                = 0,
  LTE_CPHY_STOP_REQ_REDIR_FROM_LTE_TO_X_STARTED            = 1,
  LTE_CPHY_STOP_REQ_PSHO_FROM_W_TO_LTE_ENDED               = 2,
  LTE_CPHY_STOP_REQ_PSHO_FROM_TDS_TO_LTE_ENDED             = 3,
  LTE_CPHY_STOP_REQ_REDIR_FROM_G_TO_LTE_ENDED              = 4,
  LTE_CPHY_STOP_REQ_REDIR_FROM_TDS_TO_LTE_ENDED            = 5,
  LTE_CPHY_STOP_REQ_REDIR_FROM_W_TO_LTE_ENDED              = 6,
  LTE_CPHY_STOP_REQ_RESEL_FROM_X_TO_LTE_ENDED              = 7

}lte_cphy_stop_req_cause_e;

/*! @brief Defines the type of acquisition required by RRC
*/
typedef enum
{
  /*! Acq type is based on frequency */
  LTE_CPHY_ACQ_FREQ = 0,
  /*! Acq type is based on cell */
  LTE_CPHY_ACQ_CELL = 1
}lte_cphy_acq_type_e;


/*! @brief Defines the type (RAT) of neighbor measurement results in a
    measurement report
*/
typedef enum
{
  /*! EUTRA neighbor measurement results */
  LTE_CPHY_CONN_MEAS_REPORT_TYPE_EUTRA,
  /*! WCDMA neighbor measurement results */
  LTE_CPHY_CONN_MEAS_REPORT_TYPE_WCDMA,
  /*! GERAN neighbor measurement results */
  LTE_CPHY_CONN_MEAS_REPORT_TYPE_GERAN,
  /*! CDMA2000 neighbor measurement results */
  LTE_CPHY_CONN_MEAS_REPORT_TYPE_CDMA2K
} lte_cphy_conn_meas_report_type_e;

/*! @brief Structure for EUTRA measurement result list element
*/
typedef struct
{
  /*! Physical cell ID of reported cell */
  lte_phy_cell_id_t          pci;
  /*! RSRP of reported cell (negative value indicates RSRP not reported) */
  lte_cphy_conn_meas_rsrp_t  rsrp;
  /*! RSRQ of reported cell (negative value indicates RSRQ not reported) */
  lte_cphy_conn_meas_rsrq_t  rsrq;
} lte_cphy_conn_meas_report_list_eutra_s;

/*! @brief Structure for WCDMA measurement result list element
*/
typedef struct
{
  /* Whether all information below corresponds to fdd mode or tdd mode */
  lte_cphy_meas_obj_wcdma_type_e wcdma_mode;

  /*! cell id for which measurments are reported */
  uint16   psc;

  /*! RSCP of reported cell (LTE_CPHY_WCDMA_RSCP_UNREPORTED_VALUE indicates RSCP 
  not reported)
  Range -5..91 (corresponding to -125..-20 dbm)
  */
  int8     cpich_rscp;

  /*! EcNo of reported cell (LTE_CPHY_WCDMA_ECNO_UNREPORTED_VALUE indicates EcNo 
  not reported)
  Range 0..49 (corresponding to -24..0 db)
  */
  int8     cpich_ecno;

} lte_cphy_conn_meas_report_list_wcdma_s;

/*! @brief Structure for GERAN measurement result list element
*/
typedef struct
{

  /*! GSM frequency being reported */
  lte_l1_gsm_arfcn_t freq;

  /*! GSM band of the frequency being reported

    Allows UE to differentiate between GSM DCS 1800 and PCS 1900 frequencies
    which have overlapping ARFCN values.

    This flag does not specify the band for all ARFCNs in this group, but
    only serves as a differentiator for overlapping ARFCN values.  Must
    be set to either LTE_L1_GSM_BAND_DCS_1800 or LTE_L1_GSM_BAND_PCS_1900
    and is only relevant to ARFCN values which fall in either of those bands.
   */
  lte_l1_gsm_band_e gsm_band;

  /* PhyCellIDGERAN of the cell being reported */
  lte_cphy_geran_phy_cell_id_s phy_cell_id;

  /*! Measured RSSI value for the cell being reported, required field.
    Value range (0..63)*/
  uint8 rssi;

} lte_cphy_conn_meas_report_list_gsm_s;

/*! CDMA2000 measurement result phase not specified */
#define LTE_CPHY_CONN_MEAS_REPORT_CDMA2K_PHASE_NOT_SPECIFIED (-1)

/*! @brief Structure for CDMA2000 measurement result list element
*/
typedef struct
{
  /*! pilot pn of the cell being reported */
  sys_pilot_pn_type phy_cell_id;

  /*! pilot strength of the cell being reported, required field.
    Value range (0..63) */
  uint8 pilot_strength;

  /*! pilot_pn_phase of the cell being reported, optional field. (negative value
    1 indicates pilot_pn_phase not reported). Reported value range (0..32767) */
  int16 pilot_pn_phase;

} lte_cphy_conn_meas_report_list_cdma2k_s;

/*! @brief Union containing the measurement result lists for each supported
    RAN
*/
typedef union
{
  /*! EUTRA neighbor measurement results list */
  lte_cphy_conn_meas_report_list_eutra_s   eutra[LTE_CPHY_MAX_CELL_REPORT];
  /*! WCDMA neighbor measurement results list */
  lte_cphy_conn_meas_report_list_wcdma_s    wcdma[LTE_CPHY_MAX_CELL_REPORT];
  /*! GERAN neighbor measurement results list */
  lte_cphy_conn_meas_report_list_gsm_s   gsm[LTE_CPHY_MAX_CELL_REPORT];
  /*! CDMA2000 neighbor measurement results list */
  lte_cphy_conn_meas_report_list_cdma2k_s  cdma2k[LTE_CPHY_MAX_CELL_REPORT];
} lte_cphy_conn_meas_report_list_u;

/*! @brief Structure that defines the measurement result list, used in
    measurement reports
*/
typedef struct
{
  /*! Number of results in list. Range: 0..LTE_CPHY_MAX_CELL_REPORT */
  uint8                             size;
  /*! Measurement results list type, e.g. EUTRA, WCDMA, etc. */
  lte_cphy_conn_meas_report_type_e  type;
  /*! Measurement results list; union of all types */
  lte_cphy_conn_meas_report_list_u  list;
} lte_cphy_conn_meas_report_list_s;


/*! @brief Defines filter coefficient value.
*/
typedef uint8 lte_cphy_meas_filter_coeff_t;

#define LTE_CPHY_MEAS_FILTER_COEFF_DISABLED 0


/*! @brief Filter coefficients for EUTRA measurements
*/
typedef struct
{
  /*! FC for RSRP, 0 indicates disabled */
  lte_cphy_meas_filter_coeff_t fc_rsrp;
  /*! FC for RSRQ, 0 indicates disabled */
  lte_cphy_meas_filter_coeff_t fc_rsrq;

} lte_cphy_meas_qty_cfg_eutra_s;

/*! measurement quantity for WCDMA FDD */
typedef enum
{
  LTE_CPHY_MEAS_WCDMA_QTY_TYPE_CPICH_RSCP,
  LTE_CPHY_MEAS_WCDMA_QTY_TYPE_CPICH_ECNO
} lte_cphy_meas_wcdma_fdd_qty_type_e;

/*! measurement quantity for WCDMA TDD */
typedef enum
{
  LTE_CPHY_MEAS_WCDMA_QTY_TYPE_PCCPCH_RSCP
} lte_cphy_meas_wcdma_tdd_qty_type_e;

/*! @brief Defines the type of meas quantity for GSM
*/
typedef enum
{
  /*! RSSI */
  LTE_CPHY_MEAS_QTY_GSM_RSSI = 0

}lte_cphy_meas_qty_gsm_e;

/*! @brief Filter coefficients for GSM measurements
*/
typedef struct
{
  /*!< Meas Quantity for GSM */
  lte_cphy_meas_qty_gsm_e meas_qty;
  /*!< FC for GSM, default fc2 */
  lte_cphy_meas_filter_coeff_t fc;

} lte_cphy_meas_qty_cfg_gsm_s;

/*! @brief quantity config for wcdma measurements
*/
typedef struct
{
  /*! Flag to indicate if quantityConfigUTRA rel 10 is present. 
    TRUE implies, UE applys filter coefficient specified in QuantityConfigUTRA 
    for the measurement specified in measQuantityUTRA-FDD and apply filterCoefficient2-FDD-r-10 
    for the other measurement not specified in measQuantityUTRA-FDD 
    and UE will send measurement report containing both ecno and rscp

    FALSE implies UE applys filter coefficient specified in QuantityConfigUTRA
    for both ecno and rscp and UE will send measurement report containing
    only the measurement specified in measQuantityUTRA-FDD*/

  boolean      qty_cfg_rel10_present;

  /*! filter coefficient for rscp, default fc4 */
  lte_cphy_meas_filter_coeff_t     rscp_fc;

  /*! filter coefficient for ecno, default fc4 */
  lte_cphy_meas_filter_coeff_t     ecno_fc;

  /*! indicates what FDD quantity has to be measured */
  lte_cphy_meas_wcdma_fdd_qty_type_e   qty_type_fdd;

  /*! indicates what TDD quantity has to be measured */
  lte_cphy_meas_wcdma_tdd_qty_type_e qty_type_tdd;

} lte_cphy_meas_qty_cfg_wcdma_s;

/*! @brief Quantity configs for CDMA
*/
typedef enum
{
  /*! No cdma qty config */
  LTE_CPHY_MEAS_QTY_CFG_CDMA_NONE=0,

  /*! Pilot strength measurement */
  LTE_CPHY_MEAS_QTY_CFG_PILOT_STRENGTH,

  /*! PN phase and pilot strength measurement */
  LTE_CPHY_MEAS_QTY_CFG_PN_PHASE_PILOT_STRENGTH,

  /*! Maximum number of cfgs */
  LTE_CPHY_MEAS_QTY_CFG_MAX_NUM_CFGS,
}lte_cphy_meas_qty_cfg_cdma_e;

/*! @brief Quantity configs for all RATs
*/
typedef struct
{
  /*! EUTRA quantity config */
  lte_cphy_meas_qty_cfg_eutra_s eutra;

  /*! WCDMA quantity config */
  lte_cphy_meas_qty_cfg_wcdma_s wcdma;

  /*! GERAN quantity config */
  lte_cphy_meas_qty_cfg_gsm_s gsm;

  /*! cdma quantity config */
  lte_cphy_meas_qty_cfg_cdma_e cdma;

  /*! Quantity configs for other RATs to be added later */

} lte_cphy_meas_qty_cfg_s;

/*! @brief Measurement gap patterns
*/
typedef enum
{
  /*! Pattern ID 0 - repeats every 40ms */
  LTE_CPHY_MEAS_GAP_PATTERN_ID_0 = 0,
  /*! Pattern ID 1 - repeats every 80ms */
  LTE_CPHY_MEAS_GAP_PATTERN_ID_1,
  /*! Max Pattern ID */
  LTE_CPHY_MEAS_GAP_MAX_PATTERNS
} lte_cphy_meas_gap_pattern_type_e;

/*! @brief Measurement gap configs
*/
typedef struct
{
  /*! Gap config enabled or not */
  boolean                              gap_cfg_enabled;
  /*! Gap pattern */
  lte_cphy_meas_gap_pattern_type_e     pattern_id;
  /*! Gap offset (Range either 0-39 or 0-79 depending on pattern id) */
  uint8                                gap_offset;
} lte_cphy_meas_gap_cfg_s;


/*! @brief Speed scaling parameters for connected mode EUTRA measurements
*/
typedef struct
{
  /*! False if speed scaling is disabled, True otherwise */
  boolean enabled;

  /*! Mobility State config params */
  lte_cphy_meas_mobility_state_config_param_s config_params;

  /*! Time to trigger scaling factor for medium mobility */
  lte_cphy_meas_time_sf_e time_to_trig_sf_medium;

  /*! Time to trigger scaling factor for high mobility */
  lte_cphy_meas_time_sf_e time_to_trig_sf_high;

} lte_cphy_conn_meas_speed_scaling_cfg_s;


/*! @brief Connected mode measurement configuration
*/
typedef struct
{
  /*! Measurement ID list. Index 0 corresponds to meas ID 1, ..., index 31
      corresponds to meas ID 32. */
  lte_cphy_meas_id_s meas_id_list[LTE_CPHY_MEAS_MAX_MEAS_ID];

  /*! Measurement object list. Index 0 corresponds to meas object 1, ...,
      index 31 corresponds to meas object 32. */
  lte_cphy_meas_obj_s meas_obj_list[LTE_CPHY_MEAS_MAX_MEAS_OBJ_ID];

  /*! Reporting configuration list. Index 0 corresponds to report config 1, ...,
      index 31 corresponds to report config 32. */
  lte_cphy_meas_report_cfg_s report_cfg_list[LTE_CPHY_MEAS_MAX_REPORT_CFG_ID];

  /*! Quantity Config */
  lte_cphy_meas_qty_cfg_s qty_cfg;

  /*! Gap configuration */
  lte_cphy_meas_gap_cfg_s gap_cfg;

  /*! S-Measure (-140 to -44), 0 indicates disabled */
  int16 s_measure;

  /*! Speed scaling config */
  lte_cphy_conn_meas_speed_scaling_cfg_s sc_cfg;

  /*! Number of TDS neighbours in active list (associated with valid Meas Ids)
    For TDS supported network, the range can be 0 to maximum RRC layer count.
    For Non TDS supported network, num_active_tds_freq is same as 
    number of tds frequencies configured (i.e. no dormant list)*/
  uint8 num_active_tds_freq;
  
} lte_cphy_conn_meas_cfg_info_s;


/*! @brief Enumeration for NMR report types
*/
typedef enum
{

  /* Initial value - should be used just for initialization  */
  LTE_CPHY_NMR_REPORT_NONE = 0x00,
  /* Used for requesting report info containing LTE intra frequency neighbors */
  LTE_CPHY_NMR_REPORT_INTRA_FREQ = 0x01,
  /* Used for requesting report info containing LTE inter frequency neighbors */
  LTE_CPHY_NMR_REPORT_INTER_FREQ = 0x02,
  /* Used for requesting report info containing WCDMA neighbors */
  LTE_CPHY_NMR_REPORT_IRAT_WCDMA = 0x03,
  /* Used for requesting report info containing GERAN neighbors */
  LTE_CPHY_NMR_REPORT_IRAT_GERAN = 0x04,
  /* Used for requesting report info containing C2K neighbors */
  LTE_CPHY_NMR_REPORT_IRAT_CDMA2K = 0x05

}lte_cphy_nmr_report_type_e;


/*===========================================================================

                   CPHY SAPS( REQ, CNF, IND)

===========================================================================*/

/*! @defgroup cphy_sap Control Plane (CPHY) Interface

  @brief This module describes the control interface to L1

  @detail The umids for this module are defined in @ref umids in the
          CPHY section.  The structures associated with the messages
          are defined in this module, as well as the documentation
          describing these messages

          This interface is generated from
          $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_cphy_msg.h#1 $ //source/qcom/qct/modem/lte/ML1/l1_common/main/latest/inc/lte_cphy_sap.h#46 $

          The following log describes the recent changes of the interface:
          @verbatim
          $Log:$

          @endverbatim

  @{*/

typedef enum
{
  LTE_CPHY_START_REQ_DEFAULT                                  = 0,
  LTE_CPHY_START_REQ_PSHO_FROM_W_TO_LTE_STARTED               = 1,
  LTE_CPHY_START_REQ_PSHO_FROM_TDS_TO_LTE_STARTED             = 2,
  LTE_CPHY_START_REQ_RESEL_FROM_X_TO_LTE_STARTED              = 3,
  LTE_CPHY_START_REQ_REDIR_FROM_G_TO_LTE_STARTED              = 4,
  LTE_CPHY_START_REQ_REDIR_FROM_TDS_TO_LTE_STARTED            = 5,
  LTE_CPHY_START_REQ_REDIR_FROM_W_TO_LTE_STARTED              = 6,
  LTE_CPHY_START_REQ_REDIR_FROM_DO_TO_LTE_STARTED             = 7,
  LTE_CPHY_START_REQ_SSIM_RESEL_FROM_1X_TO_LTE_STARTED        = 8
}lte_cphy_start_req_cause_e;

/*! @brief
  Structure for LTE capability info
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Below info is valid only of this flag is set */  
  boolean valid;

  /*! UE Category */  
  uint8   ue_cat;

  /*! Max CC supported on DL */
  uint8   max_num_dl_carriers; 

  /*! Max CC supported on UL */
  uint8   max_num_ul_carriers; 

} lte_ue_cap_info_s;


/*! @brief
  This message is used to signal ML1 to start handling commands in LTE mode of
  operation.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Device mode type */
  sys_modem_device_mode_e_type  device_mode;

  /*!< Number of active SIMs */
  sys_modem_dual_standby_pref_e_type  standby_mode; 
  /*! UE mode type */
  lte_cphy_ue_mode_type_e  ue_mode;

  /* Start cause for ML1 to decide TRM exchange */
  lte_cphy_start_req_cause_e    start_cause; 

  /* LTE earfcn for ML1 for TRM reservation */
  lte_earfcn_t    earfcn;

  /* LTE UE capability info */
  lte_ue_cap_info_s    cap_info;  

  /* Boolean status for DDS sub*/
  boolean dds_sub;

} lte_cphy_start_req_s;


/*! @brief
  This message confirms the start request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Whether the device is in single mode */
  boolean              single_rx_mode;
  /*! Whether or not the start was successful */
  errno_enum_type      status;
} lte_cphy_start_cnf_s;

/*! @brief
  This message is used to signal ML1 about the NV params shared between RRC/ML1
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /* Following fields are populated based on EFS files if present or set to defaults */
  
  lte_3gpp_release_ver_e lte_rrc_3gpp_release_ver;  /*!< Variable denoting the LTE 3GPP Spec Release */
   
  boolean axgp; /*!< Var for indicating that axgp band is supported*/

  lte_l1_extnd_bands_updated_info_s  lte_rrc_update_band; /*!< Bands updated info via EFS ("update_band_range") */

  lte_l1_bs_bw_supported_s  lte_rrc_bs_bw_supported; /*!< BW supported info via EFS ("bs_bw_supported") */

} lte_cphy_nv_update_req_s;

/*! @brief
  This message is used to signal ML1 to stop handling commands in LTE mode of
  operation.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Specific cause for stop */
  lte_cphy_stop_req_cause_e stop_cause;

} lte_cphy_stop_req_s;


/*! @brief
  This message is used to confirm for stop request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether or not the stop was successful */
  errno_enum_type      status;

  /*! CDMA time info: Valid only if suspended for the same cause */
  lte_stmr_cdma_rtc_pair     cdma_time_info;
} lte_cphy_stop_cnf_s;


/*! @brief
  This message is used to command L1 to acquire a cell.

  L1 performs a cell search and attempts to decode PBCH on the best detected
  cell.  If L1 acquires a cell that is listed in the blacklist, it will attempt
  to acquire another cell. L1 will attempt to acquire at most N+1 cells, where
  N is the number of blacklisted cells.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Transaction ID Range: 0..255 */
  uint8 trans_id;

  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;

  /*! 0..65535 (allowing for expansion) */
  lte_earfcn_t  earfcn;

  /*! the bandwidth info from frequency scan */
  lte_bandwidth_e      bw;

  /*! Configuration to apply partial roaming restriction */
  lte_cphy_partial_roaming_restriction_e restriction_cfg;

  /*! Indicates the number of cells in the blacklist Range: 0..4 */
  uint8 num_black_cells;

  /*! Cell blacklist; L1 must try to acquire a cell not in this list; used for
      e.g. barred cells, can be empty */
  lte_phy_cell_id_t blacklist[LTE_CPHY_MAX_ACQ_BLACKLISTED_CELLS];

  /*! Acq_type indictates the type of acquisition i.e. cell specific or any cell on a particular freq etc */
  lte_cphy_acq_type_e acq_type;

  /*! Cell id to acquire if acq type has been set to LTE_CPHY_ACQ_CELL
      This field is irrelevant for acq type LTE_CPHY_ACQ_FREQ
      Cell id provided in thie field will not be a part of the RRC populated
      blacklist field. */
  lte_phy_cell_id_t acq_cell_id[LTE_CPHY_ACQ_MAX_PCI];
      
  uint8                 pci_count;

  /*! Timing info to skip init ACQ search */
  uint64                ref_time;

  /*! timeout value for the procedure (in seconds) */
  uint32 t_timeout;

} lte_cphy_acq_req_s;


/*! @brief
  This message is to confirm and carry the results of the requested acquisition
  attempt. L1 returns results for the acquired cell if acquisition is
  successful.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Transaction ID; matches the one in the request */
  uint8 trans_id;

  /*! Acquisition success/failure */
  errno_enum_type      status;

  /*! Confirmation of the requested band */
  sys_sband_lte_e_type band;

  /*! Confirmation of the requested EARFCN */
  lte_earfcn_t earfcn;

  /*! Cell info for acquired cell; only valid if status indicates acquisition
      success */
  lte_cphy_acq_cell_info_s acquired_cell;

  /*! is cell barred */
  boolean cell_barred;
} lte_cphy_acq_cnf_s;

/*! @brief
  This message is used for configuring UL/DL channels for common configuration.
  This message allows multiple channel setup operations in a single primitive.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! The information of the current serving cell, used for sanity purposes */
  lte_cphy_cell_info_s serving_cell_info;
  lte_cphy_common_cfg_req_param_s common_cfg_param;
} lte_cphy_common_cfg_req_s;


/*! @brief
  This message is used for configuring idle drx parameters.
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;
  /*! The information of the current serving cell, used for sanity purposes */
  lte_cphy_cell_info_s serving_cell_info;
  lte_cphy_idle_drx_param_s idle_drx_param;
} lte_cphy_idle_drx_cfg_req_s;

/*! Sent from ML1 RFMGR->L2.
    RFMGR TRM checks if to allow an unlock request*/
typedef struct
{
  msgr_hdr_struct_type                msgr_hdr;  /*!< message header */

  /*! When is unlock requested - 
      in the unit of subframe from last SF_IND. 
      This also includes duration needed for suspend procedure at ML1 */
  lte_sfn_s                           unlock_by_start_time;

  /*! How long is unlock requested for. In units of msecs.
      This also includes the warm up time needed at ML1 */
  uint16                              unlock_by_duration;

  /*! Unlock requested for which client. 0 - 1x 8 - GSM*/
  uint8                               winning_client_id;

  /*! If the TRM priority in unlock is higher than EMBMS high, set this to TRUE*/
  boolean                             winning_client_trm_priority_high;
} lte_cphy_trm_unlock_allowed_req_s;

/*! Sent from L2-> ML1 RFMGR
    L2 informs RFMGR if ML1 can proceed a unlock request, or ignore the request*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;        /*!< message header */

  /*! TRUE - proceed with unlock request
      FALSE - Ignore unlock request*/
  boolean                          unlock_is_allowed;
} lte_cphy_trm_unlock_allowed_cnf_s; 

/*! @brief
  This message is used for configuring idle mode measurement parameters.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The information of the current serving cell, used for sanity purposes */
  lte_cphy_cell_info_s serving_cell_info;

  /*! Measurement config information */
  lte_cphy_idle_meas_cfg_info_s meas_cfg;

} lte_cphy_idle_meas_cfg_req_s;


/*! @brief
  This message confirms the idle mode measurement config request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether or not the configuration was successfully applied */
  errno_enum_type status;

} lte_cphy_idle_meas_cfg_cnf_s;

/*! @brief
  This message is used for configuring CDMA system time only. This is used
  for passing CDMA system time during periodic SIB8 reading or during reverse
  time transfer.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! If CDMA system time is enabled or not. Disabled when not signaled or when
    signaled value fails validation */
  boolean enabled;

  /*! CDMA system time type */
  lte_cphy_irat_cdma_system_time_source_type_e source_type;

  /*! CDMA system time info */
  lte_cphy_irat_cdma_system_time_info_u irat_cdma_system_time_info;

  /*! The cell on which SIB8 is received/reverse time transfer was peformed on */
  lte_cphy_cell_info_s cell_info;

} lte_cphy_irat_cdma_system_time_chg_req_s;

/*! @brief
  This message is used for configuring UTC system time only. This is used
  for passing UTC system time during periodic SIB16 reading.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! If UTC system time is enabled or not. Disabled when not signaled or when
    signaled value fails validation */
  boolean enabled;

  /*! UTC system time info */
  lte_cphy_utc_time_info_s utc_time_info;

  /*! The cell on which SIB16 is received */
  lte_cphy_cell_info_s cell_info;

} lte_cphy_utc_time_update_req_s;

/*! @brief
  This message confirms the configuring UTC system time request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether or not the configuration was successfully applied */
  errno_enum_type status;

} lte_cphy_utc_time_update_cnf_s;

/*!
  @brief
  An enum describing the bit positions in the other_mod_bitmask field in
  lte_cphy_conn_meas_cfg_req_s
  */
enum
{
  LTE_CPHY_MEAS_CFG_BIT_QTY_CFG         = 0x01,
  LTE_CPHY_MEAS_CFG_BIT_S_MEASURE       = 0x02,
  LTE_CPHY_MEAS_CFG_BIT_GAP_CFG         = 0x04,
  LTE_CPHY_MEAS_CFG_BIT_SPEED_SCALING   = 0x08,
};

/*! @brief
  This message is used for configuring connected mode measurement parameters.
  Note: The meas_cfg_ptr points to a buffer that is maintained by RRC, ML1
  is expected to copy the relevent configuration as needed, prior to sending the
  LTE_CPHY_CONN_MEAS_CFG_CNF msg. After the cnf msg is received by RRC, ML1 should
  not attempt to access this pointer.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The information of the current serving cell, used for sanity purposes */
  lte_cphy_cell_info_s serving_cell_info;

  /*! Bitmask which indicates which measurement objects are valid. Bit 0
      corresponds to meas object 1, ..., bit 31 corresponds to
      meas object 32. */
  uint32 meas_obj_valid_bitmask;

  /*! Bitmask which indicates which measurement ids are valid. Bit 0
      corresponds to meas ID 1, ..., bit 31 corresponds to meas ID 32. */
  uint32 meas_id_valid_bitmask;

  /*! Bitmask which indicates which report configs are valid. Bit 0
      corresponds to report config 1, ..., bit 31 corresponds to
      report config 32. */
  uint32 report_cfg_valid_bitmask;

  /*! Bitmask which indicates which meas objects have been touched
      (add/modify/delete). Bit 0 corresponds to meas object 1, ...,
      bit 31 corresponds to meas object 32. */
  uint32 meas_obj_mod_bitmask;

  /*! Bitmask which indicates which meas ids have been touched
      (add/modify/delete). Bit 0 corresponds to meas ID 1, ..., bit 31
      corresponds to meas ID 32. */
  uint32 meas_id_mod_bitmask;

  /*! Bitmask which indicates which report configs have been touched
      (add/modify/delete). Bit 0 corresponds to report config 1, ..., bit 31
      corresponds to report config 32. */
  uint32 report_cfg_mod_bitmask;

  /*! Bitmask which indicates which other cfgs have been modified */
  uint32 other_mod_bitmask;

  /*! Boolean which indicates whether reporting can be done or not*/
  boolean block_meas_rpt_trigger;

  /*! Pointer to measurement config information */
  lte_cphy_conn_meas_cfg_info_s *meas_cfg_ptr;

} lte_cphy_conn_meas_cfg_req_s;


/*! @brief
  This message confirms the conn mode measurement config request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether or not the configuration was successfully applied */
  errno_enum_type status;

} lte_cphy_conn_meas_cfg_cnf_s;


/*! @brief Measurement report CA serving freq list structure
*/
typedef struct
{
  /*! Physical cell ID of reported cell */
  lte_phy_cell_id_t          pci;
  /*! RSRP of reported  neigh cell (negative value indicates RSRP not reported) */
  lte_cphy_conn_meas_rsrp_t  neighCell_rsrp;
  /*! RSRQ of reported  neigh cell (negative value indicates RSRQ not reported) */
  lte_cphy_conn_meas_rsrq_t  neighCell_rsrq;
} lte_cphy_conn_meas_report_best_neighCell_s;


/*! @brief Measurement report CA serving freq list structure
*/
typedef struct
{
  /*! RSRP of serving cell */
  lte_cphy_conn_meas_rsrp_t         serving_rsrp;
  /*! RSRQ of reported cell */
  lte_cphy_conn_meas_rsrq_t         serving_rsrq;

} lte_cphy_conn_meas_report_serving_cell_s;

/*! @brief Measurement report CA serving freq list structure
*/
typedef struct
{
  /*ServCellIndex-r10   ::=   INTEGER   (0 .  . 7 )  */
  uint8 serv_cell_index;
  
  /*L1 to look at meas_report_scell structure only when meas_report_scell_present = TRUE*/
  boolean meas_report_scell_present;
  lte_cphy_conn_meas_report_serving_cell_s meas_report_scell;

  /*L1 to look at meas_report_best_neighCell_present structure only when meas_report_best_neighCell = TRUE*/
  boolean meas_report_best_neighCell_present;
  lte_cphy_conn_meas_report_best_neighCell_s meas_report_best_neighCell;
} lte_cphy_conn_meas_report_scell_s;

/*! @brief Measurement report CA serving freq list structure
*/
typedef struct
{
  /*maxServCell-r10   INTEGER   ::= 4   --   Maximum   number   of   Serving   cells  
   Note:- there is issue with RRC ASN.1 it says maxServCell as 5, Spec will be clarified soon*/
  uint8 num_scell_list;
  lte_cphy_conn_meas_report_scell_s meas_report_scell_list[LTE_CPHY_MAX_SERVING_CELL];
} lte_cphy_conn_meas_report_scell_freq_list_s;


/*! @brief UE Rx-Tx time diff meas result structure
*/
typedef struct
{
  /*! ue-RxTxTimeDiffResult for the serving cell; Range (0..4095) */
  uint32 ue_rxtx_timediff_result;
  /*! Current SFN in which UE rx_tx time diff meas result is sent to RRC */
  uint32 current_sfn;

} lte_cphy_rxtx_timediff_meas_result_s;

/*! @brief Measurement report message structure
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Measurement ID for report. Range 1..32. */
  lte_cphy_meas_id_t                meas_id;
  /*! TRUE if the trigger type is periodical and this is the last report. This
      indicates that ML has autonomously deleted the meas ID from the VMC. */
  boolean                           auto_delete_meas_id;
  /*! RSRP of serving cell */
  lte_cphy_conn_meas_rsrp_t         serving_rsrp;
  /*! RSRQ of reported cell */
  lte_cphy_conn_meas_rsrq_t         serving_rsrq;
  /*! Neighbor measurement results list */
  /*! For inter-freq and irat NMR reports RRC will not read this 
  but will look at n_freq_report_ptr */
  lte_cphy_conn_meas_report_list_s  neigh_results;
  /*! If UE rxtx time diff result is included for serving cell */
  boolean rxtx_timediff_included;
  /*! UE rxtx time diff result for serving cell */
  lte_cphy_rxtx_timediff_meas_result_s rxtx_result;

  lte_cphy_conn_meas_report_scell_freq_list_s meas_report_ca_srv_freq_list;
} lte_cphy_conn_meas_report_ind_s;

/*! @brief GSM NMR report message structure
*/
typedef struct
{
  /*! gsm frequency */
  lte_l1_gsm_arfcn_t freq;
  
  /*! Number of results in list. Range: 0..LTE_CPHY_MAX_CELL_REPORT */
  uint8 size;
  
  /*! GERAN neighbor measurement results list */
  lte_cphy_conn_meas_report_list_gsm_s gsm[LTE_CPHY_MAX_CELL_FOR_NMR_REPORT];

} lte_rrc_nmr_geran_info_s;

/*! @brief WCDMA NMR report message structure
*/
typedef struct
{
  /*! wcdma frequency */
  lte_uarfcn_t freq;
  
  /*! Number of results in list. Range: 0..LTE_CPHY_MAX_CELL_REPORT */
  uint8 size;
  
  /*! WCDMA neighbor measurement results list */
  lte_cphy_conn_meas_report_list_wcdma_s wcdma[LTE_CPHY_MAX_CELL_FOR_NMR_REPORT];

} lte_rrc_nmr_utra_info_s;

/*! @brief LTE NMR report message structure
*/
typedef struct
{
  /*! lte Frequency */
  lte_earfcn_t freq;
  
  /*! Number of results in list. Range: 0..LTE_CPHY_MAX_CELL_REPORT */
  uint8 size;
  
  /*! EUTRA neighbor measurement results list */
  lte_cphy_conn_meas_report_list_eutra_s eutra[LTE_CPHY_MAX_CELL_FOR_NMR_REPORT];

} lte_rrc_nmr_eutra_info_s;

/*! @brief IRAT neighbor freq measurement info
*/
typedef union
{  
  /*! EUTRA neighbor freq measurement info */
  lte_rrc_nmr_eutra_info_s eutra_report[LTE_CPHY_MEAS_MAX_NUM_FREQ];

  /*! UTRA neighbor freq measurement info */
  lte_rrc_nmr_utra_info_s utra_report[LTE_CPHY_MEAS_MAX_NUM_FREQ];

  /*! GERAN neighbor freq measurement info */
  lte_rrc_nmr_geran_info_s geran_report[LTE_CPHY_MEAS_MAX_NUM_FREQ];

  /*! TBD: Add similar structure for CDMA later if necessary */
} lte_cphy_nmr_rat_info_u;

/*! @brief NMR report message structure for n freq
*/
typedef struct
{
  /*! Number of reports for inter-freq eutra or inter-RAT measurements */
  uint8 num_report;

  /*! Indicates the type of report populated */
  lte_cphy_nmr_report_type_e report_type;

  /*! RAT neighbor freq measurement info */
  lte_cphy_nmr_rat_info_u rat_info;

} lte_cphy_nmr_n_freq_report_s;

/*! @brief NMR request message structure
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Indicate the type of report requested */
  lte_cphy_nmr_report_type_e report_type;
  /*! Pointer to the RRC memory for NMR n_freq_report 
  Must be used for report_type = LTE_CPHY_NMR_REPORT_INTER_FREQ,
                                 LTE_CPHY_NMR_REPORT_IRAT_WCDMA,
                                 LTE_CPHY_NMR_REPORT_IRAT_GERAN
  Must not be used for LTE_CPHY_NMR_REPORT_INTRA_FREQ and others
  */
  lte_cphy_nmr_n_freq_report_s *n_freq_report_ptr;
} lte_cphy_nmr_info_req_s;

/*! @brief
  NMR confirm message structure.  The meas_id field shall be set to 1 for all
  reports triggered by NMR (according to 36.111 section 8.22).
  If for any reason ML1 cannot send a valid report, it shall indicate this
  by setting the serving cell RSRP and RSRQ to -1.
*/
typedef lte_cphy_conn_meas_report_ind_s lte_cphy_nmr_info_cnf_s;

/*! @brief Serving cell measurement results
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*!Boolean to indicate if ML1 should report best neighbor cell measurements */
  boolean report_add_neigh_meas_r10;

} lte_cphy_serv_cell_meas_req_s;

/*! @brief Serving cell measurement results
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! set to E_SUCCESS if the cnf is valid; if set to
   * E_NOT_AVAILABLE, the srv cell might have changed since
   * the req was sent and RRC should disregard this cnf */
  errno_enum_type                   status;

  /*! RSRP of serving cell */
  lte_cphy_conn_meas_rsrp_t         serving_rsrp;
  /*! RSRQ of reported cell */
  lte_cphy_conn_meas_rsrq_t         serving_rsrq;

  /*! CA Serving freq list */
  lte_cphy_conn_meas_report_scell_freq_list_s meas_report_ca_srv_freq_list;

} lte_cphy_serv_cell_meas_cnf_s;

/*! @brief
  This message is used to confirm common channel configuration request.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The success of the configuration if the status was not success no
      configurations were applied */
  errno_enum_type      status;

  /*! @brief A bit-mask specifying the per-configuration results of the
    common configuration request.

    Bit turned on means configuration was valid. */
  lte_cphy_common_cfg_mask_t common_cfg_validity;
} lte_cphy_common_cfg_cnf_s;

/*! @brief This message is used for configuring UL/DL channels for dedicated
  configuration.

  This message can only be sent after a common configuration has been
  sent and should only be sent when the UE is in connected mode.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The information of the current serving cell, used for sanity purposes */
  lte_cphy_cell_info_s serving_cell_info;

  /*! Identifies whether this command is specifying the default parameters
      or is a configuration update from the network. */
  boolean is_default_cfg;

  lte_cphy_dedicated_cfg_req_param_s dedicated_cfg_param;

} lte_cphy_dedicated_cfg_req_s;

/*! @brief
  This message is used to confirm dedicated channel configuration request.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The success of the configuration */
  errno_enum_type      status;

  /*! A bit-mask specifying the per-subconfiguration validity of the
    dedicated configuration request.

    Bit turned on means configuration was successful. */
  lte_cphy_dedicated_cfg_mask_t dedicated_cfg_validity;
} lte_cphy_dedicated_cfg_cnf_s;

/*! @brief
  This message is used for configuring UL/DL channels for HO.
  This message allows multiple channel setup operations in a single primitive.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! The source cell information */
  lte_cphy_cell_info_s source_cell_info;
  /*! Target cell information */
  lte_cphy_ho_target_cell_info_s ho_target_cell_info;

  /*! Indicates if prach_mask_index is configured (Contention Free RACH)
          If set to FALSE, HO RACH will be contention based */
   boolean prach_mask_index_present;

  /*! RA PRACH Mask Index from dedicated RACH cfg; Range (0 to 15) */
  uint8 prach_mask_index;

  /*! Random Access Preamble timing for preamble format Range: 0-63
      Defined in 36.211 Table 5.7.1-2
      Defined in 36.311 Section 6.3.2 */
  uint8 prach_cfg_index;

  /*! Common configuration to be used for the handover */
  lte_cphy_common_mobility_req_param_s common_mobility_cfg_param;
  /*! Dedicated configuration for the handover */
  lte_cphy_dedicated_cfg_req_param_s dedicated_cfg_param;

} lte_cphy_handover_req_s;

/*! @brief
  This message is used to confirm channel configuration in HO Config request.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The success of the configuration if the status was not success no
   configurations were applied */
  errno_enum_type      status;
  /*! A bit-mask specifying the per-configuration common mobility
    validity paramter validity.

    Bit turned on means configuration was valid. */
  lte_cphy_common_mobility_cfg_mask_t com_mob_parm_validity;

  /*! A bit-mask specifying the per-configuration dedicated parameter
    validity.

    Bit turned on means configuration was valid. */
  lte_cphy_dedicated_cfg_mask_t dedicated_parm_validity;
} lte_cphy_handover_cnf_s;

/*! @brief
  This message is used for releasing the dedicated paramters and stopping
  the dedicated procedures when moving to IDLE state.

  Upon receiving this command L1 will drop all dedicated parameters and
  continue in idle mode, with the provided common parameters.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  lte_cphy_cell_info_s cell_info;
} lte_cphy_con_release_req_s;

/*! @brief
  Parameter struct for cell info describing cell id, frequency and bandwith
*/
typedef struct
{
  /*! The cell id this configuration applies to */
  lte_phy_cell_id_t     phy_cell_id;
  /*! The frequency this configuration applies to */
  lte_earfcn_t          dl_cell_frequency;
  /*! System bandwidth */
  lte_bandwidth_e       bandwidth;
  /*! Timing info to skip init ACQ search */
  uint64                ref_time;
}lte_cphy_conn_release_acq_cell_info_s;

/*! @brief
  This message is used to confirm that L1 has processed the release request.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                  msgr_hdr;
  /*! The status of the release request*/
  errno_enum_type                       status;
  /*! Connection Release to do ACQ */
  boolean                               is_acq_needed;
  /*! ACQ cell info*/
  lte_cphy_conn_release_acq_cell_info_s acq_cell_info;
} lte_cphy_con_release_cnf_s;


/*! @brief
  This message to abort and clean up any active operations and transitions
  to Inactive state
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
} lte_cphy_abort_req_s;


/*! @brief
  This message is used to confirm the abort reqeust.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Abort successful or not */
  errno_enum_type      status;
} lte_cphy_abort_cnf_s;


/*! @brief
  This message requests band scan to be performed by L1.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                    msgr_hdr;

  /*! Transaction identifier Range: 0..255 */
  uint8                                   trans_id;

  /*! Configuration to apply partial roaming restriction */
  lte_cphy_partial_roaming_restriction_e  restriction_cfg;

  /*! Bands to scan */
  lte_cphy_band_scan_req_params_s         req_bands;
} lte_cphy_band_scan_req_s;


/*! @brief
  This message confirms the band scan request and contains the results
  of the operation.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type             msgr_hdr;

  /*! List of scan results. band_ptr points to a static array of
      lte_cphy_band_scan_result_s of size LTE_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS,
      one for each band requested; order matches requested list of bands struct.
      THERE IS NO MEMORY ALLOCATION TO BE DONE. */
  lte_cphy_band_scan_result_s      *band_ptr;

  /*! Transaction ID; matches the one in the request */
  uint8                            trans_id;

  /*! Number of band results; matches number of bands requested
      1.. LTE_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS */
  uint8                            num_bands;
  
  /*! Status of operation */
  IxErrnoType                        status;
} lte_cphy_band_scan_cnf_s;


/*! @brief
  This message requests system scan to be performed by L1.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                    msgr_hdr;

  /*! Transaction ID */
  uint8                                   trans_id;

  /*! Configuration to apply partial roaming restriction */
  lte_cphy_partial_roaming_restriction_e  restriction_cfg;

  /*! Requested systems */
  lte_cphy_system_scan_req_params_s       req_systems;
} lte_cphy_system_scan_req_s;

/*! @brief
  This message confirms the system scan request and contains the results
  of the operation. Note L1 eliminates any results below noise thresh, and
  aborts scan early if a sufficiently strong system is detected.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type               msgr_hdr;

  /*! Transaction ID; matches the one in the request */
  uint8                              trans_id;

  /*! Band Scan results */
  lte_cphy_system_scan_cnf_params_s  sys_scan_results;
  
  /*! Status of operation */
  IxErrnoType                        status;
} lte_cphy_system_scan_cnf_s;

/*! @brief
  This message is used to provide L1 with the minimum system information
  needed to camp on the cell, and also to request L1 to calculate and return
  the S-criteria value.

  @todo resolve the details of this structure
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! E-UTRA Band */
  sys_sband_lte_e_type band;
  /*! EARFCN*/
  lte_earfcn_t earfcn;
  /*! Physical cell ID */
  lte_phy_cell_id_t phy_cell_id;
  /*! S criteria parameters */
  lte_cphy_cell_select_cfg_s cell_select_cfg;
  /*! CSG cell */
  boolean csg_cell;
} lte_cphy_cell_select_req_s;


/*! @brief
  This message confirms the camp request; it indicates whether or not L1
  is able to camp on the cell, and provides the S criteria and a failure
  reason if camping fails.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Command success/failure */
  errno_enum_type      status;
  /*! TBD */
  lte_l1_TBD_t failure_reason;
  /*! E-UTRA band */
  sys_sband_lte_e_type band;
  /*! EARFCN */
  lte_earfcn_t earfcn;
  /*! Physical cell ID */
  lte_phy_cell_id_t phy_cell_id;

  /*! the measurement of RSRP */
  int16 rsrp;
  
  /*! TBD */
  int8  s_rxlev;

  /*! Boolean to indicate if Cell selection info for Release 9 is present;
    For Rel 8 this boolean will always be set to FALSE. For Rel 9 the field will 
    always be set to TRUE and either signaled values or default values will be passed */
  boolean cell_select_info_rel9_present;

  /*! S_Qual info Release 9 */
  int32 Squal;

} lte_cphy_cell_select_cnf_s;

typedef struct
{
  /*! RAT type of barred cell */
  lte_rat_e    rat_type;
  /*! bar info */
  lte_cphy_rat_specific_cell_info_u rat_info;
} lte_cphy_cell_specific_info;

/*! @brief
    Cell bar info

  @todo remove this type
*/
typedef struct
{
  /*! Barred cell timeout value (in seconds) */
  uint32 t_barred;
  /*! Frequency also barred or not */
  boolean freq_is_barred;
  uint8 num_cells;
  lte_cphy_cell_bar_cause_e cause;
  lte_cphy_cell_specific_info cell_info_list[LTE_CPHY_MAX_OVERLAP_BANDS];
} lte_cphy_cell_bar_info_s;


/*! @brief
  RRC sends this request to indicate a barred cell to L1; no confirm is expected.

*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  lte_cphy_cell_bar_info_s  bar_info;

  boolean clear_cdb;

} lte_cphy_cell_bar_req_s;

/*! @brief
  RRC sends this request to unbar cells at L1; no confirm is expected.

*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  lte_cphy_cell_unbar_cause_e unbar_cause;

} lte_cphy_cell_unbar_req_s;

/*! @brief
    Reselection cancel info
 */
typedef struct
{
  /*! resel target cell info */
  lte_cphy_resel_target_cell_info_s  target_cell_info;

  /*! resel cancel  cause */
  lte_cphy_resel_cancel_cause_e      cancel_cause;

   /*! Barred cell timeout value (in seconds) */
  uint32 t_barred;

} lte_cphy_resel_cancel_info_s;

/*! @brief
  This msg is sent by RRC to ML1 in IDLE state when currently
  ongoing reselection is aborted.
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! current serving cell info */
  lte_cphy_cell_info_s               serv_cell_info;

  /*! resel cancel info */
  lte_cphy_resel_cancel_info_s  cancel_info;
} lte_cphy_cell_resel_cancel_req_s;

/*! @brief
  This msg is sent by RRC to CPHY when ConnReject is received from the eNB
  after a successful RACH; so that CPHY can clear the connection state
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
} lte_cphy_cancel_conn_req_s;


/*! @brief
  This msg is sent by RRC to ML1 in CONNECTED state when a RLC UM Bearer 
  is activated so that ML1 can apply DRX related optimizations. 
  This will not be sent during connection release and cphy abort 
  and ML1 will implicitly disable this field in those cases. 
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! UM mode configured */
  boolean              um_mode_enabled;
} lte_cphy_drx_opt_req_s;

/*! @brief
  This message is sent to ML1 from MAC when MAC wants to request ML1 to start
  a RACH Preamble.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! SCellIndex(0 .. 7 ). Value 0 is for Pcell.
      RACH on SCCs can only be triggered via PDCCH order */
  uint8 scell_id;

  /*! Randomly selected preamble sequence index by MAC Range: 0-63 */
  uint8 preamble_index;
  /*! Preamble index mask, range from 0 to 15. Invalid value is 0xFF */
  uint8 preamble_index_mask;
  /*! Preamble Pwr used for transmission of PRACH sequence Range: TBD */
  int16 preamble_power;
  /*! Initial Preamble Power Level */
  int16 initial_preamble_power;
} lte_cphy_start_rach_req_s;


/*! @brief
  This message is sent to MAC from ML1 to confirm that ML1 receives the
  LTE_ML1_START_RACH_REQ and lets MAC knows if ML1 can perform the request
  or not.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Indicates success/failure for START_RACH_REQ received from MAC. */
  boolean result;

  /*! SCellIndex(0 .. 7 ). Value 0 is for Pcell.
      RACH on SCCs can only be triggered via PDCCH order */
  uint8 scell_id;
} lte_cphy_start_rach_cnf_s;


/*! @brief
  This message is sent to ML1 from MAC when MAC is about to start a RACH
  procedure and wants to request ML1 for the radio condition measurement.
  The radio condition helps MAC to select the PREAMBLE index group.
*/
typedef struct
{
  /*! @todo Fill in these pameters */
  msgr_hdr_struct_type msgr_hdr;

} lte_cphy_rach_rc_req_s;


/*! @brief
  This message is sent to MAC from ML1 to report the radio condition
  measurement.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! TBD */
  int16 rc_measurement;
} lte_cphy_rach_rc_cnf_s;


/*! @brief
  This message is send to ML1 from MAC when MAC gets a match in message 2.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! SCellIndex(0 .. 7 ). Value 0 is for Pcell.
      RACH on SCCs can only be triggered via PDCCH order */
  uint8 scell_id;

  /*! Indicate if there is a new TA value */
  boolean ta_included;
  /*! Indicate if there is a T-RNTI */
  boolean trnti_included;
  /*! Timing advance to adjust UL timing Range: TDB */
  uint16 timing_advance;
  /*! Temporary RNTI assigned as part of the Message 2 for decoding Message 3 */
  lte_rnti_val_t temp_rnti;
  /*! The 20 bit grant for msg 3 (shouldn't this be a range?) */
  uint32 grant;
  /* The system time at the time of receiving the RAR */
  lte_l1_cell_systime_s  receive_time;
} lte_cphy_rar_params_req_s;


/*! @brief
  This message is sent to MAC from ML1 when ML1 receives a PDCCH order for
  DL data arrival.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! SCellIndex(0 .. 7 ). Value 0 is for Pcell */
  uint8 scell_id;

  /*! Dedicated PREAMBLE index to use Valid Range:0-0x3F Invalid Value = 0xFF */
  uint8 prach_index;

  /*! Preamble index mask, range from 0 to 15. Invalid value is 0xFF

   */
  uint8 prach_index_mask;

} lte_cphy_pdcch_order_ind_s;

/*! @brief
  This message is sent to MAC from ML1 when ML1 transmits MSG3 of RACH
  procedure .
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether or not the MSG3 transmission was successful */
  errno_enum_type      status;
} lte_cphy_msg3_transmission_ind_s;


/*! @brief
  This message is sent to MAC from ML1 when the RA timer is expired.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
} lte_cphy_ra_timer_expired_ind_s;

/*! @brief
  Sent when the downlink becomes weak when a cell is acquired.

  This is used to notify upper-layers that the cell quality has become poor.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Frequency of the cell; used with phy cell ID to identify
      the cell */
  lte_earfcn_t               freq;

  /*! Physical cell id of the cell; used with freq to identify
      the cell */
  lte_phy_cell_id_t          phy_cell_id;
} lte_cphy_dl_weak_ind_s;


/*! @brief
  Sent when the RL Failure is detected.

  This is used to notify upper-layers to start Re-etablishment procedure.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msg_hdr;

  /*! rlf reason */
  lte_l1_rlf_reason_e rlf_reason;

} lte_cphy_rl_failure_ind_s;

/*! @brief
  This msg is sent by ML1 to RRC when a better neighbor cell is
  found in IDLE state or in the case of DO reverse time transfer.
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! current serving cell info */
  lte_cphy_cell_info_s   serv_cell_info;

  /*! resel target cell info */
  lte_cphy_resel_target_cell_info_s  target_cell_info;

  /*! resel to CSG cell */
  boolean csg_resel;

  /*! The time at which this indication is sent */
  lte_ref_time_s  sfn;

} lte_cphy_cell_resel_ind_s;

/*! @brief
    This msg is sent by ML1 to RRC when the LTE CSG cell is not good
    enough to reselect
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

} lte_cphy_irat_asf_needed_ind_s;

/*! @brief
    This msg is sent by RRC to ML1 to notify the PCI range to be blocked,
    if CSG feature is not supported
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Number of CSG PCI ranges */
  uint8 num_black_cell_freq;

  /*! CSG PCI range */
  lte_cphy_meas_eutra_freq_pci_range_s black_cell_freq[LTE_CPHY_MEAS_NUM_LTE_CSG_FREQS];

} lte_cphy_blacklisted_csg_pci_range_ind_s;

/*! @brief
    This msg is sent by ML1 to RRC when it detects OOS condition in
    IDLE state
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! current serving cell info */
  lte_cphy_cell_info_s  serv_cell_info;

  /*! Target cell */
  lte_cphy_conn_release_acq_cell_info_s target_cell_info;

  /*! OOS reason */
  lte_l1_oos_reason_e oos_reason;

} lte_cphy_oos_ind_s;

/*! @brief
  This message is sent to ML1 from MAC when MAC decides to abort the RACH
  procedure and requests ML1 to do so.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
} lte_cphy_rach_abort_req_s;


/*! @brief
  This message is sent to MAC from ML1 to confirm with MAC that the request
  to abort RACH procedure is done.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
} lte_cphy_rach_abort_cnf_s;


/*! @brief
  This message is sent to ML1 from MAC when MAC wants to update ML1 a new
  RACH configuration.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! TRUE to indicate there is a new RACH config.
  FALSE to indicate no RACH config also
  the existing RACH config should be trashed*/
  boolean      rach_cfg_included;
  /*! ML1 RACH configuration pointer */
  lte_cphy_prach_setup_param_s  rach_cfg;
} lte_cphy_rach_cfg_req_s;


/*! @brief
  This message is sent to MAC from ML1 to confirm with MAC that ML1 has
  received the new RACH configuration from MAC.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  errno_enum_type      status; /*!< status of configuration */
} lte_cphy_rach_cfg_cnf_s;


/*! @brief Message sent from ML1 to RRC to inform RRC that a phychan schedule
    request has been sent to LL1 for RACH MSG1
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
}lte_cphy_rach_msg1_sched_ind_s;


/*! @brief Used to tell L1 that the uplink has gone out of sync. This
  should be sent from MAC */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type     hdr;

  /*! STAG-Id-r11    Range (1..maxSTAG-r11). Set to 0 for Pcell TAG */
  uint8  stag_id;
} lte_cphy_out_of_sync_req_s;


typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /*! STAG-Id-r11    Range (1..maxSTAG-r11). Set to 0 for Pcell TAG */
  uint8                      stag_id;

  /*! The timing Advance */
  uint16                     timing_advance;

  /*! The timing on which the TA command was received by MAC*/
  lte_l1_cell_systime_s      ta_rcvd_timing;

  /*! Flag to indicate the origin of the TA value FALSE: From TA_CMD, 
      TRUE: From MSG2 during RACH */
  boolean                    is_ta_frm_msg2;
} lte_cphy_apply_ta_params_req_s;


/*! @brief This message is sent from MAC to ML1 to update the TA value if BCR condition is met
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

  /* TA value to be used */
  int16 ta_adjust_val;

} lte_cphy_ra_one_off_ind_s;

/*! @brief MAC Scell Activation/De-activation config sent by MAC to L1 */
typedef struct
{
  /* Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /* Octet of bits indicating active/inactive status for 8 SCells  */
  uint8                      scell_status_mask;

  /*! System Frame number */
  lte_l1_sfn_t               sys_fn;

  /*! Subframe number */
  lte_l1_subfn_t             sub_fn;
} lte_cphy_scell_status_ind_s;

/*
* @brief Parameter struct for LTE_CPHY_HARQ_ACK_TX_STS_REQ. If there 
is a request from RRC to check if RLC ACK for Conn release has reached enB 
then RLC will send this request to ML1 passing HARQ_ID on 
which last SRB1 status PDU was built.indication will ensure DL to rebuilt the control
* PDU.
*/
typedef struct
{
  msgr_hdr_struct_type    msgr_hdr; /*!< Message header */
  uint8                   harq_id; /*!< Logical channel ID */
}lte_cphy_harq_ack_tx_sts_req_s;

/*! @brief TX status confirmation which RRC listens */
typedef struct
{
  msgr_hdr_struct_type    msgr_hdr; /*!< Message header */
  errno_enum_type         status; /*!< Logical channel ID */
}lte_cphy_harq_ack_tx_sts_cnf_s;


/*! @brief SIB Schedule Request from RRC to L1

    @detail RRC sends this message to request MIB/SIB decoding from L1. RRC
    includes a semi-static SIB schedule, and bitmasks that indicate which SIBs
    should be received. As SIBs are received, RRC can disable further
    reception by removing them from the bitmasks and sending another request.
    One bitmask controls which SIBs are received in the current modification
    period; this bitmask is always applied immediately by L1 to determine how
    to schedule SI message reception. Another bitmask controls which SIBs are
    received in the next modification period. This can be used to trigger a
    synchronized transition to a new schedule at a modification period
    boundary. RRC specifies the modification period boundary SFN in this case.
    L1 also sends a Boundary Indication to RRC if RRC includes the boundary
    SFN; this facilitates invalidation of parameters when SIBs change. Note
    that MIB decoding is implicit; L1 assumes that whenever SIB1 reception is
    active, it should also receive MIB. If RRC removes SIB1 from the bitmask,
    L1 assumes that it can also stop decoding MIB. If MIB is successfully
    decoded, L1 will also send a MIB Indication.
     */
typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /** Frequency of the cell; used with phy cell ID to identify
   *  the cell */
  lte_earfcn_t               freq;

  /** Physical cell id of the cell; used with freq to identify
   *  the cell */
  lte_phy_cell_id_t          phy_cell_id;
  
  /** SIB bitmask used in current modification period. It
   *  defines the SIBs that L1 must receive, and is applied
   *  immediately. While SIB collection is active, it always
   *  supersedes the mask sent in any previous requests. */
  lte_cphy_sib_mask_t        curr_mod_prd_sib_mask;

  /** Modification period boundary SFN. If set to a valid value,
   *  L1 will send a Boundary Indication at that SFN and activate
   *  the next_mod_prd_sib_mask. At the boundary SFN, the
   *  next_mod_prd_sib_mask will supersede the
   *  curr_mod_prd_sib_mask sent in this request. If the
   *  modification period boundary is unknown, or no Boundary
   *  Indication is needed, this field must be set to
   *  LTE_CPHY_NULL_SFN. */
  lte_l1_sfn_t               mod_period_boundary_sfn;


  /** Last SFN read by RRC in SIBs. Helpful in disambiguating
  *  modification boundary SFN if SFN wraps around when we
  *  receive this request.
  */
  lte_l1_sfn_t               last_sfn_in_sib_rsp;

  /** SIB bitmask to be used in the next modification period,
   *  i.e. after the specified boundary SFN. Before that SFN,
   *  the curr_mod_prd_sib_mask will be active. At the boundary
   *  SFN, L1 will immediately switch to the
   *  next_mod_prd_sib_mask. If the boundary SFN is set to
   *  LTE_CPHY_NULL_SFN, the next_mod_prd_sib_mask is ignored. */
  lte_cphy_sib_mask_t        next_mod_prd_sib_mask;

 /** SI message window length in ms as specified in SIB1 */
 lte_cphy_si_window_length_e si_window_length;

 /** Number of SI messages in the schedule as specified in
  *  SIB1; can be 0 if schedule is unknown, e.g. before SIB1
  *  is received. */
 uint8 num_si_msgs;

 /** SI scheduling information from SIB1. This is always the
  *  absolute schedule defined by the network; the sib masks
  *  are used to determine which SI message needs to be received.
  *  MIB and SIB1 have fixed schedules and should not be included
  *  in this structure. Note that if a modification period
  *  boundary is specified, this schedule applies to both the
  *  current and next modification period. Also note this
  *  schedule can be empty, e.g. before SIB1 has been
  *  received. */
 lte_cphy_si_msg_info_s      si_msg_info[LTE_CPHY_MAX_SI_MSGS];

}lte_cphy_sib_sched_req_s;

/*! @brief MIB indication from L1 to RRC.

    @detail MIB indications are sent upon successful PBCH decode whenever RRC
    requests the MIB in the SIB Schedule Request (Note that MIB is implicitly
    requested whenever SIB1 is requested). While MIB decoding is active,
    this indication is sent upon the first successful decode, or any time the
    MIB changes (SFN change is not considered a MIB change).

*/

/*! @brief This message is sent by MAC whenever it receives Timing Advance command
           over the downlink
*/
typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /** ASN.1-decoded MIB */
  lte_l1_mib_info_param_s    mib;

  /** Frequency of the cell */
  lte_earfcn_t               freq;

  /** Physical cell id of the cell */
  lte_phy_cell_id_t          phy_cell_id;

  /** SFN on which MIB was received and decoded on PBCH */
  lte_l1_sfn_t               sfn;

}lte_cphy_mib_ind_s;

/*! @brief Modification period boundary indication from L1 to RRC

    @detail Boundary Indications are sent when RRC include the modification
    period boundary SFN in the SIB Schedule Request. When the boundary is
    reached, L1 sends this indication. This is intended to facilitate the
    invalidation of SI parameters upon SIB change.
*/
typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /** Frequency of the cell */
  lte_earfcn_t               freq;

  /** Physical cell id of the cell */
  lte_phy_cell_id_t          phy_cell_id;

  /** SFN on which this boundary occurred */
  lte_l1_sfn_t               sfn;

}lte_cphy_mod_prd_boundary_ind_s;

/******************************************************************************
                                TDD cfg structs
*******************************************************************************/
/*! @brief TDD cfg req struct
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type               msgr_hdr;

  /*! Frequency of the cell; used with phy cell ID to identify
      the cell
  */
  lte_earfcn_t                       freq;

  /*! Physical cell id of the cell; used with freq to identify
      the cell
  */
  lte_phy_cell_id_t                  phy_cell_id;

  /*! TDD cfg */
  lte_cphy_tdd_cfg_param_s           cfg;

}lte_cphy_tdd_cfg_req_s;

/*! @brief TDD cfg cnf struct
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /*! Frequency of the cell; used with phy cell ID to identify
      the cell
  */
  lte_earfcn_t                       freq;

  /*! Physical cell id of the cell; used with freq to identify
      the cell
  */
  lte_phy_cell_id_t                  phy_cell_id;

  /*! TDD cfg cnf L1 status */
  errno_enum_type            status;

}lte_cphy_tdd_cfg_cnf_s;

/*! 
  @brief 
  This message is used by RRC/ML1 to request/respond for MBMS neigh freqs in the order of its cell power
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Num of eutra freqs */
  uint8 num_freq;

  /*! lte frequency list */
  lte_earfcn_t freq[LTE_CPHY_MEAS_MAX_NUM_FREQ];

} lte_cphy_mbms_neigh_freq_s;

/*! @brief
  RRC will pass a maximum of 8 earfcns to ML1 to get them arranged in order of best cell power
*/
typedef lte_cphy_mbms_neigh_freq_s lte_cphy_best_mbms_neigh_freq_req_s;

/*! @brief
  ML1 will return the valid earfcn(s) in the order of cell power. num_freq could be zero in case
  ML1 couldn't find any earfcn with valid cell info
*/
typedef lte_cphy_mbms_neigh_freq_s lte_cphy_best_mbms_neigh_freq_cnf_s;

/******************************************************************************
                              Suspend/Resume structs
*******************************************************************************/

/*! @brief Suspend/resume cause enum
*/
typedef enum
{
  LTE_CPHY_IRAT_1X_RESEL,           /*!<  L->1X [CDMA1XRTT] resel */
  LTE_CPHY_IRAT_DO_RESEL,           /*!<  L->DO [CDMAHRPD] resel */
  LTE_CPHY_IRAT_WCDMA_RESEL,        /*!<  L->WCDMA [UTRAN] resel */
  LTE_CPHY_IRAT_TDSCDMA_RESEL,        /*!<  L->TDSCDMA [UTRAN] resel */
  LTE_CPHY_IRAT_GERAN_RESEL,        /*!<  L->GERAN [GSM] resel */
  LTE_CPHY_IRAT_1X_HANDOVER,        /*!<  L->1X [CDMA1XRTT] handover */
  LTE_CPHY_IRAT_DO_HANDOVER,        /*!<  L->DO [CDMAHRPD] handover */
  LTE_CPHY_IRAT_WCDMA_HANDOVER,     /*!<  L->WCDMA [UTRAN] handover */
  LTE_CPHY_IRAT_GERAN_HANDOVER,     /*!<  L->GERAN [GSM] handover */
  LTE_CPHY_IRAT_BPLMN,              /*!<  L->other RAT BPLMN */
  LTE_CPHY_IRAT_CDMA_REVERSE_TIME_TRANSFER, /*!<  L->CDMA reverse time transfer */
  LTE_CPHY_IRAT_GERAN_CCO_NACC,     /*!<  L->G RAT cell change order recvd in LTE conncted mode  */
  LTE_CPHY_IRAT_CGI,                /*!<  L->other RAT CGI */
  LTE_CPHY_IRAT_TDSCDMA_HANDOVER,   /*!<  L->WCDMA [UTRAN] handover */
}lte_cphy_suspend_resume_cause_e;

/*! @brief CPHY suspend req struct
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! Suspend cause */
  lte_cphy_suspend_resume_cause_e   cause;

  /*! Reserve TRM for exchange*/
  boolean                           trm_irat_exchange;

  /*! TBD: More params need to be added like RAT specific target cell info,
      PLMN related etc. if needed
  */
}lte_cphy_suspend_req_s;

/*! @brief Enumeration of TRM priority level request
    Note : Please add priorities in ascending order
*/
typedef enum
{
  LTE_TRM_PRIORITY_LOW, /*!< 0 */
  LTE_TRM_PRIORITY_OOS_RLF,  /*!< 1 */
  LTE_TRM_PRIORITY_BG_TRAFFIC, /*!< 2 */    
  LTE_TRM_PRIORITY_CHAN_MAIN,  /*!< 3 */
  LTE_TRM_PRIORITY_SIGNALING,  /*!< 4 */
  LTE_TRM_PRIORITY_HIGH        /*!< 5 */
} lte_trm_priority_e;

/*! @brief Tuneaway type
*/
typedef enum
{
  LTE_CPHY_TUNEAWAY_LTA,
  LTE_CPHY_TUNEAWAY_QTA,
  LTE_CPHY_TUNEAWAY_DTA,
  LTE_CPHY_TUNEAWAY_AGAP
} lte_cphy_tuneaway_t;

/*! @brief CPHY RF Available Ind Structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! The time at which this indication is sent */
  lte_ref_time_s                    ref_time; 

  /*! Number of pages missed when RF was unavailable 
  This includes pages missed across all mod periods when rf was tuned away*/
  uint16                             num_po_missed_during_tune_away;

  /*! Tune Away duration, in ms*/
  uint16                            ta_duration;
  
  /*! Tuneaway type */
  lte_cphy_tuneaway_t               tuneaway_type;

  /*! DL pathloss */
  int16                             dl_pathloss;

}lte_cphy_rf_available_ind_s;


/*! @brief CPHY RF Unavailable Ind Structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! The time at which this indication is sent */
  lte_ref_time_s                    ref_time; 

  /*! Tuneaway type */
  lte_cphy_tuneaway_t               tuneaway_type;

  /*! Number of CCs (component carriers) for which chains are available */
  uint8                             num_cc;

  /*! Chains available */
  struct
  {
    /*! Component carrier ID - PCC is 0 */
    uint8                           cc_id;
    /*! Mask of available chains - least significant bit is PRX/RX0,
     *  next bit is RX1.. */
    uint8                           chain_avail_mask;
  } cc_chains_avail[LTE_CPHY_MAX_TUNEAWAY_CC];

  /*! Number of pages monitored in curr_mod_period 
  This includes a cumulative sum of all monitored pages in a mod period 
  irrespective of number of tune aways that occured*/
  uint16                             num_po_mtrd_in_curr_mod_pd;
  
  /* Indicates tx tune info to MAC during DTA. if tx tune is not required
     then mac don't need to restart rach as the previous rach would have 
     succeed during DTA.*/
  boolean                            tx_tune_required;

}lte_cphy_rf_unavailable_ind_s;

/*! @brief CPHY TCELL RF Unavailable Ind Structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

}lte_cphy_tcell_rf_unavailable_ind_s;

/*! @brief CPHY SCELL tuneaway start/end indicator
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! TRUE if TAW has started, FALSE is TAW has ended */
  boolean                           ta_start;
} lte_cphy_scell_tuneaway_status_s;

/*! @brief Message to indicate TRM priority to RRC.
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

 /*Indicates TRM priority(HIGH/LOW) for RRC connection establishment*/
 lte_trm_priority_e trm_priority; 
 
} lte_cphy_trm_priority_req_s;


/*! @brief Indication RRC sends to ML1 to release TRM */
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type             msg_hdr;

}lte_cphy_release_trm_req_s;

/*! @brief Indication RRC sends to ML1 to release TRM */
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type             msg_hdr;

}lte_cphy_release_trm_cnf_s;

/*! Cell timing info */
typedef union
{
  /*! WCDMA cell timing info - used only if cause is WCDMA_HANDOVER (can be
    extended to WCDMA_RESEL too) */
  lte_cphy_wcdma_time_info_s      wcdma_time_info;
  /*! TDSCDMA cell timing info*/
  lte_cphy_tdscdma_time_info_s      tdscdma_time_info;

  lte_cphy_gsm_time_info_s      gsm_time_info;
}lte_cphy_cell_time_info_u;
/*! @brief CPHY suspend cnf struct
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /*! Suspend L1 status */
  errno_enum_type            status;

  /*! Suspend cause */
  lte_cphy_suspend_resume_cause_e suspend_cause;

  /*! CDMA time info: Valid only if suspended for the same cause */
  lte_stmr_cdma_rtc_pair     cdma_time_info;

  /*! Cell timing info */
  lte_cphy_cell_time_info_u cell_time_info;

  /*! The time at which this indication is sent */
  lte_ref_time_s                   ref_time;

  /*! TBD: More params need to be added like RAT specific timing info,
      PLMN related etc. if needed
  */
}lte_cphy_suspend_cnf_s;

/*! @brief CPHY resume req struct
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type               msgr_hdr;

  /*! Resume cause: should match with suspend cause */
  lte_cphy_suspend_resume_cause_e    cause;

  /*! TRM Timeout Value in ms
          For Initial CPHY_RESUME_REQ, timeout val = 500ms
          If Initial CPHY_RESUME fails, then next CPHY_RESUME_REQ's
            timeout val = 5s (5000 ms)*/
  uint16    trm_timeout_val;

  /*! TBD: More params need to be added like PLMN related etc. if needed
  */
}lte_cphy_resume_req_s;

/*! @brief CPHY resume cnf struct
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /*! Resume L1 status
     If CPHY_RESUME_REQ fails due to TRM being not available for the 
     duration of the timeout (500 ms), status should be set to E_TIMER_EXP. 
     The status E_TIMER_EXP should not be used for other resume failure cases*/
  errno_enum_type            status;

  /*! Resume cause: should match with suspend cause */
  lte_cphy_suspend_resume_cause_e    cause;

  /*! The time at which this indication is sent */
  lte_ref_time_s                     ref_time;

  /*! TBD: More params if needed
  */
}lte_cphy_resume_cnf_s;


/*! @brief
     Data structure for PLMN earfcn number and its bandwidth.
*/
typedef struct
{
  /*! channel nunmber is EARFCN number */
  lte_earfcn_t         chan;

  /*! the bandwidth info is needed for system scanning */
  lte_bandwidth_e      bw;
} lte_cphy_bplmn_chan_s;


/*! @brief
     Data structure for PLMN frequency number and its bandwidth
*/
typedef struct
{
  /*! in units of 100Khz */
  uint32               freq;

  /*! the band information is needed to convert freq int earfcn */
  sys_sband_lte_e_type band;

  /*! the bandwidth information might not be needed. this is a
      place holder. the number is in units of 100Khz
   */
  uint32               bw;
} lte_cphy_bplmn_freq_s;


/*! @brief
    Data Structure for LTE_CPHY_BPLMN_START_REQ

    @details
      RRC uses this to send a BPLMN request. depending on the scenario,
      it may pass in new request data, or indicate ML1 to use previous
      request data to continue the PLMN search.

*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! if new_req is TRUE, ML1 shall remove previous status, if any, and
      initialize the request with the data given by this request.
      if new_req is FALSE, do not update the status of the previous request.
      if new_req is FALSE, and no previous request had been received,
      ML1 shall treat it as an empty request
    */
  boolean                          new_req;

  /*! request type */
  lte_cphy_bplmn_type_e            req_type;

  /*! Priority of the PLMN search, if set to LTE_CPHY_BPLMN_PRIORITY_HIGH
    BPLMN search should be prioritised */
  lte_cphy_bplmn_priority_e        priority;

  /* the following fields will be meaningful only to LTE to LTE and IRAT
     to LTE cases
   */

  /*! if start_now is TRUE, ML1 starts the PLMN search immediately
      if start_now is FALSE, ML1 starts the PLMN search when it is appropriate
      based on the LTE activity on ML1 side.
      When RRC can not decide which one to use, it is always a safe way to use
      FALSE. in this case, if ML1 can start it immediately, it will.

      specifically, RRC sets this to be TRUE when it expects a foreground PLMN,
      while FALSE when it expects a background PLMN
   */
  boolean                         start_now;

  /*! number of frequency included in this request */
  uint8                           num_chans;

  /*! the list of channels to be searched for PLMN */
  lte_cphy_bplmn_chan_s          chans[LTE_CPHY_BPLMN_MAX_FREQS];

  /*! number of bands included in this request */
  uint8                           num_bands;

  /*! the bands to be searched for PLMN */
  sys_sband_lte_e_type            bands[LTE_CPHY_BPLMN_MAX_BANDS];

  /*! the bandwidth masks for each band in the above list */
  lte_cphy_bw_mask_t              bw_masks[LTE_CPHY_BPLMN_MAX_BANDS];

  /*! the number of frequency in the list that ML1 should not search on */
  uint8                           num_black_freqs;

  /*! the list of frequency that ML1 should not search on for PLMN, even
      if it appears in the band scan result list
   */
  lte_cphy_bplmn_freq_s           black_freqs[LTE_CPHY_BPLMN_MAX_FREQS];

  /*! time allowed only makes sense to IRAT to LTE case. in milli-seconds.
      if it is unlimited, use LTE_CPHY_BPLMN_UNLIMITED_TIME
   */
  uint32                    irat_lte_plmn_time_allowed;
    
  /* CSG search */
  boolean                         csg_search;

  /* manual PCI search 
  Cant be used along with CSG search */
  boolean                         pci_scan;
  /*!Max Num PCIs to report per scan freq
  PCIs reported are assumed to be ranked in the order reported
  Applicable only when pci_scan is set to TRUE */
  uint8                           max_num_pci;

} lte_cphy_msg_bplmn_start_req_s;

/*! @brief
    Data Structure for LTE_CPHY_BPLMN_STOP_REQ

    @details
      RRC uses this to instruct ML1 to stop any PLMN search activities.
      ML1 should stop all the current PLMN activities if any. Further more,
      ML1 should not start any new PLMN acticities until next
      LTE_CPHY_BPLMN_START_REQ.

      ML1 keeps the current request and the intermediate PLMN
      search status.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;
} lte_cphy_msg_bplmn_stop_req_s;

/*! @brief
    Data Structure for LTE_CPHY_BPLMN_SUSPEND_REQ

    @details
      RRC uses this to instruct ML1 to suspend any PLMN search activity.
      ML1 should suspend all the current PLMN activities if any. Further more,
      ML1 should not start any new PLMN acticity until next
      LTE_CPHY_BPLMN_START_REQ.

      ML1 keeps the current request and the intermediate PLMN
      search status.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;
} lte_cphy_msg_bplmn_suspend_req_s;

/*! @brief
    Data Structure for LTE_CPHY_BPLMN_CELL_REQ

    @details
       RRC uses this command to:
         a) indicate the cell is barred; or
         b) indicate the frequency is barred; or
         c) if the cell is not barred, ask ML1 to calculate rsrp and Srxlev
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! the frequency that the cell is on */
  lte_earfcn_t                     freq;

  /*! the Updated frequency caluculated from SIB-1 Band indicator */
  lte_earfcn_t                     updated_freq;

  /*! the cell ID */
  lte_phy_cell_id_t                cell_id;

   /*! this can be set to be one of the values
       in the lte_cphy_bplmn_cell_barred_type_e
   */
  lte_cphy_bplmn_cell_barred_type_e  barred_status;


  /*! this is the cell configuration to help calculating Srxlev
      TODO: does RRC have the information of this?
   */
  lte_cphy_cell_select_cfg_s      cell_select_cfg;

  /** SI message window length in ms as specified in SIB1 */
  lte_cphy_si_window_length_e     si_window_length;

  /** Number of SI messages in the schedule as specified in
   *  SIB1; can be 0 if schedule is unknown, e.g. before SIB1
   *  is received. */
  uint8                           num_si_msgs;

  /** SI scheduling information from SIB1. This is always the
   *  absolute schedule defined by the network; the sib masks
   *  are used to determine which SI message needs to be received.
   *  MIB and SIB1 have fixed schedules and should not be included
   *  in this structure. Note that if a modification period
   *  boundary is specified, this schedule applies to both the
   *  current and next modification period. Also note this
   *  schedule can be empty, e.g. before SIB1 has been
   *  received. */
  lte_cphy_si_msg_info_s          si_msg_info[LTE_CPHY_MAX_SI_MSGS];

  /*! TDD cfg */
  lte_cphy_tdd_cfg_param_s        tdd_cfg;

} lte_cphy_msg_bplmn_cell_req_s;


/*! @brief
    Data Structure for LTE_CPHY_BPLMN_CELL_CNF

    @details
      ML1 sends this CNF to RRC in response to LTE_CPHY_BPLMN_CELL_REQ.

      RRC expects this CNF message only when the cell and the frequency
      are not barred. please also refer to LTE_CPHY_BPLMN_CELL_REQ
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! the DL earfcn that the cell is on */
  lte_earfcn_t                     earfcn;

  /*! the frequency that the cell is on, in 100Hz */
  uint16                           freq;

  /*! the bandwidth info obtained from PLMN search */
  lte_bandwidth_e                  bw;

  /*! the cell ID */
  lte_phy_cell_id_t                cell_id;

  /*! the measurement of RSRP */
  int16                            rsrp;

  /*! the measurement of RSRQ */
  int16                            rsrq;

  /*! the measurement of RSSI */
  int16                            rssi;

  /*! TRUE Srxlev is calculated, FALSE if the value is not supposed to be used
      only when RRC expects a CNF message even when the cell is barred or the
      frequency is barred. in that case, it sets the srxlev_calculated to be FALSE,
      so the message is just serving as a CNF to the LTE_CPHY_BPLMN_CELL_REQ
   */
  boolean                         srxlev_calculated;

  /*! the Srxlev value. meaningful only when srxlev_calculated is TRUE */
  int16                           srxlev;

  /*! Flag to indicate if release 9 parameters are present or not */
  boolean rel9_param_present;

  /*! the squal parameter appears only for Release 9 */
  int16   squal;

  /*! the measurement of RSRP for RX0 */
  int16                            rsrp_rx0;
  /*! the measurement of RSRP for RX1 */
  int16                            rsrp_rx1;
  /*! the measurement of RSRQ for RX0 */
  int16                            rsrq_rx0;
  /*! the measurement of RSRQ for RX1 */
  int16                            rsrq_rx1;

} lte_cphy_msg_bplmn_cell_cnf_s;

/*! @brief
    Data Structure for LTE_CPHY_BPLMN_SIB_RECEIVED_IND

    @details
      RRC sends this command to let ML1 know that SIB9 was received
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;
} lte_cphy_msg_bplmn_sib_received_ind_s;

/*! @brief
    Data Structure for LTE_CPHY_BPLMN_SKIP_REQ

    @details
      RRC uses this command to let ML1 continue its own LTE activity without
      having to suspend for IRAT PLMN search.

      Usually when ML1 sends LTE_CPHY_BPLMN_TIME_AVAIL_IND to RRC, RRC either
      suspends ML1 so that IRAT can perform PLMN search, or sends
      LTE_CPHY_BPLMN_SKIP_REQ to ML1 in case RRC decides not to give the time
      to IRAT to perform the PLMN search.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;
  /* Flag to indicate that ML1 should stay dormant for the full duration of time given in the time available indication */
  boolean                          disable_asap_resched;
} lte_cphy_msg_bplmn_skip_req_s;

/*! @brief
    Data Structure for LTE_CPHY_BPLMN_TIME_AVAIL_IND

    @details
      ML1 sends this indication to RRC in response to LTE to IRAT PLMN Search.
      When ML1 sends this indication to RRC, it means ML1 is ready to be
      suspended for IRAT to perform PLMN search.

      after receiving this, RRC should either suspend ML1, or send
      LTE_CPHY_BPLMN_SKIP_REQ to ML1 in case it decides not to give
      the time to IRAT to perform the PLMN search.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! the time available for LTE to IRAT PLMN search. if "unlimited"
      time is possible (say in OOS scenario), use LTE_CPHY_BPLMN_UNLIMITED_TIME
      the unit is milli-seconds
   */
  uint32                          time_avail;
} lte_cphy_msg_bplmn_time_avail_ind_s;



/*! @brief
    Data Structure for LTE_CPHY_BPLMN_COMPLETE_IND

    @details
      ML1 sends this indication to RRC in the following cases:
        1). in LTE to LTE case, all the PLMN search is done
        2). in IRAT to LTE case, the PLMN search is partially done
            due to the given time expired.
        3). PLMN search failed
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! for LTE to LTE, LTE to IRAT, it has to be LTE_CPHY_BPLMN_ALL_DONE

      for IRAT to LTE, if the PLMN search is partially done during the given time,
      the status is LTE_CPHY_BPLMN_PARTIAL_DONE. if the PLMN request is
      completely done, the status is LTE_CPHY_BPLMN_ALL_DONE.

      if any errors occur during the PLMN search and PLMN search can not continue,
      the status is LTE_CPHY_BPLMN_FAIL
  */
  lte_cphy_bplmn_complete_type_e  status;

  /*! if serving cell available, ML1 also attaches serving cell measurement info
      into the BPLMN COMPLETE IND
   */

  /* a flag to indicate whether serving cell is available
     normally, if it is a BPLMN, this flag should be TRUE, and for foreground
     PLMN cases, this flag should be FALSE

     only when this flag is TRUE, the next field of srv_cell_meas is meaningful
   */
  boolean srv_cell_avail;

  /* we re-use the CELL CNF payload to carry the serving cell measurement info */
  lte_cphy_msg_bplmn_cell_cnf_s srv_cell_meas;
} lte_cphy_msg_bplmn_complete_ind_s;


/*! @brief
    Data Structure for LTE_CPHY_BPLMN_STOP_CNF

    @details
       ML1 uses this to indicate that PLMN search activities
       within ML1 is completely stopped.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! the status has to be TRUE which means successful
       if it is FALSE, for now, RRC should ASSERT
   */
  boolean                          status;

  /*! if serving cell available, ML1 also attaches serving cell measurement info
      into the BPLMN COMPLETE IND
   */

  /* a flag to indicate whether serving cell is available
     normally, if it is a BPLMN, this flag should be TRUE, and for foreground
     PLMN cases, this flag should be FALSE

     only when this flag is TRUE, the next field of srv_cell_meas is meaningful
   */
  boolean srv_cell_avail;

  /* we re-use the CELL CNF payload to carry the serving cell measurement info */
  lte_cphy_msg_bplmn_cell_cnf_s srv_cell_meas;
} lte_cphy_msg_bplmn_stop_cnf_s;

/*! @brief
    Data Structure for LTE_CPHY_BPLMN_SUSPEND_CNF

    @details
       ML1 uses this to indicate that PLMN search within ML1 is suspended.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;
} lte_cphy_msg_bplmn_suspend_cnf_s;

/******************************************************************************
                              GPS structs
*******************************************************************************/

/*! @brief CPHY GPS timetag request types
*/
typedef enum
{
  LTE_CPHY_GPS_TIMETAG_REQUEST,
  LTE_CPHY_GPS_TIMETAG_PRE_CELL_CHANGE,
  LTE_CPHY_GPS_TIMETAG_POST_CELL_CHANGE,
  LTE_CPHY_GPS_TIMETAG_MAX

} lte_cphy_gps_timetag_type_t;


/*! @brief CPHY GPS timetag structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! GPS timetag request type */
  lte_cphy_gps_timetag_type_t       timetag_type;

  /*! Whether a sleep timetag is ok */
  boolean                           allow_sleep_timetag;

} lte_cphy_gps_timetag_req_s;


/*! @brief CPHY GPS event callback structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! GPS event callback function pointer */
  void
  *event_cb; /* Fool autogen.pl by putting this on the next line */

} lte_cphy_gps_event_cb_req_s;

/*! @brief CPHY GPS MRL subscribe structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! Whether GPS wants to subscribe */
  boolean                           subscribe;

} lte_cphy_gps_subscribe_req_s;

/*! @brief CPHY GPS time transfer priority enum
*/
typedef enum
{
  LTE_CPHY_GPS_TIMEXFER_PRIO_LOW = 0,
  LTE_CPHY_GPS_TIMEXFER_PRIO_HIGH,
  LTE_CPHY_GPS_TIMEXFER_PRIO_MAX

} lte_cphy_gps_timexfer_prio_e;


/*! @brief CPHY GPS time transfer request structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! GPS time transfer sequence number */
  uint32                            seq_num;

  /*! GPS time transfer priority */
  lte_cphy_gps_timexfer_prio_e      priority;

  /*! Whether SIB8 is required */
  boolean                           sib8_required;

  /*! Number of samples requested */
  uint8                             number_of_tt_samples;

} lte_cphy_gps_timexfer_req_s;

/*! @brief CPHY GPS time transfer abort structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! GPS time transfer sequence number */
  uint32                            seq_num;

} lte_cphy_gps_timexfer_abort_req_s;

typedef enum {
  CGPS_LTE_RXTX_SUCCESS                         = 0,
  CGPS_LTE_RXTX_ERROR_LTE_NOT_IN_CONNECTED_MODE = 1,
  CGPS_LTE_RXTX_ERROR_RXTX_DISABLED             = 2,
  CGPS_LTE_RXTX_ERROR_UNKNOWN                   = 3,
  CGPS_LTE_RXTX_ERROR_MAX,   
} lte_cphy_gps_rxtx_prio_e;

/*! @brief CPHY GPS RXTX request structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

} lte_cphy_gps_rxtx_req_s;

/*! @brief CPHY GPS RXTX done cnf structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type          msgr_hdr;

  /*! Whether report was captured successfully */
  lte_cphy_gps_rxtx_prio_e      return_val;

} lte_cphy_gps_rxtx_cnf_s;

/******************************************************************************
                              PRS structs
*******************************************************************************/


/*! Type of PRS session */
typedef enum
{
  LTE_CPHY_PRS_NORMAL_SESSION = 0,  /*!< Normal PRS session */
  LTE_CPHY_PRS_CONTINUOUS_SESSION,  /*!< Continuous PRS session */
  LTE_CPHY_PRS_SCANNER_SESSION,     /*!< Scanner PRS session */

} lte_cphy_prs_session_type_e;

/*! Type of PRS search */
typedef enum
{
  LTE_CPHY_PRS_SRCHTYPE_WBPRS = 0,  /*!< Normal PRS session */
  LTE_CPHY_PRS_SRCHTYPE_WBCRS,      /*!< CRS using PRS uK   */
} lte_cphy_prs_srch_type_e;

/*! Type of PRS mode */
typedef enum
{
  LTE_CPHY_PRS_SRCHMODE_PRS_CER = 0,  /*!< PRS CER mode */
  LTE_CPHY_PRS_SRCHMODE_PRS_CIR,      /*!< PRS CIR mode */
} lte_cphy_prs_srch_mode_e;

/*! LTE PRS start request structure */
typedef struct lte_cphy_prs_start_req_s
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

  /*! PRS duration in milliseconds */
  uint32 prs_duration;

  /*! Type of PRS session */
  lte_cphy_prs_session_type_e session;

  /*! Obsolete: PRS Srch Type */
  //uint8 srchtype;         /*!< Same as lte_LL1_prs_srch_type_e in intf_prs.h */

  /*! Srch Type of PRS: WBPRS or WBCRS */
  lte_cphy_prs_srch_type_e prs_crs_occ_type;

  /*! Srch Mode of PRS: PRS or PRS CIR */
  lte_cphy_prs_srch_mode_e cer_cir_occ_type;

  /*! Number of hypothesis per batch*/
  uint8 num_hyps_per_batch;

  /*! Number of hypothesis per occasion*/
  uint8 num_hyps_per_occassion;

} lte_cphy_prs_start_req_s;

/*! Maximum number of PRS neighbors */
#define LTE_CPHY_PRS_MAX_NGHBR_CELLS 72

/*! Maximum number of PRS cell hypotheses */
#define LTE_CPHY_PRS_MAX_CELL_HYP 4

/*! PRS number of downlink subframes */
typedef enum
{
  LTE_CPHY_PRS_1_SF = 0,  /*!< 1 PRS subframe in PRS occasion */
  LTE_CPHY_PRS_2_SF,      /*!< 2 PRS subframe in PRS occasion */
  LTE_CPHY_PRS_4_SF,      /*!< 4 PRS subframe in PRS occasion */
  LTE_CPHY_PRS_6_SF,      /*!< 6 PRS subframe in PRS occasion */
  LTE_CPHY_PRS_MAX_SF,    /*!< Maximum PRS subframes */

} lte_cphy_prs_num_dl_subframes_e;

/*! PRS antenna port config */
typedef enum
{
  LTE_CPHY_PRS_ANT_PORT_CONFIG_1_2 = 0,  /*!< 1-or-2-ports */
  LTE_CPHY_PRS_ANT_PORT_CONFIG_4,        /*!< 4-ports */
  LTE_CPHY_PRS_ANT_PORT_CONFIG_MAX,      /*!< Maximum antenna port config */

} lte_cphy_prs_ant_port_config_e;

/*! PRS muting repeat enum */
typedef enum
{
  LTE_CPHY_PRS_MUTING_REPEAT_2 = 0,  /*!< Repeat every 2 occasions  */
  LTE_CPHY_PRS_MUTING_REPEAT_4,      /*!< Repeat every 4 occasions  */
  LTE_CPHY_PRS_MUTING_REPEAT_8,      /*!< Repeat every 8 occasions  */
  LTE_CPHY_PRS_MUTING_REPEAT_16,     /*!< Repeat every 16 occasions  */
  LTE_CPHY_PRS_MUTING_REPEAT_MAX,    /*!< Maximum repeat field value */

} lte_cphy_prs_muting_repeat_e;

/*! LTE PRS cell muting structure */
typedef struct 
{
  /* Whether the muting information is valid */
  boolean valid;

  /*! Number of occasions before repeating */
  lte_cphy_prs_muting_repeat_e repeat;
  
  /*! Muting pattern */
  uint16 pattern;

} lte_cphy_prs_cell_muting_s;

/*! LTE PRS cell hypothesis structure */
typedef struct 
{
  /*! Symbol offset index */
  int8 symbol_offset_index;

  /*! Prune window size */
  uint16 prune_win_size;

  /*! Prune window start index */
  uint16 prune_win_start_index;

} lte_cphy_prs_cell_hypothesis_s;

/*! LTE PRS cell structure */
typedef struct 
{
  /*! Physical cell id */
  lte_phy_cell_id_t phy_cell_id;

  /*! E-ARFCN */
  lte_earfcn_t earfcn;

  /*! Antenna port config */
  lte_cphy_prs_ant_port_config_e ant_port_config;

  /*! PRS bandwidth */
  lte_bandwidth_idx_e bandwidth;

  /*! Configuration index */
  uint16 config_index;

  /*! Slot Number Offset */
  uint8 slot_num_offset;

  /*! PRS Subframe Offset*/
  uint16 prs_subframe_offset;

  /*! Number of downlink subframes */
  lte_cphy_prs_num_dl_subframes_e num_dl_subframes;

  /*! Number of consecutive measurements required*/
  uint8 NumOfConsecutiveMeas;

  /*! Is strong enough to cause x-corr */
  boolean b_is_offending_cell;

  /*! PRS Gain Bias */
  int8 prs_gain_bias;

  /*! Muting pattern */
  lte_cphy_prs_cell_muting_s muting;

  /*! Number of hypotheses */
  uint8 num_hypotheses;

  /*! Hypotheses */
  lte_cphy_prs_cell_hypothesis_s hypothesis[ LTE_CPHY_PRS_MAX_CELL_HYP ];

  /*! Cell has exceeded PRS IC threshold*/
  boolean prs_ic_offending_cell;

} lte_cphy_prs_cell_info_s;

/*! LTE PRS update cells request structure */
typedef struct lte_cphy_prs_update_cells_req_s
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Reference cell */
  lte_cphy_prs_cell_info_s ref;

  /*! Whether this update is a partial or a full update.*/
  boolean full_update;

  /*! Number of neighbor cells */
  uint8 num_nghbrs;

  /*! Neighbor cells */
  lte_cphy_prs_cell_info_s nghbrs[ LTE_CPHY_PRS_MAX_NGHBR_CELLS ];

} lte_cphy_prs_update_cells_req_s;

/*! LTE PRS stop request structure */
typedef struct lte_cphy_prs_stop_req_s
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

} lte_cphy_prs_stop_req_s;

#define MAX_NUM_RSTD_IFREQ 3

typedef enum 
{
  LTE_CPHY_IFREQ_OTDOA_MEAS_ACTION_STOP = 0,
  LTE_CPHY_IFREQ_OTDOA_MEAS_ACTION_START,

} lte_cphy_ifreq_otdoa_meas_action_e;

typedef struct lte_cphy_rstd_info_s
{

  /*! E-ARFCN */
  lte_earfcn_t earfcn;

  /*! PRS offset needed to perform ifreq measurements(0...39) */
  uint8 prs_offset;

} lte_cphy_rstd_info_s;

typedef struct lte_cphy_ifreq_otdoa_ind_s
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Meas Action */
  lte_cphy_ifreq_otdoa_meas_action_e meas_action;

  /* Num freqs populated in the array below */
  uint8 num_freq;

  /*! Measurement information needed for each frequency */
  lte_cphy_rstd_info_s meas_freqs[ MAX_NUM_RSTD_IFREQ ];

}lte_cphy_ifreq_otdoa_ind_s;

/*! @brief Message sent from ML1 to IMS to inform IMS of current CDRX cycle information
 *  so VoLTE voice packets can be synchronized with the CDRX on-duration.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /* Start time of next CDRX cycle in qtimer units */
  uint64 cdrx_cycle_start_time;

  /* Offset from start of next start of CDRX cycle to account for
   * msgr + processing delay (in qtimer units) */
  uint32 delta_time;

  /* The CDRX cycle lenght in ms, 0 indicates CDRX not configured */
  uint16 cdrx_cycle_len_ms;

  /* System frame number of start of CDRX  cycle */
  uint16 cdrx_cycle_start_sfn;

  /* Subframe number of start of CDRX  cycle */
  uint8  cdrx_cycle_start_subfn;

}lte_cphy_cdrx_pkt_timing_ind_s;

/******************************************************************************
            UE INFO REPORT - SON RLF and MDT related structures
*******************************************************************************/

/*! Maximum   number   of   carrier   frequencies */
#define LTE_CPHY_MAX_FREQUENCIES 8 

/*! @brief Types of SON Reports
*/
typedef enum
{
  LTE_CPHY_UEINFO_RPT_NONE = 0,
  LTE_CPHY_UEINFO_RPT_RLF = 1,
  LTE_CPHY_UEINFO_RPT_RACH = 2,
  LTE_CPHY_UEINFO_RPT_LOGGED_MEAS = 3,
  LTE_CPHY_UEINFO_RPT_CON_FAILURE = 4,
  LTE_CPHY_UEINFO_RPT_MAX = 255,
} lte_cphy_ueinfo_rpt_type_e;

typedef enum 
{
  LTE_CPHY_UEINFO_RPT_MASK_NONE         = 0,
  LTE_CPHY_UEINFO_RPT_MASK_RLF          = SYS_BM(LTE_CPHY_UEINFO_RPT_RLF),  
  LTE_CPHY_UEINFO_RPT_MASK_RACH                 = SYS_BM(LTE_CPHY_UEINFO_RPT_RACH),  
  LTE_CPHY_UEINFO_RPT_MASK_LOGGED_MEAS  = SYS_BM(LTE_CPHY_UEINFO_RPT_LOGGED_MEAS),  
  LTE_CPHY_UEINFO_RPT_MASK_CON_FAILURE  = SYS_BM(LTE_CPHY_UEINFO_RPT_CON_FAILURE),  
  LTE_CPHY_UEINFO_RPT_MASK_MAX,
} lte_cphy_ueinfo_rpt_mask_e;

/*! @brief Types for MDT Report
*/

typedef enum 
{
  LTE_CPHY_UEINFO_MDT_SESSION_START         = 0,
  LTE_CPHY_UEINFO_MDT_SESSION_STOP
} lte_cphy_ueinfo_mdt_session_status_e;

typedef enum 
{
  LTE_CPHY_UEINFO_MDT_LOGGING_INTERVAL_NONE         = 0,
  LTE_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS1280,
  LTE_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS2560,
  LTE_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS5120,
  LTE_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS10240,
  LTE_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS20480,
  LTE_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS30720,
  LTE_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS40960,
  LTE_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS61440
} lte_cphy_ueinfo_mdt_logging_interval_e;

/*! @brief Types of Conn failure Reports
*/
enum
{
  LTE_CPHY_UEINFO_CONN_FAILURE_RPT_NONE = 0,
  LTE_CPHY_UEINFO_CONN_FAILURE_RPT_MEAS = 0x01,
  LTE_CPHY_UEINFO_CONN_FAILURE_RPT_UL = 0x02,
  LTE_CPHY_UEINFO_CONN_FAILURE_RPT_MAX = 0xFF,
};

/*! @brief Structure that defines the measurement result list, used in
    measurement reports
*/

typedef struct 
{
  uint8 size;
  lte_cphy_conn_meas_report_list_eutra_s eutra[LTE_CPHY_MAX_CELL_REPORT];
} lte_cphy_ueinfo_meas_report_eutra_list_s;

typedef struct
{
  /*! Number of results in list. Range: 0..LTE_CPHY_MAX_CELL_REPORT */
  uint8 size;

  /*! WCDMA neighbor measurement results list */
  lte_cphy_conn_meas_report_list_wcdma_s wcdma[LTE_CPHY_MAX_CELL_REPORT];
} lte_cphy_ueinfo_meas_report_wcdma_list_s;


typedef struct
{
  /*! Number of results in list. Range: 0..LTE_CPHY_MAX_CELL_REPORT */
  uint8 size;

  /*! CDMA2000 neighbor measurement results list */
  lte_cphy_conn_meas_report_list_cdma2k_s cdma2k[LTE_CPHY_MAX_CELL_REPORT];
} lte_cphy_ueinfo_meas_report_cdma2k_list_s;

typedef struct
{
  /*! ARFCN-ValueEUTRA   ::=   INTEGER   (0 .  . 262143) */
  lte_earfcn_t carrierFreq;

  /*! Measurement results listfor eutra*/
  lte_cphy_ueinfo_meas_report_eutra_list_s eutra_list;
} lte_cphy_ueinfo_meas_neigh_results_eutran_s;

typedef struct
{
  /*! ARFCN-ValueUTRA   ::=   INTEGER   (0 .  . 16383) */
  uint16 carrierFreq;

  /*! Measurement results list */
  lte_cphy_ueinfo_meas_report_wcdma_list_s wcdma_list;
} lte_cphy_ueinfo_meas_neigh_results_wcdma_s;

typedef struct
{
  /*CDMA200 band class bc0 to bc17*/
  uint8 band_class;
    
  /*! ARFCN-ValueCDMA2000  ::=   INTEGER   (0 .  . 2047 ) */
  uint16 arfcn;

  /*! Measurement results list */
  lte_cphy_ueinfo_meas_report_cdma2k_list_s cdma2k_list;
} lte_cphy_ueinfo_meas_neigh_results_cdma2k_s;


typedef struct
{
  /*! Number of results in list. Range: 0..LTE_CPHY_MAX_FREQUENCIES */
  uint8                           eutra_size;
    
  /*! Measurement results list EUTRA */
  lte_cphy_ueinfo_meas_neigh_results_eutran_s eutra_neigh[LTE_CPHY_MAX_FREQUENCIES];

  /*! Number of results in list. Range: 0..LTE_CPHY_MAX_CELL_REPORT */
  uint8                           wcdma_size;

  /*! Measurement results list EUTRA */
  lte_cphy_ueinfo_meas_neigh_results_wcdma_s wcdma_neigh[LTE_CPHY_MAX_FREQUENCIES];

  /*! Number of results in list. Range: 0..LTE_CPHY_MAX_CELL_REPORT */
  uint8                           gsm_size;
    
  /*! GERAN neighbor measurement results list */
  lte_cphy_conn_meas_report_list_gsm_s   gsm_neigh[LTE_CPHY_MAX_CELL_REPORT];


  /*! Number of results in list. Range: 0..LTE_CPHY_MAX_CELL_REPORT */
  uint8                           cdma2k_size;

  /*! Measurement results list EUTRA */
  lte_cphy_ueinfo_meas_neigh_results_cdma2k_s cdma2k_neigh[LTE_CPHY_MAX_FREQUENCIES];

} lte_cphy_ueinfo_meas_neigh_results_s;

/*! @brief Measurement data structure for SON RLF feature
*/
typedef struct
{
  /*! RSRP of serving cell */
  lte_cphy_conn_meas_rsrp_t         serving_rsrp;

  /*! RRC to look at RSRQ of reported cell only if rsrq_info_present is TRUE*/
  boolean rsrq_info_present;

  /*! RSRQ of reported cell */
  lte_cphy_conn_meas_rsrq_t         serving_rsrq;
    
  /*! Neighbor measurement results list */
  lte_cphy_ueinfo_meas_neigh_results_s  neigh_results;
    
} lte_cphy_ueinfo_rlf_rpt_s;


/*! @brief Confirmation data structure for SON Measurement Reports
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Report Type*/
  lte_cphy_ueinfo_rpt_mask_e type; 

  /*! Whether or not the report was populated successfully */
  errno_enum_type status;
} lte_cphy_ueinfo_rpt_rsp_s;


/*! @brief Request data structure for SON Measurement Reports
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Report Type*/
  lte_cphy_ueinfo_rpt_mask_e type;  

    /*! Report */
  lte_cphy_ueinfo_rlf_rpt_s *rlf_rpt;   
} lte_cphy_ueinfo_rpt_req_s;


/*! @brief MDT report indication from ML1 to RRC
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! RSRP of serving cell */
  lte_cphy_conn_meas_rsrp_t         serving_rsrp;

  /*! RRC to look at RSRQ of reported cell only if rsrq_info_present is TRUE*/
  boolean rsrq_info_present;

  /*! RSRQ of reported cell */
  lte_cphy_conn_meas_rsrq_t         serving_rsrq;
    
  /*! Neighbor measurement results list */
  lte_cphy_ueinfo_meas_neigh_results_s  neigh_results;

} lte_cphy_ueinfo_mdt_report_ind_s;

/*! @brief Request data structure to start/stop MDT logging
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Session Type*/
  lte_cphy_ueinfo_mdt_session_status_e session_status;
  /*! Logging interval, LOGGING_INTERVAL_NONE to be used for STOP */
  lte_cphy_ueinfo_mdt_logging_interval_e mdt_logging_interval;

} lte_cphy_ueinfo_mdt_session_req_s;

/*! @brief Connection Failure report from ML1 to RRC
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  
  /*! Meas related params */
  lte_cphy_ueinfo_rlf_rpt_s rlf_rpt;

  /* To indicate whether or not the maximum power level was used 
      for the last transmitted preamble 
   */
  boolean maxTxPowerReached;

} lte_cphy_ueinfo_conn_failure_report_cnf_s;

/*! @brief Connection Failure report request from RRC to ML1
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  
} lte_cphy_ueinfo_conn_failure_report_req_s;


/******************************************************************************
                IRAT CGI related definitions
*******************************************************************************/

/*!< Minimum time required to suspend / resume LTE stack - 35 ms */
#define LTE_RRC_MIN_TIME_NEEDED_FOR_LTE_SUSP_RESUME   35

/*<! Minimum time required to read IRAT CGI in W,
     Min. Time for Acq - 230 ms
     Min. Time to setup/teardown BCH - 60 ms
     Min. Time to receive MIB/SIB1/SIB3 - 120 ms
     Min. Time to start/suspend WCDMA - 20 ms
      */
#define LTE_RRC_MIN_TIME_NEEDED_FOR_W_CGI_ACQ   430   

/*!< Total time needed for W CGI reading */
#define LTE_RRC_MIN_TIME_NEEDED_FOR_W_CGI_FROM_L   (LTE_RRC_MIN_TIME_NEEDED_FOR_W_CGI_ACQ + \
                                                    LTE_RRC_MIN_TIME_NEEDED_FOR_LTE_SUSP_RESUME)

/*!< Minimum time required to read IRAT CGI in G,
     Min. Time to acquire SI3 or SI13 - 420 ms
     Min. Time to activate/deactivate L1 - TBD
     Min. Time to tune to specific BCCH - TBD,
     for now 30 ms have been added for the above two steps */
#define LTE_RRC_MIN_TIME_NEEDED_FOR_G_CGI_ACQ   450   

/*!< Total time needed for G CGI reading */
#define LTE_RRC_MIN_TIME_NEEDED_FOR_G_CGI_FROM_L   (LTE_RRC_MIN_TIME_NEEDED_FOR_G_CGI_ACQ + \
                                                    LTE_RRC_MIN_TIME_NEEDED_FOR_LTE_SUSP_RESUME)

/*!  Minimum time required to read IRAT CGI in HRPD, 
     Min. Time to acquire HRPD and enter Idle - 70 ms
     Min. Time to acquire SectorParameters message - 35 ms
     Min. Time to activate/deactivate HRPD stack - 20 ms */
#define LTE_RRC_MIN_TIME_NEEDED_FOR_HRPD_CGI_ACQ   125   

/*!  Total time needed for HRPD CGI reading */
#define LTE_RRC_MIN_TIME_NEEDED_FOR_HRPD_CGI_FROM_L   (LTE_RRC_MIN_TIME_NEEDED_FOR_HRPD_CGI_ACQ + \
                                                       LTE_RRC_MIN_TIME_NEEDED_FOR_LTE_SUSP_RESUME)

/*!  Minimum time required to read IRAT CGI in 1X */
#define LTE_RRC_MIN_TIME_NEEDED_FOR_1X_CGI_ACQ   150

/*!  Total time needed for 1X CGI reading */
#define LTE_RRC_MIN_TIME_NEEDED_FOR_1X_CGI_FROM_L   (LTE_RRC_MIN_TIME_NEEDED_FOR_1X_CGI_ACQ + \
                                                     LTE_RRC_MIN_TIME_NEEDED_FOR_LTE_SUSP_RESUME)


/*! @brief This message is sent from ML1 to RRC to indicate that IRAT CGI reading 
    can be attempted on the other RAT. RRC may suspend ML1 soon after
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! Time available for IRAT CGI reading (in ms) */
  uint16 time_avail_for_irat_cgi;

} lte_cphy_irat_cgi_start_ind_s;

/*! @brief This message is sent from ML1 to RRC to indicate that IRAT CGI reading 
    needs to be aborted. This could be due to RACH/SR getting triggered or for
    any other reason LTE stack needs to be activated during DRX. RRC will resume 
    ML1 soon after.
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

} lte_cphy_irat_cgi_end_ind_s;

/*! @brief This message is sent from RRC to ML1 to indicate that IRAT CGI is being
    aborted due to T321 expiry or other causes. So LTE stack will not be suspended
    if ML1 receives this req from RRC. There is no CNF needed for this REQ.
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

} lte_cphy_abort_irat_cgi_req_s;

/*! @brief Random Access Attempt information */
typedef struct
{
  uint16                                 earfcn;
  uint16                                 phys_cell_id;
  uint16                                 ra_rnti;
  uint8                                  preamble_count;
  uint8                                  last_tx_power;
  uint32                                 ul_grant;
  uint16                                 ra_temp_c_rnti;
  uint16                                 timing_advance;
  uint8                                  ra_resp_successful;
} lte_cphy_random_access_attempt_s;

/*! @brief Payload of Indication sent by LTE to MMAL about MAC RAA */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                   hdr;

  /*! Metric type */
  lte_cphy_random_access_attempt_s       msg;
} lte_cphy_raa_ind_msg_s;

/*! @brief This message is sent by DO CP to request the current CDMA systime */
typedef struct 
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

} lte_cphy_cdma_systime_req_s;

/*! @brief This message is sent back to DO CP with current CDMA systime */
typedef struct 
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Whether the CDMA system time is valid */
  boolean valid;
  /*! CDMA system time */
  uint64 cdma_systime;

} lte_cphy_cdma_systime_cnf_s;

/*! @brief This message is sent by DO CP to request the current HRPD 
           measurement results */
typedef struct 
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

} lte_cphy_hrpd_meas_results_req_s;

/*! Maximum number of HRPD cells */
#define LTE_CPHY_HRPD_MEAS_RESULT_CELLS_MAX  16

/*! @brief This message is sent back to DO CP with current HRPD 
           measurement results */
typedef struct 
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether the HRPD results are valid */
  boolean valid;

  /*! Number of HRPD pilots */
  uint8 num_pilots;

  /*! HRPD pilot information */
  struct 
  {
    /*! HRPD channel */
    sys_channel_type chan;

    /*! HRPD pilot PN */
    sys_pilot_pn_type pilot_pn;

    /*! HRPD pilot phase */
    uint16 pilot_phase;

    /*! HRPD pilot strength in -db/2 units */
    uint8 pilot_strength;

  } pilot[ LTE_CPHY_HRPD_MEAS_RESULT_CELLS_MAX ];

} lte_cphy_hrpd_meas_results_cnf_s;


/*! 
  @brief 
  This message indicates to upper layers the current signal quality at L1 for each MBSFN Area.
*/
typedef struct
{
   
  /*! MBSFN area id: 0 - 255 */
  lte_mbsfn_area_id_t                       area_id;

/*! The average SNR of the serving cell over last 
      measurement period in decibels Q3[2 ^3] format
      result SNR in cnf = raw SNR value * 2^3
      Range -10..30
   */
  int16 snr;

/*! The excess SNR of the serving cell over last 
      measurement period in decibels Q3[2 ^3] format
      Range -10..30
   */
  int16 excess_snr;

  /*! The signal level of the serving cell over last 
      measurement period 
      Range 0 to 5
   */
  int8 signal_level;
 
} lte_cphy_mbsfn_area_signal_strength_s;

/*! 
  @brief 
  This message indicates to upper layers the current signal quality at L1 for each MBSFN Area.
*/

typedef struct
{
  msgr_hdr_struct_type                       msgr_hdr;
  
  /*! Number of MBSFN areas valid info schedule config */
  uint8                                    num_mbsfn_areas;

/*! Defines the trace id of the request corresponding to
     *    which this response is being sent.
     *    0 to 32768 - Represents a valid trace id
     *    -1 - Represents that this parameter is not used */
  int16                                    debug_trace_id;
  
 /*! RSSI ndication per MBSFN Area */
  lte_cphy_mbsfn_area_signal_strength_s        mbsfn_area_signal_strength[LTE_EMBMS_MAX_MBSFN_AREA];
} lte_cphy_mbsfn_signal_strength_cnf_s;

/*! @brief The parameters from SIB13 
  needed to calculate SNR for MBSFN area 
  */
typedef struct
{

  /*! MBSFN area id: 0 - 255 */
  lte_mbsfn_area_id_t                      area_id;


  /*! Non-MBSFN region length */
  lte_l1_non_mbsfn_region_length_e         non_mbsfn_symbols;

  /*! Signalling MCS */
  lte_l1_mbsfn_signalling_mcs_e            mcs;         

} lte_cphy_mbsfn_area_snr_param_s;

/*! 
  @brief 
  This message is used by upper layers to request the current signal quality at L1 for each MBSFN Area.
*/

typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

/*! Defines the trace id of the request corresponding to
     *    which this response is being sent.
     *    0 to 32768 - Represents a valid trace id
     *    -1 - Represents that this parameter is not used */

  int16            debug_trace_id;

  /*! If set to TRUE, SNR calculation params are passed by RRC */
  boolean         snr_param_present;

  /*! Number of MBSFN areas */
  uint8           num_mbsfn_areas;
  /*! Parameters needed by ML1 for excess SNR calculation. 
   RRC passes these param from SIB13 when SIB13 config 
   has not been applied in ML1 */
  lte_cphy_mbsfn_area_snr_param_s snr_param[LTE_CPHY_MAX_MCCH_CFG];
} lte_cphy_mbsfn_signal_strength_req_s;

/*! @brief Struct for req sent from RRC to ML1 requesting debug data to be printed
 * in case of a deadlock timer expiry for certain UMIDs... Putting this here to
 * avoid including ML1 headers in RRC
 */
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  msgr_umid_type                   recvd_umid;
} lte_cphy_debug_deadlock_tmr_exp_req_s;


/******************************************************************************
                Tx Power Report Msgs, enums, structs
*******************************************************************************/

/*! @brief
      Tx Power Report Bins

    @details
      Tx Power categorized into five bins
 */ 
typedef enum
{
  /*! x < 0dBm */
  LTE_TX_POWER_BIN_1,
  /*! 0dBm < x < 8dBm */
  LTE_TX_POWER_BIN_2,
  /*! 8dBm < x < 15dBm */   
  LTE_TX_POWER_BIN_3,
  /*! 15dBm < x < 20dBm */    
  LTE_TX_POWER_BIN_4,
  /*! x > 20dBm */  
  LTE_TX_POWER_BIN_5,
  /*! Max Tx Power */
  LTE_TX_POWER_BIN_MAX = LTE_TX_POWER_BIN_5
}lte_tx_power_bin_e_type;

/*! @brief
      Tx Power Report upper bounds

    @details
      Tx Power Report upper bounds for each bin
 */ 
typedef enum
{
  /*! 0dBm */
  LTE_TX_POWER_UB_1 = 0,
  /*! 0dBm */
  LTE_TX_POWER_UB_2 = 8,
  /*! 0dBm */
  LTE_TX_POWER_UB_3 = 15,
  /*! 0dBm */
  LTE_TX_POWER_UB_4 = 20,
  /*! Max Tx Power UB */
  LTE_TX_POWER_UB_MAX = LTE_TX_POWER_UB_4
}lte_tx_power_ub_e_type;

/*! @brief
      Data Structure for LTE_CPHY_TX_POWER_REPORT_IND

    @details
      REQ sent from DS to L1 to start or stop collecting Tx Power stats
 */     
typedef struct
{
  /*!< message header */
  msgr_hdr_struct_type              msgr_hdr;

  /*!< Start or Stop Tx Power Reporting */
  boolean                           enable;
}lte_cphy_tx_power_report_ind_s; 


/*! @brief
      Data Structure for LTE_CPHY_TX_POWER_REPORT_INFO_REQ

    @details
      REQ sent from DS to L1 to get Tx Power stats
 */     
typedef struct
{
  /*!< message header */
   msgr_hdr_struct_type              msgr_hdr;
}lte_cphy_tx_power_report_info_req_s; 


/*! @brief
      Data Structure for LTE_CPHY_TX_POWER_REPORT_INFO_CNF

    @details
      RSP sent by L1 to DS containing Tx Power stats and LTE instance
 */     
typedef struct
{
  /*!< message header */
   msgr_hdr_struct_type              msgr_hdr;

   /*! subscription ID */
  sys_modem_as_id_e_type             subs_id;

  /*!< Tx Power info counter array - keeps counts of each power bin */
   uint32                            tx_power_info[ LTE_TX_POWER_BIN_MAX + 1 ];
}lte_cphy_tx_power_report_info_cnf_s;


/*!
  @brief eMBMS data priority structure
 */
typedef struct lte_cphy_embms_data_priority
{
  /*! The sfn for which the specified priority should be used */
  lte_sfn_s                 start_time_sfn;
  /*! The duration in ms */
  uint16                    duration_ms;
  /*! The priority to reserve at */
  trm_reason_enum_t         priority;
  /*! The client that the reservation is placed on */
  trm_client_enum_t         client;
  /*! boolean to allow L1 to override the priority */
  boolean                   l1_override_priority;
} lte_cphy_embms_data_priority_s;

/*!
  @brief
  eMBMS set priority request API
  */
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type           msgr_hdr;

  /*! eMBMS num of dpd clients, max value is LTE_ML1_EMBMS_MAX_PRIORITY_CFG*/
  uint8                          num_client;

  /*! eMBMS priority data */
  lte_cphy_embms_data_priority_s prio[LTE_CPHY_EMBMS_MAX_PRIORITY_CFG]; 

} lte_cphy_embms_set_prio_req_s;

/*!
  @brief
  eMBMS MSP data request structure
  */
typedef struct lte_cphy_embms_msp_data_req
{
  /*! MSI of the interested service */
  lte_l1_mbsfn_msi_cfg_s         msi_data;
  /*! Logical Channel id */
  lte_lc_id_t                    lc_id;
  /*! Size of bitmask array */
  uint16                         bitmask_size;
  /*! Pointer to bitmask */
  uint16*                        bitmask_ptr;
} lte_cphy_embms_data_req_s;

/*!
  @brief
  eMBMS MSP data response structure
  */
typedef struct lte_cphy_embms_msp_data_rsp
{
  /*! Status - TRUE/FALSE */
  boolean                        status;
  /*! Area ID of requested MSI */
  lte_mbsfn_area_id_t            area_id;
  /*! PMCH ID of requested MSI */
  lte_pmch_id_t                  pmch_id;
  /*! LC ID of requested MSI */
  lte_lc_id_t                    lc_id;
  /*! Starting sysfn/subfn of interested LCID in current MSP */
  lte_sfn_s                      start_sfn;
  /*! Ending sysfn/subfn of interested LCID in current MSP */
  lte_sfn_s                      end_sfn;
  /*! Total number of subframes of the interested LCID in current MSP */
  uint16                         total_subframes;
} lte_cphy_embms_msp_data_rsp_s;

/*!
  @brief
  eMBMS MSP data request structure
  */
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type           msgr_hdr;

  lte_cphy_embms_data_req_s   request;

} lte_cphy_embms_get_msp_data_req_s;

/*!
  @brief
  eMBMS MSP data ind structure
  */
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type           msgr_hdr;

  lte_cphy_embms_msp_data_rsp_s response;

} lte_cphy_embms_get_msp_data_ind_s;

/*! @brief This message is sent from RRC to ML1 to indicate that LTE moves from DDS
to non-DDS or vice versa. Msgr hdr : LTE_CPHY_DDS_CHANGE_REQ
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;
  
  /*! True if LTE is DDS sub and FALSE if LTE is non-DDS sub*/
  boolean dds_sub;

} lte_cphy_dds_change_req_s;


/*! @brief
  Enum for the external control saps used to interface with L1
*/
enum
{
  /*=========================================================================*/
  /*                        Request/Confirm                                  */
  /*=========================================================================*/
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, ACQ, LTE_CPHY_ACQ_ID, lte_cphy_acq_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, ACQ, LTE_CPHY_ACQ_ID, lte_cphy_acq_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, START, LTE_CPHY_START_ID, lte_cphy_start_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, START, LTE_CPHY_START_ID, lte_cphy_start_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, STOP, LTE_CPHY_STOP_ID, lte_cphy_stop_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, STOP, LTE_CPHY_STOP_ID, lte_cphy_stop_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, ABORT, LTE_CPHY_ABORT_ID, lte_cphy_abort_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, ABORT, LTE_CPHY_ABORT_ID, lte_cphy_abort_cnf_s),


  MSGR_DEFINE_UMID(LTE, CPHY, REQ, COMMON_CFG,
                   LTE_CPHY_COMMON_CFG_ID, lte_cphy_common_cfg_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, COMMON_CFG,
                   LTE_CPHY_COMMON_CFG_ID, lte_cphy_common_cfg_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, DEDICATED_CFG,
                   LTE_CPHY_DEDICATED_CFG_ID, lte_cphy_dedicated_cfg_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, DEDICATED_CFG,
                   LTE_CPHY_DEDICATED_CFG_ID, lte_cphy_dedicated_cfg_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, CON_RELEASE,
                   LTE_CPHY_CON_RELEASE_ID, lte_cphy_con_release_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, CON_RELEASE,
                   LTE_CPHY_CON_RELEASE_ID, lte_cphy_con_release_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, HANDOVER, LTE_CPHY_HANDOVER_ID, lte_cphy_handover_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, HANDOVER, LTE_CPHY_HANDOVER_ID, lte_cphy_handover_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, BAND_SCAN, LTE_CPHY_BAND_SCAN_ID, lte_cphy_band_scan_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, BAND_SCAN, LTE_CPHY_BAND_SCAN_ID, lte_cphy_band_scan_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, SYSTEM_SCAN,
                   LTE_CPHY_SYSTEM_SCAN_ID, lte_cphy_system_scan_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, SYSTEM_SCAN,
                   LTE_CPHY_SYSTEM_SCAN_ID, lte_cphy_system_scan_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, CELL_SELECT,
                   LTE_CPHY_CELL_SELECT_ID, lte_cphy_cell_select_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, CELL_SELECT,
                   LTE_CPHY_CELL_SELECT_ID, lte_cphy_cell_select_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, START_RACH,
                   LTE_CPHY_START_RACH_ID, lte_cphy_start_rach_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, START_RACH,
                   LTE_CPHY_START_RACH_ID, lte_cphy_start_rach_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, RACH_RC, LTE_CPHY_RACH_RC_ID, lte_cphy_rach_rc_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, RACH_RC, LTE_CPHY_RACH_RC_ID, lte_cphy_rach_rc_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, RAR_PARAMS,
                   LTE_CPHY_RAR_PARAMS_ID, lte_cphy_rar_params_req_s),
  // No LTE_CPHY_RAR_PARAM_CNF

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, RACH_ABORT,
                   LTE_CPHY_RACH_ABORT_ID, lte_cphy_rach_abort_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, RACH_ABORT,
                   LTE_CPHY_RACH_ABORT_ID, lte_cphy_rach_abort_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, RACH_CFG, LTE_CPHY_RACH_CFG_ID, lte_cphy_rach_cfg_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, RACH_CFG, LTE_CPHY_RACH_CFG_ID, lte_cphy_rach_cfg_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, OUT_OF_SYNC,
                   LTE_CPHY_OUT_OF_SYNC_ID, lte_cphy_out_of_sync_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, APPLY_TA_PARAMS,
                   LTE_CPHY_APPLY_TA_PARAMS_ID, lte_cphy_apply_ta_params_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, SIB_SCHED,
                   LTE_CPHY_SIB_SCHED_ID, lte_cphy_sib_sched_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, CELL_BAR,
                   LTE_CPHY_CELL_BAR_ID, lte_cphy_cell_bar_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, CELL_UNBAR,
                   LTE_CPHY_CELL_UNBAR_ID, lte_cphy_cell_unbar_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, IDLE_DRX_CFG,
                   LTE_CPHY_IDLE_DRX_CFG_ID, lte_cphy_idle_drx_cfg_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, IDLE_MEAS_CFG,
                   LTE_CPHY_IDLE_MEAS_CFG_ID, lte_cphy_idle_meas_cfg_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, IDLE_MEAS_CFG,
                   LTE_CPHY_IDLE_MEAS_CFG_ID, lte_cphy_idle_meas_cfg_cnf_s),

 MSGR_DEFINE_UMID(LTE, CPHY, REQ, TRM_UNLOCK_ALLOWED,
                   LTE_CPHY_TRM_UNLOCK_ALLOWED_ID, lte_cphy_trm_unlock_allowed_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, TRM_UNLOCK_ALLOWED,
                   LTE_CPHY_TRM_UNLOCK_ALLOWED_ID, lte_cphy_trm_unlock_allowed_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, CONN_MEAS_CFG,
                   LTE_CPHY_CONN_MEAS_CFG_ID, lte_cphy_conn_meas_cfg_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, CONN_MEAS_CFG,
                   LTE_CPHY_CONN_MEAS_CFG_ID, lte_cphy_conn_meas_cfg_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, CELL_RESEL_CANCEL,
                   LTE_CPHY_CELL_RESEL_CANCEL_ID, lte_cphy_cell_resel_cancel_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, CANCEL_CONN,
                   LTE_CPHY_CANCEL_CONN_ID, lte_cphy_cancel_conn_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, SUSPEND,
                   LTE_CPHY_SUSPEND_ID, lte_cphy_suspend_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, SUSPEND,
                   LTE_CPHY_SUSPEND_ID, lte_cphy_suspend_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, RESUME,
                   LTE_CPHY_RESUME_ID, lte_cphy_resume_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, RESUME,
                   LTE_CPHY_RESUME_ID, lte_cphy_resume_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, IRAT_CDMA_SYSTEM_TIME_CHG,
                   LTE_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_ID, lte_cphy_irat_cdma_system_time_chg_req_s),

  /*! @brief LTE_CPHY_BPLMN_START_REQ

      @details
      RRC uses this to send a BPLMN request. depending on the scenario,
      it may pass in new request data, or indicate ML1 to use previous
      request data to continue the PLMN search.
  */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, BPLMN_START, LTE_CPHY_BPLMN_START_ID,
                                   lte_cphy_msg_bplmn_start_req_s),
  /*! @brief LTE_CPHY_BPLMN_STOP_REQ

     @details
      RRC uses this to instruct ML1 to stop any PLMN search activities.
      ML1 should stop all the current PLMN activities if any. Further more,
      ML1 should not start any new PLMN acticities until next
      LTE_CPHY_BPLMN_START_REQ.

      ML1 keeps the current request and the intermediate PLMN
      search status.
   */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, BPLMN_STOP, LTE_CPHY_BPLMN_STOP_ID,
                                   lte_cphy_msg_bplmn_stop_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, BPLMN_SUSPEND, LTE_CPHY_BPLMN_SUSPEND_ID,
                                   lte_cphy_msg_bplmn_suspend_req_s),
  /*! @brief LTE_CPHY_BPLMN_STOP_CNF

      @details
      ML1 sends this confirmation when the PLMN search activity is fully stopped
  */
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, BPLMN_STOP, LTE_CPHY_BPLMN_STOP_ID,
                                   lte_cphy_msg_bplmn_stop_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, CNF, BPLMN_SUSPEND, LTE_CPHY_BPLMN_SUSPEND_ID,
                                   lte_cphy_msg_bplmn_suspend_cnf_s),

  /*! @brief LTE_CPHY_BPLMN_CELL_REQ

      @details
       RRC uses this command to:
         a) indicate the cell is barred; or
         b) indicate the frequency is barred; or
         c) if the cell is not barred, ask ML1 to calculate rsrp and Srxlev
  */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, BPLMN_CELL, LTE_CPHY_BPLMN_CELL_ID,
                                   lte_cphy_msg_bplmn_cell_req_s),

  /*! @brief LTE_CPHY_BPLMN_CELL_CNF

      @details
      ML1 sends this CNF to RRC in response to LTE_CPHY_BPLMN_CELL_REQ.
  */
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, BPLMN_CELL, LTE_CPHY_BPLMN_CELL_ID,
                                   lte_cphy_msg_bplmn_cell_cnf_s),

  /*! @brief LTE_CPHY_BPLMN_SKIP_REQ

      @details
      RRC uses this command to let ML1 continue its own LTE activity without
      having to suspend for IRAT PLMN search.

      Usually when ML1 sends LTE_CPHY_BPLMN_TIME_AVAIL_IND to RRC, RRC either
      suspends ML1 so that IRAT can perform PLMN search, or sends
      LTE_CPHY_BPLMN_SKIP_REQ to ML1 in case RRC decides not to give the time
      to IRAT to perform the PLMN search.
  */

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, BPLMN_SKIP, LTE_CPHY_BPLMN_SKIP_ID,
                                   lte_cphy_msg_bplmn_skip_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, TDD_CFG,
                   LTE_CPHY_TDD_CFG_ID, lte_cphy_tdd_cfg_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, TDD_CFG,
                   LTE_CPHY_TDD_CFG_ID, lte_cphy_tdd_cfg_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, GPS_TIMETAG,
                   LTE_CPHY_GPS_TIMETAG_ID, lte_cphy_gps_timetag_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, NMR_INFO,
                   LTE_CPHY_NMR_INFO_ID, lte_cphy_nmr_info_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, NMR_INFO,
                   LTE_CPHY_NMR_INFO_ID, lte_cphy_nmr_info_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, SERV_CELL_MEAS,
                   LTE_CPHY_SERV_CELL_MEAS_ID, lte_cphy_serv_cell_meas_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, SERV_CELL_MEAS,
                   LTE_CPHY_SERV_CELL_MEAS_ID, lte_cphy_serv_cell_meas_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, GPS_MRL,
                   LTE_CPHY_GPS_MRL_ID, msgr_hdr_struct_type),
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, GPS_EVENT_CB,
                   LTE_CPHY_GPS_EVENT_CB_ID, lte_cphy_gps_event_cb_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, GPS_MRL_SUBSCRIBE,
                   LTE_CPHY_GPS_MRL_SUBSCRIBE_ID, lte_cphy_gps_subscribe_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, PRS_START,
                   LTE_CPHY_PRS_START_ID, lte_cphy_prs_start_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, PRS_UPDATE_CELLS,
                   LTE_CPHY_PRS_UPDATE_CELLS_ID, lte_cphy_prs_update_cells_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, PRS_STOP,
                   LTE_CPHY_PRS_STOP_ID, msgr_hdr_struct_type),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, ABORT_IRAT_CGI,
                   LTE_CPHY_ABORT_IRAT_CGI_ID, lte_cphy_abort_irat_cgi_req_s),

  /* MBSFN areas: MCH scheduling information per PMCH */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, MSI,
                   LTE_CPHY_MSI_ID, lte_cphy_msi_req_s),
  // No LTE_CPHY_MSI_CNF

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, CDMA_SYSTIME,
                   LTE_CPHY_CDMA_SYSTIME_ID, lte_cphy_cdma_systime_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, CDMA_SYSTIME,
                   LTE_CPHY_CDMA_SYSTIME_ID, lte_cphy_cdma_systime_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, HRPD_MEAS_RESULTS,
                   LTE_CPHY_HRPD_MEAS_RESULTS_ID, 
                   lte_cphy_hrpd_meas_results_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, HRPD_MEAS_RESULTS,
                   LTE_CPHY_HRPD_MEAS_RESULTS_ID, 
                   lte_cphy_hrpd_meas_results_cnf_s),
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, DRX_OPT,
                   LTE_CPHY_DRX_OPT_ID, 
                   lte_cphy_drx_opt_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, GPS_TIMEXFER,
                   LTE_CPHY_GPS_TIMEXFER_ID, lte_cphy_gps_timexfer_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, GPS_TIMEXFER_ABORT,
                   LTE_CPHY_GPS_TIMEXFER_ABORT_ID, lte_cphy_gps_timexfer_abort_req_s),

MSGR_DEFINE_UMID(LTE, CPHY, REQ, CDMA_CHANNEL_BAR,
                   LTE_CPHY_CDMA_CHANNEL_BAR_ID, lte_cphy_cdma_channel_bar_req_s),
  MSGR_DEFINE_UMID( LTE, CPHY, REQ, GPS_RXTX,
                    LTE_CPHY_GPS_RXTX_REQ_ID, lte_cphy_gps_rxtx_req_s ),

  MSGR_DEFINE_UMID( LTE, CPHY, CNF, GPS_RXTX,
                    LTE_CPHY_GPS_RXTX_CNF_ID, lte_cphy_gps_rxtx_cnf_s ),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, UEINFO_RPT,
                   LTE_CPHY_UEINFO_RPT_ID, lte_cphy_ueinfo_rpt_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, UEINFO_RPT,
                   LTE_CPHY_UEINFO_RPT_ID, lte_cphy_ueinfo_rpt_rsp_s),

  /*! @brief LTE_CPHY_MDT_SESSION_REQ

      @detail
      RRC sends this request to ML1 to start or stop an MDT session
  */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, UE_INFO_MDT_SESSION,
                   LTE_CPHY_UEINFO_MDT_SESSION_ID, lte_cphy_ueinfo_mdt_session_req_s),   
  
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, UTC_TIME_UPDATE, LTE_CPHY_UTC_TIME_UPDATE_REQ_ID, lte_cphy_utc_time_update_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, UTC_TIME_UPDATE, LTE_CPHY_UTC_TIME_UPDATE_CNF_ID, lte_cphy_utc_time_update_cnf_s),
  
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, BEST_MBMS_NEIGH_FREQ,
                   LTE_CPHY_BEST_MBMS_NEIGH_FREQ_ID, lte_cphy_best_mbms_neigh_freq_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, BEST_MBMS_NEIGH_FREQ,
                   LTE_CPHY_BEST_MBMS_NEIGH_FREQ_ID, lte_cphy_best_mbms_neigh_freq_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, TRM_PRIORITY,
                    LTE_CPHY_TRM_PRIORITY_ID ,  lte_cphy_trm_priority_req_s),  

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, RELEASE_TRM,
                   LTE_CPHY_RELEASE_TRM_ID, lte_cphy_release_trm_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, RELEASE_TRM,
                   LTE_CPHY_RELEASE_TRM_ID, lte_cphy_release_trm_cnf_s),

  /*! @brief Request sent from RRC to ML1 rquesting debug info to be printed out
   * on a deadlock timer expiry for certain UMIDs...
   * Putting this here to avoid including ML1 headers in RRC
  */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, DEBUG_DEADLOCK_TMR_EXP,
                  LTE_CPHY_DEBUG_DEADLOCK_TMR_EXP_ID, lte_cphy_debug_deadlock_tmr_exp_req_s),

  /*! @brief LTE_CPHY_UEINFO_CONN_FAILURE_RPT_REQ/CNF

      @detail
      RRC sends this request to ML1 at t-330 expiry for rach and meas parameters
  */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, UEINFO_CONN_FAILURE_RPT, 
                   LTE_CPHY_UEINFO_CONN_FAILURE_RPT_REQ_ID, lte_cphy_ueinfo_conn_failure_report_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, UEINFO_CONN_FAILURE_RPT, 
                   LTE_CPHY_UEINFO_CONN_FAILURE_RPT_REQ_ID, lte_cphy_ueinfo_conn_failure_report_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, NV_UPDATE, LTE_CPHY_NV_UPDATE_ID, lte_cphy_nv_update_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, HARQ_ACK_TX_STS,
                   LTE_CPHY_HARQ_ACK_TX_STS_ID, lte_cphy_harq_ack_tx_sts_req_s),
                   
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, HARQ_ACK_TX_STS,
                   LTE_CPHY_HARQ_ACK_TX_STS_ID, lte_cphy_harq_ack_tx_sts_cnf_s),
  
  /* message ID: LTE_CPHY_TX_POWER_REPORT_INFO_REQ */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, TX_POWER_REPORT_INFO,
                   LTE_CPHY_UL_TX_POWER_INFO_ID, lte_cphy_tx_power_report_info_req_s),

  /* message ID: LTE_CPHY_TX_POWER_REPORT_INFO_CNF */
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, TX_POWER_REPORT_INFO,
                   LTE_CPHY_UL_TX_POWER_INFO_ID, lte_cphy_tx_power_report_info_cnf_s),

  
  /*=========================================================================*/
  /*                          Indications                                    */
  /*=========================================================================*/
  MSGR_DEFINE_UMID(LTE, CPHY, IND, PDCCH_ORDER,
                   LTE_CPHY_IND_PDCCH_ORDER_ID, lte_cphy_pdcch_order_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, RA_TIMER_EXPIRED,
                   LTE_CPHY_IND_RA_TIMER_EXPIRED_ID, lte_cphy_ra_timer_expired_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, MIB,
                   LTE_CPHY_IND_MIB_ID, lte_cphy_mib_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, MOD_PRD_BND,
                   LTE_CPHY_IND_MOD_PRD_BND_ID, lte_cphy_mod_prd_boundary_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, DL_WEAK,
                   LTE_CPHY_IND_DL_WEAK_ID, lte_cphy_dl_weak_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, CELL_RESEL,
                   LTE_CPHY_IND_CELL_RESEL_ID, lte_cphy_cell_resel_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, IRAT_ASF_NEEDED,
                   LTE_CPHY_IND_IRAT_ASF_NEEDED_ID, lte_cphy_irat_asf_needed_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, BLACKLISTED_CSG_PCI_RANGE,
                   LTE_CPHY_IND_BLACKLISTED_CSG_PCI_RANGE_ID, lte_cphy_blacklisted_csg_pci_range_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, OOS,
                   LTE_CPHY_IND_OOS_ID, lte_cphy_oos_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, RL_FAILURE,
                   LTE_CPHY_IND_RL_FAILURE_ID, lte_cphy_rl_failure_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, MEAS_REPORT,
                   LTE_CPHY_IND_MEAS_REPORT_ID, lte_cphy_conn_meas_report_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, MSG3_TRANSMISSION,
                   LTE_CPHY_IND_MSG3_TRANSMISSION_ID, (lte_cphy_msg3_transmission_ind_s)),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, BPLMN_SIB_RECEIVED,
                   LTE_CPHY_IND_BPLMN_SIB_RECEIVED_ID, lte_cphy_msg_bplmn_sib_received_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, RF_UNAVAILABLE,
                   LTE_CPHY_IND_RF_UNAVAILABLE_ID, lte_cphy_rf_unavailable_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, RF_AVAILABLE,
                   LTE_CPHY_IND_RF_AVAILABLE_ID, lte_cphy_rf_available_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, TCELL_RF_UNAVAILABLE,
                   LTE_CPHY_IND_TCELL_RF_UNAVAILABLE_ID, lte_cphy_tcell_rf_unavailable_ind_s),


  /*! @brief LTE_CPHY_BPLMN_TIME_AVAIL_IND

      @detail
      ML1 sends this indication to RRC in response to LTE to IRAT PLMN Search.
      When ML1 sends this indication to RRC, it means ML1 is ready to be
      suspended for IRAT to perform PLMN search.

      after receiving this, RRC should either suspend ML1, or send
      LTE_CPHY_BPLMN_SKIP_REQ to ML1 in case it decides not to give
      the time to IRAT to perform the PLMN search.
  */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, BPLMN_TIME_AVAIL, LTE_CPHY_IND_BPLMN_TIME_AVAIL_ID,
                                   lte_cphy_msg_bplmn_time_avail_ind_s),

  /*! @brief LTE_CPHY_BPLMN_COMPLETE_IND

      @details
      ML1 sends this indication to RRC in the following cases:
        1). in LTE to LTE case, all the PLMN search is done
        2). in IRAT to LTE case, the PLMN search is partially done
            due to the given time expired.
        3). PLMN search failed
  */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, BPLMN_COMPLETE, LTE_CPHY_IND_BPLMN_COMPLETE_ID,
                                   lte_cphy_msg_bplmn_complete_ind_s),

  /*! @brief This message is sent from ML1 to RRC to indicate that IRAT CGI reading 
    can be attempted on the other RAT. RRC may suspend ML1 soon after 
  */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, IRAT_CGI_START, LTE_CPHY_IND_IRAT_CGI_START_ID,
                                   lte_cphy_irat_cgi_start_ind_s),

  /*! @brief This message is sent from ML1 to RRC to indicate that IRAT CGI reading 
    needs to be aborted on the other RAT. RRC will resume ML1 soon after 
  */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, IRAT_CGI_END, LTE_CPHY_IND_IRAT_CGI_END_ID,
                                   lte_cphy_irat_cgi_end_ind_s),

  /*! @brief This message is sent from ML1 to RRC to indicate M-RNTI MCCH change 
      notification with PDCCH bit mask
  */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, MCCH_CHANGE_NOTIFICATION,
                 LTE_CPHY_MCCH_CHANGE_NOTIFICATION_ID, lte_cphy_mcch_change_notification_ind_s),

  /*! @brief This message is sent from MAC to ML1 to indicate Scell activation/deactivation 
      MAC Control Element 
  */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, SCELL_STATUS, 
                   LTE_CPHY_IND_SCELL_STATUS_ID, lte_cphy_scell_status_ind_s),

  /*! @brief This message is sent from MAC to ML1 to indicate the TA Value when 
      RAID is off by one
  */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, RA_ONE_OFF, 
                   LTE_CPHY_IND_RA_ONE_OFF_ID, lte_cphy_ra_one_off_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, CNF, MBSFN_SIGNAL_STRENGTH,
                   LTE_CPHY_MBSFN_SIGNAL_STRENGTH_ID, lte_cphy_mbsfn_signal_strength_cnf_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, MBSFN_SIGNAL_STRENGTH,
                   LTE_CPHY_MBSFN_SIGNAL_STRENGTH_ID, lte_cphy_mbsfn_signal_strength_req_s),
                   
  /*! @brief MDT report Indication sent from ML1 to RRC.
      Sent at regular time intervals as configured by eNB
  */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, UE_INFO_MDT_REPORT, 
                   LTE_CPHY_IND_UEINFO_MDT_REPORT_ID, lte_cphy_ueinfo_mdt_report_ind_s),
  
  /*! @brief Indication sent from ML1 to RRC indicating we want to reconfigure for 
             for inter-freq OTDOA.
    */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, IFREQ_OTDOA, 
                   LTE_CPHY_IND_IFREQ_OTDOA_ID, lte_cphy_ifreq_otdoa_ind_s),
                     
  /*! @brief Message sent from ML1 to IMS to inform IMS of current CDRX cycle information
   *  so VoLTE voice packets can be synchronized with the CDRX on duration.
   */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, CDRX_PKT_TIMING,
                   LTE_CPHY_IND_CDRX_PKT_TIMING_ID, lte_cphy_cdrx_pkt_timing_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, SCELL_TUNEAWAY_STATUS,
                   LTE_CPHY_IND_SCELL_TUNEAWAY_STATUS_ID, lte_cphy_scell_tuneaway_status_s),

  /*! @brief Message sent from ML1 to RRC to inform RRC that a phychan schedule
   *  request has been sent to LL1 for RACH MSG1
   */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, RACH_MSG1_SCHED,
                   LTE_CPHY_IND_RACH_MSG1_SCHED_ID, lte_cphy_rach_msg1_sched_ind_s),

  /* message ID: LTE_CPHY_TX_POWER_REPORT_IND */
  MSGR_DEFINE_UMID(LTE, CPHY, IND, TX_POWER_REPORT,
                   LTE_CPHY_IND_UL_TX_POWER_ID, lte_cphy_tx_power_report_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, EMBMS_SET_PRIORITY,
                   LTE_CPHY_REQ_EMBMS_SET_PRIORITY_ID, lte_cphy_embms_set_prio_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, EMBMS_GET_MSP_DATA,
                   LTE_CPHY_REQ_EMBMS_GET_MSP_DATA_ID, lte_cphy_embms_get_msp_data_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, IND, EMBMS_GET_MSP_DATA,
                   LTE_CPHY_IND_EMBMS_GET_MSP_DATA_ID, lte_cphy_embms_get_msp_data_ind_s),
     
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, DDS_CHANGE,
                   LTE_CPHY_DDS_CHANGE_ID, lte_cphy_dds_change_req_s),
};

/*@}*//* end cphy_sap doxygen group */

/*===========================================================================

                   DATA PATH SAPS ( REQ, CNF, IND)

===========================================================================*/

/*! @defgrousp phy_sap Data Plane (CPHY) Interface

  @brief This module describes the data path interface to/from L1

  @detail The umids for this module are defined in @ref umids in the
          CPHY section.  The structures associated with the messages
          are defined in this module, as well as the documentation
          describing these messages

          This interface is generated from
          $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_cphy_msg.h#1 $ //source/qcom/qct/modem/lte/ML1/l1_common/main/latest/inc/lte_cphy_sap.h#46 $

  @todo Perhaps update CPHY->PHY

  @{*/

/*!
  @brief This indication is sent from ML1 to UL-MAC task to indicate to MAC to
  start building an uplink packet to be transmitted on PUSCH.

  */

typedef enum
{
  LTE_CPHY_NONE_PHR,
  LTE_CPHY_PHR,
  LTE_CPHY_EPHR,
} lte_cphy_phr_type_e;

/*!
  @brief This is the request type sent from DS to ML1 to start/update/stop DL
  throughput estimation.

*/
typedef enum 
{
  LTE_CPHY_DL_TPUT_ESTM_REPORT_TYPE_START,
  LTE_CPHY_DL_TPUT_ESTM_REPORT_TYPE_UPDATE,
  LTE_CPHY_DL_TPUT_ESTM_REPORT_TYPE_STOP
} lte_cphy_dl_tput_estm_report_type_e;

/*!
  @brief This is the confidence level type sent from ML1 to DS as part of
  throughput estimation report.

*/
typedef enum
{
  LTE_CPHY_DL_TPUT_ESTM_CONFIDENCE_LEVEL_LOW,     /*  0  */
  LTE_CPHY_DL_TPUT_ESTM_CONFIDENCE_LEVEL_MEDIUM,  /* 0.5 */
  LTE_CPHY_DL_TPUT_ESTM_CONFIDENCE_LEVEL_HIGH     /*  1  */
} lte_cphy_dl_tput_estm_confidence_level_e;

typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  /*! subscription ID */
  sys_modem_as_id_e_type subs_id;

  /*! report type */
  lte_cphy_dl_tput_estm_report_type_e req_type;

  /*! T_accumulate timer value. 
      Report is sent every T_accumulate.
  */
  uint16 t_accumulate;

} lte_cphy_dl_tput_estm_report_req_s;

typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  /*! subscription ID */
  sys_modem_as_id_e_type subs_id;

  /*! report type */
  lte_cphy_dl_tput_estm_report_type_e rsp_type;

} lte_cphy_dl_tput_estm_report_rsp_s;

typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  /*! subscription ID */
  sys_modem_as_id_e_type subs_id;

  /*! Additional available bytes */
  uint64 additional_avail_bytes;

  /*! confidence level about this report indication */
  lte_cphy_dl_tput_estm_confidence_level_e confidence_level;

  /*! Flag indicates if data freeze happen (no estimation performed) */
  boolean data_freeze;

  /*! Flag indicates if UE move from connected state to idle state */
  boolean moved_to_idle;

} lte_cphy_dl_tput_estm_report_ind_s;

typedef struct
{
  /*! SCellIndex(0 .. 7 ). Value 0 is for Pcell */
  uint8               scell_id;

  /*! RNTI type used to decode the PDCCH */
  lte_rnti_type_e     rnti_type;

  /*! Size of the transport block to be built.  Units are in bytes */
  uint32 trblk_size;

  /*! Address of the destination buffer */
  uint32 eib_address;

  /*! HARQ id for this interlace */
  uint8 harq_id;

  /*! Flag to indicate if extended PHR is triggered */
  lte_cphy_phr_type_e phr_type;

  /*! PHR payload - Only populated for the first carrier in the array */
  uint8 phr_payload_byte[LTE_CPHY_MAX_EPHR_PAYLOAD_LEN];

  /*! PHR payload length - Only populated for the first carrier in the array */
  uint8 phr_payload_length;

  /*! System Frame number - Only populated for the first carrier in the array*/
  lte_l1_sfn_t sys_fn;

  /*! Subframe number - Only populated for the first carrier in the array*/
  lte_l1_subfn_t  sub_fn;

  /*! Transmission type */
  lte_l1_tx_type_e  tx_type;

  /*! The USTMR time by which the write into the EIB needs to be 
     finished for this HARQ process */
  uint32 ustmr_eib_commit_time;

  /*! The flag for mac to reset the implicit release counter at SPS reactivation 
    Only populated for Pcell
  */
  boolean sps_activation;
  
  /*! Flag to indicate that this is the last HARQ retransmission 
      for this HARQ process. True only when tx_type is RETX and
      when the UE is in DSDS/SRLTE tuneaway mode */
  boolean final_harq_retx_attempt;

  /*! Flag to indicate that a CRC corruption is required to MAC. 
    This will be set for transmissions just before a QTA */
  boolean corrupt_crc;
} lte_cphy_ul_pkt_build_ind_s;

typedef struct 
{
  /*! Message header */
  msgr_hdr_struct_type     hdr;

  /*! Number of carriers (that will be included in the below array)*/
  uint8   num_carriers;

  /*! Information per carrier - PCC when present will always be at index 0*/ 
  lte_cphy_ul_pkt_build_ind_s carrier[LTE_CPHY_MAX_UL_CC];
} lte_cphy_ul_pkt_build_ind_msg_s;

/*! @brief This primitive is used by ML1 to inform MAC that all
           retransmissions on a particular packet has failed to get
           HARQ level ack from the eNodeB.

*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;
  /*! System frame number/ sub frame number (where pkt txn started) */
  lte_l1_cell_systime_s pkt_start_sfn;
} lte_cphy_ul_pkt_failed_ind_s;


/*! @brief This primitive is used by ML1 to inform MAC that the
    RACH procedure in connected mode due to PDCCH order is complete
        due to reception of C-RNTI
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

} lte_cphy_pdcch_order_rach_sucess_ind_s;


/*! @brief This message is sent to ML1 from MAC when the Up Link Timer is
           expired.

    @todo define the contents of this structure
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;
}lte_cphy_ul_ta_oos_req_s;

/*! @brief Enum of SR trigger types.
*/
typedef enum
{
    LTE_CPHY_SR_TRIGGER_NONE       =   0,              
    LTE_CPHY_SR_TRIGGER_UL_DATA,            /*!< Regular SR triggered for UL grants */
    LTE_CPHY_SR_TRIGGER_CONNECTIVITY_PROBE_WITH_DATA, /*!< Probe SR to check connectivity with retx = 3 */  
    LTE_CPHY_SR_TRIGGER_CONNECTIVITY_PROBE_NO_DATA,   /*!< Probe SR to check connectivity with retx = 1 */  
    LTE_CPHY_SR_TRIGGER_CONNECTIVITY_PROBE_INACTIVITY_EXPIRY            /*!< Probe SR to check connectivity with retx = 3 */ 
}lte_cphy_sr_trigger_type_e;

/*! @brief This message is sent from MAC to ML1 to request ML1 sending SR
           on PUCCH due to higher priority of UL Data Arrival.

    @todo define the contents of this structure
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;
  lte_cphy_sr_trigger_type_e trigger_type;
  lte_cphy_sr_retx_num_e sr_retx_num;
} lte_cphy_send_ul_sr_req_s;

/*! @brief This message is sent from MAC to ML1 to request ML1 to shrink SR max

    @todo define the contents of this structure
*/
typedef struct 
{
  msgr_hdr_struct_type     msgr_hdr;   /*!< message header */
} lte_cphy_send_gm_adj_sr_max_tx_ind_s;


/*! @brief
  This message is sent to MAC from ML1 to confirm with MAC that the status
  of the sending SR. Whether L1 gets grant due to sending SRs or L1 can't
  send SR at all.If not, then eventually, MAC will trigger a RACH.
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;
  /* TRUE to indicate that the SR has been sent by L1 and L1 just
  gets a grant. FALSE to indicate that L1 can't send out the SR becuase
  of no SR resource available/UL timing out of sync and there is no
  grant coming up */
  boolean  status;

  /* Flag to indicate to MAC that the CNF failed due to the expiry
     of allowed SR attempts */
  boolean max_sr_expired;
} lte_cphy_send_ul_sr_cnf_s;


/*! @brief This message is sent from MAC to ML1 to request ML1 to deactivate
           the Semi Persistent Scheduling.

    @todo define the contents of this structure
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;
  lte_l1_sfn_t sys_fn;        /*!< System Frame number */
  lte_l1_subfn_t  sub_fn;     /*!< Subframe number */
} lte_cphy_deactivate_sps_req_s;

/*! @brief This message is sent from ML1 to ML1 to MAC to confirm that
           Semi-Persistent Scheduling  is deactivated.

    @todo define the contents of this structure
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;
} lte_cphy_deactivate_sps_cnf_s;

/*! @brief CPHY MAC DRX CMD This message is sent from MAC to ML1 to indicate
           that a DRX command was received in the MAC CE
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! current sfn for drx req msg*/
  uint16                            mac_drx_ce_sf;  

} lte_cphy_mac_drx_req_s;


/*! @brief Indication sent from ML1 to MAC to indicate SFN is unknown/known. 
  This will only be sent at the time of handovers.

*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type  msgr_hdr;

  /*! SFN Known flag. Set to FALSE when handover starts and later to TRUE once PBCH is decoded */
  boolean               sfn_known;
  /*! System frame number/ sub frame number (valid only if sfn_known == TRUE) */
  lte_l1_cell_systime_s curr_sfn;
} lte_cphy_handover_sfn_status_ind_s;



/*! @brief
  Enum for the external data-plane saps used to interface with L1
*/
enum
{

  MSGR_DEFINE_UMID(LTE, CPHY, IND, UL_PKT_BUILD,
                   LTE_CPHY_IND_UL_PKT_BUILD_ID, lte_cphy_ul_pkt_build_ind_msg_s),
  MSGR_DEFINE_UMID(LTE, CPHY, IND, UL_PKT_FAILED,
                   LTE_CPHY_IND_UL_PKT_FAILED_ID, lte_cphy_ul_pkt_failed_ind_s),
  MSGR_DEFINE_UMID(LTE, CPHY, IND, PDCCH_ORDER_RACH_SUCCESS,
                   LTE_CPHY_IND_PDCCH_ORDER_RACH_SUCCESS_ID, lte_cphy_pdcch_order_rach_sucess_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, UL_TO_OOS,
                   LTE_CPHY_UL_TO_OOS_ID, lte_cphy_ul_ta_oos_req_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, SEND_UL_SR,
                   LTE_CPHY_SEND_UL_SR_ID, lte_cphy_send_ul_sr_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, SEND_UL_SR,
                   LTE_CPHY_SEND_UL_SR_ID, lte_cphy_send_ul_sr_cnf_s),
                   
  MSGR_DEFINE_UMID(LTE, CPHY, IND, SEND_GM_ADJ_SR_MAX_TX,
                   LTE_CPHY_SEND_GM_ADJ_SR_MAX_TX_ID, lte_cphy_send_gm_adj_sr_max_tx_ind_s), 
                   
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, DEACTIVATE_SPS,
                   LTE_CPHY_DEACTIVATE_SPS_ID, lte_cphy_deactivate_sps_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, DEACTIVATE_SPS,
                   LTE_CPHY_DEACTIVATE_SPS_ID, lte_cphy_deactivate_sps_cnf_s),
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, MAC_DRX,
                   LTE_CPHY_MAC_DRX_ID, lte_cphy_mac_drx_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, IND, HANDOVER_SFN_STATUS,
                   LTE_CPHY_IND_HANDOVER_SFN_STATUS_ID, lte_cphy_handover_sfn_status_ind_s),

  MSGR_DEFINE_UMID(LTE, CPHY, REQ, DL_TPUT_ESTM_REPORT,
                   LTE_CPHY_DL_TPUT_ESTM_REPORT_ID, lte_cphy_dl_tput_estm_report_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, RSP, DL_TPUT_ESTM_REPORT,
                   LTE_CPHY_DL_TPUT_ESTM_REPORT_ID, lte_cphy_dl_tput_estm_report_rsp_s),
  MSGR_DEFINE_UMID(LTE, CPHY, IND, DL_TPUT_ESTM_REPORT,
                   LTE_CPHY_DL_TPUT_ESTM_REPORT_ID, lte_cphy_dl_tput_estm_report_ind_s),

};

enum
{
  MSGR_DEFINE_UMID(LTE, CPHY, IND, MAC_RAA, LTE_CPHY_IND_MAC_RAA_ID, 
                   lte_cphy_raa_ind_msg_s ),
  MSGR_DEFINE_UMID(LTE, CPHY, IND, T310_STATUS, LTE_CPHY_IND_T310_STATUS_ID, 
                   lte_cphy_t310_status_ind_s ),    
};

/*! @brief After the packet is built, following status will be written
           in some shared  memory array.  Shared memory is indexed by harq_id.
           This will eventually be defined in the tx swi
  */
#define LTE_PHY_UL_PKT_READY 0xdeadbeef


/*! @brief
    Data Structure for LTE_CPHY_RFCHAIN_REQ

    @details
      DS uses LTE_CPHY_RFCHAIN_REQ to enable/disable DRX/PRX.
      the message of the payload is a byte to indicate the actions
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! the action code 
      Bit 7: 0 -- does not need LTE_CPHY_RFCHAIN_CNF
             1 -- needs to send LTE_CPHY_RFCHAIN_CNF
      Bit 6 to bit 3: reserved, all 0's
      Bit 2: specify the action of enabling or disabling. 
           0  -- enable
           1  -- disable
      Bit 1: specify whether the action applies to RX1
           0 - not applicable. The action defined by bit 2 will not apply to RX1
           1 - applicable. The action defined by bit 2 will apply to RX1
      Bit 0: specify whether the action applies to RX0
           0 - not applicable. The action defined by bit 2 will not apply to RX0
           1 - applicable. The action defined by bit 2 will apply to RX0


      The following table is a usage summary for the bit 2 to bit 0:
         Byte value      Enable/Disable Rx1     Rx0     Summary
               0                 0                 0     0   No action
               1           0                   0   1     Enables RX0 only
               2           0               1   0         Enables RX1 only
               3           0               1   1         Enables RX0 and RX1
               4           1               0     0   No action
               5           1               0   1   Disables RX0 only
               6           1               1   0         Disables RX1 only
               7           1             1   1   Disables RX0 and RX1

  */
  uint8                             action_code;
  /*! the carrier ID
      To support Carrier Aggregation(CA), the carrier ID needs to be specified:

      0: Primary Carrier Component (PCC)
      1: Secondary Carrier Component (SCC)

      As for now, only one SCC is deployed.
  */
  uint8                             carrier_id;
} lte_cphy_rfchain_req_s;


/*! @brief
    Data Structure for LTE_CPHY_RFCHAIN_CNF

    @details
      when bit 7 of action_code in LTE_CPHY_RFCHAIN_REQ payload is set to 1,
      ML1 sends back this response message LTE_CPHY_RFCHAIN_CNF, with the following
      payload definition.
 */     
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /* the following is the staus code */
  /*
      0: LTE_CPHY_RFCHAIN_REQ successfully processed
      1: ML1 is busy and can not process the command
      2: RF cannot process the command
      3: RF returns error
      4: general error
   */
  uint8                             status;
} lte_cphy_rfchain_cnf_s;

/*! @brief
  Enum of UMID to pass down RF Chain request
*/
enum
{

  /* message ID: LTE_CPHY_RFCHAIN_REQ */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, RFCHAIN,
                   LTE_CPHY_RFCHAIN_ID, lte_cphy_rfchain_req_s)
};


/*! @brief
  Enum of UMID to send back RF Chain request process status
*/
enum
{

  /* message ID: LTE_CPHY_RFCHAIN_CNF */
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, RFCHAIN,
                   LTE_CPHY_RFCHAIN_ID, lte_cphy_rfchain_cnf_s)
};

/*@}*//* end phy_sap doxygen group */



#endif /* LTE_CPHY_MSG_H */
