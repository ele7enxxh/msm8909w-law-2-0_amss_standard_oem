#ifndef _DS_QMI_CTL_REMOTE_H
#define _DS_QMI_CTL_REMOTE_H
/*=========================================================================*/
/*!
  @file
  ds_qmi_ctl_remote.h

  @brief
  This file provides remoting service for QMI_CTL 

  @details
  The following external methods are supported.

  qmi_ctl_remote_init()
    Initialize the QMI control remoting service
  
  qmi_ctl_remote_set_data_format()
    Sets the data format on the remote port.

  qmi_ctl_remote_release()
    Release the QMI control remoting services

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_ctl_remote.h#1 $ $DateTime: 2004/09/20

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/06/11    hm     Created module
===========================================================================*/

#include "comdef.h"
#include "ds_qmi_ctl.h"

/*!
  @brief
  Initialize QMI_CTL_REMOTE service.

  @params     instance - QMI CTL instance for which remoting is required.
  @see        None.
  @return     QMI_CTL_REMOTE instance if valid and available.
  @return     QMI_INSTANCE_MAX - Error/remoting not supported

*/
qmi_instance_e_type 
qmi_ctl_remote_init
(
  qmi_instance_e_type  qmi_inst
);

/*!
  @brief
  Set the data format for remote port. 

  @params[in] qmi_ctl_remote_instance - QMI_CTL_REMOTE instance.
  @params[in] link_protocol           - Link protocol (IP_MODE/ETHERNET_MODE)
  @params[in] qos_hdr_enabled         - Enable/disable QoS header during data.
  @params[in] ul_tlp                  - UL TLP setting. 

  @see        None.
  @return     None.
*/
void qmi_ctl_remote_set_data_format
(
  qmi_instance_e_type   qmi_ctl_remote_instance,
  uint16                link_prot,
  uint8                 qos_hdr_enabled,
  boolean               ul_tlp
);


/*!
  @brief
  Release ALL QMI CTL remoted instances.

  @params     None.
  @see        None.
  @return     None.
*/
void qmi_ctl_remote_release
(
  void 
);

#endif /* _DS_QMI_CTL_REMOTE_H */

