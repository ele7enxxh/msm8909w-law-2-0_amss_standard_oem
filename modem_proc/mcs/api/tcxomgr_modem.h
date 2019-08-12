#ifndef TCXOMGR_MODEM_H
#define TCXOMGR_MODEM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                T C X O    M A N A G E R    MODEM    API 
                     TCXO Manager Modem Header File

GENERAL DESCRIPTION

  This module contains API that are exposed to modem subsystem

EXTERNALIZED FUNCTIONS



REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS



  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/tcxomgr_modem.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/16/10   ns      Initial Version


============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/



/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/



/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_GET_RPUSH_UPDATE_INTERVAL

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  RPUSH update frequency in ms

SIDE EFFECTS

============================================================================*/

uint16 tcxomgr_get_rpush_update_interval(void);


#endif /* !TCXOMGR_MODEM_H */
