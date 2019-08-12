#ifndef CB_WMS_IF_H
#define CB_WMS_IF_H
/*===========================================================================

                    CB<->WMS INTERFACE HEADER FILE

DESCRIPTION
  This file contains types, function prototypes and data declarations for
  Cell Broadcast task

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cbwmsif.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/05   kvk     Enums defined to pass along with cell change ind to WMS
                   to notify the type of cell change.
07/14/04   vdr     Added new macro CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED controling 
                   maximum number of message IDs that could be processed
                   Added new primitives:
                    o CB_DUPLICATION_DETECTION_IND
                    o CB_DELETE_PAGE_REQ
06/24/03   vdr     Initial version


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"



#include "comdef.h"
#include "nas_exp.h"

#include "sys.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED 50

/* 
** The following message IDs is to be used by WMS CB tasks
** in order to handle CB reception
*/

/*
** Refer to section 9.4.1.3 ETWS primary Notification message
** in 23.041
*/

#define CB_ETWS_WARN_TYPE_LEN 2
#define CB_ETWS_WARN_SEC_INFO_LEN 50
#define CB_ETWS_PLMN_ID_LEN 3
/*
** Secondary notification is recieved to CB as a BMC page and from spec 
** 23.041 section 9.4.2.2.5 CB DATA
** MAX number of pages in a BMC page is 15 and each page occupies 83 bytes of data
** So the Max value of page content is 15*83=1245
** CB will deliver entire ETWS secondary notification (all pages) at one step to WMS
** instead of delivering page wise .
*/
#define CB_ETWS_MAX_SEC_NOFICATION_LEN 1245


typedef enum cb_wms_message_id_tag
{
  CB_SEARCH_LIST_REQ,
  CB_DATA_IND,
  CB_CELL_CHANGE_IND,
  CB_DUPLICATION_DETECTION_IND,
  CB_DELETE_PAGE_REQ,
  CB_DELETE_ALL_REQ,
  CB_ERROR_IND,

  CB_ETWS_PRIM_NOTIFICATION_IND,
  CB_ETWS_SEC_NOTIFICATION_IND,
  CB_DUAL_STANDBY_STATUS_IND

} cb_wms_message_id_tag ;

typedef enum cb_wms_search_id_tag
{
  CB_SINGLE_MESSAGE_ID ,
  CB_MESSAGE_ID_RANGE
} cb_wms_search_id_type ;

typedef struct cb_single_message_id_tag
{
  cb_wms_search_id_type message_id_type ;

  word                   message_id ;
} cb_single_message_id_type ;

typedef struct cb_message_id_range_tag
{
  cb_wms_search_id_type message_id_type ;

  word                   min_message_id ;

  word                   max_message_id ;
} cb_message_id_range_type ;

typedef union cb_wms_message_id_value_tag
{
  cb_wms_search_id_type current_message_id_type ;

  cb_single_message_id_type single_message_id ;

  cb_message_id_range_type message_id_range ;
} cb_wms_message_id_value_type ;

/*
** The following structure describes CB_SEARCH_LIST_REQ 
** which is sent from WMS to CB in order to provide CB 
** message ID list that CB is used to collect requested 
** messages
**
** NOTE: In order to start CB message collection WMS needs
**       to send CB_SEARCH_LIST_REQ to CB task which is 
**       containing at least one message ID, otherwise CB
**       message collection is to be stoped 
*/
typedef struct cb_search_list_req_tag
{
  /* Common command header */
  IMH_T message_header;

  /* Search List Length */
  byte  search_list_length ;

  /* Pointer to requested Search List (message IDs) */
  cb_wms_message_id_value_type search_list[CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED] ;


  /* 
  ** Boolean flag indicating whether BMC task is going to 
  ** process "Reading Advised" SFNs of the BMC Scheduling Message 
  */
  boolean reading_advised ;

  /* 
  ** Boolean flag indicating whether BMC task is going to 
  ** process "Reading Optional" SFNs of the BMC Scheduling Message 
  */
  boolean reading_optional ;


  /* Subscription ID */

  sys_modem_as_id_e_type as_id;


} cb_search_list_req_type ;


typedef enum cell_change_type
{
  CB_WMS_NO_SERVICE,
  CB_WMS_SAME_CELL,
  CB_WMS_CELL_CHANGE,
  CB_WMS_LAC_CHANGE,
  CB_WMS_PLMN_CHANGE
}cell_change_type_T;

/*
** The following structure describes CB_DATA_IND primitive
** which is sent from CB to WMS in order to deliver received 
** CB page
**
*/
typedef struct cb_data_ind_tag
{
  /* Common command header */
  IMH_T message_header;

  /* Received CB page length */
  byte  page_length ;

  /* Received CB message body */
  byte cb_page[88];

  /* Subscription ID */
  sys_modem_as_id_e_type as_id;

} cb_data_ind_type ;

typedef struct cb_cell_change_ind_tag
{
  /* Common command header */
  IMH_T message_header;

  /*Type of cell change*/
  cell_change_type_T cell_change_type;

  /* Subscription ID */
  sys_modem_as_id_e_type as_id;

} cb_cell_change_ind_type ;

/*
** The following structure represents CB_DUPLICATION_DETECTION_IND
** primitive which is sent from CB to WMS in order to notify the 
** WMS that duplication is detected.
**
** NOTE: This primitive is sent only for pages that their GS is 
**       equal to 0x00 (Cell-Wide Immediate Display).
**
*/
typedef struct cb_duplication_detection_ind_tag
{
  /* Common command header */
  IMH_T                  message_header ;

  /* Message Identifier */
  word                   message_id ;

  /* Message code */
  word                   message_code ;

  /* Geographic scope */
  byte                   gs_indicator ;

  /* Update Number */
  byte                   update_number ;

  /* Page Number */
  byte                   page_number ;

  /* Total number of pages */
  byte                   number_of_pages ;

  /* Data coding scheme */
  byte                   data_coding_scheme ;

/* Subscription ID */

  sys_modem_as_id_e_type as_id;

} cb_duplication_detection_ind_type ;



/*
** The following structure descibes RRC_ETWS_PRIM_NOTIFICATION_IND primitive
** which is sent from CB to WMS inorder to deliver the received ETWS primary
** notifcation
** Refer to section 9.4.1.3 ETWS primary Notification message
** in 23.041
** The flag warning secuirity info present will indicate whether security info
** is included in the ETWS primary notification or not 
** If it is set to False WMS shall not read the contents of warning sec info.
*/
typedef struct cb_etws_primary_notification_tag
{
  /* Common command header */
  IMH_T                  message_header ;

  /* Message Identifier */
  word                   message_id ;

  /* Serial Number */
  word                   serial_number ;

  /* PLMN ID */
  sys_plmn_id_s_type     plmn_id;

  byte                   warning_type[CB_ETWS_WARN_TYPE_LEN];

  boolean                warning_security_info_present;

  byte                   warning_security_info[CB_ETWS_WARN_SEC_INFO_LEN];

  /* Subscription ID */

  sys_modem_as_id_e_type as_id;

} cb_etws_prim_notification_ind_type;


typedef struct cb_etws_secondary_notification_tag
{
  /* Common command header */
  IMH_T                  message_header ;

  /* PLMN ID */
  sys_plmn_id_s_type     plmn_id;

  /* Message Identifier */
  word                   message_id ;

  /* Serial Number */
  word                   serial_number ;

  /* Current data coding scheme*/
  byte                   current_data_coding_scheme;

  /* number of pages*/
  byte                   number_of_pages;

  /*contents of the page*/
  byte                   page_data[CB_ETWS_MAX_SEC_NOFICATION_LEN];

  /* Subscription ID */

  sys_modem_as_id_e_type as_id;

} cb_etws_sec_notification_ind_type;



/*
** The following structure represents CB_DELETE_PAGE_REQ primitive 
** which is sent from WMS to CB task in order to notify the CB that
** particular message page is to be deleted from the CB task cache 
** memory and new page reception is allowed
*/
typedef struct cb_delete_page_req_tag
{
  /* Common command header */
  IMH_T message_header;

  /* Message Identifier */
  word                   message_id ;

  /* Message code */
  word                   message_code ;

  /* Page Number */
  byte                   page_number ;

  /* Subscription ID */

  sys_modem_as_id_e_type as_id;


} cb_delete_page_req_type;

/*
** The following structure represents CB_DELETE_ALL_REQ primitive 
** which is sent from WMS to CB task in order to notify the CB that
** task's cache is to be cleaned up
*/
typedef struct cb_delete_all_req_tag
{
  /* Common command header */
  IMH_T message_header;

  /* Subscription ID */

  sys_modem_as_id_e_type as_id;


} cb_delete_all_req_type;

/*
** The following structure represents CB_ERROR_INDICATION primitive 
** which is sent from WMS to CB task in order to notify WMS of an error
*/
typedef enum cb_error_type_tag
{
  CB_WMS_ERROR_LO,
  CB_WMS_ERROR_MEMORY_FULL,
  CB_WMS_ERROR_HI
}cb_error_type_T;

typedef struct cb_error_indication_tag
{
  /* Common command header */
  IMH_T message_header;

  /* indicates error type */
  cb_error_type_T error_value;


  sys_modem_as_id_e_type as_id;


} cb_error_indication_type;
 
/*
** The following structure represents CB_DUAL_STANDBY_STATUS_IND primitive 
** which is sent from WMS to CB task in order to notify the CB that
** the dual standby status has been changed. (e.g UE mode changed from 
** single standby to dual standby or dual standby to single standby
*/
typedef struct cb_dual_standby_status_ind_tag
{
  /* Common command header */
  IMH_T   message_header;

  /*Dual standby status*/
  /*0 Single Standby*/
  /*1 Dual Standby*/
  boolean dual_standby_mode;
  /*Active SUB ID*/
  /*In case UE mode changed from DS to SS*/
  /* Subscription ID */
  sys_modem_as_id_e_type active_sub_id;

} cb_dual_standby_status_ind_type;


#endif /* #ifndef CB_WMS_IF_H */
