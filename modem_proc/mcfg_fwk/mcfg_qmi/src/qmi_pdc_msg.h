#ifndef QMI_PDC_MSG_H
#define QMI_PDC_MSG_H
/*==============================================================================

    Q M I   P E R S I S T E N T   D E V I C E   C O N F I G   M E S S A G E
 
GENERAL DESCRIPTION
  Header for the Persistent Device Configuration (PDC) QMI service message
  library

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_qmi/src/qmi_pdc_msg.h#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
04/30/10 mj   Initial creation

==============================================================================*/

#include "qmi_csi.h"

#include "persistent_device_configuration_v01.h"

#include "mcfg_common.h"
/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/


/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

/*===========================================================================

  FUNCTION pdc_qmi_compose_config_change_ind

  DESCRIPTION
    This function maps the QMI config type to MCFG MEM enum value

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    qmi_ind          [out] QMI IND
    qmi_ind_size     [out] Size of composed QMI IND
    mcfg_config_type [in]  type of config file
    mcfg_config_id   [in]  ID of new config file

  RETURN VALUE
    QMI_CSI_NO_ERR if success, otherwise error

  SIDE EFFECTS
    None

===========================================================================*/
qmi_csi_error pdc_qmi_compose_config_change_ind
(
  pdc_config_change_ind_msg_v01    *qmi_ind,
  uint32                           *qmi_ind_size,
  mcfg_config_type_e_type           config_type,
  mcfg_config_id_s_type            *config_id
);

#endif /* QMI_PDC_MSG_H */

