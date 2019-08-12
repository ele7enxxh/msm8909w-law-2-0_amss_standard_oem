#ifndef DSEPSESMIF_H
#define DSEPSESMIF_H
/*===========================================================================

                 L T E  D A T A  E S M  H E A D E R   F I L E


DESCRIPTION
  This file contains data declarations and function prototypes for LTE
  Packet-Switched Data data processing routines which are related to PS 
  protocols 
  
  All EPS ESM interface defentions are declared in this file.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/inc/dsepsesmif.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/10   vd      Replaced lte_nas_esm_message_description.h with cm_gw.h
01/09/09   vd      Intial version.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "cm_gw.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Commenting out this function as this should not be used anymore.
   Eventually we will delete this file once we ensure that the header file
   is not included any more.
*/

#if 0
/*===========================================================================
FUNCTION ds_eps_get_pdn_connectivity_msg()

DESCRIPTION
  Generates the protocol config options- PCO and other parameters used for 
  the PDN connectivity message sent by ESM as part of the attach procedure.
  
PARAMETERS  
  Pointer to pdn connection data to filled by DS EPS.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_eps_get_pdn_connectivity_msg
(
  pdn_connection_data_T *pdn_connection_data_ptr 
                                         /* Pointer to pdn connection data */ 
);
#endif

#endif /* DSEPSESMIF_H */
