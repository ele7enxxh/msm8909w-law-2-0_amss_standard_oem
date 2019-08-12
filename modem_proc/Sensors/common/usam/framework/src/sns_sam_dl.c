/*============================================================================
  @file sns_sam_dl.c

  @brief
  Support for loading of SAM dynamic algorithms.  Depends on the Sensors
  Dynamic Loading (DL) service.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/

#include "dlfcn.h"
#include "sns_dl.h"
#include "sns_sam.h"
#include "sns_sam_init.h"
#include "sns_sam_dl.h"
#include "sns_debug_str.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

/* How long to wait for for the DL service before determining that it is
 * unattainable.  In microseconds. */
// TODO: For now we block until we hear back from the remote registry service.
#define USAM_DL_TIMEOUT_US 0  //SNS_SAM_INIT_REG_TIMEOUT_US

/*============================================================================
  Static Data
  ===========================================================================*/

/* Global err code to indicating status regarding USAM's use of DL services */
STATIC sns_err_code_e errCodeDL = SNS_ERR_UNKNOWN;

/* Global handle for the DL service */
STATIC sns_dl_svc_handle_t handleDL = SNS_DL_HANDLE_INITIALIZER;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Determine if DL svc is available and fully initialized
 *
 * @return true if service is available; false otherwise
 */
STATIC bool
sns_sam_is_dynamic_loading_enabled( void )
{
  if( SNS_DL_HANDLE_INITIALIZER == handleDL )
  {
    return false;
  }
  else
  {
    return true;
  }
}

/**
 * Destroy SAM's handle to the DL service.  This handle is no longer used
 * or necessary after SAM initialization.
*/
STATIC void
sns_sam_deinit_dl( void )
{
  if( sns_sam_is_dynamic_loading_enabled() )
  {
    //errCodeDL = sns_dl_svc_destroy( handleDL );
    handleDL = SNS_DL_HANDLE_INITIALIZER;
  }
}

/**
 * Function to pass into the DL SVC. Since we will load/unload algos only when
 * running in bigImage, the allocator provided will allocate from bigImage heap.
*/
STATIC void*
usam_dl_malloc( size_t num_bytes )
{
  return SNS_OS_MALLOC( SNS_SAM_DBG_MOD, num_bytes );
}

/**
 * Function to pass into the DL SVC. Since we will load/unload algos only when
 * running in bigImage, the deallocator provided will allocate from bigImage heap.
*/
STATIC void
usam_dl_free( void *ptr )
{
  SNS_OS_FREE(ptr);
}

/**
  @brief API to iterate through all loaded algorithms.
*/
/*void sns_sam_for_each_algo( pfn_sns_dl_get_info pfn )
{
  static char static_file_path[SNS_DSO_FILEPATH_SZ] = "STATIC";
  static char static_err_str[SAM_DL_ERROR_STR_SIZE] = "SUCCESS";
  sns_dl_info_t dl_info;
  sns_q_link_s *qCurr;

  if (pfn)
  {
    for( qCurr = sns_q_check( &sensorQ ); NULL != qCurr;
         qCurr = sns_q_next( &sensorQ, qCurr ) )
    {
      sns_sam_sensor_algo *algo = (sns_sam_sensor_algo*)qCurr;
      SNS_OS_MEMZERO(&dl_info, sizeof(sns_dl_info_t));
      if (NULL != algo->sensor.dlInfo)
      {
        dl_info.entry_symbol   = algo->sensor.dlInfo->entry_symbol;
        dl_info.full_file_path = algo->sensor.dlInfo->full_file_path;
        dl_info.err_str        = algo->sensor.dlInfo->err_str;
        dl_info.load_attrib    = algo->sensor.dlInfo->load_attrib;
        dl_info.file_handle    = algo->sensor.dlInfo->file_handle;
        dl_info.p_suid         = algo->sensor.sensorUID;
      }
      else
      {
        dl_info.full_file_path = static_file_path;
        dl_info.err_str        = static_err_str;
        dl_info.p_suid         = algo->sensor.sensorUID;
      }
      pfn(&dl_info);
    }
  }
}*/

/**
 * Load all algorithms found in a dynamically linked file.
 *
 * @param[i] config Configuration information from the registry group data
 * @param[i] fileHandle File used to load this algorithm
 * @param[i] regContainer Array of algorithms to load
 * @param[i] uImageAlgo Whether these algorithms are uImage-supporting algos
 *
 * @return true if this file is still needed; false otherwise
 */
STATIC bool
sns_sam_dl_load_algo( sns_reg_dyn_cfg_t const *config, void const *fileHandle,
    sns_sam_algo_config_container const *regContainer, bool uImageAlgo )
{
  uint32_t algoCount = 0;
  sns_sam_err err;
  uint32_t regIdx;

  for( regIdx = 0; regIdx < regContainer->numRegistrations; regIdx++ )
  {
    sns_sam_sensor_algo *algo;

    err = sns_sam_init_algo( &algo );
    if( SAM_ENOMEM == err )
    {
      SNS_PRINTF_STRING_ERROR_0( samModule, "Algo Allocation Failed" );
      break;
    }

    err = regContainer->registrationList[ regIdx ]( &algo->algoAPI,
        &algo->algoMsgAPI, &algo->sensor.serviceObj, algo->sensor.sensorUID );
    if( SAM_ENONE != err )
    {
      SNS_PRINTF_STRING_ERROR_2( samModule,
          "Algo#%d SUID:0x%x Registration Failed",
          regIdx, *algo->sensor.sensorUID);
      sns_sam_deinit_algo( algo );
    }
    else
    {
      if( NULL == sns_sam_lookup_sensor_from_suid( algo->sensor.sensorUID ) )
      {
        sns_sam_dl_info *dlInfo = (sns_sam_dl_info*)SNS_OS_MALLOC(
            SNS_SAM_DBG_MOD, sizeof(sns_sam_dl_info) );
        if( NULL != dlInfo )
        {
          strlcpy( dlInfo->entrySymbol, config->entry_symbol,
              SNS_DSO_LOAD_SYM_NAME_SZ );
          strlcpy( dlInfo->fullFilePath, config->full_file_path,
              SNS_DSO_FILEPATH_SZ );
          strlcpy( dlInfo->errStr, "SUCCESS", SAM_DL_ERROR_STR_SIZE );

          dlInfo->fileHandle = fileHandle;
          dlInfo->loadAttr = config->load_attrib;
          dlInfo->uImageAlgo = uImageAlgo;
          algo->sensor.dlInfo = dlInfo;
          sns_q_put( &sensorQ, (sns_q_link_s*)algo );

          algoCount++;
          SNS_PRINTF_STRING_LOW_2( samModule,
              "Algorithm# %d SUID:0x%x Registration Success",
              regIdx, *algo->sensor.sensorUID );
        }
        else
        {
          SNS_PRINTF_STRING_ERROR_0( samModule, "No memory to alloc DL Info" );
          sns_sam_deinit_algo( algo );
        }
      }
      else
      {
        SNS_PRINTF_STRING_ERROR_2( samModule,
                "Dyn Algo# %d Not Loaded into SAM. SUID:0x%llx Already Exists",
                regIdx, *algo->sensor.sensorUID );
        sns_sam_deinit_algo( algo );
      }
    }
  }

  return ( 0 != algoCount );
}

/**
  @brief This function attempts to load a shared object based algo
  into USAM. If loaded successfully the algo is enqueued into the
  USAM sensorQ.
*/
STATIC
void sns_sam_load_dynamic_algo_cb( sns_reg_dyn_cfg_t *config )
{
  void *fileHandle = NULL;
  int errDL, symLen, pathLen;
  bool fileNeeded = false;

  symLen = strlen( config->entry_symbol );
  pathLen = strlen( config->full_file_path );
#if 0
  if( symLen > 0 && pathLen > 0 )
  {
    /* Attempt to open the DSO, if the algo is uimage capable, it will
     * automatically be loaded into uimage memory */
    fileHandle = dlopen( config->full_file_path, RTLD_NOW );
    if( NULL == fileHandle )
    {
      SNS_PRINTF_STRING_ERROR_0( samModule, "dlopen failed" );
    }
    else
    {
      void *dlSymbol = NULL;

      SNS_PRINTF_STRING_LOW_1( samModule, "dlopen return %#x", fileHandle );

      dlSymbol = dlsym( fileHandle, config->entry_symbol );
      if( NULL == dlSymbol )
      {
        SNS_PRINTF_STRING_ERROR_0( samModule, "dlsym failed" );
      }
      else
      {
        sns_sam_algo_config_container *regContainer;
        sns_sam_algo_get_registration_container getRegContainer;

        /* Invoke the function and obtain the list and count of
         * algorithms to load from the DSO. */
        getRegContainer = (sns_sam_algo_get_registration_container)dlSymbol;
        regContainer = getRegContainer();
        if( NULL == regContainer )
        {
          SNS_PRINTF_STRING_ERROR_0( samModule, "NULL Registration Container" );
        }
        else
        {
          if( 0 == regContainer->numRegistrations )
          {
            SNS_PRINTF_STRING_ERROR_0( samModule, "No Algo Registrations Found" );
          }
          else
          {
            int uImageAlgo = 0;

            errDL = dlinfo( fileHandle, RTLD_DI_UIMG_IN_TREE, &uImageAlgo );
            if( -1 == errDL )
            {
              SNS_PRINTF_STRING_ERROR_0( samModule, "Error in dlinfo" );
            }
            else
            {
              fileNeeded = sns_sam_dl_load_algo( config, fileHandle,
                  regContainer, 0 != uImageAlgo );
            }
          }
        }
      }
    }
  }

  /* final clean up if required */
  if( !fileNeeded && NULL != fileHandle )
  {
    dlclose( fileHandle );
  }
#endif
}

/**
 * Print relevant debug information regarding USAM's use of DL SVC.
 *
 * PEND: Remove function if no longer necessary
 */
/*STATIC void
sns_sam_print_dynamic_info( void )
{
  bool dlEnabled = sns_sam_is_dynamic_loading_enabled();
  if( dlEnabled )
  {
    sns_dl_svc_print_state( handleDL );
  }
  SNS_PRINTF_STRING_ERROR_2(samModule,
                            "USAM is DL Enabled:%d, Err:%#x",
                            dlEnabled, errCodeDL);
}*/

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

void
sns_sam_dl_record_algos( void )
{
  sns_reg_dyn_cfg_t reg_algo_cfg;

  if( sns_sam_is_dynamic_loading_enabled() )
  {
    /*sns_dl_reg_for_each_algorithm( handleDL,
        sns_sam_load_dynamic_algo_cb, &reg_algo_cfg, USAM_DL_TIMEOUT_US );*/

    sns_sam_deinit_dl();
  }
}

/**
  Create a handle with the DL service and initialize DL global data
*/
sns_sam_err sns_sam_init_dl(void)
{
  //static char usam_dl_name[] = "USAM";
  sns_sam_err rv = SAM_ENONE;
  /*sns_err_code_e err;
  sns_dl_attr_t attr;

  err = sns_dl_attr_init( &attr );
  if( SNS_SUCCESS == err )
  {
    sns_dl_set_attr_name( &attr, usam_dl_name, sizeof(usam_dl_name) );
    sns_dl_set_attr_allocator( &attr, usam_dl_malloc, usam_dl_free );
    sns_dl_set_attr_blocking_info( &attr, true, USAM_DL_TIMEOUT_US, NULL, NULL );

    // Enable to test on platforms where DL registry is unsupported
    //sns_dl_attr_use_local_registry(&attr);

    // TODO: We should return error codes only on no memory type failures as this is an optional feature at this time.
    err = sns_dl_svc_create( &handleDL, &attr );
    if( SNS_ERR_NOMEM == err )
    {
      rv = SAM_ENOMEM;
    }
    sns_dl_attr_deinit( &attr );
  }*/

  return rv;
}
