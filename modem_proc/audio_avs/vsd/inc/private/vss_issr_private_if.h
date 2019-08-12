#ifndef __VSS_ISSR_PRIVATE_IF_H__
#define __VSS_ISSR_PRIVATE_IF_H__

/*
   Copyright (C) 2013 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/private/vss_issr_private_if.h#1 $
   $Author: mplcsds1 $
*/

#include "apr_comdef.h"

/****************************************************************************
 * CVD SUBSYSTEM RESTART (SSR) APIS                                         *
 ****************************************************************************/

/**
  Command issued to a CVD module to request cleanup due to a particular
  subsystem being restarted.
 
  @par Payload
  #vss_issr_cmd_cleanup_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  The dst_addr in the APR command must be set to a CVD module address and the 
  dst_port must be set to APR_NULL_V.
*/
#define VSS_ISSR_CMD_CLEANUP ( 0x00012ECB )

/** @brief Type definition for #vss_issr_cmd_cleanup_t.
*/
typedef struct vss_issr_cmd_cleanup_t vss_issr_cmd_cleanup_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISSR_CMD_CLEANUP command.
*/
struct vss_issr_cmd_cleanup_t
{
  uint8_t domain_id;
    /**< The APR Domain ID of the subsystem that is being restarted. */
}
#include "apr_pack_end.h"
;

#endif /* __VSS_ISSR_PRIVATE_IF_H__ */

