#ifndef TCXOMGR_IFC_INTERFACE_H
#define TCXOMGR_IFC_INTERFACE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                           T C X O    M A N A G E R

                       TCXOMGR IFC INTERFACE HEADER FILE

GENERAL DESCRIPTION
  This file contains the interface to TCXOMGR IFC module. Only this header
  file should be exposed to other modules. Care has to be taken that this
  header is not dependent on other module internal header files, to avoid
  circular dependencies

EXTERNALIZED FUNCTIONS
  ALL

REGIONAL FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/inc/tcxomgr_ifc_interface.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
07/28/14   tl      Clean up resources when shutting down Tcxomgr
11/30/11   ns      Intermediate field cal, initial version

============================================================================*/


/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
/* Make sure we dont dependecies on tcxomgr internal header files
   as it will lead to circular dependencies */
#include "tcxomgr.h"
#include "timetick.h"

/*============================================================================

                           Defines 

============================================================================*/

/* The following defines are exposed for logging functionality              */
/* Maximum buffers the buffer queue can hold                                */
#define TCXOMGR_IFC_BUFFER_MAX                        64  
/* Max rpush samples a buffer can hold                                      */
#define TCXOMGR_IFC_BUFFER_SIZE_MAX                   32 
/* RPUSH queue size                                                         */ 
#define TCXOMGR_IFC_RPUSH_SAMPLES_MAX               1024 



/*============================================================================

                           STRUCTURE DEFINITION 

============================================================================*/
/* Structure to hold rpush information */
typedef struct
{
  tcxomgr_temp_type xo_temp;        /* xo temp       */
  tcxomgr_temp_type pmic_temp;      /* pmic temp     */
  tcxomgr_ppm_type  rotator;        /* rotator info  */
  timetick_type     time;           /* time of rpush */
  boolean           traffic;        /* Traffic/idle  */
}tcxomgr_ifc_rpush_type;


/*============================================================================

FUNCTION TCXOMGR_IFC_INIT

DESCRIPTION
  Initializes the IFC module

DEPENDENCIES
  Must be called after fast field calibration is intialized 

RETURN VALUE
  None

SIDE EFFECTS
  Votes for RPUSH and Temp reads

============================================================================*/
void tcxomgr_ifc_init(void);


/*============================================================================

FUNCTION TCXOMGR_IFC_TERM

DESCRIPTION
  Terminates the IFC module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases all resources allocated by the IFC module

============================================================================*/
void tcxomgr_ifc_term(void);


/*============================================================================

FUNCTION TCXOMGR_IFC_EFS_FLUSH

DESCRIPTION
  Writes the EFS items necessary for IFC operation 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_ifc_efs_flush(void);


/*============================================================================

FUNCTION TCXOMGR_IFC_PROCESS_RPUSH

DESCRIPTION
  Takes in the new RPUSH and invokes the respective cal object. If field
  calibration kicked in, in-between will shut down IFC 

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  Might update c1 and quality indicator 

============================================================================*/
void tcxomgr_ifc_process_rpush
(
  tcxomgr_client_id_type client, 
  tcxomgr_ifc_rpush_type rpush_data
);

#endif /* TCXOMGR_IFC_INTERFACE_H */
