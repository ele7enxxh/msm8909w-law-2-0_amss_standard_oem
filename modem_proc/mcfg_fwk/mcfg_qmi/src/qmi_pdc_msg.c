/*==============================================================================

    Q M I   P E R S I S T E N T   D E V I C E   C O N F I G   M E S S A G E

GENERAL DESCRIPTION
  Persistent Device Configuration (PDC) QMI service message library

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_qmi/src/qmi_pdc_msg.c#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
04/30/10 mj   Initial creation

==============================================================================*/

#include "comdef.h"
#include <stringl/stringl.h>

#include "qmi_csi.h"

#include "qmi_pdc_msg.h"

#include "persistent_device_configuration_v01.h"

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

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

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/

/*===========================================================================

  FUNCTION pdc_qmi_from_mcfg_config_type

  DESCRIPTION
    This function maps the MCFG MEM config type to the QMI value

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    mcfg_config_type [in] MCFG type enum

  RETURN VALUE
    QMI config type enum value, MAX if not found

  SIDE EFFECTS
    None

===========================================================================*/
pdc_config_type_enum_v01 pdc_qmi_from_mcfg_config_type
( 
  mcfg_config_type_e_type mcfg_config_type
)
{
  pdc_config_type_enum_v01 qmi_config_type;
  
  /*-----------------------------------------------------------------------*/
 
  switch (mcfg_config_type)
  {
    case MCFG_TYPE_HW:
      qmi_config_type = PDC_CONFIG_TYPE_MODEM_PLATFORM_V01;
      break;
      
    case MCFG_TYPE_SW:
      qmi_config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
      break;
      
    case MCFG_NUM_OF_TYPES:
    default:
      qmi_config_type = PDC_CONFIG_TYPE_ENUM_MAX_ENUM_VAL_V01;
      break;
  }
  
  return qmi_config_type;
} /* pdc_qmi_from_mcfg_config_type() */


/*===========================================================================

  FUNCTION pdc_mcfg_from_qmi_config_type

  DESCRIPTION
    This function maps the QMI config type to MCFG MEM enum value

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    qmi_config_type [in] MCFG type enum

  RETURN VALUE
    QMI config type enum value, MAX if not found

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_config_type_e_type pdc_mcfg_from_qmi_config_type
( 
  pdc_config_type_enum_v01 qmi_config_type
)
{
  mcfg_config_type_e_type mcfg_config_type;
  
  /*-----------------------------------------------------------------------*/
 
  switch (qmi_config_type)
  {
    case PDC_CONFIG_TYPE_MODEM_PLATFORM_V01:
      mcfg_config_type = MCFG_TYPE_HW;
      break;
      
    case PDC_CONFIG_TYPE_MODEM_SW_V01:
      mcfg_config_type = MCFG_TYPE_SW;
      break;
      
      
    case PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01:
    case PDC_CONFIG_TYPE_ENUM_MAX_ENUM_VAL_V01:
    default:
      mcfg_config_type = MCFG_NUM_OF_TYPES;
      break;
  }
  
  return mcfg_config_type;
} /* pdc_mcfg_from_qmi_config_type() */


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
)
{
  
  /*-----------------------------------------------------------------------*/
  
  if (!qmi_ind || !qmi_ind_size || !config_id)
  {
    return QMI_CSI_INTERNAL_ERR;
  }
  
  /* Check MCFG values to make sure they are valid */
  if (PDC_CONFIG_TYPE_ENUM_MAX_ENUM_VAL_V01 == pdc_qmi_from_mcfg_config_type(config_type))
  {
    return QMI_CSI_INTERNAL_ERR;
  }
  
  if (PDC_CONFIG_ID_SIZE_MAX_V01 <= config_id->id_len)
  {
    return QMI_CSI_INTERNAL_ERR;
  }
  
  /* Compose the IND payload */
  qmi_ind->new_config_info.config_type = pdc_qmi_from_mcfg_config_type(config_type);
  qmi_ind->new_config_info.config_id_len = config_id->id_len;
  memscpy(qmi_ind->new_config_info.config_id,
         PDC_CONFIG_ID_SIZE_MAX_V01,
         config_id->id,
         qmi_ind->new_config_info.config_id_len);
  
  *qmi_ind_size = sizeof(pdc_config_change_ind_msg_v01);

  return QMI_CSI_NO_ERR;
} /* pdc_qmi_compose_config_change_ind() */

