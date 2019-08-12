#ifndef PS_EAP_CLIENT_H
#define PS_EAP_CLIENT_H


/*===========================================================================
                       E A P  C L I E N T  M O D U L E
                            
                   
DESCRIPTION
  This file contains client configuration processing functions for EAP clients.
     
    
  Copyright (c) 2014-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_client_config.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/10/14    fn      Created module.

===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DATA_PS_EAP
#include "ps_eap.h"
/*===========================================================================

                    EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  EAP client specific 
---------------------------------------------------------------------------*/
typedef struct
{
  boolean reauth_realm_inherit;
}eap_client_config_cb_type;

typedef enum
{
  EAP_SUBS_PRIMARY   = 0x01,
  EAP_SUBS_SECONDARY = 0x02,
  EAP_SUBS_TERTIARY  = 0x03,
  EAP_SUBS_MAX
} eap_client_config_subs_e_type;

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION EAP_CLIENT_CONFIG_INIT

DESCRIPTION
  Initializes the EAP client control blocks.This function should be called
 only once during configuration after eap_create_instance is called
 
DEPENDENCIES 
  valid eap_handle

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void eap_client_config_init
(
  eap_handle_type       eap_handle,
  eap_client_enum_type  eap_client_type,
  uint32                eap_client_enable_spec_mask
);

/*===========================================================================

FUNCTION EAP_CLIENT_DELETE_CONFIG

DESCRIPTION
  Deallocates resources for corresponding client configuration
 
DEPENDENCIES 
  valid eap_handle

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void eap_client_delete_config
(
  eap_handle_type       eap_handle
);

/*===========================================================================

FUNCTION EAP_CLIENT_CONFIG_REAUTH_REALM

DESCRIPTION
  Deallocates resources for corresponding client configuration
 
DEPENDENCIES 
  valid eap_handle

RETURN VALUE
  TRUE/FALSE for setting

SIDE EFFECTS

===========================================================================*/
boolean eap_client_config_reauth_realm
(
  eap_handle_type eap_handle
);

/*===========================================================================

FUNCTION EAP_CLIENT_CONFIG_GET_SESSION_TYPE

DESCRIPTION
  Get the session type from subscription and algo type. If user does not 
 have a preference eap_sim_aka_algo then EAP_SIM_AKA_ALGO_NONE should be 
 passed in.
 
DEPENDENCIES 
  

RETURN VALUE
  uint32 session type
 
SIDE EFFECTS

===========================================================================*/
uint32 eap_client_config_get_session_type
(
  eap_client_config_subs_e_type  subscription,
  eap_sim_aka_algo_enum_type     eap_sim_aka_algo
);

#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_CLIENT_H */
