#ifndef QMI_CMD_PROCESS_H
#define QMI_CMD_PROCESS_H
/*===========================================================================

                         Q M I _ C M D _ P R O C E S S . H

DESCRIPTION

 The Qualcomm MSM Interface(QMI) command process header file.

EXTERNALIZED FUNCTIONS


Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/qmi_cmd_process.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/31/11    rk    Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"  

#include "dsm.h" 
#include "qmi_svc_defs.h" 

/*===========================================================================

                                 DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
TYPEDEF QMI_CMD_INIT_LEGACY_SERVICES_TYPE

DESCRIPTION
  QMI legacy service init command type
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 num_qmi_instance;
} qmi_cmd_init_legacy_services_type;

/*---------------------------------------------------------------------------
TYPEDEF QMI_CMD_RECV_LEGACY_SVC_REQ_TYPE

DESCRIPTION
  QMI legacy service request recv command type
---------------------------------------------------------------------------*/
typedef struct
{
  uint32            clid;
  void *            svc_handle;
  dsm_item_type *   sdu_in;
} qmi_cmd_recv_legacy_svc_req_type;


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  FUNCTION QMI_CMD_PROCESS_LEGACY_SVC_RESPONSE()

  DESCRIPTION
    Process the legacy service response and send to qmux.
    
  PARAMETERS
    svci      : service info for the incoming request
    clid      : clinet ID where the request come from.
    qmux_pdu  : Raw QMUX PDU
   
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_cmd_process_legacy_svc_response
(
  qmux_svc_info_type * svci,
  uint8                qmux_hdr_clid, 
  dsm_item_type *      qmux_pdu
);

/*===========================================================================
  FUNCTION QMI_CMD_RECV_LEGACY_SVC_REQ()

  DESCRIPTION
    Send the legacy service request to the DCC Task. 

  PARAMETERS
    svci      : service info for the incoming request
    clid      : clinet ID where the request come from.
    qmux_sdu  : Raw QMUX SDU

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_cmd_recv_legacy_svc_req
( 
  qmux_svc_info_type * svci,
  uint8                clid, 
  dsm_item_type *      sdu_in
);

#endif /* QMI_CMD_PROCESS_H */
