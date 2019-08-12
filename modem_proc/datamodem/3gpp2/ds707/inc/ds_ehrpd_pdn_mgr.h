
#ifndef DS_EHRPD_PDN_MGR_H
#define DS_EHRPD_PDN_MGR_H

/*===========================================================================

                   D A T A   S E R V I C E S   E H R P D 
                   P A C K E T   D A T A   N E T W O R K       
                                 M A N A G E R 
DESCRIPTION
  This module is responsible for allocating and managing the PDN IDs to be 
  used in eHRPD
  
Copyright (c) 2009 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds_ehrpd_pdn_mgr.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:03 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/09   spn     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_EHRPD

#ifndef FEATURE_HDR
#error "FEATURE_EHRPD defined without FEATURE_HDR being defined"
#endif /* FEATURE_HDR */


/*===========================================================================

                        EXTERNAL FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS_EHRPD_PDN_MGR_POWERUP_INIT

DESCRIPTION
  This function does the required initializations required for this module. 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_ehrpd_pdn_mgr_powerup_init
(
  void
);

/*===========================================================================

FUNCTION DS_EHRPD_PDN_MGR_ALLOCATE_PDN_ID

DESCRIPTION
  This function is called to allocate a PDN ID for a given PS_IFACE
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If a new PDN ID is allocated
          If TRUE pdn_id_ptr will be populated
  FALSE: If a new PDN ID cannot be allocated

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_ehrpd_pdn_mgr_allocate_pdn_id
(
  void           *pdn_info_ptr,
  uint8          *pdn_id_ptr
);

/*===========================================================================

FUNCTION DS_EHRPD_PDN_MGR_FREE_PDN_ID

DESCRIPTION
  This function is called to release a PDN ID so it can be reused.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If the PDN ID is successfully free'ed
  FALSE: If the PDN ID is not free'ed

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_ehrpd_pdn_mgr_free_pdn_id
(
  void*    pdn_info_ptr
);


/*=========================================================================== 
 
FUNCTION       DS_EHRPD_PDN_MGR_GET_PDN_ID
 
DESCRIPTION    
  This function is called to get a PDN ID for a given IFACE
   
DEPENDENCIES  
  None.
 
RETURN VALUE  
  TRUE: If a valid PDN ID is returned
          If TRUE pdn_id_ptr will be populated
  FALSE: If the PDN ID cannot be found
    
 
SIDE EFFECTS  
  This function does not de-register the callbacks.
===========================================================================*/ 
boolean ds_ehrpd_pdn_mgr_get_pdn_id
(
  void             *pdn_info_ptr,
  uint8            *pdn_id_ptr
);


/*=========================================================================== 
 
FUNCTION       DS_EHRPD_PDN_MGR_IS_PDN_ID_VALID
 
DESCRIPTION    
  This function is called to verify if the PDN ID is valid
   
DEPENDENCIES  
  None.
 
RETURN VALUE  
  TRUE: If the PDN id is valid
  FALSE: If the PDN ID is invalid
    
 
SIDE EFFECTS  
  This function does not de-register the callbacks.
===========================================================================*/ 
boolean ds_ehrpd_pdn_mgr_is_pdn_id_valid
(
  uint8        pdn_id
);

/*=========================================================================== 
 
FUNCTION       DS_EHRPD_PDN_MGR_GET_NUM_ALLOCATED_PDN
 
DESCRIPTION    
  Utility function to return the number of PDNs that are allocated. 
   
DEPENDENCIES  
  None.
 
RETURN VALUE  
  Returns the number of PDNs that are allocated. 
    
 
SIDE EFFECTS  
  None.
===========================================================================*/ 
uint8 ds_ehrpd_pdn_mgr_get_num_allocated_pdn
(
  void
);




#endif /* FEATURE_EHRPD */

#endif /* DS_EHRPD_PDN_MGR_H */
