#ifndef TC_CM_H
#define TC_CM_H

/*===========================================================================
                        T C - C M  Header File

DESCRIPTION

   Definition of Primitive message types between Test Control Entity
   and Call Manager Entity (TC <-> CM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/tc_cm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/27/02   mks      Added new enums to notify CM when dummy signaling is 
                    enabled during loopback in UE
03/26/02   mks      Added a new enum CMTC_UE_TEST_SETUP_IN_PROGRESS
01/21/02   mks      Created file.
            
===========================================================================*/

/* ---------------------------------------------------------------------------
* Data types used in the Primitives in CMTC-SAP
* --------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
 Enumeration for UE mode used in CMTC_CHANGE_TEST_MODE_IND message  
** ------------------------------------------------------------------------*/               
typedef enum
{
  CMTC_UE_MODE_TYPE_NONE = -1,
  CMTC_UE_NORMAL_MODE,
  CMTC_UE_LB_MODE1,
  CMTC_UE_LB_MODE2,
  CMTC_UE_LB_MODE1_WITH_DUMMY_SIGNALLING,
  CMTC_UE_LB_MODE2_WITH_DUMMY_SIGNALLING,
  CMTC_UE_TEST_SETUP_IN_PROGRESS,
 
  CMTC_UE_LB_MODE3,
  CMTC_UE_LB_MODE3_WITH_DUMMY_SIGNALLING,
 
  CMTC_UE_MODE_TYPE_MAX
} cmtc_ue_mode_type_T;                                  

/* --------------------------------------------------------------------------
 Enumeration for status indicated by CM in CMTC_CHANGE_TEST_MODE_RSP message
** ------------------------------------------------------------------------*/
typedef enum 
{
  CMTC_CHANGE_TEST_MODE_STATUS_NONE = -1,
  CMTC_REJECT,
  CMTC_CONFIRM,
  CMTC_CHANGE_TEST_MODE_STATUS_MAX
} cmtc_change_test_mode_status_T;

/* --------------------------------------------------------------------------
** Enumeration for stop mode indicated by CM in CMTC_STOP_MODE_REQ message
** ------------------------------------------------------------------------*/
typedef enum 
{
  CMTC_STOP_MODE_TYPE_NONE = -1,
  CMTC_UE_POWER_OFF,  
  CMTC_SIM_ERROR,
  CMTC_STOP_MODE_TYPE_MAX
} cmtc_stop_mode_type_T;

/* ---------------------------------------------------------------------------
* Message Primitive Types between Test Controller (TC) and the CM.
* --------------------------------------------------------------------------*/

/* Command types from TC to CM are defined in CMLL.H*/

/* Note: This primitive will be removed once CM defines in one of its headers */
/* --------------------------------------------------------------------------
** Primitive : CM_CHANGE_TEST_MODE_IND, TC notifies change of test mode to CM
** ------------------------------------------------------------------------*/
typedef struct cm_change_test_mode_ind
{
  cmtc_ue_mode_type_T  mode_type;   // Current mode type
} cm_change_test_mode_ind_T;
             
/* --------------------------------------------------------------------------
** Message Primitive Types between CM and TC. 
** ------------------------------------------------------------------------*/

/* Commands from CM to TC */
/* These are defined in the TCTASK.H file */

/* --------------------------------------------------------------------------
** Primitive : CMTC_CHANGE_TEST_MODE_RSP, notify change test mode resp to TC 
** ------------------------------------------------------------------------*/
typedef struct cmtc_change_test_mode_rsp 
{
  cmtc_change_test_mode_status_T status;
} cmtc_change_test_mode_rsp_T;

/* --------------------------------------------------------------------------
** Primitive : CMTC_STOP_MODE_REQ, used by CM to notify TC of stop mode
** ------------------------------------------------------------------------*/
typedef struct cmtc_stop_mode_req
{
  cmtc_stop_mode_type_T stop_mode;   
} cmtc_stop_mode_req_T;

#endif
