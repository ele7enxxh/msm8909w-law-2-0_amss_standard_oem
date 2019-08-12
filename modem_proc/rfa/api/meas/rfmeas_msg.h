

/*!
  @file
  rfmeas_msg.h

  @brief
  Message router UMIDs header for RF MEAS subsystem. This file defines
  interfaces in the RF task.

  @note
  This file must be named with a _msg.h suffix for the build environment
  to correctly process the messages defined within.

*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================
                 EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/meas/rfmeas_msg.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/13   sc      Initial version

===========================================================================*/

#ifndef RF_MEAS_MSG_H
#define RF_MEAS_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "appmgr_msg.h"
#include "msgr.h"
#include "msgr_types.h"
#include "rfa_msgr.h"
#include "rfcom.h"
#include "lm_types.h"
#include "sys_type.h"
#include "mdsp_intf.h"
#include "rfmeas_types.h"

/*===========================================================================

                  EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*=============================================================================
                            MSGR SUPERVISORY ID'S
=============================================================================*/
/*! @todo Do we want the MSG IDs to start from the value 0x00? */
#define RFA_RF_MEAS_MSG_ID_GET_IRAT_INFO_PARAM    0x01
#define RFA_RF_MEAS_MSG_ID_GET_TIME_CONSTANT      0x02
#define RFA_RF_MEAS_MSG_ID_SCRIPT_ENTER           0x03
#define RFA_RF_MEAS_MSG_ID_SCRIPT_BUILD_SCRIPTS   0x04
#define RFA_RF_MEAS_MSG_ID_SCRIPT_EXIT            0x05

#define RFA_RF_MEAS_MSG_MAX_ID                    0xFFFF


/*=============================================================================
                            RF Task MEAS messages
=============================================================================*/
/*! @brief MEAS subsystem message IDs the RF task sends. 
 * This enum is never called by name. It just defines symbols.
 */
/*! MSGR_DEFINE_UMID calculation below for reference:*/
/*! RFA_RF_MEAS_<command>_CNF = (MSGR_RFA_RF_MEAS & 0xFFFF) << 16 |
 *                              (MSGR_TYPE_CNF & 0xFF) << 8 |
 *                              (RFA_RF_MEAS_MSG_ID_<command> & 0xFF)
 *
 *  RFA_RF_MEAS_CNF_<command> = (MSGR_RFA_RF_MEAS & 0xFFFF) << 16 |
 *                              (MSGR_TYPE_CNF & 0xFF) << 8 |
 *                              (RFA_RF_MEAS_MSG_ID_<command> & 0xFF)
*/
enum
{
   MSGR_DEFINE_UMID(RFA, RF_MEAS, CNF, GET_IRAT_INFO_PARAM,  RFA_RF_MEAS_MSG_ID_GET_IRAT_INFO_PARAM, rfa_rf_meas_get_irat_info_param_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_MEAS, CNF, GET_TIME_CONSTANT,    RFA_RF_MEAS_MSG_ID_GET_TIME_CONSTANT,   rfa_rf_meas_get_time_constant_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_MEAS, CNF, SCRIPT_ENTER,         RFA_RF_MEAS_MSG_ID_SCRIPT_ENTER,        rfa_rf_meas_script_enter_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_MEAS, CNF, SCRIPT_BUILD_SCRIPTS, RFA_RF_MEAS_MSG_ID_SCRIPT_BUILD_SCRIPTS,rfa_rf_meas_script_build_scripts_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_MEAS, CNF, SCRIPT_EXIT,          RFA_RF_MEAS_MSG_ID_SCRIPT_EXIT,         rfa_rf_meas_script_exit_cnf_s),
};

/*! @brief MEAS subsystem message IDs the RF task receives. 
 * This enum is never called by name. It just defines symbols.
 *
 * RFA_RF_MEAS_<command>_REQ = (MSGR_RFA_RF_MEAS & 0xFFFF) << 16 |
 *                             (MSGR_TYPE_REQ & 0xFF) << 8 |
 *                             (RFA_RF_MEAS_MSG_ID_<command> & 0xFF)
 *
 * RFA_RF_MEAS_REQ_<command> = (MSGR_RFA_RF_MEAS & 0xFFFF) << 16 |
 *                             (MSGR_TYPE_REQ & 0xFF) << 8 |
 *                             (RFA_RF_MEAS_MSG_ID_<command> & 0xFF)
*/
enum
{
   MSGR_DEFINE_UMID(RFA, RF_MEAS, REQ, GET_IRAT_INFO_PARAM,  RFA_RF_MEAS_MSG_ID_GET_IRAT_INFO_PARAM, rfa_rf_meas_get_irat_info_param_req_s),
   MSGR_DEFINE_UMID(RFA, RF_MEAS, REQ, GET_TIME_CONSTANT,    RFA_RF_MEAS_MSG_ID_GET_TIME_CONSTANT,   rfa_rf_meas_get_time_constant_req_s),
   MSGR_DEFINE_UMID(RFA, RF_MEAS, REQ, SCRIPT_ENTER,         RFA_RF_MEAS_MSG_ID_SCRIPT_ENTER,        rfa_rf_meas_script_enter_req_s),
   MSGR_DEFINE_UMID(RFA, RF_MEAS, REQ, SCRIPT_BUILD_SCRIPTS, RFA_RF_MEAS_MSG_ID_SCRIPT_BUILD_SCRIPTS,rfa_rf_meas_script_build_scripts_req_s),
   MSGR_DEFINE_UMID(RFA, RF_MEAS, REQ, SCRIPT_EXIT,          RFA_RF_MEAS_MSG_ID_SCRIPT_EXIT,         rfa_rf_meas_script_exit_req_s),
};

/*! @todo
 *  Structure members from L1 in a MSGR REQ cannot be directly updated by RF.
 *  RF makes a CNF echo of all the structure members that require use and updating.
 */

/*! @enum rfa_rf_meas_cmd_outcome_t */
/*! An enum for the result of a particular MEAS RF command.*/
typedef enum
{
  RFA_RF_MEAS_SUCCESS             =1, /**< Command result for success.*/
  RFA_RF_MEAS_STATE_MACHINE_ERROR =2, /**< Command result for machine error.*/
  RFA_RF_MEAS_EXECUTION_ERROR     =3, /**< Command result for an execution error.*/
  RFA_RF_MEAS_L1_PARAM_ERROR      =4, /**< Command result for a payload error. */
  RFA_RF_MEAS_CMD_OUTCOME_MAX     =5, /**< Maximum number of outcomes.*/
} rfa_rf_meas_cmd_outcome_t;

/*=============================================================================
                   TYPE DEFINITIONS FOR PAYLOAD STRUCTURE
=============================================================================*/

/*=============================================================================
                   RFA_RF_MEAS_MSG_ID_GET_IRAT_INFO_PARAM
=============================================================================*/
/*  RFA_RF_MEAS_GET_IRAT_INFO_PARAM_REQ                                      */
/*! @struct                                                                  */
/*  rfa_rf_meas_get_irat_info_param_req_s                                    */
/*                                                                           */
/*! @brief                                                                   */
/*  This structure is the payload for getting the iRAT timing information    */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc.    */
  msgr_hdr_struct_type          hdr;

  /*! Flag to indicate whether a confirmation message is requried to be sent */
  boolean                       cnf_required;

  /*! Pointer for RF to populate timing information                          */
  rfm_meas_irat_info_type       *rfm_meas_irat_info_param;

  /*! Call back passed from L1 to RF                                         */
  rfm_cb_handler_type           cb_handler;

  /*! Result detailing the success/failure of the command.                   */
  rfa_rf_meas_cmd_outcome_t     req_result;
} rfa_rf_meas_get_irat_info_param_req_s;

/*===========================================================================*/
/*  RFA_RF_MEAS_GET_IRAT_INFO_PARAM_CNF                                      */
/*! @struct                                                                  */
/*  rfa_rf_meas_get_irat_info_param_cnf_s                                    */
/*                                                                           */
/*! @brief                                                                   */
/*  This structure is the payload for getting the iRAT timing information    */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc.    */
  msgr_hdr_struct_type          hdr;

  /*! Flag to indicate whether a confirmation message is requried to be sent */
  boolean                       cnf_required;

  /*! Pointer for RF to populate timing information                          */
  rfm_meas_irat_info_type       *rfm_meas_irat_info_param;

  /*! Call back passed from L1 to RF                                         */
  rfm_cb_handler_type           cb_handler;

  /*! Result detailing the success/failure of the command.                   */
  rfa_rf_meas_cmd_outcome_t     req_result;
} rfa_rf_meas_get_irat_info_param_cnf_s;
/*===========================================================================*/


/*=============================================================================
                   RFA_RF_MEAS_MSG_ID_GET_TIME_CONSTANT
=============================================================================*/
/*  RFA_RF_MEAS_GET_TIME_CONSTANT_REQ                                        */
/*! @struct                                                                  */
/*  rfa_rf_meas_get_time_constant_req_s                                      */
/*                                                                           */
/*! @brief                                                                   */
/*  This structure is the payload for getting iRAT time constants            */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc.    */
  msgr_hdr_struct_type          hdr;

  /*! Flag to indicate whether a confirmation message is requried to be sent */
  boolean                       cnf_required;

  /*! RFCOM serving technology                                               */
  rfcom_mode_enum_type          source;

  /*! RFCOM target technology                                                */
  rfcom_mode_enum_type          target;

  /*! Timing information                                                     */
  rfm_meas_time_const_type      *time_const;

  /*! Result detailing the success/failure of the command.                   */
  rfa_rf_meas_cmd_outcome_t     req_result;
} rfa_rf_meas_get_time_constant_req_s;

/*===========================================================================*/
/*  RFA_RF_MEAS_GET_TIME_CONSTANT_CNF                                        */
/*! @struct                                                                  */
/*  rfa_rf_meas_get_time_constant_cnf_s                                      */
/*                                                                           */
/*! @brief                                                                   */
/*  This structure is the payload for getting the iRAT timing information    */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc.    */
  msgr_hdr_struct_type          hdr;

  /*! Flag to indicate whether a confirmation message is requried to be sent */
  boolean                       cnf_required;

  /*! RFCOM serving technology                                               */
  rfcom_mode_enum_type          source;

  /*! RFCOM target technology                                                */
  rfcom_mode_enum_type          target;

  /*! Timing information                                                     */
  rfm_meas_time_const_type      *time_const;

  /*! Result detailing the success/failure of the command.                   */
  rfa_rf_meas_cmd_outcome_t     req_result;
} rfa_rf_meas_get_time_constant_cnf_s;
/*===========================================================================*/


/*=============================================================================
                   RFA_RF_MEAS_MSG_ID_SCRIPT_ENTER
=============================================================================*/
/*  RFA_RF_MEAS_SCRIPT_ENTER_REQ                                             */
/*! @struct                                                                  */
/*  rfa_rf_meas_script_enter_req_s                                           */
/*                                                                           */
/*! @brief                                                                   */
/*  This structure is the payload for getting the iRAT timing information    */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc.    */
  msgr_hdr_struct_type          hdr;

  /*! Flag to indicate whether a confirmation message is requried to be sent */
  boolean                       cnf_required;

  /*! Params for tune-away enter                                             */
  rfm_meas_enter_param_type     *rfm_meas_enter_param;

  /*! Call back passed from L1 to RF                                         */
  rfm_cb_handler_type           cb_handler;

  /*! Result detailing the success/failure of the command.                   */
  rfa_rf_meas_cmd_outcome_t     req_result;
} rfa_rf_meas_script_enter_req_s;

/*===========================================================================*/
/*  RFA_RF_MEAS_SCRIPT_ENTER_CNF                                             */
/*! @struct                                                                  */
/*  rfa_rf_meas_script_enter_cnf_s                                           */
/*                                                                           */
/*! @brief                                                                   */
/*  This structure is the payload for getting the iRAT timing information    */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc.    */
  msgr_hdr_struct_type          hdr;

  /*! Flag to indicate whether a confirmation message is requried to be sent */
  boolean                       cnf_required;

  /*! Params for tune-away enter                                             */
  rfm_meas_enter_param_type     *rfm_meas_enter_param;

  /*! Call back passed from L1 to RF                                         */
  rfm_cb_handler_type           cb_handler;

  /*! Result detailing the success/failure of the command.                   */
  rfa_rf_meas_cmd_outcome_t     req_result;
} rfa_rf_meas_script_enter_cnf_s;
/*===========================================================================*/


/*=============================================================================
                   RFA_RF_MEAS_MSG_ID_SCRIPT_BUILD_SCRIPTS
=============================================================================*/
/*  RFA_RF_MEAS_SCRIPT_BUILD_SCRIPTS_REQ                                     */
/*! @struct                                                                  */
/*  rfa_rf_meas_script_build_scripts_req_s                                   */
/*                                                                           */
/*! @brief                                                                   */
/*  This structure is the payload for getting the iRAT timing information    */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc.    */
  msgr_hdr_struct_type          hdr;

  /*! Flag to indicate whether a confirmation message is requried to be sent */
  boolean                       cnf_required;

  /*! Structure pointer to hold all information for startup, meas and cleanup*/
  rfm_meas_setup_param_type     *rfm_meas_setup_param;

  /*! Call back passed from L1 to RF                                         */
  rfm_cb_handler_type           cb_handler;

  /*! Result detailing the success/failure of the command.                   */
  rfa_rf_meas_cmd_outcome_t     req_result;
} rfa_rf_meas_script_build_scripts_req_s;

/*===========================================================================*/
/*  RFA_RF_MEAS_SCRIPT_BUILD_SCRIPTS_CNF                                     */
/*! @struct                                                                  */
/*  rfa_rf_meas_script_build_scripts_cnf_s                                   */
/*                                                                           */
/*! @brief                                                                   */
/*  This structure is the payload for getting the iRAT timing information    */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc.    */
  msgr_hdr_struct_type          hdr;

  /*! Flag to indicate whether a confirmation message is requried to be sent */
  boolean                       cnf_required;

  /*! Structure pointer to hold all information for startup, meas and cleanup*/
  rfm_meas_setup_param_type     *rfm_meas_setup_param;

  /*! Call back passed from L1 to RF                                         */
  rfm_cb_handler_type           cb_handler;

  /*! Result detailing the success/failure of the command.                   */
  rfa_rf_meas_cmd_outcome_t     req_result;
} rfa_rf_meas_script_build_scripts_cnf_s;
/*===========================================================================*/


/*=============================================================================
                       RFA_RF_MEAS_MSG_ID_SCRIPT_EXIT
=============================================================================*/
/*  RFA_RF_MEAS_SCRIPT_EXIT_REQ                                              */
/*! @struct                                                                  */
/*  rfa_rf_meas_script_exit_req_s                                            */
/*                                                                           */
/*! @brief                                                                   */
/*  This structure is the payload for getting the iRAT timing information    */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc.    */
  msgr_hdr_struct_type          hdr;

  /*! Flag to indicate whether a confirmation message is requried to be sent */
  boolean                       cnf_required;

  /*! Structure pointer to provide necessary target-tech clean-up            */
  rfm_meas_exit_param_type      *rfm_meas_exit_param;

  /*! Call back passed from L1 to RF                                         */
  rfm_cb_handler_type           cb_handler;

  /*! Result detailing the success/failure of the command.                   */
  rfa_rf_meas_cmd_outcome_t     req_result;
} rfa_rf_meas_script_exit_req_s;

/*===========================================================================*/
/*  RFA_RF_MEAS_SCRIPT_EXIT_CNF                                              */
/*! @struct                                                                  */
/*  rfa_rf_meas_script_exit_cnf_s                                            */
/*                                                                           */
/*! @brief                                                                   */
/*  This structure is the payload for getting the iRAT timing information    */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc.    */
  msgr_hdr_struct_type          hdr;

  /*! Flag to indicate whether a confirmation message is requried to be sent */
  boolean                       cnf_required;

  /*! Structure pointer to provide necessary target-tech clean-up            */
  rfm_meas_exit_param_type      *rfm_meas_exit_param;

  /*! Call back passed from L1 to RF                                         */
  rfm_cb_handler_type           cb_handler;

  /*! Result detailing the success/failure of the command.                   */
  rfa_rf_meas_cmd_outcome_t     req_result;
} rfa_rf_meas_script_exit_cnf_s;
/*===========================================================================*/

#endif /* RF_MEAS_MSG_H */
