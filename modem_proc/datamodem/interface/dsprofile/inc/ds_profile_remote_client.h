#ifndef DS_QMI_WDS_QCCI_CLIENT_H
#define DS_QMI_WDS_QCCI_CLIENT_H

/**
@file ds_profile_remote_client.h
@brief
This file defines function, variables and data structures

@copyright (c)2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/inc/ds_profile_remote_client.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ds_profile.h"
#include "dcc_task_defs.h"
#include "wireless_data_service_v01.h"

#define DS_PROFILE_REMOTE_NAME_LEN_MAX 50
#define DS_PROFILE_REMOTE_NUM_PROFILE_MAX QMI_WDS_PROFILE_LIST_MAX_V01


/**
  @brief Data Type describing profile information
*/
typedef struct {

  uint8 profile_type;
  /**<   Identifies the technology type of the profile. Values: \n
       - 0 -- 3GPP \n
       - 1 -- 3GPP2
    */

  uint8 profile_index;
  /**<   Profile number identifying the profile. */

  char profile_name[DS_PROFILE_REMOTE_NAME_LEN_MAX + 1];
  /**<   Profile name of the profile
   */
} ds_profile_remote_profile_info_type;

/**
  @brief Data type to be used with ds_profile_remote_get_profile_list to
  retrieve list of all profiles from remote modem for given technology
  type
*/
typedef struct {
  uint32  profile_list_len;  /**< Must be set to # of elements in profile_list */
  ds_profile_remote_profile_info_type 
    profile_list[DS_PROFILE_REMOTE_NUM_PROFILE_MAX];
  /**< 
    List of all profiles
  */
}ds_profile_remote_profile_list_type;

/**
  @brief Data Type describing profile information
*/
typedef enum {
  DS_PROFILE_REMOTE_OPERATION_PROFILE_CREATE,
  DS_PROFILE_REMOTE_OPERATION_PROFILE_MODIFY,
  DS_PROFILE_REMOTE_OPERATION_PROFILE_DELETE,
  DS_PROFILE_REMOTE_OPERATION_PROFILE_RESET,
  DS_PROFILE_REMOTE_OPERATION_PROFILE_MAX,
} ds_profile_remote_operation_enum_type;

/**
  @brief Data Type describing profile information
*/
typedef struct {

  ds_profile_remote_operation_enum_type operation_type;
  /**< Identifies the operation like create profile, modify profile etc
    */

  uint8 profile_index;
  /**<   Profile number identifying the profile. */

  uint8 tech_type;
  /**<   Type of profile like 3gpp, 3gpp2 profile etc
   */
} ds_profile_remote_perform_operation_info_type;

/*===========================================================================

                        LOCAL FUNCTION DECLARATIONS

===========================================================================*/
/**
  @brief  This function Initializes the QMI-WDS client during power-up and 
          register with QMI WDS service on remote modem.
*/
void ds_profile_remote_client_init(void);

/**
  @brief  This API sends request to remote modem to create the profile
          for given tech type.

  @param[in] tech      Technology type
  
*/
void ds_profile_remote_create_profile_cmd_hdlr
(
  ds_profile_tech_etype  tech
);

/**
  @brief  This API sends request to remote modem to modify the profile
          information for given profile index 

  @param[in] profile_index      Profile index of the profile
  
*/
void ds_profile_remote_modify_profile_cmd_hdlr
(
  ds_profile_num_type    profile_index
);


/**
  @brief  This API sends request to remote modem to delete the profile
          information for given profile index 

  @param[in] profile_type       Type of the profile
  @param[in] profile_index      Profile index of the profile
  
*/
void ds_profile_remote_delete_profile_cmd_hdlr
(
  uint8           profile_type,
  uint8           profile_index
);


/**
  @brief  This API sends request to remote modem to reset the profile
          information to default values

  @param[in] profile_index      Profile index of the profile
  
*/
void ds_profile_remote_reset_profile_to_default_cmd_hdlr
(
  uint8  profile_index
);

/**
  @brief  This API sends request to remote modem to peform the necessary
          sync operation. Sync operation would make the profile on
          local and remote modem same wrt number of profiles and profile 
          information
  
*/
void ds_profile_remote_perform_sync
(
  void
);


/**
  @brief  This API sends request to remote modem to retrieve all the 
          profile list
  
  @param[in] profile_type      Type of the profile
  @param[out] profile_list     Profile list returned
  
  @return  DS_PROFILE_REG_RESULT_SUCCESS  On Success.
  @return  DS_PROFILE_REG_RESULT_FAIL     On failure. 
  
*/
int16 ds_profile_remote_get_profile_list
(
  ds_profile_remote_profile_list_type * profile_list,
  ds_profile_tech_etype                profile_type
);


/**
  @brief  This API lets local modem know that profile identifier has not
          been set and hence it neednt send it to remote modem
  
  @param[in] profile_identifier      Type of the profile
  
  @return  TRUE            Profile identifier is set.
  @return  FALSE           Profile identifier is not set. 
  
*/
boolean ds_profile_remote_is_set_profile_identifier
(
  int profile_identifier
);


/**
  @brief  This API lets the local modem know that profile identifer
          is set and it has to send it to remote modem.
  
  @param[in] profile_identifier      profile identifier
  @param[in] is_set                  Profile identifier is set or unset
  
*/
void ds_profile_remote_profile_identifier_set
(
  int profile_identifier, 
  boolean is_set
);

/**
  @brief  Called during powerup to register handler with dcc task to 
          perform remote operations like create profile, delete profile
          etc.
*/
void ds_profile_remote_init
(
  void
);

/**
  @brief  Sends cmd to dcc task to modify profile on remote modem
*/
void ds_profile_remote_modify_profile
(
  int profile_num
);

/**
  @brief  Sends cmd to dcc task to delete profile on remote modem
*/
void ds_profile_remote_delete_profile
(  
  uint8           profile_tech_type,
  uint8           profile_index
);

/**
  @brief  Sends cmd to dcc task to create profile on remote modem
*/
void ds_profile_remote_create_profile
(  
  uint8           profile_tech_type
);

/**
  @brief  Sends cmd to dcc task to reset profile to default on remote 
*/
void ds_profile_remote_reset_profile_to_default
(
  uint8           profile_index
);

#endif /* DS_QMI_WDS_QCCI_CLIENT_H */
