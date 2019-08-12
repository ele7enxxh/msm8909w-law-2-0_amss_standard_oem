/******************************************************************************

                        QMI_IP_MAIN.C

******************************************************************************/

/******************************************************************************

  @file    qmi_ip_main.c
  @brief   QMI over IP

  DESCRIPTION


  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
05/19/14   sb         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "qmi_ip.h"

/*===========================================================================
                              VARIABLE DECLARATIONS
===========================================================================*/

static qmi_ip_conf_t  qmi_ip_cfg;

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

/*=============================================================================

FUNCTION QMI_IP_SOCKET_LISTENER()

DESCRIPTION

  - select function listening to sockets.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/

int qmi_ip_socket_listener()
{
  int            i,ret;

/*-------------------------------------------------------------------------*/

  while(TRUE)
  {
/*---------------------------------------------------------------------------
  Set all the FDs
----------------------------------------------------------------------------*/
    for(i = 0; i < qmi_ip_cfg.socket_info.num_of_fds; i++ )
    {
      FD_SET(qmi_ip_cfg.socket_info.sk_conf[i].socket_fd, &(qmi_ip_cfg.socket_info.fdset));
    }

/*---------------------------------------------------------------------------
  -Call select
  -Check if FD is set
  -If yes call associated read function
  -Clear FDs
----------------------------------------------------------------------------*/
    if((ret = select(qmi_ip_cfg.socket_info.max_fd+1,
                     &(qmi_ip_cfg.socket_info.fdset),
                     NULL,
                     NULL,
                     NULL)) < 0)
    {
      LOG_MSG_ERROR("qmi_ip select failed", 0, 0, 0);
    }
    else
    {
      for(i = 0; i < qmi_ip_cfg.socket_info.num_of_fds; i++ )
      {
        if(FD_ISSET(qmi_ip_cfg.socket_info.sk_conf[i].socket_fd,
                     &(qmi_ip_cfg.socket_info.fdset)))
        {
          if(qmi_ip_cfg.socket_info.sk_conf[i].read_ex)
          {
            LOG_MSG_INFO1 ("select() returned for fd %d", qmi_ip_cfg.socket_info.sk_conf[i].socket_fd, 0, 0);
            if( SUCCESS != ((qmi_ip_cfg.socket_info.sk_conf[i].read_ex)(qmi_ip_cfg.socket_info.sk_conf[i].socket_fd)) )
            {
              LOG_MSG_ERROR("Error on read callback[%d] fd=%d",
                            i,
                            qmi_ip_cfg.socket_info.sk_conf[i].socket_fd,
                            0);
              FD_CLR(qmi_ip_cfg.socket_info.sk_conf[i].socket_fd, &(qmi_ip_cfg.socket_info.fdset));
            }
            else
            {
              FD_CLR(qmi_ip_cfg.socket_info.sk_conf[i].socket_fd, &(qmi_ip_cfg.socket_info.fdset));
            }
          }
          else
          {
            LOG_MSG_ERROR("No read function",0,0,0);
          }
        }
      }
    }
  }
  return SUCCESS;
}

/*=============================================================================

FUNCTION SIGHANDLER()

DESCRIPTION

  - handles signals to QMI_IP module.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void sighandler(int signal)
{
  switch (signal)
  {
/*---------------------------------------------------------------------------
  Handle termination signals
----------------------------------------------------------------------------*/
    case SIGTERM:
    case SIGINT:
      qmi_ip_qmi_client_release();
      memset(&qmi_ip_cfg, 0, sizeof(qmi_ip_conf_t));
      ds_system_call("pkill -SIGTERM eMBMs_TunnelingModule",
                     strlen("pkill -SIGTERM eMBMs_TunnelingModule"));
      exit(0);
      break;

    default: LOG_MSG_INFO1("Received unexpected signal %d",signal,0,0);
      break;
  }
}


/*=============================================================================

FUNCTION MAIN()

DESCRIPTION

  The main function for QMI_IP which is first called when QMI_IP gets
  started on boot up.
  This function:
  - initializes the server which listens to TCP connections from LAN clients.
  - initializes the QMI handling module
  - initializes the tunneling module interface.
  - initializes the FPOP handler module.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/

int main(int argc, char *argv[])
{

  int                      ret_val;

/*---------------------------------------------------------------------------*/
  if (TRUE != Diag_LSM_Init(NULL))
  {
     printf("Diag_LSM_Init failed !!");
  }

  memset(&qmi_ip_cfg, 0, sizeof(qmi_ip_conf_t));

/*---------------------------------------------------------------------------
  Reads XML file
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_xml_init(&qmi_ip_cfg, argv[1]);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to initialize XML info", 0, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Exit if right mode is not set. Also terminate tunneling module.
----------------------------------------------------------------------------*/
  if (qmi_ip_cfg.mode == INACTIVE_MODE )
  {
    LOG_MSG_ERROR("QMI IP mode not supported %d, exiting", qmi_ip_cfg.mode, 0, 0);
    ds_system_call("pkill -SIGTERM eMBMs_TunnelingModule",
                strlen("pkill -SIGTERM eMBMs_TunnelingModule"));
    exit(0);
  }

/*---------------------------------------------------------------------------
  Initialize server listening to client connections
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_init_master_conn_server(&qmi_ip_cfg);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to initialize TCP control server", 0, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Initialize indication socket
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_ind_hdlr_init();
  if(ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to init QMI IP indication handler", 0, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Initialize QMI message handler
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_msg_hdlr_init(&qmi_ip_cfg);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to init QMI IP message handler", 0, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Initialize tunneling module interface
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_tun_mgmt_init(&qmi_ip_cfg);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to init QMI IP tunnel mgmt module", 0, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Initialize FPOP socket interface in multi client mode
----------------------------------------------------------------------------*/
  if (qmi_ip_cfg.mode == MULTI_CLIENT_MODE)
  {
    ret_val = qmi_ip_fpop_socket_init();
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Failed to init QMI IP FPOP socket", 0, 0, 0);
      return FAILURE;
    }

/*---------------------------------------------------------------------------
  Initialize FPOP handler interface in multi client mode
  Tunneling Module uses the same port and IP combination
  for its UDP socket, so we want to make sure that its socket
  is created first and then we go ahead and open FPOP
  sockets since kernel delivers incoming unicast UDP data
  to the socket which is opened last in sequence.
----------------------------------------------------------------------------*/
    ret_val = qmi_ip_fpop_hdlr_init(&qmi_ip_cfg);
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Failed to init QMI IP FPOP handler module", 0, 0, 0);
      return FAILURE;
    }
  }

/*---------------------------------------------------------------------------
  Start listening to socket connections
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_socket_listener();
  if (ret_val != FAILURE)
  {
    LOG_MSG_ERROR("QMI listener failed", 0, 0, 0);
    return FAILURE;
  }
/*---------------------------------------------------------------------------
  Clean up
----------------------------------------------------------------------------*/
  qmi_ip_qmi_client_release();
  memset(&qmi_ip_cfg, 0, sizeof(qmi_ip_conf_t));
  ds_system_call("pkill -SIGTERM eMBMs_TunnelingModule",
                 strlen("pkill -SIGTERM eMBMs_TunnelingModule"));
  qmi_ip_netmgr_release();

  return SUCCESS;
}

