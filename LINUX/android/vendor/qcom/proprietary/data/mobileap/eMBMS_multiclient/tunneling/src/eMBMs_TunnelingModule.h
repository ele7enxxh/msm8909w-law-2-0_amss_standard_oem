#ifndef _EMBMS_TUNNELING_MODULE_H_
#define _EMBMS_TUNNELING_MODULE_H_

/*====================================================

FILE:  eMBMs_TunnelingModule.h

SERVICES:
   eMBMs Tunneling Module Class

=====================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  04/25/14   pm         Created file
===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include "stringl.h"
#include "ds_util.h"
#include "embms_tm_api.h"

#define MAX_BUF_LEN                  256
#define READ_BUFFER_SIZE             1500
#define SOCKET_INACTIVE              16384
#define EMBMS_TM_BRIDGE_INTERFACE    "bridge0"
#define MAX(a,b) (a > b ? a : b)

/*===========================================================================
                       FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION CREATE_EMBMS_CONTROL_SOCKET()

DESCRIPTION

  This function creates the socket which is used to
  receive control information from the QMI over IP
  module

DEPENDENCIES
  None.

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_ERROR on failure

SIDE EFFECTS
  None

==========================================================================*/

int create_embms_control_socket();

/*===========================================================================

FUNCTION CREATE_UDP_SOCKET()

DESCRIPTION

  This function creates a simple udp socket

DEPENDENCIES
  None.

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_ERROR on failure

SIDE EFFECTS
  None

==========================================================================*/

int create_udp_socket(unsigned int *sockfd);

/*===========================================================================

FUNCTION ADD_CLIENT_ENTRY_TO_TABLE()

DESCRIPTION

  This function creates a TMGI entry if not otherwise present
  and adds the respective clients to it.
  If TMGI entry is already present, it just adds the client entry

DEPENDENCIES
  None.

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_Exxxxxx (Error code) on failure

SIDE EFFECTS
  None

==========================================================================*/

int add_client_entry_to_table(embms_tmgi_to_client_info_update *info_update);

/*===========================================================================

FUNCTION DELETE_CLIENT_ENTRY_FROM_TABLE()

DESCRIPTION

  This function deletes a TMGI entry or a client entry
  depending upon the information received from
  QMI over IP module

DEPENDENCIES
  None.

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_Exxxxxx (Error code) on failure

SIDE EFFECTS
  None

==========================================================================*/

int delete_client_entry_from_table(embms_tmgi_to_client_info_update *info_update);

/*===========================================================================

FUNCTION SIGHANDLER()

DESCRIPTION

  Processes signals accordingly

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void sighandler(int signal);

#endif
