#ifndef PS_EAP_SIM_AKA_UME_H
#define PS_EAP_SIM_AKA_UME_H


/*===========================================================================

                   U S E R   M O D U L E   E N T I T Y
                            
                
                   
DESCRIPTION
  This file contains UME processing functions.
  The UME is responsible for providing a generic API for use of EAP SIM and 
  AKA, that interacts with the SIM/USIM and R(UIM) Implementation on the 
  system.
       
    
  Copyright (c) 2006-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
 
                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_sim_aka_ume.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/31/09    mga    Merged from eHRPD branch
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#ifdef  FEATURE_DATA_PS_EAP

#include "comdef.h"

#include "ps_eapi.h"
#include "ps_eap_aka_pb.h"

#include "ps_eap_sim_aka.h"
#include "ps_eap_sim_akai.h"
#include "ps_eap_sim_aka_ime.h"

#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#include "ps_svc.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                   MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#ifdef FEATURE_MMGSDI_SESSION_LIB

/*--------------------------------------------------------------------------
  Max supported MMGSDI sessions in EAP UME (4 = SS/DSDX, 6 = TSTX)
--------------------------------------------------------------------------*/
#ifdef FEATURE_TRIPLE_SIM
  #define  MAX_EAP_MMGSDI_SESSIONS  6
#else
  #define  MAX_EAP_MMGSDI_SESSIONS  4
#endif /* FEATURE_TRIPLE_SIM */

#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================

                             EXTERNAL DATA

===========================================================================*/

/*--------------------------------------------------------------------------
 Modes in which the UME can operate in
--------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_UME_MODE_SIM = 0,
  EAP_SIM_AKA_UME_MODE_AKA       = 1,
  EAP_SIM_AKA_UME_MODE_AKA_PRIME = 2
}eap_sim_aka_ume_mode_types;

/*--------------------------------------------------------------------------
  Type enumeration of possible contents to be retrieved from SFS.
--------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_UME_SFS_IMSI       = 0,
  EAP_SIM_AKA_UME_SFS_REAUTH_ID  = 1,
  EAP_SIM_AKA_UME_SFS_PSEUDONYM  = 2,
  EAP_SIM_AKA_UME_SFS_COUNTER    = 3,
  EAP_SIM_AKA_UME_SFS_TEKS       = 4,
  EAP_SIM_AKA_UME_SFS_MAX        = 5
} eap_sim_aka_ume_sfs_enum_type;

/*--------------------------------------------------------------------------
 The UME client context - able to identify a specific UME client
 user_data - needs to be persistent in case of async result
--------------------------------------------------------------------------*/
typedef struct
{
  /*True if IMSI field contains valid info, to be used by UME instead of card
    IMSI*/
  boolean                       imsi_present; 
  eap_sim_aka_ime_id_type       imsi;
  uint32                        min1;
  uint16                        min2;

  /*For id requests*/
  eap_sim_aka_ime_id_enum_type  id_type; 
  void*                        user_data;
  eap_sim_aka_ume_mode_types   mode; /* AKA or SIM */
}eap_sim_aka_ume_client_ctx;

/*--------------------------------------------------------------------------
 The possible UME results
--------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_UME_RES_FAILURE        = 0,
  EAP_SIM_AKA_UME_RES_SYNC_SUCCESS   = 1,
  EAP_SIM_AKA_UME_RES_ASYNC_RESPONSE = 2
}eap_sim_aka_ume_result_type;

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
STRUCT EAP_SESSION_INFO_TYPE()

DESCRIPTION
 Structure containing the following fields:
  a) session_type   - To indicate the type of session
  b) session_id     - Placeholder for session ID associated with session_type
  c) session_active - Indicates whether the session is active or not
  d) app_info_type  - App related information stored per session_id 
===========================================================================*/
typedef struct 
{
  mmgsdi_session_type_enum_type session_type; 
  mmgsdi_session_id_type        session_id;
  boolean                       session_active;
  boolean                       evt_cb_called;
  mmgsdi_app_info_type          app_info_type;
} eap_session_info_type;


/*===========================================================================
STRUCT MMGSDI_INFO_TYPE()

DESCRIPTION
 Structure containing the following fields:
  a) client_id    - Contains the client assc. with a particular MMGSDI client
  b) session_info - Array of mmgsdi_session_info_type structures containing
                      relevant information pertaining to every session of 
                      interest.
  c) client_ref   - Cookie provided to each client.
===========================================================================*/
typedef struct 
{
  mmgsdi_client_id_type             client_id;
  eap_session_info_type             session_info[MAX_EAP_MMGSDI_SESSIONS];
  mmgsdi_client_data_type           client_ref;
} mmgsdi_info_type;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================

               CALLBACK AND PROVIDER FUNCTION PROTOTYPES

===========================================================================*/

/*=========================================================================
 Prototype of callback functions for async response - registered by 
 the external clients with the UME 
===========================================================================*/

/*--------------------------------------------------------------------------
 callback function prototype for passing the identity
--------------------------------------------------------------------------*/
typedef void (*eap_sim_aka_ume_id_resp_cb_type)
(
  eap_sim_aka_ume_client_ctx*  context,
  eap_sim_aka_ume_result_type  result,
  eap_sim_aka_ime_id_type*     id
);

/*--------------------------------------------------------------------------
 callback function prototype for passing the results of SIM algo
--------------------------------------------------------------------------*/
typedef void (*eap_sim_aka_ume_sim_resp_cb_type)
(
  eap_sim_aka_ume_client_ctx*  context,
  eap_sim_aka_ume_result_type  result,
  uint8*                       sres,
  uint16                       sres_len,
  uint8*                       kc,
  uint16                       kc_len
);

/*--------------------------------------------------------------------------
 callback function prototype for passing the results of AKA algo
--------------------------------------------------------------------------*/
typedef void (*eap_sim_aka_ume_aka_resp_cb_type)
(
  eap_sim_aka_ume_client_ctx*  context,
  eap_sim_aka_ume_result_type  result,
  uint8*                       data,
  uint16                       data_len
);

/*=========================================================================
 Prototype of Callback functions for async response - internal, registered 
 by the UME with the provider function for SIM/USIM/RUIM requests.
===========================================================================*/

/*------------------------------------------------------------------------- 
 Callback function IMSI retrieval
---------------------------------------------------------------------------*/
typedef void (*eap_sim_aka_ume_get_imsi_cb_type)( uint8* id,  
                                                  uint16 max_id_len, 
                                        eap_sim_aka_ume_result_type status );

/*------------------------------------------------------------------------- 
 Callback function type for runing SIM algo that UME will register with the
 provider 
---------------------------------------------------------------------------*/
typedef void (*eap_sim_aka_ume_run_sim_algo_cb_type)(uint8*   sres,
                                                     uint16   sres_len,
                                                     uint8*   kc,
                                                     uint16   kc_len,
                                         eap_sim_aka_ume_result_type status);

/*------------------------------------------------------------------------- 
  Callback function type for runing AKA in case of USIM  algo that UME will 
  register with the provider 
---------------------------------------------------------------------------*/
typedef void (*eap_sim_aka_ume_run_usim_algo_cb_type)(uint8*  data,
                                                      uint16  data_len, 
                                          eap_sim_aka_ume_result_type status);

/*------------------------------------------------------------------------- 
 Callback function type for runing AKA algo that UME will register with the
 provider 
---------------------------------------------------------------------------*/
typedef void (*eap_sim_aka_ume_run_ruim_algo_cb_type)(uint8*  data,
                                                      uint16  data_len,
                                        eap_sim_aka_ume_result_type status );


/*=========================================================================
 UME - Introducing one layer of abstraction inside the UME so that we can
       easily stub the actual SIM/USIM/RUIM
       These are the prototypes of functions that will be registered with 
       the UME, that the UME will call in order to provide results for the 
       given requests - UME is not interested in the actual implementation
       of these functions.
===========================================================================*/

typedef struct 
{
  void*                            user_cb_ptr;
  eap_sim_aka_task_srv_req_type    task_srv_fct;
}eap_sim_aka_ume_provider_service_type;

/*-----------------------------------------------------------------------
  Struture to store the teks and teks_len
-------------------------------------------------------------------------*/
typedef struct
{
  /* Data structure to store TEKs (Temporary Keys)*/
  uint16     teks_len;
  uint8      teks[EAP_AKA_PRIME_MAX_ST_KEY_LEN];
}eap_sim_aka_teks_stored_data_type;

/*-------------------------------------------------------------------------
  Function prototype that needs to be registered with UME for providing
  IMSI
---------------------------------------------------------------------------*/
typedef eap_sim_aka_ume_result_type (*eap_sim_aka_ume_get_imsi_type)
                ( uint8* id,  uint16*               max_id_len,
                  eap_session_info_type*            eap_mmgsdi_session_info,
                  eap_sim_aka_ume_provider_service_type*  task_srv_info);

/*-------------------------------------------------------------------------
  Function prototype that needs to be registered with UME for providing
  SIM algo computation
---------------------------------------------------------------------------*/
typedef eap_sim_aka_ume_result_type (*eap_sim_aka_ume_run_sim_algo_type)
                ( eap_session_info_type *session_info, 
                  uint8* eap_rand, 
                  eap_sim_aka_ume_provider_service_type*  task_srv_info);

/*-------------------------------------------------------------------------
  Function prototype that needs to be registered with UME for providing
  AKA (UMTS) algo computation
---------------------------------------------------------------------------*/
typedef eap_sim_aka_ume_result_type (*eap_sim_aka_ume_run_usim_algo_type)
                ( eap_session_info_type *session_info, 
                  uint8* eap_rand, uint16 rand_len, 
                  uint8* autn, uint16 autn_len,
                  eap_sim_aka_ume_provider_service_type*  task_srv_info);

/*-------------------------------------------------------------------------
  Function prototype that needs to be registered with UME for providing
  AKA (1x) algo computation
---------------------------------------------------------------------------*/
typedef eap_sim_aka_ume_result_type (*eap_sim_aka_ume_run_ruim_algo_type)
                ( eap_session_info_type *session_info,
                  uint8* eap_rand, uint16 rand_len, 
                  uint8* autn, uint16 autn_len,
                  uint8* data, uint16* max_data_len,
                  eap_sim_aka_ume_provider_service_type*  task_srv_info);


/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_POWERUP_INIT

DESCRIPTION
  Initialize the UME software entity.

DEPENDENCIES
  None

PARAMETERS
  None.
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void 
eap_sim_aka_ume_powerup_init
(
  void
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_GET_ID

DESCRIPTION
  Return a specific Identiy.

DEPENDENCIES
  None

PARAMETERS
  context - the client context
  id_type - type of identity requested - Re-auth ID, Pseudonym, 
            Permanent(IMSI)
  id      - the actual identity returned is synchronous
  cb_ptr  - in case the call is asynchronous this cb function will be called 
            to pass the user the identity
  
RETURN VALUE
  - the type of the result - Failure or Success, Sync or Async

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type   
eap_sim_aka_ume_get_id
( 
  eap_sim_aka_ume_client_ctx*       context,
  eap_sim_aka_ime_id_enum_type      id_type,
  eap_sim_aka_ime_id_type*          id,
  eap_sim_aka_ume_id_resp_cb_type   cb_ptr,
  eap_sim_aka_task_srv_req_type    task_srv_fct
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_GET_SFS_INFO

DESCRIPTION
  Gets appropriate information from SFS required for fast re-authentication
  and other general purposes.

DEPENDENCIES
  None

PARAMETERS
  context - the client context
  uint8   - Enumeration map of the information to be retrieved
            from SFS.
  void*   - Pointer to data retrieved from EFS/SFS
  
RETURN VALUE
  - the type of the result - Failure or Success, it is always synchronous

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_ume_get_sfs_info 
(
  eap_sim_aka_ume_client_ctx*     context,
  eap_sim_aka_ume_sfs_enum_type   sfs_content,
  void                           *result
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_STORE_SFS_INFO

DESCRIPTION
  Store appropriate information in SFS required for fast re-authentication
  and other general purposes.

DEPENDENCIES
  None

PARAMETERS
  context - the client context
  uint8   - Enumeration map of the information to be retrieved
            from SFS.
  void*   - Pointer to data to store in EFS/SFS
  
RETURN VALUE
  - the type of the result - Failure or Success, it is always synchronous

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_ume_store_sfs_info 
(
  eap_sim_aka_ume_client_ctx*     context,
  eap_sim_aka_ume_sfs_enum_type   sfs_content,
  void                           *result
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_PROCESS_SIM_ALGO

DESCRIPTION
  Runs the SIM algorithm and returns the SRES and KC values

DEPENDENCIES
  None

PARAMETERS
  context - the client context
  eap_rand    - the rand value inside AT_RAND 
  sres    - out value - the computed SIM RES value
  kc      - out value - the computed crypto key
  cb_ptr  - in case the call is asynchronous this cb function will be called 
            to pass the res and kc values
  
RETURN VALUE
  - the type of the result - Failure or Success, Sync or Async

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_ume_process_sim_algo
(
  eap_sim_aka_ume_client_ctx*       context,
  uint8*                            eap_rand,
  uint16                            rand_len,
  uint8*                            sres,
  uint16*                           sres_len,
  uint8*                            kc,
  uint16*                           kc_len,
  eap_sim_aka_ume_sim_resp_cb_type  cb_ptr,
  eap_sim_aka_task_srv_req_type task_srv_fct
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_PROCESS_AKA_ALGO

DESCRIPTION
  Runs the AKA algorithm and returns the result data.

DEPENDENCIES
  None

PARAMETERS
  context - the client context
  eap_rand    - the rand value inside AT_RAND 
  autn    - the rand value inside AT_AUTN 
  data    - out value - the computed data ( can be the response, or auts 
            for syncro failure, or auth reject for errors )
  cb_ptr  - in case the call is asynchronous this cb function will be called 
            to pass the res and kc values
  
RETURN VALUE
  - the type of the result - Failure or Success, Sync or Async

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type   
eap_sim_aka_ume_process_aka_algo 
( 
  eap_sim_aka_ume_client_ctx*       context,
  uint8*                            eap_rand,
  uint16                            rand_len,
  uint8*                            autn,
  uint16                            autn_len,
  uint8*                            data,
  uint16*                           max_data_len,
  eap_sim_aka_ume_aka_resp_cb_type  cb_ptr,
    eap_sim_aka_task_srv_req_type task_srv_fct
);
/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_GET_REAUTH_ID

DESCRIPTION
  Used to obtain a re-authentication id if already stored in SFS/EFS.

DEPENDENCIES
  None.
    
PARAMETERS
  Not used by the default provider
  
RETURN VALUE
  Failure always
  
SIDE EFFECTS
  None  
===========================================================================*/
int 
eap_sim_aka_ume_get_reauth_id
(
  eap_sim_aka_ume_mode_types             mode, 
  uint8*                                 data,
  uint16*                                max_data_len,
  eap_handle_type                        eap_handle
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_GET_PSEUDONYM_ID

DESCRIPTION
  Used to obtain a pseudonym id if already stored in SFS/EFS.

DEPENDENCIES
  None.
    
PARAMETERS
  Not used by the default provider
  
RETURN VALUE
  Failure always
  
SIDE EFFECTS
  None  
===========================================================================*/
int 
eap_sim_aka_ume_get_pseudonym_id
(
  eap_sim_aka_ume_mode_types             mode, 
  uint8*                                 data,
  uint16*                                max_data_len,
  eap_handle_type                        eap_handle
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_NOTIFY_CORE_CLOSE

DESCRIPTION
  Used by SIM/AKA to notify UME during SIM/AKA core close to ensure proper
  clean up in UME and proper handling of late MMGSDI responses

DEPENDENCIES
  None.
    
PARAMETERS
  eap_handle of the closing SIM/AKA core

RETURN VALUE
  lengt of the filename 
  
SIDE EFFECTS
  None  
===========================================================================*/
void
eap_sim_aka_ume_notify_core_close
(
  eap_handle_type eap_handle
);

/*===========================================================================
FUNCTION UMEI_GET_SESSION_TYPE_FROM_SUBSCRIPTION

DESCRIPTION
  Helper function to obtain the session info from algo and slot

DEPENDENCIES
  None

PARAMETERS 
  uint8               subscription, 
  sim_aka_algo_type - Algorithm type
  
RETURN VALUE
  session_info ptr corresponding to the algo type / slot 

SIDE EFFECTS
    
===========================================================================*/
uint32 eap_ume_get_session_type_from_subscription
(
  uint8                      subscription,
  eap_sim_aka_algo_enum_type sim_aka_algo 
);

#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_SIM_AKA_UME_H */
