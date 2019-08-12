/*!
  @file
  cxm_wcn_tx.c

  @brief
  Implementation of TX sticky bit manager

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

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include <IxErrno.h>
#include <atomic_ops.h>
#include "cxm_utils.h"
#include "cxm_wcn_tx.h"
#include "mcs_hwio.h"
#include "wci2_uart.h"
#include "cxm_trace.h"

/*===========================================================================

                         LOCAL VARIABLES AND MACROS

===========================================================================*/

#ifndef HWIO_MSS_CXM_RX_STAT_WLAN_TX_STICKY_BMSK
  #define HWIO_MSS_CXM_RX_STAT_WLAN_TX_STICKY_BMSK 0
#endif

#ifndef HWIO_MSS_CXM_RX_STAT_IN
  #define HWIO_MSS_CXM_RX_STAT_IN 0
#endif

#if !defined( HWIO_MSS_CXM_RX_STAT_OUT )
  #define HWIO_MSS_CXM_RX_STAT_OUT( v )
#endif

#define CXM_TX_STICKY_CLEAR_VAL   1
         
#define CXM_WCN_TX_ENTER_ATOMIC_SECT() \
         while( !atomic_compare_and_set( &cxm_wcn_tx_ops_active, 0, 1 ) )

#define CXM_WCN_TX_EXIT_ATOMIC_SECT() \
         atomic_set( &cxm_wcn_tx_ops_active, 0 )

STATIC cxm_wcn_tx_info_s cxm_wcn_tx_info;
#ifndef FEATURE_MCS_COEX_NOCODE
STATIC atomic_word_t cxm_wcn_tx_ops_active = ATOMIC_INIT(0);
STATIC boolean cxm_wcn_tx_initialized = FALSE;
#endif /* FEATURE_MCS_COEX_NOCODE */

/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

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
)
{
#ifndef FEATURE_MCS_COEX_NOCODE
  uint8 i;
  /*-----------------------------------------------------------------------*/

  if ( cxm_wcn_tx_initialized )
  {
    return;
  }
  memset( &cxm_wcn_tx_info.clnt, 0, sizeof( cxm_client_wcn_tx_info_s ) 
                                            * CXM_WCN_TX_CLIENT_MAX );  
  /* create npa clients for turning on/off uart for each wcn_tx client*/
  for ( i = 0; i < CXM_WCN_TX_CLIENT_MAX; i++ )
  {
    cxm_wcn_tx_info.clnt[i].state = CXM_WCN_TX_DISABLED;
    cxm_wcn_tx_info.clnt[i].uart_client = npa_create_sync_client(
                                            "/modem/mcs/cxm_uart",
                                            "coex_wcn_tx",
                                            NPA_CLIENT_REQUIRED );
  }

  cxm_wcn_tx_initialized = TRUE;
#endif /* FEATURE_MCS_COEX_NOCODE */
  
} /* cxm_wcn_tx_init */

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
)
{
#ifndef FEATURE_MCS_COEX_NOCODE
  cxm_wcn_tx_initialized = FALSE;
#endif /* FEATURE_MCS_COEX_NOCODE */
}

/*===========================================================================

  FUNCTION:  cxm_wcn_tx_enable

===========================================================================*/
/*!
  @brief
    Power on the uart block. This must be called by clients
    before using cxm_wcn_tx_start and cxm_wcn_tx_stop. When WLAN TX is not
    needed for some time, cxm_wcn_tx_disable must be called.

  @return
    cxm_wcn_tx_rsp_codes_e
*/
/*=========================================================================*/
cxm_wcn_tx_rsp_codes_e cxm_wcn_tx_enable (
  cxm_wcn_tx_client_e client
)
{
  cxm_wcn_tx_rsp_codes_e ret_val = CXM_WCN_TX_RSP_SUCCESS;
  /*-----------------------------------------------------------------------*/
#ifndef FEATURE_MCS_COEX_NOCODE
  CXM_WCN_TX_ENTER_ATOMIC_SECT();
  cxm_wcn_tx_info.last_clnt = client;
  if( cxm_wcn_tx_initialized == FALSE )
  {
    ret_val = CXM_WCN_TX_RSP_NOT_SUPPORTED;
  }
  else if ( client >= CXM_WCN_TX_CLIENT_MAX )
  {
    ret_val = CXM_WCN_TX_RSP_INVALID_CLIENT;
  }
  else
  {
    if ( cxm_wcn_tx_info.clnt[client].state == CXM_WCN_TX_ENABLED )
    {
      ret_val = CXM_WCN_TX_RSP_CLNT_ALREADY_ENABLED;
    }
    else
    {
      cxm_wcn_tx_info.clnt[client].state = CXM_WCN_TX_ENABLED;
      npa_issue_required_request( cxm_wcn_tx_info.clnt[client].uart_client, WCI2_UART_NPA_REQ_ON );
    }
    
  }
  cxm_wcn_tx_info.clnt[client].last_rsp_code = ret_val;
  CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_WCN_TX_STATE);
  CXM_WCN_TX_EXIT_ATOMIC_SECT();
  
#endif /* FEATURE_MCS_COEX_NOCODE */
  return ret_val;
  
} /* cxm_wcn_tx_enable */

/*===========================================================================

  FUNCTION:  cxm_wcn_tx_disable

===========================================================================*/
/*!
  @brief
    Power off the uart block. 

  @return
    cxm_wcn_tx_rsp_codes_e
*/
/*=========================================================================*/
cxm_wcn_tx_rsp_codes_e cxm_wcn_tx_disable (
  cxm_wcn_tx_client_e client
)
{
  cxm_wcn_tx_rsp_codes_e ret_val = CXM_WCN_TX_RSP_SUCCESS;
  /*-----------------------------------------------------------------------*/
  
#ifndef FEATURE_MCS_COEX_NOCODE
  CXM_WCN_TX_ENTER_ATOMIC_SECT(); 
  cxm_wcn_tx_info.last_clnt = client;
  if( cxm_wcn_tx_initialized == FALSE )
  {
    ret_val = CXM_WCN_TX_RSP_NOT_SUPPORTED;
  }
  else if ( client >= CXM_WCN_TX_CLIENT_MAX )
  {
    ret_val = CXM_WCN_TX_RSP_INVALID_CLIENT;
  }
  else
  {
    if ( cxm_wcn_tx_info.clnt[client].state == CXM_WCN_TX_DISABLED )
    {
      ret_val = CXM_WCN_TX_RSP_CLNT_ALREADY_DISABLED;
    }
    else if ( cxm_wcn_tx_info.clnt[client].state == CXM_WCN_TX_ACTIVE )
    {
      ret_val = CXM_WCN_TX_RSP_DISABLE_FAIL_TX_CHECK_ACTIVE;
    }
    else
    {
      cxm_wcn_tx_info.clnt[client].state = CXM_WCN_TX_DISABLED;
      npa_issue_required_request( cxm_wcn_tx_info.clnt[client].uart_client, WCI2_UART_NPA_REQ_OFF );
    }
    
  }
  cxm_wcn_tx_info.clnt[client].last_rsp_code = ret_val;
  CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_WCN_TX_STATE);
  CXM_WCN_TX_EXIT_ATOMIC_SECT();
#endif /* FEATURE_MCS_COEX_NOCODE */
  return ret_val;

} /* cxm_wcn_tx_disable */

/*===========================================================================

  FUNCTION:  cxm_wcn_tx_start

===========================================================================*/
/*!
  @brief
    To start collecting WLAN tx state for this tech

  @return
    void
*/
/*=========================================================================*/
cxm_wcn_tx_rsp_codes_e cxm_wcn_tx_start (
  cxm_wcn_tx_client_e client
)
{
#ifndef FEATURE_MCS_COEX_NOCODE
  cxm_wcn_tx_state_e sticky_state = CXM_WCN_TX_STATE_INACTIVE;
  uint8 i = 0;
#endif /* FEATURE_MCS_COEX_NOCODE */
  cxm_wcn_tx_rsp_codes_e ret_val = CXM_WCN_TX_RSP_SUCCESS;
  /*-----------------------------------------------------------------------*/

#ifndef FEATURE_MCS_COEX_NOCODE
  CXM_WCN_TX_ENTER_ATOMIC_SECT();
  cxm_wcn_tx_info.last_clnt = client;
  if( client >= CXM_WCN_TX_CLIENT_MAX )
  {
    ret_val = CXM_WCN_TX_RSP_INVALID_CLIENT;
  }
  else
  {
    if ( cxm_wcn_tx_info.clnt[client].state == CXM_WCN_TX_DISABLED )
    {
      ret_val = CXM_WCN_TX_RSP_CLNT_NOT_ENABLED;
    }
    else if ( cxm_wcn_tx_info.clnt[client].state == CXM_WCN_TX_ACTIVE )
    {
      ret_val = CXM_WCN_TX_RSP_CLNT_ALREADY_STARTED;
    }
    else
    {
      /*update any active client with the current state of the bit 
        before clearing it*/
      if ( HWIO_MSS_CXM_RX_STAT_WLAN_TX_STICKY_BMSK & HWIO_MSS_CXM_RX_STAT_IN )
      {
        sticky_state = CXM_WCN_TX_STATE_ACTIVE;
      }
      else
      {
        sticky_state = CXM_WCN_TX_STATE_INACTIVE;
      }
      
      for ( i=0; i < CXM_WCN_TX_CLIENT_MAX; i++ )
      {
        if ( cxm_wcn_tx_info.clnt[i].state == CXM_WCN_TX_ACTIVE )
        {
          cxm_wcn_tx_info.clnt[i].tx_on |= sticky_state;
        }
      }

      /*clear the tx sticky bit*/
      HWIO_MSS_CXM_RX_STAT_OUT(CXM_TX_STICKY_CLEAR_VAL);

      /*set requesting client to active*/
      cxm_wcn_tx_info.clnt[client].state = CXM_WCN_TX_ACTIVE;
      cxm_wcn_tx_info.clnt[client].tx_on = CXM_WCN_TX_STATE_INACTIVE;
    }
  }
  cxm_wcn_tx_info.clnt[client].last_rsp_code = ret_val;
  CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_WCN_TX_STATE);
  CXM_WCN_TX_EXIT_ATOMIC_SECT();
#endif /* FEATURE_MCS_COEX_NOCODE */
  return ret_val;

} /* cxm_wcn_tx_start */


/*===========================================================================

  FUNCTION:  cxm_wcn_tx_stop

===========================================================================*/
/*!
  @brief
    To stop collecting WLAN tx state for this client and return the tx state for
    the period. cxm_wcn_tx_start must be called before calling this.

  @return
    cxm_wcn_tx_rsp_codes_e
*/
/*=========================================================================*/
cxm_wcn_tx_rsp_codes_e cxm_wcn_tx_stop (
  cxm_wcn_tx_client_e client,
  cxm_wcn_tx_state_e* tx_state
)
{
  cxm_wcn_tx_rsp_codes_e ret_val = CXM_WCN_TX_RSP_SUCCESS;
#ifndef FEATURE_MCS_COEX_NOCODE
  cxm_wcn_tx_state_e sticky_state = CXM_WCN_TX_STATE_INACTIVE;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT(tx_state != NULL);
  CXM_WCN_TX_ENTER_ATOMIC_SECT();
  cxm_wcn_tx_info.last_clnt = client;
  if( client >= CXM_WCN_TX_CLIENT_MAX )
  {
    ret_val = CXM_WCN_TX_RSP_INVALID_CLIENT;
  }
  else
  {
    if ( cxm_wcn_tx_info.clnt[client].state == CXM_WCN_TX_DISABLED )
    {
      ret_val = CXM_WCN_TX_RSP_CLNT_NOT_ENABLED;
    }
    else if( cxm_wcn_tx_info.clnt[client].state == CXM_WCN_TX_ENABLED )
    {
      ret_val = CXM_WCN_TX_RSP_CLNT_ALREADY_STOPPED;
    }
    else
    {
      /*reset the client*/
      cxm_wcn_tx_info.clnt[client].state = CXM_WCN_TX_ENABLED;

      /*wlan tx may have been ON during this client's collection period but cleared by 
        another client so we OR stored state with the current state of the sticky bit*/
      if (HWIO_MSS_CXM_RX_STAT_WLAN_TX_STICKY_BMSK & HWIO_MSS_CXM_RX_STAT_IN )
      {
        sticky_state = CXM_WCN_TX_STATE_ACTIVE;
      }
      else
      {
        sticky_state = CXM_WCN_TX_STATE_INACTIVE;
      }
      (*tx_state) = cxm_wcn_tx_info.clnt[client].last_rep_tx = cxm_wcn_tx_info.clnt[client].tx_on | sticky_state;
    }
  }
  cxm_wcn_tx_info.clnt[client].last_rsp_code = ret_val;
  CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_WCN_TX_STATE);
  /* clear the logged tx state */
  cxm_wcn_tx_info.clnt[client].last_rep_tx = 0;
  CXM_WCN_TX_EXIT_ATOMIC_SECT();
#endif /* FEATURE_MCS_COEX_NOCODE */
  return ret_val;

} /* cxm_wcn_tx_stop */

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
)
{
  return &cxm_wcn_tx_info;
} /* cxm_wcn_get_info_ptr */



