/******************************************************************************
  @file    ps_sys_conf.c
  @brief   

  DESCRIPTION
  This file defines functions, enums and structs used to read and update
  technology configurations

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2011-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ps_sys_conf.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $ 

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/12    msh    Added changes for PS_SYS_CONF_3GPP2_AN_AUTH_STATUS  
05/31/11   bvd     Created module
===========================================================================*/

/*---------------------------------------------------------------------------
                           INCLUDE FILES
---------------------------------------------------------------------------*/

#include "ps_sys_confi.h"
#include "ps_sys_eventi.h"
#include "ps_system_heap.h"
#include "dserrno.h"
#include "ds_Utils_DebugMsg.h"
#include <stringl/stringl.h>
#include "ps_crit_sect.h"
#include "ps_sys_fast_dormancy.h"
#include "ps_sys_conf_parser.h"

extern ps_crit_sect_type ps_sys_crit_section;

/*Cache for all technology configuration data*/
static ps_sys_confi_tech_all   conf_tech_all[PS_SYS_SUBS_MAX-1];

/*Cache for 3gpp2 configuration data*/
static ps_sys_confi_tech_3gpp2 conf_tech_3gpp2[PS_SYS_SUBS_MAX-1];

/*Cache for 3gpp configuration data*/
static ps_sys_confi_tech_3gpp  conf_tech_3gpp[PS_SYS_SUBS_MAX-1];

/*Cache for wlan configuration data*/
static ps_sys_confi_tech_wlan  conf_tech_wlan[PS_SYS_SUBS_MAX-1]; 

/**
  @brief  This function calculates the size of system status API values 
          based on number of available systems and number of available 
          apns.

  @param[in] argptr      Pointer to system_status_ex structure   
   
  @return  size          Returns the size of system status.    
*/
static uint32 ps_sys_conf_size_sys_status_ex
(
  ps_sys_system_status_ex_type     * argval_ptr
)
{
  uint32                             size          = 0;
  uint32                             apn_size      = 0;
  uint32                             cnt_apns      = 0;
  ps_sys_system_status_ex_type     * sys_status_ex = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("ps_sys_conf_size_sys_status_ex(): argval_ptr 0x%p",
                           argval_ptr);
                          
  sys_status_ex = (ps_sys_system_status_ex_type *)argval_ptr;
  
  /* calulate the size of the apn */
  for (cnt_apns = 0; cnt_apns < sys_status_ex->num_avail_apns; cnt_apns++)
  {
    apn_size += 
      sizeof(sys_status_ex->apn_pref_sys_info[cnt_apns].num_avail_sys) +  
      (sizeof(char)*PS_SYS_MAX_APN_LEN) + \
      (sizeof(ps_sys_system_status_info_type) * \
      sys_status_ex->apn_pref_sys_info[cnt_apns].num_avail_sys);

    apn_size += 
      sizeof(sys_status_ex->apn_pref_sys_info[cnt_apns].recommended_pref_system_index);
  }
	
  /* calculate the total size of the structure */
  size = sizeof(sys_status_ex->num_avail_sys) + \
         sizeof(sys_status_ex->num_avail_apns) + \
         (sizeof(ps_sys_system_status_info_type) * \
         sys_status_ex->num_avail_sys) + \
         sizeof(sys_status_ex->recommended_pref_system_index) + \
         (apn_size) ;

  return(size);
}

/**
  @brief  This function compares the data from cache and 
          system_status_ex structure.

  @param[in] dest        Pointer to the system_status_ex in cache 
  @param[in] source      Pointer to system_status_ex structure  
   
  @return  0             If both the data matches.
  @return  -1            On mismatch.    
*/ 
static int ps_sys_conf_comp_sys_status_ex_val
(
  void                             * dest, 
  ps_sys_system_status_ex_type     * source
)
{
  uint32                             count   = 0;
  uint32                             index   = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("ps_sys_conf_comp_sys_status_ex_val(): source 0x%p"
                           "dest 0x%x", source, dest);
                         
  if ((NULL == source) || (NULL == dest))
  {
    LOG_MSG_INFO1_0("ps_sys_conf_comp_sys_status_ex_val()" 
                    "source/dest ptr is NULL");
    return -1;              
  }
  
  /* Compare the number of available sys */
  if (0 != memcmp(&(source->num_avail_sys), 
  	          dest, 
 	          sizeof(source->num_avail_sys)))
  {
    LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() num of apns : "
                    "source %d dest %d", source->num_avail_sys, *(char*)dest);
    return -1;
  }
  /* Advance the destination ptr by the size of num_avail_sys */
  dest = (char*)dest + sizeof(source->num_avail_sys);
  
  /* Compare the technology, rat value, so mask for available sys */
  for (count = 0; count < source->num_avail_sys; count++)
  {
    if (0 != memcmp(&(source->avail_sys[count].technology), 
                    dest, 
                    sizeof(source->avail_sys[count].technology)))
    {
      LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() technology : " 
                      "source %d dest %d",
                      source->avail_sys[count].technology, *(char*)dest);
      return -1;
    }
    dest = (char*)dest + sizeof(source->avail_sys[count].technology);

    if (0 != memcmp(&(source->avail_sys[count].rat_value), 
                    dest, 
                    sizeof(source->avail_sys[count].rat_value)))
    {
      LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() rat_value : " 
                      "source %d dest %d",
                      source->avail_sys[count].rat_value, *(char*)dest);
      return -1;
    }
    dest = (char*)dest + sizeof(source->avail_sys[count].rat_value);

    if (0 != memcmp(&(source->avail_sys[count].so_mask), 
                    dest, 
                    sizeof(source->avail_sys[count].so_mask)))
    {
      LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() so_mask : " 
                      "source %d dest %d",
                      source->avail_sys[count].so_mask, *(char*)dest);
      return -1;
    }
    dest = (char*)dest + sizeof(source->avail_sys[count].so_mask);
  }
  
  /* Compare the number of available apns */
  if (0 != memcmp(&(source->num_avail_apns), 
  	          dest, 
  	          sizeof(source->num_avail_apns)))
  {
    LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() Num avail apns : " 
                    "source %d dest %d", source->num_avail_apns, *(char*)dest);
    return -1;
  }
  dest = (char*)dest + sizeof(source->num_avail_apns);
  
  /* Compare the apn name, no of sys, technology, rat mask, 
  so mask for each apn */
  for (count = 0; count < source->num_avail_apns; count++)
  {
    if (0 != memcmp(source->apn_pref_sys_info[count].apn_name, 
                    dest, 
                    PS_SYS_MAX_APN_LEN))
    {
      LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() Apn Name : " 
                      "source %s dest %s",
                      source->apn_pref_sys_info[count].apn_name, (char*)dest);
      return -1;
    }
    dest = (char*)dest + PS_SYS_MAX_APN_LEN;
    
    if (0 != memcmp(&(source->apn_pref_sys_info[count].num_avail_sys), 
                    dest, 
                    sizeof(source->apn_pref_sys_info[count].num_avail_sys)))
    {
      LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() Num avail sys : " 
                      "source %d dest %d",
                      source->apn_pref_sys_info[count].num_avail_sys,
                      *(char*)dest);
      return -1;
    }
    dest = (char*)dest + 
           sizeof(source->apn_pref_sys_info[count].num_avail_sys);
    
    for (index = 0; 
         index < source->apn_pref_sys_info[count].num_avail_sys; 
         index++)
    {					
      if (0 != memcmp(&(source->apn_pref_sys_info[count].avail_sys[index].technology), 
                      dest, 
                      sizeof((source->apn_pref_sys_info[count].avail_sys[index].technology))))
      {
        LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() Technology : " 
                        "source %d dest %d",
                        source->apn_pref_sys_info[count].avail_sys[index].technology,
                        *(char*)dest);
        return -1;
      }
      dest = (char*)dest + \
             sizeof((source->apn_pref_sys_info[count].avail_sys[index].\
               technology));
      
      if (0 != memcmp(&(source->apn_pref_sys_info[count].\
                        avail_sys[index].rat_value), 
                      dest, 
                      sizeof(source->apn_pref_sys_info[count].\
                        avail_sys[index].rat_value)))
      {
        LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() Rat value : " 
                        "source %d dest %d",
                        source->apn_pref_sys_info[count].avail_sys[index].rat_value,
                        *(char*)dest);
        return -1;
      }
      dest = (char*)dest + 
             sizeof(source->apn_pref_sys_info[count].avail_sys[index].\
               rat_value);  
      
      if (0 != memcmp(&(source->apn_pref_sys_info[count].\
                        avail_sys[index].so_mask), 
                      dest, 
                      sizeof((source->apn_pref_sys_info[count].\
                        avail_sys[index].so_mask))))
      {
        LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() So mask : " 
                        "source %d dest %d",
                        source->apn_pref_sys_info[count].avail_sys[index].so_mask,
                        *(char*)dest);
        return -1;
      }
      dest = (char*)dest + 
             sizeof(source->apn_pref_sys_info[count].avail_sys[index].\
                    so_mask);		
    }

    if (0 != memcmp(&(source->apn_pref_sys_info[count].\
                      recommended_pref_system_index), 
                      dest, 
                      sizeof(source->apn_pref_sys_info[count].\
                      recommended_pref_system_index)))
      {
        LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() recommended_pref_system_index : " 
                        "source %d dest %d",
                        source->apn_pref_sys_info[count].\
                        recommended_pref_system_index,
                        *(char*)dest);
        return -1;
      }
      dest = (char*)dest + 
             sizeof(source->apn_pref_sys_info[count].\
                     recommended_pref_system_index);

  }	
  
  /* Compare the Global Recommended System*/
  if (0 != memcmp(&(source->recommended_pref_system_index), 
                  dest, 
                  sizeof(source->recommended_pref_system_index)))
  {
    LOG_MSG_INFO1_2("ps_sys_conf_comp_sys_status_ex_val() Global Recommended pref system: "
                    "source %d dest %d", source->recommended_pref_system_index, *(char*)dest);
    return -1;
  }	
  dest = (char*)dest + 
         sizeof(source->recommended_pref_system_index);

  return 0;
}

/**
  @brief  This function updates the cache from system_status_ex 
          structure.

  @param[in] dest          Pointer to the system_status_ex in cache
  @param[in] source        Pointer to system_status_ex structure  
   
  @return 
    None  
  
*/
static void ps_sys_conf_update_sys_status_ex_cache
(
  void                             * dest, 
  ps_sys_system_status_ex_type     * source
)
{
  uint32                             count = 0;
  uint32                             index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("ps_sys_conf_update_sys_status_ex_cache(): "
                           "source 0x%p dest 0x%x", source, dest);
  
  if ((NULL == source) || (NULL == dest))
  {
    LOG_MSG_INFO1_0("ps_sys_conf_update_sys_status_ex_cache()" 
                    "source/dest ptr is NULL");
    return;              
  }
                         
  /* copy the num avail sys to the cache */                        
  memscpy(dest, sizeof(source->num_avail_sys), 
          &(source->num_avail_sys), sizeof(source->num_avail_sys));
  dest = (char*)dest + sizeof(source->num_avail_sys);
  
  /* copy the technology, rat value, so mask to the cache */ 
  for (count = 0; count < source->num_avail_sys; count++)
  {
    memscpy(dest, 
            sizeof(source->avail_sys[count].technology),
           &(source->avail_sys[count].technology), 
           sizeof(source->avail_sys[count].technology));
    dest = (char*)dest + sizeof(source->avail_sys[count].technology); 
    
    memscpy(dest, 
            sizeof(source->avail_sys[count].rat_value),
           &(source->avail_sys[count].rat_value), 
           sizeof(source->avail_sys[count].rat_value));
    dest = (char*)dest + sizeof(source->avail_sys[count].rat_value);
    
    memscpy(dest, 
            sizeof(source->avail_sys[count].so_mask),
           &(source->avail_sys[count].so_mask), 
           sizeof(source->avail_sys[count].so_mask));
    dest = (char*)dest + sizeof(source->avail_sys[count].so_mask);
  }
	
  /* copy the no of apns to the cache */
  memscpy(dest, 
          sizeof(source->num_avail_apns),
  	 &(source->num_avail_apns), 
  	 sizeof(source->num_avail_apns));
  dest = (char*)dest + sizeof(source->num_avail_apns);
  
  /* copy the apn name, no of sys, technology, rat, so mask for each apn */  
  for (count = 0; count < source->num_avail_apns; count++)
  {
    memscpy(dest,
            PS_SYS_MAX_APN_LEN, 
           source->apn_pref_sys_info[count].apn_name, 
           PS_SYS_MAX_APN_LEN);
    dest = (char*)dest + PS_SYS_MAX_APN_LEN;
    
    memscpy(dest, 
            sizeof(source->apn_pref_sys_info[count].num_avail_sys),
           &(source->apn_pref_sys_info[count].num_avail_sys), 
           sizeof(source->apn_pref_sys_info[count].num_avail_sys));
    dest = (char*)dest + 
           sizeof(source->apn_pref_sys_info[count].num_avail_sys);
				
    for (index = 0; 
         index < source->apn_pref_sys_info[count].num_avail_sys; 
         index++)
    {
      memscpy(dest, 
              sizeof(source->apn_pref_sys_info[count].\
               avail_sys[index].technology),
             &(source->apn_pref_sys_info[count].avail_sys[index].technology), 
             sizeof(source->apn_pref_sys_info[count].\
               avail_sys[index].technology));
      dest = (char*)dest + 
             sizeof(source->apn_pref_sys_info[count].avail_sys[index].\
                    technology);
      
      memscpy(dest, 
              sizeof(source->apn_pref_sys_info[count].\
               avail_sys[index].rat_value),
             &(source->apn_pref_sys_info[count].avail_sys[index].rat_value), 
             sizeof(source->apn_pref_sys_info[count].\
               avail_sys[index].rat_value));
      dest = (char*)dest + 
             sizeof(source->apn_pref_sys_info[count].avail_sys[index].\
                    rat_value);
      
      memscpy(dest,
              sizeof(source->apn_pref_sys_info[count].avail_sys[index].\
               so_mask), 
             &(source->apn_pref_sys_info[count].avail_sys[index].so_mask), 
             sizeof(source->apn_pref_sys_info[count].avail_sys[index].\
               so_mask));
      dest = (char*)dest + 
             sizeof(source->apn_pref_sys_info[count].avail_sys[index].so_mask);
    }

    //Copy recommended pref index
    memscpy(dest,
            sizeof(source->apn_pref_sys_info[count].\
             recommended_pref_system_index),
            &source->apn_pref_sys_info[count].\
              recommended_pref_system_index,
            sizeof(source->apn_pref_sys_info[count].\
             recommended_pref_system_index));

    dest = (char*)dest + 
             sizeof(source->apn_pref_sys_info[count].recommended_pref_system_index);
            
  }
  
  /*Copy global recommended pref system*/
  memscpy(dest,
          sizeof(source->recommended_pref_system_index),
          &(source->recommended_pref_system_index),
          sizeof(source->recommended_pref_system_index));
  dest = (char*)dest + sizeof(source->recommended_pref_system_index);
}

/**
  @brief  This function updates the system_status_ex structure
          from the cache.

  @param[in] dest          Pointer to system_status_ex structure
  @param[in] source        Pointer to the system_status_ex in cache  
   
  @return 
    None

*/
static void ps_sys_conf_update_from_sys_status_ex_cache
(
  ps_sys_system_status_ex_type     * dest, 
  void                             * source
)
{
  uint32                             count = 0;
  uint32                             index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("ps_sys_conf_update_from_sys_status_ex_cache():" 
                           "source 0x%p dest 0x%x", source, dest);
                         
  if ((NULL == source) || (NULL == dest))
  {
    LOG_MSG_INFO1_0("ps_sys_conf_update_from_sys_status_ex_cache()" 
                    "source/dest ptr is NULL");
    return;              
  }
  
  /* Copy num avail sys from the cache */                       
  memscpy(&(dest->num_avail_sys), sizeof(dest->num_avail_sys),
          source, sizeof(dest->num_avail_sys));
  source = (char*)source + sizeof(dest->num_avail_sys);
  
  /* Copy the technology, rat value and so mask for each sys from the cache */  
  for (count = 0; count < dest->num_avail_sys; count++)
  {
    memscpy(&(dest->avail_sys[count].technology),
            sizeof(dest->avail_sys[count].technology), 
           source, 
           sizeof(dest->avail_sys[count].technology));
    source = (char*)source + sizeof(dest->avail_sys[count].technology);
    
    memscpy(&(dest->avail_sys[count].rat_value), 
            sizeof(dest->avail_sys[count].rat_value),
           source, 
           sizeof(dest->avail_sys[count].rat_value));
    source = (char*)source + sizeof(dest->avail_sys[count].rat_value); 
    
    memscpy(&(dest->avail_sys[count].so_mask), 
            sizeof(dest->avail_sys[count].so_mask),
           source, 
           sizeof(dest->avail_sys[count].so_mask));
    source = (char*)source + sizeof(dest->avail_sys[count].so_mask);
  }
	
  /* Copy the num avail apns from the cache */
  memscpy(&(dest->num_avail_apns), sizeof(dest->num_avail_apns),
          source, sizeof(dest->num_avail_apns));
  source = (char*)source + sizeof(dest->num_avail_apns);
  
  /* copy the apn name, no of sys, technology, rat, so mask for each apn */ 
  for (count = 0; count < dest->num_avail_apns; count++)
  {
    memscpy(dest->apn_pref_sys_info[count].apn_name,
            PS_SYS_MAX_APN_LEN, 
           source, 
           PS_SYS_MAX_APN_LEN);
    source = (char*)source + PS_SYS_MAX_APN_LEN;
    
    memscpy(&(dest->apn_pref_sys_info[count].num_avail_sys), 
            sizeof(dest->apn_pref_sys_info[count].num_avail_sys),
           source, 
           sizeof(dest->apn_pref_sys_info[count].num_avail_sys));
    source = (char*)source + \
             sizeof(dest->apn_pref_sys_info[count].num_avail_sys);
				
    for (index = 0; 
         index < dest->apn_pref_sys_info[count].num_avail_sys; 
         index++)
    {
      memscpy(&(dest->apn_pref_sys_info[count].avail_sys[index].technology), 
              sizeof(dest->apn_pref_sys_info[count].\
               avail_sys[index].technology),
             source, 
             sizeof(dest->apn_pref_sys_info[count].\
               avail_sys[index].technology));
      source = (char*)source + \
               sizeof(dest->apn_pref_sys_info[count].avail_sys[index].\
                      technology);
        
      memscpy(&(dest->apn_pref_sys_info[count].avail_sys[index].rat_value),
              sizeof(dest->apn_pref_sys_info[count].\
               avail_sys[index].rat_value), 
             source, 
             sizeof(dest->apn_pref_sys_info[count].\
               avail_sys[index].rat_value));
      source = (char*)source + \
               sizeof(dest->apn_pref_sys_info[count].avail_sys[index].\
                      rat_value);
        
      memscpy(&(dest->apn_pref_sys_info[count].avail_sys[index].so_mask), 
              sizeof(dest->apn_pref_sys_info[count].avail_sys[index].so_mask),
             source, 
             sizeof(dest->apn_pref_sys_info[count].avail_sys[index].so_mask));
      source = (char*)source + 
               sizeof(dest->apn_pref_sys_info[count].avail_sys[index].so_mask);
    }

    memscpy(&(dest->apn_pref_sys_info[count].recommended_pref_system_index),
             sizeof(dest->apn_pref_sys_info[count].\
                     recommended_pref_system_index),
             source,
             sizeof(dest->apn_pref_sys_info[count].\
                     recommended_pref_system_index));
    source = (char*)source + 
             sizeof(dest->apn_pref_sys_info[count].\
                     recommended_pref_system_index);

  }
  
  memscpy(&(dest->recommended_pref_system_index),
          sizeof(dest->recommended_pref_system_index),
          source,
          sizeof(dest->recommended_pref_system_index));
  source = (char*)source + sizeof(dest->recommended_pref_system_index);
          
}	


int16 ps_sys_conf_get
(
  ps_sys_tech_enum_type              tech_type,
  ps_sys_conf_enum_type              conf_name,
  void                             * argval_ptr,
  int16                            * ps_errno
)
{
  return ps_sys_conf_get_ex(tech_type,
                            conf_name,
                            PS_SYS_PRIMARY_SUBS,
                            argval_ptr,
                            ps_errno);
}/* ps_sys_conf_get() */

int16 ps_sys_conf_get_ex
(
  ps_sys_tech_enum_type              tech_type,
  ps_sys_conf_enum_type              conf_name,
  ps_sys_subscription_enum_type      subscription_id,
  void                             * argval_ptr,
  int16                            * ps_errno
)
{
  int ret_val;
  uint32       cnt_pdn;
  uint32       cnt_network;
  uint32       num_bytes = 0;
  
  /* Configuration specific structures */
  ps_sys_system_status_type            * system_status_conf_val;
  ps_sys_3gpp2_s101_status_change_type * s101_status_change_info_type;
  ps_sys_3gpp2_epzid_info_type         * epzid_info_type;
  ps_sys_conf_3gpp2_an_auth_status_enum_type * an_auth_status_ptr;
  ps_sys_conf_enable_loopback_type     * enable_loopback_ptr;
  ps_sys_conf_get_dun_ctrl_pref_type   * dun_ctrl_pref_ptr;
  ps_sys_wlan_status_type              * wlan_status;
  ps_sys_wlan_preference_enum_type     * wlan_preference;
  ps_sys_system_status_ex_type         * system_status_ex_ptr;
  ps_sys_apn_pref_sys_type             * apn_pref_sys_ptr;
  ps_sys_wlan_mac_addr_type            * wlan_mac_addr_ptr;
  ps_sys_wlan_ssid_type                * wlan_ssid_ptr;
  ps_sys_wlan_max_conn_info_type       * wlan_max_conn_ptr = NULL;
  ps_sys_conf_throughput_timer_type    * throughput_timer;
  ps_sys_throughput_info_list_type     * throughput_info_ptr;
  ps_sys_roaming_disallowed_info_type  * roaming_disallowed_ptr = NULL;
  ps_sys_null_resolved_apn_info_type   * null_resolved_apn_ptr = NULL;
  ps_sys_3gpp2_network_qos_aware_status * net_aware_ptr = NULL;
  ps_sys_report_dl_timer_type           * dl_thrpt_timer_ptr = NULL;
  ps_sys_dl_throughput_info_type        * dl_thrpt_info_ptr = NULL;
  ps_sys_report_dl_timer_type           * min_dl_thrpt_timer_ptr = NULL;
  ps_sys_thermal_mitigation_action_enum_type *thermal_info_ptr = NULL;
  ps_sys_data_services_setting_type    * data_setting_ptr = NULL;
  ps_sys_data_services_roaming_setting_type * data_roaming_ptr = NULL;
  ps_sys_apn_info_type                 * apn_info_ptr = NULL;
  ps_sys_modem_power_stats_config_type  * power_stats_config_ptr = NULL;
  ps_sys_modem_power_stats_info_type   * power_stats_info_ptr = NULL;
  ps_sys_data_services_wifi_setting_type  * wifi_settings_ptr = NULL;
  ps_sys_remote_socket_hdlr_avail_type     * hdlr_avail = NULL;
  ps_sys_roaming_status_info_type       * roaming_status_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  LOG_MSG_FUNCTION_ENTRY_3("ps_sys_conf_get_ex(): tech %d conf %d "
                           "subscription_id %d",
                           tech_type, conf_name, subscription_id);

  if (ps_errno == NULL)
  {
    LOG_MSG_INVALID_INPUT_0("ps_sys_conf_get_ex(): Invalid argument ps_errno");
    return -1;
  }

  *ps_errno = 0;  /* Set it to success */

  ret_val = ps_sys_confi_verify_tech_conf_name(tech_type, conf_name);
  if (0 > ret_val || argval_ptr == NULL)
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_ERROR_0("ps_sys_conf_get_ex(): Invalid argument passed");
    return -1;
  }
  
  if (subscription_id >= PS_SYS_SUBS_MAX || 
      subscription_id < PS_SYS_DEFAULT_SUBS)
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_1("ps_sys_conf_get_ex(): Invalid subscription_id %d",
                            subscription_id);
    return -1;
  }
 
  if (PS_SYS_DEFAULT_SUBS == subscription_id )
  {
    subscription_id = ps_sys_get_default_data_subscription();
  }

  /*-------------------------------------------------------------------------
    Add code to Implement different configuration and remove all
    temp vars and code once xml is implemented
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&ps_sys_crit_section);
  switch(conf_name)
  {
    case PS_SYS_CONF_SYSTEM_STATUS: 
    { 
      system_status_conf_val  = (ps_sys_system_status_type *)argval_ptr;
      memscpy(system_status_conf_val,
              sizeof(ps_sys_system_status_type),
             conf_tech_all[subscription_id - 1].system_status_type,
             sizeof(ps_sys_system_status_type));
             
      LOG_MSG_INFO1_3("ps_sys_conf_get_ex : pref network %d, RAT mask 0x%x"
                      "so mask 0x%x",
                      system_status_conf_val->preferred_network,
                      system_status_conf_val->network_info[system_status_conf_val->preferred_network].rat_mask,
                      system_status_conf_val->network_info[system_status_conf_val->preferred_network].so_mask);

      break;
    }
   
    case PS_SYS_CONF_SYSTEM_STATUS_EX: 
    { 
      system_status_ex_ptr  = (ps_sys_system_status_ex_type *)argval_ptr;
      
      /* If the cache is NULL, set apn and sys to zero. If not then load 
      the data from cache */
      if (NULL == conf_tech_all[subscription_id - 1].system_status_ex_ptr)
      {
        LOG_MSG_INFO1_0("ps_sys_conf_get_ex() "
                        "sys status ex cache ptr is NULL");
        system_status_ex_ptr->num_avail_apns = 0;
        system_status_ex_ptr->num_avail_sys  = 0;
      }
      else
      {
        ps_sys_conf_update_from_sys_status_ex_cache
        (
          system_status_ex_ptr, 
          conf_tech_all[subscription_id - 1].system_status_ex_ptr
        );
      }
             
      LOG_MSG_INFO1_2("ps_sys_conf_get_ex ,"
                      "num of rats %d num of pdns %d",
                      system_status_ex_ptr->num_avail_sys,
                      system_status_ex_ptr->num_avail_apns);

      LOG_MSG_INFO1_0("ps_sys_conf_get_ex : First tupple of technology, RAT"
                      " and SO is preferred system"); 
                    
      for (cnt_network=0; cnt_network < system_status_ex_ptr->num_avail_sys; 
           cnt_network++)
      {
        LOG_MSG_INFO1_3("ps_sys_conf_get_ex : "
                        "technology %d rat value %d so mask 0x%x",
                        system_status_ex_ptr->avail_sys[cnt_network].technology,
                        system_status_ex_ptr->avail_sys[cnt_network].rat_value,
                        system_status_ex_ptr->avail_sys[cnt_network].so_mask);
      }
      
      for (cnt_pdn=0; cnt_pdn < system_status_ex_ptr->num_avail_apns; 
           cnt_pdn++)
      {
        LOG_MSG_INFO1_2("ps_sys_conf_get_ex : "
                        "pdn name %s available sys for pdn %d",
                        system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].apn_name,
                        system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].num_avail_sys);

        if(system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].num_avail_sys > 0)
        {
          LOG_MSG_INFO1_3("ps_sys_conf_get_ex pref tech %d "
                          "pref rat %d pref so mask 0x%x",
                          system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].avail_sys[0].technology,
                          system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].avail_sys[0].rat_value,
                          system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].avail_sys[0].so_mask);
        }                   
      }
      
      break;
      }             

    case PS_SYS_CONF_APN_PREF_SYS:
    {               
      apn_pref_sys_ptr  = (ps_sys_apn_pref_sys_type *)argval_ptr;
      memscpy(apn_pref_sys_ptr,
              sizeof(ps_sys_apn_pref_sys_type),
             conf_tech_all[subscription_id - 1].apn_pref_sys_ptr,
             sizeof(ps_sys_apn_pref_sys_type));
      
      LOG_MSG_INFO1_2("ps_sys_conf_get_ex : PS_SYS_CONF_APN_PREF_SYS "
                      "pdn name %s tech pref %d",
                      apn_pref_sys_ptr->apn, apn_pref_sys_ptr->pref_sys);
      break;
    }
   
    case PS_SYS_CONF_3GPP2_S101_STATUS_CHANGE:
    {
      s101_status_change_info_type  = 
        (ps_sys_3gpp2_s101_status_change_type *)argval_ptr;
      memscpy(s101_status_change_info_type,
              sizeof(ps_sys_3gpp2_s101_status_change_type),
             conf_tech_3gpp2[subscription_id - 1].\
               s101_status_change_type,
             sizeof(ps_sys_3gpp2_s101_status_change_type));
      break;
    }

    case PS_SYS_CONF_3GPP2_EPZID_INFO: 
    { 
      epzid_info_type  = (ps_sys_3gpp2_epzid_info_type *)argval_ptr;
      memscpy(epzid_info_type,
              sizeof(ps_sys_3gpp2_epzid_info_type),
             conf_tech_3gpp2[subscription_id - 1].epzid_info_type,
             sizeof(ps_sys_3gpp2_epzid_info_type));
      break;
    }

    case PS_SYS_CONF_3GPP2_AN_AUTH_STATUS: 
    { 
      an_auth_status_ptr = 
        (ps_sys_conf_3gpp2_an_auth_status_enum_type*)argval_ptr;

      memscpy(an_auth_status_ptr,
              sizeof(ps_sys_conf_3gpp2_an_auth_status_enum_type),
             conf_tech_3gpp2[subscription_id - 1].an_auth_status,
             sizeof(ps_sys_conf_3gpp2_an_auth_status_enum_type));
      break;
    }

    case PS_SYS_CONF_ENABLE_LOOPBACK: 
    { 
      enable_loopback_ptr  = (ps_sys_conf_enable_loopback_type *)argval_ptr;
      memscpy(enable_loopback_ptr,
              sizeof(ps_sys_conf_enable_loopback_type),
             conf_tech_all[subscription_id - 1].\
               enable_loopback_ptr,
             sizeof(ps_sys_conf_enable_loopback_type));
      break;
    }

    case PS_SYS_CONF_DUN_CTRL_PREF:
    {
      dun_ctrl_pref_ptr  = (ps_sys_conf_get_dun_ctrl_pref_type *)argval_ptr;
      memscpy(dun_ctrl_pref_ptr,
              sizeof(ps_sys_conf_get_dun_ctrl_pref_type),
             conf_tech_all[subscription_id - 1].\
               get_dun_ctrl_pref_ptr,
             sizeof(ps_sys_conf_get_dun_ctrl_pref_type));
      break;
    }

    case PS_SYS_CONF_3GPP_LTE_ATTACH_TYPE: 
    { 
      memscpy((ps_sys_lte_attach_enum_type*)argval_ptr,
              sizeof(ps_sys_lte_attach_enum_type),
             conf_tech_3gpp[subscription_id - 1].lte_attach_type,
             sizeof(ps_sys_lte_attach_enum_type));
      break;
    }

    case PS_SYS_CONF_3GPP_EMERGENCY_PDN_STATUS:
    {
      memscpy((ps_sys_is_pdn_only_emergency_type*)argval_ptr,
              sizeof(ps_sys_is_pdn_only_emergency_type),
             conf_tech_3gpp[subscription_id - 1].only_emergency_pdn_info,
             sizeof(ps_sys_is_pdn_only_emergency_type));
      break;
    }

    case PS_SYS_CONF_3GPP_ROAMING_DISALLOWED_INFO: 
    { 
      roaming_disallowed_ptr = (ps_sys_roaming_disallowed_info_type *)argval_ptr;
      /*If the cache is empty, the default value is returned*/
      if (NULL == conf_tech_3gpp[subscription_id - 1].\
          roaming_disallowed_info_ptr)
      {
        roaming_disallowed_ptr->is_roaming_disallowed = FALSE;
        roaming_disallowed_ptr->num_apn_names         = 0;        
      }
      else
      {
        /*Calculate the size stored in cache*/
        roaming_disallowed_ptr = conf_tech_3gpp[subscription_id - 1].\
                                  roaming_disallowed_info_ptr;
        num_bytes = sizeof(roaming_disallowed_ptr->is_roaming_disallowed) +
                    sizeof(roaming_disallowed_ptr->num_apn_names) + 
                    roaming_disallowed_ptr->num_apn_names * 
                    sizeof(ps_sys_apn_name_type);

        /*copy the value from the cache to the user buffer*/
        memscpy((ps_sys_roaming_disallowed_info_type*)argval_ptr,
                sizeof(ps_sys_roaming_disallowed_info_type),
                conf_tech_3gpp[subscription_id - 1].roaming_disallowed_info_ptr,
                num_bytes);
      }

      LOG_MSG_INFO1_2("ps_sys_conf_get_ex : Get roaming disallowed info:"
                      "is_roaming_disallowed %d, num_apn_names %d",
                       roaming_disallowed_ptr->is_roaming_disallowed,
                       roaming_disallowed_ptr->num_apn_names); 
      for(cnt_pdn = 0; cnt_pdn < roaming_disallowed_ptr->num_apn_names; cnt_pdn++)
      {
        LOG_MSG_INFO1_2("ps_sys_conf_get_ex :"
                        "apn[%d]: %s",
                        cnt_pdn,
                        roaming_disallowed_ptr->apn_list[cnt_pdn].apn_name);
      }
      break;
    }

    case PS_SYS_CONF_WLAN_STATUS: 
    { 
      wlan_status  = (ps_sys_wlan_status_type *)argval_ptr;
      memscpy(wlan_status,
              sizeof(ps_sys_wlan_status_type),
             conf_tech_wlan[subscription_id - 1].wlan_status,
             sizeof(ps_sys_wlan_status_type));

      LOG_MSG_INFO2_3("ps_sys_conf_get_ex : is_wlan_available %d, "
                      "ipv6_prefix_len %d mac address %d",
                      wlan_status->is_wlan_available,
                      wlan_status->ipv6_prefix_len,
                      wlan_status->ap_mac_addr);

      LOG_MSG_INFO2_1("ps_sys_conf_get_ex : wqe_status %d ",
                      wlan_status->wqe_status);
  
      IPV4_ADDR_MSG(wlan_status->ipv4_addr.addr.v4.ps_s_addr);  
        
      IPV6_ADDR_MSG(wlan_status->ipv4_addr.addr.v6.ps_s6_addr64);
                     
      break;
    }
    
    case PS_SYS_CONF_GET_WLAN_MAC_ADDR: 
    { 
      wlan_mac_addr_ptr  = (ps_sys_wlan_mac_addr_type *)argval_ptr;  
      memscpy(wlan_mac_addr_ptr,
              sizeof(ps_sys_wlan_mac_addr_type),
             conf_tech_wlan[subscription_id - 1]. \
               wlan_mac_addr_ptr,
             sizeof(ps_sys_wlan_mac_addr_type));       
             
      LOG_MSG_INFO2_6("ps_sys_conf_get_ex : Mac adress ret 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x ",
                      wlan_mac_addr_ptr->ap_mac_addr[0],
                      wlan_mac_addr_ptr->ap_mac_addr[1],
                      wlan_mac_addr_ptr->ap_mac_addr[2],
                      wlan_mac_addr_ptr->ap_mac_addr[3],
                      wlan_mac_addr_ptr->ap_mac_addr[4],
                      wlan_mac_addr_ptr->ap_mac_addr[5]);
      break;
    }

     case PS_SYS_CONF_GET_WLAN_SSID: 
     { 
       wlan_ssid_ptr  = (ps_sys_wlan_ssid_type *)argval_ptr;  
       memscpy(wlan_ssid_ptr,
               sizeof(ps_sys_wlan_ssid_type),
               conf_tech_wlan[subscription_id - 1]. \
                 wlan_ssid_ptr,
               sizeof(ps_sys_wlan_ssid_type));       

       LOG_MSG_INFO2_1("ps_sys_conf_get_ex : ssid ret %s ",
                       wlan_ssid_ptr->ssid);
       break;
     }

    case PS_SYS_CONF_WLAN_PREFERENCE: 
    { 
      wlan_preference  = (ps_sys_wlan_preference_enum_type *)argval_ptr;
      memscpy(wlan_preference,
              sizeof(ps_sys_wlan_preference_enum_type),
             conf_tech_wlan[subscription_id - 1].wlan_preference,
             sizeof(ps_sys_wlan_preference_enum_type));

      LOG_MSG_INFO2_1("ps_sys_conf_get_ex : wlan pref %d ", wlan_preference);
                     
      break;
    }

    case PS_SYS_CONF_WLAN_MAX_CONN:
    {
      wlan_max_conn_ptr = (ps_sys_wlan_max_conn_info_type *)argval_ptr;
      memscpy(wlan_max_conn_ptr,
              sizeof(ps_sys_wlan_max_conn_info_type),
              conf_tech_wlan[subscription_id - 1].wlan_max_conn_ptr,
              sizeof(ps_sys_wlan_max_conn_info_type));

      LOG_MSG_INFO1_2("ps_sys_conf_get_ex : wlan max conn reached %d "
                      "no of active apns %d",
                      wlan_max_conn_ptr->is_max_conn_reached, 
                      wlan_max_conn_ptr->num_apns);
      break;
    }

    case PS_SYS_CONF_THROUGHPUT_INFO_TIMER:
    {
      throughput_timer = (ps_sys_conf_throughput_timer_type *)argval_ptr;
      memscpy(throughput_timer,
              sizeof(ps_sys_conf_throughput_timer_type),
              conf_tech_all[subscription_id - 1].throughput_timer_ptr,
              sizeof(ps_sys_conf_throughput_timer_type));

      LOG_MSG_INFO2_1("ps_sys_conf_get_ex :throughput timer val %d ",    
                      throughput_timer); 

      break;
    }

    case PS_SYS_CONF_THROUGHPUT_INFO:
    {
      throughput_info_ptr = (ps_sys_throughput_info_list_type *)argval_ptr;
      if(conf_tech_all[subscription_id - 1].throughput_info_ptr == NULL)
      {
        LOG_MSG_ERROR_0("ps_sys_conf_get_ex() : Thrpt cache empty");
        *ps_errno = DS_EINVAL;
         break;
      }
	  
      memscpy(throughput_info_ptr,
              sizeof(throughput_info_ptr->num_trpt_info) + \
              (PS_SYS_MAX_NUM_THROUGHPUT_INFO * sizeof(ds_sys_throughput_info_type)),
              conf_tech_all[subscription_id - 1].throughput_info_ptr,
              sizeof(conf_tech_all[subscription_id - 1].\
              throughput_info_ptr->num_trpt_info) + 
              conf_tech_all[subscription_id - 1].\
              throughput_info_ptr->num_trpt_info *  sizeof(ds_sys_throughput_info_type));

      break;
    }

    case PS_SYS_CONF_NULL_RESOLVED_APN: 
    { 
      /* 1. 3gpp, 3gpp, wlan mode handler update the null resolved apn name 
            with ps_sys.
         2. This is cached in technology specific cache when the set operation
            is called for specific tech type.
         3. EPC mode handler resolves the null apn by querying ps_sys for different 
            tech tyep and perform set operation with tech_all conf. 
         4. Event will be generated if the set operation is performed with
            tech_all configuration. 
      */
      null_resolved_apn_ptr  = (ps_sys_null_resolved_apn_info_type *)argval_ptr;
      
      if (tech_type == PS_SYS_TECH_3GPP)
      {
        memscpy(null_resolved_apn_ptr,
                sizeof(ps_sys_null_resolved_apn_info_type),
                conf_tech_3gpp[subscription_id - 1].null_resolved_apn_ptr,
                sizeof(ps_sys_null_resolved_apn_info_type));
      }
      else if ( tech_type == PS_SYS_TECH_3GPP2)
      {
        memscpy(null_resolved_apn_ptr,
                sizeof(ps_sys_null_resolved_apn_info_type),
                conf_tech_3gpp2[subscription_id - 1].null_resolved_apn_ptr,
                sizeof(ps_sys_null_resolved_apn_info_type));
      }
      else if (tech_type == PS_SYS_TECH_WLAN)
      {
        memscpy(null_resolved_apn_ptr,
                sizeof(ps_sys_null_resolved_apn_info_type),
                conf_tech_wlan[subscription_id - 1].null_resolved_apn_ptr,
                sizeof(ps_sys_null_resolved_apn_info_type));
      }
             
      LOG_MSG_INFO1_3("ps_sys_conf_get_ex : apn name length:%d, apn_name:%s,"
                      "tech_type %d",
                      null_resolved_apn_ptr->apn_length,
                      (null_resolved_apn_ptr->apn_name != NULL) ? 
                      null_resolved_apn_ptr->apn_name : 0,
                      tech_type);

      break;
    }

    case PS_SYS_CONF_3GPP2_QOS_NETWORK_STATUS:
    {
      net_aware_ptr = (ps_sys_3gpp2_network_qos_aware_status *)argval_ptr;
      memscpy(net_aware_ptr,
              sizeof(ps_sys_3gpp2_network_qos_aware_status),
              conf_tech_3gpp2[subscription_id - 1].net_aware_status,
              sizeof(ps_sys_3gpp2_network_qos_aware_status));

      LOG_MSG_INFO2_1("ps_sys_conf_get_ex :net aware status val %d ",    
                       *net_aware_ptr); 

      break;
    }

    case PS_SYS_CONF_DL_THROUGHPUT_INFO_TIMER:
    {
      dl_thrpt_timer_ptr = (ps_sys_report_dl_timer_type *)argval_ptr;
      memscpy(dl_thrpt_timer_ptr,
              sizeof(ps_sys_report_dl_timer_type),
              conf_tech_all[subscription_id - 1].dl_thrpt_timer_ptr,
              sizeof(ps_sys_report_dl_timer_type));

      LOG_MSG_INFO2_1("ps_sys_conf_get_ex :dl throughput timer val %d ",    
                      *dl_thrpt_timer_ptr); 

      break;
    }

    case PS_SYS_CONF_DL_THROUGHPUT_INFO:
    {
      dl_thrpt_info_ptr = (ps_sys_dl_throughput_info_type *)argval_ptr;
      memscpy(dl_thrpt_info_ptr,
              sizeof(ps_sys_dl_throughput_info_type),
              conf_tech_all[subscription_id - 1].dl_thrpt_info_ptr,
              sizeof(ps_sys_dl_throughput_info_type));

      break;
    }

    case  PS_SYS_CONF_DL_MIN_THROUGHPUT_TIMER:
    {
      min_dl_thrpt_timer_ptr = (ps_sys_report_dl_timer_type *)argval_ptr;
      memscpy(min_dl_thrpt_timer_ptr,
              sizeof(ps_sys_report_dl_timer_type),
              conf_tech_all[subscription_id - 1].min_dl_thrpt_timer_ptr,
              sizeof(ps_sys_report_dl_timer_type));
      LOG_MSG_INFO2_1("ps_sys_conf_get_ex :min dl throughput timer val %d ",    
                      *min_dl_thrpt_timer_ptr); 

      break;
    }

    case PS_SYS_CONF_THERMAL_MITIGATION_INFO:
    {
      thermal_info_ptr = (ps_sys_thermal_mitigation_action_enum_type *)argval_ptr;
      memscpy(thermal_info_ptr,
              sizeof(ps_sys_thermal_mitigation_action_enum_type),
              conf_tech_all[subscription_id - 1].thermal_info_ptr,
              sizeof(ps_sys_thermal_mitigation_action_enum_type));
      LOG_MSG_INFO2_1("ps_sys_conf_get_ex :thermal action info %d ",    
                       *thermal_info_ptr); 

      break;
    }

    case PS_SYS_CONF_DATA_SERVICE_SETTING:
    {
      data_setting_ptr = (ps_sys_data_services_setting_type *)argval_ptr;
      memscpy(data_setting_ptr,
              sizeof(ps_sys_data_services_setting_type),
              conf_tech_all[subscription_id - 1].data_setting_ptr,
              sizeof(ps_sys_data_services_setting_type));

      LOG_MSG_INFO2_1("ps_sys_conf_get_ex :data setting val %d ",    
                       *data_setting_ptr); 
       break;
    }

    case PS_SYS_CONF_DATA_SERVICE_ROAMING_SETTING:
    {
      data_roaming_ptr = (ps_sys_data_services_roaming_setting_type *)argval_ptr;
      memscpy(data_roaming_ptr,
              sizeof(ps_sys_data_services_roaming_setting_type),
              conf_tech_all[subscription_id - 1].data_roaming_setting_ptr,
              sizeof(ps_sys_data_services_roaming_setting_type));

      LOG_MSG_INFO2_1("ps_sys_conf_get_ex :data roaming val %d ",    
                       *data_roaming_ptr); 
       break;
    }

    case PS_SYS_CONF_APN_INFO:
    {
      apn_info_ptr = (ps_sys_apn_info_type *)argval_ptr;
      memscpy(apn_info_ptr,
              sizeof(ps_sys_apn_info_type),
              conf_tech_all[subscription_id - 1].apn_info_ptr,
              sizeof(ps_sys_apn_info_type));
       break;
    }

    case PS_SYS_CONF_MODEM_POWER_STATS_CONFIGURATION:
    {
      power_stats_config_ptr = (ps_sys_modem_power_stats_config_type *)argval_ptr;
      memscpy(power_stats_config_ptr,
              sizeof(ps_sys_modem_power_stats_config_type),
              conf_tech_all[subscription_id - 1].power_stats_config_ptr,
              sizeof(ps_sys_modem_power_stats_config_type));

      LOG_MSG_INFO2_1("ps_sys_conf_get_ex :power stats config %d ",    
                      power_stats_config_ptr->enable_power_statistics); 

      break;
    }

    case PS_SYS_CONF_MODEM_POWER_STATISTICS:
    {
      power_stats_info_ptr = (ps_sys_modem_power_stats_info_type *)argval_ptr;
      memscpy(power_stats_info_ptr,
              sizeof(ps_sys_modem_power_stats_info_type),
              conf_tech_all[subscription_id - 1].power_stats_info_ptr,
              sizeof(ps_sys_modem_power_stats_info_type));

      break;
    }

    case PS_SYS_CONF_DATA_SERVICE_WIFI_SETTING:
    {
      wifi_settings_ptr = (ps_sys_data_services_wifi_setting_type *)argval_ptr;
      memscpy(wifi_settings_ptr,
              sizeof(ps_sys_data_services_wifi_setting_type),
              conf_tech_all[subscription_id - 1].wifi_settings_ptr,
              sizeof(ps_sys_data_services_wifi_setting_type));

      LOG_MSG_INFO2_1("ps_sys_conf_get_ex :wifi settings val %d ",    
                       *wifi_settings_ptr); 
       break;
    }
    case PS_SYS_CONF_REMOTE_SOCKET_HDLR_AVAIL:
    {
      hdlr_avail = (ps_sys_remote_socket_hdlr_avail_type *)argval_ptr;
      memscpy(hdlr_avail,
              sizeof(ps_sys_remote_socket_hdlr_avail_type),
              conf_tech_all[subscription_id - 1].hdlr_avail,
              sizeof(ps_sys_remote_socket_hdlr_avail_type));

      LOG_MSG_INFO2_1("ps_sys_conf_get_ex : hdlr available %d ",    
                       hdlr_avail->remote_socket_hdlr_avail); 
       break;
    }
    
    case PS_SYS_CONF_ROAMING_STATUS_INFO:
    {
      roaming_status_info_ptr = (ps_sys_roaming_status_info_type *)argval_ptr;
      memscpy(roaming_status_info_ptr,
              sizeof(ps_sys_roaming_status_info_type),
              conf_tech_all[subscription_id - 1].roaming_status_info_ptr,
              sizeof(ps_sys_roaming_status_info_type));

      if (roaming_status_info_ptr->technology == DS_SYS_NETWORK_MAX)
      {
        *ps_errno = DS_EINVAL;
      }
       
      break;
    }    

    default:
    {
      *ps_errno = DS_EINVAL;
      LOG_MSG_ERROR_1("ps_sys_conf_get_ex() : Conf %d not found", conf_name);
      break;
    }      
  }
  PS_LEAVE_CRIT_SECTION(&ps_sys_crit_section);
  if (*ps_errno != 0)
  {
    return -1;
  }

  return 0;
} /* ps_sys_conf_get_ex() */

int16 ps_sys_conf_set
(
  ps_sys_tech_enum_type              tech_type,
  ps_sys_conf_enum_type              conf_name,
  void                             * argval_ptr,
  int16                            * ps_errno
)
{
  return ps_sys_conf_set_ex(tech_type, 
                            conf_name, 
                            PS_SYS_PRIMARY_SUBS,
                            argval_ptr, 
                            ps_errno);
}/* ps_sys_conf_set() */

int16 ps_sys_conf_set_ex
(
  ps_sys_tech_enum_type              tech_type,
  ps_sys_conf_enum_type              conf_name,
  ps_sys_subscription_enum_type      subscription_id,
  void                             * argval_ptr,
  int16                            * ps_errno
)
{
  ps_sys_event_enum_type             event_name = PS_SYS_EVENT_MIN;
  void                             * event_info = NULL;
  ps_sys_eventi_cmd_type           * cmd_data_info;
  boolean                            invoke_cb = FALSE;
  int                                ret_val;
  uint32                             cnt_pdn;
  uint32                             cnt_network;
  uint32                             size_sys_status_ex;
  uint32                             num_bytes = 0; 

  /* Configuration specific structures */
  ps_sys_system_status_type            * conf_system_status_type;
  ps_sys_3gpp2_s101_status_change_type * s101_status_change_info_type;
  ps_sys_3gpp2_epzid_info_type         * epzid_info_type;
  ps_sys_conf_set_dun_ctrl_pref_type   * set_dun_ctrl_pref_ptr = NULL;
  ps_sys_conf_get_dun_ctrl_pref_type   * get_dun_ctrl_pref_ptr = NULL;
  ps_sys_lte_attach_enum_type          * lte_attach_type = NULL;
  ps_sys_wlan_status_type              * wlan_status;
  ps_sys_wlan_preference_enum_type     * wlan_preference;
  ps_sys_system_status_ex_type         * system_status_ex_ptr;
  ps_sys_apn_pref_sys_type             * apn_pref_sys_ptr;
  ps_sys_wlan_mac_addr_type            * wlan_mac_addr_ptr = NULL;
  ps_sys_wlan_ssid_type                * wlan_ssid_ptr = NULL;
  ps_sys_wlan_max_conn_info_type       * wlan_max_conn_ptr = NULL;
  ps_sys_conf_throughput_timer_type      *throughput_timer;
  ps_sys_throughput_info_list_type       *throughput_info_ptr;
  ps_sys_throughput_info_list_type       *temp_thrpt;
  ps_sys_is_pdn_only_emergency_type    * only_emergency_pdn_ptr = NULL;
  ps_sys_roaming_disallowed_info_type  * roaming_disallowed_info_ptr = NULL;
  ps_sys_fast_dormancy_timer_values_s    *temp_timer_p;
  ps_sys_fast_dormancy_timer_values_s    *timer_values_p;
  ps_sys_null_resolved_apn_info_type   * null_resolved_apn_ptr = NULL;
  ps_sys_3gpp2_network_qos_aware_status  *net_aware_status = NULL;
  ps_sys_report_dl_timer_type          * dl_thrpt_timer_ptr = NULL;
  ps_sys_dl_throughput_info_type       * dl_thrpt_info_ptr = NULL;
  ps_sys_thermal_mitigation_action_enum_type *thermal_info_ptr = NULL;
  ps_sys_data_services_setting_type      * data_setting_ptr = NULL;
  ps_sys_data_services_roaming_setting_type * data_roaming_ptr = NULL;
  ps_sys_apn_info_type                   * apn_info_ptr = NULL;
  ps_sys_apn_params_type                 * temp_apn_info = NULL;
  ps_sys_modem_power_stats_info_type    * power_stats_info_ptr = NULL;
  ps_sys_data_services_wifi_setting_type  * wifi_settings_ptr = NULL;
  ps_sys_remote_socket_hdlr_avail_type     * hdlr_avail = NULL;
  ps_sys_modem_power_stats_config_type  * power_stats_config_ptr = NULL;
  ps_sys_roaming_status_info_type       * roaming_status_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("ps_sys_conf_set_ex(): "
                           "tech %d conf %d argval_ptr 0x%p",
                           tech_type, conf_name, argval_ptr);

  LOG_MSG_INFO1_1("ps_sys_conf_set_ex(): subscription_id %d", subscription_id);
                         

  if (ps_errno == NULL)
  {
    LOG_MSG_INVALID_INPUT_0("ps_sys_conf_set_ex(): Invalid argument ps_errno");
    return -1;
  }
  
  ret_val = ps_sys_confi_verify_tech_conf_name(tech_type,conf_name);
  if (0 > ret_val)
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_ERROR_0("ps_sys_conf_set_ex(): Invalid argument passed");
    return -1;
  }

  if (argval_ptr == NULL)
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_0("ps_sys_conf_set_ex(): Invalid argval_ptr");
    return -1;
  }

  if (subscription_id >= PS_SYS_SUBS_MAX || 
      subscription_id < PS_SYS_DEFAULT_SUBS)
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_1("ps_sys_conf_set_ex(): Invalid subscription_id %d",
                            subscription_id);
    return -1;
  }

  if (PS_SYS_DEFAULT_SUBS == subscription_id )
  {
    subscription_id = ps_sys_get_default_data_subscription();
  }

  *ps_errno = 0;  /*Set it success*/

  /*-------------------------------------------------------------------------
    Add code to Implement different configuration and remove all temp vars
    and code
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&ps_sys_crit_section);
  switch (conf_name)
  {
    case PS_SYS_CONF_SYSTEM_STATUS: 
    { 
      ret_val = memcmp(conf_tech_all[subscription_id - 1]. \
                        system_status_type,
                      (ps_sys_system_status_type*)argval_ptr,
                       sizeof(ps_sys_system_status_type));
      if (ret_val != 0)
      {
        /*---------------------------------------------------------------------
         Process the configuration
        ---------------------------------------------------------------------*/
        PS_SYSTEM_HEAP_MEM_ALLOC( conf_system_status_type, sizeof(ps_sys_system_status_type),
                                  ps_sys_system_status_type*);
  
        if (NULL == conf_system_status_type)
        {
          *ps_errno = DS_NOMEMORY;
          goto bail;
        }
        
        memscpy(conf_system_status_type,
                sizeof(ps_sys_system_status_type),
               (ps_sys_system_status_type*)argval_ptr,
               sizeof(ps_sys_system_status_type));
  
        LOG_MSG_INFO1_3("ps_sys_conf_set_ex : pref network %d, RAT mask 0x%x"
                        "so mask 0x%x",
                        conf_system_status_type->preferred_network,
                        conf_system_status_type->network_info[conf_system_status_type->preferred_network].rat_mask,
                        conf_system_status_type->network_info[conf_system_status_type->preferred_network].so_mask);

        event_info = (void *)conf_system_status_type;
        event_name = PS_SYS_EVENT_SYSTEM_STATUS_CHANGE;
        invoke_cb = TRUE;
      
        /* Update temp storage */
        memscpy(conf_tech_all[subscription_id - 1].system_status_type,
                sizeof(ps_sys_system_status_type),
               (ps_sys_system_status_type*)argval_ptr,
               sizeof(ps_sys_system_status_type));
      }
      else
      {
        LOG_MSG_INFO2_2("ps_sys_conf_set_ex : No change in Config Value "
                        "filtering out event, tech %d, conf %d",
                        tech_type, conf_name);
      }
      break;
    }

    case PS_SYS_CONF_SYSTEM_STATUS_EX: 
    { 

      /* If the cache is NULL don't compare with the input */
      if (NULL == conf_tech_all[subscription_id - 1].system_status_ex_ptr)
      {
        LOG_MSG_INFO1_0("ps_sys_conf_set_ex() "
                        "System Status Ex cache ptr is NULL");
        ret_val = -1;
      }
      else
      {
        /* compare the cache and user input */
        ret_val = ps_sys_conf_comp_sys_status_ex_val
                  (
                    conf_tech_all[subscription_id - 1].system_status_ex_ptr,
                    (ps_sys_system_status_ex_type*)argval_ptr
                  );
      }
      
      /* If the data to be set is different from the one in cache */
      if (ret_val != 0)
      {
        /*------------------------------------------------------------------
          Process the configuration
        ------------------------------------------------------------------*/

        /* Alloacte memory for event info */
        PS_SYSTEM_HEAP_MEM_ALLOC(system_status_ex_ptr, sizeof(ps_sys_system_status_ex_type),
                                 ps_sys_system_status_ex_type*);

        if (NULL == system_status_ex_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          goto bail;
        }
      
        /* Update event info with sys status from user*/
        memscpy(system_status_ex_ptr,
                sizeof(ps_sys_system_status_ex_type),
               (ps_sys_system_status_ex_type *) argval_ptr,
               sizeof(ps_sys_system_status_ex_type));

        LOG_MSG_INFO1_2("ps_sys_conf_set_ex : "
                        "num of rats %d num of pdns %d",
                        system_status_ex_ptr->num_avail_sys,
                        system_status_ex_ptr->num_avail_apns);

        LOG_MSG_INFO1_0("ps_sys_conf_set_ex : First tupple of technology, RAT"
                        " and SO is preferred system"); 
                    
        for (cnt_network=0; cnt_network < system_status_ex_ptr->num_avail_sys; 
             cnt_network++)
        {
          LOG_MSG_INFO1_3("ps_sys_conf_set_ex: technology %d "
                          "rat value %d so mask 0x%x",
                          system_status_ex_ptr->avail_sys[cnt_network].technology,
                          system_status_ex_ptr->avail_sys[cnt_network].rat_value,
                          system_status_ex_ptr->avail_sys[cnt_network].so_mask);
        }
      
        for (cnt_pdn=0; cnt_pdn < system_status_ex_ptr->num_avail_apns; 
             cnt_pdn++)
        {
          LOG_MSG_INFO1_2("ps_sys_conf_set pdn name %s available sys for pdn %d",
                          system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].apn_name,
                          system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].num_avail_sys);
  
          if(system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].num_avail_sys > 0)
          {
            LOG_MSG_INFO1_3("ps_sys_conf_set pref tech %d "
                            "pref rat %d pref so mask 0x%x",
                            system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].avail_sys[0].technology,
                            system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].avail_sys[0].rat_value,
                            system_status_ex_ptr->apn_pref_sys_info[cnt_pdn].avail_sys[0].so_mask);
          }                   
        }                    

        event_info = (void *)system_status_ex_ptr;
        event_name = PS_SYS_EVENT_SYSTEM_STATUS_EX;
        invoke_cb  = TRUE;
        
        /* Free the memory if the system status cache ptr is not null. 
        Because of memory optimization we recalculate the cache size
        based on number of available system and apn */
        if (NULL != conf_tech_all[subscription_id - 1].system_status_ex_ptr)
        {
          PS_SYSTEM_HEAP_MEM_FREE(conf_tech_all[subscription_id - 1].\
                                    system_status_ex_ptr);
        }
		
        /* Reallocate memory based on the calculated size */
        size_sys_status_ex = 
          ps_sys_conf_size_sys_status_ex((ps_sys_system_status_ex_type *)\
                                         argval_ptr);
                                         
          PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subscription_id - 1].system_status_ex_ptr, 
                                   size_sys_status_ex,
                                   ps_sys_system_status_ex_type*);

        if (NULL == conf_tech_all[subscription_id - 1].system_status_ex_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          goto bail;
        }

        memset(conf_tech_all[subscription_id - 1].system_status_ex_ptr, 
               0, 
               size_sys_status_ex);		
		
        /* Update the cache with the new values */
        ps_sys_conf_update_sys_status_ex_cache
        (
          conf_tech_all[subscription_id - 1].system_status_ex_ptr,
          (ps_sys_system_status_ex_type *)argval_ptr 
        );		
      }
      else
      {
        LOG_MSG_INFO2_2("ps_sys_conf_set_ex : No change in Config Value "
                        "filtering out event, tech %d, conf %d",
                        tech_type, conf_name);
      }
      break;
    }    

    case PS_SYS_CONF_APN_PREF_SYS: 
    {
      /*---------------------------------------------------------------------
        Process the configuration
      ---------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_ALLOC(apn_pref_sys_ptr, sizeof(ps_sys_apn_pref_sys_type),
                               ps_sys_apn_pref_sys_type*);

      if (NULL == apn_pref_sys_ptr)
      {
        *ps_errno = DS_NOMEMORY;
        goto bail;
      }
      
      memscpy(apn_pref_sys_ptr,
              sizeof(ps_sys_apn_pref_sys_type),
             (ps_sys_apn_pref_sys_type *) argval_ptr,
             sizeof(ps_sys_apn_pref_sys_type));

      LOG_MSG_INFO1_2("ps_sys_conf_set_ex : pdn name %s tech pref %d",
                      apn_pref_sys_ptr->apn, apn_pref_sys_ptr->pref_sys);

      event_info = (void *)apn_pref_sys_ptr;
      event_name = PS_SYS_EVENT_APN_PREF_SYS_CHANGE;
      invoke_cb = TRUE;
      
      /* Update temp storage (Just a temp solution*/
      memscpy(conf_tech_all[subscription_id - 1].apn_pref_sys_ptr,
              sizeof(ps_sys_apn_pref_sys_type),
             (ps_sys_apn_pref_sys_type *) argval_ptr,
             sizeof(ps_sys_apn_pref_sys_type));
      break;
    }

    case PS_SYS_CONF_3GPP2_S101_STATUS_CHANGE:
    {
      /*---------------------------------------------------------------------
        Process the configuration
      ---------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_ALLOC(s101_status_change_info_type,
                               sizeof(ps_sys_3gpp2_s101_status_change_type),
                               ps_sys_3gpp2_s101_status_change_type*);

      if (NULL == s101_status_change_info_type)
      {
        *ps_errno = DS_NOMEMORY;
        goto bail;
      }
      
      memscpy(s101_status_change_info_type,
              sizeof(ps_sys_3gpp2_s101_status_change_type),
             (ps_sys_3gpp2_s101_status_change_type*)argval_ptr,
             sizeof(ps_sys_3gpp2_s101_status_change_type));

      event_info = (void *)s101_status_change_info_type;
      event_name = PS_SYS_EVENT_3GPP2_S101_STATUS_CHANGE;
      invoke_cb = TRUE;
      
      /* Update temp storage (Just a temp solution*/
      memscpy(conf_tech_3gpp2[subscription_id - 1].\
              s101_status_change_type,
              sizeof(ps_sys_3gpp2_s101_status_change_type),
            (ps_sys_3gpp2_s101_status_change_type*)argval_ptr,
            sizeof(ps_sys_3gpp2_s101_status_change_type));
      break;
    }

    case PS_SYS_CONF_3GPP2_EPZID_INFO: 
    { 
      /*---------------------------------------------------------------------
       Process the configuration
      ---------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_ALLOC(epzid_info_type, sizeof(ps_sys_3gpp2_epzid_info_type),
                                ps_sys_3gpp2_epzid_info_type*);

      if (NULL == epzid_info_type)
      {
        *ps_errno = DS_NOMEMORY;
        goto bail;
      }
      
      memscpy(epzid_info_type,
              sizeof(ps_sys_3gpp2_epzid_info_type),
             (ps_sys_3gpp2_epzid_info_type *)argval_ptr,
             sizeof(ps_sys_3gpp2_epzid_info_type));

      event_info = (void *)epzid_info_type;
      event_name = PS_SYS_EVENT_3GPP2_EPZID_INFO;
      invoke_cb = TRUE;
      
      /* Update temp storage */
      memscpy(conf_tech_3gpp2[subscription_id - 1].epzid_info_type,
              sizeof(ps_sys_3gpp2_epzid_info_type),
             (ps_sys_3gpp2_epzid_info_type *)argval_ptr,
             sizeof(ps_sys_3gpp2_epzid_info_type));
      break;
    }
     
    case PS_SYS_CONF_3GPP2_AN_AUTH_STATUS: 
    { 
      /*---------------------------------------------------------------------
       Process the configuration
      ---------------------------------------------------------------------*/

      /* Update temp storage */
      memscpy(conf_tech_3gpp2[subscription_id - 1].an_auth_status,
              sizeof(ps_sys_conf_3gpp2_an_auth_status_enum_type),
             (ps_sys_conf_3gpp2_an_auth_status_enum_type *)argval_ptr,
             sizeof(ps_sys_conf_3gpp2_an_auth_status_enum_type));
      break;
    }

    case PS_SYS_CONF_ENABLE_LOOPBACK: 
    { 
      /*---------------------------------------------------------------------
       Process the configuration
      ---------------------------------------------------------------------*/
      /* Update temp storage */
      memscpy(conf_tech_all[subscription_id - 1].\
                enable_loopback_ptr,
              sizeof(ps_sys_conf_enable_loopback_type),
             (ps_sys_conf_enable_loopback_type *) argval_ptr,
             sizeof(ps_sys_conf_enable_loopback_type));
      break;
    }

    case PS_SYS_CONF_DUN_CTRL_PREF:
    {          
      /*---------------------------------------------------------------------
       Process the configuration
      ---------------------------------------------------------------------*/
     PS_SYSTEM_HEAP_MEM_ALLOC(get_dun_ctrl_pref_ptr, 
                              sizeof(ps_sys_conf_get_dun_ctrl_pref_type),
                              ps_sys_conf_get_dun_ctrl_pref_type*);

      if (NULL == get_dun_ctrl_pref_ptr)
      {
        *ps_errno = DS_NOMEMORY;
        goto bail;
      }
      
      memset(get_dun_ctrl_pref_ptr,
             0,
             sizeof(ps_sys_conf_get_dun_ctrl_pref_type));
      
      set_dun_ctrl_pref_ptr = 
        (ps_sys_conf_set_dun_ctrl_pref_type *) argval_ptr;
        
      get_dun_ctrl_pref_ptr->is_dun_ctrl_enabled = 
        set_dun_ctrl_pref_ptr->enable_dun_ctrl;
     
       /*-----------------------------------------------------------------------
        if mask is not set by application then we set default value of true
        for dun allow preference
      -----------------------------------------------------------------------*/
      if (set_dun_ctrl_pref_ptr->optional_pref.param_mask & 
            PS_SYS_CONF_DUN_CTRL_PREF_ALLOW_DUN_CALL_MASK)
      {
        get_dun_ctrl_pref_ptr->allow_dun_call = 
          set_dun_ctrl_pref_ptr->optional_pref.allow_dun_call;
      }
      else
      {
        get_dun_ctrl_pref_ptr->allow_dun_call = TRUE;
      }
      
      if (TRUE == set_dun_ctrl_pref_ptr->enable_dun_ctrl)
      {
         if (set_dun_ctrl_pref_ptr->optional_pref.param_mask &
               PS_SYS_CONF_DUN_CTRL_PREF_ENTITLEMENT_CHECK_MASK)
         {
           get_dun_ctrl_pref_ptr->is_entitlement_check_enabled = TRUE;
           if (NULL != set_dun_ctrl_pref_ptr-> \
                        optional_pref.entitlement_check_cback_f_ptr)
                        
           {
            /*-----------------------------------------------------------------            
              Register for event. In case of error, we would deregister it else
              its User responsibility toto deregister
            -----------------------------------------------------------------*/
            ret_val = 
              ps_sys_event_reg
              ( 
                PS_SYS_TECH_ALL, 
                PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
                (ps_sys_event_cback_f_ptr_type)
                  set_dun_ctrl_pref_ptr-> \
                    optional_pref.entitlement_check_cback_f_ptr, 
                set_dun_ctrl_pref_ptr-> \
                  optional_pref.entitlement_check_cback_user_data_ptr, 
                ps_errno
              );

             if (0 > ret_val )
             {
               LOG_MSG_ERROR_3("ps_sys_conf_set_ex() conf %d event=%d, err=%d",
                               conf_name,
                               PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
                               *ps_errno);
               goto bail_dun_ctrl_pref;                           
             }
           }           
         }
         
         if (set_dun_ctrl_pref_ptr->optional_pref.param_mask &
               PS_SYS_CONF_DUN_CTRL_PREF_SILENT_REDIAL_CHECK_MASK)
         {
           get_dun_ctrl_pref_ptr->is_silent_redial_check_enabled = TRUE;
           if (NULL != set_dun_ctrl_pref_ptr-> \
                        optional_pref.silent_redial_check_cback_f_ptr)
           {          
             /*-----------------------------------------------------------------            
              Register for event. In case of error, we would deregister it else
              its User responsibility toto deregister
             -----------------------------------------------------------------*/
             ret_val = 
               ps_sys_event_reg
               ( 
                 PS_SYS_TECH_ALL, 
                 PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK,
                 (ps_sys_event_cback_f_ptr_type) set_dun_ctrl_pref_ptr-> \
                   optional_pref.silent_redial_check_cback_f_ptr, 
                 set_dun_ctrl_pref_ptr-> \
                   optional_pref.silent_redial_check_cback_user_data_ptr, 
                 ps_errno
               );

             if (0 > ret_val )
             {
               LOG_MSG_ERROR_3("ps_sys_conf_set_ex() conf %d event=%d, err=%d",
                               conf_name,
                               PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK,
                               *ps_errno);
               goto bail_dun_ctrl_pref;
             }
           }           
         }
         
         if (set_dun_ctrl_pref_ptr->optional_pref.param_mask &
               PS_SYS_CONF_DUN_CTRL_PREF_CALL_STATUS_NOTIFICATION_MASK)
         {
           get_dun_ctrl_pref_ptr->is_call_status_notification_enabled = TRUE;
           if (NULL != set_dun_ctrl_pref_ptr-> \
                        optional_pref.call_status_notification_cback_f_ptr)
           {
             /*-----------------------------------------------------------------            
              Register for event. In case of error, we would deregister it else
              its User responsibility toto deregister
             -----------------------------------------------------------------*/
             ret_val = 
               ps_sys_event_reg
               ( 
                 PS_SYS_TECH_ALL, 
                 PS_SYS_EVENT_DUN_CTRL_CALL_STATUS,
                 (ps_sys_event_cback_f_ptr_type) set_dun_ctrl_pref_ptr-> \
                   optional_pref.call_status_notification_cback_f_ptr, 
                 set_dun_ctrl_pref_ptr-> \
                   optional_pref.call_status_notification_cback_user_data_ptr, 
                 ps_errno
               );

             if (0 > ret_val )
             {
               LOG_MSG_ERROR_3("ps_sys_conf_set_ex() conf %d event=%d, err=%d",
                               conf_name,
                               PS_SYS_EVENT_DUN_CTRL_CALL_STATUS,
                               *ps_errno);
               goto bail_dun_ctrl_pref;
             }
           }            
         }
      }

      bail_dun_ctrl_pref : 
      
      if(0 == *ps_errno)
      {
        /* Update temp storage */
        memscpy(conf_tech_all[subscription_id - 1]. \
                get_dun_ctrl_pref_ptr,
                sizeof(ps_sys_conf_get_dun_ctrl_pref_type),
               get_dun_ctrl_pref_ptr,
               sizeof(ps_sys_conf_get_dun_ctrl_pref_type));
      }
      else
      {    
        /*---------------------------------------------------------------------
          Deregister all events callbacks in error case
        ---------------------------------------------------------------------*/
        (void)ps_sys_event_dereg
        (
          PS_SYS_TECH_ALL,
          PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK, 
          (ps_sys_event_cback_f_ptr_type)
            set_dun_ctrl_pref_ptr->optional_pref.entitlement_check_cback_f_ptr,  
          ps_errno
        );

        (void)ps_sys_event_dereg
        (
          PS_SYS_TECH_ALL,
          PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK, 
          (ps_sys_event_cback_f_ptr_type) 
            set_dun_ctrl_pref_ptr-> \
              optional_pref.silent_redial_check_cback_f_ptr,  
          ps_errno
        );

        (void)ps_sys_event_dereg
        (
          PS_SYS_TECH_ALL,
          PS_SYS_EVENT_DUN_CTRL_CALL_STATUS, 
          (ps_sys_event_cback_f_ptr_type) set_dun_ctrl_pref_ptr-> \
            optional_pref.call_status_notification_cback_f_ptr,  
          ps_errno
        );     
      }
      
      PS_SYSTEM_HEAP_MEM_FREE(get_dun_ctrl_pref_ptr);
      break;      
    }
    case PS_SYS_CONF_3GPP_LTE_ATTACH_TYPE: 
    { 
      /*---------------------------------------------------------------------
        Process the configuration
      ---------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_ALLOC(lte_attach_type, sizeof(ps_sys_lte_attach_enum_type),
                               ps_sys_lte_attach_enum_type*);

      if (NULL == lte_attach_type)
      {
        *ps_errno = DS_NOMEMORY;
        goto bail;
      }
      
      memscpy(lte_attach_type,
              sizeof(ps_sys_lte_attach_enum_type),
             (ps_sys_lte_attach_enum_type*)argval_ptr,
             sizeof(ps_sys_lte_attach_enum_type));

      event_info = (void *)lte_attach_type;
      event_name = PS_SYS_EVENT_3GPP_LTE_ATTACH_TYPE_CHANGE;
      invoke_cb = TRUE;
      
      /* Update temp storage (Just a temp solution*/
      memscpy(conf_tech_3gpp[subscription_id - 1].lte_attach_type,
              sizeof(ps_sys_lte_attach_enum_type),
             (ps_sys_lte_attach_enum_type *) argval_ptr,
             sizeof(ps_sys_lte_attach_enum_type));
      break;
    }

    case PS_SYS_CONF_3GPP_EMERGENCY_PDN_STATUS:
    {
      ret_val = memcmp(conf_tech_3gpp[subscription_id - 1]. \
                        only_emergency_pdn_info,
                      (ps_sys_is_pdn_only_emergency_type*)argval_ptr,
                       sizeof(ps_sys_is_pdn_only_emergency_type));
      if (ret_val != 0)
      {
        /*---------------------------------------------------------------------
          Process the configuration
        ---------------------------------------------------------------------*/
        PS_SYSTEM_HEAP_MEM_ALLOC(only_emergency_pdn_ptr, 
                                 sizeof(ps_sys_is_pdn_only_emergency_type),
                                 ps_sys_is_pdn_only_emergency_type*);
  
        if (NULL == only_emergency_pdn_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          goto bail;
        }
        
        memscpy(only_emergency_pdn_ptr,
                sizeof(ps_sys_is_pdn_only_emergency_type),
                (ps_sys_is_pdn_only_emergency_type*)argval_ptr,
                sizeof(ps_sys_is_pdn_only_emergency_type));
  
        event_info = (void *)only_emergency_pdn_ptr;
        event_name = PS_SYS_EVENT_3GPP_EMERGENCY_PDN_STATUS;
        invoke_cb = TRUE;
        
        /* Update temp storage */
        memscpy(conf_tech_3gpp[subscription_id - 1].only_emergency_pdn_info,
                sizeof(ps_sys_is_pdn_only_emergency_type),
                (ps_sys_is_pdn_only_emergency_type *) argval_ptr,
                sizeof(ps_sys_is_pdn_only_emergency_type));
      }
      else
      {
        LOG_MSG_INFO2_2("ps_sys_conf_set_ex : No change in Config Value "
                        "filtering out event, tech %d, conf %d",
                        tech_type, conf_name);
      }
      break;
    }

    case PS_SYS_CONF_3GPP_ROAMING_DISALLOWED_INFO: 
    {

      /* If the cache is NULL, dont compare with input */
      if (NULL == conf_tech_3gpp[subscription_id - 1].roaming_disallowed_info_ptr)
      {
        LOG_MSG_INFO1_0("ps_sys_conf_set_ex() "
                        "Roaming_disallowed cache is NULL.Generate indication");
        ret_val = -1;
      }
      else
      {
        /* compare the cache and user input */
        roaming_disallowed_info_ptr  = (ps_sys_roaming_disallowed_info_type*)
                                        argval_ptr;

        /* calculate the size stored in cache */
        num_bytes = sizeof(roaming_disallowed_info_ptr->is_roaming_disallowed) + 
                    sizeof(roaming_disallowed_info_ptr->num_apn_names) + 
                    (roaming_disallowed_info_ptr->num_apn_names * 
                    sizeof(ps_sys_apn_name_type));

        ret_val   = memcmp(conf_tech_3gpp[subscription_id - 1]. \
                           roaming_disallowed_info_ptr,
                           (ps_sys_roaming_disallowed_info_type*)argval_ptr,
                           num_bytes);
      }

      if (ret_val != 0)
      {
        /*---------------------------------------------------------------------
          Process the configuration
        ---------------------------------------------------------------------*/

        /* Allocate memory for the event info indication */
        PS_SYSTEM_HEAP_MEM_ALLOC(roaming_disallowed_info_ptr,
                                 sizeof(ps_sys_roaming_disallowed_info_type),
                                 ps_sys_roaming_disallowed_info_type*);
  
        if (NULL == roaming_disallowed_info_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          goto bail;
        }
        
        /* Copy the data to the event info ptr */
        memscpy(roaming_disallowed_info_ptr,
                sizeof(ps_sys_roaming_disallowed_info_type),
                (ps_sys_roaming_disallowed_info_type*)argval_ptr,
                sizeof(ps_sys_roaming_disallowed_info_type));
  
        event_info = (void *)roaming_disallowed_info_ptr;
        event_name = PS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO;
        invoke_cb = TRUE;

        LOG_MSG_INFO1_2("ps_sys_conf_set_ex : Roaming disallowed info:"
                        "is_roaming_disallowed %d, num_apn_names %d",
                         roaming_disallowed_info_ptr->is_roaming_disallowed,
                         roaming_disallowed_info_ptr->num_apn_names); 
                    
        for(cnt_pdn = 0; cnt_pdn < roaming_disallowed_info_ptr->num_apn_names; cnt_pdn++)
        {
          LOG_MSG_INFO1_2("ps_sys_conf_set_ex :"
                          "apn[%d]: %s",
                          cnt_pdn,
                          roaming_disallowed_info_ptr->apn_list[cnt_pdn].apn_name);
        }
        
        /* Free the memory if the roaming disallowed cache ptr is not null. 
        Because of memory optimization we recalculate the cache size
        based on number of apnz */
        if(NULL != conf_tech_3gpp[subscription_id - 1].roaming_disallowed_info_ptr)
        {
          PS_SYSTEM_HEAP_MEM_FREE(conf_tech_3gpp[subscription_id - 1].
                                    roaming_disallowed_info_ptr);
        }
		
        /* Calculate the memory required for the user data */
        num_bytes = sizeof(roaming_disallowed_info_ptr->is_roaming_disallowed) +
		    sizeof(roaming_disallowed_info_ptr->num_apn_names) + 
                    (roaming_disallowed_info_ptr->num_apn_names * 
                    sizeof(ps_sys_apn_name_type));
           
        /* Reallocate memory for the cache */                              
        PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_3gpp[subscription_id - 1].roaming_disallowed_info_ptr,
                                 num_bytes, ps_sys_roaming_disallowed_info_type*);

        if (NULL == conf_tech_3gpp[subscription_id - 1].roaming_disallowed_info_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          goto bail;
        }

        memset(conf_tech_3gpp[subscription_id - 1].roaming_disallowed_info_ptr, 
               0, 
               num_bytes);		
		
        /* Update the cache with the new values */
        memscpy(conf_tech_3gpp[subscription_id - 1].roaming_disallowed_info_ptr,
                num_bytes,
                (ps_sys_lte_attach_enum_type *) argval_ptr,
                num_bytes);
      }
      break;
    }

    case PS_SYS_CONF_WLAN_STATUS: 
    { 
      /*---------------------------------------------------------------------
       Process the configuration
      ---------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_ALLOC( wlan_status, sizeof(ps_sys_wlan_status_type),
                                ps_sys_wlan_status_type*);

      if (NULL == wlan_status)
      {
        *ps_errno = DS_NOMEMORY;
        goto bail;
      }
      
      memscpy(wlan_status,
              sizeof(ps_sys_wlan_status_type),
             (ps_sys_wlan_status_type*)argval_ptr,
             sizeof(ps_sys_wlan_status_type));

      LOG_MSG_INFO2_3("ps_sys_conf_set_ex : is_wlan_available %d, "
                      "ipv6_prefix_len %d mac address %d",
                      wlan_status->is_wlan_available,
                      wlan_status->ipv6_prefix_len,
                      wlan_status->ap_mac_addr);

      LOG_MSG_INFO2_1("ps_sys_conf_set_ex : wqe_status %d ",
                      wlan_status->wqe_status);
  
      IPV4_ADDR_MSG(wlan_status->ipv4_addr.addr.v4.ps_s_addr);  
        
      IPV6_ADDR_MSG(wlan_status->ipv4_addr.addr.v6.ps_s6_addr64);
        
      event_info = (void *)wlan_status;
      event_name = PS_SYS_EVENT_WLAN_STATUS_CHANGE;
      invoke_cb = TRUE;

      /* Update temp storage */
      memscpy(conf_tech_wlan[subscription_id - 1].wlan_status,
              sizeof(ps_sys_wlan_status_type),
             (ps_sys_wlan_status_type *) argval_ptr,
             sizeof(ps_sys_wlan_status_type));
        
      break;
    }

    case PS_SYS_CONF_GET_WLAN_MAC_ADDR:
    {
      /* Check the mac address stored in the cache and generate the event 
      only if mac address is different */
      ret_val = memcmp(conf_tech_wlan[subscription_id - 1].wlan_mac_addr_ptr,
                       (ps_sys_wlan_mac_addr_type *)argval_ptr,
                       DS_SYS_MAC_ADDR_LEN);
      if (ret_val != 0)
      {
        /*---------------------------------------------------------------------
          Process the configuration
        ---------------------------------------------------------------------*/
        PS_SYSTEM_HEAP_MEM_ALLOC(wlan_mac_addr_ptr, sizeof(ps_sys_wlan_mac_addr_type),
                                 ps_sys_wlan_mac_addr_type*);
  
        if (NULL == wlan_mac_addr_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          goto bail;
        }

        memset(wlan_mac_addr_ptr, 0, sizeof(ps_sys_wlan_mac_addr_type));
        
        memscpy(wlan_mac_addr_ptr,
                sizeof(ps_sys_wlan_mac_addr_type),
               (ps_sys_wlan_mac_addr_type *)argval_ptr,
               sizeof(uint8) * DS_SYS_MAC_ADDR_LEN);     
               
        event_info = (void *)wlan_mac_addr_ptr;
        event_name = PS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE;
        invoke_cb = TRUE;
        
        /* Update temp storage */
        memscpy(conf_tech_wlan[subscription_id - 1].wlan_mac_addr_ptr,
                sizeof(ps_sys_wlan_mac_addr_type),
                (ps_sys_wlan_mac_addr_type *) argval_ptr,
                sizeof(ps_sys_wlan_mac_addr_type));
        }        
      break;
      }      
      
    case PS_SYS_CONF_GET_WLAN_SSID:
    {
      /* Check the ssid stored in the cache and generate the event 
      only if ssid is different */
      ret_val = memcmp(conf_tech_wlan[subscription_id - 1].wlan_ssid_ptr,
                       (ps_sys_wlan_ssid_type *)argval_ptr,
                       DS_SYS_MAX_SSID_LEN);
      if (ret_val != 0)
      {
        /*---------------------------------------------------------------------
          Process the configuration
        ---------------------------------------------------------------------*/
        PS_SYSTEM_HEAP_MEM_ALLOC(wlan_ssid_ptr, sizeof(ps_sys_wlan_ssid_type),
                                 ps_sys_wlan_ssid_type*);
               
        if (NULL == wlan_ssid_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          goto bail;
      }      
      
        memset(wlan_ssid_ptr, 0, sizeof(ps_sys_wlan_ssid_type));   
      
        memscpy(wlan_ssid_ptr,
                sizeof(ps_sys_wlan_ssid_type),
               (ps_sys_wlan_ssid_type *)argval_ptr,
                sizeof(uint8) * DS_SYS_MAX_SSID_LEN); 
        
        event_info = (void *)wlan_ssid_ptr;
        event_name = PS_SYS_EVENT_WLAN_SSID_CHANGE;
        invoke_cb = TRUE;
        
        /* Update temp storage */
        memscpy(conf_tech_wlan[subscription_id - 1].wlan_ssid_ptr,
                sizeof(ps_sys_wlan_ssid_type),
                (ps_sys_wlan_ssid_type *) argval_ptr,
                sizeof(ps_sys_wlan_ssid_type));
      }  
      break;
    }

    case PS_SYS_CONF_WLAN_PREFERENCE: 
    { 
      /*---------------------------------------------------------------------
       Process the configuration
      ---------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_ALLOC( wlan_preference, sizeof(ps_sys_wlan_preference_enum_type),
                                ps_sys_wlan_preference_enum_type*);

      if (NULL == wlan_preference)
      {
        *ps_errno = DS_NOMEMORY;
        goto bail;
      }
      
      memscpy(wlan_preference,
              sizeof(ps_sys_wlan_preference_enum_type),
             (ps_sys_wlan_preference_enum_type*)argval_ptr,
             sizeof(ps_sys_wlan_preference_enum_type));

      LOG_MSG_INFO2_1("ps_sys_conf_set_ex : wlan preference %d ",
                      wlan_preference);

      event_info = (void *)wlan_preference;
      event_name = PS_SYS_EVENT_WLAN_PREFERENCE_CHANGE;
      invoke_cb = TRUE;
      /* Update temp storage */
      memscpy(conf_tech_wlan[subscription_id - 1].wlan_preference,
              sizeof(ps_sys_wlan_preference_enum_type),
             (ps_sys_wlan_preference_enum_type *) argval_ptr,
             sizeof(ps_sys_wlan_preference_enum_type));
      break;
    }
    
    case PS_SYS_CONF_WLAN_MAX_CONN:
    {
      PS_SYSTEM_HEAP_MEM_ALLOC(wlan_max_conn_ptr, sizeof(ps_sys_wlan_max_conn_info_type),
                               ps_sys_wlan_max_conn_info_type*);

      if (NULL == wlan_max_conn_ptr)
      {
        *ps_errno = DS_NOMEMORY;
        return -1;
      }
      memscpy(wlan_max_conn_ptr,
              sizeof(ps_sys_wlan_max_conn_info_type),
              (ps_sys_wlan_max_conn_info_type*)argval_ptr,
              sizeof(ps_sys_wlan_max_conn_info_type));

      LOG_MSG_INFO1_2("ps_sys_conf_set_ex : wlan max conn reached %d "
                      "no of active apns %d",
                      wlan_max_conn_ptr->is_max_conn_reached, 
                      wlan_max_conn_ptr->num_apns);

      event_info = (void *)wlan_max_conn_ptr;
      event_name = PS_SYS_EVENT_WLAN_MAX_CONN;
      invoke_cb = TRUE;
      /* Update temp storage */
      memscpy(conf_tech_wlan[subscription_id - 1].wlan_max_conn_ptr,
              sizeof(ps_sys_wlan_max_conn_info_type),
              (ps_sys_wlan_max_conn_info_type *) argval_ptr,
              sizeof(ps_sys_wlan_max_conn_info_type));
      break;
    }
    
    case PS_SYS_CONF_THROUGHPUT_INFO_TIMER:
    {
      PS_SYSTEM_HEAP_MEM_ALLOC(throughput_timer, sizeof(ps_sys_conf_throughput_timer_type),
                               ps_sys_conf_throughput_timer_type*);

      if (NULL == throughput_timer)
      {
        *ps_errno = DS_NOMEMORY;
        return -1;
      }
  
      memset(throughput_timer, 0, sizeof(ps_sys_conf_throughput_timer_type));
  
      memscpy(throughput_timer, 
              sizeof(ps_sys_conf_throughput_timer_type),
              (ps_sys_conf_throughput_timer_type *)argval_ptr,
              sizeof(ps_sys_conf_throughput_timer_type));
  
      LOG_MSG_INFO2_1("ps_sys_conf_set_ex : throughput timer  %d ",             
                     throughput_timer);              

      event_info = (void *)throughput_timer;
      event_name = PS_SYS_EVENT_UPDATED_THROUGHPUT_TIMER;
      invoke_cb = TRUE;

      /* Update temp storage */
      memscpy(conf_tech_all[subscription_id - 1].throughput_timer_ptr,
              sizeof(ps_sys_conf_throughput_timer_type),
              (ps_sys_conf_throughput_timer_type *)argval_ptr,
              sizeof(ps_sys_conf_throughput_timer_type));

      break;
    }

    case PS_SYS_CONF_THROUGHPUT_INFO:
    {
      temp_thrpt = (ps_sys_throughput_info_list_type *) argval_ptr;
      if (temp_thrpt->num_trpt_info > PS_SYS_MAX_NUM_THROUGHPUT_INFO) 
      {
        LOG_MSG_ERROR_0("Max thrpt reached! Cannot cache info!");
        goto bail;
      }

      /* Allocate memory for event info ptr */
      PS_SYSTEM_HEAP_MEM_ALLOC(throughput_info_ptr, sizeof(temp_thrpt->num_trpt_info) +
                       temp_thrpt->num_trpt_info * sizeof(ds_sys_throughput_info_type),
                       ps_sys_throughput_info_list_type*);
      
      if (NULL == throughput_info_ptr)
      {
        *ps_errno = DS_NOMEMORY;
        return -1;
      }

      memset(throughput_info_ptr, 0, sizeof(temp_thrpt->num_trpt_info) +
              temp_thrpt->num_trpt_info * sizeof(ds_sys_throughput_info_type)); 

      memscpy(throughput_info_ptr, 
              sizeof(temp_thrpt->num_trpt_info) + \
              temp_thrpt->num_trpt_info * sizeof(ds_sys_throughput_info_type), 
              temp_thrpt, 
              sizeof(temp_thrpt->num_trpt_info) + \
              temp_thrpt->num_trpt_info * sizeof(ds_sys_throughput_info_type)); 

      event_info = (void *)throughput_info_ptr;
      event_name = PS_SYS_EVENT_THROUGHPUT_INFO;
      invoke_cb  = TRUE;
      
      /* Free the memory if the throughput info ptr is not null. 
      Because of memory optimization we recalculate the cache size
      based on number of throughput info */
      if (NULL != conf_tech_all[subscription_id - 1].throughput_info_ptr)
      {
      	PS_SYSTEM_HEAP_MEM_FREE(conf_tech_all[subscription_id - 1].\
                                throughput_info_ptr);
      }
      /* Reallocate memory based on the calculated size */  
       PS_SYSTEM_HEAP_MEM_ALLOC( conf_tech_all[subscription_id - 1].throughput_info_ptr,
                                 sizeof(temp_thrpt->num_trpt_info) +
                                 temp_thrpt->num_trpt_info * sizeof(ds_sys_throughput_info_type),
                                 ps_sys_throughput_info_list_type*);

      if (NULL == conf_tech_all[subscription_id - 1].throughput_info_ptr)
      {
        *ps_errno = DS_NOMEMORY;
        return -1;
      }

      memset(conf_tech_all[subscription_id - 1].throughput_info_ptr, 
             0, 
             sizeof(temp_thrpt->num_trpt_info) +
             temp_thrpt->num_trpt_info * sizeof(ds_sys_throughput_info_type));
     
      memscpy(conf_tech_all[subscription_id - 1].throughput_info_ptr,
              sizeof(temp_thrpt->num_trpt_info) + \
              temp_thrpt->num_trpt_info * sizeof(ds_sys_throughput_info_type),
              temp_thrpt,
              sizeof(temp_thrpt->num_trpt_info) + \
              temp_thrpt->num_trpt_info * sizeof(ds_sys_throughput_info_type));

      break;
    }

    case PS_SYS_CONF_START_FAST_DORMANCY:
    {
      temp_timer_p = (ps_sys_fast_dormancy_timer_values_s*)argval_ptr;

      /* Allocate memory for event info ptr */
      timer_values_p = (ps_sys_fast_dormancy_timer_values_s*)
        ps_system_heap_mem_alloc(sizeof(ps_sys_fast_dormancy_timer_values_s));

      if (NULL == timer_values_p)
      {
        *ps_errno = DS_NOMEMORY;
        LOG_MSG_ERROR_0("ps_sys_conf_set_ex():PS_SYS_CONF_START_FAST_DORMANCY"
                        " out of memory ");
        return -1;
      }
      
      memset(timer_values_p, 0, sizeof(ps_sys_fast_dormancy_timer_values_s));
  
      /* copy  the timer values passed from MH */
      memscpy(timer_values_p,
              sizeof(ps_sys_fast_dormancy_timer_values_s),
              temp_timer_p,
              sizeof(ps_sys_fast_dormancy_timer_values_s));
   
      event_info = (void *)timer_values_p;
      event_name = PS_SYS_EVENT_START_FAST_DORMANCY;
      invoke_cb  = TRUE;
      
      break;
    }

    case PS_SYS_CONF_STOP_FAST_DORMANCY:
    { 
      /* Presently stop fast dormancy indication from MH doesnt carry any
         information with it.we may require to pass stop reason in future 
         as an enhancement  */    
      event_info = (void *)NULL;
      event_name = PS_SYS_EVENT_STOP_FAST_DORMANCY;
      invoke_cb  = TRUE;    
  
      break;
    }

    case PS_SYS_CONF_NULL_RESOLVED_APN: 
    { 
      /* 1. 3gpp, 3gpp, wlan mode handler update the null resolved apn name 
            with ps_sys.
         2. This is cached in technology specific cache when the set operation
            is called for specific tech type.
         3. EPC mode handler resolves the null apn by querying ps_sys for different 
            tech tyep and perform set operation with tech_all conf. 
         4. Event will be generated if the set operation is performed with
            tech_all configuration. 
      */
      if(tech_type == PS_SYS_TECH_ALL)
      {
        PS_SYSTEM_HEAP_MEM_ALLOC(null_resolved_apn_ptr,
               sizeof(ps_sys_null_resolved_apn_info_type),
               ps_sys_null_resolved_apn_info_type*);
  
        if (NULL == null_resolved_apn_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          goto bail;
        }
        
        memscpy(null_resolved_apn_ptr,
                sizeof(ps_sys_null_resolved_apn_info_type),
               (ps_sys_null_resolved_apn_info_type*)argval_ptr,
               sizeof(ps_sys_null_resolved_apn_info_type));
  
        event_info = (void *)null_resolved_apn_ptr;
        event_name = PS_SYS_EVENT_NULL_RESOLVED_APN;
        invoke_cb = TRUE;
      }
      /* Update the technology specific cache. Do not generate the event.
         EPC mode handler will resolve the NULL APN and the event will
         be generated */
      else
      {
        null_resolved_apn_ptr = (ps_sys_null_resolved_apn_info_type*)argval_ptr;
        invoke_cb = FALSE;

        if (tech_type == PS_SYS_TECH_3GPP)
        {
          memscpy(conf_tech_3gpp[subscription_id - 1].null_resolved_apn_ptr,
                  sizeof(ps_sys_null_resolved_apn_info_type),
                 (ps_sys_null_resolved_apn_info_type*)argval_ptr,
                 sizeof(ps_sys_null_resolved_apn_info_type));
        }
        else if (tech_type == PS_SYS_TECH_3GPP2)
        {
          memscpy(conf_tech_3gpp2[subscription_id - 1].null_resolved_apn_ptr,
                  sizeof(ps_sys_null_resolved_apn_info_type),
                 (ps_sys_null_resolved_apn_info_type*)argval_ptr,
                 sizeof(ps_sys_null_resolved_apn_info_type));
        }
        else if (tech_type == PS_SYS_TECH_WLAN)
        {
          memscpy(conf_tech_wlan[subscription_id - 1].null_resolved_apn_ptr,
                  sizeof(ps_sys_null_resolved_apn_info_type),
                 (ps_sys_null_resolved_apn_info_type*)argval_ptr,
                 sizeof(ps_sys_null_resolved_apn_info_type));
        }
      }

      LOG_MSG_INFO1_4("ps_sys_conf_set_ex : apn name length:%d, apn_name:%s,"
                      "invoke callback:%d, tech_type %d",                        
                      null_resolved_apn_ptr->apn_length,
                      ((null_resolved_apn_ptr->apn_name != NULL) ? 
                      null_resolved_apn_ptr->apn_name : 0),
                      invoke_cb, tech_type);
      break;
    }    

    case PS_SYS_CONF_3GPP2_QOS_NETWORK_STATUS:
    {

      /* Check the qos aware status stored in the cache and generate the event 
      only if network status has changed; Redundant conf set is ignored */

      ret_val = memcmp(conf_tech_3gpp2[subscription_id - 1].net_aware_status,
                       (ps_sys_3gpp2_network_qos_aware_status *)argval_ptr,
                       sizeof(ps_sys_3gpp2_network_qos_aware_status));

      if (ret_val != 0)
      {
          PS_SYSTEM_HEAP_MEM_ALLOC(net_aware_status,
                         sizeof(ps_sys_3gpp2_network_qos_aware_status),
                          ps_sys_3gpp2_network_qos_aware_status*);

        if (NULL == net_aware_status)
        {
          *ps_errno = DS_NOMEMORY;
          return -1;
        }
        
        memset( net_aware_status, 0, sizeof(ps_sys_3gpp2_network_qos_aware_status) );
        memscpy(net_aware_status, sizeof(ps_sys_3gpp2_network_qos_aware_status),
                (ps_sys_3gpp2_network_qos_aware_status *)argval_ptr,
                sizeof(ps_sys_3gpp2_network_qos_aware_status));

        LOG_MSG_INFO2_1("ps_sys_conf_set_ex : nw aware status  %d ",             
                         *net_aware_status);              

        event_info = (void *)net_aware_status;
        event_name = PS_SYS_EVENT_3GPP2_QOS_NETWORK_STATUS_CHANGE;
        invoke_cb = TRUE;

        /* Update temp storage */
        memscpy(conf_tech_3gpp2[subscription_id - 1].net_aware_status,
                sizeof(ps_sys_3gpp2_network_qos_aware_status),
                (ps_sys_3gpp2_network_qos_aware_status *)argval_ptr,
                sizeof(ps_sys_3gpp2_network_qos_aware_status));
      }

      break;
    }

    case  PS_SYS_CONF_DL_THROUGHPUT_INFO_TIMER:
    {
      PS_SYSTEM_HEAP_MEM_ALLOC(dl_thrpt_timer_ptr, sizeof(ps_sys_report_dl_timer_type),
                               ps_sys_report_dl_timer_type*);

      if (NULL == dl_thrpt_timer_ptr)
      {
        *ps_errno = DS_NOMEMORY;
        return -1;
      }
      memset(dl_thrpt_timer_ptr, 0, sizeof(ps_sys_report_dl_timer_type));

      memscpy(dl_thrpt_timer_ptr, 
              sizeof(ps_sys_report_dl_timer_type),
              (ps_sys_report_dl_timer_type *)argval_ptr,
              sizeof(ps_sys_report_dl_timer_type));

      LOG_MSG_INFO2_1("ps_sys_conf_set_ex :dl throughput timer  %d ",             
                       *dl_thrpt_timer_ptr);              

      event_info = (void *)dl_thrpt_timer_ptr;
      event_name = PS_SYS_EVENT_UPDATED_DL_THROUGHPUT_TIMER;
      invoke_cb = TRUE;

      /* Update temp storage */
      memscpy(conf_tech_all[subscription_id - 1].dl_thrpt_timer_ptr,
              sizeof(ps_sys_report_dl_timer_type),
              (ps_sys_report_dl_timer_type *)argval_ptr,
              sizeof(ps_sys_report_dl_timer_type));

      break;
    }

    case PS_SYS_CONF_DL_THROUGHPUT_INFO:
    {
      PS_SYSTEM_HEAP_MEM_ALLOC(dl_thrpt_info_ptr, sizeof(ps_sys_dl_throughput_info_type),
                               ps_sys_dl_throughput_info_type *);

      if (NULL == dl_thrpt_info_ptr)
      {
        *ps_errno = DS_NOMEMORY;
        return -1;
      }

      memset(dl_thrpt_info_ptr, 0, sizeof(ps_sys_dl_throughput_info_type));

      memscpy(dl_thrpt_info_ptr, 
              sizeof(ps_sys_dl_throughput_info_type),
              (ps_sys_dl_throughput_info_type *)argval_ptr,
              sizeof(ps_sys_dl_throughput_info_type));

      LOG_MSG_INFO2_2("ps_sys_conf_set_ex :dl thrpt allowed rate %d confidence %d",
                       dl_thrpt_info_ptr->downlink_allowed_rate,
                       dl_thrpt_info_ptr->confidence_level);              

      event_info = (void *)dl_thrpt_info_ptr;
      event_name = PS_SYS_EVENT_DL_THROUGHPUT_INFO;
      invoke_cb = TRUE;

      /* Update temp storage */
      memscpy(conf_tech_all[subscription_id - 1].dl_thrpt_info_ptr,
              sizeof(ps_sys_dl_throughput_info_type),
              (ps_sys_dl_throughput_info_type *)argval_ptr,
              sizeof(ps_sys_dl_throughput_info_type));

      break;
    }


    case  PS_SYS_CONF_DL_MIN_THROUGHPUT_TIMER:
    {
      /* Update temp storage */
      memscpy(conf_tech_all[subscription_id - 1].min_dl_thrpt_timer_ptr,
              sizeof(ps_sys_report_dl_timer_type),
              (ps_sys_report_dl_timer_type *)argval_ptr,
              sizeof(ps_sys_report_dl_timer_type));

      break;
    }

    case PS_SYS_CONF_THERMAL_MITIGATION_INFO:
    {

      /* Check the thermal mitigation action stored in the cache and generate the event 
      only if network status has changed; Redundant conf set is ignored */

      ret_val = memcmp(conf_tech_all[subscription_id - 1].thermal_info_ptr,
                       (ps_sys_thermal_mitigation_action_enum_type *)argval_ptr,
                       sizeof(ps_sys_thermal_mitigation_action_enum_type));

      if (ret_val != 0)
      {
          PS_SYSTEM_HEAP_MEM_ALLOC(thermal_info_ptr,
                         sizeof(ps_sys_thermal_mitigation_action_enum_type),
                          ps_sys_thermal_mitigation_action_enum_type*);

        if (NULL == thermal_info_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          return -1;
        }

        memset( thermal_info_ptr, 0, sizeof(ps_sys_thermal_mitigation_action_enum_type) );
        memscpy(thermal_info_ptr, sizeof(ps_sys_thermal_mitigation_action_enum_type),
                (ps_sys_thermal_mitigation_action_enum_type *)argval_ptr,
                sizeof(ps_sys_thermal_mitigation_action_enum_type));

        LOG_MSG_INFO2_1("ps_sys_conf_set_ex : thermal info action  %d ",             
                         *thermal_info_ptr);              

        event_info = (void *)thermal_info_ptr;
        event_name = PS_SYS_EVENT_THERMAL_MITIGATION_INFO;
        invoke_cb = TRUE;

        /* Update temp storage */
        memscpy(conf_tech_all[subscription_id - 1].thermal_info_ptr,
                sizeof(ps_sys_thermal_mitigation_action_enum_type),
                (ps_sys_thermal_mitigation_action_enum_type *)argval_ptr,
                sizeof(ps_sys_thermal_mitigation_action_enum_type));
      }

      break;
    }

    case PS_SYS_CONF_DATA_SERVICE_SETTING:
    {

      /* Check the data service setting stored in the cache and generate the event 
      only if setting has changed; Redundant conf set is ignored */

      ret_val = memcmp(conf_tech_all[subscription_id - 1].data_setting_ptr,
                       (ps_sys_data_services_setting_type *)argval_ptr,
                       sizeof(ps_sys_data_services_setting_type));

      if (ret_val != 0)
      {
          PS_SYSTEM_HEAP_MEM_ALLOC( data_setting_ptr,
                         sizeof(ps_sys_data_services_setting_type),
                         ps_sys_data_services_setting_type* );

        if (NULL == data_setting_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          return -1;
        }

        memset( data_setting_ptr, 0, sizeof(ps_sys_data_services_setting_type) );
        memscpy( data_setting_ptr, sizeof(ps_sys_data_services_setting_type),
                (ps_sys_data_services_setting_type *)argval_ptr,
                 sizeof(ps_sys_data_services_setting_type) );

        LOG_MSG_INFO2_1("ps_sys_conf_set_ex : data settings  %d ",             
                         *data_setting_ptr);              

        event_info = (void *)data_setting_ptr;
        event_name =  PS_SYS_EVENT_DATA_SERVICE_SETTING;
        invoke_cb = TRUE;

        /* Update temp storage */
        memscpy(conf_tech_all[subscription_id - 1].data_setting_ptr,
                sizeof(ps_sys_data_services_setting_type),
                (ps_sys_data_services_setting_type *)argval_ptr,
                sizeof(ps_sys_data_services_setting_type));

        ps_sys_conf_parser_update_efs_from_cache(subscription_id);
      }

      break;
    }

    case PS_SYS_CONF_DATA_SERVICE_ROAMING_SETTING:
    {

      /* Check the data roaming setting stored in the cache and generate the event 
      only if setting has changed; Redundant conf set is ignored */

      ret_val = memcmp(conf_tech_all[subscription_id - 1].data_roaming_setting_ptr,
                       (ps_sys_data_services_roaming_setting_type *)argval_ptr,
                       sizeof(ps_sys_data_services_roaming_setting_type));

      if (ret_val != 0)
      {
          PS_SYSTEM_HEAP_MEM_ALLOC( data_roaming_ptr,
                         sizeof(ps_sys_data_services_roaming_setting_type),
                         ps_sys_data_services_roaming_setting_type* );

        if (NULL == data_roaming_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          return -1;
        }

        memset( data_roaming_ptr, 0, sizeof(ps_sys_data_services_roaming_setting_type) );
        memscpy( data_roaming_ptr, sizeof(ps_sys_data_services_roaming_setting_type),
                (ps_sys_data_services_roaming_setting_type *)argval_ptr,
                 sizeof(ps_sys_data_services_roaming_setting_type) );

        LOG_MSG_INFO2_1("ps_sys_conf_set_ex : data roaming settings  %d ",             
                         *data_roaming_ptr);              

        event_info = (void *)data_roaming_ptr;
        event_name = PS_SYS_EVENT_DATA_SERVICE_ROAMING_SETTING;
        invoke_cb  = TRUE;

        /* Update temp storage */
        memscpy(conf_tech_all[subscription_id - 1].data_roaming_setting_ptr,
                sizeof(ps_sys_data_services_roaming_setting_type),
                (ps_sys_data_services_roaming_setting_type *)argval_ptr,
                sizeof(ps_sys_data_services_roaming_setting_type));
        ps_sys_conf_parser_update_efs_from_cache(subscription_id);
      }
      break;
    }

    case PS_SYS_CONF_APN_INFO:
    {
      temp_apn_info = (ps_sys_apn_params_type *)argval_ptr;
      /* if apn type passed by client is not valid , return */
      if( temp_apn_info->apn_type >= DS_SYS_APN_TYPE_MAX)
      {
        LOG_MSG_ERROR_1("apn type %d is not valid !", temp_apn_info->apn_type);
        *ps_errno = DS_EFAULT;
        return -1;
      } 
     /* update cache; copy old apn info and update curr info*/
      memscpy(conf_tech_all[subscription_id - 1].apn_info_ptr->prev_apn_info,
              DS_SYS_APN_TYPE_MAX * sizeof(ps_sys_apn_params_type),
              conf_tech_all[subscription_id - 1].apn_info_ptr->curr_apn_info,
              DS_SYS_APN_TYPE_MAX * sizeof(ps_sys_apn_params_type));

      memscpy( &((conf_tech_all[subscription_id - 1]).apn_info_ptr->curr_apn_info[temp_apn_info->apn_type]),
               sizeof(ps_sys_apn_params_type),
               temp_apn_info,
               sizeof(ps_sys_apn_params_type) );
      
      PS_SYSTEM_HEAP_MEM_ALLOC( apn_info_ptr,
                                sizeof(ps_sys_apn_info_type),
                                ps_sys_apn_info_type* );

      if (NULL == apn_info_ptr)
      {
        *ps_errno = DS_NOMEMORY;
        return -1;
      }

      memset( apn_info_ptr, 0, sizeof(ps_sys_apn_info_type) );

      /* Copy prev and curr info into apn info ptr */
      memscpy(  apn_info_ptr,
                sizeof(ps_sys_apn_info_type),
                conf_tech_all[subscription_id - 1].apn_info_ptr,
                sizeof(ps_sys_apn_info_type));

      event_info = (void *)apn_info_ptr;
      event_name =  PS_SYS_EVENT_APN_INFO;
      invoke_cb = TRUE;
      ps_sys_conf_parser_update_efs_from_cache(subscription_id);
      break;
    }

    case PS_SYS_CONF_MODEM_POWER_STATS_CONFIGURATION:
    {
      PS_SYSTEM_HEAP_MEM_ALLOC( power_stats_config_ptr,
                                sizeof(ps_sys_modem_power_stats_config_type),
                                ps_sys_modem_power_stats_config_type* );

      if (NULL == power_stats_config_ptr)
      {
        *ps_errno = DS_NOMEMORY;
        return -1;
      }

      memset(power_stats_config_ptr, 0, sizeof(ps_sys_modem_power_stats_config_type));
  
      memscpy(power_stats_config_ptr, 
              sizeof(ps_sys_modem_power_stats_config_type),
              (ps_sys_modem_power_stats_config_type *)argval_ptr,
              sizeof(ps_sys_modem_power_stats_config_type));
  
      event_info = (void *)power_stats_config_ptr;
      event_name =  PS_SYS_EVENT_MODEM_POWER_STATS_CONFIGURATION;
      invoke_cb = TRUE;

      /* Update temp storage */
      memscpy(conf_tech_all[subscription_id - 1].power_stats_config_ptr,
              sizeof(ps_sys_modem_power_stats_config_type),
              (ps_sys_modem_power_stats_config_type *)argval_ptr,
              sizeof(ps_sys_modem_power_stats_config_type));
      break;
    }

    case PS_SYS_CONF_QUERY_MODEM_POWER_STATS:
    { 
      event_info = (void *)NULL;
      event_name = PS_SYS_EVENT_QUERY_MODEM_POWER_STATS;
      invoke_cb  = TRUE;  

      break;
    }

    case PS_SYS_CONF_MODEM_POWER_STATISTICS:
    {
      PS_SYSTEM_HEAP_MEM_ALLOC( power_stats_info_ptr,
                                sizeof(ps_sys_modem_power_stats_info_type),
                                ps_sys_modem_power_stats_info_type* );

      if (NULL == power_stats_info_ptr)
      {
        *ps_errno = DS_NOMEMORY;
        return -1;
      }
  
      memset(power_stats_info_ptr, 0, sizeof(ps_sys_modem_power_stats_info_type));
  
      memscpy(power_stats_info_ptr, 
              sizeof(ps_sys_modem_power_stats_info_type),
              (ps_sys_modem_power_stats_info_type *)argval_ptr,
              sizeof(ps_sys_modem_power_stats_info_type));
  
      event_info = (void *)power_stats_info_ptr;
      event_name = PS_SYS_EVENT_MODEM_POWER_STATISTICS;
      invoke_cb = TRUE;

      /* Update temp storage */
      memscpy(conf_tech_all[subscription_id - 1].power_stats_info_ptr,
              sizeof(ps_sys_modem_power_stats_info_type),
              (ps_sys_modem_power_stats_info_type *)argval_ptr,
              sizeof(ps_sys_modem_power_stats_info_type));

      break;
    }
    case PS_SYS_CONF_DATA_SERVICE_WIFI_SETTING:
    {

      /* Check the wifi setting stored in the cache and generate the event 
      only if setting has changed; Redundant conf set is ignored */

      ret_val = memcmp(conf_tech_all[subscription_id - 1].wifi_settings_ptr,
                       (ps_sys_data_services_wifi_setting_type *)argval_ptr,
                       sizeof(ps_sys_data_services_wifi_setting_type));

      if (ret_val != 0)
      {
          PS_SYSTEM_HEAP_MEM_ALLOC( wifi_settings_ptr,
                         sizeof(ps_sys_data_services_wifi_setting_type),
                         ps_sys_data_services_wifi_setting_type* );

        if (NULL == wifi_settings_ptr)
        {
          *ps_errno = DS_NOMEMORY;
          return -1;
        }

        memset( wifi_settings_ptr, 0, sizeof(ps_sys_data_services_wifi_setting_type) );
        memscpy( wifi_settings_ptr, sizeof(ps_sys_data_services_wifi_setting_type),
                (ps_sys_data_services_wifi_setting_type *)argval_ptr,
                 sizeof(ps_sys_data_services_wifi_setting_type) );

        LOG_MSG_INFO2_1("ps_sys_conf_set_ex : wifi settings  %d ",             
                         *wifi_settings_ptr);              

        event_info = (void *)wifi_settings_ptr;
        event_name = PS_SYS_EVENT_WIFI_SETTINGS_CHANGE;
        invoke_cb  = TRUE;

        /* Update temp storage */
        memscpy(conf_tech_all[subscription_id - 1].wifi_settings_ptr,
                sizeof(ps_sys_data_services_wifi_setting_type),
                (ps_sys_data_services_wifi_setting_type *)argval_ptr,
                sizeof(ps_sys_data_services_wifi_setting_type));
        ps_sys_conf_parser_update_efs_from_cache(subscription_id);
      }
      break;
    }
    case PS_SYS_CONF_REMOTE_SOCKET_HDLR_AVAIL :
    {
      /* Check the value stored in the cache and generate the event 
      only if setting has changed; Redundant conf set is ignored */

      ret_val = memcmp(conf_tech_all[subscription_id - 1].hdlr_avail,
                       (ps_sys_remote_socket_hdlr_avail_type *)argval_ptr,
                       sizeof(ps_sys_remote_socket_hdlr_avail_type));

      if (ret_val != 0)
      {
          PS_SYSTEM_HEAP_MEM_ALLOC( hdlr_avail,
                         sizeof(ps_sys_remote_socket_hdlr_avail_type),
                         ps_sys_remote_socket_hdlr_avail_type* );

        if (NULL == hdlr_avail)
        {
          *ps_errno = DS_NOMEMORY;
          return -1;
        }

        memset( hdlr_avail, 0, sizeof(ps_sys_remote_socket_hdlr_avail_type) );
        memscpy( hdlr_avail, sizeof(ps_sys_remote_socket_hdlr_avail_type),
                (ps_sys_remote_socket_hdlr_avail_type *)argval_ptr,
                 sizeof(ps_sys_remote_socket_hdlr_avail_type) );

        LOG_MSG_INFO2_1("ps_sys_conf_set_ex : remote socket hdlr avail %d ",             
                         hdlr_avail->remote_socket_hdlr_avail);              

        event_info = (void *)hdlr_avail;
        event_name =  PS_SYS_EVENT_REMOTE_SOCKET_HDLR_CHANGE;
        invoke_cb = TRUE;

        /* Update temp storage */
        memscpy(conf_tech_all[subscription_id - 1].hdlr_avail,
                sizeof(ps_sys_remote_socket_hdlr_avail_type),
                (ps_sys_remote_socket_hdlr_avail_type *)argval_ptr,
                sizeof(ps_sys_remote_socket_hdlr_avail_type));

      }

      break;
    }
    
    case PS_SYS_CONF_ROAMING_STATUS_INFO:
    {
      ret_val = memcmp(conf_tech_all[subscription_id - 1].roaming_status_info_ptr,
                       (ps_sys_roaming_status_info_type *)argval_ptr,
                       sizeof(ps_sys_roaming_status_info_type));

      if ( ret_val != 0)
      {
         PS_SYSTEM_HEAP_MEM_ALLOC( roaming_status_info_ptr,
                                   sizeof(ps_sys_roaming_status_info_type),
                                   ps_sys_roaming_status_info_type* );

         if (NULL == roaming_status_info_ptr)
         {
           *ps_errno = DS_NOMEMORY;
           return -1;
         }

         memset(roaming_status_info_ptr, 0, sizeof(ps_sys_roaming_status_info_type));
         memscpy(roaming_status_info_ptr,
                 sizeof(ps_sys_roaming_status_info_type),
                 (ps_sys_roaming_status_info_type *)argval_ptr,
                 sizeof(ps_sys_roaming_status_info_type));

         event_info = (void *)roaming_status_info_ptr;
         event_name = PS_SYS_EVENT_ROAMING_STATUS_CHANGE;
         invoke_cb = TRUE;

          /* Update temp storage */
         memscpy(conf_tech_all[subscription_id - 1].roaming_status_info_ptr, 
                 sizeof(ps_sys_roaming_status_info_type),
                 roaming_status_info_ptr, 
                 sizeof(ps_sys_roaming_status_info_type));

      }
      break;
    }

    default:
    {
      *ps_errno = DS_EINVAL;
      LOG_MSG_ERROR_1("ps_sys_conf_set_ex(): Conf %d not found", conf_name);
      goto bail;
    }
  }
  PS_LEAVE_CRIT_SECTION(&ps_sys_crit_section);

  /*-------------------------------------------------------------------------
    Schedule PS task to handle the callbacks. In future, it needs
     to be called after we set the value in xml file
  -------------------------------------------------------------------------*/
  if (invoke_cb ==  TRUE)
  {
     PS_SYSTEM_HEAP_MEM_ALLOC(cmd_data_info, sizeof(ps_sys_eventi_cmd_type),
                              ps_sys_eventi_cmd_type*);

    if (NULL == cmd_data_info)
    {
      *ps_errno = DS_NOMEMORY;
      return -1;
    }

    cmd_data_info->event      = event_name;
    cmd_data_info->tech_type  = tech_type;
    cmd_data_info->event_info = event_info;
    cmd_data_info->subscription_id = subscription_id;

    ps_send_cmd(PS_SYS_EVENT_CMD_HANDLER,(void *)cmd_data_info);
  }

  return 0;

bail:
  PS_LEAVE_CRIT_SECTION(&ps_sys_crit_section);
  return -1;
} /* ps_sys_conf_set_ex() */


void ps_sys_confi_alloc_temp_data
(
  void
)
{
  int subs_id_itr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_0("ps_sys_confi_alloc_temp_data() ");

  for (subs_id_itr = 0; subs_id_itr < DS_SYS_SUBS_MAX - 1; subs_id_itr++)
  {
    /* System Status Struct*/
   PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].system_status_type, 
                           sizeof(ps_sys_system_status_type),
                           ps_sys_system_status_type*);
      
    if (NULL == conf_tech_all[subs_id_itr].system_status_type)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].system_status_type,
           0,
           sizeof(ps_sys_system_status_type));

    /* Network Status Struct*/
    conf_tech_all[subs_id_itr].system_status_ex_ptr = NULL;                 

    /* s101 status info type structure*/
     PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_3gpp2[subs_id_itr].s101_status_change_type, 
                              sizeof(ps_sys_3gpp2_s101_status_change_type),
                              ps_sys_3gpp2_s101_status_change_type*);

    if (NULL == conf_tech_3gpp2[subs_id_itr].s101_status_change_type)
    {
      goto bail;
    }

    memset(conf_tech_3gpp2[subs_id_itr].s101_status_change_type,
           0,
           sizeof(ps_sys_3gpp2_s101_status_change_type));

    /* Epzid info type*/
     PS_SYSTEM_HEAP_MEM_ALLOC( conf_tech_3gpp2[subs_id_itr].epzid_info_type,
                               sizeof(ps_sys_3gpp2_epzid_info_type),
                               ps_sys_3gpp2_epzid_info_type*);
      
    if (NULL == conf_tech_3gpp2[subs_id_itr].epzid_info_type)
    {
      goto bail;
    }

    memset(conf_tech_3gpp2[subs_id_itr].epzid_info_type,
           0,
           sizeof(ps_sys_3gpp2_epzid_info_type));

    /* AN auth status type*/
     PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_3gpp2[subs_id_itr].an_auth_status,
                              sizeof(ps_sys_conf_3gpp2_an_auth_status_enum_type),
                              ps_sys_conf_3gpp2_an_auth_status_enum_type*);
      
    if (NULL == conf_tech_3gpp2[subs_id_itr].an_auth_status)
    {
      goto bail;
    }

    memset(conf_tech_3gpp2[subs_id_itr].an_auth_status,
           0,
           sizeof(ps_sys_conf_3gpp2_an_auth_status_enum_type));

    /* Enable Loopback  type*/
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].enable_loopback_ptr,
                             sizeof(ps_sys_conf_enable_loopback_type),
                             ps_sys_conf_enable_loopback_type*);
      
    if (NULL == conf_tech_all[subs_id_itr].enable_loopback_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].enable_loopback_ptr,
           0,
           sizeof(ps_sys_conf_enable_loopback_type));

    /* Dun Ctrl pref */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].get_dun_ctrl_pref_ptr, 
                             sizeof(ps_sys_conf_get_dun_ctrl_pref_type),
                             ps_sys_conf_get_dun_ctrl_pref_type*);
      
    if (NULL == conf_tech_all[subs_id_itr].get_dun_ctrl_pref_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].get_dun_ctrl_pref_ptr,
           0,
           sizeof(ps_sys_conf_get_dun_ctrl_pref_type));    
            
    /* LTE Attach Type*/
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_3gpp[subs_id_itr].lte_attach_type,
                             sizeof(ps_sys_lte_attach_enum_type),
                             ps_sys_lte_attach_enum_type*);
    
    if (NULL == conf_tech_3gpp[subs_id_itr].lte_attach_type)
    {
      goto bail;
    }

    memset(conf_tech_3gpp[subs_id_itr].lte_attach_type,
           0,
           sizeof(ps_sys_lte_attach_enum_type));   
            
    /* Emergency Pdn only Type*/
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_3gpp[subs_id_itr].only_emergency_pdn_info,
                             sizeof(ps_sys_is_pdn_only_emergency_type),
                             ps_sys_is_pdn_only_emergency_type*);
    
    if (NULL == conf_tech_3gpp[subs_id_itr].only_emergency_pdn_info)
    {
      goto bail;
    }

    memset(conf_tech_3gpp[subs_id_itr].only_emergency_pdn_info,
           0,
           sizeof(ps_sys_is_pdn_only_emergency_type));   

    /* WLAN status struct*/
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_wlan[subs_id_itr].wlan_status,
                             sizeof(ps_sys_wlan_status_type),
                             ps_sys_wlan_status_type*);
    
    if (NULL == conf_tech_wlan[subs_id_itr].wlan_status)
    {
      goto bail;
    }

    memset(conf_tech_wlan[subs_id_itr].wlan_status,
           0,
           sizeof(ps_sys_wlan_status_type));

    /* WLAN preference */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_wlan[subs_id_itr].wlan_preference,
                             sizeof(ps_sys_wlan_preference_enum_type),
                             ps_sys_wlan_preference_enum_type*);
    
    if (NULL == conf_tech_wlan[subs_id_itr].wlan_preference)
    {
      goto bail;
    }

    memset(conf_tech_wlan[subs_id_itr].wlan_preference,
           0,
           sizeof(ps_sys_wlan_preference_enum_type)); /* This will set the default as WLAN_LB_PREFERRED */

    /* Apn Pref Sys */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].apn_pref_sys_ptr,
                             sizeof(ps_sys_apn_pref_sys_type),
                             ps_sys_apn_pref_sys_type*);
      
    if (NULL == conf_tech_all[subs_id_itr].apn_pref_sys_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].apn_pref_sys_ptr,
           0,
           sizeof(ps_sys_apn_pref_sys_type));               

    /* WLAN MAX CONN */
    PS_SYSTEM_HEAP_MEM_ALLOC( conf_tech_wlan[subs_id_itr].wlan_max_conn_ptr, 
                              sizeof(ps_sys_wlan_max_conn_info_type),
                              ps_sys_wlan_max_conn_info_type*);
    
    if (NULL == conf_tech_wlan[subs_id_itr].wlan_max_conn_ptr)
    {
      goto bail;
    }

    memset(conf_tech_wlan[subs_id_itr].wlan_max_conn_ptr,
           0,
           sizeof(ps_sys_wlan_max_conn_info_type)); 
              
    
    /* Throughput info timer */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].throughput_timer_ptr,
                             sizeof(ps_sys_conf_throughput_timer_type),
                             ps_sys_conf_throughput_timer_type*);
    
    if (NULL == conf_tech_all[subs_id_itr].throughput_timer_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].throughput_timer_ptr,
           0,
           sizeof(ps_sys_conf_throughput_timer_type)); 

    /* Throughput info ptr */
    conf_tech_all[subs_id_itr].throughput_info_ptr = NULL;  

    /* Roaming disallowed info */
    conf_tech_3gpp[subs_id_itr].roaming_disallowed_info_ptr = NULL;

    /* WLAN MAC address*/
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_wlan[subs_id_itr].wlan_mac_addr_ptr, 
                             sizeof(ps_sys_wlan_mac_addr_type),
                             ps_sys_wlan_mac_addr_type*);
    
    if (NULL == conf_tech_wlan[subs_id_itr].wlan_mac_addr_ptr)
    {
      goto bail;
    }

    memset(conf_tech_wlan[subs_id_itr].wlan_mac_addr_ptr,
           0,
           sizeof(ps_sys_wlan_mac_addr_type)); 

    /* WLAN SSID*/
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_wlan[subs_id_itr].wlan_ssid_ptr,
                             sizeof(ps_sys_wlan_ssid_type),
                             ps_sys_wlan_ssid_type*);
    
    if (NULL == conf_tech_wlan[subs_id_itr].wlan_ssid_ptr)
    {
      goto bail;
    }

    memset(conf_tech_wlan[subs_id_itr].wlan_ssid_ptr,
           0,
           sizeof(ps_sys_wlan_ssid_type)); 

    /* Null resolved APN */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_3gpp[subs_id_itr].null_resolved_apn_ptr,
              sizeof(ps_sys_null_resolved_apn_info_type),
              ps_sys_null_resolved_apn_info_type*);
      
    if (NULL == conf_tech_3gpp[subs_id_itr].null_resolved_apn_ptr)
    {
      goto bail;
    }

    memset(conf_tech_3gpp[subs_id_itr].null_resolved_apn_ptr,
           0,
           sizeof(ps_sys_null_resolved_apn_info_type));

    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_3gpp2[subs_id_itr].null_resolved_apn_ptr,
              sizeof(ps_sys_null_resolved_apn_info_type),
              ps_sys_null_resolved_apn_info_type*);
      
    if (NULL == conf_tech_3gpp2[subs_id_itr].null_resolved_apn_ptr)
    {
      goto bail;
    }

    memset(conf_tech_3gpp2[subs_id_itr].null_resolved_apn_ptr,
           0,
           sizeof(ps_sys_null_resolved_apn_info_type));

    PS_SYSTEM_HEAP_MEM_ALLOC( conf_tech_wlan[subs_id_itr].null_resolved_apn_ptr,
                sizeof(ps_sys_null_resolved_apn_info_type),
                ps_sys_null_resolved_apn_info_type*);
      
    if (NULL == conf_tech_wlan[subs_id_itr].null_resolved_apn_ptr)
    {
      goto bail;
    }

    memset(conf_tech_wlan[subs_id_itr].null_resolved_apn_ptr,
           0,
           sizeof(ps_sys_null_resolved_apn_info_type));  

    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_3gpp2[subs_id_itr].net_aware_status, 
                             sizeof(ps_sys_3gpp2_network_qos_aware_status),
                             ps_sys_3gpp2_network_qos_aware_status*);

    if (NULL == conf_tech_3gpp2[subs_id_itr].net_aware_status)
    {
      goto bail;
    }

    memset(conf_tech_3gpp2[subs_id_itr].net_aware_status,
           0,
           sizeof(ps_sys_3gpp2_network_qos_aware_status)); 

    /* DL Throughput info timer */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].dl_thrpt_timer_ptr,
                             sizeof(ps_sys_report_dl_timer_type),
                             ps_sys_report_dl_timer_type *);
    
    if (NULL == conf_tech_all[subs_id_itr].dl_thrpt_timer_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].dl_thrpt_timer_ptr,
           0,
           sizeof(ps_sys_report_dl_timer_type)); 

    /* DL Throughput info */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].dl_thrpt_info_ptr,
                             sizeof(ps_sys_dl_throughput_info_type),
                             ps_sys_dl_throughput_info_type *);
    
    if (NULL == conf_tech_all[subs_id_itr].dl_thrpt_info_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].dl_thrpt_info_ptr,
           0,
           sizeof(ps_sys_dl_throughput_info_type)); 

    /* Min DL Throughput info timer */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].min_dl_thrpt_timer_ptr,
                             sizeof(ps_sys_report_dl_timer_type),
                             ps_sys_report_dl_timer_type *);
    
    if (NULL == conf_tech_all[subs_id_itr].min_dl_thrpt_timer_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].min_dl_thrpt_timer_ptr,
           0,
           sizeof(ps_sys_report_dl_timer_type)); 

    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].thermal_info_ptr, 
                             sizeof(ps_sys_thermal_mitigation_action_enum_type),
                             ps_sys_thermal_mitigation_action_enum_type*);

    if (NULL == conf_tech_all[subs_id_itr].thermal_info_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].thermal_info_ptr,
           0,
           sizeof(ps_sys_thermal_mitigation_action_enum_type)); 


    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].data_setting_ptr,
                             sizeof(ps_sys_data_services_setting_type),
                             ps_sys_data_services_setting_type*);

    if (NULL == conf_tech_all[subs_id_itr].data_setting_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].data_setting_ptr,
           1,
           sizeof(ps_sys_data_services_setting_type)); 

    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].data_roaming_setting_ptr,
                             sizeof(ps_sys_data_services_roaming_setting_type),
                             ps_sys_data_services_roaming_setting_type*);

    if (NULL == conf_tech_all[subs_id_itr].data_roaming_setting_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].data_roaming_setting_ptr,
           1,
           sizeof(ps_sys_data_services_roaming_setting_type)); 

    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].apn_info_ptr,
                             sizeof(ps_sys_apn_info_type),
                             ps_sys_apn_info_type*);

    if (NULL == conf_tech_all[subs_id_itr].apn_info_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].apn_info_ptr,
           0,
           sizeof(ps_sys_apn_info_type)); 


    /* power stats config */

    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].power_stats_config_ptr,
                             sizeof(ps_sys_modem_power_stats_config_type),
                             ps_sys_modem_power_stats_config_type*);
   
    if (NULL == conf_tech_all[subs_id_itr].power_stats_config_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].power_stats_config_ptr,
           0,
           sizeof(ps_sys_modem_power_stats_config_type)); 

    /* power stats info */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].power_stats_info_ptr,
                             sizeof(ps_sys_modem_power_stats_info_type),
                             ps_sys_modem_power_stats_info_type*);
    
    if (NULL == conf_tech_all[subs_id_itr].power_stats_info_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].power_stats_info_ptr,
           0,
           sizeof(ps_sys_modem_power_stats_info_type)); 
    /* Remote Socket Handler */

    /* wifi switch */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].wifi_settings_ptr,
                             sizeof(ps_sys_data_services_wifi_setting_type),
                             ps_sys_data_services_wifi_setting_type*);

    if (NULL == conf_tech_all[subs_id_itr].wifi_settings_ptr)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].wifi_settings_ptr,
           1,
           sizeof(ps_sys_data_services_wifi_setting_type)); 
  
  /* Remote Socket Handler */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].hdlr_avail,
                             sizeof(ps_sys_remote_socket_hdlr_avail_type),
                             ps_sys_remote_socket_hdlr_avail_type*);
      
    if (NULL == conf_tech_all[subs_id_itr].hdlr_avail)
    {
      goto bail;
    }

    memset(conf_tech_all[subs_id_itr].hdlr_avail,
           0,
           sizeof(ps_sys_remote_socket_hdlr_avail_type));

 /* Roaming status info */
    PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all[subs_id_itr].roaming_status_info_ptr,
                             sizeof(ps_sys_roaming_status_info_type),
                             ps_sys_roaming_status_info_type*);

    if (NULL == conf_tech_all[subs_id_itr].roaming_status_info_ptr)
    {
      goto bail;
    }
    memset(conf_tech_all[subs_id_itr].roaming_status_info_ptr,
           0,
           sizeof(ps_sys_roaming_status_info_type));   

    /* Setting it to DS_SYS_NETWORK_MAX, it means roaming status is not updated by
       any technology handler */
    conf_tech_all[subs_id_itr].roaming_status_info_ptr->technology = DS_SYS_NETWORK_MAX;         
  }
  
  return;

bail:
  DATA_ERR_FATAL("ps_sys_confi_alloc_temp_data() : Mem alloc failed");
  return;
} /* ps_sys_confi_alloc_temp_data() */

int ps_sys_confi_verify_tech_conf_name
(
  ps_sys_tech_enum_type          tech_type,
  ps_sys_conf_enum_type          conf_name
)
{
  LOG_MSG_FUNCTION_ENTRY_2("ps_sys_confi_verify_tech_conf_name() : tech %d "
                           "conf %d", tech_type, conf_name);

  if (conf_name >= PS_SYS_CONF_3GPP_CONF_MIN && 
      conf_name < PS_SYS_CONF_3GPP_INTERNAL_MAX)
  {
    if(tech_type != PS_SYS_TECH_3GPP)
    {
      LOG_MSG_INVALID_INPUT_1("ps_sys_confi_verify_tech_conf_name(): "
                              "3gpp conf %d ", conf_name);
      return -1;
    }
    return 0;
  }

  if (conf_name >= PS_SYS_CONF_3GPP2_CONF_MIN && 
      conf_name < PS_SYS_CONF_3GPP2_INTERNAL_MAX)
  {
    if(tech_type != PS_SYS_TECH_3GPP2)
    {
      LOG_MSG_INVALID_INPUT_1("ps_sys_confi_verify_tech_conf_name(): "
                              "3gpp2 conf %d", conf_name);
      return -1;
    }
    return 0;
  }

  if (conf_name >= PS_SYS_CONF_WLAN_MIN && 
      conf_name < PS_SYS_CONF_WLAN_INTERNAL_MAX)
  {
    if(tech_type != PS_SYS_TECH_WLAN)
    {
      LOG_MSG_INVALID_INPUT_1("ps_sys_confi_verify_tech_conf_name(): "
                              "wlan conf %d", conf_name);
      return -1;
    }
    return 0;
  }

  if (conf_name >= PS_SYS_CONF_COMMON_CONF_MIN && 
      conf_name < PS_SYS_CONF_COMMON_INTERNAL_MAX)
  {
    if(conf_name == PS_SYS_CONF_SYSTEM_STATUS)
    {
      if(tech_type != PS_SYS_TECH_ALL)
      {
        LOG_MSG_INVALID_INPUT_1("ps_sys_confi_verify_tech_conf_name(): "
                                "common conf %d ", conf_name);
        return -1;
      }      
    }   
    return 0;
  }

  LOG_MSG_INVALID_INPUT_1("ps_sys_confi_verify_tech_conf_name(): "
                          "conf %d invalid", conf_name);

  return -1;
} /* ps_sys_confi_verify_tech_conf_name() */

void ps_sys_conf_set_default_values(uint32 subs_id)
{
   ps_sys_conf_parser_efs_items_type  conf_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 for (conf_item  = PS_SYS_CONF_PARSER_EFS_MIN; conf_item < PS_SYS_CONF_PARSER_EFS_MAX; conf_item++)
 {
   switch (conf_item)
   {
     case PS_SYS_CONF_PARSER_EFS_DATA_SERVICE_SETTING:
       *(conf_tech_all[subs_id-1].data_setting_ptr) =  TRUE;
       break;
     case PS_SYS_CONF_PARSER_EFS_DATA_SERVICE_ROAMING_SETTING:
       *(conf_tech_all[subs_id-1].data_roaming_setting_ptr) =  TRUE;
       break;
     case PS_SYS_CONF_PARSER_EFS_APN_INFO:
       break;
    case PS_SYS_CONF_PARSER_EFS_WIFI_SETTINGS:
       *(conf_tech_all[subs_id-1].wifi_settings_ptr) =  TRUE;
       break;
     default:
       LOG_MSG_ERROR_1("Conf %d not found !", conf_item);
       break;
   }/* switch*/
 }/* for conf_item*/
}/* ps_sys_conf_set_default_values */

void ps_sys_conf_update_apn_info_cache(char *param_val, uint32 subs_id)
{
  uint8  num_apn;
  uint8  apn_type;
  uint8  apn_name_index = 0;
  uint8  apn_info_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  num_apn = *param_val - '0';
  if ( num_apn > DS_SYS_APN_TYPE_MAX )
  {
    LOG_MSG_ERROR_1("Num of apn exceeded %d; Stop parsing !", num_apn);
    return;
  }
  /*-----------------------------------------------------------------            
    The parser expects APN info in the following format:
    num_apn,apn_type,apn_name. For e.g 2,0,"att",1,"vzw"
  -----------------------------------------------------------------*/
  param_val = param_val + 2;
  for ( apn_info_index = 0; apn_info_index < num_apn; apn_info_index++ )
  {
    apn_name_index = 0;
    apn_type = *param_val - '0';
    if( apn_type >= DS_SYS_APN_TYPE_MAX)
    {
      LOG_MSG_ERROR_1("apn type %d is not valid; Stop parsing !", apn_type);
      return;
    } 
    conf_tech_all[subs_id - 1].apn_info_ptr->curr_apn_info[apn_type].apn_type =
      (ds_sys_apn_enum_type)apn_type;
    conf_tech_all[subs_id - 1].apn_info_ptr->curr_apn_info[apn_type].is_configured = TRUE;
    param_val = param_val + 2;

    while (*param_val != '\0')
    {
      if (*param_val == ',')
      {
        //Null apn or end of string
        param_val++;
        break;
      }
      if( apn_name_index >= DS_SYS_MAX_APN_LEN )
      {
        LOG_MSG_INFO2_0("apn length is too long; Stop parsing !");
        return;
      }
      conf_tech_all[subs_id - 1].apn_info_ptr->curr_apn_info[apn_type].apn_name[apn_name_index++] =
      (char)(*param_val);
      param_val++;
    }
  }/* end for */
}/* ps_sys_conf_update_apn_info_cache */

void ps_sys_conf_write_apn_info_param
(
  char            *param_val,
  uint32           subs_id,
  int              avail_buff_size 
)
{
  int num_apn = 0;
  ds_sys_apn_enum_type    apn_type = 0;
  int                     apn_length = 0;
  int                     buff_index = 0;
  int                     apn_info_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  buff_index += 2;
  avail_buff_size = avail_buff_size - 2;
  /*-----------------------------------------------------------------            
    Loop thro all the apns and write the buffer in the format:
    num_apn,apn_type,apn_name; For e.g the buffer can contain,
    2,0,"att",1,"vzw"
  -----------------------------------------------------------------*/

  for ( apn_info_index = 0; apn_info_index < DS_SYS_APN_TYPE_MAX; apn_info_index++ )
  {
    apn_type = conf_tech_all[subs_id - 1].apn_info_ptr->curr_apn_info[apn_info_index].apn_type;
    if(conf_tech_all[subs_id - 1].apn_info_ptr->curr_apn_info[apn_info_index].is_configured)
    {
      num_apn++;
    }
    else
    {
      //APN not configured; Skip writing to file
      continue;
    }

    //Writing APN type
    param_val[buff_index] = (char)(apn_type + '0');
    param_val[buff_index+1] = ',';
    buff_index += 2;
    avail_buff_size -=2;
  
    //Writing APN name 
    apn_length =
      strlen(conf_tech_all[subs_id - 1].apn_info_ptr->curr_apn_info[apn_type].apn_name);

    strlcpy(&param_val[buff_index], conf_tech_all[subs_id - 1].apn_info_ptr->curr_apn_info[apn_type].apn_name,
            avail_buff_size);
    
    avail_buff_size = avail_buff_size - apn_length;
    buff_index += apn_length;
    param_val[buff_index++] = ',';
    avail_buff_size--;
  }/* end for */

  if (num_apn > 0)
  {
    param_val[0] = (char)(num_apn + '0');
    param_val[1] = ',';
  }
  else
  {
    buff_index = 0;
  }
  param_val[buff_index] = '\0';
}/* ps_sys_conf_write_apn_info_param */

int ps_sys_conf_write_conf_items_buffer
(
  char        *file_buffer,
  int         *total_bytes,
  uint32       subs_id
)
{
  ps_sys_conf_parser_efs_items_type  conf_item;
  char *param_val = NULL;
  int  num_bytes = 0;
  int curr_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_SYSTEM_HEAP_MEM_ALLOC(param_val,PS_SYS_CONF_PARSER_MAX_FILE_BUFFER_SIZE,
                           char* );

  if (param_val == NULL)
  {
    LOG_MSG_ERROR_0 ("ps_sys_conf_write_conf_items_buffer(): "
                     "No memory for param val");
    return -1;
  } 

  for (conf_item  = PS_SYS_CONF_PARSER_EFS_MIN; conf_item < PS_SYS_CONF_PARSER_EFS_MAX; conf_item++)
  {
    memset(param_val, 0, PS_SYS_CONF_PARSER_MAX_FILE_BUFFER_SIZE);
    num_bytes = 0;
    switch (conf_item)
    {
      case PS_SYS_CONF_PARSER_EFS_DATA_SERVICE_SETTING:
      {
        curr_val =  (TRUE == *(conf_tech_all[subs_id-1].data_setting_ptr)) ? 1 : 0;
       *param_val = (char)curr_val + '0';
       *(param_val +1) = '\0';
        break;
      }
      case PS_SYS_CONF_PARSER_EFS_DATA_SERVICE_ROAMING_SETTING:
      {
        curr_val = (TRUE == *(conf_tech_all[subs_id - 1].data_roaming_setting_ptr)) ? 1 : 0;
       * param_val = (char)curr_val + '0';
       *(param_val +1) = '\0';
        break;
      }
      case PS_SYS_CONF_PARSER_EFS_APN_INFO:
        ps_sys_conf_write_apn_info_param( param_val, subs_id,
                                          PS_SYS_CONF_PARSER_MAX_FILE_BUFFER_SIZE - *total_bytes);
        break;
      case PS_SYS_CONF_PARSER_EFS_WIFI_SETTINGS:
      {
        curr_val = *(conf_tech_all[subs_id - 1].wifi_settings_ptr);
       * param_val = (char)curr_val + '0';
       *(param_val + 1) = '\0';
        break;
      }
      default:
        LOG_MSG_ERROR_1("Conf %d not found !", conf_item);
        break;
    }/* switch*/

   /* write conf_item:conf_value; in the buffer */
    num_bytes = snprintf(file_buffer + *total_bytes,
                         PS_SYS_CONF_PARSER_MAX_FILE_BUFFER_SIZE,
                         "%d:%s;\n",
                         conf_item,
                         param_val);

    *total_bytes += num_bytes;
  }/* for conf_item*/

  PS_SYSTEM_HEAP_MEM_FREE(param_val);
  return 0;
}/* ps_sys_conf_write_conf_items_buffer */

int ps_sys_conf_update_cache_from_efs
( 
  int       conf_index, 
  char     *param_val,
  uint32    subs_id
)
{
  int                       err_no = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == param_val)
  {
    LOG_MSG_ERROR_0 ("ps_sys_conf_update_cache_from_efs(): "
                     "Input parameters are NULL");
    return -1;
  }

  if (conf_index < PS_SYS_CONF_PARSER_EFS_MIN || conf_index >= PS_SYS_CONF_PARSER_EFS_MAX)
  {
    LOG_MSG_ERROR_0 ("ps_sys_conf_update_cache_from_efs(): "
                     "Invalid conf update request");
    return -1;
  }

  // Update cache with value
  switch (conf_index) 
  {
    case PS_SYS_CONF_PARSER_EFS_DATA_SERVICE_SETTING:
      *(conf_tech_all[subs_id - 1].data_setting_ptr) = *param_val - '0';
      break;
    case PS_SYS_CONF_PARSER_EFS_DATA_SERVICE_ROAMING_SETTING:
      *(conf_tech_all[subs_id - 1].data_roaming_setting_ptr) = *param_val - '0';
      break;
    case PS_SYS_CONF_PARSER_EFS_APN_INFO:
      ps_sys_conf_update_apn_info_cache(param_val, subs_id);
      break;
    case PS_SYS_CONF_PARSER_EFS_WIFI_SETTINGS:
      *(conf_tech_all[subs_id - 1].wifi_settings_ptr) = *param_val - '0';
      break;
    default:
      LOG_MSG_ERROR_1("Conf %d not found !", conf_index);
      err_no = -1;
      break;
  }/* switch */

  if (err_no == -1)
  {
    LOG_MSG_ERROR_0 ("ps_sys_conf_update_cache_from_efs(): "
                     "Invalid conf update request");
    return -1;
  }

  return 0;
}/* ps_sys_conf_update_cache_from_efs */



