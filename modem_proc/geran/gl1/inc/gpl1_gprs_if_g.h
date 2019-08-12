#ifndef GPL1_GPRS_IF_G_H
#define GPL1_GPRS_IF_G_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 1  G P R S  T Y P E S   H E A D E R   F I L E

DESCRIPTION
   GPRS Layer 1 type declarations for use in both the ISR and the TASK


Copyright (c) 2001-2014 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gpl1_gprs_if_g.h#1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
03/10/14   aga      CR732455 Changes required to tune away framework for TH 1.0 single sim build 
06/08/14   df       CR536059 Allow Asynchronous sleep for all idle activities
30/05/14   ws       CR657674 Data + MMS Phase 1
08/05/14   br       CR661054 In DSDA mode send  MAC_L1 release cnf to transfer sub  after checking gas id
15/04/14   mko      CR604883 Using correct Macro to get Max Tx slots for MSC10 in gpl1_gprs_clean_rrbp_txd_info()
03/01/13   cja      CR435212 Delay sending l1 mac single block sent until in idle state.
26/03/12   jj       CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33
26/08/11   ab       CR299297 : Incorrect Handling of Control messages during TBF
                    reconfiguration boundary
27/07/2011 ab       CR298128 Enhancement of SNR reselection in Transfer mode.
21/03/11   ky       CR277664:Ignoring the DL data block received with correct DL TFI but
                    on a different timeslot which is not allocated in the current downlink tbf
26/08/11   ab       CR299297 : Incorrect Handling of Control messages during TBF
                    reconfiguration boundary
27/07/2011 ab       CR298128 Enhancement of SNR reselection in Transfer mode.
25/05/11   ap       DSDS CR:289094 - Signal release of the Single block after the mDSP
                    complete has began to terminate
21/03/11   ky       CR277664:Ignoring the DL data block received with correct DL TFI but
                    on a different timeslot which is not allocated in the current downlink tbf
14/01/11   ab       CR 258308   : P0 write buffer overflow in func
                    gpl1_gprs_calculate_paging_data()
30/11/10   ab       Support EGPRS WS REDUCTION during allocation of dynamic memory in start
                    gsm mode request when FEATURE_GSM_EGPRS_IR_OPT is defined.
06/09/10   ab       ENH Add TEST MODE B Reconfig and add asymetric TS support.
04/10/10   ab       HMSC updates.
06/05/10   dv       CR237642 - Add ASRB funtionality
10-04-01   tjw      Add customer.h and comdef.h to all SU API files
11/12/09   ab       CR 220464 Add REL_IND/REL_CNF to DL_SINGLE_BLOCK to prevent L1 and MAC
                    go out of sync.
02/12/09   ws       Added FEATURE_GSM_GPRS_AGC_UPDATE for GPRS/EGPRS AGC
                    Improvements
04/09/09   ap       CR 193147 Incorrect handling of TFI
09/07/09   ab       CR 183589 Incorrect handling of RLC mode change request
                    during extend UL TBF operation
09/06/09   nf       Added FEATURE_GSM_PCH_SINGLE_BURST_DEC
12/03/09   ws       CR 173134 Added Test mode A to L1 interface and calculate
                    RLA_P for test modes from PDCH
24/11/08   og       FEATURE_GSM_GPRS_MSC33 GPL1 changes.
14/03/08   ip       Added PSHO chnages in feature FEATURE_GPRS_PS_HANDOVER
29/06/07   cs       Fix WPLT build due to featurisation
06/06/07   ws       CR 118908 - Don't reset nc_mode in l1_sc_init() since this
                    is called when we reselect to PBCCH cell stopping NC2 reporting
05/03/07   og       Add GPRS/EGPRS Multi Slot Class 33 and 34 definitions.
06/02/07   cs       Fix compile issue when EDTM not defined
26/01/07   og       Implement UL ctrl msg deletion in L1.
30/10/06   og       DTM enhanced release code drop.
14/07/06   og       Add DTM multislot class 11.
10/07/06   og       Use DTM multislot parameters in DTM mode. Resolves
                    CR94827.
15/05/06   og       Added definitions for multislot class 5 and 9 capabilities.
17/03/06   og       Take downlink assignment mac mode into account. This is
                    required by release 4 ETSI spec.
26/01/06   og       Neighbour cell acquisition changes to handle extended dynamic.
05/09/05   og       Multislot class 12 support.
22/08/05   og       Add the setting of mac mode to NPL1.
19/08/05   og       Removal of downlink block FIFO.
17/08/05   og       Removal of dummy ctrl block dual buffer.
04/08/05   og       Merge change 225343 from the raven branch.
25/04/05   nt       Added the member tbf_release_req_type to the structure transfer_data_T.
25/04/05   ws       Changed size of dummy_ctrl_block array to even number of bytes
21/3/05    og       Lint error removal.
11/3/05    og       Changes to egprs dl tfi and IR handling.
11/02/05   ws       Added Flag for DL release handler to ensure PDAS or PTR megs
02/02/05   gfr      Moved IR defines to mDSP driver
06/12/04   ws       Updates for TEST mode B for GPRS
16/12/04   og       Merge of fix from the saber 4x branch.
2/11/04    og       Addition of egprs srb test mode capability.
19/10/04   og       Added single block bep period2 handling.
15/10/04   og       Removed Fixed TBF allocation related source code.
12/10/04   ws       Added Multiblock allocation for EGPRS
27/09/04   ws       Added ptcch_bad_snr_valid flag - Fix for CR 48942
09/07/04   ws       Updated defn for DL IR handling for EGPRS
23/06/04   og       Egprs Qual measurements handling.
13/05/04   rm       Structure alignment to minimise padding
27/04/04   rm       Moved RRBP, PTCCH and TA parameters to RRBP and PTCCH modules
19/03/04   ws       Added support for bad snr detection on PTCCH/D decodes
18/03/04   dlh      Added support for nc non drx period
09/03/04   ws       Added abort_receives flag to transfer_data structure
07/01/04   ws       Added prototype for gpl1_gprs_send_l1_mac_rrbp_failure_ind()
06/11/03   gfr      Support for quad-band.
05/11/03   kf       changed the size of SIZE_OF_RRBP_BUFF
12/09/03   pjr      Added boolean release_in_progress and release_in_progess_delay
                    to L1_TRANSFER params
09/09/03   ws       Modified reconfiguring PTCCH channels only when a param
                    change is detected
03/09/03   ws       Added types for confirmation of RRBP transmision
31/07/03   bk       Added extra field rrbp_sent_before_idle to indicate when
                    RRBP is sent in the block before the idle frame.
26/06/03   ws       wait until PCA is txd before releasing TBF
23/06/03   gfr      Added prototype for gpl1_gprs_calc_rf_pwr
17/06/03   ws       Added vars for ptcch/d retry as per GSM 05.10
10/06/03   pjr      Added prototypes for gpl1_gprs_L1_params_pending() and
                    gpl1_gprs_update_L1_params(). Added Simultaneous PUA/PDA
                    starting time variables to l1_transfer_data.
15/05/03   ws       Implemented toggling for ul dummy data
14/05/03   pjr      Increased size of SIZE_OF_RRBP_BUFF to 24.
24/04/03   ws       Added L1_NULL_ALLOC to transfer_allocation_t
07/03/03   pjr      Modifed confirm_pending enum and added abort_fn and rrbp_pending
                    to transfer_data_T
26/02/03   npr      Changed instances of INVALID to GPL1_INVALID
20/02/03   pjr      Added received_in_idle parameter to rrbp_params_T
13/02/02   ws       Added support for Dl power control
12/02/02   ws       Added extern vars for reading FN from task when drx
                    manager is asleep
19/12/02   pjr      Removed power control structures.
15/11/02   ws       Removed first_pdan_with_fai from transfer_data_T type
06/11/02   ws       Added FULL release cases to  release_T
04/11/02   pjr      Merge Neighbour cell measurements under feature switch
01/11/02   pjr      Removed L1_GPRS_INVALID_FN now defined in gmacsigl1.h
09/10/02   ws       Added sb_ul_dl_t for single dl block
13/09/02   pjr      Modified confirm_pending process to use enum
05/09/02   pjr      Added confirm_pending to transfer_data_T
05/09/02   pjr      Added l1_called_from_sngl_blk to transfer_data_T
1/08/02    ws       Added release enum type
                    added num_ul_rrbp num_dl_rrbp and num_tn_rrbp msgs

15/07/02   ws       Added ta_description_T and removed timing advance description
                    from dynamic_description_T and downlink_description_T
02/07/02   ws       Added support for radio block gap, added support for
                    release once rrbp has been scheduled and support for
                    sending L1_MAC_1ST_RADIO_BLOCK_SENT
31/05/02   ws       Added pending data pointers in transfer_data_T
27/05/02   ws       Added downlink tbf pointer to fixed and dynmaic alloc structs
05/09/02   pjr      Added single_block_tbf to transfer_data_T and definition
                    of single_blk_information_T
04/12/01   ws       Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gmacl1sig.h"
#include "gmacl1sig_g.h"
#include "gmacsigl1.h"
#include "gmacsigl1_g.h"
#include "gl1rlc.h"
#include "grlculfifo.h"
#include "gl1_defs.h"
#include "gl1_defs_g.h"
#include "l1_sc_int.h"    /* l1_sc_bs_T     */
#include "sys_stru.h"
#include "gs.h"           /* gs_status_T    */
#include "rr_l1.h"
#include "rr_l1_g.h"

#ifdef PL1_FW_SIM
#define GPL1_DEBUG printf   // for debuging purposes
#else
#define GPL1_DEBUG //
#endif

/* Defines the maximum number of PACCH messages
 * which can be buffered in L1 received via
 * MAC_PH_DATA_REQ signal
 */
#ifdef  FEATURE_GSM_GPRS_MSC33

#define  MAX_NUM_UL_PACCH_MSGS  (PL1_MULTISLOT_TX)

#else

#define  MAX_NUM_UL_PACCH_MSGS  (4)

#endif

/* Defines the maximum number of PACCH or PDTCH
 * messages which can be buffered in L1 prior to
 * sending to RLC or MAC
 * make this 12 since  12 messages can be
 * schedules before a data callback is invoked
 * each message requires
 */
#ifdef  FEATURE_GSM_GPRS_MSC33

#define  MAX_NUM_DL_MSGS  ((PL1_MULTISLOT_RX)*(GL1_DEFS_FRAMES_IN_BLK_PERIOD)) /* @@TODO double check how the value is calculated */

#else

#define MAX_NUM_DL_MSGS 12

#endif


/* Max number of UL RRBP msgs stored in txd_buff based on MSC type*/

#ifdef  FEATURE_GSM_GPRS_MSC33

#define  MAX_NUM_UL_RRBP_MSGS  (PL1_MULTISLOT_TX)

#else

#define  MAX_NUM_UL_RRBP_MSGS  (4)

#endif

/* Defines the Low watermark in RLC's uplink
 * fifo. If RLC message count falls below this
 * level then the RLC_PH_READY_TO_SEND
 */
#define RLC_UL_LOW_WATER 4

/* used to invalidate block start counters and timeslots */

#define GPL1_INVALID 0xff

#define GPL1_TFI_INVALID 0xff
/* Checks if frame number matches with a PTCCH or IDLE frame
* idle frames are on (%52) 25, 51
* PTCCH frames are on (%52) 12,38
*/

#define CHECK_FOR_PTCCH_IDLE(x) ( (x % 13) != 12 ? FALSE : TRUE)


/* Possible values for the L1 test mode
 * parameter used during packet transfer.
 * l1_transfer_test_mode_t
 */
#define L1_TEST_MODE_OFF      0x0
#define L1_TEST_MODE_B        0x01
#define L1_TEST_MODE_SRB_OFF  0x02
#define L1_TEST_MODE_SRB_ON   0x03
#define L1_TEST_MODE_A        0x04

/* L1 packet transfer test mode type */
typedef uint8  l1_transfer_test_mode_t;


/* Used to ensure mac mode is not erroroneously set
 * when pdch/tbf release is being handled during
 * reconfiguration.
 */
#define L1_MAC_MODE_INVALID  (0xFF)


#ifdef  FEATURE_GSM_DTM

/* Multislot class 5 capabilities */
#define PL1_CLASS5_RX   (2U)
#define PL1_CLASS5_TX   (2U)
#define PL1_CLASS5_SUM  (4U)

#define PL1_CLASS5_Tta  (3U)
#define PL1_CLASS5_Trb  (1U)

#define PL1_CLASS5_Ttb  (1U)
#define PL1_CLASS5_Tra  (3U)

/* Multislot class 9 capabilities */
#define PL1_CLASS9_RX   (3U)
#define PL1_CLASS9_TX   (2U)
#define PL1_CLASS9_SUM  (5U)

#define PL1_CLASS9_Tta  (3U)
#define PL1_CLASS9_Trb  (1U)

#define PL1_CLASS9_Ttb  (1U)
#define PL1_CLASS9_Tra  (2U)

/* Multislot class 11 capabilities */
#define PL1_CLASS11_RX   (4U)
#define PL1_CLASS11_TX   (3U)
#define PL1_CLASS11_SUM  (5U)

#define PL1_CLASS11_Tta  (3U)
#define PL1_CLASS11_Trb  (1U)

#define PL1_CLASS11_Ttb  (1U)
#define PL1_CLASS11_Tra  (2U)

#endif /* FEATURE_GSM_DTM */

/* Multislot class 10 capabilities */
#define PL1_CLASS10_RX   (4U)
#define PL1_CLASS10_TX   (2U)
#define PL1_CLASS10_SUM  (5U)

#define PL1_CLASS10_Tta  (3U)
#define PL1_CLASS10_Trb  (1U)

#define PL1_CLASS10_Ttb  (1U)
#define PL1_CLASS10_Tra  (2U)


/* Multislot class 12 capabilities */
#define PL1_CLASS12_RX   (4U)
#define PL1_CLASS12_TX   (4U)
#define PL1_CLASS12_SUM  (5U)

#define PL1_CLASS12_Tta  (2U)
#define PL1_CLASS12_Trb  (1U)

#define PL1_CLASS12_Ttb  (1U)
#define PL1_CLASS12_Tra  (2U)

#ifdef  FEATURE_GSM_GPRS_MSC33

/* Multislot class 33 capabilities */
#define PL1_CLASS33_RX   (5U)
#define PL1_CLASS33_TX   (4U)
#define PL1_CLASS33_SUM  (6U)

#define PL1_CLASS33_Tta  (2U)
#define PL1_CLASS33_Trb  (1U)

#define PL1_CLASS33_Ttb  (1U)
#define PL1_CLASS33_Tra  (1U)

/* Multislot class 34 capabilities */
#define PL1_CLASS34_RX   (5U)
#define PL1_CLASS34_TX   (5U)
#define PL1_CLASS34_SUM  (6U)

#define PL1_CLASS34_Tta  (2U)
#define PL1_CLASS34_Trb  (1U)

#define PL1_CLASS34_Ttb  (1U)
#define PL1_CLASS34_Tra  (1U)

#endif /* FEATURE_GSM_GPRS_MSC33 */


/* Multislot class capability parameters */


#ifdef  FEATURE_GSM_GPRS_MSC33

/* Multislot parameters specific to Packet Transfer
 * mode.
 */
#define PL1_MULTISLOT_RX   (PL1_CLASS33_RX)
#define PL1_MULTISLOT_TX   (PL1_CLASS33_TX)
#define PL1_MULTISLOT_SUM  (PL1_CLASS33_SUM)

#define PL1_MULTISLOT_Tta  (PL1_CLASS33_Tta)
#define PL1_MULTISLOT_Trb  (PL1_CLASS33_Trb)

#define PL1_MULTISLOT_Ttb  (PL1_CLASS33_Ttb)
#define PL1_MULTISLOT_Tra  (PL1_CLASS33_Tra)

#else

/* Multislot parameters specific to Packet Transfer
 * mode.
 */
#define PL1_MULTISLOT_RX   (PL1_CLASS12_RX)
#define PL1_MULTISLOT_TX   (PL1_CLASS12_TX)
#define PL1_MULTISLOT_SUM  (PL1_CLASS12_SUM)

#define PL1_MULTISLOT_Tta  (PL1_CLASS12_Tta)
#define PL1_MULTISLOT_Trb  (PL1_CLASS12_Trb)

#define PL1_MULTISLOT_Ttb  (PL1_CLASS12_Ttb)
#define PL1_MULTISLOT_Tra  (PL1_CLASS12_Tra)

#endif

/* Multislot parameters specific to Dual Transfer
 * mode.
 */
#ifdef  FEATURE_GSM_DTM

#define PL1_DTM_MULTISLOT_RX   (PL1_CLASS11_RX)
#define PL1_DTM_MULTISLOT_TX   (PL1_CLASS11_TX)
#define PL1_DTM_MULTISLOT_SUM  (PL1_CLASS11_SUM)

#define PL1_DTM_MULTISLOT_Tta  (PL1_CLASS11_Tta)
#define PL1_DTM_MULTISLOT_Trb  (PL1_CLASS11_Trb)

#define PL1_DTM_MULTISLOT_Ttb  (PL1_CLASS11_Ttb)
#define PL1_DTM_MULTISLOT_Tra  (PL1_CLASS11_Tra)

#endif



/*
BS_PAG_BLKS_RES (4 bit field)
The BS_PAG_BLKS_RES field indicates the number of blocks on each PDCH carrying the
PCCCH per multiframe where neither PPCH nor PBCCH should appear.
Range: 0-10. The BS_PAG_ BLKS_RES value shall fulfil the condition that is defined
in 3GPP TS 45.002. If the condition is not fulfilled, then the behaviour of the mobile
station is implementation dependent.
*/
#define MAX_BS_PAG_BLKS_RES 10

/*
The BS_PAG_BLKS_RES value shall fulfil the condition :
BS_PAG_BLKS_RES <= 12 - BS_PBCCH_BLKS - 1.
BS_PAG_BLKS_RES + BS_PBCCH_BLKS <= 11
*/
#define VALIDATE_PAGING_GRP_EQ  11


/* Release type enum, used to describe if a release will terminate
 * all tbf's (L1_FULL_RELEASE) or only one tbf (L1_PARTIAL_RELEASE)
 */
typedef enum
{
  L1_NO_RELEASE,
  L1_PARTIAL_RELEASE_UL,
  L1_PARTIAL_RELEASE_DL,
  L1_PARTIAL_RELEASE_TN,
  L1_FULL_RELEASE_UL,
  L1_FULL_RELEASE_DL,
  L1_FULL_RELEASE_UL_DL
} release_T;

/* Enumerated type for defining the various allocation types
 * used in packet transfer mode
 */

typedef enum
{
  L1_NULL_ALLOC,
  L1_FIXED_ALLOC,
  L1_DYNAMIC_ALLOC,
  L1_DOWNLINK_ALLOC,
  L1_SINGLE_BLOCK_ALLOC,
  L1_FIXED_DOWN_ALLOC,
  L1_DYNAMIC_DOWN_ALLOC
} transfer_allocation_T;

/* enumerated type to deine the difference between single ul block
 * and a single dl block
 */
typedef enum
{
  SINGLE_UL_BLOCK,
  SINGLE_DL_BLOCK
}sb_ul_dl_t;


/* Defines the parameters required to describe the
 * nc_non_drx_params
 */
typedef struct
{
  uint16  nc_non_drx_period;
  uint8   nco;
  boolean meas_report;
  boolean nc_non_drx_valid;
} nc_non_drx_params_T;

/* Defines the parameters required to describe the
 * downlink tbf in packet transfer mode
 */
typedef struct
{
  uint32                  dl_tfi_st;
  uint32                  tbf_start_time;
  uint8                   ts_allocation;
  uint8                   nts;
  uint8                   dl_tfi;
  uint8                   previous_dl_tfi;
  uint8                   previous_dl_ts_alloc;
  downlink_pwr_t          downlink_pwr;
  timing_advance_params_T ta_params;
  boolean                 dl_tfi_present;
  boolean                 allow_one_rb_gap;
  uint8                   link_qual_meas_mode;
  uint8                   bep_period2;
  mac_rlc_mode_T          rlc_mode;
  uint8                   mac_mode;
#ifdef FEATURE_G2X_TUNEAWAY
  boolean                dl_tbf_with_t3192_active;
#endif
} downlink_tbf_information_T;

/* Defines parameters to describe the dynamic allocation
 * parameters for use in Packet transfer mode
 */
typedef struct
{
  /*
   * mapping = element 0 = usf value for TN0
   *           element 7 = usf value for TN7
   *           usf value range 0 to 7
   *           usf value = 0xff TN not supported
   */
  uint8                         usf_value[GL1_DEFS_SLOTS_IN_FRAME];
  uint8                         nts;
  uint8                         sched_msgs;
  //timing_advance_params_T       timing_advance_params;
  boolean                       usf_granularity;
  uint32                        starting_time;

  downlink_tbf_information_T    *dl_tbf_ptr;
  uint32                         ul_tfi_st;

  test_mode_T                   test_mode;
  uint8                         test_mode_tn;
  uint8                         bep_period2;
  boolean                       srb_mode;

  boolean                       allow_one_rb_gap;

  uint8                         mac_mode;
} dynamic_tbf_information_T;



/*  Define the parameters required for single block */
typedef struct
{
  boolean                 two_phase_access;
  uint8                   ts_number;
  boolean                 alpha_present;
  uint8                   alpha;
  uint8                   gamma_tn;
  downlink_pwr_t          downlink_pwr;
  timing_advance_params_T timing_advance_params;
  sb_ul_dl_t              sb_ul_dl_type;
  nc_non_drx_params_T     nc_non_drx_params;
  uint8                   num_rb_alloc;
  boolean                 sb_crc_fail;
  uint8                   bep_period2;
} single_blk_information_T;

/* Defines strorage buffer for a PACCH control message */
/* SIZE_OF_COTROL_RADIO_BLOCK defined in gprsdef.h */

typedef struct
{

#ifdef PL1_FW_SIM
  /* Required since PL1 FW Sim requires even bytes */
  uint8             control_block[SIZE_OF_CONTROL_RADIO_BLOCK + 1];
#else
  uint16            filler; /* for aligning to an even adress */
  uint8             control_block[SIZE_OF_CONTROL_RADIO_BLOCK];
#endif

#if defined ( FEATURE_GSM_DTM ) && defined ( FEATURE_GSM_EDTM )
  boolean  valid;
#endif

} pacch_msg_buffer_T;

/* Defines buffer for storing MAC Uplink control
 * buffer received by signal MAC_L1_PH_DATA_REQ
 */
typedef struct
{
    boolean                data_valid;
    uint8                 pending_msgs; /* number of msgs awaiting to be scheduled */
    uint8                 sched_msgs;   /* number of msgs awaiting to be transmitted */
    pacch_msg_buffer_T    *pacch_data_in_ptr; /* ptr to where msg from MAC is to be stored */
    pacch_msg_buffer_T    *pacch_data_out_ptr; /* ptr to where msg is take from to be scheduled */
    pacch_msg_buffer_T    *last_pacch_txd_ptr; /* ptr to last transmitted msg */
    pacch_msg_buffer_T      pacch_data[MAX_NUM_UL_PACCH_MSGS];

} mac_ul_ctrl_buffer_T;



typedef struct
{
  uint8 *loop_bk_data_ptr;
  boolean tx_loop_bk_data;
}gpl1_gprs_tx_loop_t;


typedef struct
{
  uint8 tx_rrbp_status;
  uint8 rx_ctrl_status;
  uint8 ul_nts;
  grlc_ul_fifo_access_block_t gpl1_gprs_tx_loop[MAX_NUM_UL_PACCH_MSGS];
  uint8 ul_tn[MAX_NUM_UL_PACCH_MSGS];

}gpl1_gprs_tm_b_store_t;


/* Used for converting mcs value from RLC to MCS value used by NPL1
 * as defined in gl1_defs_coding_type enum.
 */
#define MSC_CS_OFFSET 6


/* Used for converting puncturing scheme values received from RLC
 * for use in programming NPL1 for TX's
 */
#define PS_OFFSET 1

/* Used to define length of padding added to the start of each
 * RLC payload if MCS6 or MCS3 is used to retransmit MCS8
 */

#define PADDING_OCTET_LEN 6


 /* Used to determine if associated MCS contains 2 payloads or not */

#define IS_MSG_DUAL_PAYLOAD(mcs) (((mcs >=  GL1_DEFS_MCS7_CODING) && \
                                 (msg_data->coding_scheme <= GL1_DEFS_MCS9_CODING)) \
                                 ? TRUE : FALSE)

/* Defines the maximum RLC window size for a 4 timeslot TBF */

#ifdef FEATURE_GSM_GPRS_MSC33
#define MAX_EGPRS_WINDOW_SIZE 640
#else
#define MAX_EGPRS_WINDOW_SIZE 512
#endif


/* Defines structure for storing RLC ack status and pointer
 * for a single message using either EGPRS or GPRS definition
 */

typedef struct
{
    tx_ack_state_t          ack_state;
    grlc_ul_fifo_element_t  *blk_ptr;
} rlc_ul_desc_T;


/* Defines structure for storing pointers and ack status of
 * last RLC UL data for controlling sending of RLC_PH_READY_TO_SEND
 * signal
 */

typedef struct
{
    uint8                 num_msgs;

#ifdef  FEATURE_GSM_GPRS_MSC33
    rlc_ul_desc_T         ul_desc[MAX_NUM_UL_PACCH_MSGS];
#else
    rlc_ul_desc_T         ul_desc[4];
#endif

    rlc_ul_desc_T         *ul_desc_ptr;

} rlc_ul_status_T;


typedef struct
{
  uint8 e_sp;
  uint8 rrbp;
  uint8 dl_tfi;
  uint8 pr;
  uint16 bsn1;
} pl1_mcs_hdr_info_T;

/*
 * Defines a structure containing parameters for
 * controlling the dl control buffer for sending
 * MAC_PH_DATA_IND to MAC while NPL1 is copying
 * new decoded data into PL1.
 * This fifo is needed to allow the MAC_PH_DATA_IND
 * to be sent by the L1 task and new data to be
 * copied into PL1 via the L1 ISR.
 */
typedef struct
{
    gl1_defs_rx_pkt_data_type *in_ptr;
} rlc_mac_dl_buffer_T;

/* Structure defines information required to configure
 * a single RRBP at some radio block in the future
 */

typedef struct
{
  boolean ul_cont_ta_valid;
  boolean dl_cont_ta_valid;
  timing_advance_value_T tav;
  cont_ta_information_T ul_ta_params;
  cont_ta_information_T dl_ta_params;
  cont_ta_information_T *ta_param_ptr;
} ta_description_T;


 typedef struct
{
    uint8    num_bursts;
    dBx16_T  snr_bursts[4];  /* norm or pre-mimo */
    dBx16_T  norm_snr_bursts[4]; /* always norm burst */
} ptcch_snr_buf_T;

typedef struct
{
    boolean                 ta_valid;
    boolean                 ptcch_dl_valid;
    boolean                 ptcch_reconfig;
    boolean                 ptcch_snr_bad;
    boolean                 ptcch_snr_bad_valid;
    uint8                   ptcch_decode_attempts;
    uint8                   downlink_tai;
    cont_ta_information_T  *global_ta_params;
    ptcch_snr_buf_T         ptcch_snr_buff;
    ta_description_T        active_ta_params;
    ta_description_T        pending_ta_params;
} gpl1_gprs_ta_T;

extern gpl1_gprs_ta_T l1_get_gpl1_gprs_ta_info ( gas_id_t gas_id );
extern void l1_set_gpl1_gprs_ta_info (gpl1_gprs_ta_T gpl1_gprs_ta_info,gas_id_t gas_id );

void gpl1_gprs_L1_params_pending(IMH_T *msg_header);
void gpl1_gprs_update_L1_params(gas_id_t gas_id);
void gpl1_gprs_calc_rf_pwr(gas_id_t gas_id);


dtm_tbf_type_t get_queued_mac_l1_edtm_cs_rel_tbf_alloc( gas_id_t gas_id );
void set_queued_mac_l1_edtm_cs_rel_tbf_alloc(dtm_tbf_type_t tbf_type, gas_id_t gas_id);

/* Defines a structure for storing all relevent
 * information required for packet transfer mode
 * internally in Layer 1
 */


typedef struct
{
  /* Packet hopping/single arfcn information */
  frequency_information_T     frequency_info_1;
  frequency_information_T     frequency_info_2;
  frequency_information_T     *frequency_info_ptr;
  frequency_information_T     *pending_frequency_info_ptr;

  /* Uplink Tbf information */
  dynamic_tbf_information_T   dynamic_tbf_1;
  dynamic_tbf_information_T   dynamic_tbf_2;
  dynamic_tbf_information_T   *dynamic_tbf_ptr;
  dynamic_tbf_information_T   *pending_dynamic_tbf_ptr;

  /* Downlink tbf information */
  downlink_tbf_information_T  dl_tbf_1;
  downlink_tbf_information_T  dl_tbf_2;
  downlink_tbf_information_T  *dl_tbf_ptr;
  downlink_tbf_information_T  *pending_dl_tbf_ptr;

  /* Single block tbf information */
  single_blk_information_T    single_block_tbf_1;
  single_blk_information_T    single_block_tbf_2;
  single_blk_information_T    *single_block_tbf_ptr;
  single_blk_information_T    *pending_single_tbf_ptr;
  boolean                     send_l1_mac_single_block_sent;
  boolean                     disable_rx_tx;
  rrbp_msg_t                  last_rrbp_msg_type;
  uint16                      filler; /* to align to an even adress */

  uint8                       dummy_control_block[SIZE_OF_CONTROL_RADIO_BLOCK + 1];

  uint8                       config_bitmap;
  transfer_allocation_T       current_alloc_type;
  transfer_allocation_T       pending_alloc_type;

  uint16                      tbf_est_type;

  uint32                      starting_time;
  boolean                     starting_time_elapsed;
  boolean                     starting_time_valid;

  boolean                     rlc_svc_timer_active;
  boolean                     rlc_ul_data_available;

  boolean                     mac_ul_ctrl_available;
  mac_ul_ctrl_buffer_T        *mac_ul_ctrl_buff_ptr;

  rlc_ul_status_T             rlc_ul_status;
  rlc_mac_dl_buffer_T         rlc_mac_dl_buff;

  release_T                   release_pending;
  boolean                     pca_sent;
  boolean                     first_tx_block;
  downlink_pwr_t              dl_power_control;

  boolean                     l1_called_from_sngl_blk;
  enum
  {
    SENT,
    SEND_IN_ISR,
    SEND_AT_IDLE_INIT
  }confirm_pending;

  uint32                      abort_fn;
  uint32                      wait_for_pca_fn;
  boolean                     wait_for_pca_valid;
  /*  Simultaneous PUA/PDA starting time variables */
  uint32                      dl_st;
  uint32                      ul_st;

  boolean                     release_in_progress;
  boolean                     release_in_progress_delay;

  boolean                     abort_receives;
  boolean                     first_t3192_start;
  release_tbf_t               tbf_release_req_type;

  tbf_mode_T                  egprs_or_gprs_tbf;
  mac_rlc_mode_T              rlc_mode;
  l1_transfer_test_mode_t     l1_test_mode;
  sys_algo_agc_T              packet_transfer_agc;
  uint8                       mac_mode;
  uint8                       l1_test_mode_tn;
  gas_id_t                    transfer_gas_id;
} transfer_data_T;



/*******************************************************
 *
 *    LAYER 1 INTERNAL MESSAGES
 *
 *******************************************************/


/* Defines a structure for storing all relevent
 * information required for PBCCH/PCCCH internally
 * in Layer 1
 */
extern ARFCN_T                      gprs_arfcns[GL1_DEFS_FRAMES_IN_BLK_PERIOD];
extern ARFCN_T* l1_get_gprs_arfcns ( gas_id_t gas_id );
extern ARFCN_T  l1_get_gprs_arfcns_value ( uint8 index, gas_id_t gas_id );
extern boolean                      aquiring_psi;

extern boolean  l1_get_aquiring_psi( void );
extern boolean  l1_get_ext_page_blk_flag( void );
extern gpl1_gprs_tm_b_store_t gpl1_gprs_tm_b_store[NUM_GERAN_DATA_SPACES];
extern uint32 l1_get_gpl1_gprs_idle_Fn ( void );
extern uint8  l1_get_gprs_pccch_rx_in_progress ( void );
extern void   l1_set_aquiring_psi(boolean aquiring_psi);
extern void   l1_set_ext_page_blk_flag(boolean ext_page_blk_flag);
extern void   l1_set_gpl1_gprs_idle_Fn (uint32 Fn );
extern void   l1_set_gpl1_gprs_idle_calc_gap (boolean calc_gap );
extern void   l1_set_gpl1_gprs_idle_gprs_sleep_timer (boolean gprs_sleep_timer );
extern void   l1_set_gpl1_gprs_idle_read_extended_block (boolean read_extended_block );
extern void   l1_set_gprs_pbcch_rx_in_progress (uint8 gprs_pbcch_rx_in_progress );
extern void   l1_set_gprs_pccch_rx_in_progress (uint8 gprs_pccch_rx_in_progress );

#if defined ( FEATURE_GSM_DTM ) && defined ( FEATURE_GSM_EDTM )
/* This is used to store the enhanced allocation
 * so as its kept invisible from the rest of the L1
 * scheduling system.
 */
extern mac_l1_edtm_cs_rel_tbf_alloc_t  queued_mac_l1_edtm_cs_rel_tbf_alloc;
#endif

extern uint32 l1_get_schedule_Fn_buff ( uint8 index );
extern void l1_set_schedule_Fn_buff (uint8 index,uint32 value  );

extern void l1_xmsi_copy_params(xmsi_params_T *xmsi_ptr, gas_id_t gas_id);

#endif /* end of ifdef for GPL1_GPRS_IF_H */
