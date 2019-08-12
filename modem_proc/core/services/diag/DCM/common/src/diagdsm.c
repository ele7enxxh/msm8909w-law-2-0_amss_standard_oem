/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D I A G D S M . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

  diagdsm_init()
  diagdsm_deinit()
  diagdsm_delete()
    Initialize the Data Services Memory pool unit.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

Copyright (c) 2005-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagdsm.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/31/13   is      Support for peripheral buffering mode 
04/30/13   sr      Removed SIO_RX pool. Cmd req/rsp will now use CMD_SIO_TX 
                   and CMD_SIO_RX pool even in backwards compatibility case  
01/03/13   sg      Command Response Channel separation
06/03/12   sa      Added RX/TX Control Channel Pool 
05/10/12   sa      Removed DIAG_MP_SLAVE_LAST
03/10/11   is      Move DSM_ALIGNED_ALLOC() from DSM to Diag
01/18/10   JV      Do not initilaize the 2 SMD pools for single processor targets.
10/06/09   JV      Mainlined FEATURE_DSM_DIAG_ITEMS.
09/23/09   JV      Do not initialize the SMD_RX and SMD_TX pools on the slave
                   last.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/24/09    mad    Corrected comment
06/22/09    mad    Featurized inclusion of some diag header files
04/03/09    mad    Moved DiagDSM item sizes etc to a new header file diagdsmi.h
03/26/09    mad    Re-enabled target-specific definitions of DIAG DSM item 
                   SIZ/CNT
03/24/09    mad    Correction in DSM pool macro definitions:
                   enclosed in brackets
03/16/09    mad    Changes for WM7: prevent DSM pool initialization if 
                   already initialized. Added diagdsm_deinit()
03/06/09    mad    Redefined DIAG DSM item sizes/counts.Upper-level 
                   target-specific definitions for these are now over-ridden
                   in this file.
03/05/09    vg     Corrected the DSM pools (SMD_RX_ITEM & SMD_TX_ITEM)  
01/13/09    is     Removed redundant memory allocation due to use of 
                   deprecated DSM API, dsm_init_pool()
05/01/08    as     Fixed featurization on DSM pools for modem only builds.
10/18/07    pj     Changed code to align pools on 32-byte boundary.
09/25/07    ms     Merging functions from diagdsm_wce.c into this file.
09/24/07    ms     diagdsm_init() should return false if allocation fails. 
                   Incorporating RB's changes for LG into mainline.
06/20/07    pj     changes for DSM as a DLL.
01/22/06    as     Featurized code to make DSM count & size defined by target
12/05/06    as     Increased DSM SIO and SMD TX item size.
11/21/06    as     Modified code to use diag internal features.
09/13/06    as     Created file
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "core_variation.h"
#include "comdef.h"
#ifndef FEATURE_WINCE
#include "queue.h"
#endif

#include "dsm.h"
#include "diagdsm_v.h"
#include "diagdsmi.h"
#include "diag_v.h"
#include "diagi_v.h"     /* For diag_tx_mode_type */
#include "diagcomm_io.h" /* For DIAG_STREAM_1, DIAG_STREAM_2 */

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h" 
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#if !defined (DIAG_DSM_ALIGNED_ALLOC)
#if defined __arm
#define DIAG_DSM_ALIGNED_ALLOC(name,size) \
  static uint8 __align((32)) name[size]
#elif defined __GNUC__
#define DIAG_DSM_ALIGNED_ALLOC(name, size) \
  static uint8 name[size] __attribute__ ((aligned(32)))
#else
#define DIAG_DSM_ALIGNED_ALLOC(name,size) \
  static uint32 name[(size + 31) /  sizeof(uint32)]
#endif /* __arm et. al. */
#endif /* !DIAG_DSM_ALIGNED_ALLOC */

extern diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID];

/*--------------------------------------------------------------------------
  Declare pools sizes and control structures.
--------------------------------------------------------------------------*/

#define DSMI_DIAG_SIO_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_SIO_TX_ITEM_CNT * \
   (DSMI_DIAG_SIO_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_sio_tx_item_pool;

/* Dynamic buffering pool */
dsm_pool_mgmt_table_type dsm_diag_bufering_item_pool;

#if !defined(DIAG_MP_MASTER)
//SIO Control Channel array size
#define DSMI_DIAG_SIO_CTRL_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_SIO_CTRL_RX_ITEM_CNT * \
   (DSMI_DIAG_SIO_CTRL_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_sio_ctrl_rx_item_pool;

//SIO Control Channel array size
#define DSMI_DIAG_SIO_CTRL_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_SIO_CTRL_TX_ITEM_CNT * \
   (DSMI_DIAG_SIO_CTRL_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_sio_ctrl_tx_item_pool;

#endif
#ifndef DIAG_MP_MASTER
#define DSMI_DIAG_CMD_SIO_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_RX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_rx_item_pool;

#define DSMI_DIAG_CMD_SIO_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_TX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_tx_item_pool;

#define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_dci_rx_item_pool;

#define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_dci_tx_item_pool;
#else
#define DSMI_DIAG_CMD_SMD_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SMD_RX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SMD_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_smd_rx_item_pool;

#define DSMI_DIAG_CMD_SMD_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SMD_TX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SMD_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_smd_tx_item_pool;

#define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_smd_dci_rx_item_pool;

#define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_DCI_TX_ITEM_CNT * \
   (DSMI_DIAG_CMD_DCI_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_dci_tx_item_pool;
#endif

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
  #define DSMI_DIAG_SMD_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_SMD_RX_ITEM_CNT * \
     (DSMI_DIAG_SMD_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))

dsm_pool_mgmt_table_type dsm_diag_smd_rx_item_pool;

#define DSMI_DIAG_SMD_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_SMD_TX_ITEM_CNT * \
   (DSMI_DIAG_SMD_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_smd_tx_item_pool;

//SMD Control channel array size
#define DSMI_DIAG_SMD_CTRL_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_SMD_CTRL_RX_ITEM_CNT * \
   (DSMI_DIAG_SMD_CTRL_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_smd_ctrl_rx_item_pool;

//SMD Control channel array size
#define DSMI_DIAG_SMD_CTRL_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_SMD_CTRL_TX_ITEM_CNT * \
   (DSMI_DIAG_SMD_CTRL_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_smd_ctrl_tx_item_pool;

#endif


#if !defined(FEATURE_WINCE) /* allocations are done using VirtualAlloc on WM */
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_sio_tx_item_array,DSMI_DIAG_SIO_TX_ITEM_ARRAY_SIZ);

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_sio_ctrl_rx_item_array,DSMI_DIAG_SIO_CTRL_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_sio_ctrl_tx_item_array,DSMI_DIAG_SIO_CTRL_TX_ITEM_ARRAY_SIZ);
#endif

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_smd_rx_item_array,DSMI_DIAG_SMD_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_smd_tx_item_array,DSMI_DIAG_SMD_TX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_smd_ctrl_rx_item_array,DSMI_DIAG_SMD_CTRL_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_smd_ctrl_tx_item_array,DSMI_DIAG_SMD_CTRL_TX_ITEM_ARRAY_SIZ);
#endif /* DIAG_MP && DIAG_MP_MASTER */

#ifndef DIAG_MP_MASTER
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_sio_rx_item_array,DSMI_DIAG_CMD_SIO_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_sio_tx_item_array,DSMI_DIAG_CMD_SIO_TX_ITEM_ARRAY_SIZ);

DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_sio_dci_rx_item_array,DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_sio_dci_tx_item_array,DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_ARRAY_SIZ);
#else 
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_smd_rx_item_array,DSMI_DIAG_CMD_SMD_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_smd_tx_item_array,DSMI_DIAG_CMD_SMD_TX_ITEM_ARRAY_SIZ);

DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_smd_dci_rx_item_array,DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_smd_dci_tx_item_array,DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_ARRAY_SIZ);
#endif
#endif /* ifndef FEATURE_WINCE */

#ifdef FEATURE_WINCE
#error code not present
#endif /* FEATURE_WINCE */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


/*===========================================================================
FUNCTION DIAGDSM_INIT()

DESCRIPTION
  This function will initialize the Data Service Memory Pool. It should be
  called once upon system startup. All the memory items are initialized and
  put onto their respective free queues.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if successful, or if memory already allocated. FALSE if allocation is unsuccessful.

SIDE EFFECTS
  None
  This function should be called only from Windiag, on WM.
===========================================================================*/
#if defined (FEATURE_WINCE)
#error code not present
#endif
boolean diagdsm_init(void)
{
  #if defined (FEATURE_WINCE)
  #error code not present
#endif
  
  /* In WM, dsm_init_pool() is a deprecated DSM API which is replaced by dsm_create_pool().  Since dsm_create_pool() internally calls
     dsm_init_pool(), we will continue to use this deprecated API to enable WM/AMSS code to be consistent. */
  
  dsm_init_pool(DSM_DIAG_SIO_TX_ITEM_POOL,
    (uint8*)dsm_diag_sio_tx_item_array,
    DSMI_DIAG_SIO_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SIO_TX_ITEM_SIZ);

  /* Save info for buffering mode pool */
  diag_tx_mode[DIAG_STREAM_1-1].pool_info.pool_id = DSM_DIAG_SIO_TX_ITEM_POOL;
  diag_tx_mode[DIAG_STREAM_1-1].pool_info.item_size = DIAG_DEFAULT_BUFFERED_DSM_ITEM_SIZE;
  diag_tx_mode[DIAG_STREAM_1-1].pool_info.item_cnt = DIAG_DEFAULT_BUFFERED_DSM_TOT_COUNT;
  
  /* DCI uses same pool */
  diag_tx_mode[DIAG_STREAM_2-1].pool_info.pool_id = DSM_DIAG_SIO_TX_ITEM_POOL;
  diag_tx_mode[DIAG_STREAM_2-1].pool_info.item_size = DIAG_DEFAULT_BUFFERED_DSM_ITEM_SIZE;
  diag_tx_mode[DIAG_STREAM_2-1].pool_info.item_cnt = DIAG_DEFAULT_BUFFERED_DSM_TOT_COUNT;
  
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
  //Initializing the SIO Control DSM Pool
  dsm_init_pool(DSM_DIAG_SIO_CTRL_RX_ITEM_POOL, 
    (uint8*)dsm_diag_sio_ctrl_rx_item_array,
    DSMI_DIAG_SIO_CTRL_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SIO_CTRL_RX_ITEM_SIZ);

  //Initializing the SIO TX Control DSM Pool
  dsm_init_pool(DSM_DIAG_SIO_CTRL_TX_ITEM_POOL, 
    (uint8*)dsm_diag_sio_ctrl_tx_item_array,
    DSMI_DIAG_SIO_CTRL_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SIO_CTRL_TX_ITEM_SIZ);
#endif 

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
  dsm_init_pool(DSM_DIAG_SMD_RX_ITEM_POOL, 
    (uint8*)dsm_diag_smd_rx_item_array,
    DSMI_DIAG_SMD_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SMD_RX_ITEM_SIZ);

  dsm_init_pool(DSM_DIAG_SMD_TX_ITEM_POOL, 
    (uint8*)dsm_diag_smd_tx_item_array,
    DSMI_DIAG_SMD_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SMD_TX_ITEM_SIZ);
	
  //Initializing the SMD RX Control Pool
  dsm_init_pool(DSM_DIAG_SMD_CTRL_RX_ITEM_POOL, 
    (uint8*)dsm_diag_smd_ctrl_rx_item_array,
    DSMI_DIAG_SMD_CTRL_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SMD_CTRL_RX_ITEM_SIZ);

  //Initializing the SMD TX Control Pool
  dsm_init_pool(DSM_DIAG_SMD_CTRL_TX_ITEM_POOL, 
    (uint8*)dsm_diag_smd_ctrl_tx_item_array,
    DSMI_DIAG_SMD_CTRL_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SMD_CTRL_TX_ITEM_SIZ);		
#endif

#if !defined(DIAG_MP_MASTER)
  dsm_init_pool(DSM_DIAG_CMD_SIO_RX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_sio_rx_item_array,
    DSMI_DIAG_CMD_SIO_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SIO_RX_ITEM_SIZ);

  dsm_init_pool(DSM_DIAG_CMD_SIO_TX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_sio_tx_item_array,
    DSMI_DIAG_CMD_SIO_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SIO_TX_ITEM_SIZ);

  dsm_init_pool(DSM_DIAG_CMD_SIO_DCI_RX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_sio_dci_rx_item_array,
    DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ);

  dsm_init_pool(DSM_DIAG_CMD_SIO_DCI_TX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_sio_dci_tx_item_array,
    DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ);
  #else
  dsm_init_pool(DSM_DIAG_CMD_SMD_RX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_smd_rx_item_array,
    DSMI_DIAG_CMD_SMD_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SMD_RX_ITEM_SIZ);

  dsm_init_pool(DSM_DIAG_CMD_SMD_TX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_smd_tx_item_array,
    DSMI_DIAG_CMD_SMD_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SMD_TX_ITEM_SIZ);

  dsm_init_pool(DSM_DIAG_CMD_SMD_DCI_RX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_smd_dci_rx_item_array,
    DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ);

  dsm_init_pool(DSM_DIAG_CMD_SMD_DCI_TX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_smd_dci_tx_item_array,
    DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ);
#endif

return TRUE;

} /* diagdsm_init */

#ifdef FEATURE_WINCE
#error code not present
#endif
