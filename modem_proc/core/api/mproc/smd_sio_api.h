#ifndef SMD_SIO_API_H
#define SMD_SIO_API_H
/*===========================================================================

     Shared Memory Driver SIO Interface Public Header File

 Copyright (c) 2013-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/mproc/smd_sio_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/22/13   bm      Expose API to get SIO port ID given SMD port name.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "sio.h"

/*===========================================================================
FUNCTION      smd_sio_get_sio_id_from_name

DESCRIPTION   Get SIO port ID for a given SMD port name
  
DEPENDENCIES  None

ARGUMENTS     const char * smd_port_name
               SMD port name (string)

RETURN VALUE  SIO port ID for the given SMD port name
              SIO_PORT_NULL : Error
   
SIDE EFFECTS  None
===========================================================================*/
sio_port_id_type smd_sio_get_sio_id_from_name
(
  const char * smd_port_name
);

#endif /* SMD_SIO_API_H */
