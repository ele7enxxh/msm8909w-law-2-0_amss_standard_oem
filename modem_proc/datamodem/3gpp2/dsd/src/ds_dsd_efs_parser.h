#ifndef DS_DSD_EFS_PARSER_H
#define DS_DSD_EFS_PARSER_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                       D A T A   S E R V I C E S 
                        DSD EFS PARSER HEADER FILE

GENERAL DESCRIPTION

  All the declarations and definitions necessary to support parsing  
  of DSD related EFS file.

Copyright (c) 2003-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==================================================================
                        EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_efs_parser.h#1 $

when       who         what, where, why
--------   -----       ----------------------------------------------------------
11/07/11   var         Checkin for first version of dsd_efs_parser.h

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "comdef.h"

/*===========================================================================
                              MACROS
===========================================================================*/


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_EFS_PARSER_INIT

DESCRIPTION   This function initializes the EFS parser. This is during 
              power up. 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  1. Reads EFS file is present.
===========================================================================*/
void ds_dsd_efs_parser_init(void);

#endif  /* DS_DSD_EFS_PARSER_H */

