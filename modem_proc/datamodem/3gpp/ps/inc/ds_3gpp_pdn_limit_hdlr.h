#ifndef DS_3GPP_PDN_LIMIT_HDLR_H
#define DS_3GPP_PDN_LIMIT_HDLR_H
/*===========================================================================

            DS 3GPP PDN LIMIT HANDLER

===========================================================================*/
/*!
  @file
  ds_3gpp_pdn_limit_hdlr.h

  @brief
  This header file externalizes PDN Limit handling functionality

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/        
/*==============================================================================

  Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_pdn_limit_hdlr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/15/13   vs      Initial File
==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#ifdef FEATURE_DATA_LTE
#include "rex.h"
#include "ds_3gpp_pdn_context.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Maximum number of entries in the PDN Limit Table.*/
#define DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES DS_3GPP_MAX_AVAIL_PDNS
#define DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES

/* Directory & File Length for Persistent Throttling */
#define DS_3GPP_PDN_LIMIT_THROT_DIR_LEN 25

#define DS_3GPP_PDN_LIMIT_THROT_FILE_LEN_MAX 31

/*============================================================================
  Structure which stores the information read from EFS.
  It tells us whether PDN Limit handling is enabled or not. 			
  By default it is disabled.			
    																	
  It also tells us the PDN Req Throttling Time in mins.
============================================================================*/
typedef PACKED struct PACKED_POST
{
  boolean                  pdn_limit_is_enabled;
  uint16                   pdn_req_throttling_time;
}ds_3gpp_pdn_limit_efs_info_s;

/*===========================================================================
  EFS File Element Structure: Used to make throttling info persistant across
  cycle
=============================================================================*/

typedef struct
{
  boolean           valid;
  uint32            cookie;
  byte              apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];        /*APN name */
  uint64            timer_val;                    /*PDN Req Wait timer value  */
  uint64            timestamp;                   /*time when timer was started*/
} throt_info_pdn_limit_timer_type_dyn_s;

typedef struct
{
 throt_info_pdn_limit_timer_type_dyn_s    *throt_info_pdn_limit_timer_type_dyn_p;
} ds_3gpp_pdn_limit_throt_info_pdn_limit_timer_type;

/*=========================================================================== 
  Local Structure which stores the retrieved EFS info along with sampling timer
  & IMSI information
=============================================================================*/

typedef struct
{
  /* Structure to store PDN Limit throt timer config values              */ 
  ds_3gpp_pdn_limit_throt_info_pdn_limit_timer_type   
                                    pdn_limit_timer_tbl
                                    [DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES];                                                                            
             
  rex_timer_type                    sampling_timer;       /* Sampling timer  */
  uint32                            sampling_timer_val;   /* Sampling timer val*/
  nv_ehrpd_imsi_type                imsi_data;            /* IMSI data         */

  boolean                           imsi_data_avail;/* IMSI data available flag */

  char                              efs_file_name   /* EFS File to store */
     [DS_3GPP_PDN_LIMIT_THROT_DIR_LEN+1+DS_3GPP_PDN_LIMIT_THROT_FILE_LEN_MAX+1];

} ds_3gpp_pdn_limit_throt_info_type;

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_POWERUP_INIT

  DESCRIPTION
  This function performs Powerup Initialization of the Module
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_powerup_init
(
  sys_modem_as_id_e_type  subs_id
);
/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CLEAR_PER_SUBS_ID

  DESCRIPTION
  This function clears pdn limit info per subs_id
  
  PARAMETERS  
  subs_id -subscription_id 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gpp_pdn_limit_clear_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_ENABLED

  DESCRIPTION
  This function checks whether PDN Limit handling is enabled.
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE if enabled. FALSE otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_is_enabled
(
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_TIME_REMAINING_IN_CURRENT_BLK

  DESCRIPTION
  This function fetches the time in seconds remaining before the current
  time block ends.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  Current time in milli-secs of the PDN Conn Timer.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Time in mill-seconds remaining before the current time block ends.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_tbl_get_time_remaining_in_current_blk
(
  uint8 index,
  sys_modem_as_id_e_type subs_id,
  uint32 time_in_msec
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_MAX_PDN_CONN_ALLOWED_IN_GIVEN_TIME

  DESCRIPTION
  This function fetches the Max PDN Connections allowed at any instant of time.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  Current value of PDN Con Timer in milli-secs.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Connections allowed in given time.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed_in_given_time
(
  uint8 index,
  sys_modem_as_id_e_type subs_id,
  uint32 time_in_msec
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_FIND_ENTRY

  DESCRIPTION
  This function finds entry corresponding to a given APN in PDN Limit Table.
  
  PARAMETERS  
  APN Name
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Index into the PDN limit Table.
  Returns DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY if cannot find an entry.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/

uint8 ds_3gpp_pdn_limit_tbl_find_entry
(
  byte                          *apn,
  sys_modem_as_id_e_type         subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_FIND_OR_ADD_ENTRY

  DESCRIPTION
  This function tries to finds an entry corresponding to a given APN in the
  PDN Limit Table. If it cannot find an entry it creates a new entry in the
  PDN Limit Table.
  
  PARAMETERS  
  APN Name
  Max PDN Connections Per Block.
  Max PDN Connection Time.
  PDN Req Wait Time.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Index into the PDN limit Table.
  Returns DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY if it cannot make an entry.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_3gpp_pdn_limit_tbl_find_or_add_entry
(
  byte *apn,
  uint16 max_pdn_conn_per_blk,
  uint16 max_pdn_conn_time,
  uint16                 pdn_req_wait_time,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_APN_NAME

  DESCRIPTION
  This function updates the APN Name in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New APN name.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_tbl_update_apn_name
(
  uint8    index,
  sys_modem_as_id_e_type subs_id,
  char*    apn
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_MAX_PDN_CONN_TIME

  DESCRIPTION
  This function updates the Max PDN Conn Time in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New Max PDN Conn Time
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_max_pdn_conn_time
(
  uint8 index,
  sys_modem_as_id_e_type subs_id,
  uint16 max_pdn_conn_time
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_MAX_PDN_CONN_PER_BLOCK

  DESCRIPTION
  This function updates the Max PDN Conn Per Block in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New Max PDN Conn Per Block.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_max_pdn_conn_per_blk
(
  uint8 index,
  sys_modem_as_id_e_type subs_id,
  uint16 max_pdn_conn_per_blk
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_PDN_REQ_WAIT_TIME

  DESCRIPTION
  This function updates the PDN Req Wait Time in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New PDN Req Wait Time.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_pdn_req_wait_time
(
  uint8 index,
  sys_modem_as_id_e_type subs_id,
  uint16 pdn_req_wait_time
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_PDN_CONN_TMR

  DESCRIPTION
  This function starts the PDN Conn Timer if the PDN throttling timer
  is not running and if the PDN Conn Timer has not yet started.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_start_pdn_conn_tmr
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id 
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_PDN_WAIT_TMR

  DESCRIPTION
  This function starts the PDN Req Wait Timer.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gpp_pdn_limit_start_pdn_wait_tmr
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_STOP_PDN_WAIT_TMR

  DESCRIPTION
  This function stops the PDN Req Wait Timer.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_stop_pdn_wait_tmr
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_INCREMENT_PDN_CONN_CNTR

  DESCRIPTION
  This function increments the PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_increment_pdn_conn_cntr
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_RESET_PDN_CONN_CNTR

  DESCRIPTION
  This function resets the PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_reset_pdn_conn_cntr
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_PDN_CONN_CNTR

  DESCRIPTION
  This function fetches the current PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Current PDN Conn Cntr.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint16 ds_3gpp_pdn_limit_get_pdn_conn_cntr
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CALL_BRINGUP_IS_ALLOWED

  DESCRIPTION
  This function checks whether call bringup is allowed for the given APN.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE if call is allowed.
  FALSE otherwise.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_call_bringup_is_allowed
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_CURRENT_PDN_CONN_TMR_VAL

  DESCRIPTION
  This function checks gets the current PDN Connection Timer Value.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Current PDN Conn Timer Value.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_get_current_pdn_conn_tmr_val
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_PDN_REQ_WAIT_TIMER_IN_PROGRESS

  DESCRIPTION
  This function checks whether the PDN Req Wait Timer is running.
  
  PARAMETERS  
  Index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE or FALSE based on whether PDN Wait Timer is running
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_is_pdn_req_wait_timer_in_progress
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_CMDS

  DESCRIPTION
  This function processes any commands that need to be handled by
  PDN Connection Limit Module.
  
  PARAMETERS
  1. Cmd Ptr
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_cmds
(
  const ds_cmd_type      *cmd_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_REMOVE_ENTRY
  DESCRIPTION
  This function removes entry at index 0 from PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_tbl_remove_entry
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_LIMIT_CLR_PDN_CONN_TMR 

DESCRIPTION
  This function clears the PDN connection timer

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_limit_clr_pdn_conn_tmr
(
   uint8                             index,
   sys_modem_as_id_e_type            subs_id
);

/*==============================================================================
FUNCTION DS_3GPP_PDN_CNTXT_VALIDATE_AND_UPDATE_PDN_LIMIT_ENTRY
 
DESCRIPTION
  This function initially checks if pdn limit parameters bring passed are non-
  default , 
    If Yes: Add an entry in PDN limit table
    If NO : Check all other profile for non-default pdn limit parameter
            with same apn
            If Yes : Add an entry in PDN limit table with non-default parameters
            If No  : Add an entry in PDN limit table default parameters
 
  Assumptions made 
  1: Non-default Profile PDN limit parameter will be updated over default Profile
     PDN limit parameters with the same APN
  2. First non-default Profile PDN limit parameters will be chosen over multiple
     non-default Profile parameter given they have same APN

PARAMETERS 
  uint8  Profile_id :  Profile ID being passed
  byte profiel_apn  :  APN name of Profile ID being passed
  subs_id           :  Subscription ID
 
DEPENDENCIES 
  None. 

RETURN VALUE 
  boolean DS3G_FAILURE : Failure to update PDN Limit Entry
          DS3G_SUCCESS : Sucessfully update PDN Limit Entry
 
SIDE EFFECTS 
  None 

==============================================================================*/
ds_umts_pdp_profile_status_etype ds_3gpp_pdn_cntxt_validate_and_update_pdn_limit_entry
(
 uint8                  profile_id,
 uint8                  *handle_id,
 sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_IS_PDN_LIMIT_PARAM_DEFAULT
 
DESCRIPTION
  This function verifies parameters present in profile being passed 
  are non default param

PARAMETERS 
  profile_info: Profile information
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean TRUE: PDN limit parameters present in profile are default
          FALSE: PDN limit parameters present in profile are non-default

SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_pdn_context_is_pdn_limit_param_default
(
  uint16                max_pdn_conn_per_blk,
  uint16                 max_pdn_conn_time,
  uint16                 pdn_req_wait_time
);

/*==============================================================================
FUNCTION DS_3GPP_PDN_LIMIT_IMSI_INFO_AVAIL_CMD_HDLR
 
DESCRIPTION
  This function process the IMSI information available CMD so that to read IMSI
  information and perform power-up throttling if needed

PARAMETERS 
  subs_id           :  Subscription ID
 
DEPENDENCIES 
  None. 

RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None 

==============================================================================*/
void ds_3gpp_pdn_limit_imsi_info_avail_cmd_hdlr
(
  sys_modem_as_id_e_type                       subs_id
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /*TEST_FRAMEWORK*/

#endif /*FEATURE_DATA_LTE*/
#endif /* DS_3GPP_PDN_LIMIT_HDLR_H */


