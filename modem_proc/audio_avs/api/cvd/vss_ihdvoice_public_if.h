#ifndef __VSS_IHDVOICE_PUBLIC_IF_H__
#define __VSS_IHDVOICE_PUBLIC_IF_H__

/**
  @file vss_ihdvoice_public_if.h
  @brief This file contains the API definitions for HD Voice features.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CVD_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the 
      vss_ihdvoice_public_if group description in the CVD_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (C) 2014 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

#include "apr_comdef.h"

/** @addtogroup vss_ihdvoice_public_if
@{ */

/****************************************************************************
 * HD Voice APIS                                    *
 ****************************************************************************/

/**
  This command allows the client to enable HD Voice for a voice 
  session.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/

#define VSS_IHDVOICE_CMD_ENABLE ( 0x000130A2 )

  
/**
  This command allows the client to disable HD Voice for a voice 
  session.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/

#define VSS_IHDVOICE_CMD_DISABLE ( 0x000130A3 )

#endif /* __VSS_IHDVOICE_PUBLIC_IF_H__ */

