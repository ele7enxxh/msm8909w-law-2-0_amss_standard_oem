#ifndef CXM_WCN_TX_H
#define CXM_WCN_TX_H
/*!
  @file
  cxm_wcn_tx.h

  @brief
  APIs for getting TX state of WLAN

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/25/13   tak     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <IxErrno.h>
#include "wcn_coex_mgr.h"
#include "npa.h"

/*===========================================================================

                              DEFINES

===========================================================================*/
typedef enum
{
  CXM_WCN_TX_ENABLED,
  CXM_WCN_TX_DISABLED,
  CXM_WCN_TX_ACTIVE
} cxm_wcn_tx_clnt_status_e;

typedef struct
{
  /*client's state*/
  cxm_wcn_tx_clnt_status_e state;
  /*saved tx state for this client*/
  cxm_wcn_tx_state_e tx_on;
  /* npa client for voting uart power state on/off */
  npa_client_handle uart_client;
  /* last reported WCN tx state (for logging) */
  cxm_wcn_tx_state_e last_rep_tx;
  /* last response code (for logging) */
  cxm_wcn_tx_rsp_codes_e last_rsp_code;
} cxm_client_wcn_tx_info_s;

typedef struct
{
  /* keep track of the last updated client for logging */
  cxm_wcn_tx_client_e last_clnt;
  /* client info */
  cxm_client_wcn_tx_info_s clnt[CXM_WCN_TX_CLIENT_MAX];
} cxm_wcn_tx_info_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  cxm_wcn_tx_init

===========================================================================*/
/*!
  @brief
    To initialize the WLAN TX structures

  @return
    void
*/
/*=========================================================================*/
void cxm_wcn_tx_init (
  void
);

/*===========================================================================

  FUNCTION:  cxm_wcn_tx_deinit

===========================================================================*/
/*!
  @brief
    To deinitialize the WLAN TX structures

  @return
    void
*/
/*=========================================================================*/
void cxm_wcn_tx_deinit (
  void
);

/*===========================================================================

  FUNCTION:  cxm_wcn_get_info_ptr

===========================================================================*/
/*!
  @brief
    Gets wcn tx info for logging.

  @return
    cxm_wcn_tx_info_s*
*/
/*=========================================================================*/
cxm_wcn_tx_info_s* cxm_wcn_get_info_ptr (
  void
);

#endif /* CXM_WCN_TX_H */
