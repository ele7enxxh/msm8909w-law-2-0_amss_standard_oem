#ifndef DIAGDSM_H
#define DIAGDSM_H

/*! 
@ingroup  diag_misc
@file diagdsm.h
@brief 
Definitions for the various DSM Pools used by Diagnostic Services internally.
*/
/*===========================================================================

                                  D I A G D S M . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2005-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.mpss/3.5.c12.3/api/services/diagdsm.h#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
08/31/13    is     Support for peripheral buffering mode
04/30/13    sr     Removed SIO_RX pool. Cmd req/rsp will now use CMD_SIO_TX 
                   and CMD_SIO_RX pool even in backwards compatibility case
02/02/13    sg     Command Response Channel Separation
06/03/12    sa     Added new SIO/SMD DSM Control Pool
06/17/10    mad    change in doxygen document structure, took out 
                   definition and usage of DIAGDSM_DLLIMPORT
06/11/10    vs     Doxygenation of diag headers.
09/30/09    as     Removed WM specific featurizations. 
07/06/09    JV     Carved out and moved private definitions to diagdsm_v.h
03/16/09    mad    Changes for WM7:On WM7, diag dsm pools are exported from
                   diagdsm.dll. Added diagdsm_deinit().
03/16/09    mad    Featurized inclusion of diag.h and customer.h
09/24/07    ms     diagdsm_init() should return false if allocation fails. Incorporating RB's changes to LG into mainline.
06/20/07    pj     Changes for DSM as a DLL.
11/21/06    as     Modified code to use diag internal features.
07/24/06    hal    Added HDR message item pool
03/03/06    ptm    Added oncrpc dsm item pool.
01/01/05    pjb    Created
===========================================================================*/

#include "dsm_pool.h"



/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/


/*---------------------------------------------------------------------------
  Type for identifying a particular DSM memory pool
---------------------------------------------------------------------------*/

/*!
@cond DOXYGEN_BLOAT
*/

/*===========================================================================
 *      Stubs for diagdsm.h
 *===========================================================================*/
dsm_pool_mgmt_table_type dsm_diag_sio_tx_item_pool;
dsm_pool_mgmt_table_type dsm_diag_smd_rx_item_pool;
dsm_pool_mgmt_table_type dsm_diag_smd_tx_item_pool;

/* Dynamic buffering pool */
#define DSM_DIAG_BUFFERING_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_bufering_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_bufering_item_pool;

#define DSM_DIAG_SIO_TX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_sio_tx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_sio_tx_item_pool;

/* SIO Control Pool for RX control packets */
#define DSM_DIAG_SIO_CTRL_RX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_sio_ctrl_rx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_sio_ctrl_rx_item_pool;

/* SIO Control Pool for TX control packets */
#define DSM_DIAG_SIO_CTRL_TX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_sio_ctrl_tx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_sio_ctrl_tx_item_pool;

#define DSM_DIAG_SMD_RX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_smd_rx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_smd_rx_item_pool;


#define DSM_DIAG_SMD_TX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_smd_tx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_smd_tx_item_pool;

/* SMD Control Pool for RX control packets */
#define DSM_DIAG_SMD_CTRL_RX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_smd_ctrl_rx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_smd_ctrl_rx_item_pool;

/* SMD Control Pool for TX control packets */
#define DSM_DIAG_SMD_CTRL_TX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_smd_ctrl_tx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_smd_ctrl_tx_item_pool;

#define DSM_DIAG_CMD_SIO_RX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_cmd_sio_rx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_cmd_sio_rx_item_pool;


#define DSM_DIAG_CMD_SIO_TX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_cmd_sio_tx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_cmd_sio_tx_item_pool;


#define DSM_DIAG_CMD_SIO_DCI_RX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_cmd_sio_dci_rx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_cmd_sio_dci_rx_item_pool;


#define DSM_DIAG_CMD_SIO_DCI_TX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_cmd_sio_dci_tx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_cmd_sio_dci_tx_item_pool;

#define DSM_DIAG_CMD_SMD_RX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_cmd_smd_rx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_cmd_smd_rx_item_pool;


#define DSM_DIAG_CMD_SMD_TX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_cmd_smd_tx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_cmd_smd_tx_item_pool;


#define DSM_DIAG_CMD_SMD_DCI_RX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_cmd_smd_dci_rx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_cmd_smd_dci_rx_item_pool;


#define DSM_DIAG_CMD_SMD_DCI_TX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_cmd_smd_dci_tx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_cmd_smd_dci_tx_item_pool;
/* SMD Control Pool for RX control packets */
#define DSM_DIAG_SMD_CTRL_RX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_smd_ctrl_rx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_smd_ctrl_rx_item_pool;

/* SMD Control Pool for TX control packets */
#define DSM_DIAG_SMD_CTRL_TX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_diag_smd_ctrl_tx_item_pool))
extern dsm_pool_mgmt_table_type dsm_diag_smd_ctrl_tx_item_pool;

/* Used by UART(Siors232_mdd.c and Siors232test.c) and STA(Sta_dsm_server.c) */
#ifndef DSM_DIAG_ITEM_POOL 
#define DSM_DIAG_ITEM_POOL DSM_DIAG_CMD_SIO_TX_ITEM_POOL /* allow old code to use new DSM pool */
#endif

/*!
@endcond
*/

#endif /* DIAGDSM_H */

