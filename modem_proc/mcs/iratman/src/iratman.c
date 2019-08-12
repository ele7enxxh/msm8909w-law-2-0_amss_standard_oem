/*!
  @file   iratman.c

  @brief  IRAT List Manager implementation.

  @details
  This file contains the code for operation of IRATMan.

*/

/*===========================================================================

  Copyright (c) 2011 - 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/iratman/src/iratman.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/16   vko/pk  Changes for FR-33944 (D2L redirection support)
06/29/15   pk      Added iratman_is_meas_feasible() API
01/12/15   cjb     Added Tbackoff capping algorithm
09/25/14   pk      Fixes for UE scanning LTE frequencies from lower EARFCN 
                   priority to higher EARFCN priority
08/05/14   cjb     Avoid accessing de-allocated memory and code clean-up
08/01/14   cjb     New API to return LTE freqs that are ready for measurement
06/09/14   pk      Changes for FR-17879 (Extended EARFCN and band range)
05/28/14   cjb     Added new API to return min measurement duration.
                   Round-robin approach of returning INACTIVE nodes to avoid
                   returning the same node for multi-sim cases where home_rat
                   might not be able to measure all nodes in one DRx cycle.
02/05/14   bph     Evaluate reselection algorithms independently of reporting
                   client.
01/24/14   bph     F3 removal/level audit
01/22/14   bph     Survive conflicting client initialization
01/14/14   bph     Set Tbackoff/Treselect defaults same as legacy
12/16/13   bph     Improve logging/clean up
12/10/13   bph     Remove dependancy on L1 for cell info
12/06/13   bph     FR3839 - dynamic tbackoff
11/27/13   bph     dynamic backoff API changes
09/09/13   bph     Consistant logging for Treselect == 0 case.
06/04/13   bph     Update logger for signed EARFCN priority and separate API
                   vs. timer expiry Tbackoff updating.
05/21/13   bph     Get rid of relative include path for test stubs
05/16/13   bph     LTE Deprioritization + fix freq count/get next freq logic
05/10/13   bph     Log Tbackoff expiry when it occurs
05/07/13   bph     Treselect == 0 optimization
05/09/13   bph     Hook in QTF
03/25/13   bph     Store RAT's node (more or less) preferrence
03/13/13   bph     Fix a null pointer dereference
01/23/13   cjb     Added logging capability.
01/23/13   cjb     Replace memcpy() w/ struct assignment or memscpy().
07/24/12   cjb     Fixed return value in API to update existing LTE node.
07/17/12   cjb     Handle cases where (C2K RSRP >= LTE RSRP) while measuring
                   LTE nodes in less preferred list.
07/16/12   cjb     Avoid using stale measurement results for chain denial cases.
07/05/12   cjb     Fixed compiler error.
06/26/12   cjb     Use rsrp_ant[] and rsrq_ant[] values for LTE meas evaluation.
05/17/12   cjb     Clear first/second meas arrays before adding passing cells.
                   Fixed array index handling in RSRP/RSRQ evaluation functions.
05/17/12   cjb     Whenever home_rat requests for delete_all, check whether the
                   node belongs to home_rat before attempting to delete it.
05/04/12   cjb     Remember the next_node before deleting/moving the node.
05/02/12   cjb     Updated log messages for Treselect/Tbackoff timers.
                   Fixed Treselect/Tbackoff comparison. Cleaned-up del_all api.
04/24/12   cjb     Fixed threshold comparison for both RSRP and RSRQ.
04/23/12   cjb     Adding support for LTE comparison algorithms(RSRP & RSRQ).
04/20/12   cjb     Fixed compiler error.
04/20/12   cjb     Call the wakeup callbacks of all RATs whenever timer expires.
04/17/12   cjb     Adjusted pe_max range from [0dBm,+63dBm] to [-30dBm,+33dBm].
04/10/12   cjb     Sort the LTE nodes in the list based on earfcn_priority
                   whenever a new node is added to the list.
04/06/12   cjb     Using MSG_SSID_IRATMAN for QXDM logging.
04/04/12   cjb     Using higher data_type for conversion between ms and sclks.
02/23/12   cjb     Added REX critical sections for mutual exclusion b/w tasks.
02/16/12   cjb     Compiler warning fix rework.
02/15/12   cjb     Fixed compiler warning.
02/10/12   cjb     Fixed LTE node deletion logic that was corrupting free list.
                   Increased priority for few MSGs to aid in debugging.
11/10/11   cjb     First working version.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/* Common */
#include <comdef.h>    /* Definition of basic types and macros */
#include <customer.h>
#include <amssassert.h>

/* IRATMan */
#include "iratman.h"
#include "iratman_i.h"
#include "iratman_la.h"
#include "iratman_log.h"

/* Others */
#include "err.h"
#include "hdrl1_api.h" 
#include "lte_cphy_irat_meas_msg.h"
#include "msg.h"
#include "ran.h"
#include "rex.h"
#include "secapi.h"
#include "timer.h"
#include "timetick.h"

#ifdef TEST_FRAMEWORK
 #error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------
      Mutual exclusion between two normal tasks
-------------------------------------------------------------------------*/
#define IRATMAN_INIT_CRIT_SECT()                        \
          rex_init_crit_sect( &iratman.iratman_crit_sect )

#define IRATMAN_ENTER_CRIT_SECT()                        \
          rex_enter_crit_sect( &iratman.iratman_crit_sect )

#define IRATMAN_LEAVE_CRIT_SECT()                        \
          rex_leave_crit_sect( &iratman.iratman_crit_sect )


/*-------------------------------------------------------------------------
      Variables
-------------------------------------------------------------------------*/
/* Main variable to access control info related to all RATs */
STATIC iratman_ctrl_info_type  iratman;


/*===========================================================================
                      INTERNAL FUNCTION PROTOTYPES
===========================================================================*/
STATIC void lte_do_list_maintenance(iratman_rat_node_type* node,
                                    iratman_lte_list_e_type old_list);

/*===========================================================================
                      INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

  FUNCTION:  iratman_is_dynamic_backoff_enabled

===========================================================================*/
/*!
  @brief
    Return whether dynamic backoff is enabled.

  @details
    This function returns if dynamic backoff if enabled.

  @note
    None

  @return
    TRUE if dynamic backoff is enabled, otherwise FALSE.

*/
/*=========================================================================*/
STATIC boolean iratman_is_dynamic_backoff_enabled( void )
{
  MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_LOW,
         "iratman.is_dynamic_backoff_enabled = %u",
         iratman.is_dynamic_backoff_enabled );

  return iratman.is_dynamic_backoff_enabled;
}


/*===========================================================================

  FUNCTION:  list_get_node

===========================================================================*/
/*!
  @brief
    Get a node from a list

  @details
    This function removes a buffer from the head of a singly linked list

  @note
    None

  @return
    Ptr to the node buffer

*/
/*=========================================================================*/
STATIC iratman_rat_node_type* list_get_node
(
  iratman_rat_list_type*  list_ptr  //!< Ptr to source node list
)
{
  iratman_rat_node_type*  buf_ptr = NULL;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "list_get_node()" );

  /* Error not expected, this function is only called from within this file */
  ASSERT( list_ptr != NULL );

  if( list_ptr->cnt > 0 )
  {
    /* Get buffer from head of list */
    list_ptr->cnt--;
    buf_ptr = list_ptr->head_ptr;
    list_ptr->head_ptr = buf_ptr->next;

    /* Clear stale data */
    memset( buf_ptr, 0, sizeof( iratman_rat_node_type ) );
  }
  else
  {
    IRATMAN_ERROR_MSG( "No buffer to store LTE freq parameters" );
  }

  return buf_ptr;

} /* list_get_node */


/*===========================================================================

  FUNCTION:  list_put_node

===========================================================================*/
/*!
  @brief
    Add a node into a list

  @details
    This function adds a buffer to the head of a singly linked list that is
    passed in as parameter

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void list_put_node
(
  iratman_rat_list_type*  list_ptr, //!< Ptr to destination list
  iratman_rat_node_type*  buf_ptr   //!< Ptr to buffer/node
)
{
  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "list_put_node()" );

  ASSERT( list_ptr != NULL );
  ASSERT( buf_ptr != NULL );

  /* Put buffer at head of list */
  buf_ptr->next      = list_ptr->head_ptr;
  list_ptr->head_ptr = buf_ptr;
  list_ptr->cnt++;

  /* --------- Update node header information --------- */
  buf_ptr->hdr.list[RAT_INT] = list_ptr->hdr.name.value;

} /* list_put_node */


/*===========================================================================

  FUNCTION:  list_move_node

===========================================================================*/
/*!
  @brief
    Move a node from one list to other

  @details
    This function moves a node from "source" list to "destn" list

  @note
    Updates "next" pointer of the input "node".

  @return
    boolean: TRUE if node was moved successfully. Else, FALSE

*/
/*=========================================================================*/
STATIC boolean list_move_node
(
  iratman_rat_node_type*  node,   //!< Node to be moved from one list to other
  iratman_rat_list_type*  source, //!< Ptr to source list
  iratman_rat_list_type*  destn   //!< Ptr to destination list
)
{
  iratman_rat_node_type*  prev_node = NULL;
  iratman_rat_node_type*  curr_node = NULL;
  boolean                 success = FALSE;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "list_move_node()" );

  ASSERT( node != NULL );
  ASSERT( source != NULL );
  ASSERT( destn != NULL );

  /* --------- Remove node from source list --------- */
  if( source->cnt > 0 )
  {
    /* Find the node */
    curr_node = source->head_ptr;
    prev_node = curr_node;

    while( curr_node != NULL )
    {
      if( curr_node == node )
      {
        success = TRUE;
        break; //!< Found node
      }

      /* Move to next node in the list */
      prev_node = curr_node;
      curr_node = curr_node->next;
    }

    if( success == FALSE )
    {
      IRATMAN_ERROR_MSG( "Node is not part of source list" );
      return success;
    }

    /* Node is part of the source list. Take it out of the list. */
    if( source->head_ptr == node )
    {
      /* Removing head node */
      source->head_ptr = node->next;
    }
    else
    {
      /* Removing intermediate or tail node */
      prev_node->next = node->next;
    }

    /*! Reduce the count */
    source->cnt--;
  }
  else
  {
    ERR_FATAL( "Source list is empty", 0, 0, 0 );
  }

  /* --------- Add node to the destination list --------- */
  /* Insert node at the head */
  node->next = destn->head_ptr;
  destn->head_ptr = node;
  destn->cnt++;

  /* --------- Update node header information --------- */
  node->hdr.list[RAT_INT] = destn->hdr.name.value;

  return success;

} /* list_move_node */


/*===========================================================================

  FUNCTION:  lte_init_lists

===========================================================================*/
/*!
  @brief
    Initialize LTE lists

  @details
    This function initializes frequency lists used for LTE pilot measurements.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_init_lists( void )
{
  int                    idx = 0;
  iratman_rat_list_type* lte_freq_lists = NULL;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_init_lists()" );

  /* Get LTE all frequency lists */
  lte_freq_lists = iratman.lte.all_freq_lists;

  /* --------------------------- Free list -------------------------- */
  /* Initialize list header */
  lte_freq_lists[ IRATMAN_LTE_FREE_LIST ].hdr.name.lte = IRATMAN_LTE_FREE_LIST;
  lte_freq_lists[ IRATMAN_LTE_FREE_LIST ].hdr.rat      = RAT_LTE;

  /* Link buffers in the list entry array, put all of them in the free list */
  lte_freq_lists[ IRATMAN_LTE_FREE_LIST ].head_ptr = iratman.lte.node_pool;

  lte_freq_lists[ IRATMAN_LTE_FREE_LIST ].cnt = IRATMAN_LTE_FREQ_ARR_SIZE;

  for( idx = 0; idx < ( IRATMAN_LTE_FREQ_ARR_SIZE - 1 ); idx++ )
  {
    iratman.lte.node_pool[ idx ].next = &iratman.lte.node_pool[ idx + 1 ];
  }

  /* Mark the next pointer of last node as NULL */
  iratman.lte.node_pool[ idx ].next = NULL;

  /* ---------------------- More preferred list --------------------- */
  /* Initialize the more preferred empty lists. It contains all the
     frequencies to be measured every sleep period */
  lte_freq_lists[ IRATMAN_LTE_MP_LIST ].hdr.name.lte = IRATMAN_LTE_MP_LIST;
  lte_freq_lists[ IRATMAN_LTE_MP_LIST ].hdr.rat = RAT_LTE;
  lte_freq_lists[ IRATMAN_LTE_MP_LIST ].cnt = 0;
  lte_freq_lists[ IRATMAN_LTE_MP_LIST ].head_ptr = NULL;

  /* ---------------------- Less preferred list --------------------- */
  /* Initialize the less preferred empty lists. It contains all the
     frequencies with priority <= 1X/HDR priority. These frequencies are
     measured if 1X/HDR signal strength is lower than the serving frequency
     threshold specified in the ATIM/Other-RAT neighbor list OTA message. */
  lte_freq_lists[ IRATMAN_LTE_LP_LIST ].hdr.name.lte = IRATMAN_LTE_LP_LIST;
  lte_freq_lists[ IRATMAN_LTE_LP_LIST ].hdr.rat = RAT_LTE;
  lte_freq_lists[ IRATMAN_LTE_LP_LIST ].cnt = 0;
  lte_freq_lists[ IRATMAN_LTE_LP_LIST ].head_ptr = NULL;

} /* lte_init_lists */


/*===========================================================================

  FUNCTION:  lte_find_node

===========================================================================*/
/*!
  @brief
    Find node with "earfcn" in LTE lists

  @details
    This function finds a LTE node with EARFCN="earfcn" in the More Preferred
    and Less Preferred LTE node lists

  @note
    None

  @return
    Pointer to the node, if node exists in one of the LTE lists. Else NULL.

*/
/*=========================================================================*/
STATIC iratman_rat_node_type* lte_find_node
(
  /*! Evolved Absolute Radio Frequency Channel Number (LTE freq number) */
  lte_earfcn_t  earfcn
)
{
  /* Ptr to individual LTE frequency entry in frequency list */
  iratman_rat_node_type*  curr_node = NULL;
  uint8                   count = 0;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_find_node()" );

  /* ---------------------- More preferred list --------------------- */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].cnt > 0 )
  {
    count = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].cnt;
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].head_ptr;

    while( ( curr_node != NULL ) && ( count > 0 ) )
    {
      if( curr_node->data.lte.mc.earfcn == earfcn )
      {
        return curr_node; //!< Found node
      }

      /* Move to next freq in the list */
      curr_node = curr_node->next;
      count--;
    }
  }

  /* ---------------------- Less preferred list --------------------- */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].cnt > 0 )
  {
    count = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].cnt;
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].head_ptr;

    while( ( curr_node != NULL ) && ( count > 0 ) )
    {
      if( curr_node->data.lte.mc.earfcn == earfcn )
      {
        return curr_node; //!< Found node
      }

      /* Move to next freq in the list */
      curr_node = curr_node->next;
      count--;
    }
  }

  return NULL;  //!< EARFCN doesn't exist in any of the lists

} /* lte_find_node */


/*===========================================================================

  FUNCTION:  lte_clear_rat_info

===========================================================================*/
/*!
  @brief
    Clears "home_rat" specific information from "node"

  @details
    This function clears information specific to "home_rat" from "node".
    This is used if "home_rat" requests to delete a specific "node" or to
    delete all measurement nodes that was added by it earlier.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: True RAT node was successfully added

*/
/*=========================================================================*/
STATIC void lte_clear_rat_info
(
  iratman_rat_e_type            home_rat,   //!< Home/Originating RAT
  iratman_rat_node_type*        node        //!< Meas LTE node
)
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_clear_rat_info()" );

  ASSERT( node != NULL );

  node->hdr.list[home_rat] = IRATMAN_LTE_FREE_LIST;

  /* ------------ Clear RAT's measurement criteria ------------ */
  node->data.lte.mc.earfcn_priority[home_rat]         = -1;
  node->data.lte.mc.bandwidth[home_rat]               = (lte_bandwidth_e)0;
  node->data.lte.mc.thresh_serving[home_rat]          = 0;
  node->data.lte.mc.treselect_dur_ms[home_rat]        = 0;
  node->data.lte.mc.tbackoff_dur_ms[home_rat]         = 0;

  /* ------------ Clear RAT's comparison criteria ------------ */
  node->data.lte.cc.meas_comp_algo[home_rat]          = (iratman_lte_algo_e_type)0;
  node->data.lte.cc.rx_lev_min_eutra[home_rat]        = 0;
  node->data.lte.cc.rx_lev_min_eutra_offset[home_rat] = 0;
  node->data.lte.cc.pe_max[home_rat]                  = 0;
  node->data.lte.cc.thresh_x[home_rat]                = 0;
  node->data.lte.cc.rsrq_min_eutra[home_rat]          = 0;
  node->data.lte.cc.rsrq_min_eutra_offset[home_rat]   = 0;
  node->data.lte.cc.rsrq_thresh_x[home_rat]           = 0;

} /* lte_clear_rat_info */


/*===========================================================================

  FUNCTION:  lte_get_highest_list_preference

===========================================================================*/
/*!
  @brief
    Helper function that evaluates the input parameters and returns the
    highest lte list preference of the two.

  @details
    IRATMAN_LTE_MP_LIST is the highest
    IRATMAN_LTE_LP_LIST is the lowest

  @note
    None

  @return
    iratman_lte_list_e_type highest lte list preference

*/
/*=========================================================================*/
STATIC iratman_lte_list_e_type lte_get_highest_list_preference
(
   uint8 preference1,
   uint8 preference2
)
{
  if (preference1 == IRATMAN_LTE_MP_LIST ||
      preference2 == IRATMAN_LTE_MP_LIST)
  {
    return IRATMAN_LTE_MP_LIST;
  }

  return IRATMAN_LTE_LP_LIST;

} /* lte_get_highest_list_preference */


/*===========================================================================

  FUNCTION:  lte_update_current_info

===========================================================================*/
/*!
  @brief
    Updates the current information set

  @details
    This function updates the current information set "RAT_INT". This is used
    for LTE freq measurement and also during threshold comparisons post meas.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: True RAT node was successfully added

*/
/*=========================================================================*/
STATIC void lte_update_current_info
(
  iratman_rat_node_type*  node    //!< Meas LTE node
)
{
  iratman_lte_node_type*      lte_data = NULL;
  uint32                      idx      = RAT_INT;

  /*----------------------------------------------------------------------*/

  ASSERT( node != NULL );

  /* LTE node data */
  lte_data = &node->data.lte;

  /* Assume some default values before starting over.
     NOTE: These values might have to be changed based on MIN/MAX selection. */
  node->hdr.list[RAT_INT]                       = IRATMAN_LTE_FREE_LIST;
  lte_data->mc.earfcn_priority[RAT_INT]         = -1;
  lte_data->mc.bandwidth[RAT_INT]               = LTE_BW_UNKNOWN;
  lte_data->mc.thresh_serving[RAT_INT]          = 0;
  lte_data->mc.treselect_dur_ms[RAT_INT]        = 0xFFFFFFFF;
  lte_data->mc.tbackoff_dur_ms[RAT_INT]         = 0xFFFFFFFF;
  lte_data->cc.meas_comp_algo[RAT_INT]          = IRATMAN_LTE_ALGO_RSRP;
  lte_data->cc.rx_lev_min_eutra[RAT_INT]        = 0;
  lte_data->cc.rx_lev_min_eutra_offset[RAT_INT] = 10;
  lte_data->cc.pe_max[RAT_INT]                  = 0;
  lte_data->cc.thresh_x[RAT_INT]                = 100;
  lte_data->cc.rsrq_min_eutra[RAT_INT]          = 0;
  lte_data->cc.rsrq_min_eutra_offset[RAT_INT]   = 10;
  lte_data->cc.rsrq_thresh_x[RAT_INT]           = 100;

  if ( iratman_is_dynamic_backoff_enabled() )
  {
    lte_data->mc.max_tbackoff[RAT_INT]          = 0xFF;
    lte_data->mc.min_tbackoff[RAT_INT]          = 0xFF;
  }

  /* Update current info */
  for( idx = RAT_INT + 1; idx < RAT_MAX; idx++ )
  {
    /* Consider data only from RATs which uses the node */
    if( node->hdr.rats_known & RAT_MASK( idx ) )
    {
      /* Values: IRATMAN_LTE_MP_LIST[highest] and IRATMAN_LTE_LP_LIST[lowest] */
      node->hdr.list[RAT_INT] =
        lte_get_highest_list_preference( node->hdr.list[RAT_INT],
                                         node->hdr.list[idx] );

      /* Reference: 3GPP2 C.S0097-0 v2.0 */
      /* EARFCNPriority
         Brief: Priority of the EARFCN of the neighboring EUTRA system
         Range: 0[Lowest priority] to 7[highest priority] */
      lte_data->mc.earfcn_priority[RAT_INT] =
                            MAX( lte_data->mc.earfcn_priority[RAT_INT],
                                 lte_data->mc.earfcn_priority[idx] );

      /* MeasurementBandwidth
         Brief: The values indicate the number of resource blocks[Nrb] over
                which the UE could measure.
         Range: Transmission bandwidth configuration value defined in
                36.101 Section 5.4.2.1[refer: lte_bandwidth_e] */
      lte_data->mc.bandwidth[RAT_INT] =
                            MIN( lte_data->mc.bandwidth[RAT_INT],
                                 lte_data->mc.bandwidth[idx] );

      /* ThreshServing
         Brief: Serving threshold trigger point for E-UTRAN neighbor selection
                associate with ThreshX.
         Range:  The base station shall set this field to [-2 * 10 * log10PS],
                 where PS is the strength of the reference pilot of the serving
                 Base station. */
      /******************* NOTE: CURRENTLY NOT BEING USED *******************/
      lte_data->mc.thresh_serving[RAT_INT] =
                          MAX( lte_data->mc.thresh_serving[RAT_INT],
                               lte_data->mc.thresh_serving[idx] );
      /**********************************************************************/

      /* Treselect timer duration (in ms) */
      lte_data->mc.treselect_dur_ms[RAT_INT] =
                            MIN( lte_data->mc.treselect_dur_ms[RAT_INT],
                                 lte_data->mc.treselect_dur_ms[idx] );

      if ( iratman_is_dynamic_backoff_enabled() )
      {
        lte_data->mc.min_tbackoff[RAT_INT] =
                              MIN( lte_data->mc.min_tbackoff[RAT_INT],
                                   lte_data->mc.min_tbackoff[idx] );

        lte_data->mc.max_tbackoff[RAT_INT] =
                              MIN( lte_data->mc.max_tbackoff[RAT_INT],
                                   lte_data->mc.max_tbackoff[idx] );
      }
      else
      {
        /* Tbackoff timer duration (in ms) */
        lte_data->mc.tbackoff_dur_ms[RAT_INT] =
                              MIN( lte_data->mc.tbackoff_dur_ms[RAT_INT],
                                   lte_data->mc.tbackoff_dur_ms[idx] );
      }

      /******************* NOTE: CURRENTLY NOT BEING USED *******************/
      /* Decide on measurement comparison algorithm to use */
      lte_data->cc.meas_comp_algo[RAT_INT] =
                            MAX( lte_data->cc.meas_comp_algo[RAT_INT],
                                 lte_data->cc.meas_comp_algo[idx] );

      /* --------------------- RSRP parameters --------------------- */
      if( lte_data->cc.meas_comp_algo[idx] == IRATMAN_LTE_ALGO_RSRP )
      {
        /* RxLevMinEUTRA
           Brief: Minimum received RSRP level from the EUTRA cell.(dBm)
           Range: 0 to 96, where (-44 – RxLevMinEUTRA) in dBm is equal to the
                  minimum reference signal received power (RSRP) level of a
                  EUTRA cell required for the UE to reselect EUTRA */
        lte_data->cc.rx_lev_min_eutra[RAT_INT] =
                            MAX( lte_data->cc.rx_lev_min_eutra[RAT_INT],
                                 lte_data->cc.rx_lev_min_eutra[idx] );

        /* RxLevMinEUTRAOffset
           Brief: Offset to Minimum reference power level to select EUTRA.(dBm)
           Range: 1 through 8 */
        lte_data->cc.rx_lev_min_eutra_offset[RAT_INT] =
                            MIN( lte_data->cc.rx_lev_min_eutra_offset[RAT_INT],
                                 lte_data->cc.rx_lev_min_eutra_offset[idx] );

        /* PEMax
           Brief: Maximum TX power level an UE may use when transmitting
                 on the uplink in E-UTRA (dBm) */
        lte_data->cc.pe_max[RAT_INT] =
                            MAX( lte_data->cc.pe_max[RAT_INT],
                                 lte_data->cc.pe_max[idx] );

        /* ThrexhX
           Brief: Minimum required reference signal received power threshold used
                  for selecting an EARFCN.
           Range: 0 to 31.
                  The actual threshold value in dB used to compare against the
                  measured reference signal receive power (RSRP) = ThreshX * 2.
           NOTE: Threshold used to compare ==> ThreshX * 2
                 (assumed to be in dBm, though the spec says dB) */
        lte_data->cc.thresh_x[RAT_INT] =
                            MIN( lte_data->cc.thresh_x[RAT_INT],
                                 lte_data->cc.thresh_x[idx] );
      }
      /* --------------------- RSRQ parameters --------------------- */
      else if( lte_data->cc.meas_comp_algo[idx] == IRATMAN_LTE_ALGO_RSRQ )
      {

        /* QualMinEUTRA
           Brief: Minimum received RSRQ level from the EUTRA cell.(dB)
           Range: 0 to 31, where (-3 – QualMinEUTRA) in dB is equal to the
                  minimum reference signal received quality (RSRQ) level of a
                  EUTRA cell required for the UE to reselect EUTRA */
        lte_data->cc.rsrq_min_eutra[RAT_INT] =
                              MAX( lte_data->cc.rsrq_min_eutra[RAT_INT],
                                   lte_data->cc.rsrq_min_eutra[idx] );

        /* QualMinEUTRAOffset
           Brief: Offset to Minimum reference quality level to select EUTRA.
           Range: 1 through 8 */
        lte_data->cc.rsrq_min_eutra_offset[RAT_INT] =
                              MIN( lte_data->cc.rsrq_min_eutra_offset[RAT_INT],
                                   lte_data->cc.rsrq_min_eutra_offset[idx] );

        /* ThrexhXQ
           Brief: Minimum required reference signal received qulaity threshold
                  used for selecting an EARFCN.
           Range: 0 to 31.
        */
        lte_data->cc.rsrq_thresh_x[RAT_INT] =
                              MIN( lte_data->cc.rsrq_thresh_x[RAT_INT],
                                   lte_data->cc.rsrq_thresh_x[idx] );
      }
      /**********************************************************************/

    } /* RAT uses node */

  } /* End of for loop */

} /* lte_update_current_info */


/*===========================================================================

  FUNCTION:  lte_sort_list_using_earfcn_priority

===========================================================================*/
/*!
  @brief
    Sorts the input list in descending order of earfcn_priority of the nodes

  @details
    This function sorts the nodes in the input "list" in descending order of
    their earfcn_priority.

  @note
    iratman_init() should have been previously called.
    lte_update_current_info() have been previously called to get the effective
    earfcn_priority. Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_sort_list_using_earfcn_priority
(
  iratman_rat_list_type*  list   //!< Ptr to lte list
)
{
  /* Pointers for sorting */
  iratman_rat_node_type*  check_node;
  iratman_rat_node_type*  curr_node;
  iratman_rat_node_type*  max_node;

  /* Temporary storage for use when swapping two entries */
  iratman_rat_node_type   temp_node;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_sort_list_using_earfcn_priority()" );

  /* Need to have at least 2 entries to sort them */
  if( list->cnt < 2 )
  {
    return;
  }

  /* -------------------------- Selection sort -------------------------- */
  check_node = list->head_ptr;

  while( check_node )
  {
    /* -------------- Find the max priority node -------------- */
    /* Start with the node which is looking for a potential replacement */
    curr_node = check_node;
    max_node  = curr_node;

    while( curr_node != NULL )
    {
      if( curr_node->data.lte.mc.earfcn_priority[ RAT_INT ] >
          max_node->data.lte.mc.earfcn_priority[ RAT_INT ] )
      {
        /* curr_node has higher priority than max_node */
        max_node = curr_node;
      }

      /* Move to next freq in the list */
      curr_node = curr_node->next;
    }

    /* ---------------------- Swap nodes --------------------- */
    /* Making sure we have to swap nodes */
    if( max_node != check_node )
    {
      /* Swap max_node and check_node without touching "next_ptr" */
      temp_node.hdr    = max_node->hdr;
      temp_node.data   = max_node->data;

      max_node->hdr    = check_node->hdr;
      max_node->data   = check_node->data;

      check_node->hdr  = temp_node.hdr;
      check_node->data = temp_node.data;
    }

    /* --------- Continue until we exhaust all nodes --------- */
    /* Move to next freq in the list */
    check_node = check_node->next;
  }

} /* lte_sort_list_using_earfcn_priority */


/*===========================================================================

  FUNCTION:  iratman_sec_ran_next

===========================================================================*/
/*!
  @brief
    Generate secure random number

  @details
    This function generates secure random numbers.

  @note
    None

  @return
    uint32: secure random number

*/
/*=========================================================================*/
uint32 iratman_sec_ran_next
(
   uint32 default_ran_num  /* Default value to be returned if HW block fails */
)
{
  uint32 ran_num = 0;

  secerrno_enum_type sec_ran_status = secapi_get_random( SECAPI_SECURE_RANDOM,
                                                         (uint8 *)&ran_num,
                                                         sizeof( ran_num ) );

  IRATMAN_DEBUG_MSG( "iratman_la_sec_ran_next" );

  if( sec_ran_status != E_SUCCESS )
  {
      MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "Secure API failed due to %d, Defaulting to %u",
             sec_ran_status,
             default_ran_num);

      ran_num = default_ran_num;
  }

  return( ran_num );

} /* iratman_sec_ran_next */


/*===========================================================================

  FUNCTION:  lte_set_treselct_dur_ms

===========================================================================*/
/*!
  @brief
    Set LTE treselect_dur_ms measurement criteria

  @details

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return

*/
/*=========================================================================*/
STATIC void lte_set_treselect_dur_ms
(
   iratman_rat_e_type  home_rat,
   iratman_mc_lte     *mc
)
{
  if (mc->max_treselect[home_rat] == IRATMAN_LTE_MAX_TRESELECT_DEFAULT)
  {
    mc->treselect_dur_ms[home_rat] = IRATMAN_LTE_DEFAULT_TRESELECT_DUR_MS;

    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "Using default Reselect (%u) ms",
           IRATMAN_LTE_DEFAULT_TRESELECT_DUR_MS);
  }
  else if(mc->override_treselect_tbackoff == TRUE)
  {
   /* Convert to ms */
    mc->treselect_dur_ms[home_rat] =  mc->max_treselect[home_rat]*100;
   
       MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "TRESELECT: Overrided value=%u (in milli seconds) Max=%u",
           mc->treselect_dur_ms[home_rat], mc->max_treselect[home_rat] );
  }
  else
  {
    word max_treselect_dur_s;
    uint32 rand_treselect_dur_s;

    /* Guard against evil ATIM/ORNL OTA messages */
    if ( mc->max_treselect[home_rat] > 15 ) /* > 15 is NOT valid */
    {
      mc->max_treselect[home_rat] = 15;
    }

    max_treselect_dur_s = ( word )( 1 << mc->max_treselect[home_rat] );

    /* Randomize reselection timer up to max value specified (in seconds) */
    rand_treselect_dur_s = ( uint32 ) ran_dist( iratman_sec_ran_next(0),
                                                0,
                                                max_treselect_dur_s + 1 );
    /* Convert to ms */
    mc->treselect_dur_ms[home_rat] = rand_treselect_dur_s * 1000;

    MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "TRESELECT: Min=0, Max=%u, Random=%u (in seconds)",
           max_treselect_dur_s, rand_treselect_dur_s );
  }

} /* lte_set_treselect_dur_ms */


/*===========================================================================

  FUNCTION:  lte_set_tbackoff_dur_ms

===========================================================================*/
/*!
  @brief
    Set LTE tbackoff_dur_ms measurment criteria

  @details

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return

*/
/*=========================================================================*/
STATIC void lte_set_tbackoff_dur_ms
(
   iratman_rat_e_type  home_rat,
   iratman_mc_lte     *mc
)
{
  if ( ( mc->min_tbackoff[home_rat] == IRATMAN_LTE_MIN_TBACKOFF_DEFAULT ) ||
       ( mc->max_tbackoff[home_rat] == IRATMAN_LTE_MAX_TBACKOFF_DEFAULT ) )
  {
    mc->tbackoff_dur_ms[home_rat] = IRATMAN_LTE_DEFAULT_TBACKOFF_DUR_MS;

    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "Using default Backoff (%u) ms",
           IRATMAN_LTE_DEFAULT_TBACKOFF_DUR_MS);
  }
  else if(mc->override_treselect_tbackoff == TRUE)
  {
   /* Convert to ms */
    mc->tbackoff_dur_ms[home_rat] =  mc->max_tbackoff[home_rat]*100;
   
       MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "TBACKOFF: Overrided value=%u (in milli seconds) Max=%u",
           mc->tbackoff_dur_ms[home_rat], mc->max_tbackoff[home_rat] );
  }
  else
  {
    word min_tbackoff_dur_s;
    word max_tbackoff_dur_s;
    uint32 rand_tbackoff_dur_s;

    /* Guard against evil ATIM/ORNL OTA messages */
    if ( mc->max_tbackoff[home_rat] > 15 ) /* > 15 is not valid */
    {
      mc->max_tbackoff[home_rat] = 15;
    }
    if ( mc->min_tbackoff[home_rat] > mc->max_tbackoff[home_rat] )
    {
      MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "TBACKOFF: ( Min=%u > Max=%u ). Forcing Min = Max",
             mc->min_tbackoff[home_rat],
             mc->max_tbackoff[home_rat] );

      mc->min_tbackoff[home_rat] = mc->max_tbackoff[home_rat];
    }

    min_tbackoff_dur_s = ( word )( 1 << mc->min_tbackoff[home_rat] );
    max_tbackoff_dur_s = ( word )( 1 << mc->max_tbackoff[home_rat] );

    /* Randomize backoff timer between min and max timer values (in seconds) */
    rand_tbackoff_dur_s = ( uint32 )
                               ran_dist( iratman_sec_ran_next(min_tbackoff_dur_s),
                                         min_tbackoff_dur_s,
                                         max_tbackoff_dur_s + 1 );

    /* Convert to ms */
    mc->tbackoff_dur_ms[home_rat] = rand_tbackoff_dur_s * 1000;

    MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "TBACKOFF: Min=%u, Max=%u, Random=%u (in seconds)",
           min_tbackoff_dur_s, max_tbackoff_dur_s, rand_tbackoff_dur_s );
  }

} /* lte_set_tbackoff_dur_ms */


/*===========================================================================

  FUNCTION:  lte_update_existing_node

===========================================================================*/
/*!
  @brief
    Update an existing LTE node

  @details
    This function adds "home_rat" specific information to an existing LTE node.
    If "node" is not part of desired "list" and the desired "list"=More Pref,
    then the node is moved to the More Preferred list. It then calls
    lte_update_current_info() to update current information.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: TRUE if node was updated successfully

*/
/*=========================================================================*/
STATIC boolean lte_update_existing_node
(
  iratman_rat_e_type            home_rat,   //!< Home/Originating RAT
  iratman_lte_input_node_type*  input,      //!< Input node data
  iratman_rat_node_type*        node,       //!< RAT node to be updated
  iratman_lte_list_e_type       list        //!< Meas RAT list to be used
)
{
  boolean           ret_value = TRUE;  /* Return value */
  iratman_mc_lte*   temp_mc = NULL;    /* Temp ptr for measurement criteria */
  iratman_cc_lte*   temp_cc = NULL;    /* Temp ptr for comparison criteria */
  iratman_lte_list_e_type old_list;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_update_existing_node()" );

  IRATMAN_NULL_PTR_CHECK( input, FALSE );
  IRATMAN_NULL_PTR_CHECK( node, FALSE );

  MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Updating existing node: "
         "home_rat=%d, meas_rat=LTE, EARFCN=%d, list=%d, algo=%d",
         home_rat,
         input->earfcn,
         list,
         input->meas_comp_algo );

  /* Update node data */
  node->hdr.rats_known |= RAT_MASK( home_rat );
  node->hdr.list[home_rat] = list;

  /* Pointer to node's meas & comparison criteria */
  temp_mc = &node->data.lte.mc;
  temp_cc = &node->data.lte.cc;

  /* ------------ Update meas criteria ------------ */
  temp_mc->earfcn_priority[home_rat]         = input->earfcn_priority;
  temp_mc->bandwidth[home_rat]               = input->bandwidth;
  temp_mc->thresh_serving[home_rat]          = input->thresh_serving;
  temp_mc->max_treselect[home_rat]           = input->max_treselect;
  temp_mc->min_tbackoff[home_rat]            = input->min_tbackoff;
  temp_mc->max_tbackoff[home_rat]            = input->max_tbackoff;
  temp_mc->override_treselect_tbackoff       = FALSE;

  if(input->meas_comp_algo == IRATMAN_LTE_ALGO_RSRP_RSRQ)
  {
    temp_mc->override_treselect_tbackoff = TRUE;
    /* again reset it to IRATMAN_LTE_ALGO_RSRQ 
      (or) 
       alternative approach : it can be continued with IRATMAN_LTE_ALGO_RSRP_RSRQ, with more changes being needed */
    input->meas_comp_algo = IRATMAN_LTE_ALGO_RSRQ;
    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Adding new node: override set. meas_comp_algo updated to %d",
         input->meas_comp_algo);
  }

  lte_set_treselect_dur_ms(home_rat, temp_mc);
  lte_set_tbackoff_dur_ms(home_rat, temp_mc);

  /* ------------ Update comparison criteria ------------ */
  temp_cc->meas_comp_algo[home_rat]          = input->meas_comp_algo;

  /* RSRP parameters */
  temp_cc->rx_lev_min_eutra[home_rat]        = input->rx_lev_min_eutra;
  temp_cc->rx_lev_min_eutra_offset[home_rat] = input->rx_lev_min_eutra_offset;
  temp_cc->pe_max[home_rat]                  = input->pe_max;
  temp_cc->thresh_x[home_rat]                = input->thresh_x;

  if( input->meas_comp_algo == IRATMAN_LTE_ALGO_RSRQ )
  {
    /* RSRQ parameters */
    temp_cc->rsrq_min_eutra[home_rat]          = input->rsrq_min_eutra;
    temp_cc->rsrq_min_eutra_offset[home_rat]   = input->rsrq_min_eutra_offset;
    temp_cc->rsrq_thresh_x[home_rat]           = input->rsrq_thresh_x;
  }

  /* ------------ Update RAT_INT ------------ */
  old_list = (iratman_lte_list_e_type)node->hdr.list[RAT_INT];
  lte_update_current_info( node );

  /* ------------ Move to desired list ------------ */
  /* If needed, moved node to the MP list */
  lte_do_list_maintenance(node, old_list);

  return ret_value;

} /* lte_update_existing_node */


/*===========================================================================

  FUNCTION:  lte_add_new_node

===========================================================================*/
/*!
  @brief
    Add a new LTE node for measurement

  @details
    This function adds a node to the IRAT List Manager's "list" of LTE nodes.
    It then calls lte_update_current_info() to update current information.

  @note
  iratman_init() should have been previously called
  Inputs are assumed to be pre-validated

  @return
    boolean: TRUE if RAT node was successfully added. Else, FALSE.

*/
/*=========================================================================*/
STATIC boolean lte_add_new_node
(
  iratman_rat_e_type            home_rat,   //!< Home/Originating RAT
  iratman_lte_input_node_type*  input,      //!< RAT node to be added
  iratman_lte_list_e_type       list        //!< Meas RAT list to be used
)
{
  iratman_rat_node_type*  temp_node = NULL;  /* New node */
  iratman_mc_lte*         temp_mc = NULL;    /* Ptr for measurement criteria */
  iratman_cc_lte*         temp_cc = NULL;    /* Ptr for comparison criteria */

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_add_new_node()" );

  IRATMAN_NULL_PTR_CHECK( input, FALSE );

  MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Adding new node: "
         "home_rat=%d, meas_rat=LTE, EARFCN=%d, list=%d, algo=%d",
         home_rat,
         input->earfcn,
         list,
         input->meas_comp_algo );

  /* Get free node */
  temp_node = list_get_node( &iratman.lte.
                             all_freq_lists[ IRATMAN_LTE_FREE_LIST ] );

  /* Check if we were able to get a node */
  if( temp_node == NULL )
  {
    ERR_FATAL( "Unable to retrieve node from LTE free list", 0, 0, 0 );

    return FALSE;
  }

  /* Build node */
  temp_node->hdr.rat         = RAT_LTE;
  temp_node->hdr.rats_known |= RAT_MASK( home_rat );
  temp_node->hdr.in_use      = FALSE;
  temp_node->hdr.list[home_rat] = (uint8)list;

  /* Pointer to node's meas & comparison criteria */
  temp_mc = &temp_node->data.lte.mc;
  temp_cc = &temp_node->data.lte.cc;

  /* ------------ Update meas criteria ------------ */
  temp_mc->state                             = IRATMAN_LTE_INACTIVE;
  temp_mc->earfcn                            = input->earfcn;
  temp_mc->earfcn_priority[home_rat]         = input->earfcn_priority;
  temp_mc->bandwidth[home_rat]               = input->bandwidth;
  temp_mc->thresh_serving[home_rat]          = input->thresh_serving;
  temp_mc->max_treselect[home_rat]           = input->max_treselect;
  temp_mc->min_tbackoff[home_rat]            = input->min_tbackoff;
  temp_mc->max_tbackoff[home_rat]            = input->max_tbackoff;
  temp_mc->override_treselect_tbackoff       = FALSE;
  
  if(input->meas_comp_algo == IRATMAN_LTE_ALGO_RSRP_RSRQ)
  {
    temp_mc->override_treselect_tbackoff = TRUE;
    input->meas_comp_algo = IRATMAN_LTE_ALGO_RSRQ;
 /* again reset it to IRATMAN_LTE_ALGO_RSRQ 
      (or) 
    alternative approach : it can be continued with IRATMAN_LTE_ALGO_RSRP_RSRQ, with more changes being needed */
    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Adding new node: override set. meas_comp_algo updated to %d",
         input->meas_comp_algo);
  }

  lte_set_treselect_dur_ms(home_rat, temp_mc);
  lte_set_tbackoff_dur_ms(home_rat, temp_mc);

  /* ------------ Update comparison criteria ------------ */
  temp_cc->meas_comp_algo[home_rat]          = input->meas_comp_algo;

  /* RSRP parameters */
  temp_cc->rx_lev_min_eutra[home_rat]        = input->rx_lev_min_eutra;
  temp_cc->rx_lev_min_eutra_offset[home_rat] = input->rx_lev_min_eutra_offset;
  temp_cc->pe_max[home_rat]                  = input->pe_max;
  temp_cc->thresh_x[home_rat]                = input->thresh_x;

  if( input->meas_comp_algo == IRATMAN_LTE_ALGO_RSRQ )
  {
    /* RSRQ parameters */
    temp_cc->rsrq_min_eutra[home_rat]          = input->rsrq_min_eutra;
    temp_cc->rsrq_min_eutra_offset[home_rat]   = input->rsrq_min_eutra_offset;
    temp_cc->rsrq_thresh_x[home_rat]           = input->rsrq_thresh_x;
  }

  /* ------------ Update RAT_INT ------------ */
  lte_update_current_info( temp_node );

  /* ------------ Add node to list ------------ */
  list_put_node( &iratman.lte.all_freq_lists[list], temp_node );

  /* Sort the list based on earfcn_priority */
  lte_sort_list_using_earfcn_priority( &iratman.lte.all_freq_lists[list] );

  /* New node added successfully */
  return TRUE;

} /* lte_add_new_node */


/*===========================================================================

  FUNCTION:  lte_treselect_get_active_min

===========================================================================*/
/*!
  @brief
    Get LTE node with earliest expiring active Treselect timer

  @details
    This function finds the LTE meas node with active Treselect timer which has
    the shortest expiration time in the following frequency lists:
    -> More Preferred list:
        Contains frequencies to be measured every sleep period.
    -> Less Preferred list:
        Contains frequencies with priority <= "home_rat" priority.
        These frequencies are measured if "home_rat" signal strength is lower
        than the "serving frequency threshold" specified in the OTA [ATIM/ORAT].

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    Ptr to LTE node with active Treselect timer and the minimum expiry time.
    This function may return NULL if no timer is found.

*/
/*=========================================================================*/
STATIC iratman_rat_node_type* lte_treselect_get_active_min( void )
{
  iratman_rat_node_type* curr_node = NULL; /* Ptr to individual LTE freq */
  iratman_rat_node_type* min_ptr = NULL; /* Ptr to earliest Treselect expiry */

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_treselect_get_active_min()" );

  /* ---------------------- More preferred list --------------------- */
  /* Look for frequency that has minimum Treselect timer
     in the more preferred list */
  curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].head_ptr;
  while( curr_node != NULL )
  {
    if( curr_node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_ACTIVE )
    {
      if( ( min_ptr == NULL ) ||
          TIME_OCCURS_FIRST( curr_node->data.lte.mc.treselect_exp_sclks,
                             min_ptr->data.lte.mc.treselect_exp_sclks ) )
      {
        min_ptr = curr_node;
      }
    }

    /* Move to next node in the list */
    curr_node = curr_node->next;
  }

  /* ---------------------- Less preferred list --------------------- */
  /* Continue the search in the less preferred list */
  curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].head_ptr;
  while( curr_node != NULL )
  {
    if( curr_node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_ACTIVE )
    {
      if( ( min_ptr == NULL ) ||
          TIME_OCCURS_FIRST( curr_node->data.lte.mc.treselect_exp_sclks,
                             min_ptr->data.lte.mc.treselect_exp_sclks ) )
      {
        min_ptr = curr_node;
      }
    }

    /* Move to next node in the list */
    curr_node = curr_node->next;
  }

  return( min_ptr );

} /* lte_treselect_get_active_min */


/*===========================================================================

  FUNCTION:  lte_treselect_timer_set

===========================================================================*/
/*!
  @brief
    Arm HW timer for a LTE node with earliest expiring active Treselect timer

  @details
    This function sets the Treselect timer for a LTE freq since it passed the
    RSRP/RSRQ reselection comparison. Since only 1 HW timer is used for multiple
    frequencies, the timer is set to expire for the frequency with the shortest
    expiry time from now.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_treselect_timer_set
(
  iratman_rat_node_type*      curr_node //!< RAT node that was being measured
)
{
  timetick_type          curr_time_sclks;  /* Current time in sclks units */
  iratman_rat_node_type* treselect_node = NULL; /* Ptr to individual LTE freq */

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_treselect_timer_set()" );

  /* Clear Treselect timer as we are going to run through the lists anyway */
  (void) timer_clr( &iratman.lte.treselect_timer, T_NONE );

  /* Get current time in sclk units */
  curr_time_sclks = timetick_get();

  if( curr_node != NULL )
  {
    /* Indicate that Treselect timer is active for this frequency */
    curr_node->data.lte.mc.state = IRATMAN_LTE_TRESELECT_ACTIVE;

    /* --------------------- Update timer values ---------------------- */
    /* Calculate time when timer expires, required to find the earliest timer.
       Don't need to worry about wrap-around since timetick_type is unsigned
       and several order magnitude larger than timeout values.
       NOTE:
       As per the spec, the max duration we might use is 32768000ms (2^15).
       This is well within the max value of 131071999ms which is obtained by
       using a data_type of uint32 and assuming a SCLK frequency of 32768 Hz.
       Hence, we can truncate the value returned by TIMETICK_SCLK_TO_MS. */
    curr_node->data.lte.mc.treselect_exp_sclks = curr_time_sclks +
      ( uint32 )TIMETICK_SCLK_FROM_MS(
                ( uint64 )curr_node->data.lte.mc.treselect_dur_ms[RAT_INT] );

    MSG_5( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
           "Set Treselect timer for EARFCN=%u, duration=%ums, "
           "duration_sclks=%usclks, curr=%usclks, exp=%usclks",
           curr_node->data.lte.mc.earfcn,
           curr_node->data.lte.mc.treselect_dur_ms[RAT_INT],
           ( uint32 )TIMETICK_SCLK_FROM_MS(
                 ( uint64 )curr_node->data.lte.mc.treselect_dur_ms[RAT_INT] ),
           curr_time_sclks,
           curr_node->data.lte.mc.treselect_exp_sclks );

    iratman_log_report_event( IRATMAN_LOG_T_RESEL_START_EVENT,
                              RAT_INT, RAT_LTE,
                              NULL,
                              curr_node );
  }

  /* ----------------------- Get current timer ---------------------- */
  /* If there are other timers active, need to find the earliest timer to arm,
     using system time. This way, one HW timer can be used for multiple
     Treselect timers by setting up another one every time one expires */

  /* If there are multiple active timers, find the Treselect timer with the
     shortest expiration from now */
  treselect_node = lte_treselect_get_active_min();

  /* -------------------- Set new Treselect timer ------------------- */
  if( treselect_node != NULL )
  {
    if( TIME_OCCURS_FIRST( treselect_node->data.lte.mc.treselect_exp_sclks,
                           curr_time_sclks ) )
    {
      IRATMAN_ERROR_MSG( "Treselect already expired. Do not arm HW timer" );

      /* Since sclk count is <= 0, indicate that Treselect timer has expired
         without arming HW timer. The expired timer will be processed next
         wake up time. This should happen in extremely rare cases where two
         Treselect timers expires very close to one another. */
      treselect_node->data.lte.mc.state = IRATMAN_LTE_TRESELECT_EXPIRED;

      iratman_log_report_event( IRATMAN_LOG_T_RESEL_EXP_EVENT,
                                RAT_INT, RAT_LTE,
                                NULL,
                                treselect_node );
    }
    else
    {
      MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
             "Treselect: Earliest expiring EARFCN=%u. Pending duration=%ums",
             treselect_node->data.lte.mc.earfcn,
             TIMETICK_MS_FROM_SCLK(
               ( uint64 )( treselect_node->data.lte.mc.treselect_exp_sclks -
                           curr_time_sclks ) )
           );

      /* Set timer for the (nearest Treselect expiry) in sclk units */
      timer_set( &iratman.lte.treselect_timer,
                 ( treselect_node->data.lte.mc.treselect_exp_sclks -
                   curr_time_sclks ),
                 0,
                 T_SCLK );
    }
  }
  else
  {
    IRATMAN_INFO_MSG( "No freqs with active Treselect" );
  }

} /* lte_treselect_timer_set */


/*===========================================================================

  FUNCTION:  lte_treselect_timer_update_list

===========================================================================*/
/*!
  @brief
    Update/Arm Treselect timers for all LTE frequencies

  @details
    This function manages the Treselect timers of all LTE frequencies.
    These timers have to be updated so the number of LTE frequencies
    to be measured is valid. If a timer has expired, clear the backoff
    state so it can be measured again.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_treselect_timer_update_list
(
  iratman_lte_list_e_type list,
  timetick_type           curr_time_sclks  /* Current time in sclks units */
)
{
  iratman_rat_node_type* node = iratman.lte.all_freq_lists[ list ].head_ptr;

  while( node != NULL )
  {
    if( node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_ACTIVE )
    {
      if( TIME_OCCURS_FIRST( node->data.lte.mc.treselect_exp_sclks,
                             curr_time_sclks ) )
      {
        node->data.lte.mc.state = IRATMAN_LTE_TRESELECT_EXPIRED;

        iratman_log_report_event( IRATMAN_LOG_T_RESEL_EXP_EVENT,
                                  RAT_INT, RAT_LTE,
                                  NULL,
                                  node );
      }
    }

    /* Move to next node in the list */
    node = node->next;
  }

} /* lte_treselect_timer_update_list */


/*===========================================================================

  FUNCTION:  lte_treselect_timer_update

===========================================================================*/
/*!
  @brief
    Update/Arm Treselect timers for all LTE frequencies

  @details
    This function manages the Treselect timers of all LTE frequencies.
    It double checks for any expired timers which may have expired without
    being programmed via the HW timer. This covers the case where an additional
    timer expires between the time Time Services module calls back to notify
    "home_rat" of the expiry, and when this function is called, which is after
    "home_rat" has been awake and is now ready to manage the timers. Once the
    expired timers are taken care of, this function checks for and programs the
    next Treselect timer if there are outstanding active timers.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_treselect_timer_update( void )
{
  timetick_type          curr_time_sclks;  /* Current time in sclks units */

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_treselect_timer_update()" );

  /* Get current time in sclk units */
  curr_time_sclks = timetick_get();

  /* ---------------------- More preferred list --------------------- */
  lte_treselect_timer_update_list(IRATMAN_LTE_MP_LIST, curr_time_sclks);

  /* ---------------------- Less preferred list --------------------- */
  lte_treselect_timer_update_list(IRATMAN_LTE_LP_LIST, curr_time_sclks);

  /* -------------------- Set new Treselect timer ------------------- */
  /* Expired Treselect timers have been taken care of above.
     Now look for additional active Treselect timers. */
  lte_treselect_timer_set( NULL );

} /* lte_treselect_timer_update */


/*===========================================================================

  FUNCTION:  lte_tbackoff_get_active_min

===========================================================================*/
/*!
  @brief
    Get LTE node with earliest expiring active Tbackoff timer

  @details
    This function finds the LTE meas node with active Tbackoff timer which has
    the shortest expiration time in the following frequency lists:
    -> More Preferred list:
        Contains frequencies to be measured every sleep period.
    -> Less Preferred list:
        Contains frequencies with priority <= "home_rat" priority.
        These frequencies are measured if "home_rat" signal strength is lower
        than the "serving frequency threshold" specified in the OTA [ATIM/ORAT].

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    Ptr to LTE node with active Tbackoff timer and the minimum expiry time.
    This function may return NULL if no timer is found.

*/
/*=========================================================================*/
STATIC iratman_rat_node_type* lte_tbackoff_get_active_min( void )
{
  iratman_rat_node_type* curr_node = NULL; /* Ptr to individual LTE freq */
  iratman_rat_node_type* min_ptr = NULL;   /* Ptr to earliest Tbackoff expiry */

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_tbackoff_get_active_min()" );

  /* ---------------------- More preferred list --------------------- */
  /* Look for frequency that has minimum Tbackoff timer
     in the more preferred list */
  curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].head_ptr;
  while( curr_node != NULL )
  {
    if( curr_node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_ACTIVE ||
        curr_node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_BUFFER_ACTIVE)
    {
      if( ( min_ptr == NULL ) ||
          TIME_OCCURS_FIRST( curr_node->data.lte.mc.tbackoff_exp_sclks,
                             min_ptr->data.lte.mc.tbackoff_exp_sclks ) )
      {
        min_ptr = curr_node;
      }
    }

    /* Move to next node in the list */
    curr_node = curr_node->next;
  }

  /* ---------------------- Less preferred list --------------------- */
  /* Continue the search in the less preferred list */
  curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].head_ptr;
  while( curr_node != NULL )
  {
    if( curr_node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_ACTIVE ||
        curr_node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_BUFFER_ACTIVE)
    {
      if( ( min_ptr == NULL ) ||
          TIME_OCCURS_FIRST( curr_node->data.lte.mc.tbackoff_exp_sclks,
                             min_ptr->data.lte.mc.tbackoff_exp_sclks ) )
      {
        min_ptr = curr_node;
      }
    }

    /* Move to next node in the list */
    curr_node = curr_node->next;
  }

  return( min_ptr );

} /* lte_tbackoff_get_active_min */


/*===========================================================================

  FUNCTION:  lte_tbackoff_timer_set

===========================================================================*/
/*!
  @brief
    Arm HW timer for a LTE node with earliest expiring active Tbackoff timer

  @details
    This function sets the back off timer for this frequency since it failed
    the RSRP/RSRQ reselection comparison. The back off timer is used to save
    battery life by avoiding continuous searches to LTE frequencies with
    low signal strength. The back off timer does not require a fast wake up.
    Once the timer expires, it is considered for future measurements.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_tbackoff_timer_set
(
  iratman_rat_node_type*      curr_node //!< RAT node that was being measured
)
{
  timetick_type          curr_time_sclks;  /* Current time in sclks units */
  iratman_rat_node_type* tbackoff_node = NULL; /* Node to set Tbackoff timer */

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_tbackoff_timer_set()" );

  /* Clear Tbackoff timer as we are going to run through the lists anyway */
  (void) timer_clr( &iratman.lte.tbackoff_timer, T_NONE );

  /* Get current time in sclk units */
  curr_time_sclks = timetick_get();

  if( curr_node != NULL )
  {
    /* Indicate that Tbackoff timer is active for this frequency */
    curr_node->data.lte.mc.state = IRATMAN_LTE_TBACKOFF_ACTIVE;

    /* --------------------- Update timer values ---------------------- */
    /* Calculate time when timer expires, required to find the earliest timer.
       Don't need to worry about wrap-around since timetick_type is unsigned
       and several order magnitude larger than timeout values.
       NOTE:
       As per the spec, the max duration we might use is 32768000ms (2^15 s).
       This is well within the max value of 131071999ms which is obtained by
       using a data_type of uint32 and assuming a SCLK frequency of 32768 Hz.
       Hence, we can truncate the value returned by TIMETICK_SCLK_TO_MS. */
    curr_node->data.lte.mc.tbackoff_exp_sclks = curr_time_sclks +
      ( uint32 )TIMETICK_SCLK_FROM_MS(
                ( uint64 ) curr_node->data.lte.mc.tbackoff_dur_ms[RAT_INT] );

    MSG_5( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "Set Tbackoff timer for EARFCN=%u, duration=%ums, "
           "duration_sclks=%usclks, curr=%usclks, exp=%usclks",
           curr_node->data.lte.mc.earfcn,
           curr_node->data.lte.mc.tbackoff_dur_ms[RAT_INT],
           ( uint32 )TIMETICK_SCLK_FROM_MS(
                  ( uint64 )curr_node->data.lte.mc.tbackoff_dur_ms[RAT_INT] ),
           curr_time_sclks,
           curr_node->data.lte.mc.tbackoff_exp_sclks );

    iratman_log_report_event( IRATMAN_LOG_T_BACKOFF_START_EVENT,
                              RAT_INT, RAT_LTE,
                              NULL,
                              curr_node );
  }

  /* ----------------------- Get current timer ---------------------- */
  /* If there are other timers active, need to find the earliest timer to arm,
     using system time. This way, one HW timer can be used for multiple
     Tbackoff timers by setting up another one every time one expires */

  /* If there are multiple active timers, find the Tbackoff timer with the
     shortest expiration from now */
  tbackoff_node = lte_tbackoff_get_active_min();

  /* -------------------- Set new Tbackoff timer ------------------- */
  if( tbackoff_node != NULL )
  {
    if( TIME_OCCURS_FIRST( tbackoff_node->data.lte.mc.tbackoff_exp_sclks,
                           curr_time_sclks ) )
    {
      IRATMAN_ERROR_MSG( "Tbackoff already expired. Do not arm HW timer" );

      /* Since sclk count is <= 0, indicate that Tbackoff timer has expired
         without arming HW timer. The expired timer will be processed next
         wake up time. This should happen in extremely rare cases where two
         tbackoff timers expires very close to one another. */
      tbackoff_node->data.lte.mc.state = IRATMAN_LTE_INACTIVE;

      iratman_log_report_event( IRATMAN_LOG_T_BACKOFF_EXP_EVENT,
                                RAT_INT, RAT_LTE,
                                NULL,
                                tbackoff_node );

      /* Reset timer expiry */
      tbackoff_node->data.lte.mc.tbackoff_exp_sclks = 0;
    }
    else
    {
      MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
             "Tbackoff: Earliest expiring EARFCN=%u. Pending duration=%ums",
             tbackoff_node->data.lte.mc.earfcn,
             TIMETICK_MS_FROM_SCLK(
               ( uint64 )( tbackoff_node->data.lte.mc.tbackoff_exp_sclks -
                           curr_time_sclks ) )
           );

      /* Set timer for the (nearest Tbackoff expiry) in sclk units */
      timer_set( &iratman.lte.tbackoff_timer,
                 ( ( tbackoff_node->data.lte.mc.tbackoff_exp_sclks -
                     curr_time_sclks ) ),
                 0,
                 T_SCLK );
    }
  }
  else
  {
    IRATMAN_INFO_MSG( "No freqs with active Tbackoff" );
  }

} /* lte_tbackoff_timer_set */


/*===========================================================================

  FUNCTION:  lte_tbackoff_timer_update_list

===========================================================================*/
/*!
  @brief
    Update/Arm Tbackoff timers for all LTE frequencies in a list

  @details
    This function manages the Tbackoff timers of all LTE frequencies in
    the list specified by input parameter "list".
              These timers have to be updated so the number of LTE frequencies
              to be measured is valid. If a timer has expired, clear the backoff
              state so it can be measured again.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated
    This function is called when the IRATMan API is called

  @return
    TRUE if caller should invoke the client wakeup call back function.
    Otherwise FALSE.

*/
/*=========================================================================*/
STATIC void lte_tbackoff_timer_update_list
(
  iratman_lte_list_e_type list,
  timetick_type           curr_time_sclks  /* Current time in sclks units */
)
{
  iratman_rat_node_type* node = iratman.lte.all_freq_lists[ list ].head_ptr;

  while( node != NULL )
  {
    if( node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_ACTIVE)
    {
      if( TIME_OCCURS_FIRST( node->data.lte.mc.tbackoff_exp_sclks,
                             curr_time_sclks ) )
      {

          iratman_log_report_event( IRATMAN_LOG_T_BACKOFF_EXP_EVENT,
                                    RAT_INT,
                                    RAT_LTE,
                                    NULL,
                                    node );
          node->data.lte.mc.state = IRATMAN_LTE_INACTIVE;
          node->data.lte.mc.tbackoff_exp_sclks = 0;
      }
    }
    else if( node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_BUFFER_ACTIVE )
    {
      node->data.lte.mc.state = IRATMAN_LTE_INACTIVE;
      node->data.lte.mc.tbackoff_exp_sclks = 0;
    }

    /* Move to next node in the list */
    node = node->next;
  }

} /* lte_tbackoff_timer_update_list */


/*===========================================================================

  FUNCTION:  lte_tbackoff_timer_expiry_update_list

===========================================================================*/
/*!
  @brief
    Called when Tbackoff timer expiry occurs,
    Update/Arm Tbackoff timers for all LTE frequencies in a list

  @details
    When Tbackoff timer expiry occurs, this function manages the Tbackoff
    timers of all LTE frequencies in the list specified by input parameter
    "list". These timers have to be updated so the number of LTE frequencies to
    be measured is valid. If a timer has expired, clear the backoff state so it
    can be measured again.

    Clients are notified via wakeup call back after the Tbackoff +
    Buffer expires.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated
    This function is called by the Tbackoff expiry call back.

  @return
    TRUE if caller should invoke the client wakeup call back function.
    Otherwise FALSE.

*/
/*=========================================================================*/
STATIC boolean lte_tbackoff_timer_expiry_update_list
(
  iratman_lte_list_e_type list,
  timetick_type           curr_time_sclks  /* Current time in sclks units */
)
{
  boolean should_invoke_client_wakeup_cb = FALSE;
  iratman_rat_node_type* node = iratman.lte.all_freq_lists[ list ].head_ptr;

  while( node != NULL )
  {
    if( node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_ACTIVE )
    {
      if( TIME_OCCURS_FIRST( node->data.lte.mc.tbackoff_exp_sclks,
                             curr_time_sclks ) )
      {
        iratman_log_report_event( IRATMAN_LOG_T_BACKOFF_EXP_EVENT,
                                  RAT_INT,
                                  RAT_LTE,
                                  NULL,
                                  node );
        node->data.lte.mc.state = IRATMAN_LTE_TBACKOFF_BUFFER_ACTIVE;
        node->data.lte.mc.tbackoff_exp_sclks =
          curr_time_sclks + TIMETICK_SCLK_FROM_MS(IRATMAN_LTE_TIME_BUFFER_MS );
      }
    }
    else if( node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_BUFFER_ACTIVE )
    {
      if( TIME_OCCURS_FIRST( node->data.lte.mc.tbackoff_exp_sclks,
                             curr_time_sclks ) )
      {
        should_invoke_client_wakeup_cb = TRUE;
        node->data.lte.mc.state = IRATMAN_LTE_INACTIVE;
        node->data.lte.mc.tbackoff_exp_sclks = 0;
      }
    }

    /* Move to next node in the list */
    node = node->next;
  }

  return should_invoke_client_wakeup_cb;

} /* lte_tbackoff_timer_expiry_update_list */


/*===========================================================================

  FUNCTION:  lte_tbackoff_timer_update

===========================================================================*/
/*!
  @brief
    Update/Arm Tbackoff timers for all LTE frequencies

  @details
    This function manages the Tbackoff timers of all LTE frequencies.
              These timers have to be updated so the number of LTE frequencies
              to be measured is valid. If a timer has expired, clear the backoff
              state so it can be measured again.

    Clients are notified via wakeup call back after the Tbackoff +
    Buffer expires.


  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    None.

*/
/*=========================================================================*/
STATIC void lte_tbackoff_timer_update( void )
{
  timetick_type          curr_time_sclks;  /* Current time in sclks units */

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_tbackoff_timer_update()" );

  /* Get current time in sclk units */
  curr_time_sclks = timetick_get();

  /* ---------------------- More preferred list --------------------- */
  lte_tbackoff_timer_update_list(IRATMAN_LTE_MP_LIST, curr_time_sclks);

  /* ---------------------- Less preferred list --------------------- */
  lte_tbackoff_timer_update_list(IRATMAN_LTE_LP_LIST, curr_time_sclks);

  /* -------------------- Set new Tbackoff timer ------------------- */
  /* Expired Tbackoff timers have been taken care of above.
     If there are also other timers active, need to find the earliest timer
     to arm, using the system time. This way, one HW timer can be used
     for multiple Tbackoff timers by setting up another one every time one
     expires. */
  lte_tbackoff_timer_set( NULL );

} /* lte_tbackoff_timer_update */


/*===========================================================================

  FUNCTION:  lte_tbackoff_timer_expiry_update

===========================================================================*/
/*!
  @brief
    Update/Arm Tbackoff timers for all LTE frequencies for Tbackoff expiry

  @details
    This function maintains the Tbackoff timers of all LTE frequencies.
    These timers have to be updated so the number of LTE frequencies
    to be measured is valid. If a timer has expired, clear the backoff
    state so it can be measured again.

    Clients are notified via wakeup call back after Tbackoff +
    Buffer expires.


  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    TRUE if caller should invoke the client wakeup call back function.
    Otherwise FALSE.

*/
/*=========================================================================*/
STATIC boolean lte_tbackoff_timer_expiry_update( void )
{
  timetick_type          curr_time_sclks;  /* Current time in sclks units */
  boolean should_invoke_client_wakeup_cb = FALSE;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_tbackoff_timer_expiry_update()" );

  /* Get current time in sclk units */
  curr_time_sclks = timetick_get();

  /* ---------------------- More preferred list --------------------- */
  should_invoke_client_wakeup_cb |=
    lte_tbackoff_timer_expiry_update_list(IRATMAN_LTE_MP_LIST, curr_time_sclks);

  /* ---------------------- Less preferred list --------------------- */
  should_invoke_client_wakeup_cb |=
    lte_tbackoff_timer_expiry_update_list(IRATMAN_LTE_LP_LIST, curr_time_sclks);

  /* -------------------- Set new Tbackoff timer ------------------- */
  /* Expired Tbackoff timers have been taken care of above.
     If there are also other timers active, need to find the earliest timer
     to arm, using the system time. This way, one HW timer can be used
     for multiple Tbackoff timers by setting up another one every time one
     expires. */
  lte_tbackoff_timer_set( NULL );

  return should_invoke_client_wakeup_cb;

} /* lte_tbackoff_timer_expiry_update */


/*===========================================================================

  FUNCTION:  lte_clear_timers

===========================================================================*/
/*!
  @brief
    Clear all LTE related HW timers

  @details
    This function clear the Treselect/Tbackoff timers used for LTE measurements
    & reselection decisions.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_clear_timers( void )
{
  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_clear_timers()" );

  /* Clear Treselect timer */
  (void) timer_clr( &iratman.lte.treselect_timer, T_NONE );

  /* Clear Tbackoff timer */
  (void) timer_clr( &iratman.lte.tbackoff_timer, T_NONE );

} /* lte_clear_timers */


/*===========================================================================

  FUNCTION:  lte_treselect_timer_exp_cb

===========================================================================*/
/*!
  @brief
    Callback to handle the Treselect timer expiry

  @details
    This function is called when the Treselect timer expires.
    The LTE frequencies with active Treselect timers are updated.
    It also notifies the RATs about a pending measurement via wakeup_cb().

  @note
    iratman_init() should have been previously called
    If no RAT agrees to do the measurement, IRATMan ignores the failure
    and continues to wait for some RAT to request for measurement.

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_treselect_timer_exp_cb
(
  timer_cb_data_type       data             /* data */
)
{
  int                      rat     = RAT_INT;
  boolean                  success = FALSE;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_treselect_timer_exp_cb()" );

  /* Clear Treselect timer */
  (void) timer_clr( &iratman.lte.treselect_timer, T_NONE );

  /* Check for Treselect timer expiry, the frequency with expired
     Treselect timer requires special handling */
  lte_treselect_timer_update();

  /* No RAT has attempted measurement. Trigger a measurement via call_back() */
  for( rat = RAT_INT + 1; rat < RAT_MAX; rat++ )
  {
    if( ( iratman.wakeup_cb[rat] != NULL ) && iratman.wakeup_cb[rat]() )
    {
      success = TRUE;
    }
  }

  if( success == FALSE )
  {
    IRATMAN_ERROR_MSG( "None of the RATs agreed to do the measurement. "
                       "Ignore and hope for best" );
  }

} /* lte_treselect_timer_exp_cb */


/*===========================================================================

  FUNCTION:  lte_tbackoff_timer_exp_cb

===========================================================================*/
/*!
  @brief
    Callback to handle the Tbackoff timer expiry

  @details
    This function is called when the Tbackoff timer expires.
    The LTE frequencies with active Tbackoff timers are updated.
    It also notifies the RATs about a pending measurement via wakeup_cb().

  @note
    iratman_init() should have been previously called
    If no RAT agrees to do the measurement, IRATMan ignores the failure
    and continues to wait for some RAT to request for measurement.

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_tbackoff_timer_exp_cb
(
  timer_cb_data_type       data             /* data */
)
{
  iratman_rat_e_type rat = RAT_INT;
  boolean success = FALSE;
  boolean shouldInvokeClientWakeupCB;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_tbackoff_timer_exp_cb()" );

  /* Clear Tbackoff timer */
  (void) timer_clr( &iratman.lte.tbackoff_timer, T_NONE );

  /* Check for Tbackoff timer expiry, the frequency with expired
     Tbackoff timer is eligible for pilot measurements */
  shouldInvokeClientWakeupCB = lte_tbackoff_timer_expiry_update();

  /* No RAT has attempted measurement. Trigger a measurement via call_back() */
  if (shouldInvokeClientWakeupCB == TRUE)
  {
    for( rat = RAT_INT + 1; rat < RAT_MAX; rat++ )
    {
      if( ( iratman.wakeup_cb[rat] != NULL ) && iratman.wakeup_cb[rat]() )
      {
        success = TRUE;
      }
    }

    if( success == FALSE )
    {
      IRATMAN_ERROR_MSG( "None of the RATs agreed to do the measurement. "
                         "Ignore and hope for best" );
    }
  }
} /* lte_tbackoff_timer_exp_cb */


/*===========================================================================

  FUNCTION:  lte_def_timers

===========================================================================*/
/*!
  @brief
    Define LTE related HW timers

  @details
    This function defines the Treselect/Tbackoff timers which are used for
    LTE measurements & reselection.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_def_timers( void )
{
  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_def_timers()" );

  /* Define the Treselect timer */
  timer_def( &iratman.lte.treselect_timer, NULL, NULL, 0,
             lte_treselect_timer_exp_cb, 0 );

  /* Define the Tbackoff timer */
  timer_def( &iratman.lte.tbackoff_timer, NULL, NULL, 0,
             lte_tbackoff_timer_exp_cb, 0 );

} /* lte_def_timers */


/*===========================================================================

  FUNCTION:  lte_check_meas_delayed

===========================================================================*/
/*!
  @brief
    Check if the measurement request for the "node" is delayed

  @details
    This function checks whether the measurement results for "node" are
    stale. Since IRATMan is a passive module, it can't force its clients to do
    the measurement. The TECHs are expected to request IRATMan frequently
    (based on their respective sleep time lines). There could be cases where in
    none of the active TECHs come and request for a LTE node to measure for a
    long time (e.g. chain denial scenarios). This makes the last measurement
    result to go stale and it should not be used for subsequent measurements.
    E.g.: Treselect time expired for a node, but no TECH requested the node
    for measurement for a long time. Hence, whenever a TECH comes back after a
    long delay, instead of using stale meas results, IRATMan should restart the
    measurement for that particular LTE node.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: TRUE if the measurement was delayed. Else, FALSE
    NOTE: This function can change the state of "node"

*/
/*=========================================================================*/
STATIC boolean lte_check_meas_delayed
(
  iratman_rat_node_type*  node    //!< Meas LTE node
)
{
  timetick_type   curr_time_sclks;      /* Current time in sclks units */
  boolean         meas_delayed = FALSE; /* Return value */

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_check_meas_delayed()" );

  /* Get current time in sclk units */
  curr_time_sclks = timetick_get();

  if( node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_EXPIRED )
  {
    /* Calculate time elapsed since Treselect expiry and check whether
       it is within allowed buffer (IRATMAN_LTE_TRESELECT_EXTENSION_MS) */
    if( TIME_OCCURS_FIRST( curr_time_sclks,
                           ( node->data.lte.mc.treselect_exp_sclks +
                             TIMETICK_SCLK_FROM_MS(
                               IRATMAN_LTE_TRESELECT_EXTENSION_MS ) )
                         )
      )
    {
      /* We are within the allowed buffer time. Continue with 2nd meas */
      meas_delayed = FALSE;
    }
    else
    {
      /* Reset the node since meas results are stale */
      MSG_5( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "Reset EARFCN=%u state from TRESELECT_EXPIRED to INACTIVE. "
             "Time: curr=%ums, Treselect_exp=%ums, delay=%ums, limit=%ums",
             node->data.lte.mc.earfcn,
             TIMETICK_MS_FROM_SCLK( curr_time_sclks ),
             TIMETICK_MS_FROM_SCLK( node->data.lte.mc.treselect_exp_sclks ),
             TIMETICK_MS_FROM_SCLK( curr_time_sclks -
                                    node->data.lte.mc.treselect_exp_sclks ),
             IRATMAN_LTE_TRESELECT_EXTENSION_MS
           );

      /* Set the state as INACTIVE */
      node->data.lte.mc.state = IRATMAN_LTE_INACTIVE;

      /* Reset the timers */
      node->data.lte.mc.treselect_exp_sclks = 0;
      node->data.lte.mc.tbackoff_exp_sclks = 0;

      /* Measurement is delayed  */
      meas_delayed = TRUE;
    }
  }

  return( meas_delayed );

} /* lte_check_meas_delayed */


/*===========================================================================

  FUNCTION:  lte_get_next_freq

===========================================================================*/
/*!
  @brief
    Return next LTE frequency to measure

  @details
    This function is called to get the next LTE frequency for which a
    measurement is pending[as per spec] in the following frequency lists:
    -> More Preferred list:
        Contains frequencies to be measured every sleep period.
    -> Less Preferred list:
        Contains frequencies with priority <= "home_rat" priority.
        These frequencies are measured if "home_rat" signal strength is lower
        than the "serving frequency threshold" specified in the OTA [ATIM/ORAT].

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    Ptr to LTE node which is to be measured.
    NULL if no LTE nodes are ready for measurement.

*/
/*=========================================================================*/
STATIC iratman_rat_node_type* lte_get_next_freq
(
  iratman_rat_e_type  home_rat,       //!< Home/Originating RAT
  uint8               home_rat_ecio   //!< Home RAT signal strength [-0.5 dB]
)
{
  /* Ptr to individual LTE frequency entry in frequency list */
  iratman_rat_node_type*  curr_node = NULL;

  /* Ptr to node to be measured from INACTIVE list using round-robin approach */
  iratman_rat_node_type*  meas_node = NULL;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_get_next_freq()" );

  /* First, measure frequencies that have expired Treselect timer
     so look for them in both more preferred and less preferred lists. */

  /* -------- EXPIRED Treselect timer in More preferred list -------- */
  /* Look for expired timers in more preferred list */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].cnt > 0 )
  {
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].head_ptr;
    while( curr_node != NULL )
    {
      if( ( curr_node->hdr.in_use == FALSE ) &&
          ( curr_node->hdr.rats_known & RAT_MASK( home_rat ) ) &&
          ( curr_node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_EXPIRED ) )
      {
        /* If measurement is delayed, mark the node as INACTIVE */
        ( void ) lte_check_meas_delayed( curr_node );

        /* Return this node back to requesting RAT */
        return curr_node;
      }

      /* Move to next freq in the list */
      curr_node = curr_node->next;
    }
  }

  /* -------- EXPIRED Treselect timer in Less preferred list -------- */
  /* Look for expired timers in less preferred list */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].cnt > 0 )
  {
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].head_ptr;
    while( curr_node != NULL )
    {
      if( ( curr_node->hdr.in_use == FALSE ) &&
          ( curr_node->hdr.rats_known & RAT_MASK( home_rat ) ) &&
          ( curr_node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_EXPIRED ) )
      {
        /* ThreshServing description per C.S0097-0_v2.0 & C.S0005-E_v3.0 spec:

           The base station shall set this field to [-2 * 10 * log10PS],
           where PS is the strength of the reference pilot of the serving BS.

           Both EcIo and ThreshServing are represented as positive numbers
           [-0.5 dB units] even though they are negative numbers. Thus, the "<"
           in the evaluation: ( RSRP of Serving C2K < ThreshServing )
           is changed to ">" in the SW implementation to account for the
           negative units used. */

        /* (RSRP of Serving C2K < ThreshServing)? */
        if( home_rat_ecio > curr_node->data.lte.mc.thresh_serving[home_rat] )
        {
          MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                 "Using TRESELECT_EXPIRED LP list freq: "
                 "home_rat=%d, EARFCN=%u, home_ecio=%u, Thresh=%u (-0.5dB)",
                 home_rat,
                 curr_node->data.lte.mc.earfcn,
                 home_rat_ecio,
                 curr_node->data.lte.mc.thresh_serving[home_rat] );

          /* If measurement is delayed mark the node as INACTIVE */
          ( void ) lte_check_meas_delayed( curr_node );

          /* Return this node back to requesting RAT */
          return curr_node;
        }
        else
        {
          MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                 "Skipping TRESELECT_EXPIRED LP list freq: "
                 "home_rat=%d, EARFCN=%u, home_ecio=%u, Thresh=%u (-0.5dB)",
                 home_rat,
                 curr_node->data.lte.mc.earfcn,
                 home_rat_ecio,
                 curr_node->data.lte.mc.thresh_serving[home_rat] );

          /* For cases where in serving RAT has higher priority than LTE,
             the UE should try to stay on the serving RAT as long as possible.
             Hence, whenever the RSRP of serving RAT > LTE, we move the LTE
             node to INACTIVE state there by avoiding reselecting to LTE.

             If the LTE EARFCN is known to only home_rat, set the state of the
             LTE EARFCN to INACTIVE state. If it is known to other RATs, we
             allow the other RAT to continue from TRESELECT_EXPIRED state. */
          if( curr_node->hdr.rats_known == RAT_MASK( home_rat ) )
          {
            MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                   "Change EARFCN=%u state from TRESELECT_EXPIRED to INACTIVE. "
                   "(home_ecio > ThreshServing)",
                   curr_node->data.lte.mc.earfcn );

            /* Set the state as INACTIVE */
            curr_node->data.lte.mc.state = IRATMAN_LTE_INACTIVE;

            /* Reset the timers */
            curr_node->data.lte.mc.treselect_exp_sclks = 0;
            curr_node->data.lte.mc.tbackoff_exp_sclks = 0;
          }
        } /* (RSRP of Serving C2K < ThreshServing)? */
      } /* (known to home_rat) && (Treselect timer expired) */

      /* Move to next freq in the list */
      curr_node = curr_node->next;
    }
  }

  /* ------------ INACTIVE timers in More preferred list ------------ */
  /* Next add LTE frequencies in more preferred list with no active
     backoff timer and no active reselect timer */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].cnt > 0 )
  {
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].head_ptr;
    while( curr_node != NULL )
    {
      if( ( curr_node->hdr.in_use == FALSE ) &&
          ( curr_node->hdr.rats_known & RAT_MASK( home_rat ) ) &&
          ( curr_node->data.lte.mc.state == IRATMAN_LTE_INACTIVE ) )
      {
        /* Round-robin way of returning nodes for measurement:
            Use curr_node as meas_node if any of the below conditions is met:
              1. This is the first INACTIVE node we found
              2. curr_node was measured prior to meas_node. If curr_node and 
                 meas_node both were never measured before, then meas_node will
                 not be updated. If curr_node was never measured but meas_node 
                 has been measured before, then meas_node will be updated. */
        if( ( meas_node == NULL ) ||
            ( curr_node->data.lte.mc.last_used_sclks <
                meas_node->data.lte.mc.last_used_sclks ) )
        {
          meas_node = curr_node;
        }
      } /* (known to home_rat) && (Timer inactive) */

      /* Move to next freq in the list */
      curr_node = curr_node->next;
    }
  }

  /* Return the INACTIVE node found in the MP_LIST */
  if( meas_node )
  {
    return meas_node;
  }

  /* ------------ INACTIVE timers in Less preferred list ------------ */
  /* Only consider adding LTE frequencies in less preferred list
     if home RAT signal strength is < home RAT ThreshServing threshold */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].cnt > 0 )
  {
    /* Add LTE frequencies in less preferred list with no active
       backoff timer and no active reselect timer */
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].head_ptr;
    while( curr_node != NULL )
    {
      if( ( curr_node->hdr.in_use == FALSE ) &&
          ( curr_node->hdr.rats_known & RAT_MASK( home_rat ) ) &&
          ( curr_node->data.lte.mc.state == IRATMAN_LTE_INACTIVE ) )
      {
        /* ThreshServing description per C.S0097-0_v2.0 & C.S0005-E_v3.0 spec:

           The base station shall set this field to [-2 * 10 * log10PS],
           where PS is the strength of the reference pilot of the serving BS.

           Both EcIo and ThreshServing are represented as positive numbers
           [-0.5 dB units] even though they are negative numbers. Thus, the "<"
           in the evaluation: ( RSRP of Serving C2K < ThreshServing )
           is changed to ">" in the SW implementation to account for the
           negative units used. */

        /* (RSRP of Serving C2K < ThreshServing)? */
        if( home_rat_ecio > curr_node->data.lte.mc.thresh_serving[home_rat] )
        {
          MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                 "Using INACTIVE LP list freq: "
                 "home_rat=%d, EARFCN=%u, home_ecio=%u, Thresh=%u (-0.5dB)",
                 home_rat,
                 curr_node->data.lte.mc.earfcn,
                 home_rat_ecio,
                 curr_node->data.lte.mc.thresh_serving[home_rat] );

          /* Round-robin way of returning nodes for measurement:
              Use curr_node as meas_node if any of the below conditions is met:
                1. This is the first INACTIVE node we found
                2. curr_node was measured prior to meas_node. If curr_node and 
                   meas_node both were never measured before, then meas_node will
                   not be updated. If curr_node was never measured but meas_node 
                   has been measured before, then meas_node will be updated. */
          if( ( meas_node == NULL ) ||
              ( curr_node->data.lte.mc.last_used_sclks <
                  meas_node->data.lte.mc.last_used_sclks ) )
          {
            meas_node = curr_node;
          }
        }
        else
        {
          MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                 "Skipping INACTIVE LP list freq: "
                 "home_rat=%d, EARFCN=%u, home_ecio=%u, Thresh=%u (-0.5dB)",
                 home_rat,
                 curr_node->data.lte.mc.earfcn,
                 home_rat_ecio,
                 curr_node->data.lte.mc.thresh_serving[home_rat] );
        } /* (RSRP of Serving C2K < ThreshServing)? */
      } /* (known to home_rat) && (Timer inactive) */

      /* Move to next freq in the list */
      curr_node = curr_node->next;
    }
  }

  /* Return the INACTIVE node found in the LP_LIST */
  if( meas_node )
  {
    return meas_node;
  }

  return NULL;

} /* lte_get_next_freq */


/*===========================================================================

  FUNCTION:  lte_get_meas_freq_count

===========================================================================*/
/*!
  @brief
    Return the number of frequencies that need to be measured at this instant

  @details
    This function is called to get the number of frequencies for which a
    measurement is pending[as per spec] in the following frequency lists:
    -> More Preferred list:
        Contains frequencies to be measured every sleep period.
    -> Less Preferred list:
        Contains frequencies with priority <= "home_rat" priority.
        These frequencies are measured if "home_rat" signal strength is lower
        than the "serving frequency threshold" specified in the OTA [ATIM/ORAT].

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    Number of frequencies that need measurement [ uint8 ]

*/
/*=========================================================================*/
STATIC uint8 lte_get_meas_freq_count
(
  iratman_rat_e_type               home_rat,       //!< Home/Originating RAT
  uint8                            home_rat_ecio,  //!< Home RAT signal strength [-0.5 dB]
  iratman_lte_meas_node_list_type* meas_list_ptr   //!< Measurement node list
)
{
  iratman_rat_node_type*  curr_node = NULL;   //!< Ptr to individual LTE freq
  uint8                   freq_count = 0;     //!< Number of freqs to measure
  iratman_lte_meas_node_list_type meas_list;  //!< Measurement node list

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_get_meas_freq_count()" );

  /* Clear the meas_list (local copy) */
  memset( &meas_list, 0, sizeof( meas_list ) );

  /* First, measure frequencies that have expired Treselect timer
     so look for them in both more preferred and less preferred lists. */

  /* -------- EXPIRED Treselect timer in More preferred list -------- */
  /* Look for expired timers in more preferred list */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].cnt > 0 )
  {
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].head_ptr;
    while( curr_node != NULL )
    {
      if( ( curr_node->hdr.in_use == FALSE ) &&
          ( curr_node->hdr.rats_known & RAT_MASK( home_rat ) ) &&
          ( curr_node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_EXPIRED ) )
      {
        /* Count if measurement is not delayed. If it is delayed, then
           this node will be counted while checking INACTIVE freqs */
        if( lte_check_meas_delayed( curr_node ) == FALSE )
        {
          /* Add the LTE freq to the list */
          meas_list.freq_arr[freq_count].earfcn    =
                                   curr_node->data.lte.mc.earfcn;
          meas_list.freq_arr[freq_count].bandwidth =
                                   curr_node->data.lte.mc.bandwidth[home_rat];

          /* Increment the count */
          freq_count++;
        }
      }

      /* Move to next freq in the list */
      curr_node = curr_node->next;
    }
  }

  /* -------- EXPIRED Treselect timer in Less preferred list -------- */
  /* Look for expired timers in less preferred list */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].cnt > 0 )
  {
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].head_ptr;
    while( curr_node != NULL )
    {
      if( ( curr_node->hdr.in_use == FALSE ) &&
          ( curr_node->hdr.rats_known & RAT_MASK( home_rat ) ) &&
          ( curr_node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_EXPIRED ) )
      {
        /* ThreshServing description per C.S0097-0_v2.0 & C.S0005-E_v3.0 spec:

           The base station shall set this field to [-2 * 10 * log10PS],
           where PS is the strength of the reference pilot of the serving BS.

           Both EcIo and ThreshServing are represented as positive numbers
           [-0.5 dB units] even though they are negative numbers. Thus, the "<"
           in the evaluation: ( RSRP of Serving C2K < ThreshServing )
           is changed to ">" in the SW implementation to account for the
           negative units used. */

        /* (RSRP of Serving C2K < ThreshServing)? */
        if( home_rat_ecio > curr_node->data.lte.mc.thresh_serving[home_rat] )
        {
          MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                 "Counting TRESELECT_EXPIRED freq from LP list: "
                 "home_rat=%d, EARFCN=%u, home_ecio=%u, Thresh=%u (-0.5dB)",
                 home_rat,
                 curr_node->data.lte.mc.earfcn,
                 home_rat_ecio,
                 curr_node->data.lte.mc.thresh_serving[home_rat] );

          /* Count if measurement is not delayed. If it is delayed, then
             this node will be counted while checking INACTIVE freqs */
          if( lte_check_meas_delayed( curr_node ) == FALSE )
          {
            /* Add the LTE freq to the list */
            meas_list.freq_arr[freq_count].earfcn    =
                                     curr_node->data.lte.mc.earfcn;
            meas_list.freq_arr[freq_count].bandwidth =
                                     curr_node->data.lte.mc.bandwidth[home_rat];

            /* Increment the count */
            freq_count++;
          }
        }
        else
        {
          MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                 "Skipping TRESELECT_EXPIRED freq in LP list: "
                 "home_rat=%d, EARFCN=%u, home_ecio=%u, Thresh=%u (-0.5dB)",
                 home_rat,
                 curr_node->data.lte.mc.earfcn,
                 home_rat_ecio,
                 curr_node->data.lte.mc.thresh_serving[home_rat] );

          /* For cases where in serving RAT has higher priority than LTE,
             the UE should try to stay on the serving RAT as long as possible.
             Hence, whenever the RSRP of serving RAT > LTE, we move the LTE
             node to INACTIVE state there by avoiding reselecting to LTE.

             If the LTE EARFCN is known to only home_rat, set the state of the
             LTE EARFCN to INACTIVE state. If it is known to other RATs, we
             allow the other RAT to continue from TRESELECT_EXPIRED state. */
          if( curr_node->hdr.rats_known == RAT_MASK( home_rat ) )
          {
            MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                   "Change EARFCN=%u state from TRESELECT_EXPIRED to INACTIVE. "
                   "(home_ecio > ThreshServing)",
                   curr_node->data.lte.mc.earfcn );

            /* Set the state as INACTIVE */
            curr_node->data.lte.mc.state = IRATMAN_LTE_INACTIVE;

            /* Reset the timers */
            curr_node->data.lte.mc.treselect_exp_sclks = 0;
            curr_node->data.lte.mc.tbackoff_exp_sclks = 0;
          }
        } /* (RSRP of Serving C2K < ThreshServing)? */
      } /* (known to home_rat) && (Treselect timer expired) */

      /* Move to next freq in the list */
      curr_node = curr_node->next;
    }
  }

  /* ------------ INACTIVE timers in More preferred list ------------ */
  /* Next add LTE frequencies in more preferred list with no active
     backoff timer and no active reselect timer */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].cnt > 0 )
  {
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].head_ptr;
    while( curr_node != NULL )
    {
      if( ( curr_node->hdr.in_use == FALSE ) &&
          ( curr_node->hdr.rats_known & RAT_MASK( home_rat ) ) &&
          ( curr_node->data.lte.mc.state == IRATMAN_LTE_INACTIVE ) )
      {
        /* Add the LTE freq to the list */
        meas_list.freq_arr[freq_count].earfcn    =
                                 curr_node->data.lte.mc.earfcn;
        meas_list.freq_arr[freq_count].bandwidth =
                                 curr_node->data.lte.mc.bandwidth[home_rat];

        /* Increment the count */
        freq_count++;
      }

      /* Move to next freq in the list */
      curr_node = curr_node->next;
    }
  }

  /* ------------ INACTIVE timers in Less preferred list ------------ */
  /* Only consider adding LTE frequencies in less preferred list
     if home RAT signal strength is < home RAT ThreshServing threshold */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].cnt > 0 )
  {
    /* Add LTE frequencies in less preferred list with no active
       backoff timer and no active reselect timer */
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].head_ptr;
    while( curr_node != NULL )
    {
      if( ( curr_node->hdr.in_use == FALSE ) &&
          ( curr_node->hdr.rats_known & RAT_MASK( home_rat ) ) &&
          ( curr_node->data.lte.mc.state == IRATMAN_LTE_INACTIVE ) )
      {
        /* ThreshServing description per C.S0097-0_v2.0 & C.S0005-E_v3.0 spec:

           The base station shall set this field to [-2 * 10 * log10PS],
           where PS is the strength of the reference pilot of the serving BS.

           Both EcIo and ThreshServing are represented as positive numbers
           [-0.5 dB units] even though they are negative numbers. Thus, the "<"
           in the evaluation: ( RSRP of Serving C2K < ThreshServing )
           is changed to ">" in the SW implementation to account for the
           negative units used. */

        /* (RSRP of Serving C2K < ThreshServing)? */
        if( home_rat_ecio > curr_node->data.lte.mc.thresh_serving[home_rat] )
        {
          MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                 "Counting INACTIVE freq from LP list: "
                 "home_rat=%d, EARFCN=%u, home_ecio=%u, Thresh=%u (-0.5dB)",
                 home_rat,
                 curr_node->data.lte.mc.earfcn,
                 home_rat_ecio,
                 curr_node->data.lte.mc.thresh_serving[home_rat] );

          /* Add the LTE freq to the list */
          meas_list.freq_arr[freq_count].earfcn    =
                                   curr_node->data.lte.mc.earfcn;
          meas_list.freq_arr[freq_count].bandwidth =
                                   curr_node->data.lte.mc.bandwidth[home_rat];

          /* Increment the count */
          freq_count++;
        }
        else
        {
          MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                 "Skipping INACTIVE freq from LP list: "
                 "home_rat=%d, EARFCN=%u, home_ecio=%u, Thresh=%u (-0.5dB)",
                 home_rat,
                 curr_node->data.lte.mc.earfcn,
                 home_rat_ecio,
                 curr_node->data.lte.mc.thresh_serving[home_rat] );
        } /* (RSRP of Serving C2K < ThreshServing)? */
      } /* (known to home_rat) && (Timer inactive) */

      /* Move to next freq in the list */
      curr_node = curr_node->next;
    }
  }

  /* Remember the number of LTE freqs that are ready for measurement */
  meas_list.count = freq_count;

  /* Copy the list of freqs to the caller location (passed in as a ptr) */
  if( meas_list_ptr != NULL )
  {
    *meas_list_ptr = meas_list;
  }

  return freq_count;

} /* lte_get_meas_freq_count */


/*===========================================================================

  FUNCTION:  lte_cell_passed_rsrp_thresh

===========================================================================*/
/*!
  @brief
    Checks if LTE measurement passed RSRP criteria

  @details
    This function evaluates the LTE Pilot measurement results using the
    following equation:

  S_EUTRA = Q_measured - (Qrxlevmin+Qrxlevminoffset) - max(Pemax-UEmaxpower,0)

  where:
  Q_measured:      Returned from LTE in the MEAS CNF message. Each cell's
                   Q_measured is stored in an array. RSRP combined value
                   combined across Tx-Rx pairs. In linear scale.
                   Range [-144dBm to -44dBm] with resolution of 1 dBm.
                   Reference: 36.133 Sec 9.1.4

  Qrxlevmin:       Minimum received RSRP level from the EUTRA cell in dBm.
                   Range 0 to 96, where -44 – Rxlevmin RxL is equal to the min
                   reference signal received power(RSRP) level of a EUTRA

  Qrxlevminoffset: Offset to Minimum reference power level to select EUTRA.
                   The range of values allowed is 1 through 8.

  Pemax:           Maximum TX power level an UE may use when transmitting on
                   the uplink in E-UTRA (dBm.)

  UEmaxpower:      Maximum transmit power received from LTE L1 in INIT CNF
                   message in dBm.

  For all cells returned by LTE, the cell's S_EUTRA is compared
  against the threshold ThreshX which is part of the ATIM/ORAT OTA message.
  The result of the comparison is returned to the calling function.

  ThreshX:         Minimum required signal quality threshold used for selecting
                   an EARFCN in the range 0 to 31. The actual threshold value
                   (in dB) used to compare against the measured reference
                   signal receive power(RSRP) is equal to ThreshX * 2.

  @note
  iratman_init() should have been previously called
  Inputs are assumed to be pre-validated
  meas_result is a reference parameter. The function will set it's value if
  reselection criteria is met.

  @return
    none

*/
/*=========================================================================*/
STATIC void lte_cell_passed_rsrp_thresh
(
  iratman_rat_e_type                 home_rat, //!< Home/Originating RAT
  iratman_rat_node_type*             node,     //!< RAT node being measured
  lte_cphy_irat_meas_meas_results_s* cell,     //!< LTE cell being evaluated
  lte_reselect_cell_arr_struct_type* pass_arr, //!< Cell array to be updated
  iratman_lte_meas_result_type*      meas_result,
  boolean                            update_pass_arr
)
{
  /* For loop to process all cells */
  uint8     rat_idx = 0;

  /* S_EUTRA in the equation [in dBm] */
  int16     s_eutra = 0;

  /* Q_measured (LTE pilot measurement result) in the equation [in dBm] */
  int16     q_measured = 0;

  /* RxLevelMin component of the equation [in dBm] */
  int16     rxlevelmin = 0;

  /* TxLevelMax component of the equation [in dBm] */
  int16     txlevelmax = 0;

  /* Thresh X to comp s_eutra against */
  int16     thresh_x_comp_val = 0;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_cell_passed_rsrp_thresh()" );

  /* Should never happen by the time this function is called */
  ASSERT( node != NULL );
  ASSERT( cell != NULL );
  ASSERT( pass_arr != NULL );

  if ( *meas_result == IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X )
  {
    IRATMAN_ERROR_MSG( "Previous metric evaluation passed. Skipping RSRP" );
    return;
  }

  /* This for-loop runs through RSRP parameters passed-in by different RATs.
     For each RAT, it calculates S_EUTRA and then compares it to ThreshX.
     If (S_EUTRA) >= (ThreshX*2), then conclude that the cell (and frequency)
     meets the reselection criteria. */

  /* Loop through all the RATs */
  for( rat_idx = RAT_INT + 1; rat_idx < RAT_MAX; rat_idx++ )
  {
    /* Consider data only from RATs which satisfy the below conditions :
       -> RATs which uses the node
       -> RATs that have requested for RSRP algorithm for comparison */
    if( ( node->hdr.rats_known & RAT_MASK( rat_idx ) ) &&
        (( node->data.lte.cc.meas_comp_algo[rat_idx] == IRATMAN_LTE_ALGO_RSRP )||(update_pass_arr == FALSE))
      )
    {
      /* ----------------------- Validations ------------------------ */
      /* Perform range checking of RxLevMinEUTRA and Offset. */
      if( ( node->data.lte.cc.rx_lev_min_eutra[rat_idx] >
                              IRATMAN_LTE_RSRP_MIN_EUTRA_HIGH ) ||
          ( node->data.lte.cc.rx_lev_min_eutra_offset[rat_idx] >
                              IRATMAN_LTE_RSRP_MIN_EUTRA_OFFSET_HIGH ) )
      {
        MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
               "Home_RAT=%d: Params_RAT=%d:"
               "RxLevMinEUTRA=%d or RxLevMinEUTRAOffset=%d out of range. "
               "*** SKIP ***",
               home_rat,
               rat_idx,
               node->data.lte.cc.rx_lev_min_eutra[rat_idx],
               node->data.lte.cc.rx_lev_min_eutra_offset[rat_idx] );

        /* Move to next RAT */
        continue;
      }

      /* --------------------- Calculate S_EUTRA -------------------- */
      /* Calculate the Q_measured component of the equation.
         LTE provides RSRP in dBm units, range is -144dBm to -44dBm

         NOTE: For RSRP and RSRQ calculations we use MAX(Ant0, Ant1)
         LTE spec reference: Sections 5.1.1 and 5.1.3 of 36.214 v860
           "If receiver diversity is in use by the UE, the reported value
            shall not be lower than the corresponding RSRP/Q of any of the
            individual diversity branches." */
      q_measured = CPHY_IRAT_MEAS_X16_TO_INT( MAX( cell->rsrp_ant[0],
                                                   cell->rsrp_ant[1] ) );

      /* Calculate the RxLevelMin component of the equation in dBm unit */
      rxlevelmin = (int16)IRATMAN_LTE_RSRP_MIN_EUTRA_CEILING -
                   (int16)node->data.lte.cc.rx_lev_min_eutra[rat_idx] +
                   (int16)node->data.lte.cc.rx_lev_min_eutra_offset[rat_idx];

      /* Calculate the TxLevelMax component of the equation in dBm unit.
         Adjust pe_max range from [0 dBm, +63 dBm] to [-30 dBm, +33 dBm]
        (required because of different configuration on Network side.) */
      txlevelmax = MAX( ( (int16)node->data.lte.cc.pe_max[rat_idx] +
                          IRATMAN_LTE_RSRP_PE_MAX_OFFSET -
                          node->data.lte.cc.lte_max_tx_power ),
                        ( (int16)0 ) );

      /* Calculate S_EUTRA per spec requirement */
      s_eutra = q_measured - rxlevelmin - txlevelmax;

      MSG_8( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
             "Home_RAT=%d: Params_RAT=%d: EARFCN=%d: CellID=%u: "
             "S_EUTRA=%ddBm = Qmeasured(%d) - RxLevelMin(%d) - TxLevelMax(%d)",
             home_rat,
             rat_idx,
             node->data.lte.mc.earfcn,
             cell->cell_id,
             s_eutra,
             q_measured,
             rxlevelmin,
             txlevelmax );

      /* ------------------- Threshold Comparison ------------------- */
      /* NOTE: S_EUTRA, per spec, is in dBm unit, while ThreshX is in 2*dB unit.
         There is an ambiguity as to how this comparison should be done.
         For now, assume ThreshX to be in 2*dBm units instead of 2*dB units.
         Hence, ThreshX is just multiplied by 2 and a direct comparison is made.
      */
      thresh_x_comp_val = (int16) node->data.lte.cc.thresh_x[rat_idx] * 2;
      if( s_eutra >= thresh_x_comp_val )
      {
        MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u: S_EUTRA=%ddBm >= ThreshX*2=%ddBm",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               s_eutra,
               (int16)( node->data.lte.cc.thresh_x[rat_idx] * 2 ) );

        /* One LTE cell in one LTE frequency passed the reselection criteria.
           Save frequency and cell information to activate reselection process
           with upper layer. */
        if( update_pass_arr )
        {
        pass_arr->cell_arr[pass_arr->cnt].cell_id     = cell->cell_id;
        pass_arr->cell_arr[pass_arr->cnt].s_eutra_val = s_eutra;
        pass_arr->cnt++; /* Increment the count */

        MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u passed RSRP criterion provided by RAT=%d",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               rat_idx );
        }

        *meas_result = IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X;
        break;
      }
      /* IRATMAN_LA_THRESH_LTE_CLOSE = .5, to avoid FB, right shift rather than
         multiply */
      else if (s_eutra >= ( thresh_x_comp_val >> 1 )  )
      {
        MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u FAILED RSRP criterion provided by RAT=%d,"
               "but passed Close calculation",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               rat_idx );

        MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u: S_EUTRA=%ddBm >= "
               "ThreshX*2*ThreshClose=%ddBm",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               s_eutra,
               (int16)( thresh_x_comp_val >> 1 ) );

        *meas_result = IRATMAN_LTE_MEAS_RESULT_ABOVE_CLOSE;

        break;
      }
      else
      {
        MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u: (S_EUTRA=%ddBm < ThreshX*2=%ddBm). "
               "*** IGNORE ***",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               s_eutra,
               (int16)( node->data.lte.cc.thresh_x[rat_idx] * 2 ) );
      }

    } /* end of valid RAT check */

  } /* end of for loop */

} /* lte_cell_passed_rsrp_thresh */


/*===========================================================================

  FUNCTION:  lte_cell_passed_rsrq_thresh

===========================================================================*/
/*!
  @brief
    Checks if LTE measurement passed RSRQ criteria

  @details
    This function evaluates the LTE Pilot measurement results using the
    following equation:

  S = ( S_EUTRA > 0 ) && ( S_QUAL >= ThreshXQ )

  where,
    S_EUTRA = Q_measured - (Qrxlevmin+Qrxlevminoffset) - max(Pemax-UEmaxpower,0)
    S_QUAL = Qqualmeas – (Qqualmin + Qqualminoffset)

  where:
  Qqualmeas:      Returned from LTE in the MEAS CNF message. Instantaneous RSRQ
                   value combined across Tx-Rx pairs. In linear scale.
                   Range -19.5dB ... -3dB with 0.5 dB resolution.
                   Reference: 36.133 Sec 9.1.7

  Qqualmin:       Minimum received RSRQ level from the EUTRA cell in dB.
                   Range 0 to 31, where (-3 – QualMinEUTRA) in dB is equal to
                   the minimum reference signal received quality (RSRQ) level
                   of a EUTRA cell required for the UE to reselect to EUTRA.

  Qqualminoffset: Offset to Minimum reference quality level to select EUTRA.
                   The range of values allowed is 1 through 8.

  NOTE: Refer function header of lte_cell_passed_rsrp_thresh() for parameter
        definitions for S_EUTRA evaluation

  For all cells returned by LTE, the cell's S_QUAL is compared
  against the threshold ThreshXQ which is part of the ATIM/ORAT OTA message.
  The result of the comparison is returned to the calling function.

  ThreshXQ:       Minimum required reference signal received quality threshold
                   used for selecting an EARFCN in the range 0 to 31.

  @note
  iratman_init() should have been previously called
  Inputs are assumed to be pre-validated
  meas_result is a reference parameter. The function will set it's value if
  reselection criteria is met.
  @return
    none

*/
/*=========================================================================*/
STATIC void lte_cell_passed_rsrq_thresh
(
  iratman_rat_e_type                 home_rat, //!< Home/Originating RAT
  iratman_rat_node_type*             node,     //!< RAT node being measured
  lte_cphy_irat_meas_meas_results_s* cell,     //!< LTE cell being evaluated
  lte_reselect_cell_arr_struct_type* pass_arr,  //!< Cell array to be updated
  iratman_lte_meas_result_type*      meas_result
)
{
  /* For loop to process all cells */
  uint8     rat_idx = 0;

  /* S_EUTRA in the equation [in dBm] */
  int16     s_eutra = 0;

  /* Q_measured (LTE pilot measurement result) in the equation [in dBm] */
  int16     q_measured = 0;

  /* RxLevelMin component of the equation [in dBm] */
  int16     rxlevelmin = 0;

  /* TxLevelMax component of the equation [in dBm] */
  int16     txlevelmax = 0;

  /* S_QUAL in the equation [in dB] */
  int16     s_qual = 0;

  /* Q_qualmeas (LTE pilot measurement result) in the equation [in dB] */
  int16     q_qualmeas = 0;

  /* Qqualmin component of the equation [in dB] */
  int16     q_qualmin = 0;

  /* Qqualminoffset component of the equation [in dB] */
  int16     q_qualminoffset = 0;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_cell_passed_rsrq_thresh()" );

  /* Should never happen by the time this function is called */
  ASSERT( node != NULL );
  ASSERT( cell != NULL );
  ASSERT( pass_arr != NULL );

  if ( *meas_result == IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X )
  {
    IRATMAN_ERROR_MSG( "Previous metric evaluation passed. Skipping RSRQ" );
    return;
  }

  /* This for-loop runs through RSRQ parameters passed-in by different RATs.
     For each RAT, it calculates S_QUAL and then compares it to ThreshXQ.
     If (S_QUAL) >= (ThreshXQ), then conclude that the cell (and frequency)
     meets the reselection criteria. */

  /* Loop through all the RATs */
  for( rat_idx = RAT_INT + 1; rat_idx < RAT_MAX; rat_idx++ )
  {
    /* Consider data only from RATs which satisfy the below conditions :
       -> RATs which uses the node
       -> RATs that have requested for RSRP algorithm for comparison */
    if( ( node->hdr.rats_known & RAT_MASK( rat_idx ) ) &&
        ( node->data.lte.cc.meas_comp_algo[rat_idx] == IRATMAN_LTE_ALGO_RSRQ )
      )
    {
      /* ----------------------- Validations ------------------------ */
      /* Perform range checking of RxLevMinEUTRA and Offset. */
      if( ( node->data.lte.cc.rx_lev_min_eutra[rat_idx] >
                              IRATMAN_LTE_RSRP_MIN_EUTRA_HIGH ) ||
          ( node->data.lte.cc.rx_lev_min_eutra_offset[rat_idx] >
                              IRATMAN_LTE_RSRP_MIN_EUTRA_OFFSET_HIGH ) )
      {
        MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
               "Home_RAT=%d: Params_RAT=%d:"
               "RxLevMinEUTRA=%d or RxLevMinEUTRAOffset=%d out of range. "
               "*** SKIP ***",
               home_rat,
               rat_idx,
               node->data.lte.cc.rx_lev_min_eutra[rat_idx],
               node->data.lte.cc.rx_lev_min_eutra_offset[rat_idx] );

        /* Move to next RAT */
        continue;
      }

      /* Perform range checking of QualMinEUTRA and Offset. */
      if( ( node->data.lte.cc.rsrq_min_eutra[rat_idx] >
                              IRATMAN_LTE_RSRQ_MIN_EUTRA_HIGH ) ||
          ( node->data.lte.cc.rsrq_min_eutra_offset[rat_idx] >
                              IRATMAN_LTE_RSRQ_MIN_EUTRA_OFFSET_HIGH ) )
      {
        MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
               "Home_RAT=%d: Params_RAT=%d:"
               "QualMinEUTRA=%d or QualMinEUTRAOffset=%d out of range. "
               "*** SKIP ***",
               home_rat,
               rat_idx,
               node->data.lte.cc.rsrq_min_eutra[rat_idx],
               node->data.lte.cc.rsrq_min_eutra_offset[rat_idx] );

        /* Move to next RAT */
        continue;
      }

      /* --------------------- Calculate S_EUTRA -------------------- */
      /* Calculate the Q_measured component of the equation.
         LTE provides RSRP in dBm units, range is -144dBm to -44dBm

         NOTE: For RSRP and RSRQ calculations we use MAX(Ant0, Ant1)
         LTE spec reference: Sections 5.1.1 and 5.1.3 of 36.214 v860
           "If receiver diversity is in use by the UE, the reported value
            shall not be lower than the corresponding RSRP/Q of any of the
            individual diversity branches." */
      q_measured = CPHY_IRAT_MEAS_X16_TO_INT( MAX( cell->rsrp_ant[0],
                                                   cell->rsrp_ant[1] ) );

      /* Calculate the RxLevelMin component of the equation in dBm unit */
      rxlevelmin = (int16)IRATMAN_LTE_RSRP_MIN_EUTRA_CEILING -
                   (int16)node->data.lte.cc.rx_lev_min_eutra[rat_idx] +
                   (int16)node->data.lte.cc.rx_lev_min_eutra_offset[rat_idx];

      /* Calculate the TxLevelMax component of the equation in dBm unit.
         Adjust pe_max range from [0 dBm, +63 dBm] to [-30 dBm, +33 dBm]
        (required because of different configuration on Network side.) */
      txlevelmax = MAX( ( (int16)node->data.lte.cc.pe_max[rat_idx] +
                          IRATMAN_LTE_RSRP_PE_MAX_OFFSET -
                          node->data.lte.cc.lte_max_tx_power ),
                        ( (int16)0 ) );

      /* Calculate S_EUTRA per spec requirement */
      s_eutra = q_measured - rxlevelmin - txlevelmax;

      MSG_8( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
             "Home_RAT=%d: Params_RAT=%d: EARFCN=%d: CellID=%u: "
             "S_EUTRA=%ddBm = Qmeasured(%d) - RxLevelMin(%d) - TxLevelMax(%d)",
             home_rat,
             rat_idx,
             node->data.lte.mc.earfcn,
             cell->cell_id,
             s_eutra,
             q_measured,
             rxlevelmin,
             txlevelmax );

      /* ---------------------- Calculate S_QUAL -------------------- */
      /* Calculate the Qqualmeas component of the equation.
         LTE provides RSRQ in dB units, range is -19.5dB to -3dB.

         NOTE: For RSRP and RSRQ calculations we use MAX(Ant0, Ant1)
         LTE spec reference: Sections 5.1.1 and 5.1.3 of 36.214 v860
           "If receiver diversity is in use by the UE, the reported value
            shall not be lower than the corresponding RSRP/Q of any of the
            individual diversity branches." */
      q_qualmeas = CPHY_IRAT_MEAS_X16_TO_INT( MAX( cell->rsrq_ant[0],
                                                   cell->rsrq_ant[1] ) );

      /* Calculate the Qqualmin component of the equation in dB unit */
      q_qualmin = (int16)IRATMAN_LTE_RSRQ_MIN_EUTRA_CEILING -
                   (int16)node->data.lte.cc.rsrq_min_eutra[rat_idx];

      /* Calculate the Qqualminoffset component of the equation in dBm unit */
      q_qualminoffset = (int16)node->data.lte.cc.rsrq_min_eutra_offset[rat_idx];

      /* Calculate S_QUAL per spec requirement.
         Reference: 3GPP2 C.S0087-A V1.0 */
      s_qual = q_qualmeas - q_qualmin - q_qualminoffset;

      MSG_8( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
             "Home_RAT=%d: Params_RAT=%d: EARFCN=%d: CellID=%u: S_QUAL=%ddB = "
             "Qqualmeas(%d) - ( Qqualmin(%d) + Qqualminoffset(%d) )",
             home_rat,
             rat_idx,
             node->data.lte.mc.earfcn,
             cell->cell_id,
             s_qual,
             q_qualmeas,
             q_qualmin,
             q_qualminoffset );

      MSG_5( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "EARFCN=%u: CellID=%u: "
             "(S_EUTRA=%ddBm > 0) AND (S_QUAL=%d , ThreshXQ=%d)",
             node->data.lte.mc.earfcn,
             cell->cell_id,
             s_eutra,
             s_qual,
             (int16)( node->data.lte.cc.rsrq_thresh_x[rat_idx] ) );
      

      /* ------------------- Threshold Comparison ------------------- */
      /* Check for: S = ( S_EUTRA > 0 ) && ( S_QUAL >= ThreshXQ ) */
      if( ( s_eutra > (int16)0 ) &&
          ( s_qual >= (int16)( node->data.lte.cc.rsrq_thresh_x[rat_idx] ) ) )
      {
        MSG_5( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u: "
               "(S_EUTRA=%ddBm > 0) AND (S_QUAL=%d >= ThreshXQ=%d)",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               s_eutra,
               s_qual,
               (int16)( node->data.lte.cc.rsrq_thresh_x[rat_idx] ) );

        /* One LTE cell in one LTE frequency passed the reselection criteria.
           Save frequency and cell information to activate reselection process
           with upper layer. */
        pass_arr->cell_arr[pass_arr->cnt].cell_id     = cell->cell_id;
        pass_arr->cell_arr[pass_arr->cnt].s_eutra_val = s_eutra;
        pass_arr->cnt++; /* Increment the count */

        MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u passed RSRQ criterion provided by RAT=%d",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               rat_idx );

        *meas_result = IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X;
        break;
      }
      /* Check for: S = ( S_EUTRA > 0 ) &&
                        ( S_QUAL >= ThreshXQ * IRATMAN_LA_THRESH_LTE_CLOSE)
         IRATMAN_LA_THRESH_LTE_CLOSE = .5, to avoid FB, right shift rather than
         multiply */
      if( ( s_eutra > (int16)0 ) &&
          ( s_qual >= (int16)( node->data.lte.cc.rsrq_thresh_x[rat_idx] >> 1 )))
      {
        MSG_5( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u: "
               "(S_EUTRA=%ddBm > 0) AND (S_QUAL=%d >= ThreshXQ*ThresClose=%d)",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               s_eutra,
               s_qual,
               (int16)( node->data.lte.cc.rsrq_thresh_x[rat_idx] >> 1 ) );

        MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u ALMOST passed RSRQ criterion provided by RAT=%d",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               rat_idx );

        *meas_result = IRATMAN_LTE_MEAS_RESULT_ABOVE_CLOSE;

        break;
      }
      else if( s_eutra <= (int16)0 )
      {
        MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u: (S_EUTRA=%ddBm <= 0). *** IGNORE ***",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               s_eutra );
      }
      else
      {
        MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
               "EARFCN=%u: CellID=%u: (S_QUAL=%d < ThreshXQ=%d). "
               "*** IGNORE ***",
               node->data.lte.mc.earfcn,
               cell->cell_id,
               s_qual,
               (int16)( node->data.lte.cc.rsrq_thresh_x[rat_idx] ) );
      }

    } /* end of valid RAT check */

  } /* end of for loop */

} /* lte_cell_passed_rsrq_thresh */


/*===========================================================================

  FUNCTION:  lte_meas_passed_thresh_criteria

===========================================================================*/
/*!
  @brief
    Ues the cell measurement result to update the overall measurement result.

  @details
    If cell measurement result is more successful that overall measurement
    result, over write overall meas result w/ cell meas result.

  @note
    This function makes an assumption that iratman_lte_meas_result_type enum is
    ordered from least to most successful result.

  @return
    none.
*/
/*=========================================================================*/
STATIC void lte_update_overall_meas_result
(
  iratman_lte_meas_result_type *overall_meas_result,
  iratman_lte_meas_result_type *cell_meas_result
)
{
  if( *overall_meas_result < *cell_meas_result )
  {
    *overall_meas_result = *cell_meas_result;
  }

} /* lte_update_overall_meas_result */


/*===========================================================================

  FUNCTION:  lte_meas_passed_thresh_criteria

===========================================================================*/
/*!
  @brief
    Checks if LTE measurement passed any of the comparison criteria

  @details
    This function evaluates both RSRP and RSRQ criteria for the LTE Pilot
    measurement results using the parameters provided by various RATs.

  @note
  iratman_init() should have been previously called
  Inputs are assumed to be pre-validated

  @return
    iratman_lte_meas_result_type:
      IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X - If at least 1 LTE cell passed
                                              threshold.
      IRATMAN_LTE_MEAS_RESULT_ABOVE_CLOSE - If no LTE cells passed threshold,
                                            but at least 1 LTE cell was ...
      IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE - Else,
                                            IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE.
*/
/*=========================================================================*/
STATIC iratman_lte_meas_result_type lte_meas_passed_thresh_criteria
(
  iratman_rat_e_type          home_rat, //!< Home/Originating RAT
  iratman_rat_node_type*      node,      //!< RAT node that was being measured
  iratman_comp_algo_e_type    comp_algo //!< Home RAT's preferred algo 
)
{
  /* For loop to process all cells */
  uint8                                      idx = 0;

  /* Ptr to either array 1 for the first measurement, or array 2 for the
     second measurement */
  lte_reselect_cell_arr_struct_type*         curr_reselect_cell_arr_ptr = NULL;

  /* Init to failure. Evaluation functions will update if reselection criteria
     is met.
   */
  iratman_lte_meas_result_type
    cell_meas_result    = IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE;
  iratman_lte_meas_result_type
    overall_meas_result = IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE;

  boolean update_curr_reselect_cell_arr_ptr = TRUE;
  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_meas_passed_thresh_criteria()" );

      MSG( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "lte_meas_passed_thresh_criteria()" );

  /* Should never happen by the time this function is called */
  ASSERT( node != NULL );

  /* -------------------- Data Range Validation --------------------- */
  /* Perform range checking of cell array */
  if( node->data.lte.cc.lte_meas_cnf_params.num_cells >
      LTE_CPHY_IMEAS_MAX_MEAS_CELLS )
  {
    MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
           "Home RAT=%d: LTE reported meas cells=%u exceeds Max=%u",
           home_rat,
           node->data.lte.cc.lte_meas_cnf_params.num_cells,
           LTE_CPHY_IMEAS_MAX_MEAS_CELLS );

    return overall_meas_result;
  }

  /* ----------------- Get the relevant cell array ------------------ */
  /* Determine if this is the first, or second measurement to save cell ID info
     in the appropriate array */
  if( node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_EXPIRED )
  {
    curr_reselect_cell_arr_ptr = &node->data.lte.cc.second_meas_cells;
    MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "Home RAT=%d: 2nd measurement for EARFCN=%u",
           home_rat, node->data.lte.mc.earfcn );
  }
  else
  {
    curr_reselect_cell_arr_ptr = &node->data.lte.cc.first_meas_cells;
    MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "Home RAT=%d: 1st measurement for EARFCN=%u",
           home_rat, node->data.lte.mc.earfcn );
  }

  /* Clear the array before adding cells which pass thresh criteria */
  memset( curr_reselect_cell_arr_ptr, 0,
          sizeof( *curr_reselect_cell_arr_ptr ) );

  /* --------------------- Threshold Comparison --------------------- */
  /* This for-loop runs through all the cells in the LTE measurement result.
     It evaluates the RSRP and RSRQ criteria for each cell. If any of the cells
     meet the reselection criteria, it promotes it to the appropriate array. */

  MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Number of cells in the meas results = %d",
         node->data.lte.cc.lte_meas_cnf_params.num_cells );

  /* Loop through all the cells in the measurement response list */
  for( idx = 0; idx < node->data.lte.cc.lte_meas_cnf_params.num_cells; idx++ )
  {
    if( comp_algo == IRATMAN_BOTH_RSRP_RSRQ )
    {
      update_curr_reselect_cell_arr_ptr = FALSE;
    }

    MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "---------------- Evaluating EARFCN=%u : CellID=%u comp_algo=%d----------------",
           node->data.lte.mc.earfcn,
           node->data.lte.cc.lte_meas_cnf_params.cells[idx].cell_id,
           comp_algo );

    /* Evaluate RSRP criterion for the current cell */
    lte_cell_passed_rsrp_thresh(
       home_rat,
       node,
       &node->data.lte.cc.lte_meas_cnf_params.cells[idx],
       curr_reselect_cell_arr_ptr,
       &cell_meas_result,
       update_curr_reselect_cell_arr_ptr );

    /* If RSRP did not pass, so evaluate RSRQ criterion for the current cell */
    if( (cell_meas_result != IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X )||
         ( comp_algo == IRATMAN_BOTH_RSRP_RSRQ ))
    {
      /* In this case RSRP evaluation did not modify curr_reselect_cell_arr_ptr,
         so no need to memset */
       if( comp_algo == IRATMAN_BOTH_RSRP_RSRQ )
       {
          /* reset cell_meas_result, otherwise RSRQ evaluation do not happen */
          cell_meas_result = IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE;
       }
      /* Evaluate RSRQ criterion for the current cell */
      lte_cell_passed_rsrq_thresh(
         home_rat,
         node,
         &node->data.lte.cc.lte_meas_cnf_params.cells[idx],
         curr_reselect_cell_arr_ptr,
         &cell_meas_result);
    }

    MSG( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "------------------------------------------------------------------" );

    /* update overall measurement result */
    lte_update_overall_meas_result( &overall_meas_result, &cell_meas_result );

    /* reset cell measurement result */
    cell_meas_result = IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE;

  } /* end of for loop */

  return overall_meas_result;

} /* lte_meas_passed_thresh_criteria */


/*===========================================================================

  FUNCTION:  lte_select_winning_cell_2nd_time

===========================================================================*/
/*!
  @brief
    Check whether any LTE Cells passed threshold comparison for 2nd time

  @details
    This function implements the following algorithm, as specified by
    System Engineering:

    EUTRAReselect timer per cell:  Following algorithm will be implemented to
    ensure that the cell to which the UE is reselecting to is stable across
    two measurements.

    At time 0, when EUTRAReselect is started, 1X L1 will maintain a list of
    cells that exceed ThreshX.

    At time EUTRAReselect, 1X L1 will compare the new list of cells that exceed
    ThreshX with the stored list.  If there is an overlap between the two lists,
    reselection will be performed to the strongest cell (among those in the
    overlap list) during the 2nd measurement.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: TRUE if the strongest cell has been found. Else, FALSE.

*/
/*=========================================================================*/
STATIC boolean lte_select_winning_cell_2nd_time
(
  iratman_rat_node_type*      node,     //!< RAT node that was being measured
  iratman_rat_e_type          home_rat  //!< Home/Originating RAT
)
{
  /* Nested for loop indexes to process all cells */
  uint8                                    idx1 = 0;
  uint8                                    idx2 = 0;

  /* Return whether or not the winning cell was found */
  boolean                                  found_winning_cell = FALSE;

  /* Ptr to array 1, containing winning cell IDs in the 1st measurement */
  lte_reselect_cell_arr_struct_type*       reselect_cell_arr1_ptr = NULL;

  /* Ptr to array 2, containing winning cell IDs in the 2nd measurement */
  lte_reselect_cell_arr_struct_type*       reselect_cell_arr2_ptr = NULL;

  /* Cell with max level of energy */
  iratman_lte_reselect_type*               reselect_cell = NULL;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_select_winning_cell_2nd_time()" );

  /* Should never happen by the time this function is called */
  ASSERT( node != NULL );

  /* Update array pointers */
  reselect_cell_arr1_ptr = &node->data.lte.cc.first_meas_cells;
  reselect_cell_arr2_ptr = &node->data.lte.cc.second_meas_cells;
  reselect_cell          = &node->data.lte.cc.reselect_cell;

  /* Clear reselection data */
  memset( reselect_cell, 0, sizeof( iratman_lte_reselect_type ) );

  /* Perform range checking of cell arrays */
  ASSERT( ( reselect_cell_arr1_ptr->cnt > 0 ) &&
          ( reselect_cell_arr1_ptr->cnt <= IRATMAN_LTE_MAX_MEAS_CELLS ) );
  ASSERT( ( reselect_cell_arr2_ptr->cnt > 0 ) &&
          ( reselect_cell_arr2_ptr->cnt <= IRATMAN_LTE_MAX_MEAS_CELLS ) );

  /* These nested for loops find a match between cell array 1 containing cell
     IDs of those which pass reselection criteria for the first measurement,
     and cell array 2 containing cell IDs of those which pass reselection
     criteria for the second measurement. The matched cell ID with the
     greatest energy level is the winning cell ID. */

  /* Initialize max cell energy & cell_id to zero */
  reselect_cell->valid       = FALSE;
  reselect_cell->earfcn      = node->data.lte.mc.earfcn;
  reselect_cell->s_eutra_val = 0;
  reselect_cell->cell_id     = 0;

  for( idx1 = 0; idx1 < reselect_cell_arr1_ptr->cnt; idx1++ )
  {
    for( idx2 = 0; idx2 < reselect_cell_arr2_ptr->cnt; idx2++ )
    {
      /* Finding common Cell_ID between 1st and 2nd measurement */
      if( reselect_cell_arr1_ptr->cell_arr[ idx1 ].cell_id ==
                            reselect_cell_arr2_ptr->cell_arr[ idx2 ].cell_id )
      {
        /* Finding strongest Cell_ID for reselection */
        if( reselect_cell_arr2_ptr->cell_arr[ idx2 ].s_eutra_val >
                                                  reselect_cell->s_eutra_val )
        {
          reselect_cell->cell_id =
                         reselect_cell_arr2_ptr->cell_arr[ idx2 ].cell_id;
          reselect_cell->s_eutra_val =
                         reselect_cell_arr2_ptr->cell_arr[ idx2 ].s_eutra_val;

          found_winning_cell = TRUE;
        }
      }
    } /* 2nd measurement cell array */
  } /* 1st measurement cell array */

  /* If the winning cell is found, save the cell ID to give to upper layer
     in the reselection call back function. */
  if( found_winning_cell == TRUE )
  {
    /* Reselection data is valid */
    reselect_cell->valid = TRUE;

    if( iratman_is_dynamic_backoff_enabled() )
    {
      iratman_la_report_event(&iratman.lte.la,
                              IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X,
                              node,
                              home_rat);
    }

    MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "2nd time winning EARFCN=%u, CellID=%u, S_EUTRA=%d dBm",
           reselect_cell->earfcn,
           reselect_cell->cell_id,
           reselect_cell->s_eutra_val );
  }

  return found_winning_cell;

} /* lte_select_winning_cell_2nd_time */


/*===========================================================================

  FUNCTION:  lte_select_winning_cell_1st_time

===========================================================================*/
/*!
  @brief
    Check whether any LTE Cells passed threshold comparison for 1st time

  @details
    If lte_select_winning_cell_2nd_time() failed indicating there is no match
    between the 1st and 2nd measurements, this function goes further to see if
    there is any cell in the 2nd measurement which exceeds the reselection
    criteria.

  @note
  iratman_init() should have been previously called
  Inputs are assumed to be pre-validated

  @return
    boolean: TRUE if one or more cells in the 2nd meas passed resel criteria.

*/
/*=========================================================================*/
STATIC boolean lte_select_winning_cell_1st_time
(
  iratman_rat_node_type*      node      //!< RAT node that was being measured
)
{
  /* Loop index */
  uint8                                    idx = 0;

  /* Should at least get 1 winning cell in the 2nd measurement */
  boolean                                  found_winning_cell = FALSE;

  /* Ptr to array 1, containing winning cell IDs in the 1st measurement */
  lte_reselect_cell_arr_struct_type*       reselect_cell_arr1_ptr = NULL;

  /* Ptr to array 2, containing winning cell IDs in the 2nd measurement */
  lte_reselect_cell_arr_struct_type*       reselect_cell_arr2_ptr = NULL;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_select_winning_cell_1st_time()" );

  /* Should never happen by the time this function is called */
  ASSERT( node != NULL );

  /* Update 1st meas and 2nd meas array pointers */
  reselect_cell_arr1_ptr = &node->data.lte.cc.first_meas_cells;
  reselect_cell_arr2_ptr = &node->data.lte.cc.second_meas_cells;

  /* Perform range checking of the 2nd measurement cell array */
  ASSERT( ( reselect_cell_arr2_ptr->cnt > 0 ) &&
          ( reselect_cell_arr2_ptr->cnt <=IRATMAN_LTE_MAX_MEAS_CELLS ) );

  /* Reset the cell array for the first measurement before updating it */
  memset( reselect_cell_arr1_ptr, 0,
          sizeof( node->data.lte.cc.first_meas_cells ) );

  /* We got here because there was no match between the 1st and 2nd measurement,
     but one or more of the cells in the 2nd measurement exceed reselection
     threshold. So save the list as the 1st measurement. */
  for( idx = 0; idx < reselect_cell_arr2_ptr->cnt; idx++ )
  {
    reselect_cell_arr1_ptr->cell_arr[ idx ] =
                                    reselect_cell_arr2_ptr->cell_arr[ idx ];

    reselect_cell_arr1_ptr->cnt++;

    found_winning_cell = TRUE;

    MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "EARFCN=%u: Moved CellID=%u from 2nd_list to 1st_list",
           node->data.lte.mc.earfcn,
           reselect_cell_arr2_ptr->cell_arr[ idx ].cell_id );
  }

  /* Reset the cell array containing the 2nd meas to get rid of stale data */
  memset( reselect_cell_arr2_ptr, 0,
          sizeof( node->data.lte.cc.second_meas_cells ) );

  return found_winning_cell;

} /* lte_select_winning_cell_1st_time */


/*===========================================================================

  FUNCTION:  lte_select_winning_zero_treselect

===========================================================================*/
/*!
  @brief
    Check whether any LTE Cells passed threshold comparison for 1st time.
    If cells passes, reselection succeeds since Treselect is 0.

  @details
    This is a special case, if Treselect is set to 0, then require only
    one successful pilot measurement before reselection.

  @note
  iratman_init() should have been previously called
  Inputs are assumed to be pre-validated
  Only called for special case when Treselect is set to 0

  @return
    boolean: TRUE if one or more cells in the 2nd meas passed resel criteria.

*/
/*=========================================================================*/
STATIC boolean lte_select_winning_cell_zero_treselect
(
  iratman_rat_node_type*      node,     //!< RAT node that was being measured
  iratman_rat_e_type          home_rat  //!< Home/Originating RAT
)
{
  /* Nested for loop indexes to process all cells */
  uint8                                    idx = 0;

  /* Return whether or not the winning cell was found */
  boolean                                  found_winning_cell = FALSE;

  /* Ptr to array 1, containing winning cell IDs in the 1st measurement */
  lte_reselect_cell_arr_struct_type*       reselect_cell_arr1_ptr = NULL;

  /* Cell with max level of energy */
  iratman_lte_reselect_type*               reselect_cell = NULL;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_select_winning_cell_zero_treselect()" );

  /* Should never happen by the time this function is called */
  ASSERT( node != NULL );

  /* Update array pointers */
  reselect_cell_arr1_ptr = &node->data.lte.cc.first_meas_cells;
  reselect_cell          = &node->data.lte.cc.reselect_cell;

  /* Clear reselection data */
  memset( reselect_cell, 0, sizeof( iratman_lte_reselect_type ) );

  /* Perform range checking of cell arrays */
  ASSERT( ( reselect_cell_arr1_ptr->cnt > 0 ) &&
          ( reselect_cell_arr1_ptr->cnt <= IRATMAN_LTE_MAX_MEAS_CELLS ) );

  /* Initialize max cell energy & cell_id to zero */
  reselect_cell->valid       = FALSE;
  reselect_cell->earfcn      = node->data.lte.mc.earfcn;
  reselect_cell->s_eutra_val = 0;
  reselect_cell->cell_id     = 0;

  for( idx = 0; idx < reselect_cell_arr1_ptr->cnt; idx++ )
  {
    /* Finding strongest Cell_ID for reselection */
    if( reselect_cell_arr1_ptr->cell_arr[ idx ].s_eutra_val >
        reselect_cell->s_eutra_val )
    {
      reselect_cell->cell_id =
                     reselect_cell_arr1_ptr->cell_arr[ idx ].cell_id;
      reselect_cell->s_eutra_val =
                     reselect_cell_arr1_ptr->cell_arr[ idx ].s_eutra_val;

      found_winning_cell = TRUE;
    }
  }

  /* If the winning cell is found, save the cell ID to give to upper layer
     in the reselection call back function. */
  if( found_winning_cell == TRUE )
  {
    /* Reselection data is valid */
    reselect_cell->valid = TRUE;

    if( iratman_is_dynamic_backoff_enabled() )
    {
      iratman_la_report_event(&iratman.lte.la,
                              IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X,
                              node,
                              home_rat);
    }

    MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "treselect = 0, assuming 2nd measurement same as 1st "
           "EARFCN=%u, CellID=%u, S_EUTRA=%d dBm",
           reselect_cell->earfcn,
           reselect_cell->cell_id,
           reselect_cell->s_eutra_val );
  }

  return found_winning_cell;

} /* lte_select_winning_cell_zero_treselect */


/*===========================================================================

  FUNCTION:  lte_process_meas_results

===========================================================================*/
/*!
  @brief
    Process the LTE Pilot measurement results

  @details
    This function processes the LTE pilot measurement result reported by
    "home_rat". As part of the processing of the results, it updates the LTE
    meas "node" with the result and re-arms the relevant timers.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC iratman_lte_meas_result_type lte_process_meas_results
(
  iratman_rat_e_type          home_rat, //!< Home/Originating RAT
  iratman_rat_node_type*      node,      //!< RAT node that was being measured
  iratman_comp_algo_e_type    comp_algo //!< Home RAT's preferred algo 
)
{
  iratman_lte_meas_result_type meas_result =
    IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE;
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_process_meas_results()" );

  IRATMAN_NULL_PTR_CHECK( node, meas_result );

  /* Evaluate LTE meas result against the threshold given by base station.
     Allow the creation of Treselect timer in test mode. */
  meas_result = lte_meas_passed_thresh_criteria( home_rat,
                                                 node,
                                                 comp_algo );
  if( meas_result == IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X )
  {
    /* Special case, if Treselect is set to 0, then require only
       one successful pilot measurement before reselection. */
    if ( node->data.lte.mc.treselect_dur_ms[RAT_INT] == 0 )
    {
      if (lte_select_winning_cell_zero_treselect( node, home_rat ))
      {
        node->data.lte.mc.state = IRATMAN_LTE_RESELECT;

        /* To keep logging consistent w/ "regular" cases, we must log a
           T_RESEL_START, T_RESEL_EXP, and then RESELECTION_EVENT */
        iratman_log_report_event( IRATMAN_LOG_T_RESEL_START_EVENT,
                                  home_rat, RAT_LTE,
                                  NULL,
                                  node );

        iratman_log_report_event( IRATMAN_LOG_T_RESEL_EXP_EVENT,
                                  home_rat, RAT_LTE,
                                  NULL,
                                  node );

        iratman_log_report_event( IRATMAN_LOG_RESELECTION_EVENT,
                                  home_rat, RAT_LTE,
                                  NULL,
                                  node );
      }
      else
      {
        IRATMAN_ERROR_MSG( "Could not find winning cell ID, suspect error" );

        /* Set back off timer for this frequency.  This is an error
           condition, trying to recover. */
        lte_tbackoff_timer_set( node );
      }
    }
    /* Is this the 2nd pilot measurement for this frequency? The reselection
       measurement? If so, perform more checks. */
    else if( node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_EXPIRED )
    {
      if( lte_select_winning_cell_2nd_time( node, home_rat ) )
      {
        /* Start the reselection procedure here. LTE freq passed reselection
           criteria. */
        node->data.lte.mc.state = IRATMAN_LTE_RESELECT;

        iratman_log_report_event( IRATMAN_LOG_RESELECTION_EVENT,
                                  home_rat, RAT_LTE,
                                  NULL,
                                  node );
      }
      else if( lte_select_winning_cell_1st_time( node ) )
      {
        /* Did not find a match between the 2 measurements, but if there is a
           cell that passes the reselection criteria in the 2nd measurement, it
           is saved in the list as the 1st measurement. Now start the reselect
           timer. */
        lte_treselect_timer_set( node );
      }
      else
      {
        IRATMAN_ERROR_MSG( "Could not find winning cell ID, suspect error" );

        /* Set back off timer for this frequency.  This is an error
           condition, trying to recover. */
        lte_tbackoff_timer_set( node );
      }
    }
    else /* 1st measurement for this EARFCN */
    {
      /* Set reselect timer for this frequency. This can be the candidate
         frequency for reselection. One final measurement will be taken when
         the reselect timer expires. If the final measurement also passes the
         reselection criteria, the reselection process will start. */
      lte_treselect_timer_set( node );
    }
  }
  else /* !lte_meas_passed_thresh_criteria() */
  {
    if( iratman_is_dynamic_backoff_enabled() )
    {
      iratman_la_report_event(&iratman.lte.la, meas_result, node, home_rat);
    }

    /* Set back off timer for this frequency since it failed the S_EUTRA
       reselection comparison. The back off timer is used to save battery life
       by avoiding continuous searches of LTE freqs with low signal strength. */
    lte_tbackoff_timer_set( node );
  }

  return( meas_result );
} /* lte_process_meas_results */

/*===========================================================================

  FUNCTION:  lte_add_meas_node

===========================================================================*/
/*!
  @brief
    Add LTE node for measurement into "list" specified

  @details
    This function updates an existing node or adds a new node to the
    IRAT List Manager's lists of LTE nodes.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: TRUE if RAT node was successfully added. Else, FALSE.

*/
/*=========================================================================*/
STATIC boolean lte_add_meas_node
(
  iratman_rat_e_type            home_rat,   //!< Home/Originating RAT
  iratman_lte_input_node_type*  node,       //!< RAT node to be added
  iratman_lte_list_e_type       list        //!< Meas RAT list to be used
)
{
  boolean                 ret_value = FALSE;
  iratman_rat_node_type*  temp_node = NULL;

  /*----------------------------------------------------------------------*/

  IRATMAN_NULL_PTR_CHECK( node, FALSE );

  /* Try to find the node in the LTE lists[MP & LP] */
  temp_node = lte_find_node( node->earfcn );

  if( temp_node != NULL )
  {
    /* Node already exists. Update node info. */
    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "LTE node already exists: EARFCN=%u", node->earfcn );

    if( temp_node->hdr.in_use == TRUE )
    {
      MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "RAT=%d is trying to add a node EARFCN=%u in use by other RAT. "
             "ALLOWED",
             home_rat, node->earfcn );
    }

    ret_value = lte_update_existing_node( home_rat, node, temp_node, list );
  }
  else
  {
    /* Node doesn't exist. Add a new node. */
    ret_value = lte_add_new_node( home_rat, node, list );
  }

  return ret_value;

} /* lte_add_meas_node */


/*===========================================================================

  FUNCTION:  lte_meas_pending

===========================================================================*/
/*!
  @brief
    Check whether a LTE node measurement is pending

  @details
    This function runs through the More Preferred and Less Preferred LTE lists
    and checks whether there are nodes known to "home_rat" require measurement.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    uint32: Estimated measurement duration in ms

*/
/*=========================================================================*/
STATIC uint32 lte_meas_pending
(
  iratman_rat_e_type          home_rat, //!< Home/Originating RAT
  uint32                      sleep_dur,//!< Available meas duration [in ms]
  iratman_lte_meas_req_type*  req_data  //!< LTE meas request data
)
{
  uint32  meas_dur = 0;     /* Rough estimate of measurement duration in ms */
  uint8   num_lte_freq = 0; /* Number of LTE frequencies to measure */

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_meas_pending()" );

  IRATMAN_NULL_PTR_CHECK( req_data, 0 );

  /* Check whether we have sufficient time to complete at-least one
     LTE frequency measurement + enough time to abort the measurement */
  if( sleep_dur <
      ( IRATMAN_LTE_MIN_MEAS_DURATION_MS + IRATMAN_LTE_MEAS_ABORT_BUFFER_MS ) )
  {
    MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
           "Time available[%u] < Min LTE meas duration[%u]",
           sleep_dur,
           ( IRATMAN_LTE_MIN_MEAS_DURATION_MS +
             IRATMAN_LTE_MEAS_ABORT_BUFFER_MS ) );

    return 0; /* in ms */
  }

  /* Check for Treselect timer expiry, the frequency with expired
     Treselect timer requires special handling */
  lte_treselect_timer_update();

  /* Check for Tbackoff timer expiry, the frequency with expired
     Tbackoff timer is eligible for pilot measurements */
  lte_tbackoff_timer_update();

  /* Check whether there are frequencies to measure */
  num_lte_freq = lte_get_meas_freq_count( home_rat,
                                          req_data->home_rat_ecio,
                                          NULL );

  if( num_lte_freq != 0 )
  {
    /* Rough estimate of meas duration */
    meas_dur = ( num_lte_freq * IRATMAN_LTE_MIN_MEAS_DURATION_MS ) +
                    IRATMAN_LTE_MEAS_ABORT_BUFFER_MS;

    if( meas_dur > sleep_dur )
    {
      /* What's the max we can do ?
                 (sleep_dur - ABORT_BUFFER)
      meas_dur = -------------------------- * MIN_MEAS_DURATION + ABORT_BUFFER
                     MIN_MEAS_DURATION
      */
      meas_dur = ( ( ( sleep_dur - IRATMAN_LTE_MEAS_ABORT_BUFFER_MS ) /
                     IRATMAN_LTE_MIN_MEAS_DURATION_MS ) *
                   IRATMAN_LTE_MIN_MEAS_DURATION_MS ) +
                 IRATMAN_LTE_MEAS_ABORT_BUFFER_MS;
    }
  }
  else
  {
    /* No frequencies to measure */
    meas_dur = 0;
  }

  MSG_5( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "pending=%d, home_rat=%d, meas_rat=LTE, "
         "num_freq=%u, sleep_dur=%ums, meas_dur=%ums",
         ( num_lte_freq != 0 ),
         home_rat,
         num_lte_freq,
         sleep_dur,
         meas_dur );

  /* Return measurement duration */
  return meas_dur;

} /* lte_meas_pending */


/*===========================================================================

  FUNCTION:  lte_get_min_meas_dur_ms

===========================================================================*/
/*!
  @brief
    Get minimum time required to measure a LTE node

  @details
    This function is called to get minimum time required to measure a LTE node.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC uint32 lte_get_min_meas_dur_ms
(
  iratman_rat_e_type          home_rat  //!< Home/Originating RAT
)
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_get_min_meas_dur_ms()" );

  return( IRATMAN_LTE_MIN_MEAS_DURATION_MS + IRATMAN_LTE_MEAS_ABORT_BUFFER_MS );

} /* lte_get_min_meas_dur_ms */


/*===========================================================================

  FUNCTION:  lte_get_meas_list

===========================================================================*/
/*!
  @brief
    Get a list of LTE nodes ready for measurement

  @details
    This function is called to get a list of LTE frequencies known to "home_rat"
    which require a measurement. "meas_list" is updated with the
    list of nodes that are ready for measurement.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_get_meas_list
(
  iratman_rat_e_type               home_rat,     //!< Home/Originating RAT
  iratman_lte_meas_req_type*       req_data,     //!< LTE meas request data
  iratman_lte_meas_node_list_type* meas_list_ptr //!< Measurement node list
)
{
  uint8   num_lte_freq = 0;       /* Number of LTE frequencies to measure */

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_get_meas_list()" );

  IRATMAN_NULL_PTR_CHECK(      req_data, RETURN_VOID );
  IRATMAN_NULL_PTR_CHECK( meas_list_ptr, RETURN_VOID );

  /* Check for Treselect timer expiry, the frequency with expired
     Treselect timer requires special handling */
  lte_treselect_timer_update();

  /* Check for Tbackoff timer expiry, the frequency with expired
     Tbackoff timer is eligible for pilot measurements */
  lte_tbackoff_timer_update();

  /* Check whether there are frequencies to measure */
  num_lte_freq = lte_get_meas_freq_count( home_rat,
                                          req_data->home_rat_ecio,
                                          meas_list_ptr );

  MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
         "MEAS LIST: home_rat=%d, meas_rat=LTE, num_freq=%u",
         home_rat,
         num_lte_freq );

} /* lte_get_meas_list */


/*===========================================================================

  FUNCTION:  lte_get_meas

===========================================================================*/
/*!
  @brief
    Get a LTE node to measure

  @details
    This function is called to get a LTE frequency known to "home_rat"
    which requires a measurement. "meas_node" is updated with the measurement
    node information.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_get_meas
(
  iratman_rat_e_type          home_rat, //!< Home/Originating RAT
  uint32                      sleep_dur,//!< Available meas duration [in ms]
  iratman_lte_meas_req_type*  req_data, //!< LTE meas request data
  iratman_lte_meas_node_type* meas_node //!< Measurement node [to be updated]
)
{
  iratman_rat_node_type*  temp_node = NULL;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_get_meas()" );

  IRATMAN_NULL_PTR_CHECK(  req_data, RETURN_VOID );
  IRATMAN_NULL_PTR_CHECK( meas_node, RETURN_VOID );

  /* Check if we have nodes to measure */
  if( lte_meas_pending( home_rat, sleep_dur, req_data ) > 0 )
  {
    /* Get the next node to measure */
    temp_node = lte_get_next_freq( home_rat, req_data->home_rat_ecio );

    if( temp_node != NULL )
    {
      /* Mark the node as being used & record the time of usage */
      temp_node->hdr.in_use                  = TRUE;
      temp_node->data.lte.mc.last_used_sclks = timetick_get();

      /* Update meas_node */
      meas_node->valid     = TRUE;
      meas_node->earfcn    = temp_node->data.lte.mc.earfcn;
      meas_node->bandwidth = temp_node->data.lte.mc.bandwidth[home_rat];

      MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "Meas node returned: home_rat=%d, EARFCN=%d, BW=%d",
             home_rat, meas_node->earfcn, meas_node->bandwidth );
    }
    else
    {
      IRATMAN_INFO_MSG( "No pending nodes to measure" );

      /* Mark the meas node data as invalid */
      meas_node->valid = FALSE;
    }
  }

} /* lte_get_meas */


/*===========================================================================

  FUNCTION:  lte_report_meas

===========================================================================*/
/*!
  @brief
    Report a LTE measurement

  @details
    This function is used by "home_rat" to report results for a LTE "node"
    measurement to IRATMan.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: True RAT node was successfully added

*/
/*=========================================================================*/
STATIC iratman_lte_meas_result_type lte_report_meas
(
  iratman_rat_e_type          home_rat, //!< Home/Originating RAT
  iratman_lte_meas_node_type* node,     //!< RAT node that was being measured
  iratman_lte_meas_rpt_type*  rpt,       //!< Measurement report
  iratman_comp_algo_e_type    comp_algo //!< Home RAT's preferred algo 
)
{
  iratman_rat_node_type* curr_node = NULL;
  iratman_lte_meas_result_type meas_result =
    IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE;
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_rpt_meas()" );

  IRATMAN_NULL_PTR_CHECK( node, meas_result );

  /* Find the node corresponding to the measurement result */
  curr_node = lte_find_node( node->earfcn );
  if( curr_node != NULL )
  {
    /* Mark the node as not being used anymore */
    curr_node->hdr.in_use = FALSE;

    /* Check if we have a valid report */
    if( rpt == NULL )
    {
      /* ------------ Aborting measurement ------------ */
      /* Node already marked as not "in_use". Bail out. */
      MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Measurement aborted: home_rat=%d, EARFCN=%u",
             home_rat, node->earfcn );

    }
    else
    {
      /* --------- Process measurement report --------- */

      MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "Got LTE meas results: home_rat=%d, EARFCN=%u",
             home_rat, node->earfcn );

      /* Copy max LTE tx power used */
      curr_node->data.lte.cc.lte_max_tx_power = rpt->lte_max_tx_power;

      if( rpt->lte_meas_cnf_params_ptr != NULL )
      {
        /* Copy MEAS CNF values into CC */
        curr_node->data.lte.cc.lte_meas_cnf_params =
                                                *rpt->lte_meas_cnf_params_ptr;

        /* Process measurement results */
        meas_result = lte_process_meas_results( home_rat,
                                                curr_node,
                                                comp_algo );
      }
      else
      {
        ERR_FATAL( "Invalid Measurement Report. NULL ptr passed", 0, 0, 0 );
      }
    }
  }
  else
  {
    MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_FATAL,
           "Measurement node not found: home_rat=%d, EARFCN=%u",
           home_rat, node->earfcn );
  }

  return( meas_result );
} /* lte_report_meas */

/*===========================================================================

  FUNCTION:  lte_check_reselection

===========================================================================*/
/*!
  @brief
    Check if any LTE node passed reselection criteria

  @details
    This function updates "resel" with details pertaining to LTE node which has
    passed reselection criteria. Once "home_rat" reports measurement results to
    IRATMan, this function is called to check whether any node passed
    reselection criteria and pass the information as part of the response,
    back to "home_rat". It's the home_rat's responsibility to trigger the
    reselection, given it has valid data.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_check_reselection
(
  iratman_rat_e_type          home_rat, //!< Home/Originating RAT
  iratman_lte_meas_node_type* node,     //!< RAT node that was being measured
  iratman_lte_reselect_type*  resel,    //!< Reselection node
  iratman_comp_algo_e_type    comp_algo //!< Home RAT's preferred algo 
)
{
  iratman_rat_node_type* curr_node = NULL;
  boolean                resel_found = FALSE;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_check_reselection()" );

  IRATMAN_NULL_PTR_CHECK(  node, RETURN_VOID );
  IRATMAN_NULL_PTR_CHECK( resel, RETURN_VOID );

  /* ---------------------- More preferred list --------------------- */
  curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].head_ptr;

  while( ( curr_node != NULL ) && ( resel_found == FALSE ) )
  {
    if( ( curr_node->data.lte.mc.state == IRATMAN_LTE_RESELECT ) &&
        ( curr_node->data.lte.cc.reselect_cell.valid == TRUE ) )
    {
      /* Copy the reselection data */
      *resel = curr_node->data.lte.cc.reselect_cell;

      if(comp_algo != IRATMAN_BOTH_RSRP_RSRQ)
      {
      /* Mark the node as inactive */
      curr_node->data.lte.mc.state = IRATMAN_LTE_INACTIVE;
      curr_node->data.lte.cc.reselect_cell.valid = FALSE;
      }

      /* Reset the timers */
      curr_node->data.lte.mc.treselect_exp_sclks = 0;
      curr_node->data.lte.mc.tbackoff_exp_sclks = 0;

      resel_found = TRUE;
    }

    /* Move to next freq in the list */
    curr_node = curr_node->next;
  }

  /* ---------------------- Less preferred list --------------------- */
  curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].head_ptr;

  while( ( curr_node != NULL ) && ( resel_found == FALSE ) )
  {
    if( ( curr_node->data.lte.mc.state == IRATMAN_LTE_RESELECT ) &&
        ( curr_node->data.lte.cc.reselect_cell.valid == TRUE ) )
    {
      /* Copy the reselection data */
      *resel = curr_node->data.lte.cc.reselect_cell;

      if(comp_algo != IRATMAN_BOTH_RSRP_RSRQ)
      {
      /* Mark the node as inactive */
      curr_node->data.lte.mc.state = IRATMAN_LTE_INACTIVE;
      curr_node->data.lte.cc.reselect_cell.valid = FALSE;
      }

      /* Reset the timers */
      curr_node->data.lte.mc.treselect_exp_sclks = 0;
      curr_node->data.lte.mc.tbackoff_exp_sclks = 0;

      resel_found = TRUE;
    }

    /* Move to next freq in the list */
    curr_node = curr_node->next;
  }

  if( resel_found == TRUE )
  {
    MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "ATTEMPT RESELECTION: home_rat=%d, EARFCN=%u, Cell_ID=%u",
           home_rat, resel->earfcn, resel->cell_id );
  }
  else
  {
    IRATMAN_INFO_MSG( "No nodes to reselect" );
  }

} /* lte_check_reselection */


/*===========================================================================

  FUNCTION:  lte_do_list_maintenance

===========================================================================*/
/*!
  @brief
    Helper function which checks if a node should be moved from one list to
    another, and if so moves it.

  @details
    The criteria to move is that the new value of node header's list[RAT_INT]
    value has changed. The parameter old_list is the value before calling
    lte_update_current_info.

  @note
    lte_update_current_info MUST be called prior so that the node header's
    list[RAT_INT] is valid.

  @return
    void

*/
/*=========================================================================*/
STATIC void lte_do_list_maintenance(iratman_rat_node_type* node,
                                    iratman_lte_list_e_type old_list)
{
  boolean ret_code = FALSE;

  if (node->hdr.list[RAT_INT] != old_list)
  {
    ret_code =
      list_move_node( node,
                      &iratman.lte.all_freq_lists[old_list],
                      &iratman.lte.all_freq_lists[node->hdr.list[RAT_INT]] );
    ASSERT( ret_code == TRUE );

    lte_sort_list_using_earfcn_priority(
       &iratman.lte.all_freq_lists[node->hdr.list[RAT_INT]] );
  }
}


/*===========================================================================

  FUNCTION:  lte_del_node

===========================================================================*/
/*!
  @brief
    Delete a LTE node related to "home_rat"

  @details
    This function will delete input node from IRAT Lists matching type
    "meas_rat" and known to "home_rat". If the node is added by more than one
    RAT, then node will not be deleted, instead the mask for the "home_rat"
    will be removed and "home_rat" specific info is cleared.

  @note
    iratman_init() should have been previously called.
    list_move_node() updates the "node->next" pointer.
    Does not reset the HW timers. Timers have to be taken care by the calling
    function.

  @return
    boolean: TRUE if "home_rat" node were successfully deleted

*/
/*=========================================================================*/
STATIC boolean lte_del_node
(
  iratman_rat_e_type          home_rat, //!< Home/Originating RAT
  iratman_rat_node_type*      node      //!< Node to be deleted
)
{
  boolean                 ret_value = FALSE;
  iratman_lte_list_e_type old_list;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_del_node()" );

  IRATMAN_NULL_PTR_CHECK( node, FALSE );

  /* Check if node is known to home_rat */
  if( node->hdr.rats_known & RAT_MASK( home_rat ) )
  {
    MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "home_rat=%d deleting LTE node: EARFCN=%d ",
           home_rat, node->data.lte.mc.earfcn );

    /* Push the log packet before the node data is reset */
    iratman_log_report_event( IRATMAN_LOG_DELETE_EVENT,
                              home_rat, RAT_LTE,
                              NULL,
                              node );

    /* Remove reference to home_rat */
    node->hdr.rats_known &= ~RAT_MASK( home_rat );

    /* Check whether the node is not in use any more */
    if( node->hdr.rats_known == 0 )
    {
      /* Node is not being used by any RAT, move to free list */
      ret_value =
        list_move_node( node,
                        &iratman.lte.all_freq_lists[node->hdr.list[RAT_INT]],
                        &iratman.lte.all_freq_lists[IRATMAN_LTE_FREE_LIST]
                      );
      ASSERT( ret_value == TRUE );

      /* Clear node data only. Don't clear the 'next' pointer */
      memset( &node->hdr , 0, sizeof( iratman_rat_node_hdr_type ) );
      memset( &node->data, 0, sizeof( iratman_rat_node_data ) );
    }
    else
    {
      /* Clear RAT specific info */
      lte_clear_rat_info( home_rat, node );

      /* Update the RAT_INT */
      old_list = (iratman_lte_list_e_type)node->hdr.list[RAT_INT];
      lte_update_current_info( node );

      /* If needed, moved node to the LP list */
      lte_do_list_maintenance( node, old_list );
    }

    ret_value = TRUE;
  }
  else
  {
    MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
           "home_rat=%d is trying to delete an un-owned LTE node:EARFCN=%d ",
           home_rat, node->data.lte.mc.earfcn );

    ret_value = FALSE;
  }

  return ret_value;

} /* lte_del_node */


/*===========================================================================

  FUNCTION:  lte_del_node_and_update_timer

===========================================================================*/
/*!
  @brief
    Delete a LTE node related to "home_rat" and update timers

  @details
    This function will delete input node from IRAT Lists matching type
    "meas_rat" and known to "home_rat". If the node is added by more than one
    RAT, then node will not be deleted, instead the mask for the "home_rat"
    will be removed and "home_rat" specific info is cleared. This function
    also updates the relevant timers.

  @note
    iratman_init() should have been previously called
    Does not reset the HW timers. Timers have to be taken care by the calling
    function.

  @return
    boolean: TRUE if "home_rat" node were successfully deleted

*/
/*=========================================================================*/
STATIC boolean lte_del_node_and_update_timer
(
  iratman_rat_e_type          home_rat, //!< Home/Originating RAT
  lte_earfcn_t                earfcn    //!< EARFCN of the node to be deleted
)
{
  boolean  ret_value = FALSE;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_del_node_and_update_timer()" );

  /* Clear all LTE timers.
     We will arm the timers once the nodes are updated */
  lte_clear_timers();

  /* Delete the LTE node.
     NOTE: The lte_del_node() does not update the timers.
           Hence, we need to handle it explicitly */
  ret_value = lte_del_node( home_rat, lte_find_node( earfcn ) );

  /* Set Treselect/Tbackoff timers, if required */
  lte_treselect_timer_update();
  lte_tbackoff_timer_update();

  return ret_value;

} /* lte_del_node_and_update_timer */


/*===========================================================================

  FUNCTION:  lte_trim_nodes_from_list

===========================================================================*/
/*!
  @brief
    Trim nodes of type "meas_rat" and known to "home_rat" from list

  @details
    This function runs through a list and deletes all inactive nodes or nodes
    which have have time_pending to expire > "pending_time". This function does
    not clear nodes which have Treselect expired or nodes which have Treselect
    expiring within the next "pending_time" duration. The function returns
    whether there are nodes known to "home_rat" that require measurement after
    deleting future nodes.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    uint8: number of nodes which require measurement.

*/
/*=========================================================================*/
STATIC uint8 lte_trim_nodes_from_list
(
  iratman_lte_list_e_type list,            //!< list to operate on
  iratman_rat_e_type      home_rat,        //!< Home/Originating RAT
  uint32                  pending_time,    //!< buffer time in ms
  timetick_type           curr_time_sclks  //!< Current time in sclks units
)
{

  iratman_rat_node_type*  node = NULL;     //!< Ptr to individual LTE freq
  uint8                   freq_count = 0;   //!< Number of freqs to measure

  node = iratman.lte.all_freq_lists[ list ].head_ptr;

  while( node != NULL )
  {
    /* Check if node is known to "home_rat" */
    if( node->hdr.rats_known & RAT_MASK( home_rat ) )
    {
      if( ( node->data.lte.mc.state == IRATMAN_LTE_INACTIVE ) ||
          ( node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_ACTIVE ) ||
          ( node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_BUFFER_ACTIVE ) ||
          ( node->data.lte.mc.state == IRATMAN_LTE_TBACKOFF_EXPIRED ) )
      {
        /* Delete the node */
        lte_del_node( home_rat, node );
      }
      else if( node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_EXPIRED )
      {
        /* Check if measurement is delayed */
        if( lte_check_meas_delayed( node ) == FALSE )
        {
          /* Increment the count */
          freq_count++;
        }
        else
        {
          /* Delete the node as meas results are anyway stale */
          lte_del_node( home_rat, node );
        }
      }
      else if( node->data.lte.mc.state == IRATMAN_LTE_TRESELECT_ACTIVE )
      {
        /* If Treselect expires after (curr_time + pending_time) */
        if( TIME_OCCURS_FIRST( ( curr_time_sclks + pending_time ),
                               node->data.lte.mc.treselect_exp_sclks ) )
        {
          lte_del_node( home_rat, node );
        }
        else
        {
          /* Increment the freq counter */
          freq_count++;
        }
      }
    } /* Node known to "home_rat" */

    /* Move to next node in the list */
    node = node->next;
  }

  return freq_count;
} /* lte_trim_nodes_from_list */


/*===========================================================================

  FUNCTION:  lte_trim_nodes

===========================================================================*/
/*!
  @brief
    Trim nodes of type "meas_rat" and known to "home_rat"

  @details
    This function runs through the More Preferred and Less Preferred LTE lists
    and deletes all inactive nodes or nodes which have have time_pending to
    expire > "pending_time". This function does not clear nodes which have
    Treselect expired or nodes which have Treselect expiring within the next
    "pending_time" duration. The function returns whether there are nodes
    known to "home_rat" that require measurement after deleting future nodes.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: TRUE if there are still nodes which require measurement.

*/
/*=========================================================================*/
STATIC boolean lte_trim_nodes
(
  iratman_rat_e_type    home_rat,     //!< Home/Originating RAT
  uint32                pending_time  //!< buffer time in ms
)
  {
  uint8                   freq_count = 0;   //!< Number of freqs to measure
  timetick_type           curr_time_sclks;  //!< Current time in sclks units

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_trim_nodes()" );

  /* Clear all timers. We will arm the timer once the nodes are updated */
  lte_clear_timers();

  /* Get current time in sclk units */
  curr_time_sclks = timetick_get();

  /* ---------------------- More preferred list --------------------- */
  freq_count = lte_trim_nodes_from_list(IRATMAN_LTE_MP_LIST,
                                        home_rat,
                                        pending_time,
                                        curr_time_sclks);

  /* ---------------------- Less preferred list --------------------- */
  freq_count += lte_trim_nodes_from_list(IRATMAN_LTE_LP_LIST,
                                         home_rat,
                                         pending_time,
                                         curr_time_sclks);

  /* Set Treselect/Tbackoff timers, if required */
  lte_treselect_timer_update();
  lte_tbackoff_timer_update();

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "lte_trim_nodes( home_rat=%d, meas_rat=LTE, pending_time=%ums ):"
         " freq_count=%d",
         home_rat, pending_time, freq_count );

  /* Return: Are there nodes ready to be measured ? */
  return ( freq_count > 0 );

} /* lte_trim_nodes */


/*===========================================================================

  FUNCTION:  lte_del_all_nodes

===========================================================================*/
/*!
  @brief
    Delete all LTE nodes related to "home_rat"

  @details
    This function will delete all nodes from IRAT Lists matching type
    "meas_rat" and known to "home_rat". If the node is added by more than one
    RAT, then node will not be deleted, instead the mask for the "home_rat"
    will be removed.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: TRUE if "home_rat" nodes were successfully deleted

*/
/*=========================================================================*/
STATIC boolean lte_del_all_nodes
(
  iratman_rat_e_type          home_rat  //!< Home/Originating RAT
)
{
  boolean                 ret_value = FALSE;//!< Return value
  iratman_rat_node_type*  curr_node = NULL; //!< Ptr to individual LTE freq
  iratman_rat_node_type*  next_node = NULL; //!< Ptr to next node in the list
  uint8                   count = 0;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "lte_del_all_nodes()" );

  /* Clear all timers. We will arm the timer once the nodes are updated */
  lte_clear_timers();

  /* ---------------------- More preferred list --------------------- */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].cnt > 0 )
  {
    count = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].cnt;
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_MP_LIST ].head_ptr;

    while( ( curr_node != NULL ) && ( count > 0 ) )
    {
      /* Remember the next node in the list.
         NEED : If we move the curr_node to different list,
                the next pointer is no longer valid. */
      next_node = curr_node->next;

      /* Check if node is know to "home_rat" */
      if( curr_node->hdr.rats_known & RAT_MASK( home_rat ) )
      {
        /* Delete the node */
        ret_value = lte_del_node( home_rat, curr_node );
        ASSERT( ret_value == TRUE );
      }

      /* Move to next freq in the list */
      curr_node = next_node;
      count--;
    }
  }

  /* ---------------------- Less preferred list --------------------- */
  if( iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].cnt > 0 )
  {
    count = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].cnt;
    curr_node = iratman.lte.all_freq_lists[ IRATMAN_LTE_LP_LIST ].head_ptr;

    while( ( curr_node != NULL ) && ( count > 0 ) )
    {
      /* Remember the next node in the list.
         NEED : If we move the curr_node to different list,
                the next pointer is no longer valid. */
      next_node = curr_node->next;

      /* Check if node is know to "home_rat" */
      if( curr_node->hdr.rats_known & RAT_MASK( home_rat ) )
      {
        /* Delete the node */
        ret_value = lte_del_node( home_rat, curr_node );
        ASSERT( ret_value == TRUE );
      }

      /* Move to next freq in the list */
      curr_node = next_node;
      count--;
    }
  }

  /* Set Treselect/Tbackoff timers, if required */
  lte_treselect_timer_update();
  lte_tbackoff_timer_update();

  return TRUE;

} /* lte_del_all_nodes */


/*===========================================================================

  FUNCTION:  init_lists

===========================================================================*/
/*!
  @brief
    Initialize all RAT lists

  @details
    This function initializes node lists related to all RATs which use
    IRATMan for measuring other RATs

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void init_lists( void )
{
  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "init_lists()" );

  /* Init lists related to LTE freq measurement */
  lte_init_lists();

} /* init_lists */


/*===========================================================================

  FUNCTION:  init_timers

===========================================================================*/
/*!
  @brief
    Define timers related to all measurements

  @details
    This function defines all the HW timers required for measurement &
    reselection by various RATs using IRATMan.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void init_timers( void )
{
  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "init_timers()" );

  /* Define timers required for LTE measurement */
  lte_def_timers();

} /* init_timers */


/*===========================================================================

  FUNCTION:  register_cbs

===========================================================================*/
/*!
  @brief
    Register callbacks

  @details
    This function registers the callback functions for home_rat.

  @note

  @return
    void

*/
/*=========================================================================*/
STATIC void register_cbs
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_cb_struct         cbs       //!< Structure of all IRATMan call backs
)
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "register_cbs()" );

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, RETURN_VOID );

  MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Register callback called by home_rat=%d", home_rat );

  if( cbs.wakeup_cb == NULL )
  {
    IRATMAN_ERROR_MSG( "Trying to reserve a NULL wakeup callback. ALLOWED" );
  }

  if( cbs.get_cell_info_cb == NULL )
  {
    IRATMAN_ERROR_MSG( "Trying to reserve a NULL get cell info callback."
                       " ALLOWED" );
  }

  /* Update call backs for home_rat */
  iratman.wakeup_cb[home_rat]        = cbs.wakeup_cb;
  iratman.get_cell_info_cb[home_rat] = cbs.get_cell_info_cb;

} /* register_cbs */


/*===========================================================================

  FUNCTION:  iratman_lte_get_cell_info_cb

===========================================================================*/
/*!
  @brief
    Get cell info call back for home rat.

  @details
    This function gets a clients get cell info call back function.

  @note
    None

  @return
    Get Cell Info function pointer.

*/
/*=========================================================================*/
get_cell_info_cb_type iratman_lte_get_cell_info_cb
(
   iratman_rat_e_type home_rat
)
{
  /*------------------------------------------------------------------*/

  return iratman.get_cell_info_cb[home_rat];

} /* iratman_lte_get_cell_info_cb */


/*===========================================================================
                      EXPORTED FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

  FUNCTION:  iratman_init

===========================================================================*/
/*!
  @brief
    Initialize IRATMan module

  @details
    This function initializes the IRAT List Manager (deprecated and should be
    removed once clients are changed)

  @note
    Initialization should be done first by all RATs using IRATMan

  @return
    void

*/
/*=========================================================================*/
void iratman_init( void )
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_init()" );

  if( iratman.initialized == TRUE )
  {
    /* Already done. No need to initialize again */
    return;
  }

  IRATMAN_INFO_MSG( "IRATMan initialization" );

  /* Initialize the IRATMan critical section global lock */
  IRATMAN_INIT_CRIT_SECT();

  /* Initialize measurement RAT lists */
  init_lists();

  /* Define required timers */
  init_timers();

  /* Remember initialization */
  iratman.initialized = TRUE;

  /* Initialize iratman logging module */
  iratman_log_init();

} /* iratman_init */


/*===========================================================================

  FUNCTION:  iratman_initialization

===========================================================================*/
/*!
  @brief
    Initialize IRATMan module

  @details
    This function initializes the IRAT List Manager

  @note
    Initialization should be done first by all RATs using IRATMan

  @return
    void

*/
/*=========================================================================*/
void iratman_initialization
(
   iratman_rat_e_type home_rat,                   //!< Home/Originating RAT
   iratman_cb_struct  cbs,                        //!< Client call backs
   boolean            is_dynamic_backoff_enabled  //!< True if dynamic backoff
                                                  //!< is enabled, otherwise false
)
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_initialization()" );

  /* Check for intitialization conflicts w.r.t Tbackoff optimization */
  if( (iratman.initialized == TRUE) &&
      (iratman.is_dynamic_backoff_enabled != is_dynamic_backoff_enabled) )
  {
    MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
           "Conflicting requests for Tbackoff optimization. "
           "home_rat=%u IRATMan stored=%u, Client Requested=%u",
           home_rat,
           iratman.is_dynamic_backoff_enabled,
           is_dynamic_backoff_enabled );
  }
  else
  {
    MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "Dynamic Tbackoff optimization: home_rat=%u enable=%u",
           home_rat,
           is_dynamic_backoff_enabled );
  }

  if( is_dynamic_backoff_enabled == TRUE )
  {
    if ( iratman.client_mask == 0 )
    {
      /* Initialize iratman LTE availability module */
      iratman_la_init(&iratman.lte.la);
    }

    /* Set home_rat bit */
    iratman.client_mask |= home_rat;
  }

  /* Register Callbacks */
  register_cbs( home_rat, cbs );

  /* Remember dynamic backoff mode */
  iratman.is_dynamic_backoff_enabled |= is_dynamic_backoff_enabled;

  if( iratman.initialized == TRUE )
  {
    /* Already done. No need to initialize again */
    return;
  }

  /* ---------------- Exectued only for the first init call ---------------- */

  IRATMAN_INFO_MSG( "IRATMan initialization" );

  /* Initialize the IRATMan critical section global lock */
  IRATMAN_INIT_CRIT_SECT();

  /* Initialize measurement RAT lists */
  init_lists();

  /* Define required timers */
  init_timers();

  /* Remember initialization */
  iratman.initialized = TRUE;

  /* Initialize iratman logging module */
  iratman_log_init();

} /* iratman_initialization */


/*===========================================================================

  FUNCTION: iratman_flush_persistent_data

===========================================================================*/
/*!
  @brief
    Flush IRATMan module data to file

  @details
    This function flushes the IRAT List Manager persistent data to a file

  @note
    Flushing should be done by all RATs using IRATMan before powering
    down.

  @return
    void

*/
/*=========================================================================*/
void iratman_flush_persistent_data
(
   iratman_rat_e_type home_rat //!< Home/Originating RAT
)
{
  IRATMAN_DEBUG_MSG( "iratman_flush_persistent_data()" );

  /* Unset home_rat bit */
  iratman.client_mask &= ~home_rat;

  if ( iratman.client_mask == 0 )
  {
    iratman_la_deinit(&iratman.lte.la);
  }
}

/*===========================================================================

  FUNCTION:  iratman_register_wakeup_cb

===========================================================================*/
/*!
  @brief
    Register wakeup callback

  @details
    This function registers a wakeup callback for home_rat. The callback is
    used to notify the RAT about a pending measurement if the "home_rat"
    does not request a measurement within a buffer time after a node is
    ready for measurement.

  @note
    Callback should return a boolean. A return value of TRUE indicates that
    the RAT is active and has accepted to do the measurement.

  @return
    void

*/
/*=========================================================================*/
void iratman_register_wakeup_cb
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat, //!< RAT to be measured
  wakeup_cb_type            cb        //!< RAT node to be added
)
{
  /*----------------------------------------------------------------------*/

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, RETURN_VOID );
  IRATMAN_RAT_VALID( meas_rat, RETURN_VOID );

  MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "iratman_register_wakeup_cb( home_rat=%d, meas_rat=%d, cb )",
         home_rat, meas_rat );

  if( cb == NULL )
  {
    IRATMAN_ERROR_MSG( "Trying to reserve a NULL wakeup callback. ALLOWED" );
  }

  /* Update wakeup call back for home_rat */
  iratman.wakeup_cb[home_rat] = cb;

} /* iratman_register_wakeup_cb */


/*===========================================================================

  FUNCTION:  iratman_add_meas_node

===========================================================================*/
/*!
  @brief
    Add measurement node

  @details
    This function adds a "node" to the IRAT List Manager's "list" of nodes
    of type "meas_rat"

  @note
    iratman_init() should have been previously called

  @return
    boolean: TRUE if RAT node was successfully added. Else, FALSE.

*/
/*=========================================================================*/
boolean iratman_add_meas_node
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat, //!< RAT to be measured
  iratman_input_node_type*  node,     //!< RAT node to be added
  iratman_rat_list_e_type   list      //!< Meas RAT list to be used
)
{
  boolean ret_value = FALSE; /* return value */

  /*----------------------------------------------------------------------*/

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, FALSE );
  IRATMAN_RAT_VALID( meas_rat, FALSE );
  IRATMAN_NULL_PTR_CHECK( node, FALSE );

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Add RAT node: home=%d, meas=%d, list=%u",
         home_rat, meas_rat, list.value );

  IRATMAN_ENTER_CRIT_SECT();

  switch( meas_rat )
  {
    case RAT_LTE:
    {
      /* list validation */
      IRATMAN_LTE_LIST_VALID( list.lte, FALSE );

      IRATMAN_LTE_ALGO_VALID( node->lte.meas_comp_algo, FALSE );

      ret_value = lte_add_meas_node( home_rat, &node->lte, list.lte );

      break;
    }
    default:
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d",
             meas_rat );

      ret_value = FALSE;

      break;
    }
  }

  if ( ret_value == TRUE )
  {
    iratman_log_report_event( IRATMAN_LOG_ADD_EVENT,
                              home_rat, meas_rat,
                              (void*)node,
                              lte_find_node( node->lte.earfcn ) );
  }

  IRATMAN_LEAVE_CRIT_SECT();

  return ret_value;

} /* iratman_add_meas_node */


/*===========================================================================

  FUNCTION:  iratman_meas_pending

===========================================================================*/
/*!
  @brief
    Request from "home_rat" to determine if a measurement is pending/required
    for "meas_rat"

  @details
    This function will scan through the known lists of nodes to determine if an
    IRAT measurement of RAT type "meas_rat" is needed.
    A node will be returned if the following conditions are met:
      - The node has been added by the "home_rat"
      - The node is of RAT "meas_rat"
      - At least one of the node's timers has expired or it's a new node
      - The time it takes to do a measurement on this "meas_rat" is less than
        the "available_time" value passed in
      - Other conditions based on the "req_data" and spec are satisfied

  @note
    iratman_init() should have been previously called

  @return
    uint32: Time required for max number of possible measurements within the
            "available_time" provided [in ms]

*/
/*=========================================================================*/
uint32 iratman_meas_pending
(
  iratman_rat_e_type        home_rat,       //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat,       //!< RAT to be measured
  uint32                    available_time, //!< Meas time available [in ms]
  iratman_meas_req_type*    req_data        //!< Data for meas request processing
)
{
  uint32  meas_dur = 0; /* Measurement duration */

  /*----------------------------------------------------------------------*/

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, FALSE );
  IRATMAN_RAT_VALID( meas_rat, FALSE );
  IRATMAN_NULL_PTR_CHECK( req_data, FALSE );

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Check if meas is pending: home=%d, meas=%d, dur=%ums",
         home_rat, meas_rat, available_time );

  IRATMAN_ENTER_CRIT_SECT();

  switch( meas_rat )
  {
    case RAT_LTE:
    {
      meas_dur = lte_meas_pending( home_rat, available_time, &req_data->lte );
      break;
    }
    default:
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d",
             meas_rat );

      meas_dur = 0;

      break;
    }
  }

  if ( meas_dur != 0 )
  {
    iratman_log_report_event( IRATMAN_LOG_MEAS_PEND_EVENT,
                              home_rat, meas_rat,
                              (void*)req_data,
                              NULL );
  }

  IRATMAN_LEAVE_CRIT_SECT();

  return meas_dur;

} /* iratman_meas_pending */


/*===========================================================================

  FUNCTION:  iratman_get_min_meas_dur_ms

===========================================================================*/
/*!
  @brief
    Request from RAT to get minimum duration required for a measurement (in ms)

  @details
    This function will return the minimum duration required to measure one
    frequency belonging to the "meas_rat"

  @note
    iratman_init() should have been previously called

  @return
    uint32: minimum duration required to complete one measurement in ms units

*/
/*=========================================================================*/
uint32 iratman_get_min_meas_dur_ms
(
  iratman_rat_e_type        home_rat,       //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat        //!< RAT to be measured
)
{
  uint32 ret_min_meas_dur_ms = 1000; /* return value */

  /*----------------------------------------------------------------------*/

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, ret_min_meas_dur_ms );
  IRATMAN_RAT_VALID( meas_rat, ret_min_meas_dur_ms );

  IRATMAN_ENTER_CRIT_SECT();

  switch( meas_rat )
  {
    case RAT_LTE:
    {
      ret_min_meas_dur_ms = lte_get_min_meas_dur_ms( home_rat );
      break;
    }
    default:
    {
      MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d. Returning default value: %u ms",
             meas_rat, ret_min_meas_dur_ms );

      break;
    }
  }

  IRATMAN_LEAVE_CRIT_SECT();

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Get min meas duration: home=%d, meas=%d, min_dur=%u ms",
         home_rat, meas_rat, ret_min_meas_dur_ms );

  return ret_min_meas_dur_ms;

} /* iratman_get_min_meas_dur_ms */


/*===========================================================================

  FUNCTION:  iratman_get_meas_list

===========================================================================*/
/*!
  @brief
    Request from HOME_RAT to retrieve MEAS_RAT node list ready for measurement

  @details
    This function will scan through the known lists of nodes to determine if an
    IRAT measurement of RAT type "meas_rat" is needed.
    A node will be added to the list if the following conditions are met:
      - The node has been added by the "home_rat"
      - The node is of RAT "meas_rat"
      - At least one of the node's timers has expired or it's a new node
      - Other conditions based on the "req_data" and spec are satisfied

  @note
    iratman_init() should have been previously called

  @return
    iratman_meas_node_list_type: List of MEAS_RAT nodes ready for measurement

*/
/*=========================================================================*/
iratman_meas_node_list_type iratman_get_meas_list
(
  iratman_rat_e_type        home_rat,       //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat,       //!< RAT to be measured
  iratman_meas_req_type*    req_data        //!< Data for meas request processing
)
{
  iratman_meas_node_list_type ret_meas_node_list; /* return value */

  /*----------------------------------------------------------------------*/

  memset( &ret_meas_node_list, 0, sizeof( ret_meas_node_list ) );

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, ret_meas_node_list );
  IRATMAN_RAT_VALID( meas_rat, ret_meas_node_list );
  IRATMAN_NULL_PTR_CHECK( req_data, ret_meas_node_list );

  IRATMAN_ENTER_CRIT_SECT();

  switch( meas_rat )
  {
    case RAT_LTE:
    {
      lte_get_meas_list( home_rat, &req_data->lte, &ret_meas_node_list.lte );
      break;
    }
    default:
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d",
             meas_rat );

      break;
    }
  }

  IRATMAN_LEAVE_CRIT_SECT();

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "List of nodes ready for measurement: home=%u, meas=%u, count=%u",
         home_rat, meas_rat, ret_meas_node_list.count );

  return ret_meas_node_list;

} /* iratman_get_meas_list */


/*===========================================================================

  FUNCTION:  iratman_get_meas

===========================================================================*/
/*!
  @brief
    Request from HOME_RAT to retrieve MEAS_RAT node to be measured

  @details
    This function will scan through the known lists of nodes to determine if an
    IRAT measurement of RAT type "meas_rat" is needed.
    A node will be returned if the following conditions are met:
      - The node has been added by the "home_rat"
      - The node is of RAT "meas_rat"
      - At least one of the node's timers has expired or it's a new node
      - The time it takes to do a measurement on this "meas_rat" is less than
        the "meas_dur" value passed in
      - Other conditions based on the "req_data" and spec are satisfied

  @note
    iratman_init() should have been previously called

    If a valid node is returned then that node will be locked for the home_rat
    and it's home_rat's responsibility to report measurement results thereby
    unlocking the node for further measurements.

  @return
    iratman_meas_node_type: RAT node to be measured

*/
/*=========================================================================*/
iratman_meas_node_type iratman_get_meas
(
  iratman_rat_e_type        home_rat,       //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat,       //!< RAT to be measured
  uint32                    available_time, //!< Meas time available [in ms]
  iratman_meas_req_type*    req_data        //!< Data for meas request processing
)
{
  iratman_meas_node_type ret_meas_node; /* return value */

  /*----------------------------------------------------------------------*/

  memset( &ret_meas_node, 0, sizeof( iratman_meas_node_type ) );

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, ret_meas_node );
  IRATMAN_RAT_VALID( meas_rat, ret_meas_node );
  IRATMAN_NULL_PTR_CHECK( req_data, ret_meas_node );

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Get a node for measurement: home=%d, meas=%d, dur=%ums",
         home_rat, meas_rat, available_time );

  IRATMAN_ENTER_CRIT_SECT();

  switch( meas_rat )
  {
    case RAT_LTE:
    {
      lte_get_meas( home_rat, available_time,
                    &req_data->lte, &ret_meas_node.lte );
      break;
    }
    default:
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d",
             meas_rat );

      break;
    }
  }

  if ( ret_meas_node.valid == TRUE )
  {
    iratman_log_report_event( IRATMAN_LOG_MEAS_REQ_EVENT,
                              home_rat, meas_rat,
                              (void*)req_data,
                              lte_find_node( ret_meas_node.lte.earfcn ) );
  }

  IRATMAN_LEAVE_CRIT_SECT();

  return ret_meas_node;

} /* iratman_get_meas */


/*===========================================================================

  FUNCTION:  iratman_report_meas

===========================================================================*/
/*!
  @brief
    "home_rat" reports the measurement results for the last IRAT measurement

  @details
    This function takes the measurement report from the "home_rat" and inserts
    the information into the IRAT Lists. The node is then compared against
    thresholds using comparison criteria. Timers/timeouts are then recalculated.
    A structure containing information related to reselection is returned if the
    new measurement meets the comparison criteria.

  @note
    iratman_init() should have been previously called.
    In case "home_rat" is unable to complete the meas or if meas fails,
    use iratman_abort_meas() to report the failure back to IRATMan.

  @return
    iratman_meas_rat_reselect_type:
      RAT reselection structure containing relevant information.
      The "home_rat" should initiate the reselection using this info.

*/
/*=========================================================================*/
iratman_meas_rat_reselect_type iratman_report_meas
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat, //!< RAT to be measured
  iratman_meas_node_type*   node,     //!< RAT node that was being measured
  iratman_meas_rpt_type*    report    //!< Measurement report
)
{
  iratman_meas_rat_reselect_type ret_resel; /* return value */
  iratman_lte_meas_result_type   meas_result =
    IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE;
  iratman_comp_algo_e_type  comp_algo = 
    IRATNMAN_ANY_RSRP_RSRQ;
  /*----------------------------------------------------------------------*/

  comp_algo = report->lte.comp_algo;
  
  memset( &ret_resel, 0, sizeof( iratman_meas_rat_reselect_type ) );

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, ret_resel );
  IRATMAN_RAT_VALID( meas_rat, ret_resel );
  IRATMAN_NULL_PTR_CHECK( node, ret_resel );
  IRATMAN_NULL_PTR_CHECK( report, ret_resel );

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Reporting meas results: home=%d, meas=%d, comp_algo=%d",
         home_rat,
         meas_rat,
         comp_algo );

  IRATMAN_ENTER_CRIT_SECT();

  iratman_log_report_event( IRATMAN_LOG_MEAS_RPT_EVENT,
                            home_rat, meas_rat,
                            (void*)report,
                            lte_find_node( node->lte.earfcn ) );

  switch( meas_rat )
  {
    case RAT_LTE:
    {
      /* Update node with measurement results */
      meas_result = lte_report_meas( home_rat,
                                     &node->lte,
                                     &report->lte,
                                     comp_algo );

      /* Check if any node passed reselection criteria */
      lte_check_reselection( home_rat,
                             &node->lte,
                             &ret_resel.lte,
                             comp_algo);
      break;
    }
    default:
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d",
             meas_rat );
      break;
    }
  }

  ret_resel.lte.passed_meas =
    ( meas_result == IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X ) ? TRUE : FALSE;

  IRATMAN_LEAVE_CRIT_SECT();

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Returning meas results: home=%d, meas_result=%d, passed_meas=%d",
         home_rat,
         meas_result,
         ret_resel.lte.passed_meas );

  return ( ret_resel );
} /* iratman_report_meas */

/*===========================================================================

  FUNCTION:  iratman_abort_meas

===========================================================================*/
/*!
  @brief
    Aborts an outstanding IRAT measurement

  @details
    This function informs IRATMan that measurements on a recently queried node
    has been aborted. Timers/timeouts will remain as is (which should be in an
    expired state). Then a check is done to see whether there is a node which
    has qualified for reselection.

  @note
    iratman_init() should have been previously called

  @return
    iratman_meas_rat_reselect_type:
      RAT reselection structure containing relevant information.
      The "home_rat" should initiate the reselection using this info.

*/
/*=========================================================================*/
iratman_meas_rat_reselect_type iratman_abort_meas
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat, //!< RAT to be measured
  iratman_meas_node_type*   node      //!< RAT node that was being measured
)
{
  iratman_meas_rat_reselect_type ret_resel; /* return value */

  /*----------------------------------------------------------------------*/

  memset( &ret_resel, 0, sizeof( iratman_meas_rat_reselect_type ) );

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, ret_resel );
  IRATMAN_RAT_VALID( meas_rat, ret_resel );
  IRATMAN_NULL_PTR_CHECK( node, ret_resel );

  MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Abort measurement for node: home=%d, meas=%d",
         home_rat, meas_rat );

  IRATMAN_ENTER_CRIT_SECT();

  switch( meas_rat )
  {
    case RAT_LTE:
    {
      /* Update node with measurement results */
      lte_report_meas( home_rat,
                       &node->lte,
                       NULL,
                       IRATNMAN_NONE_RSRP_RSRQ );

      /* Check if any node passed reselection criteria */
      lte_check_reselection( home_rat,
                             &node->lte,
                             &ret_resel.lte,
                             IRATNMAN_NONE_RSRP_RSRQ);
      break;
    }
    default:
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d",
             meas_rat );
      break;
    }
  }

  if ( ret_resel.valid == TRUE )
  {
    iratman_log_report_event( IRATMAN_LOG_ABORT_EVENT,
                              home_rat, meas_rat,
                              (void*)node,
                              lte_find_node( node->lte.earfcn ) );
  }

  IRATMAN_LEAVE_CRIT_SECT();

  return ret_resel;
} /* iratman_abort_meas */

/*===========================================================================

  FUNCTION:  iratman_del_node

===========================================================================*/
/*!
  @brief
    Deletes a previously added node

  @details
    This function will delete a specific node from IRAT Lists. If the node
    is added by more than one "home_rat"s, then the node will not be deleted,
    instead the mask for the "home_rat" will be removed and the "home_rat"
    specific is cleared from node data structure.

  @note
    iratman_init() should have been previously called

  @return
    boolean: TRUE if the node is successfully deleted

*/
/*=========================================================================*/
boolean iratman_del_node
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat, //!< RAT to be measured
  iratman_meas_node_type*   node      //!< RAT node that was being measured
)
{
  boolean ret_value = FALSE; /* return value */

  /*----------------------------------------------------------------------*/

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, FALSE );
  IRATMAN_RAT_VALID( meas_rat, FALSE );
  IRATMAN_NULL_PTR_CHECK( node, FALSE );

  MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Delete node: home=%d, meas=%d",
         home_rat, meas_rat );

  IRATMAN_ENTER_CRIT_SECT();

  switch( meas_rat )
  {
    case RAT_LTE:
    {
      ret_value = lte_del_node_and_update_timer( home_rat, node->lte.earfcn );

      break;
    }
    default:
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d",
             meas_rat );

      ret_value = FALSE;

      break;
    }
  }

  /* NOTE: logging should be done in the individual RAT's delete function */

  IRATMAN_LEAVE_CRIT_SECT();

  return ret_value;

} /* iratman_del_node */


/*===========================================================================

  FUNCTION:  iratman_trim_nodes

===========================================================================*/
/*!
  @brief
    Trim/delete future nodes of type "meas_rat" and known to "home_rat"

  @details
    The decision to remove or retain nodes is specific to "meas_rat".
    This function deletes future nodes of type "meas_rat" and known to
    "home_rat". The "buffer_time" is used as a minimum buffer allowed
    for nodes to try to reselect even after receiving an updated list.
    The function goes ahead and returns whether there are nodes known to
    "home_rat" that require measurement after deleting the future nodes.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: TRUE if there are still nodes which require measurement.

*/
/*=========================================================================*/
boolean iratman_trim_nodes
(
  iratman_rat_e_type    home_rat,     //!< Home/Originating RAT
  iratman_rat_e_type    meas_rat,     //!< RAT to have timers cleared
  uint32                buffer_time   //!< Buffer time for reselection [in ms]
)
{
  boolean ret_value = FALSE; /* return value */

  /*----------------------------------------------------------------------*/

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, FALSE );
  IRATMAN_RAT_VALID( meas_rat, FALSE );

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Deleting future nodes: home=%d, meas=%d, buffer_time=%ums",
         home_rat, meas_rat, buffer_time );

  IRATMAN_ENTER_CRIT_SECT();

  iratman_log_report_event( IRATMAN_LOG_TRIM_EVENT,
                            home_rat, meas_rat,
                            NULL,
                            NULL );

  switch( meas_rat )
  {
    case RAT_LTE:
    {
      ret_value = lte_trim_nodes( home_rat, buffer_time );
      break;
    }
    default:
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d",
             meas_rat );

      ret_value = FALSE;

      break;
    }
  }

  /* NOTE: logging should be done in the individual RAT's delete function */

  IRATMAN_LEAVE_CRIT_SECT();

  return ret_value;

} /* iratman_trim_nodes */


/*===========================================================================

  FUNCTION:  iratman_del_all_nodes

===========================================================================*/
/*!
  @brief
    Deletes all previously added nodes of type "meas_rat"

  @details
    This function will delete all nodes from IRAT Lists matching type
    "meas_rat" and known to "home_rat". If the node is added by more than one
    RAT, then node will not be deleted, instead the mask for the "home_rat"
    will be removed.

  @note
    iratman_init() should have been previously called

  @return
    boolean: TRUE if all nodes related to "home_rat" were deleted successfully

*/
/*=========================================================================*/
boolean iratman_del_all_nodes
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat  //!< RAT of which all nodes to be deleted
)
{
  boolean ret_value = FALSE; /* return value */

  /*----------------------------------------------------------------------*/

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, FALSE );
  IRATMAN_RAT_VALID( meas_rat, FALSE );

  MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Delete all nodes: home=%d, meas=%d",
         home_rat, meas_rat );

  IRATMAN_ENTER_CRIT_SECT();

  iratman_log_report_event( IRATMAN_LOG_DELETE_ALL_EVENT,
                            home_rat, meas_rat,
                            NULL,
                            NULL );

  switch( meas_rat )
  {
    case RAT_LTE:
    {
      ret_value = lte_del_all_nodes( home_rat );
      break;
    }
    default:
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d",
             meas_rat );

      ret_value = FALSE;

      break;
    }
  }

  /* NOTE: logging should be done in the individual RAT's delete function */

  IRATMAN_LEAVE_CRIT_SECT();

  return ret_value;

} /* iratman_del_all_nodes */

/*===========================================================================
  FUNCTION:  iratman_is_meas_feasible
===========================================================================*/
/*!
  @brief
    Checks for restrictions for IRAT MEAS from home_rat to meas_rat

  @details


  @note
    

  @return
    boolean: TRUE if it passes all restrictions for home_rat->meas_rat IRAT
*/
/*=========================================================================*/
boolean iratman_is_meas_feasible
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat  //!< RAT for which meas is to be done
)
{
  boolean ret_value = FALSE; /* return value */

  /*----------------------------------------------------------------------*/

  /* Validation */
  IRATMAN_RAT_VALID( home_rat, FALSE );
  IRATMAN_RAT_VALID( meas_rat, FALSE );

  MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "iratman_is_meas_reqd: home=%d, meas=%d",
         home_rat, meas_rat );

  IRATMAN_ENTER_CRIT_SECT();


  switch( meas_rat )
  {
    case RAT_LTE:
    {
      switch( home_rat )
      {
        case RAT_1X:
        {
          #ifndef TEST_FRAMEWORK
          /* Make sure DO is not in srch suspended state */
          ret_value = !(hdrl1_srch_is_in_suspend_state());
          #endif
          break;
        }
        case RAT_HDR:
        default:
          break;
      }
      break;
    }
    default:
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Unsupported meas RAT %d",
             meas_rat );

      ret_value = FALSE;

      break;
    }
  }

  IRATMAN_LEAVE_CRIT_SECT();

  return ret_value;
} /* iratman_is_meas_feasible */


