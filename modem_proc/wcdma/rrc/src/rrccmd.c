/*===========================================================================
                          R R C    C O M M A N D S

DESCRIPTION

 This module defines command queues and the necessary
 interface functions for all external and internal
 commands sent to RRC.

 RRC maintains several command queues. These command queues
 are assigned priorities by RRC so that commands from higher priority
 buffers are processed before those from lower priority buffers.

 The priority assignment is not explicit. The RRC dispatcher
 implicitly provides priority by checking each queue
 signal sequentially when an RRC signal is set.

 The following are the command queues maintained by RRC
 (In the order of priority starting from highest) -

 rrc_int_cmd_q - Internal commands
 rrc_l1_cmd_q  - L1 commands
 rrc_mac_cmd_q - MAC commands
 rrc_rlc_cmd_q - RLC commands
 rrc_mm_cmd_q  - MM commands
 rrc_tc_cmd_q  - TC commands
 rrc_rr_cmd_q  - RR commands

EXTERNALIZED FUNCTIONS

 rrc_get_int_cmd_buf   Get an internal command buffer
 rrc_get_l1_cmd_buf    Get an L1 command buffer
 rrc_get_mac_cmd_buf   Get a MAC command buffer
 rrc_get_rlc_cmd_buf   Get an RLC command buffer
 rrc_get_mm_cmd_buf    Get a MM command buffer
 rrc_get_tc_cmd_buf    Get a TC command buffer
 rrc_get_rr_cmd_buf    Get a RR command buffer
 rrc_get_cm_cmd_buf    Get a CM command buffer
 rrc_put_int_cmd       Put an internal command buffer in rrc_int_cmd_q
 rrc_put_l1_cmd        Put an L1 command buffer in rrc_l1_cmd_q
 rrc_put_mac_cmd       Put a mac command buffer in rrc_mac_cmd_q
 rrc_put_rlc_cmd       Put a mac command buffer in rrc_rlc_cmd_q
 rrc_put_mm_cmd        Put a mm command buffer in rrc_mm_cmd_q
 rrc_put_tc_cmd        Put a TC command buffer in rrc_tc_cmd_q
 rrc_put_rr_cmd        Put a RR command buffer in rrc_rr_cmd_q
 rrc_put_cm_cmd        Put a cm command buffer in rrc_cm_cmd_q
 rrc_free_cmd_buf      Free any RRC command buffer

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Queues must be initialized by calling rrc_init_command_queues()
  at RRC task startup.

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrccmd.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/15   vi      Fixed compilation error.
10/07/15   vi      Changes for force DRX and skip idlemode measurements based on QMI request 
05/07/14   vi      Made changes to put the rrc_l1_ff_cmd_q usage under critical section to avoid race condition in FF cmds. 
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
02/03/14   as      Made changes to backup internal/external cmds for crash debugging
12/30/13   sr      Added code to get instance id with AS ID
09/25/13   bc      Added code to support exchange of AS ID when LTE feature is enabled
07/08/13   sn      Changes for LTE CSG feature
01/03/13   ad      Made changes to reduce the redundant F3s
10/23/12   pm      Fixed compiler warning
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
09/10/12   gv      Added support for new interface between WRRC and TDS RRC
08/22/12   jd      RRC changes for online mode
05/30/12   gv      Fixed compilation issue during merging
03/28/12   zr      Adding feature definitions for Dime
01/10/12   gv      Made changes to correctly free the free floating commands
12/12/11   gv      Added code to support free floating of tasks
12/05/11   pm      Added code to support cell id request by CM in DCH state"
12/05/11   mn      Made changes to prevent dangling pointers.
04/26/11   rl      Support for transition from TMC heap to Modem Heap in WCDMA.
02/10/11   rl      Fixed the compilation errors
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   su      Made changes to not Error Fatal when mem_malloc
                   is allocating outside of tmc_heap_mem_buffer[].
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
02/15/10   as      Deleted QTF TEST_FRAMEWORK stub code
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/14/09   as      Fixed lint issues
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
08/03/09   kp      Changes to obsolete gs_alloc/gs_free under feature flag FEATURE MODEM HEAP
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
03/13/09   sks     Added code to free memory allocated for some commands.
                   Changes are for RRC UTF.
01/28/09   dm      Fixed lint errors
12/10/08   ps      Made changes to not allocate memory from static buffers
                   only for the timer expiry Ind, it should be allocated 
                   from heap only
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/27/07   rm      Added code to free the memory allocated for the cell list in
                   inter frequency measurement indication
12/21/07   vr      Added MBMS Service Manager (MSM) queue and supporting access
                   functions
09/07/07   vm      Added support for feature FEATURE_RRC_CALLOC
05/25/07   vg      added support for FEATURE_CGPS and FEATURE_CGPS_UMTS_CP_WCDMA.
05/03/07   vm      Added fix for CR 118055. Added support to free memory from the heap
                   based on the free ptr instead of assuming that memory should be freed
                   from TMC heap.
04/19/07   ps      Made modifications to use a array of static blocks for the
                   allocation of internal commands
12/11/06   ps      To allocate 10 blocks of internal cmd buffer statically and
                   if not not available there, then only go the the heap.Changes have been made
                   under the feature flag FEATURE_RRC_STATIC_BUFFER_ALLOC
06/27/06   vr      Commented out CFA logging for commands that are not supported
                   by CFA library
05/11/05   da      Added debug prints
12/17/04   vr      Added CM queue and its supporting functions for
                   WCDMA -> 1x handover
08/19/04   sgk     Type cast to int cmd_buf->cmd.downlink_sdu_ind.dl_sdu_type
                   in function call rrcasn1_free_pdu((int)cmd_buf->
                   cmd.downlink_sdu_ind.dl_sdu_type ,
                   cmd_buf->cmd.downlink_sdu_ind.decoded_msg) in function
                   rrc_free_cmd_buf to fix lint error Loss of precision
                   (arg. no. 1) (unsigned long to int)
04/13/04   bu      Send the actual RRC command minus the command header when
                   CFA message is logged.
03/22/04   kc      Added definition for rrc_free_embedded_buf().
01/26/04   vk      Defined a new command queue rrc_lsm_cmd_q for LSM to RRC
                   cmds. Also defined functions to enable LSM to post a cmd
                   to RRC under FEATURE_MMGPS
06/18/03   ram     Changed ERRs to ERR_FATALs when there is a memory allocation
                   or deallocation failure.
03/27/03   vk      Deallocated memory allocated for Measurement Report
12/16/02   vk      Changed rrc_free to mem_free for RRC_OPEN_SESSION_REQ
                   and RRC_DATA_REQ since MM uses mem_malloc for
                   embedded buffers.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
09/03/02   xfg     Fixed a dependence issue for single mode build caused by
                   CFA logging
08/23/02   xfg     Added CFA message logging for the commands sent to RRC from
                   other layers including internal RRC commands.
07/15/02   xfg     Add rrc_rr_cmd_q and rrc_get_rr_cmd_buf & rrc_put_rr_cmd
                   functions to support dual-mode inter-RAT HO feature; And
                   also added support for message logging capability.
03/25/02   rj      Modified function rrc_free_cmd_buf to use rrcasn1_free_pdu
                   instead of rrcasn1_free_pdu_with_buf since actual memory
                   leak is fixed.
01/30/02   xfg     Added functions for handling the commands from TC to support
                   loopback test
10/23/01   kc      Updated function rrc_free_cmd_buf to check if decoded msg
                   is NULL and only if not NULL delete its contents.
10/03/01   rj      Updated function rrc_free_cmd_buf to deallocate decoded
                   message pointer with rrcasn1_free_pdu_with_buf in place
                   of rrcasn1_free_pdu.
05/04/01   ram     Removed the MASK used while incrementing the queue debug
                   counts since this statement caused a problem with the
                   ARM compiler. Corrected a typo in the debug message
                   in the queue put functions. Removed FEATURE_RRC_CMD_Q_DEBUG
                   which will be put in the cust file.
03/30/01   ram     Included customer.h header file.
03/15/01   rj      Added code to free the memory for the payload in
                   RRC_OPEN_SESSION_REQ, and RRC_DATA_REQ comamnds before
                   the command is freed.
12/22/00   rj      Modified rrc_free_cmd_buf function. Used actual memory
                   freeing functions instead of template.
12/08/00   ram     Moved CMD_BASE_MASK definiton to rrccmdi.h since it's used
                   by other files also.
12/07/00   rj      Modified rrc_free_cmd_buf to deallocate DL SDU buffer.
12/05/00   ram     Several updates to RRC command queue functions - all the
                   command queues now use the same cmd_type. The free function
                   now has just one parameter passed to it, a pointer to the
                   command that needs to be freed. Updated all functions to
                   use tmc's memory heap and use mem_malloc and mem_free
                   functions to dynamically allocate and free memory.
11/29/00   ram     Updated rrc_free_cmd_buf function according to the latest
                   convention for freeing RRC command buffers.
11/27/00   ram     Added get and put functions for l1, mac, rlc and mm command
                   queues.
11/15/00   ram     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "err.h"
#include "msg.h"
#include "rex.h"
#include "queue.h"
#include "rrccmd_v.h"
#include "rrccmdi.h"
#include "rrcsigs.h"
#include "task.h"
#include "rrcdata_v.h"
#include "rrcasn1util.h"
#include "cfa.h"           /* For CFA Logging */
#include "ms.h"            /* For CFA Logging - MSG_SET IDs */
#include "gs.h"            /* For CFA Logging - QUEUE IDs*/
#include "rrcrrif.h"
#include "rrcmeas.h"
#include "rrc_tdsrrc_if.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#ifdef FEATURE_RRC_CMD_Q_DEBUG
#error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
#define MAX_CMD_WM 2

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */



/*--------------------------------------------------------*/
/*                                                        */
/*                 RRC COMMAND QUEUES                     */
/*                                                        */
/*  All Command Queues for the RRC Task are defined here  */
/*--------------------------------------------------------*/

/* Queue for internal RRC commands within the RRC task    */
q_type rrc_int_cmd_q;

/* Queue for RRC commands sent by L1 task                 */
q_type rrc_l1_cmd_q;

/* Queue for RRC commands sent by MAC task                */
q_type rrc_mac_cmd_q;

/* Queue for RRC commands sent by RLC task                */
q_type rrc_rlc_cmd_q;

/* Queue for RRC commands sent by MM task                 */
q_type rrc_mm_cmd_q;

/* Queue for RRC commands sent by TC task                 */
q_type rrc_tc_cmd_q;

/* Queue for RRC commands sent by RR task                 */
q_type rrc_rr_cmd_q;

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
/* Queue for RRC commands sent by LSM task                 */
q_type rrc_lsm_cmd_q;
#endif

/* Queue for RRC commands sent by CM task                 */
q_type rrc_cm_cmd_q;

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
/*Queue type for WCDMA RRC cmds from LTE RRC*/
q_type rrc_lte_cmd_q,rrc_lte_free_cmd_q;
#define MAX_WCDMA_RRC_LTE_IRAT_CMDS 3
static wcdma_rrc_lte_irat_cmd_type wcdma_rrc_lte_irat_cmd_pool[MAX_WCDMA_RRC_LTE_IRAT_CMDS];
#endif

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/*Queue type for WL1 sending free floating cmds to RRC*/
q_type rrc_l1_ff_cmd_q;
#endif

#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
q_type rrc_tdsrrc_cmd_q;
#endif

/*Queue type for WCDMA RRC cmds from QMI*/
  q_type rrc_qmi_cmd_q;
  q_type rrc_qmi_free_cmd_q;

#define MAX_WCDMA_RRC_QMI_CMDS 3
static wcdma_rrc_qmi_cmd_type wcdma_rrc_qmi_cmd_pool[MAX_WCDMA_RRC_QMI_CMDS];

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
/* Any forward declarations go here */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */



/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */


/*===========================================================================

FUNCTION RRC_INIT_COMMAND_QUEUES

DESCRIPTION

  This function initializes the RRC command queues. This function needs
  to be called once at the time of the RRC task start-up.

DEPENDENCIES

  This function assumes that all REX signals for RRC task have been cleared
  before this function is called.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
 /***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_init_command_queues( void )
{
  uint8 pool_idx = 0;
  /* Initialize all the command queues */
  (void)q_init(&rrc_int_cmd_q);
  (void)q_init(&rrc_l1_cmd_q);
  (void)q_init(&rrc_mac_cmd_q);
  (void)q_init(&rrc_rlc_cmd_q);
  (void)q_init(&rrc_mm_cmd_q);
  (void)q_init(&rrc_tc_cmd_q);
  (void)q_init(&rrc_rr_cmd_q);

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
  (void)q_init(&rrc_lsm_cmd_q);
#endif

  (void)q_init(&rrc_cm_cmd_q);

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
  (void)q_init(&rrc_lte_cmd_q);
  (void)q_init(&rrc_lte_free_cmd_q);
  for(pool_idx = 0; pool_idx <MAX_WCDMA_RRC_LTE_IRAT_CMDS; pool_idx++)
  {
    wcdma_rrc_lte_irat_cmd_type *cmd_ptr = &wcdma_rrc_lte_irat_cmd_pool[pool_idx];
    (void) q_link(cmd_ptr, &(cmd_ptr->link));
    q_put(&rrc_lte_free_cmd_q,&(cmd_ptr->link));
  }
#endif

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
(void)q_init(&rrc_l1_ff_cmd_q);
#endif

#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
(void)q_init(&rrc_tdsrrc_cmd_q);
#endif

  (void)q_init(&rrc_qmi_cmd_q);
  (void)q_init(&rrc_qmi_free_cmd_q);
  for(pool_idx = 0; pool_idx <MAX_WCDMA_RRC_QMI_CMDS; pool_idx++)
  {
    wcdma_rrc_qmi_cmd_type *cmd_ptr = &wcdma_rrc_qmi_cmd_pool[pool_idx];
    (void) q_link(cmd_ptr, &(cmd_ptr->link));
    q_put(&rrc_qmi_free_cmd_q,&(cmd_ptr->link));
  }

}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*===========================================================================
FUNCTION rrc_get_int_cmd_buf_from_heap_only


DESCRIPTION

  This function returns a internal command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.
 The function allocates memory from heap only and not from static buffer
DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_cmd_type *rrc_get_int_cmd_buf_from_heap_only( void )
{
  rrc_cmd_int_type *ptr;

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */
  /* Allocate from the static buffer, if available first. */

  if( (ptr = (rrc_cmd_int_type *)rrc_malloc(sizeof(rrc_cmd_int_type)))
      == NULL )
  {
    WRRC_MSG0_ERROR("Could not allocate memory for RRC Internal cmd q!");
  }

  return (rrc_cmd_type *)ptr;

}

/*===========================================================================
FUNCTION RRC_GET_INT_CMD_BUF


DESCRIPTION

  This function returns a internal command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_cmd_type *rrc_get_int_cmd_buf( void )
{

  rrc_cmd_int_type *ptr;
#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */
  /* Allocate from the static buffer, if available first. */

  if( (ptr = rrc_malloc(sizeof(rrc_cmd_int_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC Internal cmd q!", 0, 0, 0);
  }
  return((rrc_cmd_type *)ptr);

}


/*===========================================================================

FUNCTION RRC_GET_L1_CMD_BUF

DESCRIPTION

  This function returns a L1 command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_cmd_type *rrc_get_l1_cmd_buf( void )
{
  rrc_cmd_type *ptr;

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* Allocate memory */
  if( (ptr = (rrc_cmd_type *)rrc_malloc(sizeof(rrc_cmd_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC L1 cmd q!", 0, 0, 0);
  }
  return(ptr);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/



#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/*===========================================================================

FUNCTION rrc_get_l1_ff_cmd_buf

DESCRIPTION

  This function returns a L1 command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_cmd_type *rrc_get_l1_ff_cmd_buf( void )
{
  rrc_cmd_type *ptr;

  /* Allocate memory */
  if( (ptr = (rrc_cmd_type *)rrc_malloc(sizeof(rrc_cmd_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC L1 FF cmd q!", 0, 0, 0);
  }
  return(ptr);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
#endif

/*===========================================================================

FUNCTION RRC_GET_MAC_CMD_BUF

DESCRIPTION

  This function returns a MAC command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_cmd_type *rrc_get_mac_cmd_buf( void )
{
  rrc_cmd_type *ptr;

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* Allocate memory */
  if( (ptr = (rrc_cmd_type *)rrc_malloc( sizeof(rrc_cmd_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC MAC cmd q!", 0, 0, 0);
  }
  return(ptr);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/




/*===========================================================================

FUNCTION RRC_GET_RLC_CMD_BUF

DESCRIPTION

  This function returns a RLC command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_cmd_type *rrc_get_rlc_cmd_buf( void )
{
  rrc_cmd_type *ptr;

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* Allocate memory */
  if( (ptr = (rrc_cmd_type *)rrc_malloc(sizeof(rrc_cmd_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC RLC cmd q!", 0, 0, 0);
  }
  return(ptr);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/




/*===========================================================================

FUNCTION RRC_GET_MM_CMD_BUF

DESCRIPTION

  This function returns a MM command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_cmd_type *rrc_get_mm_cmd_buf( void )
{
  rrc_cmd_type *ptr;

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* Allocate memory */
  if( (ptr = (rrc_cmd_type *)rrc_malloc(sizeof(rrc_cmd_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC MM cmd q!", 0, 0, 0);
  }
  return(ptr);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/


/*===========================================================================

FUNCTION RRC_GET_TC_CMD_BUF

DESCRIPTION

  This function returns a TC command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_cmd_type *rrc_get_tc_cmd_buf( void )
{
  rrc_cmd_type *ptr;

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* Allocate memory */
  if( (ptr = (rrc_cmd_type *)rrc_malloc( sizeof(rrc_cmd_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC TC cmd q!", 0, 0, 0);
  }
  return(ptr);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
/*===========================================================================

FUNCTION RRC_GET_RR_CMD_BUF

DESCRIPTION

  This function returns a RR command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_rr_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_rr_cmd_type *rrc_get_rr_cmd_buf( void )
{
  rrc_rr_cmd_type *ptr;

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* Allocate memory */
  if( (ptr = (rrc_rr_cmd_type *)rrc_malloc( sizeof(rrc_rr_cmd_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC RR cmd q!", 0, 0, 0);
  }
  return(ptr);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
/*===========================================================================

FUNCTION RRC_GET_TRRC_CMD_BUF

DESCRIPTION

  This function returns a TDS RRC command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_tdsrrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_tdsrrc_cmd_type *rrc_get_tdsrrc_cmd_buf( void )
{
  rrc_tdsrrc_cmd_type *ptr;

  /* Allocate memory */
  if( (ptr = (rrc_tdsrrc_cmd_type *)rrc_malloc( sizeof(rrc_tdsrrc_cmd_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC RR cmd q!", 0, 0, 0);
  }
  return(ptr);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
#endif

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)

/*===========================================================================

FUNCTION RRC_GET_LSM_CMD_BUF

DESCRIPTION

  This function returns a LSM command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_cmd_type *rrc_get_lsm_cmd_buf( void )
{
  rrc_cmd_type *ptr;

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* Allocate memory */
  if( (ptr = (rrc_cmd_type *)rrc_malloc(sizeof(rrc_cmd_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC LSM cmd q!", 0, 0, 0);
  }
  return(ptr);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
#endif


/*===========================================================================

FUNCTION RRC_GET_CM_CMD_BUF

DESCRIPTION

  This function returns a CM command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_cmd_type *rrc_get_cm_cmd_buf( void )
{
  rrc_cmd_type *ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate memory.
  */
  if( (ptr = (rrc_cmd_type *)rrc_malloc(sizeof(rrc_cmd_type)))
      == NULL )
  {
    ERR_FATAL("Could not allocate memory for RRC CM cmd q!", 0, 0, 0);
  }
  return(ptr);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*===========================================================================

FUNCTION RRC_PUT_INT_CMD

DESCRIPTION

  This function puts a command buffer into RRC's internal
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_int_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_int_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{
  /* Enable CFA logging when commands are supported by CFA library */
  //cfa_log_packet_ex( GS_QUEUE_RRC,                   /* QUEUE */
  //                   MS_WCDMA_RRC_INT,               /* the message set */
  //                   (byte)cmd_buf->cmd_hdr.cmd_id,  /* the message id  */
  //                   255,                            /* first 255 for now */
  //                   (byte*)&(cmd_buf->cmd) );       /* Pointer to the message */

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_int_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_int_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in int-q, last cmd:0x%x",
             q_cnt(&rrc_int_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_INT_CMD_Q_SIG);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/


/*===========================================================================

FUNCTION RRC_PUT_L1_CMD

DESCRIPTION

  This function puts a command buffer into RRC's l1
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_l1_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_l1_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{
  /* Enable CFA logging when commands are supported by CFA library */
  //cfa_log_packet_ex( GS_QUEUE_RRC,                  /* QUEUE */
  //                   MS_WCDMA_RRC_L1,               /* the message set */
  //                   (byte)cmd_buf->cmd_hdr.cmd_id, /* the message id  */
  //                   255,                           /* first 255 for now */
  //                   (byte*)&(cmd_buf->cmd) );      /* Pointer to the message */

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_l1_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_l1_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in l1-q, last cmd:0x%x",
             q_cnt(&rrc_l1_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_L1_CMD_Q_SIG);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/*===========================================================================

FUNCTION rrc_put_l1_ff_cmd

DESCRIPTION

  This function puts a command buffer into RRC's l1 FF
  command queue. Note that the command buffer must have been
  requested through a call to rrc_get_l1_ff_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_l1_ff_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{
  REX_ISR_LOCK(&rrc_l1_ff_cmd_crit_sect);

  /* Enable CFA logging when commands are supported by CFA library */
  //cfa_log_packet_ex( GS_QUEUE_RRC,                  /* QUEUE */
  //                   MS_WCDMA_RRC_L1,               /* the message set */
  //                   (byte)cmd_buf->cmd_hdr.cmd_id, /* the message id  */
  //                   255,                           /* first 255 for now */
  //                   (byte*)&(cmd_buf->cmd) );      /* Pointer to the message */

  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_l1_ff_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_l1_ff_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in l1-q, last cmd:0x%x",
             q_cnt(&rrc_l1_ff_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_L1_FF_CMD_Q_SIG);
  REX_ISR_UNLOCK(&rrc_l1_ff_cmd_crit_sect);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
#endif
/*===========================================================================

FUNCTION RRC_PUT_MAC_CMD

DESCRIPTION

  This function puts a command buffer into RRC's mac
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_mac_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_mac_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{
  /* Enable CFA logging when commands are supported by CFA library */
  //cfa_log_packet_ex( GS_QUEUE_RRC,                  /* QUEUE */
  //                   MS_WCDMA_RRC_MAC,              /* the message set */
  //                   (byte)cmd_buf->cmd_hdr.cmd_id, /* the message id  */
  //                   255,                           /* first 255 for now */
  //                   (byte*)&(cmd_buf->cmd) );      /* Pointer to the message */

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_mac_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_mac_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in mac-q, last cmd:0x%x",
             q_cnt(&rrc_mac_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_MAC_CMD_Q_SIG);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*===========================================================================

FUNCTION RRC_PUT_RLC_CMD

DESCRIPTION

  This function puts a command buffer into RRC's rlc
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_rlc_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_rlc_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{
  /* Enable CFA logging when commands are supported by CFA library */
  //cfa_log_packet_ex( GS_QUEUE_RRC,                   /* QUEUE */
  //                   MS_WCDMA_RRC_RLC,               /* the message set */
  //                   (byte)cmd_buf->cmd_hdr.cmd_id,  /* the message id  */
  //                   255,                            /* first 255 for now */
  //                   (byte*)&(cmd_buf->cmd) );       /* Pointer to the message */

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_rlc_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_rlc_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in rlc-q, last cmd:0x%x",
             q_cnt(&rrc_rlc_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_RLC_CMD_Q_SIG);

}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
/*===========================================================================

FUNCTION RRC_PUT_MM_CMD

DESCRIPTION

  This function puts a command buffer into RRC's mm
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_mm_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_mm_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{
  /* Log the message: first 256 bytes */
  cfa_log_packet_ex( GS_QUEUE_RRC,                   /* QUEUE */
                     MS_MM_RRC,                      /* the message set */
                     (byte)cmd_buf->cmd_hdr.cmd_id,  /* the message id  */
                     255,                            /* First 255 for now */
                     (byte*)&(cmd_buf->cmd) );       /* Pointer to the message */

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_mm_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_mm_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in mm-q, last cmd:0x%x",
             q_cnt(&rrc_mm_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_MM_CMD_Q_SIG);

}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/


/*===========================================================================

FUNCTION RRC_PUT_TC_CMD

DESCRIPTION

  This function puts a command buffer into RRC's mm
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_tc_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_tc_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{
  /* Enable CFA logging when commands are supported by CFA library */
  //cfa_log_packet_ex( GS_QUEUE_RRC,                   /* QUEUE */
  //                   MS_RRC_TC,                      /* the message set */
  //                   (byte)cmd_buf->cmd_hdr.cmd_id,  /* the message id  */
  //                   255,                            /* first 255 for now */
  //                   (byte*)&(cmd_buf->cmd) );       /* Pointer to the message */

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_tc_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_tc_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in tc-q, last cmd:0x%x",
             q_cnt(&rrc_tc_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_TC_CMD_Q_SIG);

}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
/*===========================================================================

FUNCTION RRC_PUT_RR_CMD

DESCRIPTION

  This function puts a command buffer into RRC's mm
  command queue. Note that the command buffer must have been
  requested through a call to rrc_get_rr_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_rr_cmd
(
  rrc_rr_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{
  /* Log the message: first 256 bytes */
  cfa_log_packet_ex( GS_QUEUE_RRC,                   /* QUEUE */
                     MS_RRC_RR,                      /* the message set */
                     (byte)cmd_buf->cmd_hdr.cmd_id,  /* the message id  */
                     255,                            /* First 255 for now */
                     (byte*)&(cmd_buf->cmd) );       /* Pointer to the message */

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_rr_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_rr_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in tc-q, last cmd:0x%x",
             q_cnt(&rrc_rr_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_RR_CMD_Q_SIG);

}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
/*===========================================================================

FUNCTION RRC_PUT_TRRC_CMD

DESCRIPTION

  This function puts a command buffer into RRC's mm
  command queue. Note that the command buffer must have been
  requested through a call to rrc_get_rr_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_tdsrrc_cmd
(
  rrc_tdsrrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{

  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_tdsrrc_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_tdsrrc_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in tc-q, last cmd:0x%x",
             q_cnt(&rrc_tdsrrc_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_TDSRRC_CMD_Q_SIG);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
#endif

/*===========================================================================

FUNCTION RRC_PUT_CM_CMD

DESCRIPTION

  This function puts a command buffer into RRC's cm
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_cm_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_cm_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{
  /* Enable CFA logging when commands are supported by CFA library */

  //cfa_log_packet_ex( GS_QUEUE_RRC,              /* QUEUE */
  //                   MS_CM_RRC,                 /* the message set */
  //                   cmd_buf->cmd_hdr.cmd_id,   /* the message id  */
  //                   255,                       /* first 255 for now */
  //                   (byte*)&(cmd_buf->cmd) );  /* Pointer to the message */


  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_cm_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_cm_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in cm-q, last cmd:0x%x",
             q_cnt(&rrc_cm_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_CM_CMD_Q_SIG);

}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/


/*===========================================================================

FUNCTION rrccmd_free_internal_req_ptr

DESCRIPTION

  This function frees the internal pointers present in the internal commands

DEPENDENCIES


RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrccmd_free_internal_req_ptr
(
  rrc_cmd_type *cmd_buf 
)
{
  if(cmd_buf->cmd_hdr.cmd_id == RRC_LLC_CMD_REQ)
  {
    if(cmd_buf->cmd.rrc_llc_req.cmd_dest == LAYER1_PHY)
    {
      if(cmd_buf->cmd.rrc_llc_req.l1_req != NULL )
      {
        rrc_free(cmd_buf->cmd.rrc_llc_req.l1_req);
        cmd_buf->cmd.rrc_llc_req.l1_req = NULL;
      }
    }
    else if((cmd_buf->cmd.rrc_llc_req.cmd_dest == LAYER2_MAC_UL)
      ||(cmd_buf->cmd.rrc_llc_req.cmd_dest == LAYER2_RLC_UL))
    {
      if(cmd_buf->cmd.rrc_llc_req.l2_ul_req !=NULL )
      {
        rrc_free(cmd_buf->cmd.rrc_llc_req.l2_ul_req);
        cmd_buf->cmd.rrc_llc_req.l2_ul_req = NULL;
      }
    }
    else if((cmd_buf->cmd.rrc_llc_req.cmd_dest == LAYER2_RLC_DL)
   	||(cmd_buf->cmd.rrc_llc_req.cmd_dest == LAYER2_MAC_DL))
    {
      if(cmd_buf->cmd.rrc_llc_req.l2_dl_req !=NULL )
      {
        rrc_free(cmd_buf->cmd.rrc_llc_req.l2_dl_req);
        cmd_buf->cmd.rrc_llc_req.l2_dl_req = NULL;
      }
    }
    else
    {
      // Add other commands requiring embedded pointers to be freed here
      WRRC_MSG1_ERROR("Unknown type of llc cmd re %d",cmd_buf->cmd.rrc_llc_req.cmd_dest);
    }
  }
}
/*===========================================================================

FUNCTION rrc_free_embedd_buf_llc_req

DESCRIPTION

  This function frees the internal pointers present in the internal LLC commands

DEPENDENCIES


RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_free_embedd_buf_llc_req
(
    rrc_cmd_type *cmd_buf
)
{
  if(cmd_buf->cmd_hdr.cmd_id == RRC_LLC_CMD_REQ)
  {
    if(cmd_buf->cmd.rrc_llc_req.cmd_dest == LAYER1_PHY)
    {
      if(cmd_buf->cmd.rrc_llc_req.l1_req != NULL)
      {
        if(cmd_buf->cmd.rrc_llc_req.l1_cmd.cmd_id == CPHY_MEASUREMENT_REQ)
        {
          if(cmd_buf->cmd.rrc_llc_req.l1_req->meas.meas_ptr)
          {
            rrc_free_interlayer_cmd(cmd_buf->cmd.rrc_llc_req.l1_req->meas.meas_ptr);
            cmd_buf->cmd.rrc_llc_req.l1_req->meas.meas_ptr = NULL;
          }
        }
        /*Free embedded L1 ptr */
        rrc_free(cmd_buf->cmd.rrc_llc_req.l1_req);
        cmd_buf->cmd.rrc_llc_req.l1_req = NULL;
      }
    }
    else if((cmd_buf->cmd.rrc_llc_req.cmd_dest == LAYER2_MAC_UL)
        ||(cmd_buf->cmd.rrc_llc_req.cmd_dest == LAYER2_RLC_UL))
    {
      if(cmd_buf->cmd.rrc_llc_req.l2_ul_req !=NULL)
      {
        if(cmd_buf->cmd.rrc_llc_req.l2_ul_cmd.cmd_id == CMAC_UL_TRAFFIC_VOLUME_CONFIG_REQ)
        {
           if(cmd_buf->cmd.rrc_llc_req.l2_ul_req->mac_ul_traffic_vol_config)
           {
             rrc_free_interlayer_cmd(cmd_buf->cmd.rrc_llc_req.l2_ul_req->mac_ul_traffic_vol_config);
             cmd_buf->cmd.rrc_llc_req.l2_ul_req->mac_ul_traffic_vol_config = NULL;
           }
        }
        /*Free embedded L2 Ul ptr */
        rrc_free(cmd_buf->cmd.rrc_llc_req.l2_ul_req);
        cmd_buf->cmd.rrc_llc_req.l2_ul_req = NULL;
      }
    }
    else if((cmd_buf->cmd.rrc_llc_req.cmd_dest == LAYER2_RLC_DL)
        ||(cmd_buf->cmd.rrc_llc_req.cmd_dest == LAYER2_MAC_DL))
    {
      if(cmd_buf->cmd.rrc_llc_req.l2_dl_req !=NULL )
      {
        /*Free embedded L2 Dl ptr */
        rrc_free(cmd_buf->cmd.rrc_llc_req.l2_dl_req);
        cmd_buf->cmd.rrc_llc_req.l2_dl_req = NULL;
      }
    }
    else
    {
      // Add other commands requiring embedded pointers to be freed here
    }
  }
  
}


/*===========================================================================

FUNCTION RRC_FREE_CMD_BUF

DESCRIPTION

  This function frees a command buffer removed from any RRC
  command queue. Only commands removed from RRC's command queues
  can be passed into this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_free_cmd_buf
(
  rrc_cmd_type *cmd_buf    /* Command Buffer                               */
)
{


#ifdef FEATURE_RRC_CMD_Q_DEBUG
#error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* Check here if command id RRC_DOWNLINK_SDU_IND. If so, then
     deallocated DL SDU before freeing command buffer. */
  switch(cmd_buf->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_SDU_IND:

      if(cmd_buf->cmd.downlink_sdu_ind.dl_sdu != NULL)
      {
        //Free the memory allocated for the received downlink SDU.
        rrc_free( cmd_buf->cmd.downlink_sdu_ind.dl_sdu );
      }

      if( cmd_buf->cmd.downlink_sdu_ind.decoded_msg != NULL)
      {

        // deallocate cmd_ptr->cmd.downlink_sdu_ind.decoded_msg here.
        if(rrcasn1_free_pdu((int)cmd_buf->cmd.downlink_sdu_ind.dl_sdu_type ,
                            cmd_buf->cmd.downlink_sdu_ind.decoded_msg))
        {
          // This is a memory leak and it should never happen.
          ERR_FATAL("Memory freeing is unsuccessful",0,0,0);
        }
        cmd_buf->cmd.downlink_sdu_ind.decoded_msg =NULL;
      }
      break;
    case RRC_LLC_CMD_REQ:
      rrccmd_free_internal_req_ptr(cmd_buf);
      break;
    case RRC_OPEN_SESSION_REQ:
#ifdef FEATURE_RRC_UNIT_TEST
      #error code not present
#else
      rrc_free_embedded_buf(cmd_buf, cmd_buf->cmd.open_sess_req.nas_msg.value);
      cmd_buf->cmd.open_sess_req.nas_msg.value = NULL;
#endif
      break;

    case RRC_DATA_REQ:
#ifdef FEATURE_RRC_UNIT_TEST
      #error code not present
#else
      rrc_free_embedded_buf(cmd_buf, cmd_buf->cmd.data_req.nas_msg.value);
      cmd_buf->cmd.data_req.nas_msg.value = NULL;
#endif
      break;

    case RRC_CPHY_MEASUREMENT_IND:
#ifdef FEATURE_RRC_UNIT_TEST
      #error code not present
#else
      rrc_free_embedded_buf(cmd_buf, cmd_buf->cmd.meas_ind.meas_rpt);
      /*Dont bother setting NULL after freeing taken care inside the above function*/
#endif
      break;

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
    case RRC_MEASUREMENT_IND:
#ifdef FEATURE_RRC_UNIT_TEST
      #error code not present
#else
      rrc_free_embedded_buf(cmd_buf, (void *)cmd_buf->cmd.meas_rpt_ind.meas_rpt);
      cmd_buf->cmd.meas_rpt_ind.meas_rpt = NULL;
#endif
      break;
#endif

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
    case RRC_CPHY_UTRAN_NMR_INFO_CNF:
#ifdef FEATURE_RRC_UNIT_TEST
      #error code not present
#else
      rrc_free_embedded_buf(cmd_buf, (void *)cmd_buf->cmd.nmr_cnf.l1_meas_utran_nmr_info);
#endif
      break;

    case RRC_CPHY_GET_IMM_MEAS_RSP:
#ifdef FEATURE_RRC_UNIT_TEST
      #error code not present
#else
      rrc_free_embedded_buf(cmd_buf, (void *)cmd_buf->cmd.imm_meas_cnf.immed_meas);
      cmd_buf->cmd.imm_meas_cnf.immed_meas = NULL;
#endif
      break;

    case RRC_CPHY_MODEM_STATISTICS_RSP:
#ifdef FEATURE_RRC_UNIT_TEST
      #error code not present
#else
      rrc_free_embedded_buf(cmd_buf, (void *)cmd_buf->cmd.modem_stat_rsp.l1_serving_cell_power_info);
      cmd_buf->cmd.modem_stat_rsp.l1_serving_cell_power_info = NULL;
#endif
#ifdef FEATURE_RRC_UNIT_TEST
      #error code not present
#else
      rrc_free_embedded_buf(cmd_buf, (void *)cmd_buf->cmd.modem_stat_rsp.l1_active_set_info);
      cmd_buf->cmd.modem_stat_rsp.l1_active_set_info = NULL;
#endif
#ifdef FEATURE_RRC_UNIT_TEST
      #error code not present
#else
      rrc_free_embedded_buf(cmd_buf, (void *)cmd_buf->cmd.modem_stat_rsp.l1_monitored_set_info);
      cmd_buf->cmd.modem_stat_rsp.l1_monitored_set_info = NULL;
#endif
      break;
#endif


    default:
      // Do nothing
      break;
  }
  rrc_free( cmd_buf );
} /* rrc_free_cmd_buf */
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
/*===========================================================================

FUNCTION RRC_FREE_RR_CMD_BUF

DESCRIPTION

  This function frees a RR command buffer removed from RRC's RR
  command queue. Only commands removed from RRC's RR command queue
  can be passed into this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_free_rr_cmd_buf
(
  rrc_rr_cmd_type *cmd_buf    /* Command Buffer */
)
{
#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif
  rrc_free( cmd_buf );
} /* end of rrc_free_rr_cmd_buf */
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
/*===========================================================================

FUNCTION RRC_FREE_TDSRRC_CMD_BUF

DESCRIPTION

  This function frees a TRRC command buffer removed from RRC's TRRC
  command queue. Only commands removed from RRC's TRRC command queue
  can be passed into this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_free_tdsrrc_cmd_buf
(
  rrc_cmd_type *cmd_buf    /* Command Buffer */
)
{
  rrc_free( cmd_buf );
} /* end of rrc_free_tdsrrc_cmd_buf */
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
#endif

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
/*===========================================================================

FUNCTION RRC_PUT_LSM_CMD

DESCRIPTION

  This function puts a command buffer into RRC's LSM
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_lsm_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_put_lsm_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
)
{
  /* Enable CFA logging when commands are supported by CFA library */
  //cfa_log_packet_ex( GS_QUEUE_RRC,              /* QUEUE */
  //  MS_LSM_RRC,                                 /* the message set */
  //  (byte)cmd_buf->cmd_hdr.cmd_id,              /* the message id  */
  //  255,                                        /* first 255 for now */
  //  (byte*)&(cmd_buf->cmd) );                   /* Pointer to the message */

#ifdef FEATURE_RRC_CMD_Q_DEBUG
  #error code not present
#endif /* FEATURE_RRC_CMD_Q_DEBUG */

  /* First initialize the queue link. This is always done
  before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->cmd_hdr.link));

  /* Then put the buffer in the queue */
  q_put(&rrc_lsm_cmd_q, &(cmd_buf->cmd_hdr.link));

  /* if number of commands in que has already reached wm indicate a warning */
  if (MAX_CMD_WM < q_cnt(&rrc_lsm_cmd_q))
  {
    WRRC_MSG2_HIGH("%d cmds in lsm-q, last cmd:0x%x",
             q_cnt(&rrc_lsm_cmd_q), cmd_buf->cmd_hdr.cmd_id);
  }

  /* Then set the signal for this queue */
  (void) rex_set_sigs(wcdma_rrc_get_tcb(), RRC_LSM_CMD_Q_SIG);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
#endif

/*====================================================================
FUNCTION: rrc_free_embedded_buf()

DESCRIPTION:
  This function frees an embedded pointer (if any) from the command,
  queue.  Embedded memory allocations are not freed by rrc_free_cmd_buf.
  Only the memory allocated for rrc_cmd_type is freed.  This is needed
  to free memory allocations in CPHY_MEASUREMENT_REQ and TVM_REQ from
  RRC to L1 and RRC to MAC, when they are not routed through LLC.  If
  these commands are routed to LLC, LLC takes care of sending them to
  L1/MAC and deleting them if they are not routed and discarded.
  This function also frees memory allocated for the NAS Payload and
  measurement ind

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

=====================================================================*/
 /***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrc_free_embedded_buf
(
  rrc_cmd_type *cmd_buf,    /* Command Buffer */
  void         *ptr         /* ptr to the payload that needs to be freed */
)
{
  switch(cmd_buf->cmd_hdr.cmd_id)
  {
    case RRC_LLC_CMD_REQ:
      rrc_free_embedd_buf_llc_req(cmd_buf);
      break;
      
    case RRC_OPEN_SESSION_REQ:
    case RRC_DATA_REQ:
    case RRC_CPHY_MEASUREMENT_IND:
    case RRC_MEASUREMENT_IND:
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
    case RRC_CPHY_UTRAN_NMR_INFO_CNF:
    case RRC_CPHY_GET_IMM_MEAS_RSP:
    case RRC_CPHY_MODEM_STATISTICS_RSP:
#endif
      if(cmd_buf->cmd_hdr.cmd_id == RRC_CPHY_MEASUREMENT_IND)
      {
        rrcifreq_free_memory_allocated_for_cell_list_in_meas_ind(cmd_buf);
      }
      if((cmd_buf->cmd_hdr.cmd_id == RRC_OPEN_SESSION_REQ)||
        (cmd_buf->cmd_hdr.cmd_id == RRC_DATA_REQ))
      {
        rrc_free_for_external_cmd(ptr);
      }
      else
      {
        rrc_free_interlayer_cmd(ptr);
      }
      break;

    default:
      WRRC_MSG1_ERROR("ERR: handling of cmd 0x%x not supported", cmd_buf->cmd_hdr.cmd_id);
      break;
  }
} /* rrc_free_embedded_buf */


/*====================================================================
FUNCTION: rrc_send_lte_cmd

DESCRIPTION:
Sends command to LTE RRC

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

=====================================================================*/
void rrc_send_lte_cmd(msgr_hdr_struct_type* hdr, msgr_umid_type umid, uint32 size)
{
  WRRC_MSG1_HIGH_OPT("Dispatching out LTE cmd wcdma_lte_event_umid_value_0x%x",umid );
#ifdef FEATURE_DUAL_SIM
  msgr_init_hdr_inst(hdr,MSGR_WCDMA_RRC,umid,SYS_AS_ID_TO_INST_ID(rrc_get_as_id()));
#else
  msgr_init_hdr(hdr,MSGR_WCDMA_RRC,umid);
#endif
  msgr_send(hdr, size);
  rrc_crash_debug_save_rrc_to_lte_cmds(umid);
}

