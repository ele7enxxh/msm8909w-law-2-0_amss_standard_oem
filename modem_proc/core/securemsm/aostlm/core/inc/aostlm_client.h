#ifndef AOSTLM_CLIENT_H
#define AOSTLM_CLIENT_H

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    A O S T L M   S U B S Y S T E M

GENERAL DESCRIPTION
   The AOSTLM client allows features to request validation of AOST licenses
   for features.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/aostlm/core/inc/aostlm_client.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/2014   eh      initial version 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/**
 * @brief Starts the AOSTLM Client.  Enables QMI and populates license cache
 * 
 * @return none
 */
void aostlm_client_startup(void);

/**
 * @brief Stops the AOSTLM client.  Clears the license cache and 
 *        disconnects from QMI
 * 
 * @return none
 */
void aostlm_client_shutdown(void);

#endif
