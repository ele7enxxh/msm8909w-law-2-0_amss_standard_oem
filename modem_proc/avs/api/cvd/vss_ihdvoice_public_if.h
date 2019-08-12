#ifndef __VSS_IHDVOICE_PUBLIC_IF_H__
#define __VSS_IHDVOICE_PUBLIC_IF_H__

/**
  @file vss_ihdvoice_public_if.h
  @brief This file contains the API definitions for HD Voice features.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR FILE

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/28/14    sw     (Tech Pubs) Edited Doxygen comments for 2.6.
===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * HD Voice APIS                                                            *
 ****************************************************************************/

/** @ingroup cvd_hdvoice_cmd_enable
  Enables HD Voice for a voice session.

  @apr_msgpayload
  None.

  @detdesc
  The client must set dst_addr in the APR command to the MVM address.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_IHDVOICE_CMD_ENABLE ( 0x000130A2 )

  
/** @ingroup cvd_hdvoice_cmd_disable
  Disables HD Voice for a voice session.

  @apr_msgpayload
  None.

  @detdesc
  The client must set dst_addr in the APR command to the MVM address.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_IHDVOICE_CMD_DISABLE ( 0x000130A3 )


#endif /* __VSS_IHDVOICE_PUBLIC_IF_H__ */

