/*!
  @file
  lte_pdcp_ext_api.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_pdcp_ext_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/26/15   sb      CR862079: Change to upgrade PDCP support to
                   ELS spec ver D and UDC ver F
05/13/15   sb      CR 803880: modified PDCP interface for UDC feature
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
11/18/13   sb      CR576794: New PDCP API prototype for DS to tag UL packet
10/02/13   mg      CR489345: Get num of PDCP PDU bytes transmitted for an EPSID
12/03/12   sb      CR427147: instead of sending RoHC feedback pkt as a separate message,
                         it will be copied to PDCP-OFFLOAD circular buffer just like data PDU
11/07/12   ax      CR383618: L2 modem heap use reduction phase I
10/22/12   sb      CR411260: Handle the case properly when UE moves from non-ciphering to 
                         ciphering enabled case during RoHC scenario
05/16/12   sb      CR361321: Added L2 indication optimization structure to reduce
                         frequency between PDCP to OFFLOAD notification.
01/04/10   ax      move what is needed by FTM under API dir
05/18/10   ax      consolidate pdcp external APIs and place them under lte/api/
==============================================================================*/

#ifndef LTE_PDCP_EXT_API_H
#define LTE_PDCP_EXT_API_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte.h>
#include <queue.h>
#include <dsm_item.h>
#include <dsm_queue.h>
#include <mutils_circ_q.h>
#include <lte_security.h>
#include <lte_pdcp_msg.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief PDCPDL->PDCP OFFLOAD circular queue size (number of elements)
     10ms*1 entry per TTI per bearer*total number of active bearers
*/
#define	LTE_MAX_ACTIVE_ROHC_RB 32

#define PDCP_DL_SEC_LTE_NUM_KEYS_MAX 16
#define PDCP_DL_SEC_KEY_SIZE 16

#define LTE_PDCP_CFG_DL_PDU_CIRC_Q_SIZE                    (LTE_MAX_ACTIVE_ROHC_RB)
/*macro to help OFFLOAD identify if the pkt is RoHC feedback or Data packet*/
#define LTE_PDCPDL_ROHC_FEEDBACK_PDU                     0xFF

/*! @brief Maximum UDC prefill context IDs */
#define LTE_PDCPUL_UDC_PREFILL_CNTXT_ID_MAX                8

/*! @brief DL data activity statistics
*/
typedef struct
{
  uint32  num_bytes;        /*!< data pdu bytes recieved*/
  uint32  num_pdus;         /*!< number of data PDUs received */
  uint32  num_pdus_dropped; /*!< number of data pdus dropped */
} lte_pdcpdl_activity_stats_s;

/*! @brief PDCP indication optimization related structure.
    This is to track that PDCP layers are not indicating OFFLOAD task more than 
    once if the signal/indication is not yet processed.
*/
typedef struct
{
  volatile boolean pending; /*!< indication is enqueued but not yet processed*/
  uint32  probable_enqueue_count; /*!< probable number of times indication
                                       command could have been enqueued */
  uint32  enqueue_count; /*<! absolute number of times indication command
                              enqueued for the receiver */
  uint32  dequeue_count; /*!< absolute number of times indication command
                              is received and processed by the receiver task */
} lte_l2_ind_info_s;

/*! @brief PDCP DL-PDCP OFFLOAD circular PDCP PDU buffer element structure
*/
typedef struct
{
  lte_rb_cfg_idx_t rb_cfg_idx;       /*!< unique RB configuration index */
  lte_eps_id_t eps_id;
  dsm_item_type*   head_ptr;         /*!< dsm pointer to the 1st PDCP PDU */
  dsm_item_type*   tail_ptr;         /*!< dsm pointer to the Last PDCP PDU */
} lte_pdcpdl_pdu_buffer_element_s;

/*! @brief PDCP DL-PDCP OFFLOAD circular PDCP PDU buffer element structure
*/
typedef struct
{
  lte_pdcpdl_pdu_buffer_element_s pdu_buf[LTE_PDCP_CFG_DL_PDU_CIRC_Q_SIZE];
  mutils_circ_q_s      pdu_circ_q_ptr;/*!< circ. q used to carry PDCP PDU to PDCP OFFLOAD*/  
  uint32               num_pending_pdus;
  lte_l2_ind_info_s   *pdcp_data_ind_info_ptr; /*!< pdcpdl -> pdcpoffload data */
  uint32               pdcp_offload_q_full_cnt;/*!<number of times the PDCP->OFFLOAD Q is full */
  uint32               num_pkt_drop_pdcp_offload_q_full;
} lte_pdcpdl_pdu_buffer_element_data_s;

typedef struct
{
  uint8 key_index; /*!< valid range 0..15 */
  /*! Use uint8 instead of a2_cipher_algo_e algo to make sure size of this
      this sturcture is always 2 bytes in all compilers */
  uint8 algo; 
}lte_pdcpdl_cipher_init_param_s;

typedef union
{
  lte_pdcpdl_cipher_init_param_s ciph_init_param;
  uint16 value;
}lte_pdcpdl_cipher_param_u;

/*! @brief PDCP UL RLC WM state
*/
typedef enum
{
  LTE_PDCPUL_RLC_WM_SUSPEND = 0, /* enable */
  LTE_PDCPUL_RLC_WM_ENABLE = 1  /* suspend */
} lte_pdcpul_rlc_wm_state_e;

/*! @brief PDCP UL RLC WM
*/
typedef struct
{
  dsm_watermark_type  wm_item; /*!< PDCP RLC tx watermark */
  q_type              wm_q;       /*!< PDCP RLC tx queue */
} lte_pdcpul_rlc_wm_s;

/*! @brief UDC Capability details
*/
typedef struct
{ 
  /*!< UDCMemorySizeMax */
  udc_context_buf_size_e_type  max_cntx_buff_size;

  /*!< UDCPrefillContextId List*/
  uint16  prefill_context_id_list[LTE_PDCPUL_UDC_PREFILL_CNTXT_ID_MAX];

  /*!< Number of UDCPrefillContextId*/
  uint8                          num_prefill_context_id;

  /*!< Algo version supported - bitfield */
  uint8                          algo_version_supported;

  /*!< Max number of DRBs on which compression is supported */
  uint8                          max_drb_supported;
  
  /*!< Indicates if UE supports buffer transfer for PDCP reestablishment */
  boolean                        buffer_transfer_supported;

} lte_pdcpul_udc_cap_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


/*==============================================================================

  FUNCTION:  lte_pdcpdl_get_activity_statistics

==============================================================================*/
/*!
    @brief
    access function for DL data statistics that can be called from other module.

    @detail
    Caller needs to be aware of the possibility of statistics overflow, and 
    handle it appropriately.

    @return
    TRUE if the statistics returned is valid. FALSE if the statistics for the 
    eps_id is not available (bearer removed, etc)
*/
/*============================================================================*/
extern boolean lte_pdcpdl_get_activity_statistics
(
  lte_eps_id_t                  eps_id,   /*!< eps ID */
  lte_pdcpdl_activity_stats_s*  stats_ptr /*!< pointer to caller provided 
                                               staticstics struct */ 
);


/*==============================================================================

  FUNCTION:  lte_pdcpdl_get_circ_q_handle

==============================================================================*/
/*!
    @brief
    This function is used by PDCP OFFLOAD to get the handle to the circular
    queue shared between PDCP DL and PDCP_OFFLOAD.

    @detail
    PDCP and PDCP_OFFLOAD share a circular queue on the DL. PDCP acts as the producer by
    putting the pdus for OFFLOAD. PDCP_OFFLOAD consumes the pdus enqueued by
    PDCP. This function is used by PDCP OFFLOAD to get the handle to the circular
    queue shared between PDCP DL and PDCP_OFFLOAD.

    @return
    Pointer to the PDCPDL-PDCPOFFLOAD circular queue
*/
/*============================================================================*/

extern mutils_circ_q_s* lte_pdcpdl_get_circ_q_handle( void );

/*==============================================================================

  FUNCTION:  lte_pdcpul_get_pdu_bytes_tx

==============================================================================*/
/*!
    @brief
    Get the number of PDCP PDU bytes transmitted per epsid

    @detail
    Gives the total number of PDCP PDU bytes transmitted so far per epsid.
    The values are reset during the connection release.

    @return
    Number of PDCP PDU bytes transmitted per epsid if it is valid. 
    0 Otherwise

*/
/*============================================================================*/
extern uint64 lte_pdcpul_get_pdu_bytes_tx( lte_eps_id_t epsid );


/*==============================================================================

  FUNCTION:  lte_pdcpul_tag_each_pkt

==============================================================================*/
/*!
    @brief
    This function is called to tag eack packet with discard tick and populate
    kind fill as "NEW"

    @detail

    @return

    @note

    @see

*/
/*============================================================================*/

extern void lte_pdcpul_tag_each_pkt
(
  dsm_item_type* pkt_ptr      /*packet to be enqueued to PS-PDCP WM*/
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_tag_and_enqueue_comp_pkt

==============================================================================*/
/*!
    @brief
    This function is called to tag eack packet with discard tick. 
    it also enqueues RoHC packet to WM passed during
    function call

    @detail

    @return

    @note

    @see

*/
/*============================================================================*/

extern void lte_pdcpul_tag_and_enqueue_comp_pkt
(
  dsm_watermark_type* wm_ptr,
  dsm_item_type* pkt_ptr      /*packet to be enqueued to PS-PDCP WM*/
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_get_udc_capability

==============================================================================*/
/*!
    @brief
    This function returns the UDC capability parameters

    @detail

    @return

    @note

    @see

*/
/*============================================================================*/

extern void lte_pdcpul_get_udc_capability
(
  lte_pdcpul_udc_cap_s* udc_cap_ptr /* UDC capability pointer */
);

#endif /* LTE_PDCP_EXT_API_H */
