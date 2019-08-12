/**
  @file mcfg_dynrec.h

  @brief  Definitions used in Dynamically Reclaiming.
*/
/*
  Copyright (c) 2014 Qualcomm Technologies Incorporated.
          All Rights Reserved.
    Qualcomm Confidential and Proprietary
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/api/mcfg_dynrec.h#1 $
  $DateTime: 2016/12/13 08:00:27 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
 2014/11/11   hzhi    Updated for Dynamic Reclaiming feature v2.0.
 2014/08/19   hzhi    Created for Dynamic Reclaiming.
===========================================================================*/
#ifndef MCFG_DYNREC_H
#define MCFG_DYNREC_H

/*=========================================================================
ENUM TYPE: MCFG_DYNREC_CLIENT_T

This data type is defined for software clients of dynamic reclaiming feature
to pass in as input parameter when calling mcfg_dynrec_load_ext() and 
mcfg_dynrec_reclaim_ext() API's.
===========================================================================*/
typedef enum {
  MCFG_DYNREC_CLIENT_UNSUPPORTED  = 0,
  MCFG_DYNREC_CLIENT_RFC          = 1,
  MCFG_DYNREC_CLIENT_WTR          = 2,
  MCFG_DYNREC_CLIENT_RFC_INTERNAL = 3,
} mcfg_dynrec_client_t;

/*=============================================================================
FUNCTION: MCFG_DYNREC_LOAD()

Params[in]:
  list - Pointer to the list of module names that need to be loaded.
         e.g.: rfc_153 for /modem_proc/rfc_dimepm/rf_card/rfc_wtr4905_na2_v2/,
         please refer to /modem_proc/build/ms/dynrec.lst for samples.
  num_module_to_load - number of items in the specified list.

Returns: 
  0 on Successfully copying and mapping the modules(s), and 
  a non zero value on Failure.

Description: 
  --> Wrapper to invoke new API, for RFC to use only.
  --> Pass a list of module names that will be copied to a different region 
      and mapped. This function can be called multiple times to add more modules.
===============================================================================*/
extern int mcfg_dynrec_load(char* list[], int num_modules_to_load);

/*=============================================================================
FUNCTION: MCFG_DYNREC_LOAD_EXT()

Params[in]:
  list - Pointer to the list of folder names that need to be loaded
  client_id - indicate current request comes from which dynrec client.

Returns: 
  0 on Successfully copying and mapping the folder(s), and 
  a non zero value on Failure.

Description: 
  --> Pass a list of folder names that will be copied to a different 
      region and mapped. This function can be called multiple times to add 
      more folders.
  --> WTR client(s) can access modules before requesting to load and reclaim.
*==============================================================================*/
extern int mcfg_dynrec_load_ext(char* module_to_load, mcfg_dynrec_client_t client_id);

/*=============================================================================
FUNCTION: MCFG_DYNREC_RECLAIM()

Param[in]:
  num_used = number of RFC modules that were successfully loaded: this
             value is updated at the end of function call for caller to 
             check for error checking on caller side.

Returns:
  0 on Successful reclaiming, and 
  a non zero value on Failure.

Description: 
  --> Reclaims memory of all RFC modules specified in the linker script. 
  --> Loaded RFC module(s) through MCFG_DYNREC_LOAD() API will be kept in
      new memory chunk created and mapped in TLB, won't be affected by this
      reclaim API.
  --> If called multiple times by RFC client, only the very first request
      will get processed and all other requests will get rejected with 
      non-zero return value.
*====== ======================================================================*/
extern int mcfg_dynrec_reclaim(int* num_modules_loaded);

/*============================================================================
FUNCTION: MCFG_DYNREC_RECLAIM_EXT()

Param[in]:
  num_modules_loaded
      number of RFC modules that were successfully loaded: this value is 
      updated at the end of function call for caller to check for error 
      checking on caller side.
  
  client_id
      indicate current request comes from which dynrec client.

Returns:
  0 on Successful reclaiming, and 
  a non zero value on Failure.

Description: 
  --> Reclaims memory of all dynrec-pattern modules specified in the linker 
      script. 
  --> Loaded dynrec-pattern module(s) through MCFG_DYNREC_LOAD_EXT() API 
      will be kept in new memory chunk created and mapped in TLB, won't be 
      affected by this reclaim API.
  --> If called multiple times by same client, only the very first request
      will get processed and all other requests will get rejected with 
      non-zero return value.
  --> WTR client(s) can access modules before requesting to load and reclaim.
*============================================================================*/
extern int mcfg_dynrec_reclaim_ext(int* num_modules_loaded, mcfg_dynrec_client_t client_id);

/*===========================================================================
FUNCTION: MCFG_DYNREC_INIT()

Param[in]:
  None.

Description: 
  Init function to check if RFC dynamic reclaiming is turned on or off:
    --> if turned on, will load first all-tech RFC module before any other
        as workaround for RFC dynrec veneer issue. 
    --> if turned off, will return non-zero value to indicate this.
  Should not be invoked by any caller other than modem_cfg_task().
*============================================================================*/
extern int mcfg_dynrec_init(void);

/*===========================================================================
FUNCTION: MCFG_DDYNREC_ATA_RECLAIM()

on TODO list, not implemented yet. 
*============================================================================*/
/*
extern int mcfg_dynrec_data_reclaim();
*/
#endif /*MCFG_DYNREC_H*/