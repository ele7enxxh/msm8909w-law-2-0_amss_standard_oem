#ifndef PS_EAP_SIM_AKA_IME_H
#define PS_EAP_SIM_AKA_IME_H


/*===========================================================================

          I D E N T I T Y   M A N E G M E N T   E N T I T Y
                            
                
                   
DESCRIPTION
  This file contains IME processing functions.
     
    
  Copyright (c) 2006-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_sim_aka_ime.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/25/12    fn     Added prefix for AKA PRIME
08/31/09    mga    Merged from eHRPD branch
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_EAP

#include "comdef.h"
#include "ps_eap.h"

/*===========================================================================

                            DATA TYPES

===========================================================================*/
#define EAP_SIM_AKA_IME_MAX_ID_LEN  EAP_IDENTITY_MAX_CHAR

/*The IMSI in case of EAP-SIM must be prefixed with 1 - RFC4186 - pg. 13 
 in order to obtain the user name of a permanent identity */
#define EAP_SIM_AKA_IME_SIM_PREFIX  0x31

/*The IMSI in case of EAP-AKA must be prefixed with 0 - RFC4187 - pg. 16
  in order to obtain the user name of a permanent identity */
#define EAP_SIM_AKA_IME_AKA_PREFIX  0x30

/*The IMSI in case of EAP-AKA PRIME must be prefixed with 6 - RFC5448- 
  pg. 15 in order to obtain the user name of a permanent identity */
#define EAP_SIM_AKA_IME_AKA_PRIME_PREFIX  0x36

/*--------------------------------------------------------------------------
  IME Result Enumeration
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_IME_RES_FAILURE        = 0,
  EAP_SIM_AKA_IME_RES_SYNC_SUCCESS   = 1,
  EAP_SIM_AKA_IME_RES_ASYNC_RESPONSE = 2,
  EAP_SIM_AKA_IME_NOT_INITIALIZED    = 3
}eap_sim_aka_ime_result_type;

/*--------------------------------------------------------------------------
  IME States
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_IME_NO_ID_REQ_ST      = 0,
  EAP_SIM_AKA_IME_REAUTH_ID_REQ_ST  = 1,
  EAP_SIM_AKA_IME_PSEUDONYM_REQ_ST  = 2,
  EAP_SIM_AKA_IME_PERM_ID_REQ_ST    = 3,
  EAP_SIM_AKA_IME_MAX_ST
}eap_sim_aka_ime_states;

/*--------------------------------------------------------------------------
  IME Modes
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_IME_MODE_SIM   = 0,
  EAP_SIM_AKA_IME_MODE_AKA         = 1,
  EAP_SIM_AKA_IME_MODE_AKA_PRIME   = 2
}eap_sim_aka_ime_modes_type;

/*--------------------------------------------------------------------------
  IME Identity Type Enumeration
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_IME_ID_PERMANET = 0,
  EAP_SIM_AKA_IME_ID_FULL     = 1,
  EAP_SIM_AKA_IME_ID_ANY      = 2
}eap_sim_aka_ime_id_enum_type;

/*--------------------------------------------------------------------------
  IME Identity Type
---------------------------------------------------------------------------*/
typedef struct
{
  eap_sim_aka_ime_id_enum_type  returned_id;
  uint16                        id_len;
  uint8                         id[EAP_SIM_AKA_IME_MAX_ID_LEN];
}eap_sim_aka_ime_id_type;

/*--------------------------------------------------------------------------
  IME Identity Response Callback 
---------------------------------------------------------------------------*/
typedef void (*eap_sim_aka_ime_id_resp_cback_type)
             ( eap_sim_aka_ime_id_enum_type  id_type, 
               eap_sim_aka_ime_id_type* id, void* user_data,
               eap_sim_aka_ime_result_type   result);

/*--------------------------------------------------------------------------
  IME Client Context
---------------------------------------------------------------------------*/
typedef struct
{
   eap_sim_aka_ime_modes_type          mode;
   eap_sim_aka_ime_states              current_state;
   void*                               client_data;
   eap_sim_aka_ime_id_resp_cback_type  cb_ptr;
   boolean                             any_id_cached;
   eap_sim_aka_ime_id_type             cached_id;
   eap_sim_aka_ime_id_enum_type        last_req_id_type;

   boolean                             prefix_id;
   boolean                             imsi_cached;
   eap_sim_aka_ime_id_type             imsi;

   void*                               task_srv_fct; 
   /*eap_sim_aka_task_srv_req_type*/


   /*Re-auth ID and Pseudonym obtained during the authentication
     that needs to be stored*/
   boolean                    reauth_id_set;
   eap_sim_aka_ime_id_type    reauth_id_tbst;

   boolean                    pseudonym_set;
   eap_sim_aka_ime_id_type    pseudonym_tbst;


   /*Cached values of re-auth id and pseudonym - for usage within the 
     same conversation*/
   boolean                    reauth_id_cached;
   eap_sim_aka_ime_id_type    reauth_id;

   boolean                    pseudonym_cached;
   eap_sim_aka_ime_id_type    pseudonym;

   /* UME context for each IME instance to run UME services*/
   void * ume_ctx_ptr;

}eap_sim_aka_ime_client_ctx;

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_IME_INIT

DESCRIPTION
  Initialize the IME software entity.

DEPENDENCIES
  None

PARAMETERS
  None.
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void 
eap_sim_aka_ime_init
(
  void
);

/*===========================================================================
FUNCTION EAP_EAP_SIM_AKA_IME_CACHE_PERMANENT_ID

DESCRIPTION
  Caches an IMSI.

DEPENDENCIES
  None

PARAMETERS
  context - current IME context
  id      - id to be cached as IMSI
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void 
eap_sim_aka_ime_cache_permanent_id
(
  eap_sim_aka_ime_client_ctx*      context, 
  uint8*                           provided_id,
  uint8                            provided_id_len
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_IME_PROCESS_ID_REQ

DESCRIPTION
  Called by the SIM/AKA core in order to process a request for Identity, the 
  response value will contain the identity that will be set as attribute value 
  AT_IDENTITY in the response packet. 
  In case the give identity is a re-authentication identity it will be deleted 
  from the secure storage and kept in cache for key processing until the 
  current client closes.

DEPENDENCIES
  UME Initialized

PARAMETERS
  context: context value able to uniquely identify a client
  id_type: type of identity requested (ANY/FULLAUTH/RE-AUTH)
  id: return value in case call is synchronous 
  cb_ptr : callback function in case call is asynchronous
  
RETURN VALUE
  Outcome of the call (SUCCESS/FAIL) and in case of SUCCESS type (SYNC/ASYNC)

SIDE EFFECTS
  Internal client state will will change.
  
===========================================================================*/
eap_sim_aka_ime_result_type   
eap_sim_aka_ime_process_id_req
( 
  eap_sim_aka_ime_client_ctx*    context,
  eap_sim_aka_ime_id_enum_type   id_type,
  eap_sim_aka_ime_id_type*       id
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_IME_GET_ID_REQ

DESCRIPTION
  Called by the SIM/AKA core in order to obtain the current identity for use 
  in key computation.

DEPENDENCIES
  UME Initialized

PARAMETERS
 context: context value able to uniquely identify a client
 id: return value in case call is synchronous 

  
RETURN VALUE
  Outcome of the call (SUCCESS/FAIL) and in case of SUCCESS type (SYNC)

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_ime_result_type   
eap_sim_aka_ime_get_id_req
( 
  eap_sim_aka_ime_client_ctx*  context,
  eap_sim_aka_ime_id_type*        id
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_IME_SET_ID_REQ

DESCRIPTION
  Called by the SIM/AKA core in order to save a re-authentication identity or 
  pseudonym obtained during SIM/AKA conversation.
    
DEPENDENCIES
  None

PARAMETERS
  context: context value able to uniquely identify a client
  id_type: type of identity to be saved (FULLAUTH/RE-AUTH)
  id: the identity to be saved
  
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void   
eap_sim_aka_ime_set_id_req
(
  eap_sim_aka_ime_client_ctx*  context,
  eap_sim_aka_ime_id_enum_type id_type,
  eap_sim_aka_ime_id_type*     id 
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_IME_RESET

DESCRIPTION
  Called by the SIM/AKA core when closing in order to reset the internal 
  state machine for the given client. Will also delete any cached values.
    
DEPENDENCIES
  None

PARAMETERS
  context: context value able to uniquely identify a client
  
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void   
eap_sim_aka_ime_reset
(
  eap_sim_aka_ime_client_ctx*  context
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_IME_STORE

DESCRIPTION
  Called by the SIM/AKA core a result indication was received from EAP 
  notifying of success. At this point the IME will store the value of the 
  re-authentication and pseudonym in a permanent storage. 
  The function will further call the reset function in order to reinitialize 
  the state machine for the current client. 
    
DEPENDENCIES
  None

PARAMETERS
  context: context value able to uniquely identify a client
  
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_ime_result_type 
eap_sim_aka_ime_store
(
  eap_sim_aka_ime_client_ctx*  context
);

#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_SIM_H */
