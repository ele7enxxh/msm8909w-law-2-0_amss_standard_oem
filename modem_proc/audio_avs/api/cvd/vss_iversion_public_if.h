#ifndef __VSS_IVERSION_PUBLIC_IF_H__
#define __VSS_IVERSION_PUBLIC_IF_H__

/**
  @file vss_iversion_public_if.h
  @brief This file contains the APR API definitions for Core Voice Driver
  version information.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CVD_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_iversion_public_if 
      group description in the CVD_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (c) 2014 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary. 
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_iversion_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/


#include "apr_comdef.h"


/** @addtogroup vss_iversion_public_if
@{ */

/****************************************************************************
 * CVD VERSION APIS                                                         *
 ****************************************************************************/

/**
  Retrieves the CVD version string.
 
  @par Payload
  None.
 
  @return
  #VSS_IVERSION_RSP_GET -- In case of success. \n.
  #APRV2_IBASIC_RSP_RESULT -- In case of failure.

  @dependencies
  None.

  @comments
  The dst_addr in the APR command must be set to the MVM address and the dst_port
  must be set to APR_NULL_V.
*/
#define VSS_IVERSION_CMD_GET ( 0x00011378 )


/**
  Response to the #VSS_IVERSION_CMD_GET command, containing the CVD version
  string.
 
  @par Payload
  #vss_iversion_rsp_get_t

  @dependencies
  None.
*/
#define VSS_IVERSION_RSP_GET ( 0x00011379 )

/** @brief Type definition for #vss_iversion_rsp_get_t.
*/
typedef struct vss_iversion_rsp_get_t vss_iversion_rsp_get_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVERSION_RSP_GET command response.
*/
struct vss_iversion_rsp_get_t
{
  char_t version[31];
    /**< NULL-terminated version string. */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_iversion_public_if */

#endif /* __VSS_IVERSION_PUBLIC_IF_H__ */

