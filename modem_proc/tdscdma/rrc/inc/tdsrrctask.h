#ifndef TDSRRCTASK_H
#define TDSRRCTASK_H
/*===========================================================================
                        R R C    T A S K  

DESCRIPTION

   This file contains data types and declarations associated with the RRC
   task entry point and maintaining RRC state.
   
      
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
Export of this technology or software is regulated by the U.S. Government. 
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rrc/inc/tdsrrctask.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/11   ysh   Merged with VU_MODEM_WCDMA_RRC_OSYS.01.93.53

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "tdscdma_variation.h"
#ifndef FEATURE_CMI
/* -------------------------------------- */
/* PUBLIC API of tdsrrctask.h               */
/* -------------------------------------- */
/*===========================================================================

FUNCTION TDS_RRC_TASK

DESCRIPTION
  This function is the entry point for RRC Task. It initializes RRC state
  and individual procedures by calling tdsrrc_init(). The ACK signal is sent back
  to TMC and a wait is done on Task Start Signal from TMC. Once a Task Start
  Signal is received, tdsrrc_dispatcher() is called that takes control now onwards.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void  tds_rrc_task(dword param);

/*===========================================================================

FUNCTION TDSRRC_INIT

DESCRIPTION
  This function does the initialization for RRC layer. It means setting the 
  state and mode of RRC and initializing all RRC global data. It calls 
  tdsrrctask_init_procedures() to initialize each procedure specific data.

  NOTE: We may add NV read here later. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsrrc_init( void );

#else
/* Public portion of the API exists in tdsmvsif.h */
#include "tdstmcif.h"

#endif /* FEATURE_CMI */

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */


#ifdef FEATURE_MODEM_RCINIT
/*===========================================================================

FUNCTION TDSRRC_TASK_INIT

DESCRIPTION
  This function does the initialization for mutex of RRC layer under
  RC_INIT control
  
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tds_rrc_task_init(void);
#endif /*FEATURE_MODEM_RCINIT*/


#endif /* TDSRRCTASK_H */

