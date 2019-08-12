/*==========================================================================*/
/*!
  @file
  ds_Utils_MemManager.cpp

  @brief
  This file provides methods to initialize the ds::Utils PS Mem pools during
  powerup. It also exports a wrapper on top of ps_mem_get_buf() API.

  @see ps_mem_get_buf()
  @see ps_mem_pool_init()
  @see ds_Utils_MemManager.h

            Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_MemManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-06-30 hm  Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ps_mem.h"
#include "dss_config.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_MemConfig.h"
#include "ds_Utils_MemManager.h"
#include "ds_Utils_Signal.h"
#include "ds_Utils_SignalCtl.h"
#include "ds_Utils_SignalBus.h"
#include "ds_Utils_SignalFactory.h"
#include "ds_Utils_CritSect.h"

using namespace ds::Utils;


/*---------------------------------------------------------------------------
  Macros for sizes of objects of these classes.
---------------------------------------------------------------------------*/
#define SIGNAL_SIZE               ((sizeof(Signal) + 3) & ~3)
#define SIGNAL_CTL_SIZE           ((sizeof(SignalCtl) + 3) & ~3)
#define SIGNAL_BUS_SIZE           ((sizeof(SignalBus) + 3) & ~3)
#define SIGNAL_FACTORY_SIZE       ((sizeof(SignalFactory) + 3) & ~3)
#define CRIT_SECT_BUF_SIZE        ((sizeof(CritSect) + 3) & ~3)

/*---------------------------------------------------------------------------
  Macros for number of buffers needed, high and low watermarks.
  These are valid for both high end and low end targets.
---------------------------------------------------------------------------*/
#define SIGNAL_NUM_BUFS                (MAX_SIGNAL_OBJS)
#define SIGNAL_CTL_NUM_BUFS            (MAX_SIGNAL_OBJS)
#define SIGNAL_BUS_NUM_BUFS_APPS       (MAX_SIGNAL_BUS_PER_APP * DSS_MAX_APPS)
#define SIGNAL_BUS_NUM_BUFS_SOCKS      (ON_DEMAND_SIGNAL_BUS_PER_SOCKET * DSS_MAX_SOCKS)
#define SIGNAL_BUS_NUM_BUFS            (SIGNAL_BUS_NUM_BUFS_APPS + SIGNAL_BUS_NUM_BUFS_SOCKS)
#define SIGNAL_FACTORY_NUM_BUFS        (1)
#define CRIT_SECT_INITIAL              (50) // critical sections used prior to starting any app
//One critical section is needed for SignalBus::Strobe.
//In this case the list in the signal bus is copied and one critical section is used for that purpose.
#define CRIT_SECT_SB_STROBE            (1)
#define CRIT_SECT_NUM_BUFS             (CRIT_SECT_INITIAL + SIGNAL_BUS_NUM_BUFS + CRIT_SECT_SB_STROBE + CRIT_SECT_EXTRA_SOCKET_TOTAL)

#define SIGNAL_HIGH_WM                 (SIGNAL_NUM_BUFS - 5)
#define SIGNAL_CTL_HIGH_WM             (SIGNAL_CTL_NUM_BUFS - 5)
#define SIGNAL_BUS_HIGH_WM             (SIGNAL_BUS_NUM_BUFS - 5)
#define SIGNAL_FACTORY_HIGH_WM         (1)
#define CRIT_SECT_HIGH_WM              (800)

#define SIGNAL_LOW_WM                  (5)
#define SIGNAL_CTL_LOW_WM              (5)
#define SIGNAL_BUS_LOW_WM              (2)
#define SIGNAL_FACTORY_LOW_WM          (0)
#define CRIT_SECT_LOW_WM               (200)

/*---------------------------------------------------------------------------
  Allocate memory to hold different ds Net objects along with ps_mem header.
---------------------------------------------------------------------------*/

static int signal_buf[PS_MEM_GET_TOT_SIZE_OPT(SIGNAL_NUM_BUFS,
                                              SIGNAL_SIZE)];
static int signal_ctl_buf[PS_MEM_GET_TOT_SIZE_OPT(SIGNAL_CTL_NUM_BUFS,
                                                  SIGNAL_CTL_SIZE)];
static int signal_bus_buf[PS_MEM_GET_TOT_SIZE_OPT(SIGNAL_BUS_NUM_BUFS,
                                                  SIGNAL_BUS_SIZE)];
static int signal_factory_buf[PS_MEM_GET_TOT_SIZE(SIGNAL_FACTORY_NUM_BUFS,
                                                  SIGNAL_FACTORY_SIZE)];
static int crit_sect_buf[PS_MEM_GET_TOT_SIZE_OPT (CRIT_SECT_NUM_BUFS,
                                                  CRIT_SECT_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*---------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to actual object array.
---------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type   * signal_buf_hdr_ptr[SIGNAL_NUM_BUFS];
static Signal                * signal_buf_ptr[SIGNAL_NUM_BUFS];

static ps_mem_buf_hdr_type   * signal_ctl_buf_hdr_ptr[SIGNAL_CTL_NUM_BUFS];
static SignalCtl             * signal_ctl_buf_ptr[SIGNAL_CTL_NUM_BUFS];

static ps_mem_buf_hdr_type   * signal_bus_buf_hdr_ptr[SIGNAL_BUS_NUM_BUFS];
static SignalBus             * signal_bus_buf_ptr[SIGNAL_BUS_NUM_BUFS];

static ps_mem_buf_hdr_type   * signal_factory_buf_hdr_ptr[SIGNAL_FACTORY_NUM_BUFS];
static SignalFactory         * signal_factory_buf_ptr[SIGNAL_FACTORY_NUM_BUFS];

static ps_mem_buf_hdr_type   * crit_sect_buf_hdr_ptr[CRIT_SECT_NUM_BUFS];
static SignalBus             * crit_sect_buf_ptr[CRIT_SECT_NUM_BUFS];

#endif /* FEATURE_DATA_PS_MEM_DEBUG */


void MemoryManager::MemPoolInit
(
  void
)
throw()
{
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO1_0 ("MemoryManager::MemPoolInit(): "
                   "Initing mem pools for DSUtils");

  /*-------------------------------------------------------------------------
    Initialize DSUtils mem pools
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_UTILS_SIGNAL,
                           signal_buf,
                           SIGNAL_SIZE,
                           SIGNAL_NUM_BUFS,
                           SIGNAL_HIGH_WM,
                           SIGNAL_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) signal_buf_hdr_ptr,
                           (int *) signal_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }


  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_UTILS_SIGNAL_CTL,
                           signal_ctl_buf,
                           SIGNAL_CTL_SIZE,
                           SIGNAL_CTL_NUM_BUFS,
                           SIGNAL_CTL_HIGH_WM,
                           SIGNAL_CTL_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) signal_ctl_buf_hdr_ptr,
                           (int *) signal_ctl_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_UTILS_SIGNAL_BUS,
                           signal_bus_buf,
                           SIGNAL_BUS_SIZE,
                           SIGNAL_BUS_NUM_BUFS,
                           SIGNAL_BUS_HIGH_WM,
                           SIGNAL_BUS_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) signal_bus_buf_hdr_ptr,
                           (int *) signal_bus_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (ps_mem_pool_init(PS_MEM_DS_UTILS_SIGNAL_FACTORY,
                       signal_factory_buf,
                       SIGNAL_FACTORY_SIZE,
                       SIGNAL_FACTORY_NUM_BUFS,
                       SIGNAL_FACTORY_HIGH_WM,
                       SIGNAL_FACTORY_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) signal_factory_buf_hdr_ptr,
                       (int *) signal_factory_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_UTILS_CRIT_SECT,
                           crit_sect_buf,
                           CRIT_SECT_BUF_SIZE,
                           CRIT_SECT_NUM_BUFS,
                           CRIT_SECT_HIGH_WM,
                           CRIT_SECT_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) crit_sect_buf_hdr_ptr,
                           (int *) crit_sect_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  return;

bail:
  DATA_ERR_FATAL("MemoryManager::MemPoolInit(): Cant init DSUTILS modules");
  return;

} /* MemPoolInit() */
