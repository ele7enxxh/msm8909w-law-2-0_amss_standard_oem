#ifndef SMD_DSM_V_H
#define SMD_DSM_V_H

/*===========================================================================

            Shared Memory Driver Watermark AU Private Header File


 Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/smd/inc/smd_dsm_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/09   bfc     Initial version...Split smd_dsm.h into AU public and AU
                   private headers
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smd_wm_init

DESCRIPTION   Initialize the data structures internal to this subsystem.
              Must be called before any other APIs in this subsystem are
              called.

ARGUMENTS     None.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
void smd_wm_init( void );

#ifdef __cplusplus
}
#endif

#endif /* SMD_DSM_V_H */
