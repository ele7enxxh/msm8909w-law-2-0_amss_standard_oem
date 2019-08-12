#ifndef BMC_L1_IF_H
#define BMC_L1_IF_H
/*===========================================================================

                    BMC --> WCDMA L1 INTERFACE HEADER FILE

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/bmcl1if.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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

/* 
** The follwing enumerator defines the interface 
** message IDs between UMTS L1 and BMC tasks 
*/
typedef enum umtsl1_bmc_message_id_tag
{
  L1_BMC_SCHED_EXPIRY_IND,
  L1_BMC_SKIP_CNF,
  L1_BMC_SCHEDULING_CNF,
  L1_BMC_DROPPED_PDU_IND
} umtsl1_bmc_message_id_type ;

/*
** The following enumerator defines UMTS L1 Reception mode
*/
typedef enum ctch_scheduling_mode_tag
{
  CTCH_DRX_MODE ,
  CTCH_NON_DRX_MODE
} ctch_scheduling_mode_type ;

/* 
** This primitive provides UMTS L1 list of SFNs when
** BMC message reseption either is required or forbidden
*/
typedef struct ctch_scheduling_descriptor_tag
{
  /* 
  ** Number of expected CTCH SFNs during next
  ** scheduling period
  */
  word                    scheduling_period_length ;

  /*
  ** List of the SFNs which are belonging to the next 
  ** scheduling period/skip request
  ** Changed type to support CB_BMC_MAX_ROLLOVER_SFN
  */
  dword                  *scheduling_period_sfn_list ;
} ctch_scheduling_descriptor_type ;

/*
** The follwoing structure describes next CTCH scheduling 
** period
*/ 
typedef struct bmc_l1_scheduling_req_tag
{
  /* Common command header */
  IMH_T message_header;

  /* Required reception mode */
  ctch_scheduling_mode_type scheduling_mode ;

  /*
  ** This variables contains description of the next
  ** sceduling period
  ** 
  ** NOTE: This parameter is ignored when BMC task 
  **       indicates that CTCH_NON_DRX_MODE is required
  */
  ctch_scheduling_descriptor_type scheduling_descriptor ;

  uint16 bmc_schedule_msg_periodicity;

  sys_modem_as_id_e_type as_id;
} bmc_l1_scheduling_req_type ;

/* 
** The following structure describes next skip period
*/
typedef struct bmc_l1_skip_req_tag
{
  /* Common command header */
  IMH_T message_header;

  /*
  ** This variables contains SFN list is to be skiped 
  */
  ctch_scheduling_descriptor_type scheduling_descriptor ;

  sys_modem_as_id_e_type as_id;
} bmc_l1_skip_req_type ;

/* 
** This primitive is sent to UMTS L1 in order to stop CTCH 
** processing in order to put CTCH into dormant mode
*/
typedef struct bmc_l1_deactivate_req_tag
{
  IMH_T message_header;

  sys_modem_as_id_e_type as_id;
} bmc_l1_deactivate_req_type ;

/* 
** This primitive is sent by UMTS L1 to BMC in order to 
** indicate that the last required CTCH SFN has already 
** been monitored
*/
typedef struct l1_bmc_sched_expiry_ind_tag
{
  IMH_T message_header;

  sys_modem_as_id_e_type as_id;
} l1_bmc_sched_expiry_ind_type ;

/* 
** The following structure describes L1_BMC_SKIP_CNF
** primitive
*/
typedef struct l1_bmc_skip_cnf_tag
{
  /* Common command header */
  IMH_T message_header;

  /* 
  ** Pointer to the recently processed skip SFN list,
  ** see bmc_l1_skip_req_type for more info
  */
  dword *scheduling_period_sfn_list ;

  sys_modem_as_id_e_type as_id;
} l1_bmc_skip_cnf_type ;

/* 
** The following structure describes L1_BMC_SCHEDULING_CNF
** primitive
*/
typedef struct l1_bmc_scheduling_cnf_tag
{
  /* Common command header */
  IMH_T message_header;

  /* 
  ** Pointer to recently  processed scheduling SFN list,
  ** see bmc_l1_scheduling_req_type for more info
  */
  dword *scheduling_period_sfn_list ;

  sys_modem_as_id_e_type as_id;
} l1_bmc_scheduling_cnf_type ;

typedef struct l1_bmc_dropped_pdu_ind_tag
{
  /* Common command header */
  IMH_T message_header;

  /*dropped pdu sfn*/
  dword pdu_sfn ;

  sys_modem_as_id_e_type as_id;
}l1_bmc_dropped_pdu_ind_type;


#endif /* #ifndef BMC_L1_IF_H */


