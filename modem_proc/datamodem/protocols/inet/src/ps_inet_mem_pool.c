/*===========================================================================

@file: ps_inet_mem_pool.c

DESCRIPTION
 The protocol services inet memory pool manager file. Contains definitions 
 and initializations for inet memory pools.

EXTERNAL FUNCTIONS
  INET_MEM_POOL_INIT()
    Initialzes memory pools for inet module.

Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                             EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Note that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_inet_mem_pool.c#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/13   rk      Removed feature mobileap.
12/12/2012 ds      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
10/21/2008 hm      Created the module
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ps_mem.h"
#include "dss_config.h"

#include "ps_tcp.h"
#include "ps_udp_io.h"
#include "ps_icmp.h"
#include "ds_Utils_DebugMsg.h"



/*---------------------------------------------------------------------------
  Tuning the number of tcp sequence block list buffers needed by this module
---------------------------------------------------------------------------*/
#define TCP_SEQUENCE_BLOCK_LIST_BUF_SIZE  ((sizeof(struct tcp_sequence_block_list) + 3) & ~3)

#ifdef FEATURE_DATA_PS_LOW_MEM_CHIPSET
#define TCP_SEQUENCE_BLOCK_LIST_BUF_NUM       30
#define TCP_SEQUENCE_BLOCK_LIST_BUF_HIGH_WM   20
#define TCP_SEQUENCE_BLOCK_LIST_BUF_LOW_WM     5

#else
#define TCP_SEQUENCE_BLOCK_LIST_BUF_NUM       100
#define TCP_SEQUENCE_BLOCK_LIST_BUF_HIGH_WM    95
#define TCP_SEQUENCE_BLOCK_LIST_BUF_LOW_WM     40

#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold tcp_sequenct_block_list_buf along with ps_mem header
----------------------------------------------------------------------------*/
static int tcp_sequence_block_list_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                           (
                                             TCP_SEQUENCE_BLOCK_LIST_BUF_NUM,
                                             TCP_SEQUENCE_BLOCK_LIST_BUF_SIZE
                                           )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to tcp_sequence_block_list_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type            * tcp_sequence_block_list_buf_hdr[TCP_SEQUENCE_BLOCK_LIST_BUF_NUM];
static struct tcp_sequence_block_list * tcp_sequence_block_list_buf_ptr[TCP_SEQUENCE_BLOCK_LIST_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */


/*---------------------------------------------------------------------------
  Tuning the number of tcb buffers needed by this module
---------------------------------------------------------------------------*/
#define TCB_BUF_SIZE  ((sizeof(struct tcb) + 3) & ~3)

#define TCB_BUF_NUM         DSS_MAX_TCBS
#define TCB_BUF_HIGH_WM     (DSS_MAX_TCBS - 1)
#define TCB_BUF_LOW_WM      1

/*----------------------------------------------------------------------------
  Allocate memory to hold tcb_buf along with ps_mem header
----------------------------------------------------------------------------*/
static int tcb_buf_mem[PS_MEM_GET_TOT_SIZE_OPT(TCB_BUF_NUM, TCB_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to tcb_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * tcb_buf_hdr[TCB_BUF_NUM];
static struct tcb_type     * tcb_buf_ptr[TCB_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of udp callback buffers needed by this module
---------------------------------------------------------------------------*/
#define UDP_CB_BUF_SIZE  ((sizeof(struct udp_cb) + 3) & ~3)

#define UDP_CB_BUF_NUM        DSS_MAX_UDP_SOCKS
#define UDP_CB_BUF_HIGH_WM    (DSS_MAX_UDP_SOCKS - 1)
#define UDP_CB_BUF_LOW_WM     1

/*----------------------------------------------------------------------------
  Allocate memory to hold udp_cb_buf along with ps_mem header
----------------------------------------------------------------------------*/
static int udp_cb_buf_mem[PS_MEM_GET_TOT_SIZE_OPT(UDP_CB_BUF_NUM, UDP_CB_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to udp_cb_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * udp_cb_buf_hdr[UDP_CB_BUF_NUM];
static struct udp_cb       * udp_cb_buf_ptr[UDP_CB_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of icmp callback buffers needed by this module
---------------------------------------------------------------------------*/
#define ICMP_CB_BUF_SIZE  ((sizeof(struct icmp_cb) + 3) & ~3)

#define ICMP_CB_BUF_NUM         DSS_MAX_ICMP_SOCKS
#define ICMP_CB_BUF_HIGH_WM     (DSS_MAX_ICMP_SOCKS - 1)
#define ICMP_CB_BUF_LOW_WM      (0)

/*----------------------------------------------------------------------------
  Allocate memory to hold icmp_cb_buf along with ps_mem header
----------------------------------------------------------------------------*/
static int icmp_cb_buf_mem[PS_MEM_GET_TOT_SIZE_OPT(ICMP_CB_BUF_NUM,
                                                   ICMP_CB_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to icmp_cb_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type        * icmp_cb_buf_hdr[ICMP_CB_BUF_NUM];
static struct icmp_cb             * icmp_cb_buf_ptr[ICMP_CB_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_INET_MEM_POOL_INIT()

DESCRIPTION
  Initializes memory for inet module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_inet_mem_pool_init
(
  void
)
{
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_TCP_SACK_BLOCKS_TYPE,
                           tcp_sequence_block_list_buf_mem,
                           TCP_SEQUENCE_BLOCK_LIST_BUF_SIZE,
                           TCP_SEQUENCE_BLOCK_LIST_BUF_NUM,
                           TCP_SEQUENCE_BLOCK_LIST_BUF_HIGH_WM,
                           TCP_SEQUENCE_BLOCK_LIST_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) tcp_sequence_block_list_buf_hdr,
                           (int *) tcp_sequence_block_list_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }


  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DSS_TCB_TYPE,
                           tcb_buf_mem,
                           TCB_BUF_SIZE,
                           TCB_BUF_NUM,
                           TCB_BUF_HIGH_WM,
                           TCB_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) tcb_buf_hdr,
                           (int *) tcb_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DSS_UCB_TYPE,
                           udp_cb_buf_mem,
                           UDP_CB_BUF_SIZE,
                           UDP_CB_BUF_NUM,
                           UDP_CB_BUF_HIGH_WM,
                           UDP_CB_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) udp_cb_buf_hdr,
                           (int *) udp_cb_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DSS_ICMB_TYPE,
                           icmp_cb_buf_mem,
                           ICMP_CB_BUF_SIZE,
                           ICMP_CB_BUF_NUM,
                           ICMP_CB_BUF_HIGH_WM,
                           ICMP_CB_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) icmp_cb_buf_hdr,
                           (int *) icmp_cb_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }

} /* ps_inet_mem_pool_init() */

