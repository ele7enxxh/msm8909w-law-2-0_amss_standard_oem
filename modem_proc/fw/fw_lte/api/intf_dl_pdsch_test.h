/*!
  @file
  intf_dl_test_pdsch.h

  @brief
  This file contains lte LL downlink command id's and parameters structures
  for SI message collection.

 
*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_dl_pdsch_test.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef INTF_DL_PDSCH_TEST_H
#define INTF_DL_PDSCH_TEST_H

#include "msgr.h"  
#include "intf_common.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/

   

/*===========================================================================

      Typedefs

===========================================================================*/

// --------------------------------------------------------------------------
//
// data structures for PDSCH config 
//
// --------------------------------------------------------------------------

/*! @brief  PDSCH config request data structure
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;  ///< Common request header
  uint32 num_params;
  lte_LL1_config_element_struct config_params[10];
  
} lte_LL1_dl_pdsch_config_req_struct;

LTE_LL1_INTF_MSG( dl_pdsch_config_req );


/*! @brief  PDSCH test request confirmation data structure
*/
typedef lte_LL1_generic_cnf_struct     lte_LL1_dl_pdsch_config_cnf_struct;
typedef lte_LL1_generic_cnf_msg_struct lte_LL1_dl_pdsch_config_cnf_msg_struct;



// --------------------------------------------------------------------------
//
// data structures for PDSCH test
//
// --------------------------------------------------------------------------

/*! @brief  PDSCH test request data structure
*/
typedef struct {
  uint8   tblk_id;  
  uint8   mod_type;
  uint8   ndi;
  uint8   rv;
  uint8   cw_idx;
  uint32  tblk_size;
  uint16  num_re;
  uint8   llr_scale_select;
  uint8   mcs;
  uint8   num_layers_per_tblk;
} lte_LL1_dl_pdsch_tblk_struct;


typedef struct {
  lte_LL1_req_header_struct   req_hdr;      ///< Common request parameters
  
  boolean subframe_ping_pong; //CCS interface program toggler per subframe
  boolean first_grant;
  boolean last_grant;

  //pdsch grant data
  uint8   rnti_type;
  uint16  rnti;
  uint16  num_tblks;
  uint8   num_code_words;
  uint8   num_layers;
  uint8   harq_id;
  lte_LL1_dl_pdsch_tblk_struct tblk_param[2];
  
 //new ones
  uint8   cell_idx;
  uint8   dci_format; //lte_l1_dci_format_e
  uint8   tx_scheme; //lte_LL1_tx_scheme_e
  uint8   num_rb;  // ?or uint8  num_rb_pairs;            ///< num of bits set in ra_bits bit field
  uint8   pdsch_cfg_ppidx;
  uint32  sic_cb_to_sym_map_0;
  uint32  sic_cb_to_sym_map_1;

} lte_LL1_dl_pdsch_test_req_struct;

LTE_LL1_INTF_MSG( dl_pdsch_test_req );


/*! @brief  PDSCH test request confirmation data structure
*/
typedef lte_LL1_generic_cnf_struct     lte_LL1_dl_pdsch_test_cnf_struct;
typedef lte_LL1_generic_cnf_msg_struct lte_LL1_dl_pdsch_test_cnf_msg_struct;


#endif /* INTF_DL_PDSCH_TEST_H */
