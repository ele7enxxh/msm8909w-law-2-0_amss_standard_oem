/*===========================================================================

                       P S _ S O C K _ M E M _ P O O L _ C

DESCRIPTION
 The Protocol Services Socket Memory Pool Header File. Contains declarations
 of functions specifice to sockets memory initialization.

EXTERNAL FUNCTIONS
  PS_SOCK_MEM_POOL_INIT()
    Initialized memory for sockets module

Copyright (c) 2007-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                             EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Note that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/ps_sock_mem_pool.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/11   pp      Re-tune metainfo buffers.
06/28/11   am      Moved meta info buffers to static configuration.
12/27/10   rp      RAM optimization changes.
10/07/10   am      Increased meta info buf nums for SoftAp modem.
10/20/08   hm      Moved inet defs to ps_inet_mem_pool.c file, removed
                   ACB, SCB, dssocki, route scope defs.
03/13/08   pp      Metainfo optimizations.
07/16/07   va      Created the module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "err.h"

#include "ps_mem.h"
#include "ps_meta_info.h"
#include "ps_tx_meta_info.h"
#include "ps_pkt_info.h"
#include "ps_socket_defs.h"
#include "ps_sock_mem_pool.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                     LOCAL DATA DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Tuning the number of ps meta info buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_TX_META_INFO_BUF_SIZE  ((sizeof(ps_tx_meta_info_type) + 3) & ~3)
#define PS_RX_META_INFO_BUF_SIZE  ((sizeof(ps_rx_meta_info_type) + 3) & ~3)
#define PS_META_INFO_EX_BUF_SIZE  ((sizeof(ps_meta_info_type_ex) + 3) & ~3)

#define PS_META_INFO_BUF_NUM       100
#define PS_META_INFO_BUF_HIGH_WM   80
#define PS_META_INFO_BUF_LOW_WM    20

/*----------------------------------------------------------------------------
 * Allocation of meta info to be static for LTE/HSPA+
----------------------------------------------------------------------------*/
#if ((defined(FEATURE_DATA_LTE)) || (defined(FEATURE_WCDMA_REL7)) ||         \
     (defined(FEATURE_WCDMA_HSUPA_2MS_TTI)))

#define PS_META_INFO_GET_MEM_SIZE PS_MEM_GET_TOT_SIZE
#define PS_META_INFO_POOL_INIT ps_mem_pool_init

#else /* FEATURE_DATA_LTE || FEATURE_WCDMA_REL7 */

#define PS_META_INFO_GET_MEM_SIZE PS_MEM_GET_TOT_SIZE_OPT
#define PS_META_INFO_POOL_INIT PS_MEM_POOL_INIT_OPT

#endif /* FEATURE_DATA_LTE || FEATURE_WCDMA_REL7 */

/*----------------------------------------------------------------------------
  Allocate memory to hold different ps_meta_info buffers along with ps_mem
  header
----------------------------------------------------------------------------*/
static int ps_tx_meta_info_buf_mem[PS_META_INFO_GET_MEM_SIZE(
                                                    PS_META_INFO_BUF_NUM,
                                                    PS_TX_META_INFO_BUF_SIZE)];

static int ps_rx_meta_info_buf_mem[PS_META_INFO_GET_MEM_SIZE(
                                                    PS_META_INFO_BUF_NUM,
                                                    PS_RX_META_INFO_BUF_SIZE)];

static int ps_meta_info_ex_buf_mem[PS_META_INFO_GET_MEM_SIZE(
                                                    PS_META_INFO_BUF_NUM,
                                                    PS_META_INFO_EX_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to specific meta_info_type_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type   * ps_tx_meta_info_buf_hdr[PS_META_INFO_BUF_NUM];
static ps_tx_meta_info_type  * ps_tx_meta_info_buf_ptr[PS_META_INFO_BUF_NUM];

static ps_mem_buf_hdr_type   * ps_rx_meta_info_buf_hdr[PS_META_INFO_BUF_NUM];
static ps_rx_meta_info_type  * ps_rx_meta_info_buf_ptr[PS_META_INFO_BUF_NUM];

static ps_mem_buf_hdr_type   * ps_meta_info_ex_buf_hdr[PS_META_INFO_BUF_NUM];
static ps_meta_info_type_ex  * ps_meta_info_ex_buf_ptr[PS_META_INFO_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_SOCK_MEM_POOL_INIT()

DESCRIPTION
  Initializes memory for sockets module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_sock_mem_pool_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pools
  -------------------------------------------------------------------------*/
  if (PS_META_INFO_POOL_INIT(PS_MEM_TX_META_INFO_TYPE,
                           ps_tx_meta_info_buf_mem,
                           PS_TX_META_INFO_BUF_SIZE,
                           PS_META_INFO_BUF_NUM,
                           PS_META_INFO_BUF_HIGH_WM,
                           PS_META_INFO_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_tx_meta_info_buf_hdr,
                           (int *) ps_tx_meta_info_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("ps_sock_mem_pool_init(): "
                    "Can't init the module");
  }

  if (PS_META_INFO_POOL_INIT(PS_MEM_RX_META_INFO_TYPE,
                           ps_rx_meta_info_buf_mem,
                           PS_RX_META_INFO_BUF_SIZE,
                           PS_META_INFO_BUF_NUM,
                           PS_META_INFO_BUF_HIGH_WM,
                           PS_META_INFO_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_rx_meta_info_buf_hdr,
                           (int *) ps_rx_meta_info_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("ps_sock_mem_pool_init(): "
                    "Can't init the module");
  }

  if (PS_META_INFO_POOL_INIT(PS_MEM_META_INFO_TYPE_EX,
                           ps_meta_info_ex_buf_mem,
                           PS_META_INFO_EX_BUF_SIZE,
                           PS_META_INFO_BUF_NUM,
                           PS_META_INFO_BUF_HIGH_WM,
                           PS_META_INFO_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_meta_info_ex_buf_hdr,
                           (int *) ps_meta_info_ex_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("ps_sock_mem_pool_init(): "
                    "Can't init the module");
  }

} /* ps_sock_mem_pool_init() */

