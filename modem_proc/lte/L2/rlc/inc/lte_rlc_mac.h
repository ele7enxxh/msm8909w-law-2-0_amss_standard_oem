/*!
  @file
  lte_rlc_mac.h

  @brief
  RLC header file related to MAC.

  @author
  araina

*/

/*==============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/inc/lte_rlc_mac.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/12/15   sb      CR776561: FR 25276: Fast LTE connection release
02/07/10   sm      Added data indicaiton optimization in L2 path
03/26/09   ar      added sfn parameter to lte_rlc_build_pdus() api
02/10/09   ar      Added comments to external function prototypes
07/31/08   ax      initial version
==============================================================================*/

#ifndef LTE_RLC_MAC_H
#define LTE_RLC_MAC_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte_as.h>
#include <lte_mac_rlc.h>
#include <lte_l2_common.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  lte_rlc_get_buffer_status

==============================================================================*/
/*!
    @brief
    This function returns the size of the buffered data(in bytes) in the RLC
    layer.

    @detail
    As part of the Buffer Status Reporting, MAC needs to provide the serving eNB
    with information about the amount of data in the UL buffers of the UE.
    lte_rlc_get_buffer_status() is called periodically (every 1ms) from one of
    the MAC Control task to query how much data (in bytes) is buffered in the
    RLC and PDCP layer that needs to be transmitted, for each active RB. The
    data that is accounted in the buffer status calculation includes:
    a. Estimated size of control PDU on the corresponding RLC DL entity for AM
    b. RLC re-transmission buffer data
    c. RLC watermark data that contains the RLC SDUs
    d. Size of fragmented RLC SDU, if any

    For every active RB denoted by lc_bitmask filed, RLC will update the buffer
    size value (in bytes) in lc_size array in the lte_mac_buffer_info_s
    structure, pointed to by the buf_info_ptr.

    @return
    Although the return type of this function is void, the buffer status is
    indicated to MAC by populating the structure pointed by buf_info_ptr.

*/
/*============================================================================*/
EXTERN void lte_rlc_get_buffer_status
(
  lte_mac_buffer_info_s* const buf_info_ptr  /*!< This struct ptr needs to be
                                          updated with the buffer status info */
);


/*==============================================================================

  FUNCTION:  lte_rlc_build_pdus

==============================================================================*/
/*!
    @brief
    A functional API called from MAC UL context for building RLC pdus

    @detail
    MAC UL task calls this RLC function for every LC. MAC UL assigns a minimum
    and a maximum allotment that could be used by RLC for building pdus for that
    LC. RLC uses the minimum allotment as much as it can and borrows from the
    maximum allotment only if it could avoid SDU segmentation. The information
    about the built MAC sdus is stored in the UL data structure pointed to by
    the ul_data_info_ptr.

    Input arguments:
    lc_id:              Logical channel ID for which lte_rlc_build_pdus()
                        is called
    allotment_info_ptr: Pointer to lte_mac_allotment_info_s. Size (in bytes)
                        allotted by MAC to build RLC packets on a particular LC.
                        The lte_mac_allotment_info_s struct has the following
                        members:
        min_allotment:  Size allocated for the LC by MAC. In principle, RLC
                        shall use up at least mim_allotment unless it runs out
                        of data to send. In realty, it is possible that RLC may
                        end up using only (min_allotment-x) where x is a small
                        configurable number.
        max_allotment:  To help combating fragmentation, this is the
                        max_allotment this LC can grab. This allows RLC the
                        potential to send a complete RLC SDU whereas
                        min_allotment may result in fragmenting the RLC SDU.
        extra_allotment:Extra allotment that can be used by RLC if and only if
                        using the extra allotment would help draining all RLC
                        PDUs from the logical channel. This helps in a very rare
                        scenario where by canceling the MAC control BSR, the
                        extra space could help emptying all data from the UE.
        one_byte_hdr_for_last_pdu_allowed: MAC will indicate to RLC when it can
                                           use the one byte MAC SDU header for
                                           the last SDU. When it is set to TRUE,
                                           RLC can use 1-byte-MAC-SDU header
                                           only if it can empty all the max
                                           allotment and last SDU

    ul_data_info_ptr:   Pointer to the lte_mac_ul_data_info_s which is updated
                        by RLC during lte_rlc_build_pdus(). The structure has
                        the following members:

        total_bytes_used:   The number of bytes consumed for building pdus over
                            the current logical channel,including the MAC sub
                            header size
        remaining_bytes:    The number of bytes remaining with the logical
                            channel. This can be used by MAC for BSR reporting.
        num_mac_sdus_built: Number of MAC sdus built by RLC for a particular LC
                            during lte_rlc_build_pdus()
        mac_sdu_size[]:     This array holds the individual size of all MAC SDUs
                            that are built during lte_rlc_build_mac_sdus() and
                            also the corresponding SDU header size in bytes

    @return
    void

    @retval value
    The return value of the API is void but the information is exported out by
    updating the structure pointed by ul_data_info_ptr.
    ul_data_info_ptr:   Pointer to the lte_mac_ul_data_info_s which is updated
                        by RLC during lte_rlc_build_pdus(). The structure has
                        the following members:

        total_bytes_used:   The number of bytes consumed for building pdus over
                            the current logical channel,including the MAC sub
                            header size
        remaining_bytes:    The number of bytes remaining with the logical
                            channel. This can be used by MAC for BSR reporting.
        num_mac_sdus_built: Number of MAC sdus built by RLC for a particular LC
                            during lte_rlc_build_mac_sdus()
        mac_sdu_size[]:     This array holds the individual size of all MAC SDUs
                            that are built during lte_rlc_build_pdus() and
                            also the corresponding SDU header size in bytes

*/
/*============================================================================*/
EXTERN void lte_rlc_build_pdus
(
  lte_lc_id_t                lc_id,              /*!< logical channel id */
  lte_mac_allotment_info_s*  allotment_info_ptr, /*!< ptr to mac
                                                      allotment info struct */
  lte_mac_ul_data_info_s*    data_info_ptr,      /*!< ptr to mac ul data
                                                      info struct */
  lte_sfn_s                  sfn                 /*!< sys/sub frame number */
);

/*===========================================================================

  FUNCTION:  lte_rlcdl_task_get_data_ind_info

===========================================================================*/
/*!
  @brief
  Get mac data ind info pointer
*/
/*=========================================================================*/
lte_l2_indication_info_s *lte_rlcdl_task_get_data_ind_info(void);

/*==============================================================================

  FUNCTION:  lte_rlcul_am_ack_tx_sts_req_handler

==============================================================================*/
/*!
    @brief
    This function handles RRC request to check if RRC conn rel ACK is sent. 

    @detail
    This handler is called by RLCUL on getting request from RRC to check
    if RLC ACK for connection release has been received by enB.

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_ack_tx_sts_req_handler
(
);

#endif /* LTE_RLC_MAC_H */
