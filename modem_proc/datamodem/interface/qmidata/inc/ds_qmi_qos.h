#ifndef _DS_QMI_QOS_H
#define _DS_QMI_QOS_H
/*===========================================================================

                         D S _ Q M I _ Q O S . H

DESCRIPTION

 The Data Services Qualcomm Wireless Data Services MSM Interface header file.

EXTERNALIZED FUNCTIONS

  qmi_qos_init()
    Register the Wireless Data Service with QMUX for all applicable QMI links.
     
  
Copyright (c) 2004-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_qos.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/19/10    vs     Changes for Dual IP support over single QMI instance
12/12/06    ks     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"   

#include "ds_qmi_defs.h" 
#include "ps_iface_defs.h"


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  FUNCTION QMI_QOS_INIT()

  DESCRIPTION
    Register the Quality Of Service with QMUX 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_qos_init
(
  void
);

/*===========================================================================
  FUNCTION QMI_QOS_GET_FLOW_PTR_FROM_HANDLE()

  DESCRIPTION
    Given a qos handle it returns the corresponding flow pointer

  PARAMETERS
    qos_handle : QOS handle for which flow pointer is to be determined
    flow_ptr   : Flow pointer corresponding to QOS handle

  RETURN VALUE
    0 on Failure
    1 on Succes

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_qos_get_flow_ptr_from_handle
(
  uint32  qos_handle,
  ps_flow_type **flow_ptr
);

/*===========================================================================
  FUNCTION QMI_QOS_GET_FC_STATS()

  DESCRIPTION
    This function retrieves QoS flow control stats

  PARAMETERS
    num_flow_enable: address of flow enable cnt
    num_flow_disable: address of flow disable cnt

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qos_get_fc_stats
(
  uint32  * num_flow_enable,
  uint32  * num_flow_disable
);

#endif /* _DS_QMI_QOS_H */
