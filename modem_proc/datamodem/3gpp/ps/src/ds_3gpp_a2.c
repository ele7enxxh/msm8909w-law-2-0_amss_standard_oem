
/*!
  @file
  ds_3gpp_a2.c

  @brief
  This contains code related to interaction with A2 driver.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_a2.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/10   vk      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DATA_A2

#include "ds_3gpp_a2.h"

/*===========================================================================

                       INTERNAL DEFINITIONS AND TYPES

===========================================================================*/



/*===========================================================================

                              LOCAL VARIABLES

===========================================================================*/


/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/



/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_3GPP_A2_MODEM_PORT

DESCRIPTION
  Checks if passed SIO port is an A2 modem port. 

PARAMETERS
  sio_port - SIO Port ID to check

RETURN VALUE
  TRUE, if port is an A2 modem port, FALSE otherwise

DEPENDENCIES  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_3gpp_a2_modem_port
(
  sio_port_id_type sio_port
)
{
  boolean rval = FALSE;

  switch (sio_port)
  {
    case SIO_PORT_A2_MDM:
    case SIO_PORT_SDIO_MUX_A2_DUN_0:
      rval = TRUE;
      break;
    default:
      /* Do nothing
      */
      break;
  }

  return rval;
}

/*===========================================================================
FUNCTION      DS_3GPP_A2_RMNET_PORT

DESCRIPTION
  Checks if passed SIO port is an A2 RMNET port. 

PARAMETERS
  sio_port - SIO Port ID to check

RETURN VALUE
  TRUE, if port is an A2 RMNET port, FALSE otherwise

DEPENDENCIES  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_3gpp_a2_rmnet_port
(
  sio_port_id_type sio_port
)
{
  boolean rval = FALSE;

  switch (sio_port)
  {
    case SIO_PORT_A2_RMNET_1:
      /*lint -fallthrough */
    case SIO_PORT_A2_RMNET_2:
    case SIO_PORT_A2_RMNET_3:
    case SIO_PORT_SDIO_MUX_A2_RMNET_0:
    case SIO_PORT_SDIO_MUX_A2_RMNET_1:
    case SIO_PORT_SDIO_MUX_A2_RMNET_2:
    case SIO_PORT_SDIO_MUX_A2_RMNET_3:
    case SIO_PORT_SDIO_MUX_A2_RMNET_4:
    case SIO_PORT_SDIO_MUX_A2_RMNET_5:
    case SIO_PORT_SDIO_MUX_A2_RMNET_6:
    case SIO_PORT_SDIO_MUX_A2_RMNET_7:
    case SIO_PORT_SDIO_MUX_A2_RMNET_8:
    case SIO_PORT_SDIO_MUX_A2_RMNET_9:
#if defined(FEATURE_RMNET_PORT_CONFIG_MDM_LE)
    case SIO_PORT_SDIO_MUX_A2_RMNET_10:
#endif /* FEATURE_RMNET_PORT_CONFIG_MDM_LE */
    case SIO_PORT_SDIO_MUX_A2_RMNET_TETH_0:
    case SIO_PORT_MUX_A2_CH_0:
    case SIO_PORT_MUX_A2_CH_1:
    case SIO_PORT_MUX_A2_CH_2:
    case SIO_PORT_MUX_A2_CH_3:
    case SIO_PORT_MUX_A2_CH_4:
    case SIO_PORT_MUX_A2_CH_5:
    case SIO_PORT_MUX_A2_CH_6:
    case SIO_PORT_MUX_A2_CH_7:
      rval = TRUE;
      break;
    default:
      /* Do nothing
      */
      break;
  }

  return rval;
}

/*===========================================================================
FUNCTION      DS_3GPP_A2_PORT

DESCRIPTION
  Checks if passed SIO port is an A2 port. 

PARAMETERS
  sio_port - SIO Port ID to check

RETURN VALUE
  TRUE, if port is an A2 port, FALSE otherwise

DEPENDENCIES  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_3gpp_a2_port
(
  sio_port_id_type sio_port
)
{
  return (ds_3gpp_a2_modem_port(sio_port) || ds_3gpp_a2_rmnet_port(sio_port));
}

#endif /* FEATURE_DATA_A2 */

/*===========================================================================

                                UNIT TEST

===========================================================================*/

