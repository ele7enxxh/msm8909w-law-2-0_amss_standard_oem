#ifndef DS_MPPM_PDN_DB_MGR_H
#define DS_MPPM_PDN_DB_MGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            D S  M P P M  P D N  D B  M G R

GENERAL DESCRIPTION

    This file contains declarations and definitions to support PDN DB Update
    APIs.
    Note: Pointers about usage and behaviour.
    1)At any point of time there can be only one active transaction.
    2)All APIs/operations should be called inside a transaction.
    3)Multiple APIs/operations can be done within a transaction.
    4)During the end transaction, user can choose to either commit or cancel
      the changes done during that transaction.

Copyright (c) 2013-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
            EDIT HISTORY FOR FILE

  $Header: //components/dev/data.mpss/3.0/3gpp2/dsd/inc/ds_mppm_pdn_db_mgr.h#3

when       who         what, where, why
--------   -----       ------------------------------------------------------
10/23/13   vl           Created the file
===========================================================================*/

/*===========================================================================
            INCLUDEFILES
===========================================================================*/

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds_mppm_efs_parser.h"

/*===========================================================================
            DATA STRUCTURES
===========================================================================*/
/*---------------------------------------------------------------------------
  Enum for RAT Types
---------------------------------------------------------------------------*/
typedef enum
{
  DS_MPPM_PDN_DB_RAT_TYPE_WWAN = 0,
  DS_MPPM_PDN_DB_RAT_TYPE_WLAN_LB,
  DS_MPPM_PDN_DB_RAT_TYPE_IWLAN,
  DS_MPPM_PDN_DB_RAT_TYPE_MAX = DS_MPPM_PDN_DB_RAT_TYPE_IWLAN
}ds_mppm_pdn_db_rat_type;

/*---------------------------------------------------------------------------
  Enum for Transaction Return Code types
---------------------------------------------------------------------------*/
typedef enum
{
  DS_MPPM_PDN_DB_TXN_SUCCESS = 0,
  DS_MPPM_PDN_DB_TXN_IN_PROGRESS,
  DS_MPPM_PDN_DB_TXN_INVALID_ID,
  DS_MPPM_PDN_DB_TXN_INVALID_EXEC_TYPE,
  DS_MPPM_PDN_DB_TXN_FAILED,
  DS_MPPM_PDN_DB_TXN_RET_CODE_TYPE_MAX = DS_MPPM_PDN_DB_TXN_FAILED
}ds_mppm_pdn_db_txn_ret_code_type;

/*---------------------------------------------------------------------------
  Enum for Transaction Execution Types - Used during end transaction to
  either commit or cancel the changes done during the transaction.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_MPPM_PDN_DB_TXN_COMMIT = 0,
  DS_MPPM_PDN_DB_TXN_CANCEL,
  DS_MPPM_PDN_DB_TXN_EXEC_TYPE_MAX = DS_MPPM_PDN_DB_TXN_CANCEL
}ds_mppm_pdn_db_txn_exec_type;

/*---------------------------------------------------------------------------
  Enum for Failure Code Types
  DS_MPPM_FAILURE_CODE_OTHER - is used to notify non-categorized errors like
                               DB read fail, memory alloca fail etc. In these
                               cases the actual error will be logged in to F3
---------------------------------------------------------------------------*/
typedef enum
{
  DS_MPPM_PDN_DB_FAILURE_CODE_NONE = 0,
  DS_MPPM_PDN_DB_FAILURE_CODE_OTHER,
  DS_MPPM_PDN_DB_SUBSCRIPTION_ID_INVALID,
  DS_MPPM_PDN_DB_INVALID_APN_NAME,
  DS_MPPM_PDN_DB_APN_ALREADY_EXISTS,
  DS_MPPM_PDN_DB_APN_DOES_NOT_EXIST,
  DS_MPPM_PDN_DB_MAX_PDN_CONN_REACHED,
  DS_MPPM_PDN_DB_INVALID_RAT_PRIORITY_LIST,
  DS_MPPM_PDN_DB_INVALID_OVERRIDE_TYPE,
  DS_MPPM_PDN_DB_FAILURE_CODE_TYPE_MAX = DS_MPPM_PDN_DB_INVALID_OVERRIDE_TYPE
}ds_mppm_failure_code_type;

/*---------------------------------------------------------------------------
  RAT PRIORITY LIST Structure
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  rat_priority_list_len;
  ds_mppm_pdn_db_rat_type  ds_mppm_rat_list[PDN_CONFIG_MAX_SUPPORTED_RATS];
}ds_mppm_pdn_db_rat_priority_list_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB ENTRY Structure - This is the template of a single PDN Policy
  Entry in the db.
    apn_name   - APN name is the key element identifies the policy for a
                 PDN. APN length should be always>0 and <=100 characters.
                 NULL APNs are identified by "NULL" string.
    supported_rat_priority_list_present - Indicates
                 that supported_rat_priority_list attribute is present in
                 the struct and should be read.
    supported_rat_priority_list - is a priority list of utmost 3 RATs which
                 typically indicates the priority order in which the APN
                 supports WWAN, IWLAN and WLAN_LB RAT types.
    default_apn_present - indicates that default_apn is present in the
                 struct and should be read.
    default_apn - is used to indicate whether a particular APN name specified
                  in PDN_APN_String token is the default APN for the carrier
    override_type_present - Indicates that override type is present in the
                  struct and should be read.
    override_type - overide_type is used to indicate the override type of
                    type ds_mppm_override_type.
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_apn_name_type                    apn_name;
  boolean                                  supported_rat_priority_list_present;
  ds_mppm_pdn_db_rat_priority_list_s_type  supported_rat_priority_list;
  boolean                                  default_apn_present;
  boolean                                  default_apn;
  boolean                                  override_type_present;
  ds_mppm_override_type                    override_type;
}ds_mppm_pdn_db_entry_s_type;

/*---------------------------------------------------------------------------
  Transaction ID Type
---------------------------------------------------------------------------*/
typedef uint32 ds_mppm_pdn_db_txn_id_type;

/*---------------------------------------------------------------------------
  APN List Structure
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                  apn_name_list_len;
  ds_mppm_apn_name_type   apn_name_list[DS_MAX_PDN_COUNT];
}ds_mppm_pdn_db_apn_name_list_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB START TXN REQUEST Structure
  subscription_id - identifies the policy DB for which the transaction should
                    be started.
  Note: Currently only default subscription id is supported.
---------------------------------------------------------------------------*/
typedef struct
{
  ps_sys_subscription_enum_type   subscription_id;
}ds_mppm_pdn_db_start_txn_request_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB START TXN RESPONSE Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_ret_code_type   txn_ret_code;
  ds_mppm_failure_code_type          failure_code;
  ds_mppm_pdn_db_txn_id_type         txn_id;
}ds_mppm_pdn_db_start_txn_response_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB ADD POLICY REQUEST Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_id_type      txn_id;
  ds_mppm_pdn_db_entry_s_type     pdn_db_entry;
}ds_mppm_pdn_db_add_policy_request_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB ADD POLICY RESPONSE Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_ret_code_type   txn_ret_code;
  ds_mppm_failure_code_type          failure_code;
}ds_mppm_pdn_db_add_policy_response_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB MODIFY POLICY REQUEST Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_id_type      txn_id;
  ds_mppm_pdn_db_entry_s_type     pdn_db_entry;
}ds_mppm_pdn_db_modify_policy_request_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB MODIFY POLICY RESPONSE Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_ret_code_type   txn_ret_code;
  ds_mppm_failure_code_type          failure_code;
}ds_mppm_pdn_db_modify_policy_response_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB DELETE POLICY REQUEST Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_id_type       txn_id;
  ds_mppm_apn_name_type            apn_name;
}ds_mppm_pdn_db_delete_policy_request_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB DELETE POLICY RESPONSE Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_ret_code_type   txn_ret_code;
  ds_mppm_failure_code_type          failure_code;
}ds_mppm_pdn_db_delete_policy_response_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB GET PDN POLICY APN LIST REQUEST Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_id_type     txn_id;
}ds_mppm_pdn_db_get_apn_list_request_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB GET PDN POLICY APN LIST RESPONSE Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_ret_code_type       txn_ret_code;
  ds_mppm_failure_code_type              failure_code;
  ds_mppm_pdn_db_apn_name_list_s_type    apn_list;
}ds_mppm_pdn_db_get_apn_list_response_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB GET SETTINGS FOR APN REQUEST Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_id_type     txn_id;
  ds_mppm_apn_name_type          apn_name;
}ds_mppm_pdn_db_get_settings_for_apn_request_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB GET SETTINGS FOR APN RESPONSE Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_ret_code_type   txn_ret_code;
  ds_mppm_failure_code_type          failure_code;
  ds_mppm_pdn_db_entry_s_type        pdn_db_entry;
}ds_mppm_pdn_db_get_settings_for_apn_response_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB END TRANSACTION REQUEST Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_id_type     txn_id;
  ds_mppm_pdn_db_txn_exec_type   txn_exec_type;
}ds_mppm_pdn_db_end_txn_request_s_type;

/*---------------------------------------------------------------------------
  MPPM PDN DB END TRANSACTION RESPONSE Structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mppm_pdn_db_txn_ret_code_type   txn_ret_code;
  ds_mppm_failure_code_type          failure_code;
}ds_mppm_pdn_db_end_txn_response_s_type;

/*===========================================================================
            EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION  DS_MPPM_PDN_DB_START_TXN

DESCRIPTION   This function will start a transaction for pdb db.

PARAMETERS  *start_txn_request_ptr  - Pointer to a request structure
                                      containing subscription id
            *start_txn_response_ptr - Pointer to a response structure. The
                                      response structure will contain the
                                      return codes and values.

            Note: Currently only default subscription id is supported in the
                  request.
DEPENDENCIES  API caller should provide memory for request and response
              structures.

RETURN VALUE  TRUE - if function completed successfully
              FALSE - If function failed due to internal error.

              Note: The return value only indicates internal error, it doesnt
              says anything about the transaction state. The values in
              response structure are valid only when function returns TRUE.

SIDEEFFECTS   If transaction is successfull, a temporal DB for this
              transaction is created by duplicating the existing DB and a
              valid transaction id is returned.
===========================================================================*/
boolean ds_mppm_pdn_db_start_txn
(
  ds_mppm_pdn_db_start_txn_request_s_type  *start_txn_request_ptr,
  ds_mppm_pdn_db_start_txn_response_s_type *start_txn_response_ptr
);

/*===========================================================================
FUNCTION  DS_MPPM_PDN_DB_ADD_POLICY

DESCRIPTION   This function will add a pdn policy entry to pdb policy db.

PARAMETERS  *add_policy_request_ptr  - Pointer to a request structure
                                       containing transaction id and settings
                                       for pdn policy
            *add_policy_response_ptr - Pointer to a response structure. The
                                       response structure will contain the
                                       return codes and values.

DEPENDENCIES  API caller should provide memory for request and response
              structures.

RETURN VALUE  TRUE - if function completed successfully
              FALSE - If function failed due to internal error.

              Note: The return value only indicates internal error, it doesnt
              says anything about the transaction state. The values in
              response structure are valid only when function returns TRUE.

SIDEEFFECTS   If transaction is successfull, policy entry will be added to
              the temporal DB.
===========================================================================*/
boolean ds_mppm_pdn_db_add_policy
(
  ds_mppm_pdn_db_add_policy_request_s_type   *add_policy_request_ptr,
  ds_mppm_pdn_db_add_policy_response_s_type  *add_policy_response_ptr
);

/*===========================================================================
FUNCTION  DS_MPPM_PDN_DB_MODIFY_POLICY_SETTINGS

DESCRIPTION   This function will modify a pdn policy settings in the
              pdb policy db.

              NOTE: To update policy settings, the existing policy entry
              should be read from the PDN db and provided with the modified
              values.

PARAMETERS  *modify_policy_request_ptr  - Pointer to a request structure
                                          containing transaction id and pdn
                                          policy settings to be modified.
            *modify_policy_response_ptr - Pointer to a response structure.
                                          The response structure will contain
                                          the return codes and values.
DEPENDENCIES  API caller should provide memory for request and response
              structures.

RETURN VALUE  TRUE - if function completed successfully
              FALSE - If function failed due to internal error.

              Note: The return value only indicates internal error, it doesnt
              says anything about the transaction state. The values in
              response structure are valid only when function returns TRUE.

SIDEEFFECTS   If transaction is successfull, policy entry will be modified in
              the temporal DB.
===========================================================================*/
boolean ds_mppm_pdn_db_modify_policy_settings
(
  ds_mppm_pdn_db_modify_policy_request_s_type  *modify_policy_request_ptr,
  ds_mppm_pdn_db_modify_policy_response_s_type *modify_policy_response_ptr
);

/*===========================================================================
FUNCTION  DS_MPPM_PDN_DB_DELETE_POLICY_BY_APN

DESCRIPTION   This function will delete a pdn policy identified by APN name
              from pdb policy db.

PARAMETERS  *delete_policy_request_ptr  - Pointer to a request structure
                                          containing transaction id and apn
                                          name of pdn policy to be deleted.
            *delete_policy_response_ptr - Pointer to a response structure.
                                          The response structure will contain
                                          the return codes and values.

DEPENDENCIES  API caller should provide memory for request and response
              structures.

RETURN VALUE  TRUE - if function completed successfully
              FALSE - If function failed due to internal error.

              Note: The return value only indicates internal error, it doesnt
              says anything about the transaction state. The values in
              response structure are valid only when function returns TRUE.

SIDEEFFECTS   If transaction is successfull, policy entry will be deleted
              from the temporal DB.
===========================================================================*/
boolean ds_mppm_pdn_db_delete_policy_by_apn
(
  ds_mppm_pdn_db_delete_policy_request_s_type  *delete_policy_request_ptr,
  ds_mppm_pdn_db_delete_policy_response_s_type *delete_policy_response_ptr
);

/*===========================================================================
FUNCTION  DS_MPPM_PDN_DB_GET_APN_LIST

DESCRIPTION This function will get list of APN names of all the pdn policies
            configured in the pdb policy db.

PARAMETERS  *get_apn_list_request_ptr  - Pointer to a request structure
                                         containing transaction id.
            *get_apn_list_response_ptr - Pointer to a response structure.
                                         The response structure will contain
                                         the return codes and values.

DEPENDENCIES  API caller should provide memory for request and response
              structures.

RETURN VALUE  TRUE - if function completed successfully
              FALSE - If function failed due to internal error.

              Note: The return value only indicates internal error, it doesnt
              says anything about the transaction state. The values in
              response structure are valid only when function returns TRUE.

SIDEEFFECTS   If transaction is successfull, a list of APNs of all the
              policies in the temporal DB will be returned.
===========================================================================*/
boolean ds_mppm_pdn_db_get_apn_list
(
  ds_mppm_pdn_db_get_apn_list_request_s_type   *get_apn_list_request_ptr,
  ds_mppm_pdn_db_get_apn_list_response_s_type  *get_apn_list_response_ptr
);

/*===========================================================================
FUNCTION  DS_MPPM_PDN_DB_GET_SETTINGS_FOR_APN

DESCRIPTION   This function will get the policy settings of a PDN identified
              by the APN name.

PARAMETERS  *get_apn_settings_request_ptr - Pointer to a request structure
                                            containing transaction id. and
                                            APN name.
            *get_apn_settings_response_ptr - Pointer to a response structure.
                                             The response structure will
                                             contain the return codes and
                                             values.

DEPENDENCIES  API caller should provide memory for request and response
              structures.

RETURN VALUE  TRUE - if function completed successfully
              FALSE - If function failed due to internal error.

              Note: The return value only indicates internal error, it doesnt
              says anything about the transaction state. The values in
              response structure are valid only when function returns TRUE.

SIDEEFFECTS   If transaction is successfull, policy settings corresponding
              to the APN in the temporal DB will be returned.
===========================================================================*/
boolean ds_mppm_pdn_db_get_settings_for_apn
(
  ds_mppm_pdn_db_get_settings_for_apn_request_s_type
                                          *get_apn_settings_request_ptr,
  ds_mppm_pdn_db_get_settings_for_apn_response_s_type
                                          *get_apn_settings_response_ptr
);

/*===========================================================================
FUNCTION  DS_MPPM_PDN_DB_END_TXN

DESCRIPTION  This function will end the transaction. during the end
             transaction based on the execution type pdb db changes will be
             committed to the file or canceled.

PARAMETERS  *end_txn_request_ptr  - Pointer to a request structure containing
                                    transaction id and execution
                                    type(COMMIT or CANCEL).
            *end_txn_response_ptr - Pointer to a response structure. The
                                    response structure will contain the
                                    return codes and values.

DEPENDENCIES  API caller should provide memory for request and response
              structures.

RETURN VALUE  TRUE - if function completed successfully
              FALSE - If function failed due to internal error.

              Note: The return value only indicates internal error, it doesnt
              says anything about the transaction state. The values in
              response structure are valid only when function returns TRUE.

SIDEEFFECTS   If transaction is successfull, if the transaction execution
              type specified is COMMIT, the temporal DB will be made
              persistent, else the temporal DB is discarded.
===========================================================================*/
boolean ds_mppm_pdn_db_end_txn
(
  ds_mppm_pdn_db_end_txn_request_s_type  *end_txn_request_ptr,
  ds_mppm_pdn_db_end_txn_response_s_type *end_txn_response_ptr
);

#endif /*FEATURE_DATA_WLAN_MAPCON*/
#endif /*DS_MPPM_PDN_DB_MGR_H*/