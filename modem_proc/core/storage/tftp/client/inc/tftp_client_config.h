/***********************************************************************
 * tftp_client_config.h
 *
 * Short description.
 * Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose Description
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/tftp/client/inc/tftp_client_config.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-12-05   dks   Target compilation and audio compilation fixes.
2014-06-04   rp    Switch to IPCRouter sockets.
2013-12-26   rp    Add tftp-client module.
2013-11-14   rp    Create

===========================================================================*/

#ifndef __TFTP_CLIENT_CONFIG_H__
#define __TFTP_CLIENT_CONFIG_H__

#include "tftp_config_i.h"

#ifndef TFTP_CLIENT_MAX_CLIENTS
  #define TFTP_CLIENT_MAX_CLIENTS    128
#endif

#if defined (TFTP_MSM_BUILD)
  #if defined (TFTP_MPSS_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MSM_MPSS
  #elif defined (TFTP_ADSP_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MSM_ADSP
  #endif
#elif defined (TFTP_MDM_BUILD)
  #if defined (TFTP_MPSS_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MDM_MPSS
  #elif defined (TFTP_ADSP_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MDM_ADSP
  #elif defined (TFTP_SPARROW_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MDM_SPARROW
  #endif
#elif defined (TFTP_GSS_BUILD)
  #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_APQ_GSS
#else
  #error "Configure TFTP Build correctly"
#endif

#endif /* __TFTP_CLIENT_CONFIG_H__ */
