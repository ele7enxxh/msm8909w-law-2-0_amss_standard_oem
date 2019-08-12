#ifndef __TESTCLIENT_MSG_IF_H__
#define __TESTCLIENT_MSG_IF_H__

/*
  Copyright (C) 2011 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/apr/example/toyclient/inc/toyclient_msg_if.h#1 $
  $Author: mplcsds1 $
*/

#include "apr_comdef.h"

/*****************************************************************************
 * Messages                                                                  *
 ****************************************************************************/

#define TOYCLIENT_ITEST_CMD_START_TEST ( 0x00011302 )
  /**<
   * This command starts APR test. The command response
   * APR_EOK is returned to the client.
   */

#endif /* __TESTCLIENT_MSG_IF_H__ */

