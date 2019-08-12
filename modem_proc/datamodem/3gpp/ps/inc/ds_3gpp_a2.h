/*!
  @file
  ds_3gpp_a2.h

  @brief
  REQUIRED brief header file for a2 related routines.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_a2.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/10   vk      Initial version
===========================================================================*/

#ifndef DS_3GPP_A2_H
#define DS_3GPP_A2_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_A2

#include "sio.h"

/*===========================================================================

                   EXTERNAL FUNCTION DECLARATIONS

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
);

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
);

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
);

#endif /* FEATURE_DATA_A2 */

#endif /* DS_3GPP_A2_H */

