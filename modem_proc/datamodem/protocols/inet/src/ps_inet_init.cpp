/*==========================================================================*/
/*!
  @file ps_inet_init.c
  @brief
  This file provides functions that are used to perform initializations
  of INET module.

  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_inet_init.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2012-09-20  dvk Made changes for accessing tcp_ack_prio_enabled global 
                  variable from a accessor function.
  2011-09-14  sm  Added TCP ACK PRIO NV item check for dynamic enablement
                  of ACK tagging.
  2009-07-14  hm  Created module.

===========================================================================*/

/*===========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#include "ps_inet_init.h"
#include "ps_inet_event_mgr.h"
#include "ps_inet_mem_pool.h"
#include "dss_ps.h"
#include "ds_Utils_DebugMsg.h"

extern "C"
{
  #include "ps_pkt_info_utils.h"
  #include "fs_lib.h"
  #include "dhcp.h"
  #include "mcfg_fs.h"
}


/*-------------------------------------------------------------------------
    Macros for TCP ACK PRIO NV item read
  -------------------------------------------------------------------------*/
#define  TCP_ACK_PRIO_NV_PATH "/nv/item_files/modem/data/protocols/tcp_ack_prio"

/*===========================================================================

                       EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
extern "C" void ps_inet_powerup
(
  void
)
{
  uint8 tcp_ack_prio_eb = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize inet memory
  -------------------------------------------------------------------------*/
  ps_inet_mem_pool_init();

  dssps_powerup_init();

  /*------------------------------------------------------------------------
    Set the global var tcp_ack_prio_nv_val to NV item val tcp_ack_priority
  -------------------------------------------------------------------------*/
  if(mcfg_fs_read(TCP_ACK_PRIO_NV_PATH, 
                  &tcp_ack_prio_eb, 
                  sizeof(uint8), 
                  MCFG_FS_TYPE_EFS,
                  MCFG_FS_SUBID_NONE) 
     != MCFG_FS_STATUS_OK)
  {
    /* By Default tcp_ack_prio_enabled variable is set to one (Enabled) */
    LOG_MSG_INFO1_1(
        "Bydefault tcp_ack_prio_enabled is set to 1, tcp_ack_prio_enabled=%d",
         ps_pkt_info_get_tcp_ack_prio_enabled());
  }
  else
  {
    ps_pkt_info_set_tcp_ack_prio_enabled(tcp_ack_prio_eb);
  }
  return;
} /* ps_inet_powerup() */

extern "C" void ps_inet_powerdown
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  // TODO need MemPoolDeinit()
}

extern "C" void ps_inet_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dssps_init();

  ps_inet_event_mgr_init();

  dhcp_server_ipfltr_init();
  
  dhcp6_server_ipfltr_init();

  return;
} /* ps_inet_init() */

extern "C" void ps_inet_deinit
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dhcp_server_ipfltr_deinit();
  
  dhcp6_server_ipfltr_deinit();
}

