/**
@file ps_sys_conf_parser.h
@brief
This file defines functions, enums and structs used to read and 
update efs parsing for technology configurations 
*/

#ifndef PS_SYS_CONF_PARSER_H
#define PS_SYS_CONF_PARSER_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S _ C O N F_ P A R S E R . H


GENERAL DESCRIPTION
  This file defines functions, enums and structs used to read and update
  technology configurations

Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/inc/ps_sys_conf_parser.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/06/14    svj    Created module 
 
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ps_svc.h"
//#include "ps_sys.h"
//#include "ds_sys_conf.h"
//#include "ps_sys_event.h"


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/
/**
  @brief Definition of various interface ctrl operations. Please do NOT
         change the values of the existing enumerated types when adding
         new ones (to ensure that object code released with previous
         versions of the code do not break). 
*/

#define PS_SYS_CONF_PARSER_DATA_SERVICES_SETTINGS   "/data/ps_sys_data_configurations.txt"

#define PS_SYS_CONF_PARSER_MAX_NAME_STRING  255
#define PS_SYS_CONF_PARSER_MAX_FILE_BUFFER_SIZE  512 //Increase as per needed

/**
  @brief Enum for the connfigurations that are present in EFS 
         file
*/
typedef enum
{
   PS_SYS_CONF_PARSER_EFS_MIN = 0x00,
   PS_SYS_CONF_PARSER_EFS_DATA_SERVICE_SETTING = PS_SYS_CONF_PARSER_EFS_MIN,
   PS_SYS_CONF_PARSER_EFS_DATA_SERVICE_ROAMING_SETTING,
   PS_SYS_CONF_PARSER_EFS_APN_INFO,
   PS_SYS_CONF_PARSER_EFS_WIFI_SETTINGS,
   PS_SYS_CONF_PARSER_EFS_MAX                
}ps_sys_conf_parser_efs_items_type;

/**
  @brief  This function updates the EFS config file from the 
          configuration cache for the given subscription. The
          file stores all the configurations that need to be
          persistent. Format of the EFS file:conf_item1:
          value1;conf_item2:value2; 
          Sample config file:
          /data/ps_sys_data_configurations_SUB1.txt ;
         file contents: 0:0;1:0;2:1,0,"ims";
   
  @param[in] subs_id     Subs id of the config file to be 
                         updated
*/
void ps_sys_conf_parser_update_efs_from_cache
(
  uint32             subs_id
);
/**
  @brief  This function updated the configuration cache 
*/
void ps_sys_conf_parser_update_cfg_cache
(
  void
);
#ifdef __cplusplus
}
#endif

#endif /* PS_SYS_CONF_H */
