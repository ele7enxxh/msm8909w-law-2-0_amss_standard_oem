#ifndef CB_GSM_L1_IF_H
#define CB_GSM_L1_IF_H
/*===========================================================================

                    CB --> GSM L1 INTERFACE HEADER FILE

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cbl1if.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/14/04   vdr     Added new primitives definitions:
                    o L1_CB_SKIP_CNF
                    o L1_CB_SCHEDULING_CNF
                    o L1_CB_MISSING_BLOCK_IND
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

#define CB_PAGING_BLOCK_LENGTH 22

/* 
** The follwing enumerator defines the interface 
** message IDs between CB and GSM L1 tasks
*/
typedef enum cb_gsml1_message_id_tag
{
  CB_SCHEDULING_REQ,
  CB_SKIP_REQ,
  CB_DEACTIVATE_REQ,
  L1_CB_SCHED_EXPIRY_IND,
  L1_CB_SKIP_CNF,
  L1_CB_SCHEDULING_CNF,
  L1_CB_MISSING_BLOCK_IND
} cb_gsml1_message_id_type ;

/*
** The following enumerator defines L1 Reception mode
*/
typedef enum cb_scheduling_mode_tag
{
  CB_DRX_MODE ,
  CB_NON_DRX_MODE
} cb_scheduling_mode_type ;

/* 
** The following enumerator defines possible 
** combination of CBCH which NW could use in 
** order to broadcast CB messages
*/
typedef enum cb_channel_ind_tag
{
  CBCH,
  ECBCH,
  CBCH_ECBCH
} cb_channel_ind_type ;

/*
** The following enumerator defines possible CB_SKIP_REQ 
** types which CB task can use in order either to skip 
** reception of a number of CB blocks during particular 
** list of FNs or to order to GSM L1 completely stop CB 
** reception
*/
typedef enum cb_skip_ind_tag
{
  CB_ERASE_LAST_SCHEDULING, /* Stop CB reception */
  CB_SKIP_FN_LIST           /* Skip number of CB blocks */
} cb_skip_ind_type ;


/* 
** This type provides to GSM L1 list of FNs when
** CB reseption either is required or forbidden
*/
typedef struct cb_scheduling_descriptor_tag
{
  /* 
  ** Number of expected CB paging blocks during next
  ** scheduling period
  */
  byte                    scheduling_period_length ;

  /*
  ** List of the FNs which are belonging to the next 
  ** sceduling period
  */
  dword                  *scheduling_period_fn_list ;
} cb_scheduling_descriptor_type ;

/*
** The follwoing structure describes next CB scheduling 
** period
*/ 
typedef struct cb_scheduling_req_tag
{
  /* Common command header */
  IMH_T message_header;

  /* 
  ** Channel Indicator.
  */
  cb_channel_ind_type channel_ind ;

  /* Required reception mode */
  cb_scheduling_mode_type scheduling_mode ;

  /*
  ** This variables contains description of the next
  ** sceduling period
  ** 
  ** NOTE: This parameter is ignored when CB task 
  **       indicates that CB_NON_DRX_MODE is required
  */
  cb_scheduling_descriptor_type scheduling_descriptor ;


  sys_modem_as_id_e_type as_id;


  /*
  ** SFN to let GL1 identify the end of schedule period
  ** and send L1_CB_SCHED_EXPIRY_IND.
  */
  dword scheduling_period_last_fn;
} cb_scheduling_req_type ;

/* 
** The following structure describes the next skip period
*/
typedef struct cb_skip_req_tag
{
  /* Common command header */
  IMH_T message_header;

  /* 
  ** Channel Indicator.
  */
  cb_channel_ind_type channel_ind ;

  /* Skip indicator */
  cb_skip_ind_type skip_ind ;

  /*
  ** This variables contains FN list is to be skiped
  ** 
  ** NOTE: This parameter is ignored when CB task 
  **       indicates that most recent scheduling 
  **       period is to be erased
  */
  cb_scheduling_descriptor_type scheduling_descriptor ;


  sys_modem_as_id_e_type as_id;


} cb_skip_req_type ;

/* 
** This primitive is sent to GSM L1 in order to stop CBCH 
** processing attempts
*/
typedef struct cb_deactivate_req_tag
{
  IMH_T message_header;

  cb_channel_ind_type channel_ind ;


  sys_modem_as_id_e_type as_id;


} cb_deactivate_req_type ;

typedef struct l1_cb_sched_expiry_ind_tag
{
  IMH_T message_header;
  /* 
  ** Channel Indicator.
  */
  cb_channel_ind_type channel_ind ;


  sys_modem_as_id_e_type as_id;


} l1_cb_sched_expiry_ind_type ;

/* 
** The following structure describes CB_SKIP_CNF
** primitive
*/
typedef struct cb_skip_cnf_tag
{
  /* Common command header */
  IMH_T message_header;

  /* 
  ** Pointer to the recently processed skip FN list,
  ** see cb_skip_req_type for more info
  */
  dword *scheduling_period_fn_list ;


  sys_modem_as_id_e_type as_id;


} l1_cb_skip_cnf_type ;

/* 
** The following structure describes CB_SCHEDULING_CNF
** primitive
*/
typedef struct cb_scheduling_cnf_tag
{
  /* Common command header */
  IMH_T message_header;

  /* 
  ** Pointer to the recently  processed scheduling FN list,
  ** see cb_skip_req_type for more info
  */
  dword *scheduling_period_fn_list ;


  sys_modem_as_id_e_type as_id;


} l1_cb_scheduling_cnf_type ;

/* 
** The following structure describes L1_CB_MISSING_BLOCK_IND
** primitive
*/
typedef struct l1_cb_missing_block_ind_tags
{
  /* Common command header */
  IMH_T message_header;

  dword fn ;
 
  boolean is_l1state_idle;

  sys_modem_as_id_e_type as_id;


} l1_cb_missing_block_ind_type ;



#endif /* #ifndef CB_GSM_L1_IF_H */


