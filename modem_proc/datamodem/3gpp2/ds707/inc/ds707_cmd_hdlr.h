#ifndef DS707_CMD_HDLR_H
#define DS707_CMD_HDLR_H
/** 
  @file ds707_cmd_hdlr.h
  @brief 
   Handles 3GPP2 MH specific commands
*/
/** Contains API's to handle commands specific to 3GPP2 MH */

/*===========================================================================

                         D S 7 0 7 _ C M D _ H D L R
GENERAL DESCRIPTION
  This file contains the 3GPP2 MH command handling functionality.

EXTERNALIZED FUNCTIONS

INTERNAL FUNCTIONS 

 Copyright (c) 2014 - 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_cmd_hdlr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ------------------------------------------------------- 
03/14/14   jee     Created Module
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/


/*===========================================================================
                          PUBLIC DATA DECLARATIONS
===========================================================================*/


/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/\
/*===========================================================================
FUNCTION      DS707_CMD_HDLR_INIT

DESCRIPTION   Called once at power-up to init 3GPP2 MH cmd hdlr
 
DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_cmd_hdlr_init
(
  void
);

#endif /* DS707_CMD_HDLR_H */
