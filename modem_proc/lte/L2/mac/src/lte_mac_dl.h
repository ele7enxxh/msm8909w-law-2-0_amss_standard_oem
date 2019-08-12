/*!
  @file 
  lte_mac_dl.h

  @brief
  This file contains all the common define structures/function prototypes 
  for LTE MAC Control Task.


*/

/*===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/src/lte_mac_dl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   el      CR911691: FR30143 stop TA timer when re-try preamble for contention
                   based HO rach to allow apply TA in msg2
04/10/15   mg      CR812886: Updated DPD approximate loss and estimated loss
                   calculation - Calculate number of subframes from bitmask
12/23/14   sb      CR772190: Extend A2 diag framework for L2 
10/13/14   mg      CR738157: QSH - Memory held in RLC UL buffers
08/03/14   mg      CR702710: QSH analysis and reset
03/28/14   mg      CR632129: Print the MAC MCE payload in the log. 
08/22/13   sb      CR488783: For eMBMS PDU if reserved bits are invalid, 
                   ignore only  corresponding part of SDU/MCE rather than whole PDU
06/04/13   st      CR 495466: Added MAC_STOP_REQ/START_REQ handling in DL
05/09/13   st      CR 474371: Don't assert when DLM msg send to RRC fails
04/08/13   md      Reduced stack size
12/03/12   st      CR 402042: RACH one off RA_ID optimization
09/21/12   st      CR 384924: Scell MCE support added
08/09/12   st      CR 385047: Added new fields to save TA value in MSG2
04/20/12   st      Removed an unnecessary macro
04/17/12   ru      CR 342859: Added eMBMS logging changes  
04/06/12   st      Refactored the code to clean up some eMBMS hack
04/02/12   st      Changed a logging related eMBMS macro to fix a KW warning
02/08/12   st      Added the send_msi hack back in
01/04/12   st      Removed the send_msi hack
12/07/11   st      Added eMBMS specific statistics 
12/06/11   st      Made send_msi hack
11/16/11   st      Added support for the eMBMS EFS based NV item hack
10/27/11   st      CR #310206: Added a function for external access to the DL
                   internal top level structure for timing advance value
09/20/11   st      Added definitions for eMBMS
09/01/11   bn      CR 303654:Only process RAR when in RACH MSG2 state
06/10/11   bn      Renamed the reserved bit mask for more generic
03/01/11   bn      CR 277148: Always mask out the 2 MSB reserved bits in TA
02/28/11   bn      CR 277148: Check to insure TA reserved bits are zeros
09/20/10   wshaw   move wdog info to lte_common.h
09/17/10   wshaw   add watchdog support for LTE tasks
07/09/10   bn      Increase the MAX SDUs to 40 and optimize the discard logic
06/28/10   bn      Defined max payload to account for MCEs + max RLC LCIDs
06/10/10   bn      Included LTE_L2_DLSCH_LCID_DRX_CMD in the DL MCE check
04/05/10   ax      removed #define LTE_MAC_DL_MAILPOOL_SIZE
02/07/10   sm      Added data indicaiton optimization in L2 path
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
08/12/09   bn      Make sure Contention decision is finallized when send MSG4
                   to RRC
07/27/09   bn      Removed the dependencies on lte_a2_phy_dl_msg.h 
03/20/09   yg      Change in GRANT field in MAC RAR from 21 to 20 bits. 
01/23/08   ax      increase stack size from 8192 to 16384
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_DL_H
#define LTE_MAC_DL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <lte_l2_common.h>
#include <lte_l2_timer.h>
#include <dsm_item.h>
#include <mutils_circ_q.h>
#include <a2_dl_phy.h>
#include <lte_cphy_msg.h>
#include <lte_mac_rlc.h>
#include <timetick.h>
#include "lte_mac_log.h"
#include "lte_mac_int_msg.h"
#include "lte_mac_ind.h"
#include "lte_common.h"
#include "qsh.h"

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief RLC-MAC circular queue for DL MAC SDUs
*/
extern mutils_circ_q_s  lte_mac_dl_circ_queue;

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Union of all MAC DL external and internal messages */
typedef struct
{
  union
  {
    /*! ************* system general external messages *************/
    /*! Message header */
    msgr_hdr_struct_type                hdr;            

    /*! ***************** external Messages **********************/
    /*! MAC A2 DL Data Indication */
    lte_mac_a2_dl_phy_data_ind_s        a2_dl_data_ind;
    /*! RA timer expired Indication */
    lte_cphy_ra_timer_expired_ind_s     ra_tmr_expired_ind;
    /*! Start Dynamic Memory allocation Request*/
    lte_mac_start_req_msg_s         start_req_msg;
    /*! Stop Dynamic Memory allocation Request*/
    lte_mac_stop_req_msg_s          stop_req_msg;
    /*! ****************** internal Messages *********************/
    /*! DL Config Request */
    lte_mac_intmsg_dl_config_req_s      dl_config_req;
    /*! Monitor RAR Indication */
    lte_mac_intmsg_monitor_rar_ind_s    monitor_rar_ind;
    /*! Contention Result Indication */
    lte_mac_contention_result_ind_msg_s contention_result_ind;
    /*! send from MAC Control task to inform current action */
    lte_mac_intmsg_inform_action_ind_s  inform_action_ind;
    /*! eMBMS config request */
    lte_mac_intmsg_embms_config_req_s   embms_config_req;
    /*Internal message from lte_mac_ctrl_qsh_analysis_routine*/
    lte_mac_intmsg_qsh_analysis_req_s   qsh_analysis_reqi;
    /*L2 Diag command indication*/
    lte_mac_diag_data_ind_s             diag_data_ind;
  } msg;
} lte_mac_dl_msg_u;

/*! @brief Define Mailbox priority for DL Task
*/
#define LTE_MAC_DL_MB_PRIO 1

/*! @brief Define stack size for DL Task
*/
#define LTE_MAC_DL_TASK_STACK_SIZE 8192

/*! @brief 
    Maximum number of MAC sub-payloads that can be present in a DL MAC PDU 
    A MAC PDU consists of several MAC control elements, MAC SDUs and paddings.
    Each of these is referred here as a "sub-payload". For now, define the max
    number of payload can be processed is 40. Any more than that, will be 
    discarded. 
*/
#define LTE_MAC_DL_MAX_PDU_SUB_PAYLOADS  40

/*! @brief 
    Maximum value of RA ID in MAC RAR. (6-bits)
*/
#define LTE_MAC_DL_MAX_RA_ID_INDEX_VAL (0x3f)

/*! @brief 
    MAC PDU header reserved bits mask (2 MSBs)
*/
#define LTE_MAC_DL_MAC_PDU_RESERVED_BITS_BM (0xC0)

/*! @brief 
    MAC SCell act/deact config control element reserved bit mask
*/
#define LTE_MAC_DL_MAC_SCELL_MCE_RSRVD_BITMASK (0x1)

/*! @brief 
    Maximum value of OVERLOAD IND in MAC RAR.
*/
#define LTE_MAC_DL_MAX_OVERLOAD_INDEX_VAL (12)

/*! @brief 
    Maximum value of TA in MAC RAR. (11-bits)
*/
#define LTE_MAC_DL_MAX_TA_VAL (0x7ff)

/*! @brief 
    Maximum value of UL GRANT field in MAC RAR. (21-bits)
*/
#define LTE_MAC_DL_MAX_UL_FIELD_VAL (0x1fffff)

/*! @brief 
    Value of bitmask in order to send a unicast data ind to RLC
*/
#define LTE_MAC_DL_SEND_UNICAST_IND    1

/*! @brief 
    Value of bitmask in order to send a multicast data ind to RLC
*/
#define LTE_MAC_DL_SEND_MULTICAST_IND  2

/*! @brief 
    Value of bitmask in order to send a dual data ind to RLC
*/
#define LTE_MAC_DL_SEND_DUALDATA_IND   3

/*! @brief
    Get the 1-bit E field from first byte of R-R-E-LCID MAC sub-header 
*/
#define LTE_MAC_DL_GET_SUB_HEADER_E_FIELD(hdr) (((hdr) >> 5) & 0x1 )

/*! @brief
    Get the 5-bit LCID field from first byte of R-R-E-LCID MAC sub-header 
*/
#define LTE_MAC_DL_GET_SUB_HEADER_LCID_FIELD(hdr) ((hdr) & 0x1f )

/*! @brief
    Get the 1-bit F field from second byte of R-R-E-LCID MAC sub-header 
*/
#define LTE_MAC_DL_GET_SUB_HEADER_F_FIELD(hdr) (((hdr) >> 7) & 0x1 ) 

/*! @brief
    Get the 7-bit LENGTH field from second byte of R-R-E-LCID MAC sub-header 
*/
#define LTE_MAC_DL_GET_SUB_HEADER_LENGTH_FIELD(hdr) ((hdr) & 0x7f ) 

/*! @brief
    Get the 1-bit E field from E-T-RAID or E-T-R-R-OI MAC RAR sub-header 
*/
#define LTE_MAC_DL_RAR_GET_SUB_HEADER_E_FIELD(hdr) (((hdr) >> 7) & 0x1) 

/*! @brief
    Get the 1-bit T field from E-T-RAID or E-T-R-R-OI MAC RAR sub-header 
*/
#define LTE_MAC_DL_RAR_GET_SUB_HEADER_T_FIELD(hdr) (((hdr) >> 6) & 0x1) 

/*! @brief
    Get the 6-bit RAID field from E-T-RAID or E-T-R-R-OI MAC RAR sub-header 
*/
#define LTE_MAC_DL_RAR_GET_SUB_HEADER_RAID_FIELD(hdr) ((hdr) & 0x3f ) 

/*! @brief
    Get the 4-bit OI field from E-T-RAID or E-T-R-R-OI MAC RAR sub-header 
*/
#define LTE_MAC_DL_RAR_GET_SUB_HEADER_OI_FIELD(hdr) ((hdr) & 0xf ) 

/*! @brief
    Get the 7-bit TA field from 1st byte of  MAC RAR
*/
#define LTE_MAC_DL_RAR_GET_FIRST_BYTE_TA_FIELD(hdr) (hdr & 0x7f) 

/*! @brief
    Get the 4-bit TA field from 2nd byte of  MAC RAR
*/
#define LTE_MAC_DL_RAR_GET_SECOND_BYTE_TA_FIELD(hdr) (((hdr) >> 4) & 0xf) 

/*! @brief
    Get the 4-bit GRANT field from 2nd byte of  MAC RAR
*/
#define LTE_MAC_DL_RAR_GET_SECOND_BYTE_GRANT_FIELD(hdr) ((hdr) & 0xf) 

/*! @brief
    Determines whether the LCID is for a MAC control element
*/
#define LTE_MAC_DL_IS_A_CTRL_ELEM_LCID(lcid) ((lcid == LTE_L2_DLSCH_LCID_TA_CMD) || \
  (lcid == LTE_L2_DLSCH_LCID_UE_CONT_RES) || (lcid == LTE_L2_DLSCH_LCID_DRX_CMD) ) || \
  (lcid == LTE_L2_DLSCH_LCID_SCELL_ACT_DEACT)

/*! @brief
    Length of TA_CMD MAC control element in bytes
*/
#define LTE_MAC_DL_TA_CMD_LENGTH_IN_BYTES 1

/*! @brief
    Length of SCELL activation/deactivation configuration MAC control
    element in bytes
*/
#define LTE_MAC_DL_SCELL_CFG_LENGTH_IN_BYTES 1

/*! @brief
    Length of DRX_CMD MAC control element in bytes
*/
#define LTE_MAC_DL_DRX_CMD_LENGTH_IN_BYTES 0

/*! @brief
    Length of UE Contention Resolution MAC control element in bytes
*/
#define LTE_MAC_DL_UE_CONT_RES_LENGTH_IN_BYTES 6

/*! @brief
    Length of UE Contention Resolution MAC control element in bytes
*/
#define LTE_MAC_DL_RAR_LENGTH_IN_BYTES 6

/*! Number of functions to process sub headers - unicast and multicast  */
#define LTE_MAC_DL_NUM_PROC_FUNC_PTRS 2

/*! Macro to set the unicast bit (lsb) of the data indication bitmask (bm) */
#define LTE_MAC_DL_SET_UNICAST_IND_BIT(bm, is_q_empty) (bm = bm | is_q_empty)

/*! Macro to set the multicast bit of the data indication bitmask (bm) */
#define LTE_MAC_DL_SET_MULTICAST_IND_BIT(bm, is_q_empty) (bm = bm | (is_q_empty << 1))

/*! @brief MAC DL task states */
typedef enum
{
  LTE_MAC_DL_STATE_IDLE,                            /*<  MAC DL idle state. Not yet configured. */
  LTE_MAC_DL_STATE_ACTIVE                           /*<  MAC DL is configured by DL CONFIG req  */
} lte_mac_dl_state_e;

/*! @brief internal Confirmations that the LTE MAC module sends/receives  */
typedef enum
{
  LTE_MAC_DL_TIMER_TA,
  LTE_MAC_DL_TIMER_TB_LOG
} lte_mac_dl_timer_e;

/*! @brief Context structure for timer expiration callback
*/
typedef struct
{
  lte_mac_dl_timer_e        type;                  /*!< Timer type */
} lte_mac_dl_timer_ctx_s;

/*! @brief Enum for MAC "sub-payload" type of MAC PDU
    A MAC PDU consists of several MAC control elements, MAC SDUs and a padding.
    Each of these is referred here as a "sub-payload"
    This is used to represent all MAC PDUs except Transparent PDU and Random Access
    Response PDU
*/
typedef enum 
{
  LTE_MAC_DL_SUBPAYLOAD_RRC_PDU,
  LTE_MAC_DL_SUBPAYLOAD_RLC_PDU,
  LTE_MAC_DL_SUBPAYLOAD_TA_CMD,
  LTE_MAC_DL_SUBPAYLOAD_UE_CONTENTION_REZ_CMD,
  LTE_MAC_DL_SUBPAYLOAD_DRX_CMD,
  LTE_MAC_DL_SUBPAYLOAD_PADDING,
  LTE_MAC_DL_SUBPAYLOAD_EMBMS_PDU,
  LTE_MAC_DL_SUBPAYLOAD_MSI_PDU,
  LTE_MAC_DL_SUBPAYLOAD_SCELL_CFG,
  LTE_MAC_DL_SUBPAYLOAD_EMBMS_PDU_R_BIT_SET
} lte_mac_dl_pdu_sub_payload_type_e;

/*! @brief Structure for MAC Timing Advance Command */
typedef struct
{
  uint8 ta;
} lte_mac_dl_ta_cmd;

/*! @brief Structure for MAC Contention Resolution Command */
typedef struct
{
  uint8 ue_id[LTE_MAC_DL_UE_CONT_RES_LENGTH_IN_BYTES];
} lte_mac_contention_rez_cmd;

/*! @brief Structure for MAC "sub-payload" of MAC PDU
    A MAC PDU consists of several MAC control elements, MAC SDUs and a padding.
    Each of these is referred here as a "sub-payload"
    This is used to represent all MAC PDUs except Transparent PDU and Random Access
    Response PDU
*/
typedef struct
{
  union {
    dsm_item_type* rrc_pdu;                         /*!< MAC SDU destined to RRC */
    dsm_item_type* rlc_pdu;                         /*!< MAC SDU destined to RLC */
    dsm_item_type* msi_pdu;                         /*!< MAC SDU destined to MAC embms module */
    uint8          scell_cfg;                       /*!< SCell act/de-act cfg control elem */
    lte_mac_dl_ta_cmd ta_cmd;                       /*!< Timing Advance Command Control Elem*/
    lte_mac_contention_rez_cmd cont_rez_cmd;        /*!< Contention Resolution Command Control Elem*/
    dsm_item_type* padding;                         /*!< Padding */
  };
  uint32                    lc_id;                  /*!< LCID for DL-SCH */
  uint16                    length;                 /*!< Length of MAC SDU, Control Elem or padding */
  uint8                     reserved_bits;          /*!< reserved bits */     
  boolean                   good;                   /*!< Whether the subpayload and its header was extracted
                                                         successfully from MAC PDU */
  lte_mac_dl_pdu_sub_payload_type_e type;           /*!< Type of MAC sub-payload */

} lte_mac_dl_pdu_sub_payload_s;

/*! @brief Structure for MAC PDU
    This is used to represent all MAC PDUs except Transparent PDU and Random Access
*/
typedef struct
{
  /*! Number of MAC SDU, Control Elem or padding*/
  uint8                        num_sub_payloads;     
  /*! All MAC SDU, Control Elem or padding of the PDU*/
  lte_mac_dl_pdu_sub_payload_s sub_payloads[LTE_MAC_DL_MAX_PDU_SUB_PAYLOADS + 1]; 
  uint8      num_rlc_pdus;            /*!< Number of RLC PDUs in the MAC PDU */
  uint16      num_padding_bytes;       /*!< Number of Padding bytes in the MAC PDU */
  uint16      mac_hdr_len;      /*!< Length of MAC header*/
} lte_mac_dl_pdu_s;

/*! @brief Structure for MAC Random Access Response
    A Random Access Response MAC PDU consists of several Random Access Responses
*/
typedef struct
{
  uint8                     ra_id;                  /*!< RA ID */
  uint16                    ta;                     /*!< Timing Advance */
  uint32                    ul_grant;               /*!< Uplink grant */
  uint16                    t_rnti;                 /*!< T-RNTI */
  boolean                   good;                   /*!< Whether the MAC RAR and its header was extracted
                                                         successfully from MAC PDU */
} lte_mac_dl_rar_s;

/*! @brief Structure for MAC Random Access Response PDU 
*/
typedef struct
{
  boolean                      overload_ind_present;   /*! Whether the over_load field is valid */
  uint8                        overload_ind;           /*!< Overload indicator */
  lte_mac_dl_rar_s             rar;                    /*!< Random Access Respone*/
} lte_mac_dl_rar_pdu_s;

/*! @brief Internal structure for DL embms statistics Log sub packet */
typedef struct
{
  uint16     area_id;                          /*!< Area Id, size is uint16 to insure
                                                    struct is on a 4 byte boundary*/
  uint16     pmch_id;                          /*!< Pmch Id*/
  uint32     num_embms_data_pdu;               /*!< Number of received data pdu*/
  uint32     num_embms_dropped_data_pdu;       /*!< Number of dropped data pdu*/
  uint32     num_embms_ctrl_pdu;               /*!< Number of received ctrl pdu*/
  uint32     num_total_tb_bytes;               /*!< Number of total tb bytes*/
  uint32     num_total_padding_bytes;          /*!< Number of total padding bytes*/
  uint32     num_total_msi_bytes;              /*!< Number of total msi bytes*/
  uint32     num_lcid_bytes[LTE_MAC_EMBMS_LOG_LC]; /*!< Number of bytes per lc_id*/
} lte_mac_dl_embms_sub_pkt_stats_s;

/*! @brief Structure for MAC data process statistics counters
*/
typedef struct
{
  uint32      total_num_dropped_pkts;           /*!< Total number of dropped packets*/
  lte_mac_dl_embms_sub_pkt_stats_s area_pmch_combo[LTE_MAC_LOG_AREA_PMCH_MAX];  
} lte_mac_dl_embms_stats_s;

/*! @brief Structure for MAC data process statictis counters
*/
typedef struct
{
  /*! counter to indicate the total number of bad PDU*/
  uint32  total_bad_pdu_cnt;
  /*! counter to indicate the total number of good PDU */
  uint32  total_good_pdu_cnt;
  /*! counter to indicate the total number of bad RAR*/
  uint32  total_bad_rar_cnt;
  /*! counter to indicate the total number of good RAR */
  uint32  total_good_rar_cnt;
  /*! counter to indicate the total number of unexpected RAR */
  uint32  total_unexpected_rar_cnt;
  /*! counter to indicate the number of invalid reserved bits */
  uint32  invalid_reserved_bits_cnt;
  /*! counter to indicate the number of BCCH_IND data drops */
  uint32  bcch_data_drop_cnt;
  /*! counter to indicate the number of PCCH_IND data drops */
  uint32  pcch_data_drop_cnt;
  /*! counter to indicate the number of PCCH_IND data drops */
  uint32  ccch_data_drop_cnt;
  /*! eMBMS specific statistics structure */
  lte_mac_dl_embms_stats_s embms;
  /*! DSDS Num of RLC PDU enqueue operations*/
  uint32  num_rlc_pdu_enq;
  /*! Number of timing advances received*/
  uint32  num_ta_rcvd;
  /*! Number of total valid payload bytes received on the DL*/
  uint64  total_bytes;
  /*! Number of padding bytes received on the DL*/
  uint64  padding_bytes;
} lte_mac_dl_data_stats_s;

/*! @brief Global CS structure for MAC DL task */
typedef struct
{
  boolean task_is_active; /*!< Whether the "main" function of the thread is running */
  msgr_client_t   msgr_client;  /*!< MSGR client */
  msgr_id_t       mb_id;        /*!< Mailbox Id */
  lte_wdog_info_s wdog_info;    /*!< wdog information */
} lte_mac_dl_task_s;

/*! @brief Global Timer structure for MAC DL task
*/
typedef struct
{
  lte_l2_timer_s ta_timer;                     /*!< Timing Advance Expiry timer*/
  int    ta_timer_exp_ctx;                     /*!< Timing Advance Expiry timer Context */
  lte_l2_timer_s dl_tb_log_timer;                     /*!< DL Transport Log timer*/
  int    dl_tb_log_timer_exp_ctx;                     /*!< DL Transport Log timer Context */
  lte_l2_timer_s wdog_timer;                     /*!< DL Transport Log timer*/
} lte_mac_dl_timer_s;

/*! @brief Global sub header processing function type */
typedef boolean (*lte_mac_dl_extract_mac_sub_header_fp)
  (lte_mac_dl_pdu_sub_payload_s* sub_payload, dsm_item_type** data,
   boolean* more_subheaders);

/*! @brief Global sub payload processing function type */
typedef boolean (*lte_mac_dl_extract_mac_sub_payload_fp)
  (lte_mac_dl_pdu_sub_payload_s* sub_payload, dsm_item_type* data,
   lte_mac_dl_pdu_s* mac_pdu);

/*! @brief Global MAC PDU processing function type */
typedef void (*lte_mac_dl_proc_mac_pdu_fp)
  (lte_mac_dl_pdu_s* mac_pdu, a2_dl_phy_tb_info_t* tb_meta_info);

/*! @brief Data structure to hold the ta value in order to be used later */
typedef struct
{
  uint16 ta_val;
  lte_l1_cell_systime_s ta_rcvd_timestamp;
} lte_mac_dl_ta_val_record_s;

/*! @brief Global Data structure for MAC DL task
*/
typedef struct
{
  lte_mac_dl_state_e        state;       /*!< MAC DL state */
  lte_mac_buffer_element_s  dl_rlc_q_buf[LTE_MAC_DL_CIRC_BUFF_MAX_SIZE]; /*!< Buffer for Circular Queue with DL RLC */
  lte_mac_embms_buf_element_s dl_rlc_embms_q_buf[LTE_MAC_DL_EMBMS_CIRC_BUFF_MAX_SIZE]; /*!< Buffer for embms Queue with DL RLC */
  mutils_circ_q_s           dl_rlc_circ_queue;         /*!< RLC-MAC circular queue for DL MAC SDUs */
  mutils_circ_q_s           dl_rlc_embms_circ_queue;   /*!< RLC-MAC circular queue for DL eMBMS MAC SDUs */
  boolean                   back_off_ind_valid;    /*!< Whether the field "back_off_ind" is valid */
  uint8                     back_off_ind;          /*!< back_off_ind value received in MAC RAR subheader */
  uint8                     current_ra_id;         /*!< RA_ID received in MONITOR_RAR_IND message */
  uint8                     current_ue_id[LTE_MAC_MAX_CONN_REQ_SDU_SIZE]; /*!< UE_ID received in MONITOR_RAR_IND message */
  uint16                    ta_timer_val;          /*!< Timing Advance timer expiry value */
  boolean                   cb_rach;               /*!< Contention Based RACH, won't terminate after MSG2 */
  boolean                   check_crnti;           /*!< check to make sure the C-RNTI is used to decode PDCCH */
  boolean                   match_contention_mce; /*!< match the contention resolution MCE */
  a2_dl_phy_transport_block_t tb_array[A2_DL_PHY_MAX_TR_BLK]; /*!< Temp location passed to A2 to get DL transport blocks */
  lte_mac_dl_pdu_s          mac_pdu[A2_DL_PHY_MAX_TR_BLK];    /*!< Temp location for processing of incoming MAC PDUs */
  lte_mac_dl_rar_pdu_s      mac_rar_pdu[A2_DL_PHY_MAX_TR_BLK];  /*!< Temp location for processing of incoming MAC RAR PDUs */
  uint8                     current_num_mac_pdu;   /*! Number of elements valid in mac_pdu array */
  uint8                     current_num_mac_rar_pdu;   /*! Number of elements valid in mac_rar_pdu array */
  boolean                   ta_sync;               /*!< TRUE if Uplink is time-synced */
  /*! MAC DL needs to store RRC SDU until it receives the
  final verdict of contention resolution indication from MAC UL,
  then depends on the contention resolution result. If it is passed,
  MAC DL will send the RRC SDU up to RRC. If not passed, it will
  discard the RRC SDU. Doing this will avoid timing problem and
  insure that RRC SDU will be only sent out when contention 
  resolution is resolved */
  boolean                   contention_resolved;   /*!< indicated if contention is resolved*/
  boolean                   got_rrc_sdu;  /*!< indicate that there is a CCCH SDU */
  lte_mac_rrc_dl_data_ind_s rrc_sdu_ind;  /*!< local copy of RRC SDU IND MSG*/
  /*! statistic counters */
  lte_mac_dl_data_stats_s   stats;
  /* Function array to hold unicast vs multicast function pointers to extract
   * sub header information from the TB
   */
  lte_mac_dl_extract_mac_sub_header_fp 
      hdr_func_arr[LTE_MAC_DL_NUM_PROC_FUNC_PTRS];
  /* Function array to hold unicast vs multicast function pointers to extract
   * sub payload information from the TB
   */
  lte_mac_dl_extract_mac_sub_payload_fp 
      payload_func_arr[LTE_MAC_DL_NUM_PROC_FUNC_PTRS];
  /* Function array to hold unicast vs multicast function pointers to process
   * the payloads from the TB
   */
  lte_mac_dl_proc_mac_pdu_fp 
      payload_proc_func_arr[LTE_MAC_DL_NUM_PROC_FUNC_PTRS];
  uint8                    embms_on;
  /* This is a bitmask to decide what data indication to send to RLC
   * 0x0 -> Do nothing
   * 0x1 -> Send LTE_MAC_RLC_DL_DATA_IND
   * 0x2 -> Send LTE_MAC_BCAST_DATA_IND
   * 0x3 Send LTE_MAC_DUAL_DATA_IND
   */
  uint8                    rlc_ind_bm;
  lte_mac_dl_ta_val_record_s ta_record;
  timetick_type  last_reset; /*!< Time at which QSH reset is called*/
  boolean                  msi_present_in_tb; /*Whether MSI is prsent in TB*/
  uint32                   temp_last_embms_tb_size; /*Size of the last eMBMS TB. */
  uint32                   last_embms_tb_size; /*Size of the last eMBMS TB which
  does not contain an MSI.Used in DPD to calculate loss*/
  lte_mac_access_reason_e  access_reason;  /*!< access_reason */
} lte_mac_dl_data_s;

/*! @brief Global structure for MAC DL task
*/
typedef struct
{
  lte_mac_dl_task_s   task;        /*!< all the task control related data */
  lte_mac_dl_timer_s  timer;       /*!< all the timer related data */
  lte_mac_dl_data_s   data;        /*!< all the local data structure */
  lte_mac_log_s*      log;         /*!< Pointer to global MAC Log structure */
  boolean test_mode;               /*!< Set to true only during unit testing */
  lte_l2_indication_info_s a2_data_ind_info; /*!< a2 -> mac dl data ind opt */
  lte_l2_indication_info_s *mac_data_ind_info_ptr; /*!< macdl -> rlcdl data 
                                                        ind opt */
} lte_mac_dl_s;

/*===========================================================================

  FUNCTION:  lte_mac_dl_task_is_active

===========================================================================*/
/*!
  @brief
  Can be used to query if the mac_dl task has been initialized

  @return
  TRUE if the task has been initialized.
*/
/*=========================================================================*/
boolean lte_mac_dl_task_is_active
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_is_ul_time_synced

===========================================================================*/
/*!
    @brief
    This function returns whether the uplink time is in sync.

    @detail

    @return
    boolean

    @note

    @see related_function()
*/
/*=========================================================================*/
extern boolean lte_mac_dl_is_ul_time_synced
( 
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_compare_mac_pdu

===========================================================================*/
/*!
  @brief
  Compares two MAC PDUs
  
  @return
  Returns TRUE if equal

*/
/*=========================================================================*/
boolean lte_mac_dl_compare_mac_pdu
(
  lte_mac_dl_pdu_s* mac_pdu_1,
  lte_mac_dl_pdu_s* mac_pdu_2 
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_compare_mac_rar_pdu

===========================================================================*/
/*!
  @brief
  Compares two MAC RAR PDUs
  
  @return
  Returns TRUE if equal

*/
/*=========================================================================*/
boolean lte_mac_dl_compare_mac_rar_pdu
( 
  lte_mac_dl_rar_pdu_s* mac_rar_pdu_1,
  lte_mac_dl_rar_pdu_s* mac_rar_pdu_2 
);


/*===========================================================================

  FUNCTION:  lte_mac_dl_data_get_access_ptr

===========================================================================*/
/*!
    @brief
    This function return a pointer to the data structure of lte_mac_dl_data
    module.

    @detail

    @return
    None.

    @see related_function()

*/
/*=========================================================================*/
extern lte_mac_dl_data_s* lte_mac_dl_data_get_access_ptr
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_check_area_pmch_combo

===========================================================================*/
/*!
  @brief
  Checks if the area + pmch combo exists in the lte_mac_embms_log_stats_s
  structure.

  @return
  Returns the array index at which the combo exists and if it does not exist
  returns LTE_MAC_LOG_AREA_PMCH_MAX
*/
/*=========================================================================*/

extern uint8 lte_mac_dl_check_area_pmch_combo
(
  a2_dl_phy_tb_info_t* tb
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_get_num_rlc_pdu_enq

===========================================================================*/
/*!
  @brief
  accessor method for the current value of rlc pdu enqueue operation

  @return
  uin32 value of rlc pdu enqueue operations
*/
/*=========================================================================*/

uint32 lte_mac_dl_get_num_rlc_pdu_enq
(
  void
);

/*==============================================================================

  FUNCTION:  lte_mac_dl_qsh_analysis_routine

==============================================================================*/
/*!
    @brief
    This API runs the MAC DL QSH analysis routine

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC DL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_dl_qsh_analysis_routine 
(
  qsh_action_e action,
  uint32 category_mask
);
#endif /* LTE_MAC_DL_H */
