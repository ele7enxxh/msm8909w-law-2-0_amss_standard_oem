/*!
  @file   iratman_log.c

  @brief  IRAT List Manager Logging implementations.

  @details
  This file contains the code for logging of IRATMan.

*/

/*===========================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/iratman/src/iratman_log.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/14   pk      Changes for FR-17879 (Extended EARFCN and band range)
04/02/14   bph     Address KW issue
12/06/13   bph     FR3839 - dynamic tbackoff
06/04/13   bph     Update logger for signed EARFCN priority and separate API
                   vs. timer expiry Tbackoff updating.
03/25/13   bph     Store RAT's node (more or less) preferrence
03/13/13   bph     Fix a null pointer dereference
01/08/13   cjb     First working version.
11/07/12   sst     Initial logging version.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/* Common */
#include <comdef.h>    /* Definition of basic types and macros */
#include <customer.h>

/* IRATMan */
#include "iratman.h"
#include "iratman_i.h"
#include "iratman_log.h"

/* Others */
#include "err.h"
#include "log.h"
#include "lte_cphy_irat_meas_msg.h"
#include "msg.h"
#include "rex.h"
#include "timer.h"
#include "timetick.h"


/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------
      Defined Constants & Macros
-------------------------------------------------------------------------*/


/*===========================================================================
         List related generic typedef, struct, union, variable, ...
===========================================================================*/


/*-------------------------------------------------------------------------
      Variables
-------------------------------------------------------------------------*/


/*===========================================================================
                      INTERNAL FUNCTION PROTOTYPES
===========================================================================*/


/*===========================================================================
                      INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

  FUNCTION:  get_log_alloc

===========================================================================*/
/*!
  @brief
    Allocate memory for the log packet

  @details
    This function allocates memory for the log packet based on the "meas_rat"
    and the "event" being logged.

  @note
    The size of all subpackets must be multiples of 4_bytes for word alignment.
    This is checked in iratman_log_init()

  @return
    void*

*/
/*=========================================================================*/
static void* get_log_alloc
(
  iratman_log_event_enum     event,
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  iratman_rat_e_type         meas_rat, //!< RAT to be measured
  void*                      info_ptr, //!< Information specific to event
  iratman_rat_node_type*     node
)
{
  uint16    size        = 0;
  void*     log_pkt_ptr = NULL;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "get_log_alloc()" );

  /* ------------------ Calculate log buffer size ------------------- */

  /* COMMON: add log header size */
  size += sizeof( log_hdr_type );

  /* COMMON: add in the size of the info "subpacket" */
  size += sizeof( iratman_log_info_type );

  /* figure out if there's any more "subpackets" */
  switch ( event )
  {
    case IRATMAN_LOG_MEAS_PEND_EVENT:
    case IRATMAN_LOG_ABORT_EVENT:
    case IRATMAN_LOG_DELETE_ALL_EVENT:
    case IRATMAN_LOG_TRIM_EVENT:
      /* will only have iratman_log_info_type */
      break;

    case IRATMAN_LOG_ADD_EVENT:
      switch ( meas_rat )
      {
        case RAT_LTE:
          size += sizeof( iratman_log_lte_add_type );
          size += sizeof( iratman_log_lte_timers_type );
          break;
        default:
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, " unexpected meas_rat %d", meas_rat);
          break;
      }
      break;

    case IRATMAN_LOG_MEAS_REQ_EVENT:
      switch ( meas_rat )
      {
        case RAT_LTE:
          size += sizeof( iratman_log_lte_meas_req_type );
          size += sizeof( iratman_log_lte_timers_type );
          break;
        default:
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, " unexpected meas_rat %d", meas_rat);
          break;
      }
      break;

    case IRATMAN_LOG_MEAS_RPT_EVENT:
      switch ( meas_rat )
      {
        case RAT_LTE:
          if (info_ptr == NULL)
          {
            MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Null info_ptr");
            return NULL;
          }
          else
          {
            size += sizeof( iratman_log_lte_meas_rpt_type );

            /* Number of cells in the result packet */
            size += ( ( ( iratman_lte_meas_rpt_type* )info_ptr )->
                          lte_meas_cnf_params_ptr->num_cells ) *
                    sizeof( iratman_log_lte_meas_rpt_cell_type );

            size += sizeof( iratman_log_lte_timers_type );
          }
          break;
        default:
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, " unexpected meas_rat %d", meas_rat);
          break;
      }
      break;

    case IRATMAN_LOG_DELETE_EVENT:
      switch ( meas_rat )
      {
        case RAT_LTE:
          size += sizeof( iratman_log_lte_del_node_type );
          size += sizeof( iratman_log_lte_timers_type );
          break;
        default:
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, " unexpected meas_rat %d", meas_rat);
          break;
      }
      break;

    case IRATMAN_LOG_RESELECTION_EVENT:
      switch ( meas_rat )
      {
        case RAT_LTE:
          IRATMAN_ERR_FATAL_IF_NULL( node );
          size += ( node->data.lte.cc.reselect_cell.valid == TRUE ) ?
                  sizeof( iratman_log_lte_reselect_type ) : 0;
          size += sizeof( iratman_log_lte_timers_type );
          break;
        default:
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, " unexpected meas_rat %d", meas_rat);
          break;
      }
      break;

    case IRATMAN_LOG_T_RESEL_START_EVENT:
    case IRATMAN_LOG_T_RESEL_ABORT_EVENT:
    case IRATMAN_LOG_T_RESEL_EXP_EVENT:
    case IRATMAN_LOG_T_BACKOFF_START_EVENT:
    case IRATMAN_LOG_T_BACKOFF_ABORT_EVENT:
    case IRATMAN_LOG_T_BACKOFF_EXP_EVENT:
      switch ( meas_rat )
      {
        case RAT_LTE:
          size += sizeof( iratman_log_lte_timers_type );
          break;
        default:
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, " unexpected meas_rat %d", meas_rat);
          break;
      }
      break;

    default:
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, " unexpected event %d", event);
      break;

  } /* event */


  /* -------------------- Allocate a log buffer --------------------- */

  log_pkt_ptr = (void*) log_alloc( IRATMAN_INFO_C, size );

  if ( log_pkt_ptr == NULL )
  {
    MSG_2(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "Unable to allocate %u bytes log packet for log packet: 0x%X",
          size, IRATMAN_INFO_C);
  }

  /* ------------------- Return allocated buffer -------------------- */

  return log_pkt_ptr;

} /* get_log_alloc */


/*===========================================================================

  FUNCTION:  set_log_info

===========================================================================*/
/*!
  @brief
    Set common log information subpacket

  @details
    This function defines updates the log_info subpacket which is common
    for all logs. This subpacket consists of the "event" which in turn
    provides the subpackets present in the current log.

  @note
    None

  @return
    void*

*/
/*=========================================================================*/
static void* set_log_info
(
  void*                      log_pkt,  //!< Pointer to log pkt location
  iratman_log_event_enum     event,    //!< Logging event identifier
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  iratman_rat_e_type         meas_rat, //!< RAT to be measured
  iratman_rat_node_type*     node
)
{
  iratman_log_info_type*     buff;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "set_log_info()" );

  IRATMAN_NULL_PTR_CHECK( log_pkt, NULL );

  buff = ( iratman_log_info_type*) log_pkt;

  buff->version = IRATMAN_INFO_CURR_VER;
  buff->event   = (uint8)event;
  buff->s_rat   = (uint8)home_rat;
  buff->m_rat   = (uint8)meas_rat;

  /* Override source rat */
  if( ( node != NULL ) && ( home_rat == RAT_INT ) )
  {
    /* Check for RATs that are aware of this node */
    if( ( node->hdr.rats_known & RAT_MASK( RAT_1X  ) ) &&
        ( node->hdr.rats_known & RAT_MASK( RAT_HDR ) ) )
    {
      buff->s_rat = (uint8)RAT_LOG_1X_HDR;
    }
    else if( node->hdr.rats_known & RAT_MASK( RAT_1X ) )
    {
      buff->s_rat = (uint8)RAT_LOG_1X;
    }
    else if( node->hdr.rats_known & RAT_MASK( RAT_HDR ) )
    {
      buff->s_rat = (uint8)RAT_LOG_HDR;
    }
  }

  /* Pointer arithmetic: This will take us past the subpacket */
  buff++;

  return buff;

} /* set_log_info */


/*===========================================================================

  FUNCTION:  process_lte_add

===========================================================================*/
/*!
  @brief
    Log node add event for meas_rat=LTE

  @details
    This function updates the lte_add_subpacket with the information related
    to the new LTE node being added by the "home_rat" into IRATMan.

  @note
    None

  @return
    void* (Pointer to the next subpacket)

*/
/*=========================================================================*/
static void* process_lte_add
(
  void*                      log_pkt,  //!< Pointer to log pkt location
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  void*                      info_ptr, //!< Information specific to event
  iratman_rat_node_type*     node
)
{
  iratman_log_lte_add_type*    buff;
  iratman_lte_input_node_type* info;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "process_lte_add()" );

  IRATMAN_NULL_PTR_CHECK( log_pkt, NULL );
  IRATMAN_NULL_PTR_CHECK( info_ptr, log_pkt )

  buff = (iratman_log_lte_add_type*)    log_pkt;
  info = (iratman_lte_input_node_type*) info_ptr;

  buff->earfcn                  = (uint32)info->earfcn;
  buff->earfcn_priority         = (int8  )info->earfcn_priority;
  buff->bandwidth               = (uint8 )info->bandwidth;
  buff->thresh_serving          = (uint8 )info->thresh_serving;
  buff->meas_comp_algo          = (uint8 )info->meas_comp_algo;
  buff->rx_lev_min_eutra        = (uint8 )info->rx_lev_min_eutra;
  buff->rx_lev_min_eutra_offset = (uint8 )info->rx_lev_min_eutra_offset;
  buff->pe_max                  = (uint8 )info->pe_max;
  buff->thresh_x                = (uint8 )info->thresh_x;
  buff->rsrq_min_eutra          = (uint8 )info->rsrq_min_eutra;
  buff->rsrq_min_eutra_offset   = (uint8 )info->rsrq_min_eutra_offset;
  buff->rsrq_thresh_x           = (uint8 )info->rsrq_thresh_x;
  buff->max_treselect           = (uint8 )info->max_treselect;
  buff->max_tbackoff            = (uint8 )info->max_tbackoff;
  buff->min_tbackoff            = (uint8 )info->min_tbackoff;
  buff->rats_known              = (uint8 )node->hdr.rats_known;
  buff->list                    = (uint8 )node->hdr.list[home_rat];
  buff->list_actual             = (uint8 )node->hdr.list[RAT_INT];

  // CHETANB: Add "new" or "existing node" info
  //buff->is_new                  = (uint8 ) is_new;

  /* Pointer arithmetic: This will take us past the subpacket */
  buff++;

  return buff;

} /* process_lte_add */


/*===========================================================================

  FUNCTION:  process_lte_meas_req

===========================================================================*/
/*!
  @brief
    Log measurement request event for meas_rat=LTE

  @details
    This function updates the lte_meas_req_subpacket with the information
    related to the LTE node being returned back to "home_rat" for measurement.

  @note
    None

  @return
    void* (Pointer to the next subpacket)

*/
/*=========================================================================*/
static void* process_lte_meas_req
(
  void*                      log_pkt,  //!< Pointer to log pkt location
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  void*                      info_ptr, //!< Information specific to event
  iratman_rat_node_type*     node
)
{
  iratman_log_lte_meas_req_type* buff;
  iratman_lte_meas_req_type*     info;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "process_lte_meas_req()" );

  IRATMAN_NULL_PTR_CHECK( log_pkt, NULL );
  IRATMAN_NULL_PTR_CHECK( info_ptr, log_pkt )

  buff = (iratman_log_lte_meas_req_type*) log_pkt;
  info = (iratman_lte_meas_req_type*)     info_ptr;

  buff->home_rat_ecio       = (uint8 )info->home_rat_ecio;
  buff->meas_node.valid     = (uint8 )TRUE; // Log packet sent only for valid=1
  buff->meas_node.earfcn    = (uint32)node->data.lte.mc.earfcn;
  buff->meas_node.bandwidth = (uint8 )node->data.lte.mc.bandwidth[RAT_INT];

  /* Pointer arithmetic: This will take us past the subpacket */
  buff++;

  return buff;

} /* process_lte_meas_req */


/*===========================================================================

  FUNCTION:  process_lte_meas_rpt

===========================================================================*/
/*!
  @brief
    Log measurement report event for meas_rat=LTE

  @details
    This function updates the lte_meas_rpt_subpacket with the meas results
    reported by the "home_rat" post LTE measurements.

  @note
    None

  @return
    void* (Pointer to the next subpacket)

*/
/*=========================================================================*/
static void* process_lte_meas_rpt
(
  void*                      log_pkt,  //!< Pointer to log pkt location
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  void*                      info_ptr, //!< Information specific to event
  iratman_rat_node_type*     node
)
{
  uint8*                              ret_ptr;
  iratman_log_lte_meas_rpt_type*      buff;
  iratman_log_lte_meas_rpt_cell_type* cell_buff;

  iratman_lte_meas_rpt_type*          info;
  lte_cphy_irat_meas_meas_results_s*  cell_info;

  uint32 c=0, a=0;
  int16  max_rsrp, max_rsrq;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "process_lte_meas_rpt()" );

  IRATMAN_NULL_PTR_CHECK( log_pkt, NULL );
  IRATMAN_NULL_PTR_CHECK( info_ptr, log_pkt )

  /* Pointer arithmetic: Initialize the return pointer */
  ret_ptr = (uint8*) log_pkt;

  buff = (iratman_log_lte_meas_rpt_type*) log_pkt;
  info = (iratman_lte_meas_rpt_type*)     info_ptr;

  buff->earfcn           = (uint32)node->data.lte.mc.earfcn;
  buff->bandwidth        = (uint8 )node->data.lte.mc.bandwidth[RAT_INT];
  buff->meas_comp_algo   = (uint8 )node->data.lte.cc.meas_comp_algo[RAT_INT];
  buff->lte_max_tx_power = ( int16)info->lte_max_tx_power;
  buff->rsrp_thresh      = (uint8 )( node->data.lte.cc.thresh_x[home_rat] * 2 );
  buff->rsrq_thresh      = (uint8 )node->data.lte.cc.rsrq_thresh_x[home_rat];
  buff->resel_valid      = (uint8 )node->data.lte.cc.reselect_cell.valid;
  buff->num_cells        = (uint8 )info->lte_meas_cnf_params_ptr->num_cells;

  /* Pointer arithmetic: This will take us past "meas_rpt" subpacket */
  ret_ptr += sizeof( *buff );

  /* loop through the cells */
  for ( c=0; c<buff->num_cells; c++ )
  {
    cell_buff = (iratman_log_lte_meas_rpt_cell_type*) ret_ptr;
    cell_info = (lte_cphy_irat_meas_meas_results_s*)
                  &(info->lte_meas_cnf_params_ptr->cells[ c ]);

    cell_buff->cell_id = (uint16)cell_info->cell_id;

    /* Init max variables to antenna_0 */
    max_rsrp = (int16)cell_info->rsrp_ant[0];
    max_rsrq = (int16)cell_info->rsrq_ant[0];

    /* loop through the antennae */
    for ( a=0; a<IRATMAN_LOG_LTE_NUM_RX_ANTENNAS; a++ )
    {
      /* Update antenna specific values in the log packet */
      cell_buff->rsrp_ant[a] = (int16)cell_info->rsrp_ant[a];
      cell_buff->rsrq_ant[a] = (int16)cell_info->rsrq_ant[a];

      /* Calculate MAX RSRP/RSRQ values */
      max_rsrp = MAX( max_rsrp, (int16)cell_info->rsrp_ant[a] );
      max_rsrq = MAX( max_rsrq, (int16)cell_info->rsrq_ant[a] );
    } /* antennae */

    cell_buff->rsrp_used = max_rsrp;
    cell_buff->rsrq_used = max_rsrq;

    /* Pointer arithmetic: This will take us past "meas_rpt_cell" subpacket */
    ret_ptr += sizeof( *cell_buff );

  } /* cells */

  return ret_ptr;

} /* process_lte_meas_rpt */


/*===========================================================================

  FUNCTION:  process_lte_del

===========================================================================*/
/*!
  @brief
    Log node delete event for meas_rat=LTE

  @details
    This function updates the lte_del_subpacket with the information related
    to the LTE node being deleted by the "home_rat".

  @note
    None

  @return
    void* (Pointer to the next subpacket)

*/
/*=========================================================================*/
static void* process_lte_del
(
  void*                      log_pkt,  //!< Pointer to log pkt location
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  iratman_rat_node_type*     node
)
{
  iratman_log_lte_del_node_type* buff;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "process_lte_del()" );

  IRATMAN_NULL_PTR_CHECK( log_pkt, NULL );
  IRATMAN_NULL_PTR_CHECK( node, log_pkt )

  buff = (iratman_log_lte_del_node_type*) log_pkt;

  buff->earfcn     = (uint32)node->data.lte.mc.earfcn;
  buff->rats_known = (uint8 )node->hdr.rats_known;

  /* Pointer arithmetic: This will take us past the subpacket */
  buff++;

  return buff;

} /* process_lte_del */


/*===========================================================================

  FUNCTION:  process_lte_resel

===========================================================================*/
/*!
  @brief
    Log reselection event for meas_rat=LTE

  @details
    This function updates the lte_resel_subpacket with the information related
    to the LTE node that meets reselection criteria and being returned back to
    the "home_rat".

  @note
    LIMITATION: The previous subpacket should have information on whether this
                subpacket exists or not.
    Currently, RESEL subpacket is sent along with REPORT subpacket. The REPORT
    subpacket has the "resel_valid" field which gives this info to the parser.

  @return
    void* (Pointer to the next subpacket)

*/
/*=========================================================================*/
static void* process_lte_resel
(
  void*                      log_pkt,  //!< Pointer to log pkt location
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  iratman_rat_node_type*     node
)
{

  iratman_log_lte_reselect_type* buff;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "process_lte_resel()" );

  IRATMAN_NULL_PTR_CHECK( log_pkt, NULL );
  IRATMAN_NULL_PTR_CHECK( node, log_pkt )

  buff = (iratman_log_lte_reselect_type*) log_pkt;

  /* Log memory allocated for RESEL only if data is present */
  if( node->data.lte.cc.reselect_cell.valid == TRUE )
  {
    buff->earfcn      = (uint32)node->data.lte.cc.reselect_cell.earfcn;
    buff->cell_id     = (uint16)node->data.lte.cc.reselect_cell.cell_id;
    buff->s_eutra_val = (int16 )node->data.lte.cc.reselect_cell.s_eutra_val;

    /* Pointer arithmetic: This will take us past the subpacket */
    buff++;
  }
  else
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_FATAL, "ERROR: No valid data for RESELECTION log packet");
  }

  return buff;

} /* process_lte_resel */


/*===========================================================================

  FUNCTION:  process_lte_timers

===========================================================================*/
/*!
  @brief
    Log timers used during measurement for meas_rat=LTE

  @details
    This function updates the lte_timers_subpacket with the current values.
    The lte_timers_subpacket is sent along with event specific subpackets for
    multiple events so as to provide the timer state at the time of the event.

  @note
    None

  @return
    void* (Pointer to the next subpacket)

*/
/*=========================================================================*/
static void* process_lte_timers
(
  void*                      log_pkt,  //!< Pointer to log pkt location
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  iratman_rat_node_type*     node
)
{
  iratman_log_lte_timers_type*  buff;

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "process_lte_timers()" );

  IRATMAN_NULL_PTR_CHECK( log_pkt, NULL );
  IRATMAN_NULL_PTR_CHECK( node, log_pkt )

  buff = (iratman_log_lte_timers_type*) log_pkt;

  buff->current_time  = (uint32)timetick_get();
  buff->tbackoff      = (uint32)node->data.lte.mc.tbackoff_dur_ms[RAT_INT];
  buff->tbackoff_exp  = (uint32)node->data.lte.mc.tbackoff_exp_sclks;
  buff->treselect     = (uint32)node->data.lte.mc.treselect_dur_ms[RAT_INT];
  buff->treselect_exp = (uint32)node->data.lte.mc.treselect_exp_sclks;
  buff->earfcn        = (uint32)node->data.lte.mc.earfcn;
  buff->state         = (uint8 )node->data.lte.mc.state;

  /* Pointer arithmetic: This will take us past the subpacket */
  buff++;

  return buff;

} /* process_lte_timers */


/*===========================================================================

  FUNCTION:  set_log_event_specific_info

===========================================================================*/
/*!
  @brief
    Set event specific log information

  @details
    This function updates the event specific log information

  @note
    Passing NULL for the node parameter is a FATAL ERROR.

  @return
    void

*/
/*=========================================================================*/
static void set_log_event_specific_info
(
  void*                   log_pkt_curr, //!< event specific subpacket pt
  iratman_log_event_enum  event,        //!< Logging event identifier
  iratman_rat_e_type      home_rat,     //!< Home/Originating RAT
  void*                   info_ptr,     //!< Information specific to event
  iratman_rat_node_type*  node          //!< Measurement "node" of interest)
)
{
  IRATMAN_DEBUG_MSG( "set_log_event_specific_info" );

  IRATMAN_ERR_FATAL_IF_NULL( node );

  switch ( event )
  {
    case IRATMAN_LOG_ADD_EVENT:
      log_pkt_curr = process_lte_add( log_pkt_curr, home_rat, info_ptr, node );
      break;

    case IRATMAN_LOG_MEAS_REQ_EVENT:
      log_pkt_curr = process_lte_meas_req( log_pkt_curr, home_rat,
                                             info_ptr, node );
      break;

    case IRATMAN_LOG_MEAS_RPT_EVENT:
      log_pkt_curr = process_lte_meas_rpt( log_pkt_curr, home_rat,
                                             info_ptr, node );
      break;

    case IRATMAN_LOG_DELETE_EVENT:
      log_pkt_curr = process_lte_del( log_pkt_curr, home_rat, node );
      break;

    case IRATMAN_LOG_RESELECTION_EVENT:
      log_pkt_curr = process_lte_resel( log_pkt_curr, home_rat, node );
      break;

    case IRATMAN_LOG_T_RESEL_START_EVENT:
    case IRATMAN_LOG_T_RESEL_ABORT_EVENT:
    case IRATMAN_LOG_T_RESEL_EXP_EVENT:
    case IRATMAN_LOG_T_BACKOFF_START_EVENT:
    case IRATMAN_LOG_T_BACKOFF_ABORT_EVENT:
    case IRATMAN_LOG_T_BACKOFF_EXP_EVENT:
      /* only require process_lte_timers (below) */
      break;

    default:
      ERR_FATAL( "Unexpected event: 0x%X", event, 0, 0 );
      break;
  }

  /* Update the timer information subpacket */
  log_pkt_curr = process_lte_timers( log_pkt_curr, home_rat, node );
}


/*===========================================================================

  FUNCTION:  process_lte_log_event

===========================================================================*/
/*!
  @brief
    Process event based logging request for meas_rat=LTE

  @details
    This function takes care of the following:
      1) Allocating memory for the log packet depending on the event
      2) Calling event based handler functions to update relevant subpackets
      3) Commiting the updated log packet to CORE/DIAG

  @note
    None

  @return
    void

*/
/*=========================================================================*/
static void process_lte_log_event
(
  iratman_log_event_enum     event,
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  void*                      info_ptr, //!< Information specific to event
  iratman_rat_node_type*     node      //!< Measurement "node" of interest
)
{
  void*                      log_pkt_head;  //!< pointer to head of log packet
  void*                      log_pkt_curr;  //!< pointer to current subpacket

  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "process_lte_log_event()" );

  /* Allocate memory for log packet based on the event */
  log_pkt_head = get_log_alloc( event, home_rat, RAT_LTE, info_ptr, node );
  if ( log_pkt_head == NULL )
  {
    MSG_3(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "Unable to alloc log pkt for event=%u; home_rat=%u; meas_rat=%u",
          event, home_rat, RAT_LTE);
    return;
  }

  /* Skip the log header part */
  log_pkt_curr = (void*) ( (uint8*)log_pkt_head + sizeof( log_hdr_type ) );

  /* Update common information */
  log_pkt_curr = set_log_info( log_pkt_curr, event, home_rat, RAT_LTE, node );

  /* Update event specific information */
  switch ( event )
  {
    case IRATMAN_LOG_ADD_EVENT:
    case IRATMAN_LOG_MEAS_REQ_EVENT:
    case IRATMAN_LOG_MEAS_RPT_EVENT:
    case IRATMAN_LOG_DELETE_EVENT:
    case IRATMAN_LOG_RESELECTION_EVENT:
    case IRATMAN_LOG_T_RESEL_START_EVENT:
    case IRATMAN_LOG_T_RESEL_ABORT_EVENT:
    case IRATMAN_LOG_T_RESEL_EXP_EVENT:
    case IRATMAN_LOG_T_BACKOFF_START_EVENT:
    case IRATMAN_LOG_T_BACKOFF_ABORT_EVENT:
    case IRATMAN_LOG_T_BACKOFF_EXP_EVENT:
      set_log_event_specific_info(log_pkt_curr,
                                  event,
                                  home_rat,
                                  info_ptr,
                                  node);
      break;

    case IRATMAN_LOG_MEAS_PEND_EVENT:
    case IRATMAN_LOG_ABORT_EVENT:
    case IRATMAN_LOG_DELETE_ALL_EVENT:
    case IRATMAN_LOG_TRIM_EVENT:
      /* No event specific subpacket */
      break;

    default:
      ERR_FATAL( "Unexpected event: 0x%X", event, 0, 0 );
      break;

  } /* switch event */

  if ( log_pkt_head )
  {
    /* everything is in, tell diag we are done */
    log_commit( log_pkt_head );
  }

} /* process_lte_log_report_event */


/*===========================================================================
                      EXPORTED FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

  FUNCTION:  iratman_log_init

===========================================================================*/
/*!
  @brief
    Initialize IRATMan logging module

  @details
    This function initializes the IRAT List Manager logging module

  @note
    Initialization should be done first by all RATs using IRATMan

  @return
    void

*/
/*=========================================================================*/
void iratman_log_init( void )
{
  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_log_init(): Only X->LTE is currently supported" );

  /* Validate log structs for word boundary alignment */
  IRATMAN_ASSERT( sizeof( log_hdr_type )%4 == 0 );
  IRATMAN_ASSERT( sizeof( iratman_log_info_type )%4 == 0 );
  IRATMAN_ASSERT( sizeof( iratman_log_lte_add_type )%4 == 0 );
  IRATMAN_ASSERT( sizeof( iratman_log_lte_meas_req_type )%4 == 0 );
  IRATMAN_ASSERT( sizeof( iratman_log_lte_meas_rpt_type )%4 == 0 );
  IRATMAN_ASSERT( sizeof( iratman_log_lte_meas_rpt_cell_type )%4 == 0 );
  IRATMAN_ASSERT( sizeof( iratman_log_lte_reselect_type )%4 == 0 );
  IRATMAN_ASSERT( sizeof( iratman_log_lte_del_node_type )%4 == 0 );
  IRATMAN_ASSERT( sizeof( iratman_log_lte_timers_type )%4 == 0 );

} /* iratman_log_init */


/*===========================================================================

  FUNCTION:  iratman_log_report_event

===========================================================================*/
/*!
  @brief
    Generate IRATMan log packet for the input event

  @details
    This function handles event based logging within IRAT List Manager

  @note
    iratman_log_init() should have been previously called

  @return
    void

*/
/*=========================================================================*/
void iratman_log_report_event
(
  iratman_log_event_enum     event,
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  iratman_rat_e_type         meas_rat, //!< RAT to be measured
  void*                      info_ptr, //!< "event" specific data
  iratman_rat_node_type*     node      //!< Measurement "node" of interest
)
{
  /*------------------------------------------------------------------*/

  /* Make sure the log packet is enabled */
  if ( !log_status( IRATMAN_INFO_C ) )
  {
    /* Do nothing */
    return;
  }

  /* Call MEAS_RAT specific handler */
  switch ( meas_rat )
  {
    case RAT_LTE:
      process_lte_log_event( event, home_rat, info_ptr, node );
      break;

    default:
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Unexpected measured rat: %u", meas_rat);
      break;
  }

} /* iratman_log_report_event */

