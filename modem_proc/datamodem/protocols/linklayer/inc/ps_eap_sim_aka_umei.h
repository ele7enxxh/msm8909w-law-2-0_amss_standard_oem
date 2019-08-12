#ifndef PS_EAP_SIM_AKA_UMEI_H
#define PS_EAP_SIM_AKA_UMEI_H


/*===========================================================================

          U S E R   M O D U L E   E N T I T Y   I N T E R N A L S
                            
                
                   
DESCRIPTION
  This file contains UME Internal processing functions.
  The UME is responsible for providing a generic API for use of EAP SIM and 
  AKA, that interacts with the SIM/USIM and R(UIM) Implementation on the 
  system.
       
    
  Copyright (c) 2006-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
 
                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_sim_aka_umei.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/12/12    manum  Increased MAX_UME_CLIENTS to match EAP instances 
08/31/09    mga    Merged from eHRPD branch
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#ifdef  FEATURE_DATA_PS_EAP 

#include "comdef.h"
#include "ps_eap_sim_aka_ime.h"
#include "ps_eap_sim_aka_ume.h"
#include "ps_eapi.h"
#include "ps_eap_aka_pb.h"

/*===========================================================================

                             INTERNAL DATA

===========================================================================*/
/*-------------------------------------------------------------------------
  Number of UME clients that can be queued at one time - calls from clients 
  when queue is full will fail 
---------------------------------------------------------------------------*/
/* arbitrary */
#define MAX_UME_CLIENTS EAP_MAX_INSTANCES 

/*--------------------------------------------------------------------------
  UME - Internal States
--------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_UME_ST_BUSY = 0,
  EAP_SIM_AKA_UME_ST_FREE = 1
}eap_sim_aka_ume_state;

/*-------------------------------------------------------------------------
  SIM/USIM/RUIM Module states 
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_UME_MMGSDI_INIT     = 0,
  EAP_SIM_AKA_UME_MMGSDI_OPENING  = 1,
  EAP_SIM_AKA_UME_MMGSDI_OPENED   = 2
}eap_sim_aka_ume_state_enum;


/*--------------------------------------------------------------------------
 Client data stored for a SIM request
 In case of EAP-SIM the challenge will contain more than one RAND - up to 3
 SRES and KC need to be computed for each one of them and their concatenation
 passed as result
--------------------------------------------------------------------------*/
typedef struct 
{
  uint8                      total_rand;
  uint8                      current_rand;
  eap_sim_aka_rand_type      eap_rand[EAP_SIM_MAX_RAND_NO];
  eap_sim_aka_len_val_type   sres[EAP_SIM_MAX_RAND_NO];
  eap_sim_aka_len_val_type   kc[EAP_SIM_MAX_RAND_NO];
}eap_sim_aka_ume_sim_algo_req_type;

/*--------------------------------------------------------------------------
 Client data stored for an AKA request
--------------------------------------------------------------------------*/
typedef struct 
{
  uint8  eap_rand[EAP_SIM_AKA_MAX_RAND_LEN];
  uint8  autn[EAP_AKA_MAX_AUTN];
}eap_sim_aka_ume_aka_algo_req_type;

/*--------------------------------------------------------------------------
 Client data stored for queued requests
--------------------------------------------------------------------------*/
typedef union
{
  eap_sim_aka_ime_id_type            id;
  eap_sim_aka_ume_sim_algo_req_type  sim;
  eap_sim_aka_ume_aka_algo_req_type  aka;
}eap_sim_aka_ume_req_data_type;

/*--------------------------------------------------------------------------
  Type enumeration of possible requests
--------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_UME_SIM_ID_REQ        = 0,
  EAP_SIM_AKA_UME_USIM_ID_REQ       = 1,
  EAP_SIM_AKA_UME_RUIM_ID_REQ       = 2,
  EAP_SIM_AKA_UME_SIM_ALGO_REQ      = 3,
  EAP_SIM_AKA_UME_USIM_ALGO_REQ     = 4,
  EAP_SIM_AKA_UME_RUIM_ALGO_REQ     = 5,
  EAP_SIM_AKA_UME_ID_REQ            = 6
}eap_sim_aka_ume_type_req_enum;

/*--------------------------------------------------------------------------
 Client structure for queued requests
 cb_ptr - can be ume_aka_resp_cb_type, ume_sim_resp_cb_type or 
          ume_id_resp_cback_type
--------------------------------------------------------------------------*/
typedef struct
{
  /* Queue link  */
  q_link_type                    link;     

  /*client context of the initial call */
  eap_sim_aka_ume_client_ctx     cl_ctx;

  /*cb ptr registered by the UME for async response */
  void*                          cb_ptr;

  /* data saved from the initial request */
  eap_sim_aka_ume_req_data_type  req_data;

  /*type of the request queued */
  eap_sim_aka_ume_type_req_enum  type;

  /*session info of the queued request*/
  eap_session_info_type*          eap_session_info;

  /*task service info*/
  eap_sim_aka_ume_provider_service_type  task_srv_info;

}eap_sim_aka_ume_client_type;

/*--------------------------------------------------------------------------
 UME - Individual Provider Structure - such as SIM/USIM/RUIM/SFS
--------------------------------------------------------------------------*/
typedef struct
{
  /*state of the provider */
  eap_sim_aka_ume_state           state;

  /* Clients queue */
  q_type                          clients_q;  

  /*Pending client in case of asynchroneous call */
  eap_sim_aka_ume_client_type     pending_client;

} eap_sim_aka_ume_provider_type;

typedef struct
{
  uint8  data_len;
  uint8  data[EAP_SIM_AKA_MAX_DATA];
}eap_sim_aka_ume_read_data_type;

#ifdef FEATURE_MMGSDI_SESSION_LIB
typedef struct
{
  mmgsdi_return_enum_type   status;
  mmgsdi_cnf_enum_type      cnf;
  union
  {
    eap_sim_aka_ume_read_data_type          read_data;
    mmgsdi_get_file_attr_cnf_type           get_file_attr_cnf;
  } mmgsdi_resp_data;
  void*                                   user_cb_ptr;
}eap_sim_aka_umei_mmgsdi_response;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#ifdef FEATURE_DATA_PS_EAP_AKA_2GRUIM
typedef enum eap_sim_aka_umei_2gruim_cmd_type {
  EAP_AKA_2GRUIM_CMD_NULL     = 0,
  EAP_AKA_2GRUIM_CMD_KEYSN    = 1,
  EAP_AKA_2GRUIM_CMD_KEYSM    = 2,
  EAP_AKA_2GRUIM_CMD_KEYSM_ME = 3
} eap_sim_aka_umei_2gruim_cmd_type;
#endif /* FEATURE_DATA_PS_EAP_AKA_2GRUIM */

/*--------------------------------------------------------------------------
 UME - Control Block
--------------------------------------------------------------------------*/
typedef struct
{
  /*-----------------------------------------------------------------------
    client id and session info registered with mmgsdi lib
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_info_type                    mmgsdi_info;

  /*-----------------------------------------------------------------------
    Current auth context
  -------------------------------------------------------------------------*/
  //authentication_context_T            current_auth_request_context;

  /*-----------------------------------------------------------------------
   Current response received - used for keeping data while task switching
  -------------------------------------------------------------------------*/
  mmgsdi_session_run_gsm_algo_cnf_type  run_gsm_alg_cnf; 
  mmgsdi_usim_auth_cnf_type             usim_auth_cnf; 

  eap_sim_aka_umei_mmgsdi_response    mmgsdi_response;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

  /* UME provider specific structure */
  eap_sim_aka_ume_provider_type       ume_provider;

  /* UME core closed notification flag */
  boolean                             ume_sim_aka_core_closed; 
 
  /*-----------------------------------------------------------------------
    Pointer to a function UME can call to retrieve IMSI - reg with UME 
    at startup 
  -------------------------------------------------------------------------*/
  eap_sim_aka_ume_get_imsi_type       imsi_ret_ptr;

  /*-----------------------------------------------------------------------
    Pointer to a function UME can call to run SIM algo - reg with UME 
    at startup 
  -------------------------------------------------------------------------*/
  eap_sim_aka_ume_run_sim_algo_type   sim_algo_ptr;

  /*-----------------------------------------------------------------------
    Pointer to a function UME can call to run USIM algo - reg with UME 
    at startup 
  -------------------------------------------------------------------------*/
  eap_sim_aka_ume_run_usim_algo_type  usim_auth_ptr;

  /*-----------------------------------------------------------------------
    Pointer to a function UME can call to run CSIM algo - reg with UME 
    at startup 
  -------------------------------------------------------------------------*/
  eap_sim_aka_ume_run_ruim_algo_type  csim_auth_ptr;
  /*-----------------------------------------------------------------------
    Pointer to a function UME can call to run RUIM algo - reg with UME 
    at startup 
  -------------------------------------------------------------------------*/
  eap_sim_aka_ume_run_ruim_algo_type  ruim_auth_ptr;

  uint8                               aka_result[256]; 

#ifdef FEATURE_DATA_PS_EAP_AKA_2GRUIM
  struct eap_aka_2gruim_data {
    uint8 RANDM_ME[8];
    uint8  SQN_ME[5];
    uint8 SQN[5];
    uint8 RANDM[8];
    uint8 MAC[8];
    uint8 RANDN[12];
    uint8 RANDU[3];
    uint8 keysm[16];
    uint8 keysn[16];
    eap_sim_aka_ume_run_ruim_algo_cb_type cb_ptr;
    eap_sim_aka_task_srv_req_type         task_srv_fct; 
    uint8 autn[16];
    uint8 eap_rand[16];
    uint8 authr[3];
    boolean first_resync; 
    uint32 min1;
    uint16 min2;
    eap_session_info_type* session_info;
  } eap_aka_2gruim_data;
#endif /* FEATURE_DATA_PS_EAP_AKA_2GRUIM */

}eap_sim_aka_ume_cb_type;


/*-----------------------------------------------------------------------
  Struture of the stored file 
-------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  /*IMSI - used to match the store file with the card */
  uint16     imsi_len;
  uint8      imsi[EAP_SIM_AKA_IME_MAX_ID_LEN];

  /*Re-authentication Id used for F.R. procedure */
  uint16     reauth_id_len;
  uint8      reauth_id[EAP_SIM_AKA_IME_MAX_ID_LEN];

  /*Pseudonym - used for privacy support */
  uint16     pseudonym_len;
  uint8      pseudonym[EAP_SIM_AKA_IME_MAX_ID_LEN];

  /*TEKs (Temporary Keys) and F.R. Counter  */
  uint16     teks_len;
  uint8      teks[EAP_AKA_PRIME_MAX_ST_KEY_LEN];
  uint16     counter;
}eap_sim_aka_stored_data_type;


/*=========================================================================
                         Internal Functions
===========================================================================*/

/*=========================================================================
   UME ID Operation functions
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_GET_IMSI_HLP

DESCRIPTION
  Helper method - to unify identity retrieval for all modules. 

DEPENDENCIES
  None

PARAMETERS
  id      - the actual identity returned is synchronous
  cb_ptr  - in case the call is asynchronous this cb function will be called 
            to pass the user the identity
  state   - state of the UME module - will change to busy if call is SYNC 
  type    - the type of permanent id to be retrieved - SIM/USIM/RUIM 
  
RETURN VALUE
  - the type of the result - Failure or Success, Sync or Async

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type   
eap_sim_aka_umei_get_imsi_hlp
( 
  /* the caller ctx */
  eap_sim_aka_ume_client_ctx*       context,

  /* return id if async */
  eap_sim_aka_ime_id_type*          id,

  /* pointer to the cb of the initial client */
  eap_sim_aka_ume_id_resp_cb_type     ume_cb_ptr,

  /*  */
  eap_sim_aka_task_srv_req_type   task_srv_fct
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_GET_IMSI_FROM_CARD_HLP

DESCRIPTION
  To read the IMSI from the card since the EAP's client has not done it

DEPENDENCIES
  None

PARAMETERS
  context - caller contex
  id      - the actual identity returned is synchronous
  cb_ptr  - in case the call is asynchronous this cb function will be called 
            to pass the user the identity
  type    - the type of permanent id to be retrieved - SIM/USIM/RUIM 

  ume_cb_ptr - UME client cb pointer
  module     - pointer to the actual module structure 

  
RETURN VALUE
  - the type of the result - Failure or Success, Sync or Async

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type   
eap_sim_aka_umei_get_imsi_from_card_hlp
( 
  /* the caller ctx */
  eap_sim_aka_ume_client_ctx*       context,

  /* return id if async */
  eap_sim_aka_ime_id_type*          id,

  /* session info ptr */
  eap_session_info_type*            session_info,

  /* pointer to the cb of the initial client */
  eap_sim_aka_ume_id_resp_cb_type   ume_cb_ptr,

  /* pointer to the cb of the initial client */
  eap_sim_aka_task_srv_req_type    task_srv_fct
);

/*=========================================================================
   UME ID Internal Callbacks
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_ID_REQ_HLP_CB

DESCRIPTION
  Processing common logic for SIM/USIM/RUIM Callback functions.

DEPENDENCIES
  None

PARAMETERS
  identity - the actual identity
  id_len   - the identity len 
  status   - the status of the request - success or failure 
  provider - ptr to the actual provider being processed 
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void 
eap_sim_aka_umei_id_req_hlp_cb
(
  uint8*                          identity,
  uint16                          id_len,
  eap_sim_aka_ume_result_type     status
);


/*=========================================================================
   UME Algorithm Operation functions
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_RUN_SIM_ALGO_N_TIMES

DESCRIPTION
  Runs the SIM algorithm for each random value given as input and returns 
  the SRES and KC values

DEPENDENCIES
  None

PARAMETERS
  sim - pointer to the sim structure of the requested data of a pending client
  sres    - out value - the computed SIM RES value
  kc      - out value - the computed crypto key
  
RETURN VALUE
  - the type of the result - Failure or Success, Sync or Async

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_run_sim_algo_n_times
(
  eap_sim_aka_ume_sim_algo_req_type* sim,
  uint8*                             sres,
  uint16*                            sres_len,
  uint8*                             kc,
  uint16*                            kc_len
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_SIM_ALGO_REQ_CB

DESCRIPTION
  Callback function to be called by SIM module when result is available

DEPENDENCIES
  None

PARAMETERS
  sres, sres_len - computed res and its len
  kc, kc_len - computed kc and its len
  status   - failure or success
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void 
eap_sim_aka_umei_sim_algo_req_cb
(
  uint8*                       sres,
  uint16                       sres_len,
  uint8*                       kc,
  uint16                       kc_len,
  eap_sim_aka_ume_result_type  status
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_AKA_ALGO_REQ_CB

DESCRIPTION
  Callback function to be called by RUIM or USIM module when result is 
  available

DEPENDENCIES
  None

PARAMETERS
  data, data_len - the actual data computed inside the module and its len
  status         - failure or success
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void eap_sim_aka_umei_aka_algo_req_cb
(
  uint8*                       data,
  uint16                       data_len, 
  eap_sim_aka_ume_result_type  status
);

/*=========================================================================
  Client Queue Operation
===========================================================================*/

/*===========================================================================
FUNCTION UMEI_INIT_QUEUE

DESCRIPTION
  Initialize a given UME queue

DEPENDENCIES
  None

PARAMETERS
  
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void 
eap_sim_aka_umei_init_client_queue
(
  eap_sim_aka_ume_provider_type*  provider
);

/*===========================================================================
FUNCTION UMEI_ENQUEUE_CLIENT

DESCRIPTION
  Enqueue a client if SM of the given module shows BUSY state. 

DEPENDENCIES
  None

PARAMETERS
  context - the client context
  cb_ptr  - in case the call is asynchronous this cb function will be called 
            to pass the answer
  client_queue - the actual watermark to add to SIM/USIM/RUIM/SFS
  client_no    - number of clients present in the queue
  max_clients  - the maximum number of clients in the queue
  type         - the type of request enqueued
  
RETURN VALUE
  - the type of the result - Failure or Success, Success is always Sync 

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_enqueue_client
( 
  eap_sim_aka_ume_client_ctx*    context,
  void*                          cb_ptr,
  eap_sim_aka_ume_req_data_type* req_data,
  q_type*                        client_q,
  uint16                         max_clients,
  eap_sim_aka_ume_type_req_enum  type,
  eap_session_info_type*         session_info,
  eap_sim_aka_task_srv_req_type  task_srv_fct
);

/*===========================================================================
FUNCTION UMEI_DEQUEUE_CLIENT

DESCRIPTION
  Return the first waiting client from our client queue.

DEPENDENCIES
  None

PARAMETERS
  client - output value - the first client
  client_wm - the actual wm to extract from SIM/USIM/RUIM/SFS
    
RETURN VALUE
  - the type of the result - Failure or Success, Success is always Sync

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_dequeue_client
( 
  eap_sim_aka_ume_client_type*  client,
  q_type*                       client_q
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_PROCESS_CLIENTS_FROM_QUEUE

DESCRIPTION
  Internal function - called when one entity becomes available and the clients
  from the queue can be processed. 

DEPENDENCIES
  None

PARAMETERS
  clients_wm - the actual queue SIM/USIM/RUIM/SFS
  clients_no - the number of clients in the queue
  pending_client - the current pending client
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void 
eap_sim_aka_umei_process_clients
(
  q_type*                       clients_q,
  eap_sim_aka_ume_client_type*  pending_client,
  eap_sim_aka_ume_state*        client_state
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_IMSI_QUEUE_REQ

DESCRIPTION
  Handles a request for IMSI retrieval of a client that was previously queued. 
  
DEPENDENCIES
  None

PARAMETERS
  pending_client - the current pending client
  imsi_type - SIM/USIm/RUIM
  imsi_req_cb - to be called in case of async calls 
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_imsi_queue_req
(
  eap_sim_aka_ume_client_type*      pending_client
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_ALGO_QUEUE_REQ

DESCRIPTION
  Handles a request for SIM/AKA algo of a client that was previously queued. 
  
DEPENDENCIES
  None

PARAMETERS
  pending_client - the current pending client
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_algo_queue_req
(
  eap_sim_aka_ume_client_type*    pending_client
);


/*=========================================================================
                         MMGSDI LIB INTERACTION
                         FOR SIM/USIM Operation 
===========================================================================*/
/*=========================================================================
  UME Providers for IMSI/SIM/USIM through MMGSDI
===========================================================================*/
#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_GET_IMSI

DESCRIPTION
  Will call SIM/USIM/RUIM module to get IMSI async.

DEPENDENCIES
  None.
    
PARAMETERS
  id - the IMSI id - to be filled when call is sync. 
  cb_ptr - pointer to a cb function to be called if async
  type - type of IMSI - SIM/USIM/CDMA
  
RETURN VALUE
  Failure, Sync Success or Async response - callback will be invoked
  
SIDE EFFECTS
  None  
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_get_card_imsi
( 
  uint8*                            id, 
  uint16*                           max_id_len,
  eap_session_info_type*            session_info,
  eap_sim_aka_ume_provider_service_type*  task_srv_info
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_RUN_SIM_ALGO

DESCRIPTION
  Will call SIM module to execute sim algorithm on a given rand.
  Will retrieve res and kc if synchronous or will call cb function if call 
  async.

DEPENDENCIES
  None.
    
PARAMETERS
  eap_rand - the value of the AT_RAND attribute
  sres, kc - results of the SIM algo 
  cb_ptr - pointer to a cb function to be called if async
  
RETURN VALUE
  Failure, SYNC Success or ASYNC Response
  
SIDE EFFECTS
  None  
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_run_sim_algo
( 
  eap_session_info_type*                  session_info,
  uint8*                                  eap_rand, 
  eap_sim_aka_ume_provider_service_type*  task_srv_info
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_RUN_USIM_ALGO

DESCRIPTION
  Will call SIM module to execute sim algorithm on a given rand.
  Will retrieve res and kc if synchronous or will call cb function if call 
  async.

DEPENDENCIES
  None.
    
PARAMETERS
  eap_rand - the value of the AT_RAND attribute
  autn, autn_len - results of the SIM algo 
  
RETURN VALUE
  ASYNC or SYNC
  
SIDE EFFECTS
  None  
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_run_usim_algo
(
  eap_session_info_type*                session_info,     
  uint8*                                eap_rand, 
  uint16                                rand_len,
  uint8*                                autn, 
  uint16                                autn_len,
eap_sim_aka_ume_provider_service_type*  task_srv_info
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_IMSI_RET_CB_TASK

DESCRIPTION
     Used for task ctx switching from GSDI to wlan SEC task. Set from UME 
    SIM/USIM IMSI cb function.

  
DEPENDENCIES
  None.
    
PARAMETERS
  status - success or failure 
  cnf - type of result being retrieved 
  cnf_ptr - the result being retrieved 
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None  
===========================================================================*/
void 
eap_sim_aka_umei_imsi_ret_cb_task
(
  void* user_data
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_IMSI_RET_CB

DESCRIPTION
  Will be registered with the SIM/USIM (MMGSDI) module and will be called as 
  a response to a retrieve IMSI async.
  
DEPENDENCIES
  None.
    
PARAMETERS
  status - success or failure 
  cnf - type of result being retrieved 
  cnf_ptr - the result being retrieved 
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None  
===========================================================================*/
void 
eap_sim_aka_umei_imsi_ret_cb
(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const  mmgsdi_cnf_type    *cnf_ptr
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_FILE_ATTR_CB

DESCRIPTION
  Will be passed to MMGSDI as cb function for Async File attr retrieval.
  before requesting the IMSI - try to retrieve the IMSI file attributes.

DEPENDENCIES
  None.
    
PARAMETERS
  status - success or failure 
  cnf - type of result being retrieved 
  cnf_ptr - the result being retrieved 
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None  
===========================================================================*/
void 
eap_sim_aka_umei_file_attr_cb 
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const  mmgsdi_cnf_type  *cnf_ptr
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_SIM_ALGO_RET_CB

DESCRIPTION
    Will be registered with the SIM/USIM module and will be called as a 
  response to a retrieve RUN SIM algo request.

DEPENDENCIES
  SIM/USIM module up and running
    
PARAMETERS
  status       - success or failure 
  cnf, cnf_ptr - type of result & the result being retrieved 
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None  
===========================================================================*/
void 
eap_sim_aka_umei_sim_algo_ret_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);


/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_USIM_AUTH_RET_CB

DESCRIPTION
  Will be registered with the SIM/USIM module and will be called as a 
  response to a retrieve RUN USIM auth request.

DEPENDENCIES
  SIM/USIM module up and running
    
PARAMETERS
  status       - success or failure 
  cnf, cnf_ptr - type of result & the result being retrieved
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None  
===========================================================================*/
void 
eap_sim_aka_umei_usim_auth_ret_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_MMGSDI_EVT_CB

DESCRIPTION
  Function registered with the MMGSDI events, needed in order to know 
  the state of SIM/USIM.

  We are interested in events like : CARD_INSERTED, CARD_REMOVE and 
                                     CARD_ERROR

DEPENDENCIES
  None.
  
PARAMETERS
  event - the event information 
  
RETURN VALUE
  None 
  
SIDE EFFECTS
    
===========================================================================*/
void 
eap_sim_aka_ume_mmgsdi_evt_cb
(
  const mmgsdi_event_data_type *event
);
#endif /*FEATURE_MMGSDI_SESSION_LIB*/


/*=========================================================================
                              SFS/EFS Interaction
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_SFS_WRITE

DESCRIPTION
  Writes a data field to a given file using secure FS

DEPENDENCIES
  None.
    
PARAMETERS
  data, data_len - data to be written into the secure file and its len
  file_name, file_name_len - name of the file to write to and its len 
  
RETURN VALUE
  Failure or Sync Success 
  
SIDE EFFECTS
  None  
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_sfs_write
( 
  uint8*                            data, 
  uint8*                            file_name,
  uint16                            data_len,
  uint16                            file_name_len
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UME_SFS_READ

DESCRIPTION
  Reads a data field from a given file using secure FS

DEPENDENCIES
  None.
    
PARAMETERS
  data, data_len - data to read into from the secure file and its len
  file_name, file_name_len - name of the file to read from and its len 
  
RETURN VALUE
  Failure or Sync Success 
  
SIDE EFFECTS
  None  
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_sfs_read
( 
  uint8*                            data, 
  uint8*                            file_name,
  uint16*                           data_len,
  uint16                            file_name_len
);


/*=========================================================================
   Default UME Providers 
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_DEFAULT_IMSI_PROVIDER

DESCRIPTION
  Used to register with UME upon init - if no provider will be registered 
  for an IMSI retrieval the call will always fail

DEPENDENCIES
  None.
    
PARAMETERS
  Not used by the default provider
  
RETURN VALUE
  Failure always
  
SIDE EFFECTS
  None  
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_default_imsi_provider
( 
  uint8*                                  id,  
  uint16*                                 max_id_len,
  eap_session_info_type*                  session_info,
  eap_sim_aka_ume_provider_service_type*  task_srv_info
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_DEFAULT_SIM_PROVIDER

DESCRIPTION
  Used to register with UME upon init - if no provider will be registered 
  for an SIm request the call will always fail

DEPENDENCIES
  None.
    
PARAMETERS
  Not used by the default provider
  
RETURN VALUE
  Failure always
  
SIDE EFFECTS
  None  
===========================================================================*/
eap_sim_aka_ume_result_type
eap_sim_aka_umei_default_sim_provider
( 
  eap_session_info_type*                  session_info,
  uint8*                                  eap_rand, 
  eap_sim_aka_ume_provider_service_type*  task_srv_info
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_DEFAULT_USIM_PROVIDER

DESCRIPTION
  Used to register with UME upon init - if no provider will be registered 
  for an USIm request the call will always fail

DEPENDENCIES
  None.
    
PARAMETERS
  Not used by the default provider
  
RETURN VALUE
  Failure always
  
SIDE EFFECTS
  None  
===========================================================================*/
eap_sim_aka_ume_result_type 
eap_sim_aka_umei_default_usim_provider
( 
  eap_session_info_type*                 session_info, 
  uint8*                                 eap_rand, 
  uint16                                 rand_len, 
  uint8*                                 autn, 
  uint16                                 autn_len,
eap_sim_aka_ume_provider_service_type*  task_srv_info
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_DEFAULT_RUIM_PROVIDER

DESCRIPTION
  Used to register with UME upon init - if no provider will be registered 
  for an RUIM request the call will always fail

DEPENDENCIES
  None.
    
PARAMETERS
  Not used by the default provider
  
RETURN VALUE
  Failure always
  
SIDE EFFECTS
  None  
===========================================================================*/
eap_sim_aka_ume_result_type 
eap_sim_aka_umei_default_ruim_provider
( 
  eap_session_info_type*                 session_info,
  uint8*                                 eap_rand, 
  uint16                                 rand_len, 
  uint8*                                 autn, 
  uint16                                 autn_len,
  uint8*                                 data, 
  uint16*                                max_data_len,
eap_sim_aka_ume_provider_service_type*  task_srv_info
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_DEFAULT_CSIM_PROVIDER

DESCRIPTION
  Used to register with UME upon init - if no provider will be registered 
  for an CSIM request the call will always fail

DEPENDENCIES
  None.
    
PARAMETERS
  Not used by the default provider
  
RETURN VALUE
  Failure always
  
SIDE EFFECTS
  None  
===========================================================================*/
eap_sim_aka_ume_result_type 
eap_sim_aka_umei_default_csim_provider
( 
  eap_session_info_type*                 session_info,
  uint8*                                 eap_rand, 
  uint16                                 rand_len, 
  uint8*                                 autn, 
  uint16                                 autn_len,
  uint8*                                 data, 
  uint16*                                max_data_len,
eap_sim_aka_ume_provider_service_type*  task_srv_info
);

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_USIM_2G_AUTH_RET_CB()

DESCRIPTION
  Will be registered with MMGSDI and is a callback when a authentication with
  USIM app is attempted

DEPENDENCIES
  None
 
PARAMETERS 
  mmgsdi_usim_auth_cnf_type - contains the srv available status
 
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void eap_sim_aka_umei_usim_2g_auth_ret_cb
(
  mmgsdi_usim_auth_cnf_type* usim_auth_cnf
);
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================
FUNCTION EAP_SIM_AKA_UMEI_GET_SFS_FILENAME()

DESCRIPTION
  Used to get the filename 

DEPENDENCIES
  None
 
PARAMETERS 
  mmgsdi_usim_auth_cnf_type - contains the srv available status
 
RETURN VALUE
  filename length

SIDE EFFECTS
  None
===========================================================================*/
uint16 eap_sim_aka_umei_get_sfs_filename
(
  uint8*                       identity, 
  uint16                        identity_len,
  uint8*                       filename,
  eap_sim_aka_ume_mode_types   mode
);

#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_SIM_AKA_UMEI_H */
