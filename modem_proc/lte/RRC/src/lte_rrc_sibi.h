/*!
  @file lte_rrc_sibi.h

  @brief
  SIB module's internal header file.
*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_sibi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/07/11   mm      Changes for supporting CMAS
09/03/10   amit    Using cphy macro LTE_CPHY_MAX_SI_MSGS for LTE_RRC_SIB_MAX_SIS
06/23/10   sureshs Added b12_to_b17 to static data 
04/26/10   amit    Added etws_supported to static data 
04/20/10   amit    Added periodic sib8 timer   
04/16/10   amit    Moved serv_cell_index from static data to dynamic data
04/05/10   amit    Updated SIB bitmask macros and added tdd_cfg_param_s to event
03/24/10   da      Added code to read SIB1 from EFS if file exists
02/26/10   da      Increased LTE_RRC_SIB_MIB_SIB1_WT_TMR to 3000
02/22/10   amit    Added fields for SIB11 to event
01/14/10   amit    Removed sib8_ts from event
12/04/09   amit    Added sib8_ts to event
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
11/17/09   amit    Moved LTE_RRC_MAX_SI_LENGTH to lte_rrc_int_msg.h
11/03/09   amit    Changed update_sent flags to a sib_update_needed_bitmask
10/14/09   amit    Updated SIB wait timer from a single 30s timer to separate 
                   timers for MIB/SIB1, SIB2 and the rest of the SIBs
08/28/09   amit    Updated code to remove lint warnings
08/27/09   amit    Added support for long SIBs
08/04/09   amit    Added boolean curr_serv_cell to event structure
06/25/09   amit    Added encoded SIB1 to events and cells in db
06/25/09   amit    Added macros for SFN rollover/mod prd ambiguity fix
06/12/09   amit    Replaced LTE_RRC_SIB_WT_TMR_MAND_SIBS & 
                   LTE_RRC_SIB_WT_TMR_NON_MAND_SIBS with one LTE_RRC_SIB_WT_TMR
06/05/09   amit    Added macro for min mod prd window length
05/21/09   amit    Code clean up
04/08/09   amit    Incorporated SIB code review comments
03/08/09   amit    Modified the structures for the new SIB design
11/13/08   amit    Changed sib wait timers to 2 sec and 10 sec
10/15/08   amit    Added wt_tmr to event_s
10/08/08   amit    Renamed some variables, changed structures for storing SIBs
09/24/08   amit    Removed SIB bitmasks defined in lte_rrc_int_msg.h, made
                   changes to structs since SIBs not dual encoded
08/06/08   amit    Initial Version
===========================================================================*/

#ifndef LTE_RRC_SIBI_H
#define LTE_RRC_SIBI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_variation.h"
#include <comdef.h>

#include "lte_rrc_int_msg.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_sib.h"
#include "lte_rrc_pendq.h"
#include "lte_rrc_controller.h"
#include <lte_cphy_msg.h>
#include <lte_l1_types.h>

#if !defined(TEST_FRAMEWORK)
  #include <time_genoff.h>
#endif

/*! @brief Bitmask for Mandatory SIBs (MIB, SIB1, SIB2) (mandatory for camping 
on a cell)
*/
#ifdef FEATURE_LTE_RRC_DISABLE_MIB_READING
#define LTE_RRC_SIB_MANDATORY_SIBS_BITMASK  (LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK)
#else
#define LTE_RRC_SIB_MANDATORY_SIBS_BITMASK  (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | LTE_RRC_SIB2_BITMASK)
#endif

#define LTE_RRC_SIB_MANDATORY_SIBS_BITMASK_SCC (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK | \
                                                LTE_RRC_SIB2_BITMASK | LTE_RRC_SIB13_BITMASK)

/*! @brief Bitmask for Non-mandatory SIBs (SIBs other than MIB, SIB1, SIB2 - not 
mandatory for camping on a cell)
*/
#define LTE_RRC_SIB_NON_MANDATORY_SIBS_BITMASK  \
  (LTE_RRC_SIB3_BITMASK     |               \
   LTE_RRC_SIB4_BITMASK     |               \
   LTE_RRC_SIB5_BITMASK     |               \
   LTE_RRC_SIB6_BITMASK     |               \
   LTE_RRC_SIB7_BITMASK     |               \
   LTE_RRC_SIB8_BITMASK     |               \
   LTE_RRC_SIB9_BITMASK     |               \
   LTE_RRC_SIB10_BITMASK    |               \
   LTE_RRC_SIB11_BITMASK    |               \
   LTE_RRC_SIB12_BITMASK    |               \
   LTE_RRC_SIB13_BITMASK    |               \
   LTE_RRC_SIB14_BITMASK    |               \
   LTE_RRC_SIB15_BITMASK    |               \
   LTE_RRC_SIB16_BITMASK)

/*! @brief Bitmask for All SIBs
*/
#define LTE_RRC_SIB_ALL_SIBS_BITMASK              \
  (LTE_RRC_SIB_MANDATORY_SIBS_BITMASK      |      \
   LTE_RRC_SIB_NON_MANDATORY_SIBS_BITMASK)

/*! @brief Bitmask for MIB & SIB1
*/
#ifdef FEATURE_LTE_RRC_DISABLE_MIB_READING
#define LTE_RRC_SIB_MIB_SIB1_BITMASK  (LTE_RRC_SIB1_BITMASK)
#else
#define LTE_RRC_SIB_MIB_SIB1_BITMASK  (LTE_RRC_MIB_BITMASK | LTE_RRC_SIB1_BITMASK)
#endif

/*! @brief Bitmask for SIB3 & SIB4
*/
#define LTE_RRC_SIB_SIB3_SIB4_BITMASK  (LTE_RRC_SIB3_BITMASK | \
                                        LTE_RRC_SIB4_BITMASK)

/*! @brief Bitmask for ETWS SIBs
*/
#define LTE_RRC_SIB_ETWS_SIBS_BITMASK  (LTE_RRC_SIB10_BITMASK | LTE_RRC_SIB11_BITMASK)

/*! @brief Bitmask for SIBs for which sib_update_indi is never sent
*/
#define LTE_RRC_SIB_UPDATE_NOT_NEEDED_BITMASK  (LTE_RRC_SIB_ETWS_SIBS_BITMASK | LTE_RRC_SIB12_BITMASK)

/*! @brief Bitmask for SIBs for which sib_update_indi is sent
*/
#define LTE_RRC_SIB_UPDATE_NEEDED_BITMASK  (LTE_RRC_SIB_ALL_SIBS_BITMASK &  \
                                            ~LTE_RRC_SIB_UPDATE_NOT_NEEDED_BITMASK)

/*! @brief Bitmask for EMBMS SIBs
*/
#define LTE_RRC_SIB_EMBMS_SIBS_BITMASK  (LTE_RRC_SIB13_BITMASK | LTE_RRC_SIB15_BITMASK)

/*! @brief Cell info expires in this many timer ticks (3hr timer, each tick is 10min)
*/
#define LTE_RRC_SIB_CELL_INFO_AGE 18

/*! @brief 10min (600000ms) tick used for 3hr timer
*/
#define LTE_RRC_SIB_3HR_TMR_TICK_VALUE 600000

/*! @brief Number of MIB & SIBs
*/
#define LTE_RRC_SIB_NUM_OF_MIB_N_SIBS (LTE_RRC_SIB_NUM_OF_SIBS + 1)

/*! @brief Maximum number of SIB events
*/
#define LTE_RRC_SIB_MAX_EVENTS 2

/*! @brief Invalid value for mod_period_window_length
*/
#define LTE_RRC_SIB_INVALID_WINDOW_LENGTH 0

/*! @brief Maximum number of SIs (Maximum supported SIB - 1)
e.g.: SIB16 is the max, then 15. Since SIB1 doesnt come a SI
*/
#define LTE_RRC_SIB_MAX_SIS 15

/*! @brief Wait timer for MIB & SIB1
*/
#define LTE_RRC_SIB_MIB_SIB1_WT_TMR ((lte_rrc_ctlr_get_single_rx_mode())? 640 : 320)

/*! @brief Wait timer for SIB2 is x times periodicity of SIB2. This macro is x
*/
#define LTE_RRC_SIB_SIB2_WT_TMR_FACTOR ((lte_rrc_ctlr_get_single_rx_mode())? 8 : 4)

/*! @brief Wait timer for SI is x times max periodicity among SIs. This macro is x
*/
#define LTE_RRC_SIB_SI_WT_TMR_FACTOR ((lte_rrc_ctlr_get_single_rx_mode())? 8 : 4)

/*! @brief Wait timer for non-mandatory SIBs (20 seconds)
*/
#define LTE_RRC_SIB_NON_MAND_SIBS_WT_TMR 20000

/*! @brief Periodic sib8 timer (4 minutes)
*/
#define LTE_RRC_SIB_PERIODIC_SIB8_TMR_DURATION 240000

/*! @brief Periodic sib16 timer (20 minutes)
*/
#define LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION 1200000

/*! @brief Maximum value of SFN (rollover at this value)
*/
#define LTE_RRC_SIB_MAX_SFN 1024

/*! @brief Invalid time value (used for last_sfn_rem_time); Also used for invalid 
SIB8 timestamp
*/
#define LTE_RRC_SIB_INVALID_TIME ((uint32)(-1))

/*! @brief Time in ms it takes for a complete SFN cycle
*/
#define LTE_RRC_SIB_SFN_ROLLOVER_TIME_MS (LTE_RRC_SIB_MAX_SFN * 10)

/*! @brief Buffer time in ms
*/
#define LTE_RRC_SIB_BUFFER_TIME_MS 100

/*! @brief Maximum SFN difference in out of order SFNs
*/
#define LTE_RRC_SIB_BUFFER_TIME_SFN 10

/*! @brief Minimum modification period window length - product of minimum mod 
prd coeff (2) & min paging cycle (32)
*/
#define LTE_RRC_SIB_MIN_MOD_PRD_WIN_LEN 64

/*! @brief Maximum number of SIB11 segments
*/
#define LTE_RRC_SIB_MAX_SIB11_SEGMENTS 64

/*! @brief Maximum number of SIB12 segments
*/
#define LTE_RRC_SIB_MAX_SIB12_SEGMENTS 64

/*! @brief Threshold for minimum number of paging cyles available in the
  current modification period. 
*/
#define LTE_RRC_SIB_MIN_THRESHOLD_PAGING_CYCLES 2

/*! @brief Maximum length of the warning message (obtained by concatenating   
  warning message segments received in SIB11). This is the same as the max length 
  of CB data (23.041, section 9.4.2.2.5)
*/
#define LTE_RRC_SIB_MAX_WARN_MSG_LENGTH 1246

/*! @brief Minimum value of message ID for ETWS/CMAS. Used for the start of range for   
  ETWS/CMAS message IDs
*/
#define LTE_RRC_SIB_DEFAULT_MSG_ID_MIN 0x1100

/*! @brief Maximum value of message ID for ETWS/CMAS. Used for the end of range for   
  ETWS/CMAS message IDs
*/
#define LTE_RRC_SIB_DEFAULT_MSG_ID_MAX 0x1107

/*! @brief Maximum mandatory sibs (MIB/SIB1/SIB2) timer value in ms   
  Only use when set mandatory timer value from EFS 
*/
#define LTE_RRC_SIB_MAX_MAND_SIBS_TMR 10240

/*! @brief Additional time buffer for ETWS timer
*/
#define LTE_RRC_SIB_ETWS_EXTRA_TMR_FACTOR 16

/*! @brief Additional time buffer for Resel Ind Time Reference
*/
#define LTE_RRC_SIB_RESEL_EXTRA_TIME_MS 5

/*! @brief Additional timer value for RF unavailibility during Resel
*/
#define LTE_RRC_SIB_RESEL_TCELL_UNAVAIL_TIME 80


/*! @brief Numbered representation of SIBs
*/
#define LTE_RRC_SIB_SIB1 1
#define LTE_RRC_SIB_SIB2 2
#define LTE_RRC_SIB_SIB3 3
#define LTE_RRC_SIB_SIB4 4
#define LTE_RRC_SIB_SIB5 5
#define LTE_RRC_SIB_SIB6 6
#define LTE_RRC_SIB_SIB7 7
#define LTE_RRC_SIB_SIB8 8
#define LTE_RRC_SIB_SIB9 9
#define LTE_RRC_SIB_SIB10 10
#define LTE_RRC_SIB_SIB11 11
#define LTE_RRC_SIB_SIB12 12
#define LTE_RRC_SIB_SIB13 13
#define LTE_RRC_SIB_SIB14 14
#define LTE_RRC_SIB_SIB15 15
#define LTE_RRC_SIB_SIB16 16

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Enum to indicate which mod prd window is being referred to
*/
typedef enum
{
  LTE_RRC_SIB_MOD_PRD_WIN_OLD, /*!< Old mod prd window */
  LTE_RRC_SIB_MOD_PRD_WIN_CURR, /*!< Current mod prd window */
  LTE_RRC_SIB_MOD_PRD_WIN_NEW, /*!< New mod prd window */
  LTE_RRC_SIB_MOD_PRD_WIN_INVALID /*!< Invalid */
} lte_rrc_sib_mod_prd_win_e;

/*! @brief Enum to indicate whether SIB-16 Indication is sent to NAS
*/
typedef enum
{
  LTE_RRC_SIB16_INITIAL,                 /*!< SIB16 is NOT available in SIB1 and Indication is NOT sent */
  LTE_RRC_SIB16_NOT_AVAIL_INDI_SENT,    /*!< SIB16 is NOT available in SIB1 and Indication is already sent */
  LTE_RRC_SIB16_AVAIL_INDI_NOT_SENT,    /*!< SIB16 is available in SIB1 and Indication is NOT sent */
  LTE_RRC_SIB16_AVAIL_INDI_SENT        /*!< SIB16 is available in SIB1 and Indication is already sent */
} lte_rrc_sib_sib16_ind_sent_e;

/*! @brief This structure holds cell identity
*/
typedef struct
{
  /*!< PLMN of the cell */
  lte_rrc_plmn_s plmn;
  /*!< Cell identity (received in SIB1) */
  lte_rrc_global_cell_id_t global_cell_id;
  /*!< Holds phy_cell_id and freq of the cell*/
  lte_rrc_cell_id_s cell_id;
} lte_rrc_sib_cell_id_s;

/*! @brief This structure stores an encoded SI
*/
typedef struct
{
  /*!< Size of encoded SI in bytes */
  uint32 encoded_si_size;
  /*!< Encoded SI */
  uint8 encoded_si[LTE_RRC_MAX_SI_LENGTH];
  /*!< Encoded SI pointer - used when encoded msg is longer than 
       LTE_RRC_MAX_SI_LENGTH */
  uint8 *encoded_si_ptr;
} lte_rrc_sib_encoded_si_info_s;

/*! @brief This structure holds all encoded & decoded SIs of a cell
*/
typedef struct
{
  /*!< Bitmask indicating what SIBs are present */
  lte_rrc_sib_bitmask_t sibs_bitmask;
  /*!< SIBs contained in SI */
  lte_rrc_sib_bitmask_t sibs_in_si_bitmask[LTE_RRC_SIB_MAX_SIS];
  /*!< Array of encoded SIs */
  lte_rrc_sib_encoded_si_info_s encoded_si_info[LTE_RRC_SIB_MAX_SIS];
  /*!< Indicates whether decoded SIs are present or not. */
  boolean si_decoded;
  /*!< Array of decoded SIs */
  lte_rrc_osys_BCCH_DL_SCH_Message *decoded_si_ptr[LTE_RRC_SIB_MAX_SIS];
} lte_rrc_sib_all_si_s;

/*! @brief This structure stores pointers to decoded SIBs
*/
typedef struct
{
  /*!< Pointer to SIB2 */
  lte_rrc_osys_SystemInformationBlockType2 *sib2_ptr;
  /*!< Pointer to SIB3 */
  lte_rrc_osys_SystemInformationBlockType3 *sib3_ptr;
  /*!< Pointer to SIB4 */
  lte_rrc_osys_SystemInformationBlockType4 *sib4_ptr;
  /*!< Pointer to SIB5 */
  lte_rrc_osys_SystemInformationBlockType5 *sib5_ptr;
  /*!< Pointer to SIB6 */
  lte_rrc_osys_SystemInformationBlockType6 *sib6_ptr;
  /*!< Pointer to SIB7 */
  lte_rrc_osys_SystemInformationBlockType7 *sib7_ptr;
  /*!< Pointer to SIB8 */
  lte_rrc_osys_SystemInformationBlockType8 *sib8_ptr;
  /*!< Pointer to SIB9 */
  lte_rrc_osys_SystemInformationBlockType9 *sib9_ptr;
  /*!< Pointer to SIB10 */
  lte_rrc_osys_SystemInformationBlockType10 *sib10_ptr;
  /*!< Pointer to SIB11 */
  lte_rrc_osys_SystemInformationBlockType11 *sib11_ptr;
  /*!< Pointer to SIB12 */
  lte_rrc_osys_SystemInformationBlockType12_r9 *sib12_ptr;
  /*!< Pointer to SIB13 */
  lte_rrc_osys_SystemInformationBlockType13_r9 *sib13_ptr;
  /*!< Pointer to SIB14 */
  lte_rrc_osys_SystemInformationBlockType14_r11 *sib14_ptr;
  /*!< Pointer to SIB15 */
  lte_rrc_osys_SystemInformationBlockType15_r11 *sib15_ptr;
  /*!< Pointer to SIB16 */
  lte_rrc_osys_SystemInformationBlockType16_r11 *sib16_ptr;

} lte_rrc_sib_all_decoded_sibs_s;

/*! @brief This structure holds a cell in the SIB db
*/
typedef struct
{
  /*!< Indicates if the info contained here is valid */
  boolean index_valid;
  /*!< To maintain age or rank of a cell in db; used for LRU algo used to remove 
       cells from db when new ones are added (0 indicates youngest or most 
       recently used cell) */
  uint8 index_rank;
  /*!< 10 min counter; used for 3hr timer */
  uint8 index_counter;
  /*!< Cell Identity */
  lte_rrc_sib_cell_id_s sib_cell_id;
  /*!< Value tag */
  uint32 value_tag;
  /*!< MIB */
  lte_l1_mib_info_param_s mib;
  /*!< Encoded SIB1 */
  lte_rrc_sib_encoded_si_info_s encoded_sib1;
  /*!< Decoded SIB1 */
  lte_rrc_osys_BCCH_DL_SCH_Message *sib1_ptr;
  /*!< Encoded & decoded SIs */
  lte_rrc_sib_all_si_s all_si;
  /*!< Decoded SIBs (SIB2 - SIB12) */
  lte_rrc_sib_all_decoded_sibs_s all_decoded_sibs;
  /*!< Index corresponding to band chosen in multiBandInfoList */
  uint8 mfbi_index;
} lte_rrc_sib_cell_s;

/*! @brief This structure holds the SIB db
*/
typedef struct
{
  /*!< Cells in the db */
  lte_rrc_sib_cell_s cell[LTE_RRC_SIB_DB_MAX_CELLS];
} lte_rrc_sib_db_s;

/*! @brief Indicates which timer is running for the event
*/
typedef enum
{
  LTE_RRC_SIB_EVENT_TMR_NONE,  /*!< 0 None */
  LTE_RRC_SIB_EVENT_TMR_MIB_SIB1,  /*!< 1 Timer for MIB & SIB1 */
  LTE_RRC_SIB_EVENT_TMR_SIB2,  /*!< 2 Timer for SIB2 */
  LTE_RRC_SIB_EVENT_TMR_SIB3_SIB4,  /*!< 3 Timer for SIB3 and SIB4 */
  LTE_RRC_SIB_EVENT_TMR_NON_MAND_SIBS,  /*!< 4 Timer for non-mandatory SIBs */
  LTE_RRC_SIB_EVENT_TMR_ETWS,  /*!< 5 Timer for ETWS */
  LTE_RRC_SIB_EVENT_TMR_MAX  /*!< 6 Invalid */
} lte_rrc_sib_event_tmr_e;

/*! @brief This structure represents a SIB11 segment
*/
typedef struct
{
  /*!< Pointer to the actual segment */
  unsigned char *seg_ptr;
  /*!< Length of the segment */
  uint32 seg_len;
} lte_rrc_sib_sib11_segment_s;

/*! @brief This structure represents a SIB11 segment
*/
typedef struct
{
  /*!< Pointer to the actual segment */
  unsigned char *seg_ptr;
  /*!< Length of the segment */
  uint32 seg_len;
} lte_rrc_sib_sib12_segment_s;


/*! @brief This structure represents the datastructure for
extended timer req
*/
typedef struct
{
  /* Reason for timer extension */
  lte_rrc_sib_extnd_tmr_reason_e reason;
  
  /*! Timestamp of reselection ind from CPHY */
  uint64 extnd_tmr_duration;
  
  /*! Timestamp of reselection ind from CPHY */
  uint64 extnd_tmr_reqi_timestamp;
  
} lte_rrc_sib_extnd_tmr_info_s;


/*! @brief This structure holds a SIB event
*/
typedef struct
{
  /*!< Event is active or not */
  boolean event_active;
  /*!< Cell identity (PCI & freq) */
  lte_rrc_cell_id_s cell_id;
  /*!< Requesting procedure */
  lte_rrc_proc_e req_proc;
  /*!< Cause received in get_sibs req */
  lte_rrc_get_sibs_cause_e cause;
  /*!< Index in SIB db; invalid if corresponding cell not present in db */
  uint8 sib_db_index;

  /**************************************************************************/

  /*!< MIB */
  lte_l1_mib_info_param_s mib;
  /*!< Encoded SIB1 */
  lte_rrc_sib_encoded_si_info_s encoded_sib1;
  /*!< Decoded SIB1 */
  lte_rrc_osys_BCCH_DL_SCH_Message *sib1_ptr;
  /*!< Encoded & decoded SIs */
  lte_rrc_sib_all_si_s all_si;
  /*!< SFNs for MIB and SIBs */
  lte_l1_sfn_t sib_sfn[LTE_RRC_SIB_NUM_OF_MIB_N_SIBS];
  /*!< Time stamp at last SFN */
  uint32 last_sfn_rem_time;

  /**************************************************************************/

  /*!< Bitmask specifying sibs required based on UE capability and controller 
       state */
  lte_rrc_sib_bitmask_t sibs_required_bitmask;
  /*!< Bitmask specifying sibs to be acquired by this event. Reasons for 
       difference from sibs_required_mask:
        - Not all SIBs may be scheduled by SIB1
        - Event may be for reading only MIB & SIB1 */
  lte_rrc_sib_bitmask_t sibs_to_acquire_bitmask;
  /*!< Bitmask specifying sibs received so far regardless of they are current or 
       not (compare with sibs_rcvd_curr). This indicates SIBs that are either 
       current, or those that we are not sure of. Those that are stale are stale 
       are removed from this bitmask */
  lte_rrc_sib_bitmask_t sibs_rcvd_bitmask;
  /*!< SIBs that have been received and are current (based on value tags and 
       modification period info) */
  lte_rrc_sib_bitmask_t sibs_rcvd_curr_bitmask;

  /**************************************************************************/

  /*!< get_sibs_cnfi needed or not */
  boolean cnf_needed;
  /*!< get_sibs_cnf sent or not */
  boolean get_sibs_cnf_sent;
  /*!< Boolean indicating whether the sib_db_key in this event is for the 
       current serving cell or not */
  boolean curr_serv_cell;
  /*!< Bitmask indicating what SIBs sib_updated_indi needs to be sent for */
  lte_rrc_sib_bitmask_t sib_update_needed_bitmask;

  /**************************************************************************/

  /*!< Modification period window length (in SFN) */
  uint16 mod_period_window_length;
  /*!< SFN where current modification window starts */
  lte_l1_sfn_t curr_window_start;
  /*!< SFN where current modification window ends */
  lte_l1_sfn_t curr_window_end;

  /**************************************************************************/

  /*!< SFN after which SIBs are to be read. This is the SFN of the start of the 
       next modification period in case of si_update and next_mod_prd_value_tag_check. 
       This field is NULL otherwise. */
  lte_l1_sfn_t next_mod_prd_sfn;
  /*!< CPHY sib sched req */
  lte_cphy_sib_sched_req_s sched_req;
  /*!< SIB wait timer */
  lte_rrc_tmr_s wt_tmr;
  /*!< Enum indicating which timer is currently running for the event */
  lte_rrc_sib_event_tmr_e running_tmr;

  /** Last SFN when reading of mandatory SIBs completed.
  *   Used for avoiding next value tag if sufficient paging cycles are
  *   available in current mod period.
  */
  lte_l1_sfn_t last_mandatory_sib_sfn;

  /** Last SFN when reading a SIB
  */
  lte_l1_sfn_t last_read_sib_sfn;

  /***************** SIB11 related fields ***********************************/

  /*!< Boolean indicating if SIB11 reading is active */
  boolean sib11_read_active;
  /*!< Msg ID of SIB11 currently being read */
  uint16 sib11_curr_msg_id;
  /*!< Serial number of SIB11 currently being read */
  uint16 sib11_curr_sr_num;
  /*!< Array of segments of SIB11 */
  lte_rrc_sib_sib11_segment_s sib11_segment[LTE_RRC_SIB_MAX_SIB11_SEGMENTS];
  /*!< Number of segments rcvd so far. We are done when num_segments_rcvd is 
       last_segment_index + 1 */
  uint8 sib11_num_segments_rcvd;
  /*!< Index of the last segment. Valid value is >= 0 and <= (LTE_RRC_SIB_MAX_SIB11_SEGMENTS - 1).
       We start with an invalid value which is LTE_RRC_SIB_MAX_SIB11_SEGMENTS */
  uint8 sib11_last_segment_index;
  /*!< Boolean to indicate if data coding scheme is present */
  boolean sib11_data_coding_scheme_present;
  /*!< Sib 11 data coding scheme */
  unsigned char sib11_data_coding_scheme; 
  /*!< Sib 11 timer duration, sib11 periodicity mutiply LTE_RRC_SIB_MAX_SIB11_SEGMENTS */
  uint32 sib11_timer_duration;

  /***************** SIB12 related fields ***********************************/

  /*!< Boolean indicating if SIB12 reading is active */
  boolean sib12_read_active;
  /*!< Msg ID of SIB12 currently being read */
  uint16 sib12_curr_msg_id;
  /*!< Serial number of SIB12 currently being read */
  uint16 sib12_curr_sr_num;
  /*!< Array of segments of SIB12 */
  lte_rrc_sib_sib12_segment_s sib12_segment[LTE_RRC_SIB_MAX_SIB12_SEGMENTS];
  /*!< Number of segments rcvd so far. We are done when num_segments_rcvd is 
       last_segment_index + 1 */
  uint8 sib12_num_segments_rcvd;
  /*!< Index of the last segment. Valid value is >= 0 and <= (LTE_RRC_SIB_MAX_SIB12_SEGMENTS - 1).
       We start with an invalid value which is LTE_RRC_SIB_MAX_SIB12_SEGMENTS */
  uint8 sib12_last_segment_index;
  /*!< Data coding scheme from SIB12, present only forfirst segment */
  unsigned char sib12_data_coding_scheme; 

  /**************************** TDD Config **********************************/

  /*!< Last TDD config parameters sent for this cell (since the event has been 
       active) */
  lte_cphy_tdd_cfg_param_s last_tdd_cfg_param;

  /**************************** Non-Mand SIBs *******************************/
  /*!< Sib 3 through 13 timer duration, max periodicity among those SIBs multiplied by 
       LTE_RRC_SIB_SI_WT_TMR_FACTOR*/
  uint32 sib_non_mand_timer_duration;

  /**************************** MFBI ****************************************/
  
  /*!< Index corresponding to band chosen in multiBandInfoList */
  uint8 mfbi_index;
  /*!< Is to store list of overlapping frequencies found in sib1*/
  lte_rrc_earfcn_list new_dl_freq_list;
 
  /*!< Last SIB sched req sent */
  lte_cphy_sib_sched_req_s last_sib_sched_req;

  /*!< SIB restart timer counter
       Counter to keep track how many times we restart the sib timer by adding of MIB/SIB1*/
  uint8 sib_tmr_restart_counter;

} lte_rrc_sib_event_s;

/*! @brief This structure holds the SIB Events db
*/
typedef struct
{
  /*!< Number of events currently active */
  uint8 num_active_events;
  /*!< Array of events */
  lte_rrc_sib_event_s event[LTE_RRC_SIB_MAX_EVENTS];
} lte_rrc_sib_events_db_s;

/*! @brief This structure holds the Static part of private data
*/
typedef struct
{
  /*!< SIB db */
  lte_rrc_sib_db_s sib_db;
  /*!< Index of current serving cell */
  uint8 serv_cell_index;
  /*!< Timer ticks for the 3hr timer */
  lte_rrc_tmr_s three_hr_tick_tmr;
  /*!< Boolean indicating whether to use canned sib1 */
  boolean use_canned_sib1;
  /*!< Canned SIB1 info */
  unsigned char canned_sib1[LTE_RRC_MAX_SI_LENGTH];
	/*!< Canned SIB1 length */
	int32 canned_sib1_size;
	/*!< Boolean indicating whether to use canned sib2 */
  boolean use_canned_sib2;
  /*!< Canned SIB2 info */
  unsigned char canned_sib2[LTE_RRC_MAX_SI_LENGTH];
	/*!< Canned SIB2 length */
	int32 canned_sib2_size;
	/*!< Boolean indicating whether to use canned sib13 */
  boolean use_canned_sib13;
	/*!< Canned SIB13 info */
  unsigned char canned_sib13[LTE_RRC_MAX_SI_LENGTH];
	/*!< Canned SIB13 length */
	int32 canned_sib13_size;
  /*!< Boolean indicating whether ETWS is supported */
  boolean etws_supported;
  /*!< Boolean indicating whether CMAS is supported */
  boolean cmas_supported;
  /*!< Boolean indicating whether B12 to B17 converstion is needed */
  boolean b12_to_b17_conversion_is_needed;
  /*!< Boolean indicating whether to use mandatory sib timer value 
       from EFS file */
  boolean use_efs_mand_sib_tmr;
  /*!< EFS mandatory timer value that higher 16-bits are for MIB/SIB1 
       timer value and lower 16-bits are for SIB2 timer value. (In ms)
       Max value for each is 10.24s (0x2800 ms) */
  uint32 efs_mand_sib_tmr;
  /*!< Boolean indicating whether to use non-mandatory sib timer value 
       from EFS file */
  boolean use_efs_non_mand_sib_tmr;
  /*!< EFS non-mandatory timer value 
       min is 320 ms and maximum is 20000 ms */
  uint32 efs_non_mand_sib_tmr;
  /*!< Last msg id list req sent */
  lte_rrc_msg_id_list_req_s last_msg_id_list_req;
  /*!< Boolean indicating whether to use sib16 timer value from EFS file */
  boolean use_efs_sib16_utc_tmr;
  /*!< EFS sib16 timer value 
       min is 320 ms and maximum is 1200000 ms */
  uint32 efs_sib16_utc_tmr;

  /*!< EFS value that says if we should throw RLF/SIB read error in connected mode for SIB8 */
  uint8 efs_skip_rlf_for_sib8;

  /*!< Boolean to indicate if SIB14 is mandatory or not*/
  boolean is_sib14_mandatory;
  /*!< Boolean to indicate if SIB14 is present in DB or not*/
  boolean is_sib14_available;
} lte_rrc_sib_static_data_s;

/*! @brief This structure holds the Dynamic part of private data
*/
typedef struct
{
  /*!< Events db */
  lte_rrc_sib_events_db_s events_db;

  /*!< Serving cell SIBs in the db are a consistent set or not */
  boolean serv_cell_sibs_consistent;
  /*!< Timer for periodic sib8 reading */
  lte_rrc_tmr_s periodic_sib8_tmr;
  /*!< Cell id for which sib8 timer is running (if it is running) */
  lte_rrc_cell_id_s periodic_sib8_cell_id;
  /*!< Boolean indicating whether SIB13 is scheduled in SIB1 */
  boolean sib13_is_sched;
  /*!< Pointer to store the pending queue item ptr for ETWS get_sibs_reqi */
  lte_rrc_pendq_queue_item_s *etws_sib_reqi_ptr;
  /*!< Pointer to store the pending queue item ptr for CMAS get_sibs_reqi */
  lte_rrc_pendq_queue_item_s *cmas_sib_reqi_ptr;
  /*!< Pointer to store the pending queue item ptr for EAB get_sibs_reqi */
  lte_rrc_pendq_queue_item_s *eab_sib_reqi_ptr;
  /*!< Timer for periodic sib16 reading */
  lte_rrc_tmr_s periodic_sib16_tmr;
  /*!< Cell id for which sib16 timer is running (if it is running) */
  lte_rrc_cell_id_s periodic_sib16_cell_id;
  /*!< for Periodic QMI - NAS Indication for UTC time update 
    to be sent once every LTE_RRC_SIB_PERIODIC_SIB16_TMR_DURATION*/
  lte_rrc_sib_sib16_ind_sent_e utc_time_indi_is_sent;
  /*!< Time stamp at which UNAVAIL_IND is rcvd */
  uint32 rf_unavail_ind_time;
  /*!< SFN at which UNAVAIL_IND is rcvd */
  lte_l1_sfn_t rf_unavail_sfn;
  /*!< PO monitoring info from RF_UNAVAIL_IND*/
  uint8 rf_unavail_num_po_mtred;
  /*!< Save remaining time for wt_tmr during ML1 suspend*/
  uint32 wt_tmr_val_on_rf_unavail;
  /*!< Current rf_status to avoid multiple listening to multiple indications*/
  lte_rrc_rf_status_e rf_status;
  
  /*! Boolean to indicate whether the stored reselec ref time info is valid */
  boolean resel_ref_time_valid;
  /*! Resel Ind SFN */
  lte_l1_sfn_t resel_ind_sfn;
  /*! Timestamp of reselection ind from CPHY */
  uint64 resel_ind_timestamp;

  /*! Boolean to indicate whether the stored reselec ref time info is valid */
  boolean extnd_tmr_valid;
  /*! CB holding the extended timer info */
  lte_rrc_sib_extnd_tmr_info_s extnd_tmr_info;
  /*! frequency of reselection*/
  lte_earfcn_t resel_freq;
  /* Resume cnf SFN*/
  lte_l1_sfn_t resume_cnf_sfn;
} lte_rrc_sib_dynamic_data_s;

/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{
  /*!< Static part of private data */
  lte_rrc_sib_static_data_s *sd_ptr;
  /*!< Dynamic part of private data */
  lte_rrc_sib_dynamic_data_s *dd_ptr;
} lte_rrc_sib_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


#endif /* LTE_RRC_SIBI_H */
