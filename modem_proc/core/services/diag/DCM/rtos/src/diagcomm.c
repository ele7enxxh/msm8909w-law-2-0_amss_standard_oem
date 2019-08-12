/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications via SIO

General Description
  The routines in this file handle the communications layer for the
  diagnostic subsystem when communicating via Socket layers. This was 
  branched off diagcomm_sio.c to handle request/response packets only.

Copyright (c) 2001-2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagcomm_sio.c_v   1.10   08 May 2002 14:37:42   ldefauw  $
 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/rtos/src/diagcomm.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush. 
03/04/11   is      Support for Diag over SMD-Lite
05/11/10   is      Removed unused sdevmap.h
                   Replace FEATURE_DATA with FEATURE_DIAG_ON_DATA_PORT.
04/29/10   is      Removed support for FEATURE_BT, FEATURE_DIAG_SUPPORT_WIRELESS, DEBUG_DIAG
02/18/10   JV      Fix klocwork errors
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
11/30/09   ps      Fixed compiler warnings.
08/05/09   JV      task.h is now included in osal.
07/31/09   JV      Brought back FEATURE_DIAG_NV
07/10/09   as      Mainline code under FEATURE_DIAG_NV
07/06/09   as      Removed support for FEATURE_DATA_SERIALIZER
06/11/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   vk      changed includes from external to internal headers
10/31/05   as      Fixed lint errors.
09/09/05   sl      Added support for multiple tx/rx queues for the case of
                   FEATURE_DIAG_MULTI_PORTS
05/15/05   as      Added support for FEATURE_DATA_SERIALIZER
03/28/05   sl      Added an array of watermark queues for the multi port feature
03/24/05   as      Added check in UART/USB notify functions to check if the
                   connected accessory detected by AAM is UART and USB
03/05/05   sl      Corrected BT Featurization for FEATURE_DIAG_MULTI_PORTS
02/20/05   sl      Added support for DIAG over BT via RDM under
                   FEATURE_DIAG_MULTI_PORTS
02/12/05   sl      Fixed a bug that caused the close/open port bit masks
                   not being cleared
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag  
01/25/04   sl      Added support for callback notifications from Accessory
                   Attachment Manager (AAM) for USB and UART cable attach and
                   detach under FEATURE_ACCESSORY_ATTACHMENT_MANAGER.
12/14/04   sl      Added support for DIAG to hold multiple SIO ports
                   controlled by FEATURE_DIAG_MULTI_PORTS
04/20/04   pn      Added support for DIAG over Bluetooth.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
09/29/03   dna     Get T_QUASAR to compile and work again for MSM6500
08/14/03   vsk     DSM 2.0 changes 
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
05/08/02   lad     Moved dsm_simple_enqueue_isr() call above diagcomm_sio_cb
                   to enqueue data prior to potential context switch in cb.
02/27/02   dwp     Additional changes to use new DSM interface functions for
                   registering/handling memory events.
02/19/02   lad     Made changes to use new DSM interface functions for
                   registering/handling memory events.
01/21/02   as      Changed nv_sio_baud_type to nv_sio_baudrate_type
11/06/01   abp     Added diagcomm_flush_tx() to flush SIO transmit channel
10/30/01   abp     Added diagcomm_sio_close_to_open_cb().
                   In diagcomm_sio_close_done_cb() callback function ptr is
                   checked for NULL.
                   In diagcomm_open() added mechanism to close connection
                   before opening a new connection
10/09/01   ttl     Added second parameter to diagcomm_sio_tx_start_cb() and
                   diagcomm_sio_tx_stop_cb() due the DSM memory pool registration
                   changes.
09/17/01   jal     Fixed diagcomm_close() to notify RDM of completion
                   properly
05/07/01   lad     Cleaned up file.
04/17/01   dwp     Initialize diagcomm_sio_open_params.port_id to NULL. RDM
                   will instruct DIAG as to what port to open.
04/06/01   lad     Introduced types for callback function pointers.
                   Cleaned up FEATURE_RUNTIME_DEVMAP support.
                   Updated watermark settings (this may be changed to save RAM).
02/23/01   lad     Created file.

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#include "assert.h"

#include "diag_v.h"
#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagcomm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "msg.h"
#include "diagcomm_io.h"

#include "dsm.h"

#if defined (FEATURE_DIAG_NV)
  #include "diagnv_v.h"
  #include "nv.h"
#endif

#if defined (DIAG_SIO_SUPPORT)
  #include "rdevmap.h"
#endif

#include "osal.h"

extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */

/*lint -save -e751 */
/* Supress local typedef 'diag_config_comm_enum_type' not referenced */


/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */
#ifdef FEATURE_DIAG_ON_DATA_PORT

/* $QDCMG can let DIAG grab DS's port and QXDM's cmd to_data_mode can change
   them back. The port change is called port dancing. Here is the different 
   state of port dancing
*/
typedef enum {
  DIAGCOMM_DANCING_IDLE,        /* idle state                           */
  DIAGCOMM_DANCING_IN_DANCE,    /* in dancing, $QCDMG has succeeded     */
  DIAGCOMM_DANCING_QCDMG_OPENING,
                                /* DIAG has asked RDM to open DS's port 
                                   for $QCDMG                           */
  DIAGCOMM_DANCING_TDM_OPENING_DS,
                                /* DIAG has asked RDM to open DS's 
                                   port for to_data_mode                */
  DIAGCOMM_DANCING_TDM_OPENING_DIAG
                                /* DIAG has asked RDM to open its 
                                   old port for to_data_mode            */
} diagcomm_dancing_state_e_type;

/* Type of dancing state with useful infos                              */
typedef struct {
  diagcomm_dancing_state_e_type  state; /* dancing state                */
  rdm_device_enum_type  ds_old_dev;     /* DS's old port before dance   */
  rdm_device_enum_type  diag_old_dev;   /* DIAG's old port before dance */
} diagcomm_dancing_state_type;

/* contains rdm_assign_port_cb parameter        */
typedef struct {
  /* if the numbers are valid now               */
  boolean                       valid;

  /* status of rdm_assign_port                  */
  rdm_assign_status_type        status;

  /* the service requesting the assignment       */
  rdm_service_enum_type         srv_requester;

  /* the desired port for the srv_requester     */
  rdm_device_enum_type          dev_desired;
} diagcomm_dancing_assign_port_cb_param_type;

/* port dancing state                           */
static diagcomm_dancing_state_type diagcomm_dancing_state = {
  DIAGCOMM_DANCING_IDLE,        /* state        */
  RDM_NULL_DEV,                 /* ds_old_dev   */
  RDM_NULL_DEV                  /* diag_old_dev */
};

static diagcomm_dancing_assign_port_cb_param_type 
diagcomm_dancing_assign_port_cb_params = {
  FALSE,        /* not valid                */
};

#endif /* FEATURE_DIAG_ON_DATA_PORT */

/*-------------------------------------------------------------------------
                   Local function prototype
 ------------------------------------------------------------------------- */
#ifdef FEATURE_DIAG_ON_DATA_PORT
static void diagcomm_dancing_to_data_mode( void );

static void diagcomm_assign_port_cb
( 
  /* status of rdm_assign_port                  */
  rdm_assign_status_type        status,

  /* the service requesting the assignment       */
  rdm_service_enum_type         srv_requester,

  /* the desired port for the srv_requester     */
  rdm_device_enum_type          dev_desired
);

static void diagcomm_reset_dancing_state( void );

static void diagcomm_process_rdm_cb( void *unused );
#endif /* FEATURE_DIAG_ON_DATA_PORT */

/*lint -save -e754 */

/*===========================================================================

FUNCTION DIAG_RECORD_DM_BITRATE                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function is used by caller to set baudrate for diag which will take
  effect at the next occurence of DIAG_SIO_OPEN_SIG (calls sio_open).

PARAMETERS
  bitrate - Bitrate to use at next SIO open
  
RETURN VALUE
  None
  
===========================================================================*/
void
diag_record_dm_baudrate ( sio_bitrate_type bitrate )
{
#ifdef DIAG_SIO_SUPPORT
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.rx_bitrate = bitrate;
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.tx_bitrate = bitrate;
#endif

} /* diag_record_dm_baudrate */


/*===========================================================================

FUNCTION DIAG_CHANGE_DEFAULT_SIO_BITRATE                EXTERNALIZED FUNCTION

DESCRIPTION
  This function changed the default diag bitrate as well as change the
  current operating bitrate.

PARAMETERS
  bitrate - Bitrate to use at next SIO open
  
RETURN VALUE
  None

===========================================================================*/
void
diag_change_default_sio_bitrate (sio_bitrate_type bitrate)
{
#ifdef DIAG_DEFAULT_BITRATE_NV
  /* Change NV item that stores the default diag bitrate. */
  static nv_cmd_type nvcmd;
  static nv_sio_baudrate_type nv_item;

  /* Set bitrate */
  nv_item = (nv_sio_baudrate_type) bitrate;

  nvcmd.cmd = NV_WRITE_F;
  nvcmd.data_ptr = (nv_item_type *) & nv_item;
  nvcmd.item = NV_DIAG_DEFAULT_BAUDRATE_I;

  nvcmd.done_q_ptr = NULL;
  nvcmd.sigs = 0;       /* don't need an ACK from NV on this. */
  nvcmd.tcb_ptr = osal_thread_self ();

  nv_cmd (&nvcmd);

  /* Set diag's bitrate for next open cycle */
  diag_record_dm_baudrate (bitrate);

  /* If diag has a port open, change the bitrate */
  if (diagcomm_status (DIAGCOMM_PORT_1))
  {
    diagcomm_io_ioctl( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1],
                       (uint32)SIO_IOCTL_CHANGE_BAUD_NOW,
                       (void*)&bitrate );
  }
#endif /* DIAG_DEFAULT_BITRATE_NV */

} /* diag_change_default_sio_bitrate */


/*===========================================================================

FUNCTION DIAGPKT_TO_DATA_MODE

DESCRIPTION
  This procedure processes to_data_mode command.

============================================================================*/
PACK (void *)
diagpkt_to_data_mode (PACK (void *) req_pkt, uint16 pkt_len)
{
  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

#ifdef FEATURE_DIAG_ON_DATA_PORT
  diagcomm_dancing_to_data_mode();
#else
  MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Switch port to Data Service not supported.");
#endif
  return NULL;
}               /* diagpkt_to_data_mode */

/*===========================================================================

FUNCTION DIAGCOMM_SIO_QCDMG
FUNCTION DIAGCOMM_SIO_QCDMG_PORT


DESCRIPTION
  Execute AT$QCDMG command. Diag will close its open port and grab DS's
  port.

  It returns TRUE if RDM accepts the port mapping; FALSE if RDM can't 
  change the port mapping.

===========================================================================*/
boolean diagcomm_sio_qcdmg (void)
{
#ifdef FEATURE_DIAG_ON_DATA_PORT

  /* function return                            */
  boolean               result;

  /* data's current device                      */
  rdm_device_enum_type  ds_dev;

  /* diag's current device                      */
  rdm_device_enum_type  diag_dev;

/*------------------------------------------------------------------*/
  MSG(MSG_SSID_DIAG,MSG_LEGACY_MED,"diagcomm_sio_qcdmg");

  if( (diagcomm_dancing_state.state != DIAGCOMM_DANCING_IDLE) &&
      (diagcomm_dancing_state.state != DIAGCOMM_DANCING_IN_DANCE) )
  {
    /* called in wrong state, we are doing $qcdmg or to_data_mode       */
    return FALSE;
  }

  ds_dev                        = rdm_get_device(RDM_DATA_SRVC);
  
  /* get DIAG's current port                    */
  diag_dev                      = rdm_get_device(RDM_DIAG_SRVC);

  /* since the $QCDMG is from DS port, data port is open        */
  ASSERT(ds_dev != RDM_NULL_DEV);

  /* test if diag can use DS's port             */
  if(!rdm_is_srv_dev_compat(RDM_DIAG_SRVC, ds_dev))
  {
    return FALSE;
  }

  /* passed condition checking                  */

  /* save the port setting's of DIAG and DS     */
  diagcomm_dancing_state.state           = DIAGCOMM_DANCING_QCDMG_OPENING;
  diagcomm_dancing_state.ds_old_dev      = ds_dev;
  diagcomm_dancing_state.diag_old_dev    = diag_dev;
  
  /* open the ds_old_dev for DIAG                               */
  result = rdm_assign_port(RDM_DIAG_SRVC, diagcomm_dancing_state.ds_old_dev,
                           diagcomm_assign_port_cb);

  return result;
#else
  return FALSE;

#endif /* FEATURE_DIAG_ON_DATA_PORT */
} /* diagcomm_sio_qcdmg */


boolean diagcomm_sio_qcdmg_port (rdm_device_enum_type port_id)
{
#ifdef FEATURE_DIAG_ON_DATA_PORT

  /* function return                            */
  boolean               result;

  /* data's current device                      */
  rdm_device_enum_type  ds_dev;

  /* diag's current device                      */
  rdm_device_enum_type  diag_dev;

/*------------------------------------------------------------------*/
  MSG(MSG_SSID_DIAG,MSG_LEGACY_MED,"diagcomm_sio_qcdmg_port");

  if( (diagcomm_dancing_state.state != DIAGCOMM_DANCING_IDLE) &&
      (diagcomm_dancing_state.state != DIAGCOMM_DANCING_IN_DANCE) )
  {
    /* called in wrong state, we are doing $qcdmg or to_data_mode       */
    return FALSE;
  }

  ds_dev = port_id;
  
  /* get DIAG's current port                    */
  diag_dev                      = rdm_get_device(RDM_DIAG_SRVC);

  /* since the $QCDMG is from DS port, data port is open        */
  ASSERT(ds_dev != RDM_NULL_DEV);

  /* test if diag can use DS's port             */
  if(!rdm_is_srv_dev_compat(RDM_DIAG_SRVC, ds_dev))
  {
    return FALSE;
  }

  /* passed condition checking                  */

  /* save the port setting's of DIAG and DS     */
  diagcomm_dancing_state.state           = DIAGCOMM_DANCING_QCDMG_OPENING;
  diagcomm_dancing_state.ds_old_dev      = ds_dev;
  diagcomm_dancing_state.diag_old_dev    = diag_dev;
  
  /* open the ds_old_dev for DIAG                               */
  result = rdm_assign_port(RDM_DIAG_SRVC, diagcomm_dancing_state.ds_old_dev,
                           diagcomm_assign_port_cb);

  return result;
#else
  return FALSE;
#endif /* FEATURE_DIAG_ON_DATA_PORT */
} /* diagcomm_sio_qcdmg_port */



#ifdef FEATURE_DIAG_ON_DATA_PORT
/*===========================================================================

FUNCTION DIAGCOMM_DANCING_TO_DATA_MODE

DESCRIPTION
  Execute DM to_data_mode command. Diag will return its port to DS and open
  DIAG's old port before dance.

===========================================================================*/
static void diagcomm_dancing_to_data_mode( void )
{
  /* current DS port                                    */
  rdm_device_enum_type  cur_ds_dev;

  /* current DIAG port                                  */
  rdm_device_enum_type  cur_diag_dev;
  
/*------------------------------------------------------------------*/

  if(diagcomm_dancing_state.state != DIAGCOMM_DANCING_IN_DANCE)
  {
    /* the DIAG and DS ports was not in dance state     */
    return;
  }

  cur_ds_dev    = rdm_get_device(RDM_DATA_SRVC);
  cur_diag_dev  = rdm_get_device(RDM_DIAG_SRVC);

  if(cur_diag_dev != diagcomm_dancing_state.ds_old_dev)
  {
    /* DIAG has opened a different port, the dance has broken   */
    diagcomm_reset_dancing_state();
    return;
  }

  /* pass condition checking                                    */
  if(cur_ds_dev != RDM_NULL_DEV)
  {
    /* the DS acquired new port after last $QCDMG command, 
       No need to return the port back to DS,
       We will return diag to its old port                      */
    diagcomm_dancing_state.state   = DIAGCOMM_DANCING_TDM_OPENING_DIAG;
    if(!rdm_assign_port(RDM_DIAG_SRVC, diagcomm_dancing_state.diag_old_dev, 
                        diagcomm_assign_port_cb))
    {
      /* rdm reject the opening request                   */
      ERR("fail to return DIAG port",0,0,0);
      diagcomm_reset_dancing_state();
    }
  }
  else
  {
    /* DS don't not have a port, return the DS's old port to DS */
    diagcomm_dancing_state.state   = DIAGCOMM_DANCING_TDM_OPENING_DS;
    if(!rdm_assign_port(RDM_DATA_SRVC, cur_diag_dev, diagcomm_assign_port_cb))
    {
      /* rdm reject the opening request                   */
      ERR("TDM: fail to return DS port",0,0,0);
      diagcomm_reset_dancing_state();
    }
  }
} /* diagcomm_dancing_to_data_mode */

/*===========================================================================

FUNCTION DIAGCOMM_ASSIGN_PORT_CB

DESCRIPTION
  The call back function for rdm_assign_port for $QCDMG and to_data_mode
  commands.

===========================================================================*/
static void diagcomm_assign_port_cb
( 
  /* status of rdm_assign_port                  */
  rdm_assign_status_type        status,

  /* the service requesting the assignment       */
  rdm_service_enum_type         srv_requester,

  /* the desired port for the srv_requester     */
  rdm_device_enum_type          dev_desired
)
{
  MSG_4(MSG_SSID_DIAG,MSG_LEGACY_MED,
        "diagcomm_assign_port_cb, state: %d, status: %d, srv: %d, dev: %d", 
        diagcomm_dancing_state.state, status, srv_requester, dev_desired);

  /* make sure the parameters from last time has been processed   */
  ASSERT(!diagcomm_dancing_assign_port_cb_params.valid);
  
  diagcomm_dancing_assign_port_cb_params.valid           = TRUE;
  diagcomm_dancing_assign_port_cb_params.status          = status;
  diagcomm_dancing_assign_port_cb_params.srv_requester   = srv_requester;
  diagcomm_dancing_assign_port_cb_params.dev_desired     = dev_desired;

  // This function will be called in DIAG task context.
  (void) diag_idle_processing_register (diagcomm_process_rdm_cb, NULL);
} /* diagcomm_assign_port_cb */
#endif /* FEATURE_DIAG_ON_DATA_PORT */

#ifdef FEATURE_DIAG_ON_DATA_PORT
/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_RDM_CB

DESCRIPTION
  The function process call back status of rdm_assign_port for $QCDMG 
  and to_data_mode commands.

===========================================================================*/
static void diagcomm_process_rdm_cb (void *unused)
{

  /* diagcomm_dancing_assign_port_cb parameters                 */
  diagcomm_dancing_assign_port_cb_param_type rdm_cb_params;

  (void) unused;      /* suppress compiler warning */

  ASSERT(diagcomm_dancing_assign_port_cb_params.valid);
    
  /* get the parameters of rdm_cb to process                    */
  rdm_cb_params = diagcomm_dancing_assign_port_cb_params;

  /* diagcomm_dancing_assign_port_cb_params can be reused       */
  diagcomm_dancing_assign_port_cb_params.valid = FALSE;

  switch (diagcomm_dancing_state.state)
  {
    case DIAGCOMM_DANCING_IDLE:
    case DIAGCOMM_DANCING_IN_DANCE:
      /* we should not be called in this state                  */
      ERR("diagcomm_assign_port_cb called in IDLE",0,0,0);
      break;

    case DIAGCOMM_DANCING_QCDMG_OPENING:
      /* DIAG has asked RDM to open DS's port for $QCDMG        */
      ASSERT(rdm_cb_params.srv_requester == RDM_DIAG_SRVC);
      ASSERT(rdm_cb_params.dev_desired == diagcomm_dancing_state.ds_old_dev);

      /* DIAG has asked RDM to open DS's port for $QCDMG        */
      if(rdm_cb_params.status == RDM_DONE_S)
      {
        /* succeeds                     */
        diagcomm_dancing_state.state = DIAGCOMM_DANCING_IN_DANCE;
      }
      else
      {
        /* can't close open DS's port   */
        ERR("QCDMG: fail to open DS port",0,0,0);
        diagcomm_reset_dancing_state();
      }
      break;

    case DIAGCOMM_DANCING_TDM_OPENING_DS:
      /* DIAG has asked RDM to open DS's port for to_data_mode  */
      ASSERT(rdm_cb_params.srv_requester == RDM_DATA_SRVC);
      ASSERT(rdm_cb_params.dev_desired == diagcomm_dancing_state.ds_old_dev);

      if(rdm_cb_params.status == RDM_DONE_S)
      {
        /* continue to get the old diag port back               */
        if(rdm_get_service(diagcomm_dancing_state.diag_old_dev) != 
           RDM_NULL_SRVC)
        {
          /* Diag's old port has been take by other service. Stop here  */
          diagcomm_reset_dancing_state();
        }
        else
        {
          diagcomm_dancing_state.state = DIAGCOMM_DANCING_TDM_OPENING_DIAG;

          if(!rdm_assign_port(RDM_DIAG_SRVC,
                              diagcomm_dancing_state.diag_old_dev, 
                              diagcomm_assign_port_cb))
          {
            /* can't open diag's old port                               */
            ERR("fail to open dev: %d", diagcomm_dancing_state.diag_old_dev,
                0,0);
            diagcomm_reset_dancing_state();
          }
        }
      }
      else
      {
        /* rdm fail to open DS's old port                               */
        ERR("fail to open dev: %d", diagcomm_dancing_state.ds_old_dev,0,0);
        diagcomm_reset_dancing_state();
      }
      break;

    case DIAGCOMM_DANCING_TDM_OPENING_DIAG:
      /* DIAG has asked RDM to open its old port for to_data_mode       */
      ASSERT(rdm_cb_params.srv_requester == RDM_DIAG_SRVC);
      ASSERT(rdm_cb_params.dev_desired == diagcomm_dancing_state.diag_old_dev);

      if(rdm_cb_params.status != RDM_DONE_S)
      {
        /* failed                                               */
        ERR("fail to open %d", diagcomm_dancing_state.diag_old_dev,0,0);
      }
      diagcomm_reset_dancing_state();
      break;

    default:
      /* any other state is wrong                               */
      ERR("wrong state: %d", diagcomm_dancing_state.state,0,0);
      break;
  }

  (void) diag_idle_processing_unregister (diagcomm_process_rdm_cb, NULL);

} /* diagcomm_process_rdm_cb */
#endif /* FEATURE_DIAG_ON_DATA_PORT */

#ifdef FEATURE_DIAG_ON_DATA_PORT
/*===========================================================================

FUNCTION DIAGCOMM_RESET_DANCING_STATE

DESCRIPTION
  Reset dancing_state.

===========================================================================*/
static void diagcomm_reset_dancing_state( void )
{
  diagcomm_dancing_state.state           = DIAGCOMM_DANCING_IDLE;
  diagcomm_dancing_state.ds_old_dev      = RDM_NULL_DEV;
  diagcomm_dancing_state.diag_old_dev    = RDM_NULL_DEV;

} /* diagcomm_reset_dancing_state */
#endif /* FEATURE_DIAG_ON_DATA_PORT */

/*===========================================================================

PACKET   DIAG_CONFIG_COMM_F

PURPOSE  Sent by DM to request a baud rate changefor DIAG serial
         communications.

RESPONSE DMSS returns diag_config_comm_rsp_type packet that contains
         possible configurations or an acknowledgement to a change request.
         Upon sending

===========================================================================*/
typedef enum
{
  DIAG_RS232_CONFIG_INFO = 0,   /* Request for baud rate capabilities */
  DIAG_RS232_CONFIG,        /* Set baud rate                      */
  DIAG_USB_CONFIG_INFO,     /* UNIMPLEMENTED!!!                   */
  DIAG_USB_CONFIG       /* UNIMPLEMENTED!!!                   */
}
diag_config_comm_enum_type;

typedef PACK(struct)
{
  byte cmd_code;        /* Command code                         */
  byte sub_cmd;         /* Use diag_config_comm_enum_type       */
  dword baudrate;       /* Baud rate to be set - use only in a  */
  /* config command, not an info request  */
}
diag_config_comm_req_type;

typedef PACK(struct)
{
  byte cmd_code;        /* Command code                   */
  byte sub_cmd;         /* Use diag_config_comm_enum_type */

  PACK(union)
  {
    boolean success;        /* TRUE if baud rate change ACK   */
    byte num_options;       /* Number of options following    */
  }
  cmd_specific;

  /* The following only exist if responding to an info request.    */

  dword baudrate[4];        /* Baud rates available           */
  /* Only used if num_options is nonzero */
}
diag_config_comm_rsp_type;

/*===========================================================================

FUNCTION DIAGCOMM_CONFIG_COMM

DESCRIPTION
  This procedure processes a handset emulation request for bitmap
  screen data request.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
PACK(void *)
diagcomm_config_comm (PACK(void *) req_pkt, word pkt_len)
{
#ifdef DIAG_SIO_SUPPORT
  diag_config_comm_req_type *req = (diag_config_comm_req_type *) req_pkt;
  diag_config_comm_rsp_type *rsp;
  dword rsp_len = FPOS (diag_config_comm_rsp_type, cmd_specific);
  sio_bitrate_type new_bitrate;

  /* First, we must find out what command we are servicing */
  switch (req->sub_cmd)
  {
  case DIAG_RS232_CONFIG_INFO:  /* Send available baudrates */
    /* Check for valid packet length */
    if (pkt_len != FSIZ (diag_config_comm_req_type, cmd_code) +
    FSIZ (diag_config_comm_req_type, sub_cmd))
    {
      return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len);
    }

    rsp_len = sizeof (diag_config_comm_rsp_type);
    break;

  case DIAG_RS232_CONFIG:   /* Change baudrate */
    rsp_len += FSIZ (diag_config_comm_rsp_type, cmd_specific.success);
    break;

  default:
    /* Do nothing */
    break;
  }

  rsp = (diag_config_comm_rsp_type *) diagpkt_alloc (DIAG_CONFIG_COMM_F, rsp_len);

  if(rsp != NULL)
  {
  	rsp->sub_cmd = req->sub_cmd;
  	switch (req->sub_cmd)
	  {
	  case DIAG_RS232_CONFIG_INFO:  /* Send available baudrates */

	    /* Hardcode for now.  More dynamic later if necessary. */
	    rsp->cmd_specific.num_options = 4;

	    rsp->baudrate[0] = 19200;
	    rsp->baudrate[1] = 38400;
	    rsp->baudrate[2] = 57600;
	    rsp->baudrate[3] = 115200;

	    break;

	  case DIAG_RS232_CONFIG:   /* Change baudrate */

	    rsp->cmd_specific.success = TRUE;

	    switch (req->baudrate)
	    {
	    case 19200:
	      new_bitrate = SIO_BITRATE_19200;
	      break;
	    case 38400:
	      new_bitrate = SIO_BITRATE_38400;
	      break;
	    case 57600:
	      new_bitrate = SIO_BITRATE_57600;
	      break;
	    case 115200:
	      new_bitrate = SIO_BITRATE_115200;
	      break;
	    default:
	      rsp->cmd_specific.success = FALSE;
	      break;
	    }
	    break;

	  default:
	    /* Do nothing */
	    break;
	  }
    if (rsp->cmd_specific.success == TRUE)
    {
     /* Prior to flush, we must commit the packet. */
      diagpkt_commit (rsp);

     rsp = NULL;

    /* If streaming, we need to drain the buffer to ensure
       the ACK is sent.  We will also stop streaming. */
      diagbuf_flush_timeout(DIAG_SIO_TIMEOUT_TIMER_LEN);
    
    diagcomm_io_ioctl( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], 
                       (uint32)SIO_IOCTL_CHANGE_BAUD_RATE,
                       (void *) &new_bitrate);
    }
  }

  return (rsp);
#else
  return NULL;
#endif //DIAG_SIO_SUPPORT

} /* diagcomm_config_comm */


#ifdef FEATURE_DIAG_ON_DATA_PORT
/*==========================================================================

PACKET   DIAG_SERIAL_CHG_F

PURPOSE  Request sent from the DM to the DMSS to change the serial port's
         mode back to Data Services

============================================================================*/
DIAGPKT_RSP_DEFINE (DIAG_SERIAL_CHG_F)
     byte mode_changed;     /* Boolean whether mode changed */

DIAGPKT_RSP_END
/*===========================================================================

FUNCTION DIAGCOMM_MODE_CHG_PKT

DESCRIPTION
  This procedure switches Diag's serial port back to Data Services mode

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
PACK(void *)
diagcomm_mode_chg_pkt (PACK(void *) req_pkt, uint16 pkt_len)
{
  DIAG_SERIAL_CHG_F_rsp_type *rsp;
  const uint32 rsp_len = sizeof (DIAG_SERIAL_CHG_F_rsp_type);

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

  rsp = (DIAG_SERIAL_CHG_F_rsp_type *) diagpkt_alloc (DIAG_SERIAL_CHG_F, rsp_len);
  if(rsp)
  {
	 rsp->mode_changed = TRUE;
     diagpkt_commit (rsp);
     rsp = NULL;
  }

  /* Flush DIAG's TX buffer, make sure the packet gets sent before
     we close down */
  diagbuf_flush_timeout(DIAG_SIO_TIMEOUT_TIMER_LEN);

  diagcomm_dancing_to_data_mode();

  return (rsp);
}
#endif /* FEATURE_DIAG_ON_DATA_PORT */
 

static const diagpkt_user_table_entry_type diagcomm_tbl[] = {
#ifdef FEATURE_DIAG_ON_DATA_PORT
  {DIAG_SERIAL_CHG_F, DIAG_SERIAL_CHG_F, diagcomm_mode_chg_pkt},
#endif
  {DIAG_CONFIG_COMM_F, DIAG_CONFIG_COMM_F, diagcomm_config_comm}
};


/*===========================================================================

FUNCTION DIAGCOMM_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communcations layer.

===========================================================================*/
void
diagcomm_init (void)
{
  /* Register packets with the diag dispatcher */
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, diagcomm_tbl);

}               /* diagcomm_init */


/*lint -restore */  
