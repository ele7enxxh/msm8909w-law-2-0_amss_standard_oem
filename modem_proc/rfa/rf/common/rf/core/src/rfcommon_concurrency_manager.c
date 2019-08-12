/*!
   @file
   rfcommon_concurrency_manager.c

   @brief
 
*/

/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================
Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 

                       EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------------------------------------------------------------------------------
05/31/16   rs      Reduce redundant debug messages
02/24/15   am      Change IRAT alt path lookup implementation 
12/19/14   kg      For EXPECTED reservations reset the RFM_DEVICE to default state
12/12/14   kg/sty  Fixed checks to be able to pickup entries [inv-inv-Tech-Tech]
                   from the RFC tables
12/04/14   kg      Added device alloc update buffer to log TRM's updates
                   Added the dev4, dev5 entries to debug logs
                   Do not update db state if the assignment is of expected type
11/25/14   kg      Added concurreny DB read/write mutex
11/17/14   tks     Removed future reservation handling in dev alloc update
11/12/14   tks     Fix bit mask manipulation issue
11/11/14   cvd     Log inputs and outputs of functions in a circular buffer
11/10/14   kg      Add ftm api for setting concurrency manager debug mode
10/30/14   tks     Updated concurreny function to offer faster exec time
10/27/14   pl      Bug fixes on decoupling
10/23/14   tks/pl  Update concurrency manager to decouple alt_path update from TRM
                   by searching again in device_alloc()
10/17/14   pl      Update concurrency manager IRAT search algorithm
10/17/14   tks     Updated check concurrency api to return alt path info in all 
                   control return points 
10/07/14   tks     Updated rf interface to provide alt path info 
09/19/14   tks     Converted if checks to rfm max wan devices
09/19/14   tks     Added time profiling code and default state is disabled
08/19/14   pl      Update alt_path search algorithm for IRAT
08/08/14   tks     Updated table size validation procedure
08/04/14   tks     Updated validate scenario api to return status per rfm dev
07/27/14   tks     Updated alternate rx path function to handle irat/standalone
07/23/14   tks     Included core utils header file to use helper function
07/22/14   tks     Update ccry mgr to support alt path for irat/ifreq 
07/07/14   tks     Modified return logic for validate scenario api
06/16/14   tks     Added support for trm concurrency functions
05/28/14   tks     Added interface api's for trm support
05/12/14   tks     Updated api names to indicate rx behavior
05/12/14   kab     Remove RFCOM_RECEIVER_DUAL Support
04/24/14   tks     Fix klocwork error
04/23/14   tks     Initial revision 
 
==============================================================================*/

#include "rfcommon_concurrency_manager.h"
#include "rfm.h"
#include "rfc_card.h"
#include "rfcommon_core_utils.h"
#include "rfcommon_time_profile.h"
#include "stringl.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif 

/* One single copy of the concurrency manager */
static rfcmn_state_mgr_type rfcmn_concurrency_db;

/* Mutex for the concurrency DB read/write operations*/
static qurt_mutex_t rfcmn_concurrency_mgr_db_lock;

/* Time profiling for concurrency api's */
static boolean rf_ccry_mgr_time_prof = FALSE;

/* Structure to hold Rx reconfig status for each logical device */
rfm_device_allocation_status_type rx_reconfig; 

/*----------------------------------------------------------------------------*/
/*          Cirular buffer logging concurrency manager                        */
/*----------------------------------------------------------------------------*/

#define RFM_CM_CMD_NUM_BUFFS 800

#define RFM_CMD_DEV_ALLOC_BUFFS 10

typedef struct {
  void * func_ptr;
  uint32 args[5];
} rfm_cm_cmd_type;

typedef struct {
  int32 last_entry;
  rfm_cm_cmd_type entry[RFM_CM_CMD_NUM_BUFFS];
} rfm_cm_cmd_buffer_type;


typedef struct {
  int8 last_entry;
  rfm_device_allocation_type entry[RFM_CMD_DEV_ALLOC_BUFFS];
} rfm_cm_dev_alloc_buffer_type;


rfm_cm_cmd_buffer_type rfm_cm_cmd_buffs;

rfm_cm_dev_alloc_buffer_type rfm_cm_dev_alloc_buffs;

/* circular buffer */
#define RFM_CM_DEBUG_EN_Q(x,a,b,c,d,e)                   \
{                                                   \
  rfm_cm_cmd_buffs.last_entry++;                       \
  rfm_cm_cmd_buffs.last_entry = rfm_cm_cmd_buffs.last_entry % RFM_CM_CMD_NUM_BUFFS;    \
  rfm_cm_cmd_buffs.entry[rfm_cm_cmd_buffs.last_entry].func_ptr = (void*)(x); \
  rfm_cm_cmd_buffs.entry[rfm_cm_cmd_buffs.last_entry].args[0] = (uint32)(a);    \
  rfm_cm_cmd_buffs.entry[rfm_cm_cmd_buffs.last_entry].args[1] = (uint32)(b);    \
  rfm_cm_cmd_buffs.entry[rfm_cm_cmd_buffs.last_entry].args[2] = (uint32)(c);    \
  rfm_cm_cmd_buffs.entry[rfm_cm_cmd_buffs.last_entry].args[3] = (uint32)(d);    \
  rfm_cm_cmd_buffs.entry[rfm_cm_cmd_buffs.last_entry].args[4] = (uint32)(e);    \
}

#define RFM_CM_DEBUG_DEV_ALLOC_BUFFS_EN_Q(input_arg)                \
{                                                                   \
  rfm_cm_dev_alloc_buffs.last_entry++;                              \
  rfm_cm_dev_alloc_buffs.last_entry = rfm_cm_dev_alloc_buffs.last_entry % RFM_CMD_DEV_ALLOC_BUFFS;  \
  memscpy(&rfm_cm_dev_alloc_buffs.entry[rfm_cm_dev_alloc_buffs.last_entry],                   \
         sizeof(rfm_device_allocation_type), input_arg, sizeof(rfm_device_allocation_type));  \
}

/*----------------------------------------------------------------------------*/
/*          Function prototypes internal to concurrency manager               */
/*----------------------------------------------------------------------------*/
boolean rfcmn_reset_rx_concurrency_tbl_entry(rfm_device_enum_type rfm_dev); 

rfcmn_status_type rfcmn_concurrency_mgr_get_alt_rx_path
(
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type curr_mode,
  int curr_band,
  uint8 *alt_path,
  boolean is_irat_mode
); 

/*----------------------------------------------------------------------------*/
/*                          Interface functions                               */
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  API to initialize concurrency manager 
 
  @details
  Function is used to initialize the concurrency manager to default state. The
  current band and technologies per logical device are set to default state,
  while the alternate path and wide band indices are set to zero. This function
  should be called as part of rfm_init
 
  @return
  TRUE indicates initialization is successful 
*/
rfcmn_status_type rfcmn_concurrency_mgr_rx_init(void)
{
  uint8 loop_idx = 0;

  RFM_CM_DEBUG_EN_Q(&rfcmn_concurrency_mgr_rx_init, 0, 0, 0, 0, 0);

  for(loop_idx = (uint8)RFM_DEVICE_0; loop_idx < RFM_MAX_WAN_DEVICES; loop_idx++)
  {
    rfcmn_reset_rx_concurrency_tbl_entry((rfm_device_enum_type)loop_idx); 
  }

  qurt_mutex_init(&rfcmn_concurrency_mgr_db_lock);

  return RFCMN_PATH_SEL_SUCCESS; 

}/* end of rfcmn_concurrency_mgr_rx_init() */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  API to update concurrency manager at start of channel tune 
 
  @details
  Function is used to determine the alternate path to use for a particular
  scenario. The alternate path and wide band indices help decide which
  physical path needs to be configured in a particular scenario. The selection
  of a specific path depends on which tech is using a particular chain and how
  to perform configuration without breaking the existing link. 
 
  @param rfm_dev
  current logical device in use 
 
  @param curr_mode
  current technology in use 
 
  @param curr_band
  current band in use 

  @return
  TRUE is a successful match is found in path selection table, else return FALSE 

*/
rfcmn_status_type rfcmn_concurrency_mgr_update_rx_state
(
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type curr_mode,
  int curr_band
)
{

  RFM_CM_DEBUG_EN_Q(&rfcmn_concurrency_mgr_update_rx_state, rfm_dev, curr_mode, curr_band, 0, 0);

  /* Update concurrency manager table */
  rfcmn_concurrency_db.curr_state[rfm_dev].logical_dev = rfm_dev;
  rfcmn_concurrency_db.curr_state[rfm_dev].curr_mode   = curr_mode;
  rfcmn_concurrency_db.curr_state[rfm_dev].curr_band   = curr_band;

  return RFCMN_CONCURRENCY_MGR_UPDATE_SUCCESS; 

} /* end of rfcmn_concurrency_mgr_update_rx_state() */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  API to update concurreny manager during exit mode 
 
  @details
  Function resets the all information related to particular logical device.
  Clean up here effectively means that a particular tech/band/chan is done using
  a specific logical device and is quitting that mode. 
 
  @param rfm_dev
  current logical device in use 
 
  @param curr_mode
  current technology in use 
 
  @param curr_band
  current band in use 

  @return
  TRUE if clean up of a specific logical device is successful, else return FALSE 

*/
rfcmn_status_type rfcmn_concurrency_mgr_rx_cleanup
(
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type curr_mode,
  int curr_band
)
{
  boolean api_status = FALSE; 

  if( rfcmn_concurrency_db.curr_state[rfm_dev].logical_dev == rfm_dev &&
      rfcmn_concurrency_db.curr_state[rfm_dev].curr_mode == curr_mode &&
      rfcmn_concurrency_db.curr_state[rfm_dev].curr_band == curr_band )
  {
    api_status = rfcmn_reset_rx_concurrency_tbl_entry(rfm_dev);
  }

  RFM_CM_DEBUG_EN_Q(&rfcmn_concurrency_mgr_rx_cleanup, rfm_dev, curr_mode, curr_band, api_status, 0);

  if (api_status)
  {
    return RFCMN_PATH_SEL_SUCCESS; 
  }
  else
  {
    return RFCMN_PATH_SEL_FAILED; 
  }

} /* end of rfcmn_concurrency_mgr_rx_cleanup() */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Helper function to detect alternate path information and update the devices
 
  @details
  Function is called when an alternate path selection has to be made based 
  on concurrency scenario. Based on the alternate path selected the api needs
  to figure out the path swapping requirement, cell reconfiguration and also
  indicate if a possible reconfig has succeeded or failed. 
 
  @param rfm_dev
  current logical device in use 
 
  @param curr_mode
  current technology in use 
 
  @param curr_band
  current band in use 
 
  @return alt_path
  return the alternate path to use for current logical device

*/
rfcmn_status_type rfcmn_concurrency_mgr_get_alt_rx_path
(
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type curr_mode,
  int curr_band,
  uint8 *alt_path,
  boolean is_irat_mode
)
{
  uint32 idx                         = 0;
  uint32 idx_in_irat_group           = 0;
  uint32 num_irat_groups             = 0;
  uint32 num_band_group_entries      = 0;
  uint32 tbl_index                   = 0; 
  uint32 tbl_size                    = 0;
  uint32 lst_size                    = 0; 
  int matching_band                  = 0; 
  int temp_band                      = 0;
  int home_band                      = 0;
  rfcom_mode_enum_type home_mode     = RFM_NUM_MODES;
  uint8 temp_alt_path                = 0;
  boolean match_found                = FALSE;
  boolean entry_found                = FALSE; 
  boolean second_match_found         = FALSE; 
  boolean measuring_tech_match_found = FALSE;
  rfm_device_enum_type temp_rfm_dev  = RFM_INVALID_DEVICE; 
  rfcmn_status_type api_status       = RFCMN_PATH_SEL_INVALID; 
  rfcom_mode_enum_type matching_mode = RFM_NUM_MODES;
  rfcom_mode_enum_type temp_mode     = RFM_NUM_MODES;
  rfc_alt_path_sel_type *curr_tbl    = NULL;
  rf_time_type prof_cal_func_val     = 0; 
  rf_time_tick_type prof_cal_func; 
  rfc_alt_path_sel_type *irat_alt_path_tbl_entry  = NULL;

  RFM_CM_DEBUG_EN_Q(&rfcmn_concurrency_mgr_get_alt_rx_path, rfm_dev, curr_mode, curr_band, is_irat_mode, 0);
  
  if (rf_ccry_mgr_time_prof)
  {
    prof_cal_func = rf_time_get_tick();
  }

  /* Parameter checks */
  if (rfm_dev >= RFM_MAX_WAN_DEVICES)
  {
    RF_MSG_1(RF_HIGH, "rf_ccry_get_alt_path: Invalid rfm_dev %d", rfm_dev); 
    return RFCMN_PATH_SEL_FAILED;
  }

  if (curr_mode >= RFM_NUM_MODES)
  {
    RF_MSG_1(RF_HIGH, "rf_ccry_get_alt_path: Invalid mode %d", curr_mode); 
    return RFCMN_PATH_SEL_FAILED;
  }

  /* In IRAT mode, we use the IRAT alt path selection table and the 
     band/mode information provided by the tech */
  if (is_irat_mode)
  {
    curr_tbl      = rfc_get_irat_alt_path_tbl( &num_band_group_entries, &num_irat_groups); 
    matching_mode = curr_mode;
    matching_band = curr_band;
  }
  /* In normal/standalone/non-IRAT scenarios the band/mode 
     information from concurrency database should be used */
  else
  {
    curr_tbl      = rfc_get_alt_path_tbl( &tbl_size ); 
    matching_mode = rfcmn_concurrency_db.curr_state[rfm_dev].curr_mode;
    matching_band = rfcmn_concurrency_db.curr_state[rfm_dev].curr_band;
  }

  /* Capping list size at max rfm devices */
  lst_size = (uint32)RFM_MAX_WAN_DEVICES; 

  /* Check if this specific rf_card supports alternate path selection */
  if (curr_tbl == NULL)
  {
    RF_MSG(RF_HIGH, "rf_ccry_get_alt_path: Alternate path selection is "
                    "not supported on this specific rf_card"); 

    /* Set default alternate path & return path selection is not supported */
    *alt_path = 0; 
    return RFCMN_PATH_SEL_NOT_SUPPORTED; 
  }


  qurt_mutex_lock(&rfcmn_concurrency_mgr_db_lock);

  /* Loop through the alt path table to find a matching mode and band  */
  /* IRAT scenario */
  if( is_irat_mode )
  {
      temp_alt_path = 0;
      home_mode = rfcmn_concurrency_db.curr_state[rfm_dev].curr_mode;
      home_band = rfcmn_concurrency_db.curr_state[rfm_dev].curr_band;

      if (rf_ccry_mgr_time_prof)
      {
        RF_MSG_2(RF_HIGH, "rf_ccry_get_alt_path: Got num irat groups %d, and num band groups %d", num_irat_groups, num_band_group_entries); 
        RF_MSG_6(RF_HIGH, "rf_ccry_get_alt_path: rfm_dev %d, home_band shift %ld, home_mode %d, home altpath %d, matching_mode %d, matching_band %ld", rfm_dev, 
             ((uint64)1 << home_band), home_mode, rfcmn_concurrency_db.curr_state[rfm_dev].alt_path, matching_mode, (uint64)1 << matching_band);
      }

      curr_tbl += (rfm_dev*num_irat_groups*num_band_group_entries);

      for (idx = 0; idx < num_irat_groups; idx++) /* idx should be bounded by ?s*/
      {
        irat_alt_path_tbl_entry = curr_tbl + idx*num_band_group_entries;

        for (idx_in_irat_group = 0; idx_in_irat_group < num_band_group_entries; idx_in_irat_group++, irat_alt_path_tbl_entry++)
        {
          if  ((irat_alt_path_tbl_entry->band & ((uint64)1 << home_band)) &&
              (irat_alt_path_tbl_entry->tech == home_mode) &&
              (irat_alt_path_tbl_entry->alt_path == rfcmn_concurrency_db.curr_state[rfm_dev].alt_path))
          {
            /*We found the entry in one group*/
            second_match_found = TRUE;
            if (rf_ccry_mgr_time_prof)
            {
              RF_MSG_2(RF_HIGH, "rf_ccry_get_alt_path: Found matching entry at index %d, big idx %d", idx_in_irat_group, idx); 
            }
          }
          /*Store the measuring tech alt path in temp var, if group matches then we return this*/
          if (((irat_alt_path_tbl_entry->band) & (uint64)1 << matching_band) &&
              (irat_alt_path_tbl_entry->tech == matching_mode))
          {
            temp_alt_path = irat_alt_path_tbl_entry->alt_path;
            measuring_tech_match_found = TRUE;
            if (rf_ccry_mgr_time_prof)
            {
              RF_MSG_3(RF_HIGH, "rf_ccry_get_alt_path: Alt path %d, at index %d, big idx %d", temp_alt_path, idx_in_irat_group, idx); 
            }
          }
          if (second_match_found == TRUE && measuring_tech_match_found == TRUE)
          {
            break;
          }
        }
        if (second_match_found == TRUE)
        {
          break;
        }
      }
      
      if (second_match_found == TRUE)
      {
        *alt_path = temp_alt_path;
      }
      else
      {
        *alt_path = 0;
        RF_MSG(RF_HIGH, "rf_ccry_get_alt_path: Found no match in the table"); 
      }
  }
  /* Normal/standalone/non-IRAT scenario */
  else
  {
    /* Retrieve the information from rf concurrency database */
    *alt_path = rfcmn_concurrency_db.curr_state[rfm_dev].alt_path; 
  }

  qurt_mutex_unlock(&rfcmn_concurrency_mgr_db_lock);

  /* Path selection is successfull */
  api_status = RFCMN_PATH_SEL_SUCCESS; 

  if (rf_ccry_mgr_time_prof)
  {
    prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );
    RF_MSG_1(RF_HIGH, "T_PROF: rfcmn_concurrency_mgr_get_alt_rx_path - %d", prof_cal_func_val);
    RF_MSG_4(RF_HIGH, "T_PROF: rfcmn_concurrency_mgr_get_alt_rx_path - dev %d, tech %d, band %d, alt_path %d", 
             rfm_dev,curr_mode,curr_band,*alt_path);
  }

  RFM_CM_DEBUG_EN_Q(&rfcmn_concurrency_mgr_get_alt_rx_path, *alt_path, 0, 0, 0, 0);

  return api_status; 

} /* end of rfcmn_concurrency_mgr_get_alt_rx_path() */

/*----------------------------------------------------------------------------*/
rfcmn_status_type rfcmn_concurrency_mgr_update_rx_path
(
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type curr_mode,
  int curr_band,
  uint8 *alt_path
)
{
  /* Get alternate path for standalone tech operation */
  return 
    (rfcmn_concurrency_mgr_get_alt_rx_path(rfm_dev, curr_mode, 
                                           curr_band, alt_path, FALSE)); 

} /* end of rfcmn_concurrency_mgr_update_rx_path() */

/*----------------------------------------------------------------------------*/
rfcmn_status_type rfcmn_concurrency_mgr_update_irat_rx_path
(
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type curr_mode,
  int curr_band,
  uint8 *alt_path
)
{
  /* Get alternate path for inter-rat or inter-freq tech operation */
  return 
    (rfcmn_concurrency_mgr_get_alt_rx_path(rfm_dev, curr_mode, 
                                           curr_band, alt_path, TRUE)); 

} /* end of rfcmn_concurrency_mgr_update_irat_rx_path() */

/*----------------------------------------------------------------------------*/
/*                         TRM interface functions                            */
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Logical device allocation for a tech and band combo
 
  @details
  This function will update the rf concurrency manager with the current
  tech and band allocation details provided by TRM. 
 
  @param dev_allocation
  contains details about which band and tech is used on a specific logical
  device 
  
  @return 
  Flag to indicate if the device allocation has been successful or not 
*/
boolean 
rfcmn_concurrency_mgr_rx_update_device_alloc
(
  rfm_device_allocation_type *dev_list
)
{
  uint8 arr_idx = 0; 
  uint32 idx                 = 0;
  uint32 tbl_index           = 1024; 
  uint32 tbl_size            = 0;
  boolean match_found        = FALSE;
  boolean entry_found        = FALSE; 
  boolean is_reconfig_needed = FALSE; 
  int test_rf_band           = -1; 
  rfcom_mode_enum_type test_rf_mode = RFCOM_NUM_MODES; 
  rfm_device_enum_type temp_rfm_dev = RFM_INVALID_DEVICE; 
  rfc_alt_path_sel_type *curr_tbl   = rfc_get_alt_path_tbl( &tbl_size );
  uint32 lst_size                   = (uint8)RFM_MAX_WAN_DEVICES; 

  RFM_CM_DEBUG_EN_Q(&rfcmn_concurrency_mgr_rx_update_device_alloc, &dev_list, 0, 0, 0, 0);
  RFM_CM_DEBUG_DEV_ALLOC_BUFFS_EN_Q(dev_list);

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif 

  if (dev_list == NULL)
  {
    RF_MSG(RF_ERROR, "RF concurrency mgr device alloc list is empty"); 
    return FALSE; 
  }

  if (rf_ccry_mgr_time_prof)
  {
    RF_MSG_6(RF_HIGH, "rf_ccry_update_dev_alloc: Tech input from MCS - %d, %d, %d, %d, %d, %d", 
             dev_list->device_allocation[0].tech,
             dev_list->device_allocation[1].tech,
             dev_list->device_allocation[2].tech,
             dev_list->device_allocation[3].tech,
             dev_list->device_allocation[4].tech,
             dev_list->device_allocation[5].tech); 
    RF_MSG_6(RF_HIGH, "rf_ccry_update_dev_alloc: Band input from MCS - %d, %d, %d, %d, %d, %d", 
             rfcommon_core_convert_sys_band_to_rfcom_band(dev_list->device_allocation[0].band),
             rfcommon_core_convert_sys_band_to_rfcom_band(dev_list->device_allocation[1].band),
             rfcommon_core_convert_sys_band_to_rfcom_band(dev_list->device_allocation[2].band),
             rfcommon_core_convert_sys_band_to_rfcom_band(dev_list->device_allocation[3].band),
             rfcommon_core_convert_sys_band_to_rfcom_band(dev_list->device_allocation[4].band),
             rfcommon_core_convert_sys_band_to_rfcom_band(dev_list->device_allocation[5].band)); 
    RF_MSG_6(RF_HIGH, "rf_ccry_update_dev_alloc: Alt path input from MCS - %d, %d, %d, %d, %d, %d",  
             dev_list->device_allocation[0].alt_path, 
             dev_list->device_allocation[1].alt_path, 
             dev_list->device_allocation[2].alt_path, 
             dev_list->device_allocation[3].alt_path, 
             dev_list->device_allocation[4].alt_path, 
             dev_list->device_allocation[5].alt_path); 
  }


  qurt_mutex_lock(&rfcmn_concurrency_mgr_db_lock);

  for ( arr_idx = (uint8)RFM_DEVICE_0; 
        arr_idx < (uint8)RFM_MAX_WAN_DEVICES; 
        arr_idx++ )
  {
    /* Update the state machine only for assign request, future reservations
       would be skipped this way */
    if ( dev_list->device_allocation[arr_idx].allocation_type != 
         RFM_DEVICE_EXPECTED )
    {
      /* Update concurrency db with band info */
      rfcmn_concurrency_db.curr_state[arr_idx].curr_band 
        = rfcommon_core_convert_sys_band_to_rfcom_band(dev_list->device_allocation[arr_idx].band); 

      /* Update concurrency db with tech info */
      rfcmn_concurrency_db.curr_state[arr_idx].curr_mode
        = dev_list->device_allocation[arr_idx].tech; 

      /* Update concurrency db with logical dev info */
      rfcmn_concurrency_db.curr_state[arr_idx].logical_dev
        = (rfm_device_enum_type)arr_idx;

      /* Setting to default alt path */
      rfcmn_concurrency_db.curr_state[arr_idx].alt_path = 0; 
    }
    else
    {
      /* Reset the device to when state EXPECTED */
      rfcmn_concurrency_db.curr_state[arr_idx].curr_mode   = RFCOM_NUM_MODES; 
      rfcmn_concurrency_db.curr_state[arr_idx].curr_band   = 0xFF; 
      rfcmn_concurrency_db.curr_state[arr_idx].logical_dev
        = (rfm_device_enum_type)arr_idx;
      rfcmn_concurrency_db.curr_state[arr_idx].alt_path    = 0;
    }
  }

  /* Update the alternate path information in db */
  if ((curr_tbl != NULL) && (tbl_size != 0))
  {
    /*** Loop through the table to find a match ***/
    for (idx = 0; idx < tbl_size; idx++)
    {
      match_found = TRUE;
      entry_found = FALSE; 

      /*** Loop through all logical devices ***/
      for ( temp_rfm_dev = RFM_DEVICE_0; 
            temp_rfm_dev < (rfm_device_enum_type)lst_size; temp_rfm_dev++ )
      {
        test_rf_mode = rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_mode;
        test_rf_band = rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_band;

        /* Check if a table match is found */
        if( ((curr_tbl + idx * lst_size + temp_rfm_dev)->rx_path == RFC_PRX_PATH)  && 
           ((curr_tbl + idx * lst_size + temp_rfm_dev)->tech != RFM_ALL_MODES) )
        {
          if( ((((curr_tbl + idx * lst_size + temp_rfm_dev)->band) & ((uint64)1 << test_rf_band)) || test_rf_mode == RFM_INVALID_MODE )&& 
              ((curr_tbl + idx * lst_size + temp_rfm_dev)->tech == test_rf_mode ) )
          {
            match_found &= TRUE; 
            entry_found  = TRUE; 
          }
          else
          {
            match_found &= FALSE; 
          }
        }
      }
  
      /*** If match found then store the table index ***/
      if (match_found == TRUE && entry_found == TRUE)
      {
        tbl_index = idx;
        break; 
      }
    }
  
    /*if (rf_ccry_mgr_time_prof)
    {
      RF_MSG_4(RF_HIGH, "rf_ccry_update_dev_alloc: MF - %d, EF = %d, Index = %d, Tbl_index = %d",  
               match_found, entry_found, idx, tbl_index);
    }*/

    /** Match is found, then update alt paths correctly */
    if (match_found == TRUE && entry_found == TRUE)
    {
      for (idx = (uint8)RFM_DEVICE_0; idx < (uint8)RFM_MAX_WAN_DEVICES; idx++)
      {
        if ( rfcmn_concurrency_db.curr_state[idx].curr_mode < RFM_NUM_MODES && 
             rfcmn_concurrency_db.curr_state[idx].curr_mode != RFM_PARKED_MODE )
        {
          rfcmn_concurrency_db.curr_state[idx].alt_path = 
            (curr_tbl + tbl_index * lst_size + idx)->alt_path; 
        }
      }
      if (rf_ccry_mgr_time_prof)
      {
        RF_MSG_1(RF_HIGH, "rf_ccry_update_dev_alloc: Path updated for index = %d", idx); 
      }
    }
  }
  
  qurt_mutex_unlock(&rfcmn_concurrency_mgr_db_lock);  
  
  if (rf_ccry_mgr_time_prof)
  {
    RF_MSG_6(RF_HIGH, "rf_ccry_update_dev_alloc: updated DB tech - %d, %d, %d, %d, %d, %d",  
             rfcmn_concurrency_db.curr_state[0].curr_mode, 
             rfcmn_concurrency_db.curr_state[1].curr_mode, 
             rfcmn_concurrency_db.curr_state[2].curr_mode, 
             rfcmn_concurrency_db.curr_state[3].curr_mode, 
             rfcmn_concurrency_db.curr_state[4].curr_mode, 
             rfcmn_concurrency_db.curr_state[5].curr_mode); 
    RF_MSG_6(RF_HIGH, "rf_ccry_update_dev_alloc: updated DB band - %d, %d, %d, %d, %d, %d",  
             rfcmn_concurrency_db.curr_state[0].curr_band, 
             rfcmn_concurrency_db.curr_state[1].curr_band, 
             rfcmn_concurrency_db.curr_state[2].curr_band,
             rfcmn_concurrency_db.curr_state[3].curr_band, 
             rfcmn_concurrency_db.curr_state[4].curr_band,
             rfcmn_concurrency_db.curr_state[5].curr_band); 
    RF_MSG_6(RF_HIGH, "rf_ccry_update_dev_alloc: updated DB alt_path - %d, %d, %d, %d, %d, %d",  
             rfcmn_concurrency_db.curr_state[0].alt_path, 
             rfcmn_concurrency_db.curr_state[1].alt_path,
             rfcmn_concurrency_db.curr_state[2].alt_path,
             rfcmn_concurrency_db.curr_state[3].alt_path,
             rfcmn_concurrency_db.curr_state[4].alt_path,
             rfcmn_concurrency_db.curr_state[5].alt_path); 
  }
  
  return TRUE; 

} /* end of rfcmn_concurrency_mgr_rx_update_device_alloc */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Check concurrency possibilities for a particular tech and band combo
 
  @details
  The function which check with the rf concurrency manager if it is possible
  to allocate a particular logical device to a specific tech and band combo
 
  @param dev_client
  contains details about which band and tech is used on a specific logical
  device
 
  @return 
  Flag to indicate if the dev client info is valid or not
*/
rfm_device_allocation_status_type* 
rfcmn_concurrency_mgr_rx_validate_scenario
(
  rfm_device_client_info_type *dev_client
)
{
  uint32 idx                 = 0;
  uint32 tbl_index           = 1024; 
  uint32 tbl_size            = 0;
  uint32 lst_size            = 0; 
  boolean match_found        = FALSE;
  boolean entry_found        = FALSE; 
  boolean is_reconfig_needed = FALSE; 
  int test_rf_band    = -1; 
  rfcom_mode_enum_type test_rf_mode = RFCOM_NUM_MODES; 
  rfm_device_enum_type temp_rfm_dev = RFM_INVALID_DEVICE; 
  rfc_alt_path_sel_type *curr_tbl = rfc_get_alt_path_tbl( &tbl_size );
  rf_time_type prof_cal_func_val = 0; 
  rf_time_tick_type prof_cal_func; 

  if (rf_ccry_mgr_time_prof)
  {
    prof_cal_func = rf_time_get_tick();
  }

  lst_size = (uint32)RFM_MAX_WAN_DEVICES; 

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif 

  memset(&(rx_reconfig.curr_status[0]), 
         RFM_DEVICE_STATUS_INVALID, 
         sizeof(rfm_device_reconfig_status_type)*RFM_MAX_WAN_DEVICES); 

  memset(&(rx_reconfig.alt_path[0]), 0, sizeof(uint8)*RFM_MAX_WAN_DEVICES); 


  /*! Control return point 1 => dev_client struct from MCS is NULL */
  if (dev_client == NULL)
  {
    RF_MSG(RF_ERROR, "rf_ccry_validate_scenario: RF concurrency mgr device alloc client is empty"); 
    return &rx_reconfig;
  }

  /*! Control return point 2 => RFC has no support for path selection */
  if (curr_tbl == NULL)
  {
    RF_MSG(RF_HIGH, "rf_ccry_validate_scenario: RFC doesn't have path selection table - all ccry modes supproted"); 
    return &rx_reconfig; 
  }

  if (rf_ccry_mgr_time_prof)
  {
    RF_MSG_3(RF_HIGH, "rf_ccry_validate_scenario: validate input from MCS mode - dev:%d, tech:%d, band:%d", 
             dev_client->rfm_dev, dev_client->tech,dev_client->band); 

    RF_MSG_6(RF_HIGH, "rf_ccry_validate_scenario: validate input DB tech - %d, %d, %d, %d, %d, %d", 
             rfcmn_concurrency_db.curr_state[0].curr_mode,
             rfcmn_concurrency_db.curr_state[1].curr_mode,
             rfcmn_concurrency_db.curr_state[2].curr_mode,
             rfcmn_concurrency_db.curr_state[3].curr_mode,
             rfcmn_concurrency_db.curr_state[4].curr_mode,
             rfcmn_concurrency_db.curr_state[5].curr_mode); 

    RF_MSG_6(RF_HIGH, "rf_ccry_validate_scenario: validate input DB band - %d, %d, %d, %d, %d, %d", 
             rfcmn_concurrency_db.curr_state[0].curr_band,
             rfcmn_concurrency_db.curr_state[1].curr_band,
             rfcmn_concurrency_db.curr_state[2].curr_band,
             rfcmn_concurrency_db.curr_state[3].curr_band,
             rfcmn_concurrency_db.curr_state[4].curr_band,
             rfcmn_concurrency_db.curr_state[5].curr_band); 
  }

  /*! Control return point 3 => Chain is already in use, so no checks */
  if ( rfcmn_concurrency_db.curr_state[dev_client->rfm_dev].curr_mode < RFM_NUM_MODES && 
       rfcmn_concurrency_db.curr_state[dev_client->rfm_dev].curr_mode != RFM_PARKED_MODE )
  {
    for (temp_rfm_dev = 0; temp_rfm_dev < RFM_MAX_WAN_DEVICES; temp_rfm_dev++)
    {
      /* Provide the same alt path information to TRM */
      rx_reconfig.alt_path[temp_rfm_dev] = rfcmn_concurrency_db.curr_state[temp_rfm_dev].alt_path; 

      /* Check for chain status on each request */
      if (rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_mode != RFM_NUM_MODES && 
          rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_mode != RFM_PARKED_MODE)
      {
        if ((temp_rfm_dev == dev_client->rfm_dev) && 
            (rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_band != 
             rfcommon_core_convert_sys_band_to_rfcom_band(dev_client->band)))
        {
          rx_reconfig.curr_status[temp_rfm_dev] = RFM_DEVICE_RECONFIG_REQUIRED; 
        }
        else
        {
          rx_reconfig.curr_status[temp_rfm_dev] = RFM_DEVICE_RECONFIG_NOT_REQD; 
        }
      }
      else
      {
        rx_reconfig.curr_status[temp_rfm_dev] = RFM_DEVICE_CURRENTLY_INACTIVE;
      }
    }

    if (rf_ccry_mgr_time_prof)
    {
      prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );
      /* Chain is already in use, hence deny the request */
      RF_MSG_6(RF_HIGH, "rf_ccry_validate_scenario: Chain_In_Use - Alt path sent to MCS - %d, %d, %d, %d, %d, %d", 
               rx_reconfig.alt_path[0], rx_reconfig.alt_path[1], 
               rx_reconfig.alt_path[2], rx_reconfig.alt_path[3], 
               rx_reconfig.alt_path[4], rx_reconfig.alt_path[5]); 

      RF_MSG_6(RF_HIGH, "rf_ccry_validate_scenario: Chain_In_Use - Reconfig sent to MCS - %d, %d, %d, %d, %d, %d", 
               rx_reconfig.curr_status[0], rx_reconfig.curr_status[1], 
               rx_reconfig.curr_status[2], rx_reconfig.curr_status[3], 
               rx_reconfig.curr_status[4], rx_reconfig.curr_status[5]); 
  
      RF_MSG_1(RF_HIGH, "rf_ccry_validate_scenario: Chain_In_Use - rfcmn_concurrency_mgr_rx_validate_scenario - %d", 
               prof_cal_func_val);
    }

    RFM_CM_DEBUG_EN_Q(&rfcmn_concurrency_mgr_rx_validate_scenario, dev_client->rfm_dev, dev_client->tech, dev_client->band, dev_client->rx_operation, &rx_reconfig);

    return &rx_reconfig; 
  }

  /*! Control return point 4 => Valid concurrency check */
  /* If the logial device is not used by anyone then lets compare the table 
     entries to see if we can get a match */
  for (idx = 0; idx < tbl_size; idx++)
  {
    match_found = TRUE;
    entry_found = FALSE; 

    /*** Loop through all logical devices ***/
    for ( temp_rfm_dev = RFM_DEVICE_0; 
          temp_rfm_dev < (rfm_device_enum_type)lst_size; temp_rfm_dev++ )
    {
      /* Determine the test params - rf mode, rf band and logical device */
      if(dev_client->rfm_dev == temp_rfm_dev)
      {
        // test_rfm_dev = dev_client->rfm_dev; 
        test_rf_mode = dev_client->tech;
        test_rf_band = rfcommon_core_convert_sys_band_to_rfcom_band(dev_client->band); 
      }
      else
      {
        // test_rfm_dev = rfcmn_concurrency_db.curr_state[temp_rfm_dev].logical_dev;
        test_rf_mode = rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_mode;
        test_rf_band = rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_band;
      }

      /* Check if a table match is found */
      if( ((curr_tbl + idx * lst_size + temp_rfm_dev)->rx_path == RFC_PRX_PATH)  && 
          ((curr_tbl + idx * lst_size + temp_rfm_dev)->tech    != RFM_ALL_MODES))
      {
        if( ((((curr_tbl + idx * lst_size + temp_rfm_dev)->band) & ((uint64)1 << test_rf_band)) || test_rf_mode == RFM_INVALID_MODE )&& 
            ((curr_tbl + idx * lst_size + temp_rfm_dev)->tech == test_rf_mode ) )
        {
          match_found &= TRUE; 
          entry_found  = TRUE; 
        }
        else
        {
          match_found &= FALSE; 
        }
      }
    }

    /*** If match found then store the table index ***/
    if (match_found == TRUE && entry_found == TRUE)
    {
      tbl_index = idx;
      break; 
    }
  }

  /*** Check if a match is found in the path selection table ***/
  if (match_found == TRUE && entry_found == TRUE)
  {
    /* Loop thro all logical devices and update status accordingly */
    for (temp_rfm_dev = RFM_DEVICE_0; 
          temp_rfm_dev < RFM_MAX_WAN_DEVICES; temp_rfm_dev++)
    {
      /* Update alt path information to TRM */
      rx_reconfig.alt_path[temp_rfm_dev] = (curr_tbl + idx * lst_size + temp_rfm_dev)->alt_path; 

      /* Update the path status information to TRM */
      if (temp_rfm_dev != dev_client->rfm_dev)
      {
        /* Current logical dev should be in a mode other num mode or parked mode */
        if (rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_mode != RFM_NUM_MODES && 
            rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_mode != RFM_PARKED_MODE)
        {
          /* If alt path is different then re-config is needed */
          if ( rfcmn_concurrency_db.curr_state[temp_rfm_dev].alt_path != 
               (curr_tbl + idx * lst_size + temp_rfm_dev)->alt_path )
          {
            rx_reconfig.curr_status[temp_rfm_dev] = RFM_DEVICE_RECONFIG_REQUIRED; 
          }
          /* If alt path is not different then reconfig is not required */
          else
          {
            rx_reconfig.curr_status[temp_rfm_dev] = RFM_DEVICE_RECONFIG_NOT_REQD; 
          }
        }
        /* Current logical dev is in parked or num mode which means its inactive */
        else
        {
          rx_reconfig.curr_status[temp_rfm_dev] = RFM_DEVICE_CURRENTLY_INACTIVE; 
        }
      }
      /* Current device which is being checked for adding a new cell */
      else
      {
        rx_reconfig.curr_status[temp_rfm_dev] = RFM_DEVICE_CURRENTLY_INACTIVE; 
      }
    }

    RF_MSG_1(RF_HIGH,"rf_ccry_validate_scenario: Match found & Updated - %d", tbl_index); 
  }
  /*** No match is found in path selection table but still need to return an
       accurate picture of what is recorded in rf state machine ***/
  else
  {
    for (temp_rfm_dev = RFM_DEVICE_0; 
          temp_rfm_dev < RFM_MAX_WAN_DEVICES; temp_rfm_dev++)
    {
      /* If there is no path selection entry found then alt path is set to default */
      rx_reconfig.alt_path[temp_rfm_dev] = 0; 

      /* If current mode is not parked or num mode then mark it as no-reconfig */
      if (rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_mode != RFM_NUM_MODES && 
          rfcmn_concurrency_db.curr_state[temp_rfm_dev].curr_mode != RFM_PARKED_MODE)
      {
        rx_reconfig.curr_status[temp_rfm_dev] = RFM_DEVICE_RECONFIG_NOT_REQD; 
      }
      /* If current mode is num or parked mode then mark it as inactive */
      else
      {
        rx_reconfig.curr_status[temp_rfm_dev] = RFM_DEVICE_CURRENTLY_INACTIVE; 
      }
    }

    /*RF_MSG_1(RF_HIGH,"rf_ccry_validate_scenario: Match not found - %d", tbl_index);*/
  }

  if (rf_ccry_mgr_time_prof)
  {
    RF_MSG_6(RF_HIGH, "rf_ccry_validate_scenario: Alt path sent to MCS - %d, %d, %d, %d, %d, %d", 
             rx_reconfig.alt_path[0], rx_reconfig.alt_path[1], 
             rx_reconfig.alt_path[2], rx_reconfig.alt_path[3], 
             rx_reconfig.alt_path[4], rx_reconfig.alt_path[5]); 
  
    RF_MSG_6(RF_HIGH, "rf_ccry_validate_scenario: Reconfig sent to MCS - %d, %d, %d, %d, %d, %d", 
             rx_reconfig.curr_status[0], rx_reconfig.curr_status[1], 
             rx_reconfig.curr_status[2], rx_reconfig.curr_status[3], 
             rx_reconfig.curr_status[4], rx_reconfig.curr_status[5]); 

    prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

    RF_MSG_1(RF_HIGH, "T_PROF: rfcmn_concurrency_mgr_rx_validate_scenario - %d", 
             prof_cal_func_val);
  }

  RFM_CM_DEBUG_EN_Q(&rfcmn_concurrency_mgr_rx_validate_scenario, dev_client->rfm_dev, dev_client->tech, dev_client->band, dev_client->rx_operation, &rx_reconfig);

  return &rx_reconfig; 

} /* end of rfcmn_concurrency_mgr_rx_validate_scenario */

/*----------------------------------------------------------------------------*/
/*                            Helper functions                                */
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Helper function to reset the a particular row in concurrency manager table 
 
  @param rfm_dev
  current logical device in use 

  @return
  TRUE if update is successful, else return FALSE 

*/
boolean rfcmn_reset_rx_concurrency_tbl_entry
(
  rfm_device_enum_type rfm_dev
)
{
  RFM_CM_DEBUG_EN_Q(&rfcmn_reset_rx_concurrency_tbl_entry, rfm_dev, 0, 0, 0, 0);

  rfcmn_concurrency_db.curr_state[rfm_dev].logical_dev = rfm_dev; 
  rfcmn_concurrency_db.curr_state[rfm_dev].curr_mode   = RFCOM_NUM_MODES; 
  rfcmn_concurrency_db.curr_state[rfm_dev].curr_band   = 0xFF; 
  rfcmn_concurrency_db.curr_state[rfm_dev].alt_path    = 0;
  rfcmn_concurrency_db.curr_state[rfm_dev].wb_index    = 0;
  return TRUE; 

} /* end of rfcmn_reset_concurrency_tbl_entry() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to turn on/off the debug modes
 
  @details
  Function is called to set up the debug modes (time profiling, etc)
 
  @return
  Boolean indicating success/failure

*/
boolean rfcmn_concurrency_mgr_set_debug_mode
(
  uint32 debug_mask,
  boolean debug_en
)
{
  if (debug_en)
  {
    RF_MSG(RF_MED, "Setting T_PROF true");
    rf_ccry_mgr_time_prof = TRUE;
  }
  else
  {
    RF_MSG(RF_MED, "Setting T_PROF false");
    rf_ccry_mgr_time_prof = FALSE;
  }

  RF_MSG_1(RF_MED,"rf_ccry_mgr_time_prof is set to - %d", rf_ccry_mgr_time_prof);
  return TRUE;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif