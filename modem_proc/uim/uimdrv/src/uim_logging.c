/*==============================================================================
  FILE:         uim_logging.c

  OVERVIEW:     File contains the functions for APDU and EFS logging.

  DEPENDENCIES: N/A

                Copyright (c) 2014-2015 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_logging.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       ---------------------------------------------------------
05/10/16    sam        Remove the logic to store timeout info in EFS
04/28/16    na         Restricting EFS APDU Logging with Feature
09/08/15    ll         UIMDRV migrates to UIM COMMON EFS APIs for all EFS items
12/22/14    na         Making APDU Logging file instance based
11/10/14    akv        Support for user to customize LDO settings
10/01/14    ll         KW errors
09/18/14    akv        UIMDRV UT enhancements
06/16/14    ll         Switch to new string based TLMM APIs
03/03/14    nmb        Directory maintenance I&T bug fixes
02/20/14    ak         Fix compile errors due to strict compiler on 9x35
01/29/14    akv        RX ISR refactoring changes
01/23/14    yk         The first revision
==============================================================================*/
#include "intconv.h"
#include "rex.h"
#include "fs_public.h"
#include "comdef.h"
#include "time_svc.h"
#include "log.h"
#include "uimdrv_msg.h"
#include "fs_lib.h"
#include "fs_stdlib.h"
#include "fs_diag_access.h"
#include "mcfg_fs.h"

#include "uim_p.h"
#include "uimi.h"
#include "uimglobals.h"
#include "uim_logging.h"
#include "uimdrv_gpio.h"


/**
 * DECLARATIONS OF INTERNAL FUNCTIONS
 */
static boolean uim_log_put_buffer( uim_instance_global_type *uim_ptr );
static void uim_log_put_char( uim_instance_global_type *uim_ptr );
static void uim_log_put_tstamp( uim_instance_global_type *uim_ptr );


/**
 * DEFINITIONS OF EXTERNAL FUNTIONS
 *
 */

/**
 * Put a byte to APDU log buf
 *
 * @param attr Attribute of logging data
 * @param ch Data to be logged.
 * @param uim_ptr Pointer to the global data common.
 */
void uim_log_put_byte(uim_log_attrib_type attr,
                      char ch,
                      uim_instance_global_type *uim_ptr)
{
  uim_ptr->debug.log_char.attrib = attr;
  uim_ptr->debug.log_char.the_char = ch;
  uim_ptr->debug.log_char.slot_id = uim_ptr->id;

  uim_log_put_char(uim_ptr);
  if( (uim_nv_is_feature_enabled(UIMDRV_FEATURE_LOG_APDU_TO_EFS,
                               uim_ptr) == TRUE) &&
      (TRUE == uim_ptr->debug.efslog_logging_in_progress)
    )
  {
    uim_efslog_apdu_to_buffer(uim_ptr);
  }
}


/**
 * Put a time stamp then logging all data in APDU log bug.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_log_put_last_byte(uim_instance_global_type *uim_ptr)
{
  uim_ptr->debug.log_char.slot_id = uim_ptr->id;

  if (uim_ptr->debug.log_data.length != 0)
  {
    uim_log_put_tstamp(uim_ptr);
    uim_log_put_buffer(uim_ptr);
  }
}


/**
 * Save GPIO register state to globals upon command response
 * timeout(Saves the 10 most recent occurances of command
 * response timeout on an internal or external command).
 *
 * @param uim_ptr Pointer to the global data common.
 * @param is_internal_command Internal or External.
 */
void uim_save_gpio_info(
  uim_instance_global_type *uim_ptr,
  boolean                  is_internal_command
)
{
  uint32 i;
  static uint32 num_times_called = 0;
  boolean uim_populate_hw_gpio_info_status = FALSE;

  if( NULL == uim_ptr )
  {
    UIMDRV_MSG_ERR_0(0, "uim_save_gpio_info: uim_ptr is NULL");
    return;
  }
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  UIMDRV_MSG_HIGH_0(uim_ptr->id, "uim_save_gpio_info in globals");
  /* index for circular buffer */
  i = num_times_called % UIM_NUM_TIMEOUT_INFO;
  if(i >= UIM_NUM_TIMEOUT_INFO)
  {
    UIMDRV_MSG_ERR_1(uim_ptr->id, "uim_save_gpio_info index out of range %d",
                     i);
    i = 0;
  }

  uim_ptr->debug.uim_timeout_gpio_info[i].isInternalCmd  = is_internal_command;
  uim_ptr->debug.uim_timeout_gpio_info[i].num_timeouts   = num_times_called;

  uim_populate_hw_gpio_info_status = uim_populate_hw_gpio_info(uim_ptr, &uim_ptr->debug.uim_timeout_gpio_info[i]);

  if (uim_populate_hw_gpio_info_status)
  {
    UIMDRV_MSG_ERR_3( uim_ptr->id, "Timeout Print UIM GPIO input RST Value:0x%x CLK Value:0x%x DATA Value:0x%x",
                      uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input.reset,
                      uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input.clk,
                      uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input.data);
    UIMDRV_MSG_ERR_1( uim_ptr->id, "Detect Value: 0x%x",
                      uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input.detect);
    UIMDRV_MSG_ERR_3( uim_ptr->id, "Data_CFG:Dir: 0x%x Pull: 0x%x Driv: 0x%x",
                      uim_ptr->debug.uim_timeout_gpio_info[i].data_config.eDirection,
                      uim_ptr->debug.uim_timeout_gpio_info[i].data_config.ePull,
                      uim_ptr->debug.uim_timeout_gpio_info[i].data_config.eDriveStrength);
    UIMDRV_MSG_ERR_3( uim_ptr->id, "CLK_CFG:Dir: 0x%x Pull: 0x%x Driv: 0x%x",
                      uim_ptr->debug.uim_timeout_gpio_info[i].clk_config.eDirection,
                      uim_ptr->debug.uim_timeout_gpio_info[i].clk_config.ePull,
                      uim_ptr->debug.uim_timeout_gpio_info[i].clk_config.eDriveStrength);
    UIMDRV_MSG_ERR_3( uim_ptr->id, "Reset_CFG:Dir: 0x%x Pull: 0x%x Driv: 0x%x",
                      uim_ptr->debug.uim_timeout_gpio_info[i].reset_config.eDirection,
                      uim_ptr->debug.uim_timeout_gpio_info[i].reset_config.ePull,
                      uim_ptr->debug.uim_timeout_gpio_info[i].reset_config.eDriveStrength);

    if(uim_ptr->flag.runtime_disable_recovery)
    {
      UIMDRV_MSG_ERR_3(uim_ptr->id, "Set the UIM lines to pull-up input GPIOs values:RST:0x%x CLK:0x%x DATA:0x%x",
                       uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input_pull_up.reset,
                       uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input_pull_up.clk,
                       uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input_pull_up.data);
      UIMDRV_MSG_ERR_1(uim_ptr->id, "Detect value: 0x%x",
                       uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input_pull_up.detect);
    }
  }
  num_times_called++;
} /* uim_save_gpio_info */


/**
 * DEFINITIONS OF INTERNAL FUNTIONS
 *
 */

/**
 * This function allocate memory and copy buffered data to the
 * memory for diag. It initializes the buffer parameters when
 * copying is done.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
static boolean uim_log_put_buffer
(
  uim_instance_global_type *uim_ptr
)
{
  uint8             log_len_local = 0;
  uim_log_data_type *log_ptr  = NULL;
  boolean           status = FALSE;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  if (uim_ptr->debug.log_data.length == 0)
    return TRUE; /* Nothing to log */

  /* Use local variable to store length of buffer, so that it
     does not get updated while switching to isr context */
  log_len_local = uim_ptr->debug.log_data.length;

  if (MULTI_SLOT_LOGGING_MASK_14CE == uim_ptr->debug.apdu_log_mask.log_mask)
  {
    log_ptr = (uim_log_data_type *) log_alloc (LOG_UIM_DS_DATA_C,
              FPOS(uim_log_data_type, data) + log_len_local);
  }
  else if (SINGLE_SLOT_LOGGING_MASK_1098 == uim_ptr->debug.apdu_log_mask.log_mask)
  {
    log_ptr = (uim_log_data_type *) log_alloc (LOG_UIM_DATA_C,
                FPOS(uim_log_data_type, data) + log_len_local);
  }

  if (log_ptr != NULL)
  {
    log_ptr->length = log_len_local;

    if (log_ptr->length <= (LOG_UIM_MAX_CHARS + LOG_UIM_TSTAMP_SIZE))
    {
      uim_memscpy( (void *) (log_ptr->data),
               sizeof(log_ptr->data),
              (void *) uim_ptr->debug.log_data.data,
              log_ptr->length );
    }

    /* Do not commit data if uim_log_data got updated after log_alloc
       as we may loose data because of this */
    if(log_len_local != uim_ptr->debug.log_data.length)
    {
      log_free((uim_log_data_type  *) log_ptr);
      return FALSE;
    }
    log_commit((uim_log_data_type  *) log_ptr);
    status = TRUE;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
      #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  }

  /* Reset the parameters */
  uim_ptr->debug.log_data.length = 0;
  uim_ptr->debug.log_char.attrib = UIM_LOG_ATTRIB_INIT;
  return status;
} /* uim_log_put_buffer */


/**
 * This funciton put a byte along with the attrib byte to the
 * buffer.
 *
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_log_put_char( uim_instance_global_type *uim_ptr )
{
  /* LOG_UIM_MAX_CHARS is 247 while uim_log_data_ptr->data[] size is
     255 bytes. Last 8 bytes are reserved for time stamp.
     Need to add boundary checks so that for always keep last 8 bytes
     free for time stamp.

     We add minimum three log bytes. We need to adjust boundary check
     appropriately.
  */
  if (MULTI_SLOT_LOGGING_MASK_14CE == uim_ptr->debug.apdu_log_mask.log_mask &&
        uim_ptr->debug.log_data.length  <= (LOG_UIM_MAX_CHARS - 3))
  {
    /* Store the attribute byte */
    uim_ptr->debug.log_data.data[uim_ptr->debug.log_data.length++] = uim_ptr->debug.log_char.attrib;
    /* Store the slot byte */
    uim_ptr->debug.log_data.data[uim_ptr->debug.log_data.length++] = uim_ptr->debug.log_char.slot_id;
    /* Store the byte */
    uim_ptr->debug.log_data.data[uim_ptr->debug.log_data.length++] = uim_ptr->debug.log_char.the_char;
  }
  else if (SINGLE_SLOT_LOGGING_MASK_1098 == uim_ptr->debug.apdu_log_mask.log_mask &&
             uim_ptr->debug.log_data.length  <= (LOG_UIM_MAX_CHARS - 2))
  {
    /* Store the attribute byte */
    uim_ptr->debug.log_data.data[uim_ptr->debug.log_data.length++] = uim_ptr->debug.log_char.attrib;
    /* Store the byte */
    uim_ptr->debug.log_data.data[uim_ptr->debug.log_data.length++] = uim_ptr->debug.log_char.the_char;
  }

  if(uim_ptr->debug.log_data.length  >= LOG_UIM_MAX_CHARS)
  {
    if(!uim_log_put_buffer(uim_ptr))
    {
      /* As buffer could not be committed, reset the length of buffer
         to prevent buffer overflow */
      uim_ptr->debug.log_data.length = 0;
      uim_ptr->debug.log_char.attrib = UIM_LOG_ATTRIB_INIT;
    }
  }
} /* uim_log_put_char */


/**
 * This function put time stamp along with attrib byte to the
 * buffer
 *
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_log_put_tstamp
(
  uim_instance_global_type *uim_ptr
)
{
  qword                t_stamp;
  uim_log_data_type   *uim_log_data_ptr = NULL;

  if (uim_ptr->id  >= UIM_MAX_INSTANCES)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"uim_log_put_tstamp invalid instance");
    return;
  }

  uim_log_data_ptr = &uim_ptr->debug.log_data;

  /* LOG_UIM_MAX_CHARS is 247 while uim_log_data_ptr->data[] size is
     255 bytes. Last 8 bytes are reserved for time stamp.
     Need to add boundary checks so that for always keep last 8 bytes
     free for time stamp.

     We add minimum three log bytes log bytes before time stamp. We need to
     adjust boundary check appropriately.
  */
  if (uim_log_data_ptr->length < (LOG_UIM_MAX_CHARS - 1))
  {
    uim_ptr->debug.log_char.attrib = UIM_LOG_TSTAMP;
    if (MULTI_SLOT_LOGGING_MASK_14CE == uim_ptr->debug.apdu_log_mask.log_mask)
    {
      /* Store the attribute byte */
      uim_log_data_ptr->data[uim_log_data_ptr->length++] = uim_ptr->debug.log_char.attrib;
      /* Store the slot byte */
      uim_log_data_ptr->data[uim_log_data_ptr->length++] = uim_ptr->debug.log_char.slot_id;
    }
    else if (SINGLE_SLOT_LOGGING_MASK_1098 == uim_ptr->debug.apdu_log_mask.log_mask)
    {
      /* Store the attribute byte */
      uim_log_data_ptr->data[uim_log_data_ptr->length++] = uim_ptr->debug.log_char.attrib;
    }
    /* Get time stamp */
    /*lint -e{792) supress void cast of void */
    (void)time_get(t_stamp);
    /* Copy the time stamp to the buffer */
    uim_memscpy( (void *) &uim_log_data_ptr->data[uim_log_data_ptr->length],
            (sizeof(uim_log_data_ptr->data) - uim_log_data_ptr->length),
            (void *) t_stamp,
            LOG_UIM_TSTAMP_SIZE);
    /* Update the buffer length */
    uim_log_data_ptr->length   += LOG_UIM_TSTAMP_SIZE;
  }

  if(uim_log_data_ptr->length  >= LOG_UIM_MAX_CHARS)
  {
    if(!uim_log_put_buffer(uim_ptr))
    {
      /* As buffer could not be committed, reset the length of buffer
         to prevent buffer overflow */
      uim_log_data_ptr->length = 0;
      uim_ptr->debug.log_char.attrib = UIM_LOG_ATTRIB_INIT;
    }
  }
} /* uim_log_put_tstamp */
