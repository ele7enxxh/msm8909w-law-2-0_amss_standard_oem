/*============================================================================
@file
sns_debug_main.c

@brief
Contains main implementation of the Debug thread (the entry point for
the debug module on Apps processor).

Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  INCLUDE FILES
=============================================================================*/
#include "sns_common.h"
#include "sns_debug_str.h"

# undef LOG_TAG
# include "diag_lsm.h"
# undef LOG_TAG
# define LOG_TAG "Sensors"

#include "sns_log_api.h"
#include "sns_diag_dsps_v01.h"
#include "sns_init.h"
#include "sns_memmgr.h"
#include "sns_osa.h"
#include "sns_queue.h"
#include "sns_smr_util.h"
#include "sns_debug_api.h"
#include "sns_log_types.h" /* For bit mask types */
#include "sns_em.h"
#include "sns_debug.h"

#include "msg.h"
#include "msgcfg.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#ifdef SNS_DIAG_TX_MODE
#include <sys/system_properties.h>

/*============================================================================
  MACRO DEFINES
  ==============================================================================*/
#define SNS_DIAG_TX_MODE_PROP "debug.qti.sns.diag.tx.mode"
#define SNS_DIAG_PERIPHERAL_ID 3
#define SNS_DIAG_TX_MODE_STREAMING 0
#define SNS_DIAG_TX_MODE_CIRCULAR_BUFF 2
#define SNS_DIAG_LOW_WM_VALUE 20
#define SNS_DIAG_HIGH_WM_VALUE 70
#endif /* SNS_DIAG_TX_MODE */

/*============================================================================
  GLOBAL DEFINES
  ============================================================================*/
/* Global variable to track if sns_debug module has been initializied */
static boolean sns_debug_test_initialized = false;

uint64_t debug_options_arr[SNS_DIAG_MAX_OPTIONS_V01];

/* Timer object, used one time for delaying getting of log masks from
 * DIAG. This delay (~10 seconds) is required after DIAG_LSM_Init is called
 * We need the delay so that DIAG kernel code can get all the log
 * masks from QXDM.
 */
static sns_em_timer_obj_t sns_diag_logmask_delay_tmr_ptr = (sns_em_timer_obj_t)NULL;

/*===========================================================================
  FUNCTIONS
  ============================================================================*/
static void sns_debug_timer_cb(void* cb_data);
static sns_err_code_e sns_read_debug_input(sns_debug_mask_t* debug_str_mask,
                                           uint64_t *debug_options);

#ifdef SNS_DIAG_TX_MODE
/*===========================================================================

  FUNCTION:   sns_set_diag_tx_mode

  ===========================================================================*/
/*!
  @brief
  This function sets peripheral(SLPI) Diag's TX mode.

  @param[i]
  No input parameters.

  @return
  No return value.
*/
/*=========================================================================*/
void sns_set_diag_tx_mode(void)
{
  int err;
  char sns_diag_prop[PROP_VALUE_MAX];
  int sns_diag_prop_len;
  sns_diag_prop_len = __system_property_get( SNS_DIAG_TX_MODE_PROP, sns_diag_prop );
  if( sns_diag_prop_len == 1 )
  {
    switch( sns_diag_prop[0] )
    {
      case '1':
        SNS_PRINTF_STRING_ERROR_0( DBG_MOD_INIT, "1 Setting SNS TX MODE to CIRCULAR BUFF!" );
        err = diag_configure_peripheral_buffering_tx_mode( SNS_DIAG_PERIPHERAL_ID,
                                                           SNS_DIAG_TX_MODE_CIRCULAR_BUFF,
                                                           SNS_DIAG_LOW_WM_VALUE,
                                                           SNS_DIAG_HIGH_WM_VALUE );
        break;
      default:
        SNS_PRINTF_STRING_ERROR_0( DBG_MOD_INIT, "Setting SNS TX MODE to STREAMING!" );
        err = diag_configure_peripheral_buffering_tx_mode( SNS_DIAG_PERIPHERAL_ID,
                                                           SNS_DIAG_TX_MODE_STREAMING,
                                                           SNS_DIAG_LOW_WM_VALUE,
                                                           SNS_DIAG_HIGH_WM_VALUE );
        break;
    }
  }
  else
  {
    SNS_PRINTF_STRING_ERROR_0( DBG_MOD_INIT, "Setting SNS TX MODE to STREAMING!" );
    err = diag_configure_peripheral_buffering_tx_mode( SNS_DIAG_PERIPHERAL_ID,
                                                       SNS_DIAG_TX_MODE_STREAMING,
                                                       SNS_DIAG_LOW_WM_VALUE,
                                                       SNS_DIAG_HIGH_WM_VALUE );
  }
  SNS_PRINTF_STRING_ERROR_1( DBG_MOD_INIT, "Set SNS TX MODE return val:%d", err );
  return;
}
#endif /* SNS_DIAG_TX_MODE */

/*===========================================================================

  FUNCTION:   sns_debug_timer_cb

===========================================================================*/
/*!
  @brief
  When timer expires we call the funciton to get all the log mask values
  using log_status DIAG api. We sent the log mask to the DSPS.

  @param[i] cb_data  :  Data used when registering callbacks.
                        Notused currently
  @return
  No return value
*/
/*=========================================================================*/
static void sns_debug_timer_cb(void* cb_data)
{
  sns_log_mask_t logging_mask[2];

  UNREFERENCED_PARAMETER(cb_data);
  SNS_PRINTF_STRING_ERROR_0(DBG_MOD_INIT,
                    "Debug Timer Callback called");

  if (sns_fill_log_mask(logging_mask) == SNS_SUCCESS)
  {
    /* Note: There is no need to check on the log mask on apps processor
     * because malloc through DIAG does that for us for free! */
    sns_diag_send_mask_info(LOG_MASK_INFO, logging_mask[0], logging_mask[1]);

    /* Configure the  peripheral Diag's TX mode */
#ifdef SNS_DIAG_TX_MODE
    sns_set_diag_tx_mode();
#endif /* SNS_DIAG_TX_MODE */
  }

  /* Delete timer as well */
  sns_em_delete_timer_obj(sns_diag_logmask_delay_tmr_ptr);
}

/*===========================================================================

  FUNCTION:   sns_read_debug_input

===========================================================================*/
/*!
  @brief
  Reads the debug input and creates two inputs
  1) Debug options - an array of options, value pairs
    Debug options are interpreted by the recipient only. We can currently
    send 16 debug options in  one message.

  and

  2) a debug string mask
  Fills the debug string mask for modules printing debug strings.
  Every module has one bit in the mask. That bit specifies if the debug
  strings from the module are enabled or disabled. We are using a 64 bit
  bit mask for debug strings for modules on DSPS, Apps and Modem. So
  there can be a total of only 64 modules across all the three processors
  outputting debug strings.

  @param[i] debug_str_mask  : Pointer where the debug string mask needs
                              to be filled in.
  @param[i] debug_options   : Pointer where the debug options needs
                              to be filled in.

  @return
  sns_err_code_e: Error code indicating if init was successful.
*/
/*=========================================================================*/
static sns_err_code_e sns_read_debug_input(sns_debug_mask_t* debug_str_mask,
                                           uint64_t *debug_options
                                           )
{
#if defined(SNS_LA) || defined(SNS_LA_SIM)
  /* Local arrays to help construct the bit mask
   * If modules are added then the arrays below need to be edited to
   * add support for the new modules
   */
  static const char *module_name_arr[] = {
                                    "AppsSMR",
                                    "AppsDiag",
                                    "AppsACM",
                                    "AppsInit",
                                    "DSPSSMGR",
                                    "DSPSSAM",
                                    "DSPSSMR",
                                    "DDAccel",
                                    "DDGyro",
                                    "DDAlsPrx",
                                    "DDMag8975",
                                    "AppsMain",
                                    "EM",
                                    "AppsPWR",
                                    "AppsSAM",
                                    "DSPSSCM",
                                    "AppsSCM",
                                    "MDMSMR",
                                    "MDMDiag",
                                    "MDMACM",
                                    "MDMInit",
                                    "MDMMain",
                                    "MDMPwr",
                                    "DSPSDAL",
                                    "DSPSDDF",
                                    "AppsReg",
                                    "AppsTime",
                                    "DSPSDiag",
                                    "DSPSPWR",
                                    "AppsFile",
                                    "AppsFSA"
                                  };

  static const char *options_name_arr[] = {
                                            "opt_uimage_vote",
                                            "opt_ext_smr_ind",
                                            "opt_generic_01",
                                            "opt_generic_02",
                                            "opt_generic_03",
                                            "opt_generic_04",
                                          };

  uint16_t module_mask_arr[SNS_DBG_NUM_MOD_IDS];
  FILE *dbg_config_file_ptr;
  char line[100],*token_str_ptr, *val_str_ptr;
  sns_debug_module_id_e i;
  sns_debug_mask_t bit_mask = 1;
  uint16_t j;
  uint64_t option_val;
  uint8_t ignore_module_names = 0;


  /* Input checks */
  if (debug_str_mask == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(DBG_MOD_INIT,
                      "Set_debug_str_mask ERROR. Input pointer not valid");
    return SNS_ERR_BAD_PARM;
  }

  /* Open and read the mask from file */
  if ((dbg_config_file_ptr = fopen(SNS_DBG_CONFIG_FILE_NAME,"r")) == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(DBG_MOD_INIT,
                              "Debug Config File missing in EFS!");
    return SNS_ERR_FAILED;
  }

  if (SNS_DBG_NUM_MOD_IDS !=
      sizeof(module_name_arr) / sizeof(*module_name_arr))
  {
    SNS_PRINTF_STRING_ERROR_0(DBG_MOD_INIT,
                    "Length of module IDs not same as module names, Ignoring");

    ignore_module_names = 1;
  }

  /* Initialize all modules in the mask array to disabled. */
  for ( j = 0; j < SNS_DBG_NUM_MOD_IDS && !ignore_module_names; j++ )
  {
    module_mask_arr[j] = 1;
  }

  for (j = 0; j < SNS_DIAG_MAX_OPTIONS_V01; j++)
  {
    debug_options[j] = 0;
  }

  /* Read a line and parse */
  while ( fgets(line,sizeof(line),dbg_config_file_ptr ) != NULL )
  {
    /* Comment lines start with #, If comment skip line */
    if (line[0] != '#')
    {
      /* The lines in the file will look like this:
       * SMGR=0. So its fine if the token is =
       */
      char *last_tok;
      token_str_ptr = strtok_r(line,"=", &last_tok);
      val_str_ptr = strtok_r(NULL,"=", &last_tok);
      if (val_str_ptr)
      {
        option_val = strtoll(val_str_ptr,NULL,0);
        SNS_PRINTF_STRING_HIGH_1(DBG_MOD_INIT, "Setting option val to %d", option_val);
      }
      else
      {
        SNS_PRINTF_STRING_HIGH_0(DBG_MOD_INIT, "defaulting option val to 0 ");
        option_val = 0; // Default
      }

      if (token_str_ptr != NULL)
      {
        /* Lets check if this is a debug option */
        for (i=0; i < sizeof(options_name_arr)/sizeof(options_name_arr[0]); i++)
        {
          if (strncmp(token_str_ptr,
                      options_name_arr[i],
                      strlen(options_name_arr[i])) == 0)
          {
            SNS_PRINTF_STRING_HIGH_2(DBG_MOD_INIT,
                            "Setting option %d to %d", i, option_val);
            debug_options[i] = option_val;
            break;
          }
        }

        if (i == sizeof(options_name_arr)/sizeof(options_name_arr[0]) && !ignore_module_names)
        {
          /* Match the module name */
          for (i=0; i < SNS_DBG_NUM_MOD_IDS; i++)
          {
            if (strncmp(token_str_ptr,
                        module_name_arr[i],
                        strlen(module_name_arr[i])) == 0)
            {
              module_mask_arr[i] = option_val;
              break;
            }
          } // End loop over module names
        } // End should check modules
      } // End valid token ptr
    } // end of outer if
  } // end of while

  /* Construct the bit mask from the array */
  *debug_str_mask = 0;
  for(i=0; i < SNS_DBG_NUM_MOD_IDS; i++)
  {
    /* Note: In the file 1 means enabled and 0 means disabled
     * This is easier for users to enter in a file
     * In the Mask however 1 means Disabled and 0 means enabled.
     * The meaning is reversed in the mask
     */
    if (module_mask_arr[i] == 0)
    {
      *debug_str_mask = (*debug_str_mask) | bit_mask;
    }
    bit_mask = bit_mask << 1;
  } // end of for

  SNS_PRINTF_STRING_MEDIUM_1(DBG_MOD_INIT,
                             "Debug Str BitMask=%lu",
                             (*debug_str_mask));

  if (ignore_module_names)
  {
    return SNS_ERR_FAILED;
  }

#if 0
  /* Does not work on target...Hangs need to be debugged */
  fclose(dbg_config_file_ptr);
#endif
#elif defined(_WIN32) /* defined(SNS_LA_SIM) || defined(SNS_LA) */
  /* Input checks */
  if (debug_str_mask == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(DBG_MOD_INIT,
                              "Set_debug_str_mask ERROR. Input pointer not valid");
    return SNS_ERR_BAD_PARM;
  }

  *debug_str_mask = SNS_DIAG_DBG_MASK_DEFAULT;

  SNS_PRINTF_STRING_MEDIUM_1(DBG_MOD_INIT,
                             "Debug Str BitMask=%lu",
                             (int32_t)(*debug_str_mask));
#endif /* defined(_WIN32) */
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_debug_test_init1

  ===========================================================================*/
/*!
  @brief
  Entry point to the sns_debug module. Registers and initializes with DIAG.
  This would help to see debug messages during sensors initialization in
  QXDM.

  @param[i]
  No input parameters.

  @return
  sns_err_code_e: Error code indicating if init was successful.
*/
/*=========================================================================*/
sns_err_code_e sns_debug_test_init1( void )
{
  if (sns_debug_test_initialized == true)
  {
    SNS_PRINTF_STRING_FATAL_0(DBG_MOD_INIT,
                              "DIAG init: Already initialized. Nothing to do!");

    return SNS_SUCCESS;
  }

  if (!Diag_LSM_Init( NULL ))
  {
    SNS_PRINTF_STRING_FATAL_0(DBG_MOD_INIT,
        "Diag_LSM_Init failed: log packets and messages will not be available");
  }

  sns_debug_set_dbg_str_mask( SNS_DIAG_DBG_MASK_DEFAULT );

  sns_init_done();

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_debug_test_init2

  ===========================================================================*/
/*!
  @brief
  Second entry point to the sns_debug module.
  Creates and initializes the debug thread.

  @param[i]
  No input parameters.

  @return
  sns_err_code_e: Error code indicating if init was successful.
*/
/*=========================================================================*/
sns_err_code_e sns_debug_test_init2( void )
{
  sns_err_code_e error_code;
  sns_debug_mask_t debug_str_bit_mask;

  SNS_PRINTF_STRING_MEDIUM_0(DBG_MOD_INIT,
                             "Init: In DIAG_Debug_Test_Init function!" );

  if (sns_debug_test_initialized)
  {
    SNS_PRINTF_STRING_FATAL_0(DBG_MOD_INIT,
                              "DIAG init: Already initialized. Nothing to do!" );
    return SNS_SUCCESS;
  }

  error_code = sns_debug_mr_init();
  if (error_code != 0)
  {
    SNS_PRINTF_STRING_ERROR_0(DBG_MOD_INIT,
                              "Sensors DIAG init: Can't register with debug" );
    return SNS_ERR_FAILED;
  }

  error_code = sns_em_create_timer_obj( sns_debug_timer_cb, NULL,
                                        SNS_EM_TIMER_TYPE_ONESHOT,
                                        &sns_diag_logmask_delay_tmr_ptr);
  if (error_code == SNS_SUCCESS)
  {
    SNS_PRINTF_STRING_MEDIUM_0(DBG_MOD_INIT,
                              "Sensors Diag Init:Can create EM timer object");
    /* Register with timer now */
    sns_em_register_timer(sns_diag_logmask_delay_tmr_ptr,
                          sns_em_convert_usec_to_localtick(SNS_DIAG_LOGMASK_DELAY_USEC) );
    /* We dont return in case of ERRORs...this is not so FATAL that we
     * should bail out
     */
  }

  /* Get mask info */
  if (sns_read_debug_input(&debug_str_bit_mask, debug_options_arr) == SNS_SUCCESS)
  {
    /* Set masks on the apps/modem processor */
    sns_debug_set_dbg_str_mask(debug_str_bit_mask);
    /* Send the mask info to DSPS */
    sns_diag_send_mask_info(DEBUG_STR_MASK_INFO, debug_str_bit_mask, 0);
    /* Send the debug options */
    sns_diag_send_debug_options(debug_options_arr);
  }

  sns_init_done();
  sns_debug_test_initialized = true;

  SNS_PRINTF_STRING_MEDIUM_0(DBG_MOD_INIT,
                             "Sensors DIAG Init: Initialization complete");
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_debug_test_deinit2

  ===========================================================================*/
/*!
  @brief
  Clean up debug resources. Destroy debug thread.

  @param[i]
  No input parameters.

  @return
  sns_err_code_e: Error code indicating if init was successful.
*/
/*=========================================================================*/
sns_err_code_e sns_debug_test_deinit2( void )
{
    sns_err_code_e sns_err;

    sns_debug_test_initialized = false;

    // Deinit MR
    sns_err = sns_debug_mr_deinit();
    if(SNS_SUCCESS != sns_err)
    {
        SNS_PRINTF_STRING_ERROR_1( DBG_MOD_INIT, "sns_acm_mr_deinit: failed %d", sns_err );
    }

    if(NULL != sns_diag_logmask_delay_tmr_ptr)
    {
        sns_em_cancel_timer(sns_diag_logmask_delay_tmr_ptr);
        sns_em_delete_timer_obj(sns_diag_logmask_delay_tmr_ptr);
        sns_diag_logmask_delay_tmr_ptr = (sns_em_timer_obj_t)NULL;
    }

    return SNS_SUCCESS;
}
