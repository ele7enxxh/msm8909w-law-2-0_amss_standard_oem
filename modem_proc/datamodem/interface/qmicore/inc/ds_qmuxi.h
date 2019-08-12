#ifndef _DS_QMUXI_H
#define _DS_QMUXI_H
/*===========================================================================

                         D S _ Q M U X I . H

DESCRIPTION

  The Data Services Qualcomm MSM Interface control channel message
  multiplexing protocol internal header file.

  This file externalizes information required by the QMI control service.

EXTERNALIZED FUNCTIONS

   qmux_get_service_cfg
     Retreive control block for QMI_CTL to call callbacks and get sp from

Copyright (c) 2005,2009,2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/ds_qmuxi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
02/09/11    ua     Added support for DUAL IP Autoconnect. 
                   Default Autoconnect configuration is supported from EFS.
11/30/09    ar     Add QMUX PDU framing supprot for streaming devices
10/23/09    ar     Add supprot for power save indication filtering and 
                   port configuration from NVRAM.
05/31/05   jd/ks   Code review updates
05/11/05    ks     changed include file to ds_qmux.h
03/14/05    ks     Removed featurization
03/08/05    ks     Clean up
01/26/05    jd     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "sio.h"
#include "dsm.h"

#include "ds_qmux.h"
#include "ds_qmux_msg.h"
#include "ds_qmi_svc_ext.h"


/*===========================================================================

                                DATA TYPES

===========================================================================*/

typedef enum
{
  QMI_PORT_NONE             = 0,
  QMI_PORT_TO_OPEN          = 1,
  QMI_PORT_OPENED           = 2,
  QMI_PORT_TO_CLOSE         = 3
} qmi_port_status_enum;

typedef struct qmi_port_request_s
{
  boolean                     is_control_port;
  qmi_port_status_enum        status;
  sio_port_id_type            sio_port;
  uint32                      ep_id;
  uint32                      consumer_pipe_num;
  uint32                      producer_pipe_num;
  struct qmi_port_request_s * next;
} qmi_port_request_type;

typedef enum qmux_device_stream_e
{
  QMUX_DEVSTREAM_CONTROL = 0x01,
  QMUX_DEVSTREAM_PRIMARY = 0x02
} qmux_device_stream_type;

typedef enum qmux_frame_mode_e
{
  QMUX_FRAME_MODE_NULL   = 0x00,                     /* Control value */
  QMUX_FRAME_MODE_IF1    = QMUXI_IFTYPE_QMUX,
  QMUX_FRAME_MODE_IF2    = QMUXI_IFTYPE_QMUX_FRAMED
} qmux_frame_mode_type;

typedef struct qmux_port_config_s
{
  /* QMI device for SIO port */
  sio_port_id_type                                 sio_port; 
  /* Default EP ID */
  uint32                                           default_ep_id;
  /* SIO device stream for QMI */
  qmux_device_stream_type                          qmi_stream;
  /* QMUX PDU frame mode */
  qmux_frame_mode_type                             frame_mode;
} qmux_port_config_type;

/*---------------------------------------------------------------------------
  QMUX PDU Framing constants
---------------------------------------------------------------------------*/
#define QMUX_FRAME_MODE_NONE          (QMUX_FRAME_MODE_IF1)
#define QMUX_FRAME_MODE_TYPE1         (QMUX_FRAME_MODE_IF2)

#define QMUX_FRAME_FLAG_START         ((uint8)0xBE)    /* Frame start flag */
#define QMUX_FRAME_FLAG_STOP          ((uint8)0xEF)    /* Frame stop flag  */
#define QMUX_FRAME_PDU_START_OFFSET   (0)
#define QMUX_FRAME_PDU_IF_OFFSET      (1)
#define QMUX_FRAME_PDU_FCS_OFFSET     (2)                      /* From end */
#define QMUX_FRAME_PDU_STOP_OFFSET    (1)                      /* From end */
#define QMUX_FRAME_PDU_LEN_OFFSET     (2)
#define QMUX_FRAME_PDU_LEN_OVERHEAD   (4)    /* START+STOP+IFtype+FCS bytes*/

/* Macro to test for supported values of frame mode (iftype) */
#define IS_FRAME_MODE_VALID( frame_mode )                               \
        ((QMUX_FRAME_MODE_NONE  == (qmux_frame_mode_type)frame_mode) || \
         (QMUX_FRAME_MODE_TYPE1 == (qmux_frame_mode_type)frame_mode))

/* Macro to test if SDU framing required */
#define IS_FRAMING_REQUIRED( frame_mode )                               \
         (QMUX_FRAME_MODE_TYPE1 == (qmux_frame_mode_type)frame_mode)

/* Macro to test for supported values of QMI device stream */
#define IS_DEVSTREAM_VALID( stream )                                    \
        ((QMUX_DEVSTREAM_CONTROL == (qmux_device_stream_type)stream) || \
         (QMUX_DEVSTREAM_PRIMARY == (qmux_device_stream_type)stream))


/*---------------------------------------------------------------------------
  QMUX instance state structure
---------------------------------------------------------------------------*/
typedef struct
{
  struct                               /* data for USB control channel use */
  {
    sio_stream_id_type    sio_handle;
    dsm_watermark_type    rx_wm;
    q_type                rx_wm_q;       /* Corresponding USB ctl rx queue */
    dsm_watermark_type    tx_wm;
    q_type                tx_wm_q;       /* Corresponding USB ctl tx queue */
    boolean               ports_active;  /* Are the SIO queues open? */
    qmux_port_config_type port_info;     /* Port configuration */
    dsm_item_type *       pending_sdu_ptr; /* SDU pending unframing */
  } io;
  qmux_svc_info_type *  service[QMUX_SERVICE_MAX];
  boolean               initialized;
  boolean               dtr_status;      /* current DTR status */

} qmux_state_type;


/*===========================================================================

                        EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMUX_GET_SERVICE_CFG()

  DESCRIPTION
    Get Service configuration

  PARAMETERS
    qmux_handle : handle assigned to each service by qmux
    svc_type    : type of qmux service

  RETURN VALUE
    qmux_svc_config_type
    NULL if input service handle or service type is invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern qmux_svc_config_type *  qmux_get_service_cfg
(
  void *               qmux_sp,
  qmux_service_e_type  svc_type
);

/*===========================================================================
  FUNCTION QMUXI_REGISTER_DEVICES_WITH_RDM

  DESCRIPTION
    This function registers the configured ports with SIO Runtime
    Devive Mapper (RDM)
  
  PARAMETERS  
    None
  
  DEPENDENCIES
    None

  RETURN VALUE
    TRUE if read operation successful 
    FALSE if error occurs

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmuxi_register_devices_with_rdm (void);


/*===========================================================================
  FUNCTION QMUXI_GET_QMI_CONTROL_DEVICE_ID()
   
  DESCRIPTION
    Returns instance QMUX device ID type associated with QMI instance.

  PARAMETERS
    qmi_instance : instance of QMI

  RETURN VALUE
    Device ID if valid instance.
    QMI_DEV_NONE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sio_port_id_type qmuxi_get_qmi_control_device_id
(
  qmi_instance_e_type     qmi_inst
);

#endif /* _DS_QMUXI_H */

