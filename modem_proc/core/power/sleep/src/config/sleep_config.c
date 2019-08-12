/*=============================================================================

  FILE:         sleep_config.h
  
  OVERVIEW:     This file provides methods for reading sleep configuration
                data.
 
                Copyright (c) 2010 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/config/sleep_config.c#1 $

=============================================================================*/

#include "sleep_config.h"
#include "CoreVerify.h"
#include "stdio.h"
#include "CoreConfig.h"
#include "sleepi.h"

/* ==================================================================
                        GLOBAL VARIABLES
   ================================================================== */              

/**
 * MAX_SECTION_NAME_LEN 
 *  
 * @brief Max number of characters for a section name in the 
 *        sleep ini file.
 */
#define MAX_SECTION_NAME_LEN  128


/* ==================================================================
              EXTERNAL FUNCTION DECLARATIONS
   ================================================================== */              

/*
 * sleep_config_read_bool
 */
unsigned int sleep_config_read_bool( unsigned int core_num, char *lpr_name, 
                                     char *lprm_name, char *param, 
                                     unsigned char *result )
{
  char section[MAX_SECTION_NAME_LEN];
  unsigned int retVal = FALSE;
  CoreConfigHandle config;

  /* Get the CoreConfig object */
  config = sleep_os_create_config_handle( core_num );

  if ( config == NULL )
  {
    return retVal;
  }

  CORE_VERIFY_PTR( lpr_name );

  /* Build the section to read */
  if ( lprm_name )
  {
    snprintf( section, MAX_SECTION_NAME_LEN, "%s.%s", 
              lpr_name, lprm_name );
  }
  else
  {
    snprintf( section, MAX_SECTION_NAME_LEN, "%s", lpr_name );
  }     

  /* Call upon the generic config reader subsystem to read the data
   * for the requested item.  The data will be returned as a boolean. */
  retVal = CoreConfig_ReadBool( config, section, param, result );

  sleep_os_destroy_config_handle( config );

  return ( ( retVal == CORE_CONFIG_SUCCESS ) ? TRUE : FALSE );
}
