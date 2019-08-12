#ifndef __VSS_IAVTIMER_PUBLIC_IF_H__
#define __VSS_IAVTIMER_PUBLIC_IF_H__

/**
  @file vss_iavtimer_public_if.h
  @brief This file contains the APR API definitions for the CVD supported
  AVTimer interfaces.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_iavtimer_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14    sw     (Tech Pubs) Updated with Doxygen comments from 2.2.

===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * AVTimer APIS                                                             *
 ****************************************************************************/

/** @ingroup cvd_avtimer_cmd_get_time
  Gets the current AVTimer timestamp in microseconds.

  @apr_msgpayload
  None.

  @detdesc
  The dst_addr in the APR command must be set to the MVM address, and the
  dst_port must be set to APR_NULL_V.
  @par
  The client is responsible for handling any unsigned 64-bit overflow of the
  AVTimer timestamp.
  @par
  There are inherent delays associated with receiving, processing, and
  responding to this command.

  @return
  #VSS_IAVTIMER_RSP_GET_TIME -- Success.
  @par
  APRV2_IBASIC_RSP_RESULT -- Failure (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  The client must start the MVM service to have a valid AVTimer value.
*/ /* Q2 = Async Packet Router API for MDM9x15 Interface Spec (80-N1463-2)
*/
#define VSS_IAVTIMER_CMD_GET_TIME ( 0x00012EF0 )


/** @ingroup cvd_avtimer_rsp_get_time
  Response to the #VSS_IAVTIMER_CMD_GET_TIME command.

  @apr_msgpayload{vss_iavtimer_rsp_get_time_t}
  @wktable{weak__vss__iavtimer__rsp__get__time__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IAVTIMER_RSP_GET_TIME ( 0x00012EF1 )

/* Type definition for vss_iavtimer_rsp_get_time_t. */
typedef struct vss_iavtimer_rsp_get_time_t vss_iavtimer_rsp_get_time_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_iavtimer_rsp_get_time_t
@{ */
/* Payload structure for the VSS_IAVTIMER_RSP_GET_TIME command
    response.
*/
struct vss_iavtimer_rsp_get_time_t
{
  uint64_t timestamp_us;    /**< AVTimer timestamp in microseconds. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_iavtimer_rsp_get_time_t */


#endif /* __VSS_IAVTIMER_PUBLIC_IF_H__ */
