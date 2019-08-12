#ifndef __VSS_IAVTIMER_PUBLIC_IF_H__
#define __VSS_IAVTIMER_PUBLIC_IF_H__

/**
  @file vss_iavtimer_public_if.h
  @brief This file contains the APR API definitions for the CVD supported
  AVTimer interfaces.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_iavsync_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_iavtimer_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup vss_iavtimer_public_if
@{ */

/****************************************************************************
 * AVTimer APIS                                                             *
 ****************************************************************************/

/**
  Gets the current AVTimer timestamp in microseconds.

  @par Payload
  None.

  @return
  #VSS_IAVTIMER_RSP_GET_TIME -- In case of success.
  #APRV2_IBASIC_RSP_RESULT -- In case of failure.

  @dependencies
  None.

  @comments
  The dst_addr in the APR command must be set to MVM address and the dst_port
  must be set to APR_NULL_V.

  The client is responsible for handling any unsigned 64-bit overflow of the
  AVTimer timestamp.

  There are inherent delays associated with receiving, processing and
  responding of this command.
*/
#define VSS_IAVTIMER_CMD_GET_TIME ( 0x00012EF0 )


/**
  Response to the #VSS_IAVTIMER_CMD_GET_TIME command.

  @par Payload
  #vss_iavtimer_rsp_get_time_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IAVTIMER_RSP_GET_TIME ( 0x00012EF1 )

/** @brief Type definition for #vss_iavtimer_rsp_get_time_t.
*/
typedef struct vss_iavtimer_rsp_get_time_t vss_iavtimer_rsp_get_time_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IAVTIMER_RSP_GET_TIME command
    response.
*/
struct vss_iavtimer_rsp_get_time_t
{
  uint64_t timestamp_us;
    /**< AVTimer timestamp in microseconds. */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_iavtimer_public_if */

#endif /* __VSS_IAVTIMER_PUBLIC_IF_H__ */
