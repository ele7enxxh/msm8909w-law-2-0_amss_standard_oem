/*==============================================================================

  FILE:         lm_utils.c

  OVERVIEW:     This file provides S1 LM utility functions

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: 
$DateTime: 
==============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "lm_internal.h"
#include "lm_hwio.h"
#include "HALhwio.h"
#include "msmhwiobase.h"

 
 /* Flags to control LM behaviour */
 volatile static uint32 lm_extended_debug_logs = TRUE;
 volatile static uint32 lm_force_limits_vote   = FALSE;
 
 /* Global variables to store current cx state and vote */
 uint32 greg_cx_state = 0;
 uint32 greg_lm_vote = 0;
 
 extern LMContext gLMContext; 
 extern volatile boolean g_lm_nv_enable;
 
/* Table maintaining Modem Cx state --> Vote to apps */
 
static lm_table_type lm_votes_table[] = {
  {
    PMIC_NPA_MODE_ID_CORE_RAIL_OFF,            0
  },
  {
    PMIC_NPA_MODE_ID_CORE_RAIL_RETENTION,      0
  },
  {
    PMIC_NPA_MODE_ID_CORE_RAIL_RETENTION_PLUS, 0
  },
  {
    PMIC_NPA_MODE_ID_CORE_RAIL_LOW_MINUS,      0
  },
  {
    PMIC_NPA_MODE_ID_CORE_RAIL_LOW,            0
  },
  {
    PMIC_NPA_MODE_ID_CORE_RAIL_LOW_PLUS,       0
  },
  {
    PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL,        0
  },
  {
    PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL_PLUS,   0
  },
  {
    PMIC_NPA_MODE_ID_CORE_RAIL_TURBO,          1
  }
};
 
 
/* Functions used */

/*
 * lm_os_create_config_handle
 *
*/
CoreConfigHandle lm_os_create_config_handle( unsigned int core_num )
{
  char path[MAX_INI_PATH_LEN];

  /* Build the path to the lm_config.ini file */
  snprintf( path, MAX_INI_PATH_LEN,
            "/nv/item_files/lm/core%d/lm_config.ini", core_num );

  return( CoreIni_ConfigCreate( path ) );

}

/*
 * lm_os_destroy_config_handle
 *
*/
void lm_os_destroy_config_handle( CoreConfigHandle config )
{
  CoreIni_ConfigDestroy( config );
}

/*
 * sleep_config_read_bool
 */
unsigned int lm_config_read_bool( unsigned int core_num, char *state_name, 
                                     char *lprm_name, char *param, 
                                     unsigned char *result )
{
  char section[MAX_SECTION_NAME_LEN];
  unsigned int retVal = FALSE;
  CoreConfigHandle config;

  /* Get the CoreConfig object */
  config = lm_os_create_config_handle( core_num );

  if ( config == NULL )
  {
    return retVal;
  }

  CORE_VERIFY_PTR( state_name );

  /* Build the section to read */
  if ( lprm_name )
  {
    snprintf( section, MAX_SECTION_NAME_LEN, "%s.%s", 
              state_name, lprm_name );
  }
  else
  {
    snprintf( section, MAX_SECTION_NAME_LEN, "%s", state_name );
  }     

  /* Call upon the generic config reader subsystem to read the data
   * for the requested item.  The data will be returned as a boolean. */
  retVal = CoreConfig_ReadBool( config, section, param, result );

  lm_os_destroy_config_handle( config );

  return ( ( retVal == CORE_CONFIG_SUCCESS ) ? TRUE : FALSE );
}


/**
 *    <!-- lm_efs_initialize -->
 *    @brief check with lm initialize NV variable
 *    @return None
 */
 void lm_efs_initialize(void)
 {
  boolean lm_config_value = FALSE;
  unsigned int ret = FALSE;

  /* Check NV param to enable/disable lm callback */
  ret = lm_config_read_bool( 0, ( char * )"all_state", NULL,
                                ( char * )"disable", &lm_config_value );

  if ( ret )
  {
    g_lm_nv_enable = !lm_config_value;
  }

  if(g_lm_nv_enable == FALSE)
  {
    ULOG_RT_PRINTF_0( gLMContext.ulog_handle, "S1 LM Disabled from NV settings" );
  }
}
 
 /**
 *    <!-- lm_apply_limits_vote -->
 *    @brief Applies the LM vote when it is changed
 *    
 *    @param lm_vote : Current Modem lm vote
 */
void lm_apply_limits_vote(uint32 lm_vote )
{
  ULOG_RT_PRINTF_1(gLMContext.ulog_handle, " Applying Clk Div vote from MPSS, lm_vote=%d ",lm_vote);
  out_dword(HWIO_GCC_S1LM_MODEM_DIV_VOTE_ADDR, lm_vote);

  if(TRUE == lm_extended_debug_logs)
  {
    greg_lm_vote = in_dword(HWIO_GCC_S1LM_MODEM_DIV_VOTE_ADDR);
    if ( greg_lm_vote != lm_vote)
    {
      ULOG_RT_PRINTF_2(gLMContext.ulog_handle,
                       "WARNING: Clk Div vote read from register:0x%x is %d",
                       HWIO_GCC_S1LM_MODEM_DIV_VOTE_ADDR, greg_lm_vote );      
    }
  }
}
 
 /**
 *    <!-- lm_send_interrupt_data -->
 *    @brief Writes interrupt data(cx_state) and generates interupt to Apps
 *    
 *    @param cx_state : Current Modem cx state
 */
void lm_send_interrupt_data(uint32 cx_state)
{
  /* Updating MPSS cx state */
  ULOG_RT_PRINTF_1(gLMContext.ulog_handle, "Sending cx_state = %d to APPS ", cx_state);      
  out_dword(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_ADDR, cx_state);
  
  if( TRUE == lm_extended_debug_logs )
  {
    greg_cx_state  = in_dword(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_ADDR);
    if ( greg_cx_state != cx_state)
    {
      ULOG_RT_PRINTF_2(gLMContext.ulog_handle,
                       "WARNING: cx state read from address = 0x%x is %d",
                       HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_ADDR, greg_cx_state );      
    }
  }

  /* Send interrupt to Apps */
  ULOG_RT_PRINTF_0(gLMContext.ulog_handle, "Sending LM interrupt to APPS");
  out_dword(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_ADDR, 1);
}
 
 /**
 *    <!-- lm_get_limts_vote -->
 *    @brief Gets the lm_vote from lm_votes_table
 *    @return : lm_vote
 *
 *    @param cx_state : Current Modem cx state
 */
 
uint32 lm_get_limts_vote( uint32 cx_state)
{
  int index;
  uint32 lm_vote = 0;
  if (cx_state <= PMIC_NPA_MODE_ID_CORE_RAIL_TURBO)
  {
    for (index=0; index <= PMIC_NPA_MODE_ID_CORE_RAIL_TURBO; index++)
    {
      if (cx_state == lm_votes_table[index].cx_npa_state)
      {
        lm_vote = lm_votes_table[index].lm_vote;
        break;
      }
    }
	
    ULOG_RT_PRINTF_2(gLMContext.ulog_handle, "LM vote for cx_state = %d lm_vote = %d", cx_state, lm_vote);
  }
  else
  {
    ULOG_RT_PRINTF_1(gLMContext.ulog_handle, "Received incorrect cx state, cx_state = %d",cx_state);
  }
  
  //Always send limits vote as 1
  if( TRUE == lm_force_limits_vote )
  {
    lm_vote = 1;
	ULOG_RT_PRINTF_0(gLMContext.ulog_handle, "lm_force_liimits_vote set to TRUE so applying lm_vote = 1");
  }
  return lm_vote;
}
 
