/*!
  @file
  lte_mac_rlc.h

  @brief
  This file contains all the common define structures/function prototypes
  interface between MAC and RLC.


*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/inc/lte_mac_rlc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/05/16   vt      CR 980235: Wrapper MACRO to get A2 meta info adaption
05/15/15   sb      CR 803880: MAC Changes for UDC feature
04/10/15   mg      CR812886: Updated DPD approximate loss and estimated loss
                   calculation - Calculate number of subframes from bitmask
10/30/14   el      CR740054: Decrease Bj with proper MAC sdu size
12/10/12   st      CR399603: Added a new API for handling VoLTE LCID
07/09/12   ar      CR 374751 fix: increased LTE_MAC_MAX_SDU_PER_LC from 50 
                   to 100 
10/25/11   st      Added a new function for PDCP to callback for Ping RTT
                   changes 
09/20/11   st      Added definitions required for eMBMS
05/23/11   ar      added field first_pkt_build_call_on_curr_harq to allotment
                   info structure
05/03/11   bn      Removed the feature T_MSM8960
04/14/11   bn      Added in field to keep track the number PDCP PDU built
12/10/10   bn      Modified packet build API to allow request to build 
                   control pdu only
07/13/10   ar      CR 245961 fix: increased LTE_MAC_MAX_SDU_PER_LC from 20 
                   to 50
07/08/10   ar      moved the LTE_MAC_SDU_MIN_SIZE macro to RLCUL since its
                   not used by MAC anymore
06/30/10   ar      updated the remaining_size comments
11/12/09   ar      changed LTE_MAC_SDU_MIN_SIZE from 5 bytes to 6 bytes
09/16/09   ar      Updated macro LTE_MAC_SDU_MIN_SIZE comments
08/25/09   ax      update MAC->RLC circular buffer size
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_RLC_H
#define LTE_MAC_RLC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <dsm_item.h>
#include <msgr.h>
#include <lte_as.h>
#include <lte_l1_types.h>
#include "mutils_circ_q.h"
#include <a2_dl_phy.h>
#include <dsm_queue.h>

/* Start of L2 internal including */


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief defined the maximum DL circular buffer size
     10ms*4PDU per TTI per bearer (MIMO)* total active bearers
*/
#define LTE_MAC_DL_CIRC_BUFF_MAX_SIZE     40 * LTE_MAX_ACTIVE_RB

/*! @brief The maximum DL circular buffer size for eMBMS data    
*/
#define LTE_MAC_DL_EMBMS_CIRC_BUFF_MAX_SIZE  LTE_MAC_DL_CIRC_BUFF_MAX_SIZE

/*! @brief defined the maximum MAC SDU to build per Logical channel
  RLC header size = 100 x 4 ~= 400bytes
  For 8 harqs = 400x8 = 3200bytes
  CRM RLC header buffer size = 512x8 = 4096bytes

  A2 UL PHY taskq memory / harq:
  100 * (4 (dma fill for pdcp hdr) + 2(cipher task) + 3 (dma task for sdu) + 
  3(dma task for each rlc pdu header)) + 20 words (misc tasks) ~= 1200 words
  CRM A2 UL PHY taskq size/harq = 1750words

  MAC UL data struct:
  worst case size = 100*4bytes for each RLC pdu = 400bytes
  MAC UL struct is based off this MACRO, so no further change is needed.
*/
#define LTE_MAC_MAX_SDU_PER_LC            100

/*! @brief MAC SDU size limit for selecting a 2byte vs 3byte mac sub header.
 *  SDU size < 128 = 2byte mac sub header
 *  SDU size >= 128 = 3byte mac sub header
*/
#define LTE_MAC_3BYTE_SUB_HDR_SDU_SIZE_LIMIT 128

/*! @brief Macro for MAC sub header size 1 byte */
#define LTE_MAC_1BYTE_SUB_HDR_SIZE       1

/*! @brief Macro for MAC sub header size 2 bytes */
#define LTE_MAC_2BYTE_SUB_HDR_SIZE       2

/*! @brief Macro for MAC sub header size 3 bytes */
#define LTE_MAC_3BYTE_SUB_HDR_SIZE       3

/*! @brief RLC-MAC DL circular buffer element structure
*/
typedef struct
{
  uint8           lcid;       /*!< logical channel ID */
  lte_sfn_s       sfn;        /*!< The system frame number */
  dsm_item_type   *head_ptr;  /*!< dsm pointer points to the 1st MAC SDU */
} lte_mac_buffer_element_s;

/*! @brief RLC-MAC DL circular buffer element structure for eMBMS
*/
typedef struct
{
  uint8           lcid;       /*!< logical channel ID */
  lte_sfn_s       sfn;        /*!< The system frame number */
  lte_earfcn_t     freq;       /*!< Frequency of the TB's cell */
  dsm_item_type   *head_ptr;  /*!< dsm pointer points to the 1st MAC SDU */
  lte_pmch_id_t   pmch_id;    /*!< PMCH ID of this eMBMS packet */
  lte_mbsfn_area_id_t area_id; /*!< MBSFN Area ID of this eMBMS packet */
} lte_mac_embms_buf_element_s;

/*! @brief RLC-MAC Logical channel Packet handler structure
*/
typedef struct
{
  uint8    lcid;            /*!< LC ID*/
  void     *handler;        /*!< packet handler belongs to this LC ID*/
} lte_mac_lc_packet_handler_s;

/*! @brief RLC-MAC HARQ failed packet handler structure
*/
typedef struct
{
  /*! Total number of LCs */
  uint8                           total_lc;
  /*! list of packet handlers*/
  lte_mac_lc_packet_handler_s     lc_handler[LTE_MAX_LC];
} lte_mac_harq_failed_handler_s;

/*! @brief RLC-MAC LC buffer information
*/
typedef struct
{
  uint32  total_size;     /*!< total size in bytes for the logical channel */
  uint32  control_size;   /*!< Size of control PDU */
  uint32  rtx_size;       /*!< Size of re-txed PDU */
  uint32  new_uncomp_data_size;  /*!< Size of new uncompressed PDU */
  uint32  new_comp_data_size;    /*!< Size of new compressed PDU */
} lte_mac_lc_buffer_info_s;

/*! @brief RLC-MAC Buffer Info Structure
*/
typedef struct
{
  uint32                    lc_bitmask; /*!< bitmasks for all active LCs*/
  lte_mac_lc_buffer_info_s  buffer[LTE_MAX_LC]; /*!< buffer info */
} lte_mac_buffer_info_s;

/*! @brief MAC to RLC Logical channel QoS suggested allotment
*/
typedef struct
{
  /*! Min allotment size can be used to build PDU*/
  uint16   min_allotment;
  /*! Max allotment size can be used to build PDU*/
  uint16   max_allotment;
  /*! extra allotment size. only use this if all the LCs can be empty */
  uint16   extra_allotment;
  /*! MAC will indicate to RLC when it can use the one byte MAC SDU header for the
  last SDU. When it is set to TRUE, RLC can use 1-byte-MAC-SDU header only if it
  can empty all the max allotment and last SDU*/
  boolean one_byte_hdr_for_last_pdu_allowed;
  /* if this is set to TRUE, Only control PDU should be built */
  boolean build_ctrl_pdu_only;
  /* total number of PDCP PDU built */
  uint16               num_pdcp_pdus_built;
  /* harq id for which packet build is called */
  uint8    harq_id;
  /* flag to indicate if mac is calling rlc for the first time for this harq,
  this info helps rlc manage the rlc header memory pointers  */
  boolean  first_pkt_build_call_on_curr_harq;
} lte_mac_allotment_info_s;

/*! @brief RLC to MAC actual MAC SDU info
*/
typedef struct
{
  uint16   sdu_size;        /*!< SDU size in byte*/
  uint8    sdu_header_size; /*!< SDU header size in byte */
} lte_mac_ul_sdu_info;


/*! @brief RLC to MAC actual Logical channel PDU build info
*/
typedef struct
{
  uint16               used_size;      /*!< actual used size */
  uint16               total_mac_sdu_size;  /*!< actual mac sdu size*/
  uint32               remaining_size; /*!< logical channel buffer status after
                                            pdu build */
  uint16               num_sdu_built;  /*!< total number of MAC SDU built */
  lte_mac_ul_sdu_info  sdu_info[LTE_MAC_MAX_SDU_PER_LC];/*!< size of each MAC SDU  */
} lte_mac_ul_data_info_s;

/*-----------------------------------------------------------------------*/
/*     LTE MAC/RLC External message definitions                          */
/*-----------------------------------------------------------------------*/

/*! @brief HARQ failed packet handler indication:
    LTE_MAC_HARQ_FAILED_PKT_HANDLER_IND
*/
typedef struct
{
  msgr_hdr_struct_type            hdr;         /*!< message header */
  /* Message payload */
  lte_mac_harq_failed_handler_s   handler_info; /*!< handler info*/
} lte_mac_harq_failed_pkt_handler_ind_msg_s;

/*! @brief VoLTE LCID enable/disable notification indication:
    LTE_MAC_VOLTE_LCID_IND
*/
typedef struct
{
  msgr_hdr_struct_type  hdr;        /*! < Message header */
  lte_lc_id_t           lcid;       /*! LCID of the VoLTE service */
  boolean               is_enabled; /*! Is VoLTE enabled or disabled */
} lte_mac_lcid_priority_ind_s;

/*===========================================================================

  FUNCTION:  lte_mac_dl_init_rlc_circ_queue

===========================================================================*/
/*!
    @brief
    This function initializes the RLC-MAC DL circular queue.

    @detail

    @return
    void

    @note

    @see related_function()
*/
/*=========================================================================*/
extern void lte_mac_dl_init_rlc_circ_queue
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_get_rlc_circ_queue

===========================================================================*/
/*!
    @brief
    This function returns a pointer to RLC-MAC DL circular queue.

    @detail

    @return
    mutils_circ_q_s*

    @note

    @see related_function()
*/
/*=========================================================================*/
extern mutils_circ_q_s* lte_mac_dl_get_rlc_circ_queue( void );

/*===========================================================================

  FUNCTION:  lte_mac_dl_init_rlc_embms_queue

===========================================================================*/
/*!
    @brief
    This function initializes the RLC-MAC DL circular queue.

    @detail

    @return
    void

    @note

    @see related_function()
*/
/*=========================================================================*/
extern void lte_mac_dl_init_rlc_embms_queue
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_get_rlc_embms_queue

===========================================================================*/
/*!
    @brief
    This function returns a pointer to RLC-MAC DL circular queue.

    @detail

    @return
    mutils_circ_q_s*

    @note

    @see related_function()
*/
/*=========================================================================*/
extern mutils_circ_q_s* lte_mac_dl_get_rlc_embms_queue
(
  void 
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_non_zero_buf_cb

===========================================================================*/
/*!
    @brief
    This is a call back function from RLC to notify MAC Control about a high
    data arrival.

    @detail
    This function triggers MAC UL task to schedule an SR request with ML1 via
    MAC Control task. This is intended to send high priority data on uplink as
    soon as RLC notifies MAC UL about it instead of waiting for the periodic
    buffer status checks that trigger an SR. 
 
    @return


    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_qos_non_zero_buf_cb
(
  dsm_watermark_type *wm_ptr,
  void *callback_data
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_get_last_embms_tb_size

===========================================================================*/
/*!
    @brief
    This function returns the size of the last eMBMS transport block size.

    @detail
    This value is used in DPD to calculate the approximate and estimated loss.

    @return
    uint32

    @note

    @see related_function()
*/
/*=========================================================================*/
uint32 lte_mac_dl_get_last_embms_tb_size
(
  void
);

#endif /* LTE_MAC_RLC_H */
