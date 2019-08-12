#ifndef SMD_V_H
#define SMD_V_H

/*===========================================================================

             Shared Memory Driver AU Private Header File


-----------------------------------------------------------------------------
Copyright (c) 2009-2013 Qualcomm Technologies Incorporated
All Rights Reserved. Qualcomm Proprietary and Confidential.
-----------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/smd/inc/smd_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/13   bt      Make SMD use secure SMEM partitions.
08/01/12   bt      Remove residual Block defines.
04/20/09   bfc     Initial version...Split smd.h into AU public and AU
                   private headers
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                      TYPE DECLARATIONS
                      
===========================================================================*/
#if !defined(SMD_NUM_CHANNELS)   
#define SMD_NUM_CHANNELS             64
#endif

#ifdef __cplusplus
}
#endif

#endif /* SMD_V_H */
