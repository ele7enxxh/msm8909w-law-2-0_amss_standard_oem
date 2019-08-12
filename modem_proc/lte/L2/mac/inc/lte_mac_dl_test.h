#ifndef LTE_MAC_DL_TEST_H
#define LTE_MAC_DL_TEST_H
/*!
  @file lte_mac_dl_test.h

  @brief
   A header specifying prototypes for functions used during unit testing
   of MAC DL. These functions can be used to build a DSM item having the given MAC PDU or MAC RAR
   PDU. 
   Usage: The first parameter to these functions is a pointer to a 
   pointer to DSM item. The pointer must be set to NULL.
   
   To build a DSM item containing a MAC PDU, num_sub_payloads and the lc_id for each 
   subpayload must be set.
   If the subpayload is a mac_sdu or padding, the length must be set.
*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/inc/lte_mac_dl_test.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/12   st      CR 384924: Scell MCE test case support added
01/05/12   st      Made minor compilation changes to comply with eMBMS changes
07/21/08   yg     Initial Checkin

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <lte_l2_comdef.h>
#include <dsm_item.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief 
    Maximum number of MAC sub-payloads that can be present in a DL MAC PDU 
    A MAC PDU consists of several MAC control elements, MAC SDUs and a padding.
    Each of these is referred here as a "sub-payload"
*/
#define LTE_MAC_DL_TEST_MAX_PDU_SUB_PAYLOADS 10

/*! @brief 
    Maximum number of MAC Random Access Responses that can be present in a 
    MAC Random Access Response PDU 
*/
#define LTE_MAC_DL_TEST_MAX_PDU_RARS 10

/*! @brief
    Maximum length of a MAC SDU. i.e Max RLC PDU length approx: IP packet + headers
*/
#define LTE_MAC_DL_TEST_MAX_MAC_SDU_SIZE (1500+5)

/*! @brief
    Maximum length of a MAC PADDING.
*/
#define LTE_MAC_DL_TEST_MAX_MAC_PAD_SIZE 1500

/*! @brief
    Length of UE CONT RES Cmd.
*/
#define LTE_MAC_DL_TEST_UE_CONT_RES_LENGTH 6

/*! @brief Structure for MAC Random Access Response */
typedef struct
{
  boolean ra_id_present;        /*!< TRUE if ra_id is valid, FALSE if overload_ind is valid */
  uint8 ra_id;                  /*!< RA ID */
  uint8 overload_ind;           /*!< Overload indicator */
  uint16 ta;                    /*!< Timing Advance */
  uint32 ul_grant;              /*!< Uplink grant */
  uint16 t_rnti;                /*!< T-RNTI */
} lte_mac_dl_test_mac_rar_s;

/*! @brief Structure for MAC Random Access Response PDU 
*/
typedef struct
{
  uint8                        num_rars;      /*!< Number of Random Access Responses*/
  lte_mac_dl_test_mac_rar_s    rars[LTE_MAC_DL_TEST_MAX_PDU_RARS]; /*!< All Random Access Responses*/
} lte_mac_dl_test_mac_rar_pdu_s;

/*! @brief Structure for MAC Timing Advance Command */
typedef struct
{
  uint8 ta;
} lte_mac_dl_test_ta_cmd;

/*! @brief Structure for MAC Contention Resolution Command */
typedef struct
{
  uint8 ue_id[LTE_MAC_DL_TEST_UE_CONT_RES_LENGTH];
} lte_mac_dl_test_contention_rez_cmd;

/*! @brief Structure for MAC "sub-payload" of MAC PDU
    A MAC PDU consists of several MAC control elements, MAC SDUs and a padding.
    Each of these is referred here as a "sub-payload"
    This is used to represent all MAC PDUs except Transparent PDU and Random Access
    Response PDU
*/
typedef struct
{
  uint32                      lc_id;                  /*!< LCID */
  uint16                      length;                 /*!< Length of mac_sdu or padding */
  union {
    uint8 mac_sdu[LTE_MAC_DL_TEST_MAX_MAC_SDU_SIZE];     /*!< MAC SDU */
    uint8 scell_cfg;
    lte_mac_dl_test_ta_cmd ta_cmd;                       /*!< Timing Advance Command Control Elem*/
    lte_mac_dl_test_contention_rez_cmd cont_rez_cmd;             /*!< Contention Resolution Command Control Elem*/
    uint8 padding[LTE_MAC_DL_TEST_MAX_MAC_PAD_SIZE];     /*!< Padding */
  };
} lte_mac_dl_test_mac_sub_payload_s;

/*! @brief Structure for MAC PDU
    This is used to represent all MAC PDUs except Transparent PDU and Random Access
*/
typedef struct
{
  uint8                        num_sub_payloads;      /*!< Number of MAC SDU, Control Elem or padding*/
  lte_mac_dl_test_mac_sub_payload_s sub_payloads[LTE_MAC_DL_TEST_MAX_PDU_SUB_PAYLOADS]; /*!< All MAC SDU, Control Elem or padding of the PDU*/
} lte_mac_dl_test_mac_pdu_s;
/*===========================================================================

  FUNCTION:  lte_mac_dl_test_set_test_mode

===========================================================================*/
/*!
    @brief
    This function sets the MAC DL to test mode.

    @detail

    @return

    @note

    @see related_function()
*/
/*=========================================================================*/
void lte_mac_dl_test_set_test_mode
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_test_build_mac_pdu

===========================================================================*/
/*!
  @brief
  Builds a MAC PDU and inserts the MAC PDU into the given DSM item according to the MAC PDU
  format specified by the standard

*/
/*=========================================================================*/
boolean lte_mac_dl_test_build_mac_pdu
(
  lte_mac_dl_test_mac_pdu_s* mac_pdu,
  dsm_item_type** buf, 
  dsm_mempool_id_type pool_id
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_test_build_mac_rar_pdu

===========================================================================*/
/*!
  @brief
  Builds a MAC RAR PDU and inserts the MAC RAR PDU into the given DSM item according to the MAC PDU
  format specified by the standard

*/
/*=========================================================================*/
boolean lte_mac_dl_test_build_mac_rar_pdu
( 
  lte_mac_dl_test_mac_rar_pdu_s* mac_rar_pdu,
  dsm_item_type** buf, 
  dsm_mempool_id_type pool_id
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_test_build_ta_cmd_pdu

===========================================================================*/
/*!
  @brief
  Builds a MAC TA CMD PDU and inserts the MAC TA CMD PDU into the given DSM item according to the MAC PDU
  format specified by the standard

*/
/*=========================================================================*/
boolean lte_mac_dl_test_build_ta_cmd_pdu
( 
  uint8 ta,
  dsm_item_type** buf, 
  dsm_mempool_id_type pool_id
);
/*===========================================================================

  FUNCTION:  lte_mac_dl_test_build_ue_cont_rez_cmd_pdu

===========================================================================*/
/*!
  @brief
  Builds a MAC UE CONT REZ CMD PDU and inserts the MAC UE CONT REZ CMD PDU into the given DSM item according to the MAC PDU
  format specified by the standard

*/
/*=========================================================================*/
boolean lte_mac_dl_test_build_ue_cont_rez_cmd_pdu
( 
  uint8 ue_id[LTE_MAC_DL_TEST_UE_CONT_RES_LENGTH],
  dsm_item_type** buf, 
  dsm_mempool_id_type pool_id
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_test_insert_mac_pdu

===========================================================================*/
/*!
  @brief
  Inserts the MAC PDU into the given DSM item
  
  @detail
  Inserts the MAC PDU into the given DSM item according to the MAC PDU
  format specified by the standard
  
  @return
  boolean
  
  @note

  @see related_function()

*/
/*=========================================================================*/
boolean lte_mac_dl_test_insert_mac_pdu
( 
  dsm_item_type** buf, 
  lte_mac_dl_test_mac_pdu_s* mac_pdu,
  dsm_mempool_id_type pool_id
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_test_insert_mac_rar_pdu

===========================================================================*/
/*!
  @brief
  Inserts the MAC RAR PDU into the given DSM item according to the MAC PDU
  format specified by the standard

*/
/*=========================================================================*/
boolean lte_mac_dl_test_insert_mac_rar_pdu
( 
  dsm_item_type** buf, 
  lte_mac_dl_test_mac_rar_pdu_s* mac_rar_pdu,
  dsm_mempool_id_type pool_id
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_test_dl_state_is_active

===========================================================================*/
/*!
  @brief
  check if MAC DL state is active
  
  @return
  Returns TRUE if equal

*/
/*=========================================================================*/
extern boolean lte_mac_dl_test_dl_state_is_active
( 
  void
);

#endif /* LTE_MAC_DL_TEST_H */

