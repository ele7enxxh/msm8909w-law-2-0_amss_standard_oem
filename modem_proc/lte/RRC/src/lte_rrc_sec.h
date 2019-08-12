/*!
  @file lte_rrc_sec.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_sec.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/09   dd      Initial Version
===========================================================================*/

#ifndef LTE_RRC_SEC_H
#define LTE_RRC_SEC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_sec.h>
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
#include "lte_rrc_int_msg.h"
#include "lte_pdcp_msg.h"
#include "lte_as.h"
#include <lte_rrc_csp.h>
#include "lte_security.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*Returns TRUE if SEC is in secure state (i.e.) if security is configured; and
FALSE if security is not configured*/
extern boolean lte_rrc_sec_security_is_configured
(
 void
);

/*Returns E_SUCCESS if validation passes */
extern lte_errno_e lte_rrc_sec_validate_config
(
  /*! Pointer to decoded ASN.1 DL DCCH Message */
  lte_rrc_osys_RRCConnectionReconfiguration *dl_dcch_msg_ptr
);

/*Returns E_SUCCESS on successful preparation of security Config
The RRC reconfig msg (a DCCH msg)--handover is used as an input */
extern lte_errno_e lte_rrc_sec_dcch_prepare_config
(
  /*! DL DCCH Message transaction id */
  lte_rrc_osys_RRC_TransactionIdentifier dcch_trans_id,
  /*!Sec Cfg Ptr */
  lte_pdcp_security_cfg_s *sec_config_ptr,
  /*!Phy cell ID of the target cell*/
  lte_phy_cell_id_t phy_cell_id,
  /*!Downlink EARFCN of the target cell*/
  lte_earfcn_t freq
);

/*Returns E_SUCCESS on successful preparation of security Config
The RRC reconfig msg (a DCCH msg)--handover is used as an input */
extern lte_errno_e lte_rrc_sec_process_irat_to_lte_ho_config
(
  /*! DL DCCH Message transaction id */
  lte_rrc_osys_RRC_TransactionIdentifier,
  /*!Sec Cfg Ptr */
  lte_pdcp_security_cfg_s *sec_config_ptr,
  /*!Phy cell ID of the target cell*/
  lte_phy_cell_id_t phy_cell_id,
  /*!Downlink EARFCN of the target cell*/
  lte_earfcn_t freq
);

/*Returns E_SUCCESS on successful preparation of  security Config.
This can be used during initial security activation(SMC) or RRC re-establishment 
For Sec Cfg during initial security activation, use ncc_is_valid = FALSE*/
extern lte_errno_e lte_rrc_sec_param_prepare_config
(
  /*! Nexthop Chaining Count */
  uint8 ncc,
  boolean ncc_is_valid,
  /*!Sec Cfg Ptr */
  lte_pdcp_security_cfg_s *sec_config_ptr
);

/*Returns the 16 bit Short MAC-I*/
extern uint16 lte_rrc_sec_compute_short_maci
(
 lte_phy_cell_id_t phy_cell_id,  /*!< Physical cell id of the previous cell*/ 
 lte_rnti_val_t c_rnti/*C-RNTI the UE had on the previous cell*/
);


/*===========================================================================

  FUNCTION: lte_rrc_sec_init 

===========================================================================*/
/*!
    @brief
    Allocates and initializes dynamic memory
    @return
    void

*/
/*=========================================================================*/

extern void lte_rrc_sec_init(void);

/*===========================================================================

  FUNCTION: lte_rrc_sec_deinit 

===========================================================================*/
/*!
    @brief
    Deallocates  dynamic memory
    @return
    void

*/
/*=========================================================================*/

extern void lte_rrc_sec_deinit(void);

/*===========================================================================

  FUNCTION:  lte_rrc_sec_send_pdcpdl_sec_reset_req

===========================================================================*/
/*!
    @brief
    Sends PDCPDL_SEC_RESET_REQ to PDCP
    @return
    void 

*/
/*=========================================================================*/
extern void lte_rrc_sec_send_pdcpdl_sec_reset_req
(
  void
);
/*===========================================================================

  TRANSITION FUNCTION:  lte_rrc_sec_need_to_skip_l2x_key_derivation

===========================================================================*/
  /*!
      @brief
      Do we need to skip L2X key derivation during HO
  
      @return
      Returns TRUE if no AUTH/NAS keys are presnt.
  
  */

/*=========================================================================*/
boolean lte_rrc_sec_need_to_skip_l2x_key_derivation
(
 void 
);

#endif /* LTE_RRC_SEC_H */
