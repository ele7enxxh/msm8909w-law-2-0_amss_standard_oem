/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diag I/O Configuration

General Description

Copyright (c) 2012-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/platform/bmp/src/diagcomm_cfg.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
08/08/13   sg      Command/response separation

===========================================================================*/

#include "diagcomm_io.h"
#include "stdio.h"               /* For snprintf */


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
#define DIAG_EDGE_APPS_RIVA_DATA     "APPS_RIVA_DATA"
#define DIAG_EDGE_APPS_RIVA_CTRL     "APPS_RIVA_CTRL"
#define DIAG_EDGE_APPS_QDSP_DATA     "DIAG"    
#define DIAG_EDGE_APPS_QDSP_CTRL     "DIAG_CNTL"
#define DIAG_EDGE_APPS_MODEM_CMD     "DIAG_CMD"
#define DIAG_EDGE_APPS_MODEM_DCI_CMD     "DIAG_2_CMD"


diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; 

/*===========================================================================

FUNCTION DIAGCOMM_IO_INIT

DESCRIPTION
  Initilizes Diag variables for interprocess communication and I/O
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_io_init( void )
{
  uint8 i, j = 0;
  
  for( i=0; i < NUM_SMD_PORT_TYPES; i++ )
  {
    for( j=0; j < NUM_SMD_PORTS; j++ )
    {
      (void)memset( &diagcomm_io_conn[i][j], 0, sizeof(diagcomm_io_conn_type) );
      (void)memset( &diagcomm_io_ctrl_conn[i][j], 0, sizeof(diagcomm_io_conn_type) );
      (void)memset( &diagcomm_io_cmd_conn[i][j], 0, sizeof(diagcomm_io_conn_type) );
	  
      diagcomm_io_conn[i][j].port_num = j;
      diagcomm_io_conn[i][j].connected = FALSE;
      diagcomm_io_conn[i][j].channel_connected = FALSE;
      diagcomm_io_conn[i][j].data_pending = FALSE;
      diagcomm_io_conn[i][j].io_type = DIAGCOMM_IO_TYPE_INVALID;
      diagcomm_io_conn[i][j].channel_type = DIAGCOMM_IO_DATA;
      diagcomm_io_conn[DIAGCOMM_PORT_SMD][j].port_type = DIAGCOMM_PORT_SMD;
      diagcomm_io_conn[DIAGCOMM_PORT_SIO][j].port_type = DIAGCOMM_PORT_SIO;
      
      diagcomm_io_ctrl_conn[i][j].port_num = j;
      diagcomm_io_ctrl_conn[i][j].connected = FALSE;
      diagcomm_io_ctrl_conn[i][j].channel_connected = FALSE;
      diagcomm_io_ctrl_conn[i][j].data_pending = FALSE;
      diagcomm_io_ctrl_conn[i][j].io_type = DIAGCOMM_IO_TYPE_INVALID;
      diagcomm_io_ctrl_conn[i][j].channel_type = DIAGCOMM_IO_CTRL;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][j].port_type = DIAGCOMM_PORT_SMD;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][j].port_type = DIAGCOMM_PORT_SIO;

      diagcomm_io_cmd_conn[i][j].port_num = j;
      diagcomm_io_cmd_conn[i][j].connected = FALSE;
      diagcomm_io_cmd_conn[i][j].channel_connected = FALSE;
      diagcomm_io_cmd_conn[i][j].data_pending = FALSE;
      diagcomm_io_cmd_conn[i][j].io_type = DIAGCOMM_IO_TYPE_INVALID;
      diagcomm_io_cmd_conn[i][j].channel_type = DIAGCOMM_IO_CMD;
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][j].port_type = DIAGCOMM_PORT_SMD;
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][j].port_type = DIAGCOMM_PORT_SIO;
    }
  }
 
     
#if defined(DIAG_MP)
  //Use SMD/SIO on Data & Ctrl path from Apps to Modem.
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].io_type = DIAGCOMM_IO_TYPE_SIO;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].handle.sio_handle = SIO_NO_STREAM_ID;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = SIO_PORT_NULL;
  
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].io_type = DIAGCOMM_IO_TYPE_SIO;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].handle.sio_handle = SIO_NO_STREAM_ID;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = SIO_PORT_NULL;
  
  //Always use SMDL on cmd/rsp path
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].io_type = DIAGCOMM_IO_TYPE_SMDL;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].handle.smdl_handle = NULL;
  #if defined(DIAG_IMAGE_MODEM_PROC)
     diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.smdl_params.edge = SMD_APPS_MODEM;
  #elif defined(DIAG_IMAGE_RIVA_PROC)
    diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.smdl_params.edge = SMD_APPS_RIVA;
  #elif defined(DIAG_QDSP6_APPS_PROC)
    diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.smdl_params.edge = SMD_APPS_QDSP;
  #endif
  (void)snprintf( diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.smdl_params.name,
                  DIAG_SMDL_CHANNEL_NAME_SIZE_MAX, DIAG_EDGE_APPS_MODEM_CMD );
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].io_type = DIAGCOMM_IO_TYPE_SMDL;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].handle.smdl_handle = NULL;
  #if defined(DIAG_IMAGE_MODEM_PROC)
     diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].open_params.smdl_params.edge = SMD_APPS_MODEM;
  #elif defined(DIAG_IMAGE_RIVA_PROC)
    diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].open_params.smdl_params.edge = SMD_APPS_RIVA;
  #elif defined(DIAG_QDSP6_APPS_PROC)
    diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].open_params.smdl_params.edge = SMD_APPS_QDSP;
  #endif
  (void)snprintf( diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].open_params.smdl_params.name,
                  DIAG_SMDL_CHANNEL_NAME_SIZE_MAX, DIAG_EDGE_APPS_MODEM_DCI_CMD );
#else
  //Always use SIO to USB/UART
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].io_type = DIAGCOMM_IO_TYPE_SIO;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].handle.sio_handle = SIO_NO_STREAM_ID;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = SIO_PORT_NULL;
#endif
  /*Set up the DCI ports */
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].io_type = DIAGCOMM_IO_TYPE_SIO;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].handle.sio_handle = SIO_NO_STREAM_ID;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].open_params.sio_params.params.port_id = SIO_PORT_NULL;
		
} /* diagcomm_io_init */
