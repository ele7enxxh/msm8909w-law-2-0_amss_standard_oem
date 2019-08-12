/*===========================================================================

                           P D C P  R R C  I F

                               C   F I L E


DESCRIPTION
  This file contains data declarations and function prototypes for wcdma
  that are shared between PDCP and RRC layers as part of the control path.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 - 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/pdcp/src/pdcprrcif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/27/12   ss      Check comp_eng_type before moving PDCP to UP state in
                   pdcp_reconfig_hdlr.
01/22/10   sch     reset rlc_dl_id/rlc_ul_id to invalid (0xff) during release
10/23/09   asn     Lint fixes
08/31/09   ss      SU level CMI modifications.
07/30/09   sch     Checking to see if pdcp_info marked as valid, actually 
                   contains valid compression engine info.
03/04/09   sa      AU level CMI modifications.
09/21/07   rsl     num_allowed_pkt_sizes can be null in rohc_info based on rel 6
                   ASN 1 update.  Removed this check in validate_params.
02/26/07   aw      Lint Error Fixes.
11/06/06   vk      Added Data on Apps support.
09/19/06   rsl     Fixed new API to tune WM levels based on RAB rates after
                   testing in PDCP transparent case.
09/01/06   rsl     Support for new IE that dictates the mode ROHC decompressor
                   should transition to.  Also, support for dynamic WM level
                   updates based on RAB DL rates.
08/31/06   rsl     Featurized PDCP
08/18/06   rsl     Code cleanup in setup_hdlr().
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "pdcprrcif.h"
#include "pdcpi.h"

#include "pdcprlcif.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "ps_rohc.h"
#include "ps_iphc.h"

#ifdef FEATURE_TDSCDMA
#ifndef FEATURE_SEGMENT_LOADING
//#include "tdsrlcdsapif.h"
#include "tdsl2external.h" 
#include "tdsrrcdataext.h"
#endif /* FEATURE_SEGMENT_LOADING  */
#endif /* FEATURE_TDSCDMA */

#include "dsm_queue.h"
#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_TDSCDMA)
#ifdef FEATURE_SEGMENT_LOADING
/*===========================================================================
                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
TDSCDMA and WCDMA interface tables
---------------------------------------------------------------------------*/
extern interface_t *ds_3gpp_ps_tds_tbl;

#endif /* FEATURE_SEGMENT_LOADING  */
/*===========================================================================
FUNCTION PDCP_RESET_PID_TABLES

DESCRIPTION This function resets pid entries to invalid values, the ones at
            powerup.

PARAMETERS
          pdcp_inst - the index of pdcp ctl block array.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void pdcp_reset_pid_tables
(
  uint8 pdcp_inst
)
{
  uint32 j, hash_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (j = 0; j < MAX_PID_ENTRIES; j++) 
  {
    /*-------------------------------------------------------------------------
    Reset both UL and DL pid table entries to NULL values.
    -------------------------------------------------------------------------*/
    pdcp_ctl_blk[pdcp_inst].dl_pid_map_table[j].pkt_type = INVALID_PKT_TYPE;
    pdcp_ctl_blk[pdcp_inst].ul_pid_map_table[j].pkt_type = INVALID_PKT_TYPE;
  }
  pdcp_ctl_blk[pdcp_inst].ul_next_free_pid = 0;
  pdcp_ctl_blk[pdcp_inst].dl_next_free_pid = 0;

  for (hash_index=0; hash_index<(uint32)INVALID_PKT_TYPE; hash_index++) 
  {
    pdcp_ctl_blk[pdcp_inst].ul_hash_table[hash_index] = MAX_PID_ENTRIES;
  }
} /* pdcp_reset_pid_table */


/*===========================================================================
FUNCTION PDCP_GET_NEW_PDCP_INST

DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
static uint8 pdcp_get_new_pdcp_inst
(
  void
)
{
  uint8 pdcp_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (pdcp_inst = 0; pdcp_inst < MAX_PDCP_INST; pdcp_inst++) 
  {
    if (pdcp_ctl_blk[pdcp_inst].pdcp_entry_valid == FALSE) 
    {
      return pdcp_inst;
    }
  }
  DATA_MSG0_ERROR("No free PDCP inst available");
  return MAX_PDCP_INST;
} /* pdcp_get_new_pdcp_inst */


/*===========================================================================
FUNCTION PDCP_GET_PDCP_INST_GIVEN_LC_ID

DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
uint8 pdcp_get_pdcp_inst_given_lc_id
(
  sys_modem_as_id_e_type subs_id,
  uint8          rlc_dl_id,
  uint8          rlc_ul_id
)
{
  uint8 pdcp_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (pdcp_inst = 0; pdcp_inst < MAX_PDCP_INST; pdcp_inst++) 
  {
    if (pdcp_ctl_blk[pdcp_inst].is_mcast_call == FALSE)
    {
        if ((pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_dl_id == (rlc_lc_id_type)rlc_dl_id) 
            && (pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_ul_id == (rlc_lc_id_type)rlc_ul_id)
#ifdef FEATURE_DUAL_WCDMA
	    &&(pdcp_ctl_blk[pdcp_inst].subs_id == subs_id)
#endif /* FEATURE_DUAL_WCDMA */
            ) 
        {
          return pdcp_inst;
        }
    }
    else
    {
      if ( pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_dl_id == rlc_dl_id
#ifdef FEATURE_DUAL_WCDMA
	   &&(pdcp_ctl_blk[pdcp_inst].subs_id == subs_id)
#endif /* FEATURE_DUAL_WCDMA */
          ) 
      {
        return pdcp_inst;
      }
    }
  }
  DATA_MSG0_ERROR("Given LC ID does not match pdcp ctl blk entries");
  return MAX_PDCP_INST;
} /* pdcp_get_pdcp_inst_given_lc_id */

/*===========================================================================
FUNCTION pdcp_RETURN_PID_FOR_PKT_TYPE

DESCRIPTION
  This function return the PID value corresponding to a given packet type.
  
PARAMETERS
  pdcp_inst: index into pdcp control block.
  pkt_type: ROHC CID / IPHC packet type to match.

DEPENDENCIES
  None

RETURN VALUE
  Returns the pid index.
  Returns -1 if pkt_type is not found in pid table.
  
SIDE EFFECTS
  None

===========================================================================*/
static int pdcp_return_pid_for_pkt_type
(
  const pid_map_table_entry_type pid_map_table[],
  pkt_e_type               pkt_type
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0; i < MAX_PID_ENTRIES; i++) 
  {
    /*-------------------------------------------------------------------------
    Return the index that matches the pkt type being passed in.
    -------------------------------------------------------------------------*/
    if (pid_map_table[i].pkt_type == pkt_type) 
    {
      return i;
    }
  }
  return -1;
} /* pdcp_return_pid_for_pkt_type */


/*===========================================================================
FUNCTION PDCP_ADD_ROHC_PKT_FMT_ENTRY

DESCRIPTION This functions adds one pid entry for the ROHC packet format.

PARAMETERS  pdcp_inst - index into the pdcp control block

DEPENDENCIES
  None

RETURN VALUE TRUE - if this entry was successfully added to the pid table.
             FALSE - if the pid index exceeded 31.
SIDE EFFECTS
  None

===========================================================================*/
static boolean pdcp_add_rohc_pkt_fmt_entry
(
  pid_map_table_entry_type pid_map_table[],
  uint8                    *next_free_pid
)
{
  boolean ret_val = TRUE;
  uint8 pid_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG0_HIGH("add_rohc_pkt_fmt_entry");
  /*-------------------------------------------------------------------------
  find first available free pid index
  -------------------------------------------------------------------------*/
  pid_index = *next_free_pid;

  /*-------------------------------------------------------------------------
  this means all entries are full
  -------------------------------------------------------------------------*/
  if (pid_index == MAX_PID_ENTRIES) 
  {
    DATA_MSG0_ERROR("pid table is full");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
  if entry is available, fill with packet entry information
  -------------------------------------------------------------------------*/
  pid_map_table[pid_index].pkt_type = ROHC_PKT_FORMAT;
  (*next_free_pid)++;
  return ret_val;
} /* pdcp_add_rohc_pkt_fmt_entry */


/*===========================================================================
FUNCTION PDCP_ADD_ROHC_ENTRIES

DESCRIPTION  This function calls appropriate utility functions that add 
             multiple entries to pid table one each for ROHC cid, 
             going from 0 - max_cid.

PARAMETERS   pdcp_inst - index into the pdcp control block.
             max_cid - determines how many pid entries to add.

DEPENDENCIES
  None

RETURN VALUE
              TRUE - if adding entries succeeded.
              FALSE - if pid index exceeded 31.
SIDE EFFECTS
  None

===========================================================================*/
static boolean pdcp_add_rohc_entries
(
  uint8          pdcp_inst,
  rohc_info_type rohc_info
)
{
  boolean ret_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    if cid_inc_info says CID is in ROHC pkt, add entry for rohc_pkt_fmt.
  -------------------------------------------------------------------------*/
    DATA_MSG0_HIGH("DL uses ROHC pkt format, just add to DL PID table");
    ret_val = pdcp_add_rohc_pkt_fmt_entry (pdcp_ctl_blk[pdcp_inst].dl_pid_map_table,
                                           &pdcp_ctl_blk[pdcp_inst].dl_next_free_pid);
  
    /*-------------------------------------------------------------------------
      if cid_inc_info says CID is in ROHC pkt, add entry for rohc_pkt_fmt.
    -------------------------------------------------------------------------*/
    DATA_MSG0_HIGH("UL uses ROHC pkt format, just add to UL PID table");
    ret_val = pdcp_add_rohc_pkt_fmt_entry (pdcp_ctl_blk[pdcp_inst].ul_pid_map_table,
                                           &pdcp_ctl_blk[pdcp_inst].ul_next_free_pid);
  return ret_val;
} /* pdcp_add_rohc_entries */


/*===========================================================================
FUNCTION PDCP_ADD_IPHC_TO_PID_TABLE

DESCRIPTION This function adds 5 entries one each for IPHC packet types,
            Full Header, Compressed TCP, Compressed TCP nondelta,
            Compressed non TCP & Context State.       

PARAMETERS  pdcp_inst - index into pdcp control block

DEPENDENCIES
  None

RETURN VALUE  TRUE - if adding was successful
              FALSE - if pid index exceeded 31.

SIDE EFFECTS
  None

===========================================================================*/
static boolean pdcp_add_iphc_to_pid_table
(
  pid_map_table_entry_type pid_map_table[],
  uint8                    *next_free_pid
)
{
  boolean ret_val = TRUE;
  int   temp_pkt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG0_HIGH("pdcp_add_iphc_to_pid_table");

  if (((*next_free_pid)+NUM_IPHC_ENTRIES) >= MAX_PID_ENTRIES) 
  {
    DATA_MSG0_ERROR("pid table is full");
    return FALSE;
  }

  temp_pkt_type = (int) IPHC_FULL_HEADER;
  pid_map_table[(*next_free_pid)++].pkt_type = (pkt_e_type)temp_pkt_type;

  temp_pkt_type = (int) IPHC_COMP_TCP;
  pid_map_table[(*next_free_pid)++].pkt_type = (pkt_e_type)temp_pkt_type;

  temp_pkt_type = (int) IPHC_COMP_TCP_NODELTA;
  pid_map_table[(*next_free_pid)++].pkt_type = (pkt_e_type)temp_pkt_type;  

  temp_pkt_type = (int) IPHC_COMP_NON_TCP;
  pid_map_table[(*next_free_pid)++].pkt_type = (pkt_e_type)temp_pkt_type;  

  temp_pkt_type = (int) IPHC_CONTEXT_STATE;
  pid_map_table[(*next_free_pid)++].pkt_type = (pkt_e_type)temp_pkt_type;

  return ret_val;
} /* pdcp_add_iphc_to_pid_table */


/*===========================================================================
FUNCTION PDCP_ADD_IPHC_ENTRIES

DESCRIPTION This function adds 5 entries one each for IPHC packet types,
            Full Header, Compressed TCP, Compressed TCP nondelta,
            Compressed non TCP & Context State.       

PARAMETERS  pdcp_inst - index into pdcp control block

DEPENDENCIES
  None

RETURN VALUE  TRUE - if adding was successful
              FALSE - if pid index exceeded 31.

SIDE EFFECTS
  None

===========================================================================*/
static boolean pdcp_add_iphc_entries
(
  uint8 pdcp_inst
)
{
  boolean ret_val = TRUE;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG0_HIGH("pdcp_add_iphc_entries");
  /*-------------------------------------------------------------------------
    Both pid tables are to be used, add iphc entries to both UL & DL 
    pid tables.
  -------------------------------------------------------------------------*/
  ret_val = pdcp_add_iphc_to_pid_table 
    ( pdcp_ctl_blk[pdcp_inst].ul_pid_map_table,
      &pdcp_ctl_blk[pdcp_inst].ul_next_free_pid );
  ret_val = pdcp_add_iphc_to_pid_table 
    ( pdcp_ctl_blk[pdcp_inst].dl_pid_map_table,
      &pdcp_ctl_blk[pdcp_inst].dl_next_free_pid );
  return ret_val;
} /* pdcp_add_iphc_entries */


/*===========================================================================
FUNCTION PDCP_GET_COMP_ENG_TYPE

DESCRIPTION  This function returns the compression engine configured in 
             the given index of comp_eng_info array.

PARAMETERS
  index - index into the comp_eng_info array

DEPENDENCIES
  None

RETURN VALUE
  IPHC or ROHC
  
SIDE EFFECTS
  None

===========================================================================*/
static pdcp_comp_eng_e_type pdcp_get_comp_eng_type
(
  pdcp_info_type    pdcp_info,
  uint32            index
)
{
  pdcp_comp_eng_e_type  ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
  Based on which comp eng flag is set, return corresponding comp eng type
  -------------------------------------------------------------------------*/
  if (pdcp_info.comp_eng_info[index].iphc_info.comp_eng_type == PDCP_IPHC) 
  {
    ret_val = PDCP_IPHC;
  }
  else if (pdcp_info.comp_eng_info[index].rohc_info.comp_eng_type == PDCP_ROHC) 
  {
    ret_val = PDCP_ROHC;
  }
  else
  {
    ret_val = PDCP_NO_COMP;
  }
  return ret_val;
} /* pdcp_get_comp_eng_type */


/*===========================================================================
FUNCTION PDCP_IS_ROHC_CFG

DESCRIPTION  This function returns TRUE if ROHC is one of the compression
             engines configured in pdcp_info.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if ROHC is configured
  FALSE - if ROHC is not configured
  
SIDE EFFECTS
  None

===========================================================================*/
static boolean pdcp_is_rohc_cfg
(
  pdcp_info_type pdcp_info
)
{
  uint32 index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (index=0; index<MAX_COMP_ENG_TYPES; index++) 
  {
    if (pdcp_get_comp_eng_type(pdcp_info, index) == PDCP_ROHC)
    {
      DATA_MSG0_HIGH("ROHC is cfg");
      return TRUE;
    }
  }
  return FALSE;
} /* pdcp_is_rohc_cfg */

/*===========================================================================
FUNCTION PDCP_IS_IPHC_CFG

DESCRIPTION  This function returns TRUE of IPHC is one of the compression
             engines configured in the global pdcp_info.

PARAMETERS
  None
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - if IPHC is one of the compression engines configured
  FALSE - if IPHC is not configured

SIDE EFFECTS
  None

===========================================================================*/
static boolean pdcp_is_iphc_cfg
(
  pdcp_info_type pdcp_info
)
{
  uint32 index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (index=0; index<MAX_COMP_ENG_TYPES; index++) 
  {
    if (pdcp_get_comp_eng_type(pdcp_info, index) == PDCP_IPHC)
    {
      DATA_MSG0_HIGH("IPHC is cfg");
      return TRUE;
    }
  }
  return FALSE;
} /* pdcp_is_iphc_cfg */

/*===========================================================================
FUNCTION PDCP_GET_INDEX_GIVEN_COMP_ENG

DESCRIPTION  This function returns the index of the comp eng info array
              given the comp eng type.

PARAMETERS
  IPHC / ROHC
  
DEPENDENCIES
  None

RETURN VALUE
  index into the table
  
SIDE EFFECTS
  None

===========================================================================*/
static uint32 pdcp_get_index_given_comp_eng
(
  pdcp_info_type    pdcp_info,
  pdcp_comp_eng_e_type   comp_eng
)
{
  uint32 index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (index = 0; index < MAX_COMP_ENG_TYPES; index ++) 
  {
    if (pdcp_get_comp_eng_type(pdcp_info, index) == comp_eng)
    {
      return index;
    }
  }
  return MAX_COMP_ENG_TYPES;
} /* pdcp_get_index_given_comp_eng */


/*===========================================================================
FUNCTION PDCP_ADD_COMP_ENG_ENTRIES

DESCRIPTION This function adds compression engine entries into the pid table
            in the right order. Whichever compression engine gets configured
            first, that will go first.

PARAMETERS
            pdcp_inst - index into the pdcp control block.
            max_cid - how many cids to add for ROHC.
DEPENDENCIES
  None

RETURN VALUE
              TRUE - if this succeeded.
              FALSE - if adding to pid table failed.
SIDE EFFECTS
  None

===========================================================================*/
static boolean pdcp_add_comp_eng_entries
(
  uint8 pdcp_inst
)
{
  uint32      index;
  pdcp_comp_eng_e_type comp_eng;
  boolean  ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (index = 0; index < MAX_COMP_ENG_TYPES; index ++) 
  {
    /*-------------------------------------------------------------------------
    Fill in iphc and rohc entries in the order the comp eng were cfg.
    -------------------------------------------------------------------------*/
    comp_eng = pdcp_get_comp_eng_type (pdcp_ctl_blk[pdcp_inst].pdcp_info,
                                       index);

    DATA_MSG1_HIGH("adding %d entries",comp_eng);

    switch (comp_eng) 
    {
      case PDCP_IPHC:
        ret_val = pdcp_add_iphc_entries ( pdcp_inst );
        break;
      case PDCP_ROHC:
        ret_val = pdcp_add_rohc_entries ( pdcp_inst,
                                          pdcp_ctl_blk[pdcp_inst].pdcp_info.comp_eng_info[index].rohc_info );
        break;
      case PDCP_NO_COMP:
        break;
    default:
        DATA_MSG0_ERROR("invalid comp eng type");
        break;
    }
    if (comp_eng == PDCP_NO_COMP) 
    {
      break;
    }
  }
  return ret_val;
} /* pdcp_add_comp_eng_entries */



/*===========================================================================
FUNCTION PDCP_CONSTRUCT_PID_TABLE

DESCRIPTION  This function constructs the UL and DL pid tables based on the
             pdcp_info received.

PARAMETERS
  pdcp_inst - pid table for which pdcp_inst.
  
DEPENDENCIES
  None

RETURN VALUE
  true - if pid table construction passed
  false - if pid table construction failed
  
SIDE EFFECTS
  None

===========================================================================*/
static boolean pdcp_construct_pid_table
(
  uint8         pdcp_inst
)
{
  boolean  ret_val = TRUE;
  uint8    hash_index = 0;
  int      pid_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG0_HIGH("pdcp_construct_pid_table");
  /*-------------------------------------------------------------------------
  Start off with clean pid tables.
  -------------------------------------------------------------------------*/
  pdcp_reset_pid_tables(pdcp_inst);

  /*-------------------------------------------------------------------------
  First entry is always reserved for NO_COMPRESSION
  -------------------------------------------------------------------------*/
  pdcp_ctl_blk[pdcp_inst].dl_pid_map_table[0].pkt_type = NO_HEADER_COMPRESSION;
  pdcp_ctl_blk[pdcp_inst].dl_next_free_pid ++;
  pdcp_ctl_blk[pdcp_inst].ul_pid_map_table[0].pkt_type = NO_HEADER_COMPRESSION;
  pdcp_ctl_blk[pdcp_inst].ul_next_free_pid ++;
  /*-------------------------------------------------------------------------
  Add one or more comp eng type entries based on what is cfg.
  -------------------------------------------------------------------------*/
  
  if ((pdcp_is_iphc_cfg(pdcp_ctl_blk[pdcp_inst].pdcp_info) == TRUE) ||
      (pdcp_is_rohc_cfg(pdcp_ctl_blk[pdcp_inst].pdcp_info) == TRUE))
  {
    ret_val = pdcp_add_comp_eng_entries (pdcp_inst);
  }
  
  /*lint -e641*/
  for (hash_index=0; hash_index<INVALID_PKT_TYPE; hash_index++) 
  {
    pid_val = pdcp_return_pid_for_pkt_type ( pdcp_ctl_blk[pdcp_inst].ul_pid_map_table,
                                             (pkt_e_type)hash_index );
    if (pid_val == -1)
    {
      pdcp_ctl_blk[pdcp_inst].ul_hash_table[hash_index] = MAX_PID_ENTRIES;
    }
    else
    {
      pdcp_ctl_blk[pdcp_inst].ul_hash_table[hash_index] = (uint8)pid_val;
    }
  }
  /*lint +e641*/
  /*-------------------------------------------------------------------------
  If adding any of the entries failed, reset pid table.
  -------------------------------------------------------------------------*/
  if (ret_val == FALSE) 
  {
    DATA_MSG0_ERROR("pid table construction failed, resetting pid tables");
    pdcp_reset_pid_tables(pdcp_inst);
  }
  return ret_val;
} /* pdcp_construct_pid_table */

/*===========================================================================
FUNCTION PDCP_SETUP_ROHC_COMP_ENG

DESCRIPTION
  This function sets up [arameters required as part of ROHC_alloc.
  Used by RB_Setup and RB_Reconfig handlers.
  
PARAMETERS
  pdcp_inst - index into pdcp ctl blk.
  pdcp_info - pdcp_info for this pdcp inst.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void pdcp_setup_rohc_comp_eng
(
  uint8           pdcp_inst,
  pdcp_info_type  pdcp_info
)
{
  rohc_info_type                    rohc_info;
  rohc_params_comp_channel_s_type   comp_channel_params;
  rohc_params_comp_flow_s_type      comp_flow_params;
  rohc_params_decomp_channel_s_type decomp_channel_params;
  rohc_params_decomp_flow_s_type    decomp_flow_params;
  errno_enum_type                   err_num;
  uint16                            index;
  uint8                             comp_idx = 
    MAX_COMP_ENG_TYPES;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  pdcp_ctl_blk[pdcp_inst].pdcp_info = pdcp_info;

  memset( &comp_channel_params, 0, sizeof(comp_channel_params));
  memset( &decomp_channel_params, 0, sizeof(decomp_channel_params));
  memset( &comp_flow_params, 0, sizeof(comp_flow_params));
  memset( &decomp_flow_params, 0, sizeof(decomp_flow_params));
  
  /*-------------------------------------------------------------------------
  create required comp eng instances
  -------------------------------------------------------------------------*/
  if (pdcp_is_rohc_cfg(pdcp_ctl_blk[pdcp_inst].pdcp_info) == TRUE) 
  {
    comp_idx = (uint8) pdcp_get_index_given_comp_eng(pdcp_ctl_blk[pdcp_inst]
                                             .pdcp_info, PDCP_ROHC);
    if (comp_idx >= MAX_COMP_ENG_TYPES)
    {
      DATA_MSG0_ERROR("Invalid comp_eng_info passed");      
      return;
    }

    rohc_info = pdcp_ctl_blk[pdcp_inst].pdcp_info.comp_eng_info
      [comp_idx].rohc_info;

    /*-------------------------------------------------------------------------
      Hard coded values in the rest of this function were suggested by
      Corp R&D ROHC simulation group.
    -------------------------------------------------------------------------*/
    if (rohc_info.rohc_ul_info_present == FALSE) 
    {
      rohc_info.rohc_ul_max_cid = ROHC_DEF_MAX_CID;
    }
    if (rohc_info.rohc_dl_info_present == FALSE) 
    {
      rohc_info.rohc_dl_max_cid = ROHC_DEF_MAX_CID;
      rohc_info.rohc_dl_rev_decomp_depth = ROHC_REV_DECOMP_DEPTH;
    }

    rohc_info.rohc_ul_cid_inc_info = ROHC_PACKET_FORMAT;
    rohc_info.rohc_dl_cid_inc_info = ROHC_PACKET_FORMAT;
    pdcp_ctl_blk[pdcp_inst].rohc_dl_cid_inc_info = ROHC_PACKET_FORMAT;
    pdcp_ctl_blk[pdcp_inst].rohc_ul_cid_inc_info = ROHC_PACKET_FORMAT;

    comp_channel_params.max_cid = rohc_info.rohc_ul_max_cid;
    comp_channel_params.use_large_cids = (comp_channel_params.max_cid > ROHC_DEF_MAX_CID) ?
      TRUE : FALSE;
    comp_channel_params.mrru = ROHC_MRRU;
    comp_channel_params.max_jitter_cd = ROHC_MAX_JITTER_CD;
    comp_channel_params.num_allowed_pkt_sizes = rohc_info.rohc_num_allowed_pkt_sizes;

    for (index = 0; index < comp_channel_params.num_allowed_pkt_sizes; index ++) 
    {
      comp_channel_params.allowed_pkt_sizes[index] = 
        rohc_info.rohc_ul_pkt_sizes_allowed[index];
    }

    for ( index = 0; index < (uint32)ROHC_PROFILE_MAX; index++ ) 
    {
      /*lint -e641*/
      comp_channel_params.profiles = (rohc_profiles_mask_e_type)( (int)comp_channel_params.profiles | 
                                                                  (1<<rohc_info.rohc_profiles[index]) ); 
      decomp_channel_params.profiles = (rohc_profiles_mask_e_type)( (int)decomp_channel_params.profiles | 
                                                                    (1<<rohc_info.rohc_profiles[index]) );
      /*lint +e641*/
    }
    decomp_channel_params.max_cid = rohc_info.rohc_dl_max_cid;
    decomp_channel_params.use_large_cids = (decomp_channel_params.max_cid > ROHC_DEF_MAX_CID) ?
      TRUE : FALSE;
    decomp_flow_params.rd_depth = rohc_info.rohc_dl_rev_decomp_depth;
    decomp_channel_params.mrru = ROHC_MRRU;

    decomp_channel_params.k1_out_n1 = ROHC_POLICY_1_OUT_1;
    decomp_channel_params.k2_out_n2 = ROHC_POLICY_1_OUT_1;

    comp_flow_params.ww_num_update        = ROHC_COMP_FLOW_WIN_WIDTH;
    comp_flow_params.num_update_IR        = ROHC_COMP_FLOW_NUM_UPDATE_IR;
    comp_flow_params.num_nonupdates       = ROHC_COMP_FLOW_NUM_NON_UPDATES;
    comp_flow_params.SO_IRt               = ROHC_COMP_FLOW_SO_IRT;
    comp_flow_params.SO_FOt               = ROHC_COMP_FLOW_SO_FOT;
    comp_flow_params.timer_based_comp     = ROHC_COMP_FLOW_TIMER_COMP;
    comp_flow_params.profile_hint         = (rohc_profiles_mask_e_type)ROHC_COMP_FLOW_PROF_HINT;
    comp_flow_params.f_max_time           = ROHC_COMP_FLOW_F_MAX_TIME;
    comp_flow_params.exp_backoff          = ROHC_COMP_FLOW_EXP_BACKOFF;

    if (pdcp_ctl_blk[pdcp_inst].is_mcast_call) 
    {
      decomp_flow_params.preferred_mode       = ROHC_U_MODE;
    }
    else
    {
      decomp_flow_params.preferred_mode       = ROHC_O_MODE;
    }
    decomp_flow_params.optional_ack_enabled = ROHC_DECOMP_FLOW_ACK_ENAB;
    decomp_flow_params.rd_depth             = ROHC_DECOMP_FLOW_RD_DEPTH;
    decomp_flow_params.timer_based_decomp   = ROHC_DECOMP_FLOW_TIMER_BASED_DECOMP;

    /*-------------------------------------------------------------------------
      Allocate a ROHC instance based on the values filled in so far.
    -------------------------------------------------------------------------*/
    pdcp_ctl_blk[pdcp_inst].rohc_hdl = rohc_alloc(&comp_channel_params,
                                                  &comp_flow_params,
                                                  &decomp_channel_params,
                                                  &decomp_flow_params,
                                                  &err_num);
  }
} /* pdcp_setup_rohc_comp_eng */
 
/*===========================================================================
FUNCTION PDCP_SETUP_IPHC_COMP_ENG

DESCRIPTION
  This function sets up [arameters required as part of IPHC_alloc.
  Used by RB_Setup and RB_Reconfig handlers.
  
PARAMETERS
  pdcp_inst - index into pdcp ctl blk.
  pdcp_info - pdcp_info for this pdcp inst.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void pdcp_setup_iphc_comp_eng
(
  uint8           pdcp_inst,
  pdcp_info_type  pdcp_info
)
{
  iphc_info_type                    iphc_info;
  iphc_config_options_s_type        ipv4_comp_decomp_opt;
  uint8                             comp_idx = 
    MAX_COMP_ENG_TYPES;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  pdcp_ctl_blk[pdcp_inst].pdcp_info = pdcp_info;

  if (pdcp_is_iphc_cfg(pdcp_ctl_blk[pdcp_inst].pdcp_info) == TRUE) 
  {
    comp_idx = (uint8) pdcp_get_index_given_comp_eng(pdcp_ctl_blk[pdcp_inst]
                                             .pdcp_info, PDCP_IPHC);
    if (comp_idx >= MAX_COMP_ENG_TYPES)
    {
      DATA_MSG0_ERROR("Invalid comp_eng_info passed");      
      return;
    }
    
    iphc_info = pdcp_ctl_blk[pdcp_inst].pdcp_info.comp_eng_info
      [comp_idx].iphc_info;
    ipv4_comp_decomp_opt.f_max_period = iphc_info.iphc_f_max_period;
    ipv4_comp_decomp_opt.f_max_time = iphc_info.iphc_f_max_time;
    ipv4_comp_decomp_opt.max_header = iphc_info.iphc_max_header;
    ipv4_comp_decomp_opt.non_tcp_space = iphc_info.iphc_non_tcp_space;
    ipv4_comp_decomp_opt.tcp_space = iphc_info.iphc_tcp_space;
    ipv4_comp_decomp_opt.rtp_enabled = FALSE;

    /*-------------------------------------------------------------------------
      Allocate a IPHC instance based on the values filled in so far.
    -------------------------------------------------------------------------*/
    pdcp_ctl_blk[pdcp_inst].iphc_hdl = iphc_alloc(&ipv4_comp_decomp_opt,
                                                  &ipv4_comp_decomp_opt,
                                                  NULL,
                                                  NULL);
  }
} /* pdcp_setup_iphc_comp_eng */

/*===========================================================================
FUNCTION pdcp_comp_eng_info_valid

DESCRIPTION
  This function is used to check if the pdcp_info received with a valid 
  comp_eng_info[] i.e. atleast one of ROHC or IPHC compression engine
  parameters should be present.

PARAMETERS
  pdcp_info - the parameters to be validated

DEPENDENCIES
  None

RETURN VALUE
              TRUE - if comp_eng_info[] is valid
              FALSE - if comp_eng_info[] is invalid.
SIDE EFFECTS
  
===========================================================================*/
static boolean pdcp_comp_eng_info_valid
(
  pdcp_info_type pdcp_info
)
{
  uint32 index;
  pdcp_comp_eng_e_type comp_eng_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (index=0; index<MAX_COMP_ENG_TYPES; index++) 
  {
    comp_eng_type = pdcp_get_comp_eng_type(pdcp_info, index); 
    if ((comp_eng_type == PDCP_ROHC) ||
        (comp_eng_type == PDCP_IPHC))
    {
      DATA_MSG0_HIGH("comp_eng_info[] recd. in pdcp_info is valid");
      return TRUE;
    }
  }
  DATA_MSG0_HIGH("comp_eng_info[] recd. in pdcp_info is NOT valid");
  return FALSE;
} /* pdcp_comp_eng_info_valid */

/*===========================================================================
FUNCTION PDCP_SETUP_HDLR

DESCRIPTION
  This function gets invoked when RRC forwards a RB_Setup request to PDCP.
  
PARAMETERS
  cfg_req

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void pdcp_setup_hdlr
(
  const pdcp_cfg_req_data_type *cfg_req
)
{
  uint8                             pdcp_inst;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  pdcp_inst = pdcp_get_new_pdcp_inst();
  DATA_MSG1_HIGH("pdcp_setup_hdlr for pdcp_inst = %d", pdcp_inst);

  /*-------------------------------------------------------------------------
  Cannot use validate_pdcp_inst here, since rb_setup will be received before
  pdcp_ctl_blk table is populated for the first time.
  -------------------------------------------------------------------------*/
  if (pdcp_inst >= MAX_PDCP_INST) 
  {
    DATA_MSG1_ERROR("invalid pdcp_inst = %d", pdcp_inst);
    return;
  }

  if (cfg_req == NULL) 
  {
    DATA_MSG0_ERROR("cfg_req is NULL");
    return;
  }

  /*Validate subscription id*/
  if(!ds3gsubsmgr_is_subs_id_valid(cfg_req->subs_id))
  {
    DATA_MSG1_ERROR("Invalid Subs id(CM):%d in pdcp_setup_hdlr",
                     cfg_req->subs_id);
    return ;
  }

  /*-------------------------------------------------------------------------
    Store the advertized DL data rate in the PDCP control block
  -------------------------------------------------------------------------*/
  pdcp_ctl_blk[pdcp_inst].subs_id = cfg_req->subs_id;
  pdcp_ctl_blk[pdcp_inst].dl_data_rate = cfg_req->rab_dl_rate;

  pdcp_ctl_blk[pdcp_inst].is_mcast_call = cfg_req->is_mcast_call;
  if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_DOWN) 
  {
    /*-------------------------------------------------------------------------
    Set this ctl blk entry valid.
    -------------------------------------------------------------------------*/
    pdcp_ctl_blk[pdcp_inst].pdcp_entry_valid = TRUE;

    /*-------------------------------------------------------------------------
    Reserve a PS to PDCP Tx signal.
    -------------------------------------------------------------------------*/
    pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig = comp_reserve_to_comp_tx_sig();
    
    pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_dl_id = cfg_req->rlc_dl_id;
    pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_ul_id = cfg_req->rlc_ul_id;
    
    /*-------------------------------------------------------------------------
    if pdcp_info is included in rb_setup request & contains valid compresssion
    engine info, then PDCP goes to UP state.  
    -------------------------------------------------------------------------*/
    if ((cfg_req->pdcp_info.pdcp_info_valid == TRUE) &&
        (pdcp_comp_eng_info_valid(cfg_req->pdcp_info)))
    {
      pdcp_ctl_blk[pdcp_inst].pdcp_state = PDCP_UP;
      DATA_MSG0_HIGH("pdcp DOWN -> UP state");
      
      pdcp_setup_rohc_comp_eng (pdcp_inst, cfg_req->pdcp_info);
      pdcp_setup_iphc_comp_eng (pdcp_inst, cfg_req->pdcp_info);
      
      /*-------------------------------------------------------------------------
      Construct the pid table for this pdcp instance.
      -------------------------------------------------------------------------*/
      if (pdcp_ctl_blk[pdcp_inst].pdcp_info.pdcp_pdu_hdr == PRESENT) 
      {
        if (pdcp_construct_pid_table(pdcp_inst) != TRUE)
        {
          /*-------------------------------------------------------------------------
            This should never happen since validate_params should have detected any
            bad parameter values passed in pdcp_info.
          -------------------------------------------------------------------------*/
          DATA_MSG0_ERROR("pid table construction failed");
        }
      }
      else
      {
        pdcp_reset_pid_tables(pdcp_inst);
        DATA_MSG0_HIGH("pdcp header absent, no pid table required");
      }
    }
    /*-------------------------------------------------------------------------
    if pdcp_info is not included in rb_setup_req PDCP goes to TRANSPARENT state
    -------------------------------------------------------------------------*/
    else
    {
      pdcp_ctl_blk[pdcp_inst].pdcp_state = PDCP_TRANSPARENT;
      DATA_MSG0_HIGH("pdcp DOWN -> TRANSPARENT state");
      return;
    }
  }
  else
  {
    DATA_MSG0_ERROR("PDCP not in down state when rb_setup is received");
    return;
  }
}/*pdcp_setup_hdlr*/

/*===========================================================================
FUNCTION PDCP_RELEASE_HDLR

DESCRIPTION
  This function is invoked when RRC forwards a RB_Release request to PDCP.
  
PARAMETERS
  cfg_req

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static void pdcp_release_hdlr
(
  const pdcp_cfg_req_data_type *cfg_req
)
{
  uint8                   pdcp_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cfg_req == NULL) 
  {
    DATA_MSG0_ERROR("cfg_req is NULL");
    return;
  }

  /*Validate subscription id*/
  if(!ds3gsubsmgr_is_subs_id_valid(cfg_req->subs_id))
  {
    DATA_MSG1_ERROR("Invalid Subs id(CM):%d in pdcp_release_hdlr",
                     cfg_req->subs_id);
    return ;
  }

  pdcp_inst = pdcp_get_pdcp_inst_given_lc_id (cfg_req->subs_id,
                                             (uint8)(cfg_req->rlc_dl_id),
                                              (uint8)(cfg_req->rlc_ul_id));
  /*-------------------------------------------------------------------------
  check if this instance has a valid entry in the pdcp_ctl_blk table.
  -------------------------------------------------------------------------*/

  if (pdcp_inst >= MAX_PDCP_INST) 
  {
    DATA_MSG0_ERROR("pdcp inst validation failed");
    return;
  }


  if (pdcp_validate_pdcp_inst(pdcp_inst) == FALSE) 
  {
    DATA_MSG0_ERROR("pdcp inst validation failed");
    return;
  }

  pdcp_ctl_blk[pdcp_inst].pdcp_entry_valid = FALSE;
  pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_dl_id = PDCP_INVALID_RLC_LC_ID;
  pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_ul_id = PDCP_INVALID_RLC_LC_ID;
  pdcp_ctl_blk[pdcp_inst].next_rx_sn = 0;
  pdcp_ctl_blk[pdcp_inst].pdcp_state = PDCP_DOWN;
  pdcp_ctl_blk[pdcp_inst].pdcp_to_rlc_flow_mask = DS_FLOW_DISABLE;
  pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_flow_mask = DS_FLOW_DISABLE;
  
  /*-------------------------------------------------------------------------
  Unreserve the PS to PDCP Tx signal.
  -------------------------------------------------------------------------*/
  comp_unreserve_to_comp_tx_sig(pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig);
  pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig = COMP_MAX_SIGNALS;

  /*-------------------------------------------------------------------------
  Reset UL and DL pid tables for all PDCP inst.
  -------------------------------------------------------------------------*/
  pdcp_reset_pid_tables (pdcp_inst);

  if (pdcp_ctl_blk[pdcp_inst].rohc_hdl != NULL) 
  {
    rohc_free(pdcp_ctl_blk[pdcp_inst].rohc_hdl);
    pdcp_ctl_blk[pdcp_inst].rohc_hdl = NULL;
    pdcp_ctl_blk[pdcp_inst].rohc_dl_cid_inc_info = ROHC_INVALID_CID_INC;
    pdcp_ctl_blk[pdcp_inst].rohc_ul_cid_inc_info = ROHC_INVALID_CID_INC;
  }

  if (pdcp_ctl_blk[pdcp_inst].iphc_hdl != NULL)
  {
    iphc_free(pdcp_ctl_blk[pdcp_inst].iphc_hdl);
    pdcp_ctl_blk[pdcp_inst].iphc_hdl = NULL;
  }

  /*-------------------------------------------------------------------------
  Cleanup PDCP's tx and rx wm with RLC.  De-register is not required since
  RLC channels are released before PDCP is released.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
       if( (ds_3gpp_ps_tds_tbl != NULL) && 
       (ITDSCDMA_rrc_is_tds_active_sub(ds_3gpp_ps_tds_tbl, cfg_req->subs_id) == TRUE) )
#else
  if( (ds_3gpp_ps_tds_tbl != NULL) && 
       (ITDSCDMA_rrc_is_tds_active(ds_3gpp_ps_tds_tbl) == TRUE) )
#endif
  
#else
#ifdef FEATURE_DUAL_WCDMA
     if(rrc_is_tds_active_sub(cfg_req->subs_id) == TRUE)
#else
  if(rrc_is_tds_active() == TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
  {
    pdcp_cleanup_tdrlc_watermarks (pdcp_inst);
  }
  else
#endif /* FEATURE_TDSCDMA */
  {
    pdcp_cleanup_rlc_watermarks (pdcp_inst);
  }

  /*-------------------------------------------------------------------------
   Reset the subsid after the pdcp send cmd to clean up rlc watermarks
  -------------------------------------------------------------------------*/
  pdcp_ctl_blk[pdcp_inst].subs_id = SYS_MODEM_AS_ID_NONE;

}/*pdcp_release_hdlr*/

/*===========================================================================
FUNCTION PDCP_RELEASE_ALL_HDLR

DESCRIPTION
  This function is invoked when RRC forwards a Release_all request to PDCP.
  
PARAMETERS
  cfg_req

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static void pdcp_release_all_hdlr
(
  pdcp_cfg_req_data_type *cfg_req
)
/*lint -e715*/
{
  uint8                   pdcp_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG0_HIGH("pdcp_release_all_hdlr");
  for (pdcp_inst = 0; pdcp_inst<MAX_PDCP_INST; pdcp_inst++) 
  {
    cfg_req->rlc_ul_id = pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_ul_id;
    cfg_req->rlc_dl_id = pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_dl_id;
    
    if (pdcp_validate_pdcp_inst(pdcp_inst) == TRUE) 
    {
      pdcp_release_hdlr (cfg_req);
    }
  }
}/*pdcp_release_all_hdlr*/
/*lint +e715*/


/*===========================================================================
FUNCTION PDCP_RESET_COMP_ENG_HDLR

DESCRIPTION
  This function is invoked when RRC forwards a reset_comp_eng request to PDCP
  because of a RNC HO.
  
PARAMETERS
  cfg_req

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static void pdcp_reset_comp_eng_hdlr
(
  const pdcp_cfg_req_data_type *cfg_req
)
{
  uint8                             pdcp_inst;
  rohc_ioctl_e_type                 rohc_ioctl_type = 
    ROHC_IOCTL_CONTEXT_REINITIALIZATION;
  errno_enum_type                   rohc_ioctl_errno;

  iphc_ioctl_e_type                 iphc_comp_ioctl_type =
    IPHC_IOCTL_COMPRESSOR_V4_INVALIDATE_CONTEXTS;
  iphc_ioctl_e_type                 iphc_decomp_ioctl_type =
    IPHC_IOCTL_DECOMPRESSOR_V4_INVALIDATE_CONTEXTS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cfg_req == NULL) 
  {
    DATA_MSG0_ERROR("cfg_req is NULL");
    return;
  }

  pdcp_inst = pdcp_get_pdcp_inst_given_lc_id (cfg_req->subs_id,
                                             (uint8)(cfg_req->rlc_dl_id),
                                              (uint8)(cfg_req->rlc_ul_id));

  if (pdcp_inst >= MAX_PDCP_INST) 
  {
    DATA_MSG1_ERROR("invalid pdcp_inst = %d", pdcp_inst);
    return;
  }
  DATA_MSG0_HIGH("pdcp_reset_comp_eng_hdlr");
  if (cfg_req->srns_reloc_flag == PDCP_RESET_COMP_ENG) 
  {
    if (pdcp_ctl_blk[pdcp_inst].rohc_hdl != NULL) 
    {
      /*-------------------------------------------------------------------------
        Use the ROHC IOCTL API to update the comp/decomp parameters.
      -------------------------------------------------------------------------*/
      if (rohc_ioctl( pdcp_ctl_blk[pdcp_inst].rohc_hdl,
                  rohc_ioctl_type,
                  NULL,
                  &rohc_ioctl_errno ) == -1)
      {
        DATA_MSG0_ERROR("rohc_ioctl failed, returned -1");
      }
    }
    
    if (pdcp_ctl_blk[pdcp_inst].iphc_hdl != NULL) 
    {
      /*-------------------------------------------------------------------------
        Use the IPHC IOCTL to update the comp eng params.
      -------------------------------------------------------------------------*/
      if (iphc_ioctl( pdcp_ctl_blk[pdcp_inst].iphc_hdl,
                      iphc_comp_ioctl_type,
                      NULL ) == -1)
      {
        DATA_MSG0_ERROR("iphc_ioctl for comp failed, returned -1");
      }
      
      /*-------------------------------------------------------------------------
        Use the IPHC IOCTL to update the decomp eng params.
      -------------------------------------------------------------------------*/
      if (iphc_ioctl( pdcp_ctl_blk[pdcp_inst].iphc_hdl,
                      iphc_decomp_ioctl_type,
                      NULL ) == -1)
      {
        DATA_MSG0_ERROR("iphc_ioctl for decomp failed, returned -1");
      }
    }
  }
}/*pdcp_reset_comp_eng_hdlr*/

/*===========================================================================
FUNCTION PDCP_SET_ROHC_TARGET_MODE_HDLR

DESCRIPTION This function specific what mode ROHC decompressor
should transition to.
  
PARAMETERS
  cfg_req

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static void pdcp_set_rohc_target_mode_hdlr
(
  const pdcp_cfg_req_data_type *cfg_req
)
{
  uint8                             pdcp_inst;
  rohc_ioctl_e_type                 rohc_ioctl_type = 
    ROHC_IOCTL_SWITCH_DECOMP_MODE;
  errno_enum_type                   rohc_ioctl_errno;
  rohc_ioctl_u_type                 rohc_ioctl_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cfg_req == NULL) 
  {
    DATA_MSG0_ERROR("cfg_req is NULL");
    return;
  }

  pdcp_inst = pdcp_get_pdcp_inst_given_lc_id (cfg_req->subs_id,
                                             (uint8)(cfg_req->rlc_dl_id),
                                              (uint8)(cfg_req->rlc_ul_id));
  
  if (pdcp_inst >= MAX_PDCP_INST) 
  {
    DATA_MSG1_ERROR("invalid pdcp_inst = %d", pdcp_inst);
    return;
  }
  DATA_MSG0_HIGH("pdcp_set_rohc_target_mode_hdlr");

  if ((pdcp_ctl_blk[pdcp_inst].rohc_hdl != NULL) && 
      ((cfg_req->rohc_target_mode != ROHC_R_MODE) || 
       (cfg_req->rohc_target_mode != ROHC_O_MODE)))
  {
    rohc_ioctl_data.switch_decomp_mode = cfg_req->rohc_target_mode;
    if (rohc_ioctl( pdcp_ctl_blk[pdcp_inst].rohc_hdl,
                    rohc_ioctl_type,
                    &rohc_ioctl_data,
                    &rohc_ioctl_errno ) == -1)
    {
      DATA_MSG0_ERROR("rohc_ioctl failed, returned -1");
    } 
  }
  else
  {
    DATA_MSG2_ERROR ("rohc_hdl = %d, rohc_target_mode = %d",
               pdcp_ctl_blk[pdcp_inst].rohc_hdl,
               cfg_req->rohc_target_mode);
  }
}/*pdcp_set_rohc_target_mode_hdlr*/

/*===========================================================================
FUNCTION PDCP_UPDATE_DL_WM_LEVELS

DESCRIPTION
  This function gets invoked when RRC forwards RAB_Reconfig request to PDCP.
This function dynamically updates DL WM levels based on RAB rates.

PARAMETERS
  rab_dl_rate - new RAB DL rate to tune the WM levels to.
  wm_ptr - WM to be tuned.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pdcp_update_dl_wm_levels
(
  uint32                    rab_dl_rate,
  dsm_watermark_type        *wm_ptr
)
{

  if ( wm_ptr == NULL ) 
  {
    DATA_MSG0_ERROR("wm_ptr = NULL in update_dl_wm_levels");
    return;
  }

  DATA_MSG1_HIGH("updating WM for rab_dl_rate %d", rab_dl_rate);

  /*-------------------------------------------------------------------------
    WM levels for non-HS rates.
  -------------------------------------------------------------------------*/
  if (rab_dl_rate <= 1258291) 
  {
    dsm_set_low_wm (wm_ptr,
                    DSUMTSPS_DL_WM_DEFAULT_LO);
    dsm_set_hi_wm (wm_ptr,
                    DSUMTSPS_DL_WM_DEFAULT_HI);
    dsm_set_dne (wm_ptr,
                 DSUMTSPS_DL_WM_DEFAULT_DNE);
    dsm_set_dne_q_cnt(wm_ptr,
		      DSUMTSPS_DL_WM_DEFAULT_DNE_QCNT);
  }
  /*-------------------------------------------------------------------------
    WM levels for upto 1.8Mbps
  -------------------------------------------------------------------------*/
  else if (rab_dl_rate > 1258291 && rab_dl_rate <= 1800000) 
  {
    dsm_set_low_wm (wm_ptr,
                    DSWPSD_DL_WM_DEFAULT_HS_LO);
    dsm_set_hi_wm (wm_ptr,
                    DSWPSD_DL_WM_DEFAULT_HS_HI);
    dsm_set_dne (wm_ptr,
                 DSWPSD_DL_WM_DEFAULT_HS_DNE);
    dsm_set_dne_q_cnt(wm_ptr,
		      DSWPSD_DL_WM_DEFAULT_HS_DNE_QCNT);
  }
  /*-------------------------------------------------------------------------
    WM levels for upto 3.6Mbps
  -------------------------------------------------------------------------*/
  else if (rab_dl_rate > 1800000 && rab_dl_rate <= 3600000) 
  {
    dsm_set_low_wm (wm_ptr,
                    DSWPSD_DL_WM_DEFAULT_HS_P1_LO);
    dsm_set_hi_wm (wm_ptr,
                    DSWPSD_DL_WM_DEFAULT_HS_P1_HI);
    dsm_set_dne (wm_ptr,
                 DSWPSD_DL_WM_DEFAULT_HS_P1_DNE);
    dsm_set_dne_q_cnt(wm_ptr,
		      DSWPSD_DL_WM_DEFAULT_HS_P1_DNE_QCNT);
  }
  /*-------------------------------------------------------------------------
    WM levels for upto 7.2
  -------------------------------------------------------------------------*/
  else if (rab_dl_rate > 3600000 && rab_dl_rate <= 7200000) 
  {
    dsm_set_low_wm (wm_ptr,
                    DSWPSD_DL_WM_DEFAULT_HS_P2_LO);
    dsm_set_hi_wm (wm_ptr,
                    DSWPSD_DL_WM_DEFAULT_HS_P2_HI);
    dsm_set_dne (wm_ptr,
                 DSWPSD_DL_WM_DEFAULT_HS_P2_DNE);
    dsm_set_dne_q_cnt(wm_ptr,
		      DSWPSD_DL_WM_DEFAULT_HS_P2_DNE_QCNT);
  }
  /*-------------------------------------------------------------------------
    WM levels for rab rates upto 42
  -------------------------------------------------------------------------*/
  else if ((rab_dl_rate > 7200000) && (rab_dl_rate <= 42000000))
  {
    dsm_set_low_wm (wm_ptr,
                    DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_LO);
    dsm_set_hi_wm (wm_ptr,
                   DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_HI);
    dsm_set_dne (wm_ptr,
                 DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_DNE);
    dsm_set_dne_q_cnt(wm_ptr,
		      DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_DNE_QCNT);
  }
  /*-------------------------------------------------------------------------
	WM levels for rab rates upto 63
  -------------------------------------------------------------------------*/
  else if (rab_dl_rate > 42000000)  
  {
    dsm_set_low_wm (wm_ptr,
                    DSWPSD_DL_WM_DEFAULT_3C_HSDPA_LO);
    dsm_set_hi_wm (wm_ptr,
                   DSWPSD_DL_WM_DEFAULT_3C_HSDPA_HI);
    dsm_set_dne (wm_ptr,
                 DSWPSD_DL_WM_DEFAULT_3C_HSDPA_DNE);
  }


  MSG_HIGH("WM levels after update - Lo: %d, Hi: %d, DNE: %d",
	   wm_ptr->lo_watermark,
           wm_ptr->hi_watermark,
           wm_ptr->dont_exceed_cnt);

}/* pdcp_update_dl_wm_levels */

/*===========================================================================
FUNCTION PDCP_RECONFIG_HDLR

DESCRIPTION
  This function gets invoked when RRC forwards RAB_Reconfig request to PDCP.
  
PARAMETERS
  cfg_req

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void pdcp_reconfig_hdlr
(
  const pdcp_cfg_req_data_type *cfg_req
)
{
  uint8                             pdcp_inst;
  rohc_info_type                    rohc_info;
  errno_enum_type                   rohc_ioctl_errno;
  rohc_params_comp_channel_s_type   comp_channel_params;
  rohc_params_decomp_channel_s_type decomp_channel_params;
  uint16                            index;
  iphc_info_type                    iphc_info;
  iphc_ioctl_u_type                 iphc_ioctl_data;
  iphc_ioctl_e_type                 iphc_comp_ioctl =
    IPHC_IOCTL_COMPRESSOR_V4_SET_OPTIONS;
  iphc_ioctl_e_type                 iphc_decomp_ioctl =
    IPHC_IOCTL_DECOMPRESSOR_V4_SET_OPTIONS;
  iphc_config_options_s_type        iphc_options;
  rohc_ioctl_u_type                 ioctl_data;
  uint8                             comp_idx = 
    MAX_COMP_ENG_TYPES;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cfg_req == NULL) 
  {
    DATA_MSG0_ERROR("cfg_req is NULL");
    return;
  }

  pdcp_inst = pdcp_get_pdcp_inst_given_lc_id (cfg_req->subs_id, 
                                              (uint8)(cfg_req->rlc_dl_id),
                                              (uint8)(cfg_req->rlc_ul_id));
  
  /*-------------------------------------------------------------------------
  check if this instance has a valid entry in the pdcp_ctl_blk table.
  -------------------------------------------------------------------------*/
  if (pdcp_inst >= MAX_PDCP_INST) 
  {
    DATA_MSG0_ERROR("pdcp inst validation failed");
    return;
  }

  if (pdcp_validate_pdcp_inst(pdcp_inst) == FALSE) 
  {
    DATA_MSG0_HIGH("pdcp inst validation failed");
    return;
  }

  pdcp_ctl_blk[pdcp_inst].is_mcast_call = cfg_req->is_mcast_call;

  /*-------------------------------------------------------------------------
    Store the advertized DL data rate in the PDCP control block
  -------------------------------------------------------------------------*/
  pdcp_ctl_blk[pdcp_inst].dl_data_rate = cfg_req->rab_dl_rate;

  /*-------------------------------------------------------------------------
  if rb_recfg was received with no pdcp_info present and PDCP was previously
  in UP state, this is invalid, so fail recfg.
  -------------------------------------------------------------------------*/
  if ((pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_UP) &&
      (cfg_req->pdcp_info.pdcp_info_valid == FALSE)) 
  {
    DATA_MSG0_ERROR("rb_recfg without pdcp_info rcvd in UP state");
    return;
  }
  
  DATA_MSG1_HIGH("pdcp_reconfig_hdlr for pdcp_inst = %d", pdcp_inst);

  /*-------------------------------------------------------------------------
    if rb_recfg was received with pdcp_info present with valid compression 
    engine info and PDCP was previously in TRANSPARENT state, 
    then PDCP moves to UP state.
  -------------------------------------------------------------------------*/
  if ((pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_TRANSPARENT) &&
      ((cfg_req->pdcp_info.pdcp_info_valid == TRUE) &&
       (pdcp_comp_eng_info_valid(cfg_req->pdcp_info))))
  {
    DATA_MSG0_HIGH("rb_recfg with pdcp_info rcvd in TRANSPARENT state");
    pdcp_ctl_blk[pdcp_inst].pdcp_state = PDCP_UP;
  }
  
  /*-------------------------------------------------------------------------
  if rb_recfg was received while PDCP in DOWN state, this is invalid.
  -------------------------------------------------------------------------*/
  if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_DOWN)
  {
    DATA_MSG0_ERROR("rb_recfg rcvd in DOWN state");
    return;
  }
  
  if ( pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_UP )
  {
    if (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm != NULL) 
    {
      DATA_MSG0_HIGH("rb_recfg in UP state, updating pdcp_to_ps rx WM");
      pdcp_update_dl_wm_levels(cfg_req->rab_dl_rate,
                               pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm);
    }
    DATA_MSG0_HIGH("rb_recfg in UP state, updating rlc rx WM");
    pdcp_update_dl_wm_levels(cfg_req->rab_dl_rate,
                             &(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_rx_wm_item));
  }
  
  pdcp_ctl_blk[pdcp_inst].pdcp_info = cfg_req->pdcp_info;
  /*-------------------------------------------------------------------------
  Construct pid table for this pdcp inst.
  -------------------------------------------------------------------------*/
  if (pdcp_ctl_blk[pdcp_inst].pdcp_info.pdcp_pdu_hdr == PRESENT) 
  {
    if (pdcp_construct_pid_table(pdcp_inst) != TRUE)
    {
      DATA_MSG0_ERROR("pid table const failed");
    }
  }
  else
  {
    pdcp_reset_pid_tables(pdcp_inst);
    DATA_MSG0_HIGH("pdcp header absent, reset pid table");
  }

  /*-------------------------------------------------------------------------
    Support is available to only reset the context, need support to update
    parameters.
  -------------------------------------------------------------------------*/
  
  if (pdcp_is_rohc_cfg(pdcp_ctl_blk[pdcp_inst].pdcp_info) == TRUE)
  {
    if (pdcp_ctl_blk[pdcp_inst].rohc_hdl != NULL) 
    {
      memset( &comp_channel_params, 0, sizeof(comp_channel_params));
      memset( &decomp_channel_params, 0, sizeof(decomp_channel_params));
      comp_idx = (uint8)pdcp_get_index_given_comp_eng(pdcp_ctl_blk[pdcp_inst]
                                               .pdcp_info, PDCP_ROHC);
      if (comp_idx >= MAX_COMP_ENG_TYPES)
      {
        DATA_MSG0_ERROR("Invalid comp_eng_info passed");      
        return;
      }      
      rohc_info = pdcp_ctl_blk[pdcp_inst].pdcp_info.comp_eng_info
        [comp_idx].rohc_info;
      
      comp_channel_params.max_cid = rohc_info.rohc_ul_max_cid;
      comp_channel_params.use_large_cids = (comp_channel_params.max_cid > ROHC_DEF_MAX_CID) ?
        TRUE : FALSE;
      comp_channel_params.num_allowed_pkt_sizes = rohc_info.rohc_num_allowed_pkt_sizes;
      
      for (index = 0; index < comp_channel_params.num_allowed_pkt_sizes; index ++) 
      {
        comp_channel_params.allowed_pkt_sizes[index] = 
          rohc_info.rohc_ul_pkt_sizes_allowed[index];
      }
      
      for ( index = 0; index < (uint32)ROHC_PROFILE_MAX; index++ ) 
      {
        /*lint -e641*/
        comp_channel_params.profiles = (rohc_profiles_mask_e_type)( (int)comp_channel_params.profiles | 
                                                                  (1<<rohc_info.rohc_profiles[index]) ); 
        decomp_channel_params.profiles = (rohc_profiles_mask_e_type)( (int)decomp_channel_params.profiles | 
                                                                    (1<<rohc_info.rohc_profiles[index]) );
        /*lint +e641*/
      }
      decomp_channel_params.max_cid = rohc_info.rohc_dl_max_cid;
      decomp_channel_params.use_large_cids = (decomp_channel_params.max_cid > 
                                              ROHC_DEF_MAX_CID) ? TRUE : FALSE;
      ioctl_data.comp_channel_params = &comp_channel_params;
      ioctl_data.decomp_channel_params = &decomp_channel_params;
      /*-------------------------------------------------------------------------
        Use the ROHC IOCTL API to update the comp/decomp parameters.
      -------------------------------------------------------------------------*/
      if (rohc_ioctl( pdcp_ctl_blk[pdcp_inst].rohc_hdl,
                  ROHC_IOCTL_REINIT_COMP_CHANNEL,
                  &ioctl_data,
                  &rohc_ioctl_errno ) == -1)
      {
        DATA_MSG0_ERROR("rohc_ioctl failed, returned -1");
      }
      if (rohc_ioctl( pdcp_ctl_blk[pdcp_inst].rohc_hdl,
                  ROHC_IOCTL_REINIT_DECOMP_CHANNEL,
                  &ioctl_data,
                  &rohc_ioctl_errno ) == -1)
      {
        DATA_MSG0_ERROR("rohc_ioctl failed, returned -1");
      }
    }
    else
    {
      pdcp_setup_rohc_comp_eng(pdcp_inst, cfg_req->pdcp_info);
    }
  }
  else
  {
    if (pdcp_ctl_blk[pdcp_inst].rohc_hdl != NULL) 
    {
      rohc_free(pdcp_ctl_blk[pdcp_inst].rohc_hdl);
      pdcp_ctl_blk[pdcp_inst].rohc_hdl = NULL;
      pdcp_ctl_blk[pdcp_inst].rohc_dl_cid_inc_info = ROHC_INVALID_CID_INC;
      pdcp_ctl_blk[pdcp_inst].rohc_ul_cid_inc_info = ROHC_INVALID_CID_INC;
    }
  }

  if (pdcp_is_iphc_cfg(pdcp_ctl_blk[pdcp_inst].pdcp_info) == TRUE)
  {
    /*check if not null in pdcp info, not in ctl blk */
    if (pdcp_ctl_blk[pdcp_inst].iphc_hdl != NULL) 
    {
      comp_idx = (uint8) pdcp_get_index_given_comp_eng(pdcp_ctl_blk[pdcp_inst]
                                               .pdcp_info, PDCP_ROHC);
      if (comp_idx >= MAX_COMP_ENG_TYPES)
      {
        DATA_MSG0_ERROR("Invalid comp_eng_info passed");      
        return;
      }
      iphc_info = pdcp_ctl_blk[pdcp_inst].pdcp_info.comp_eng_info
        [comp_idx].iphc_info;
      
      /*lint -e641*/
      iphc_options.expect_reordering = iphc_info.expect_reorder;
      /*lint +e641*/
      iphc_options.f_max_period = iphc_info.iphc_f_max_period;
      iphc_options.f_max_time = iphc_info.iphc_f_max_time;
      iphc_options.max_header = iphc_info.iphc_max_header;
      iphc_options.rtp_enabled = FALSE;
      iphc_options.non_tcp_space = iphc_info.iphc_non_tcp_space;
      iphc_options.tcp_space = iphc_info.iphc_tcp_space;
      iphc_ioctl_data.options = iphc_options;
      
      /*-------------------------------------------------------------------------
        Use the IPHC IOCTL to update the comp eng params.
      -------------------------------------------------------------------------*/
      if (iphc_ioctl( pdcp_ctl_blk[pdcp_inst].iphc_hdl,
                      iphc_comp_ioctl,
                      &iphc_ioctl_data ) == -1)
      {
        DATA_MSG0_ERROR("iphc_ioctl for comp failed, returned -1");
      }
      
      /*-------------------------------------------------------------------------
        Use the IPHC IOCTL to update the decomp eng params.
      -------------------------------------------------------------------------*/
      if (iphc_ioctl( pdcp_ctl_blk[pdcp_inst].iphc_hdl,
                      iphc_decomp_ioctl,
                      &iphc_ioctl_data ) == -1)
      {
        DATA_MSG0_ERROR("iphc_ioctl for decomp failed, returned -1");
      }
    }
    else
    {
      pdcp_setup_iphc_comp_eng(pdcp_inst, cfg_req->pdcp_info);
    }
  }
  else
  {
    if (pdcp_ctl_blk[pdcp_inst].iphc_hdl != NULL) 
    {
      iphc_free(pdcp_ctl_blk[pdcp_inst].iphc_hdl);
      pdcp_ctl_blk[pdcp_inst].iphc_hdl = NULL;
    }
  }
}/*pdcp_reconfig_hdlr*/

/*===========================================================================
FUNCTION pdcp_RRC_CFG_REQ_CMD_HDLR

DESCRIPTION
  This function will be reigstered as a callback for PDCP CFG REQ CMD
  with PS task.
  
PARAMETERS
  cmd: the CMD itself being processed by ps-task
  cfg_req_data: data to be used for processing this command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pdcp_rrc_cfg_req_cmd_hdlr
(
  const comp_cmd_type  *cmd_ptr                             /* Pointer to command */
)
{
  pdcp_cfg_req_data_type    *cfg_req = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd_ptr == NULL)
  {
    DATA_MSG0_ERROR("Command pointer is NULL");
    return ;
  }

  cfg_req = (pdcp_cfg_req_data_type *)(&cmd_ptr->cmd.pdcp_cfg_req);

  if (cfg_req == NULL)
  {
    DATA_MSG0_ERROR("Cfg_req is NULL");
    return;
  }

  cfg_req->subs_id = cmd_ptr->hdr.subs_id;

  /*Validate subscription id*/
  if(!ds3gsubsmgr_is_subs_id_valid(cfg_req->subs_id))
  {
    DATA_MSG1_ERROR("Invalid Subs id(CM):%d in pdcp_rrc_cfg_req_cmd_hdlr "
                       ,cfg_req->subs_id);
    return ;
  }

  if (cmd_ptr->hdr.cmd_id != CPDCP_CONFIG_REQ) 
  {
    DATA_MSG1_ERROR("invalid cmd id in cfg_req_cmd_hdlr %d", cmd_ptr->hdr.cmd_id);
    ASSERT(0);
    return;
  }

  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  DATA_MSG1_HIGH("Calling %d hdlr", cfg_req->action);

  switch (cfg_req->action) 
  {
    /*-------------------------------------------------------------------------
    Act on RB_Setup from RRC
    -------------------------------------------------------------------------*/
    case SETUP_PDCP:
      pdcp_setup_hdlr (cfg_req);
      break;
    
    /*-------------------------------------------------------------------------
    Act on RAB_Release from RRC
    -------------------------------------------------------------------------*/
    case RELEASE_PDCP:
      pdcp_release_hdlr (cfg_req);
      break;

    /*-------------------------------------------------------------------------
    Act on RB_Reconfig from RRC
    -------------------------------------------------------------------------*/
    case RECONFIG_PDCP:
      pdcp_reconfig_hdlr (cfg_req);
      break;

    /*-------------------------------------------------------------------------
    Act on RAB_Release from RRC
    -------------------------------------------------------------------------*/
    case RELEASE_ALL_PDCP:
      pdcp_release_all_hdlr (cfg_req);
      break;
      
    /*-------------------------------------------------------------------------
    RRC will send this when there is a RNC HO and the comp eng needs to be reset.
    -------------------------------------------------------------------------*/
    case RESET_COMP_ENG_PDCP:
      pdcp_reset_comp_eng_hdlr (cfg_req);
      break;

    /*-------------------------------------------------------------------------
    RRC will send this when ROHC_TARGET_MODE IE is received.
    -------------------------------------------------------------------------*/
    case SET_ROHC_TARGET_MODE_PDCP:
      pdcp_set_rohc_target_mode_hdlr (cfg_req);
      break;

      /*-------------------------------------------------------------------------
    Invalid pdcp action
    -------------------------------------------------------------------------*/
    default:
      DATA_MSG1_ERROR("Invalid PDCP action for (CM) Subs ID: %d",
                      cfg_req->subs_id);
      break;
  }
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
} /* pdcp_rrc_cfg_req_cmd_hdlr */

/*===========================================================================
FUNCTION pdcp_VALIDATE_PARAMS_SUB

DESCRIPTION
  This primitive is used by RRC to validate pdcp_info parameters 
  sent by UTRAN for a given pdcp_instance.

PARAMETERS
  subs_id -  Subscription ID
  pdcp_info - the parameters to be validated
  is_mcast_call - boolean mcast call


DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS If the validation succeeded.
  E_FAILURE If the validation failed.

SIDE EFFECTS
  None

===========================================================================*/
boolean pdcp_validate_params_sub
(
  sys_modem_as_id_e_type  subs_id,
  pdcp_info_type pdcp_info,
  boolean        is_mcast_call
)
{
  uint32 index;
  pdcp_comp_eng_e_type comp_eng;
  boolean ret_val = TRUE;
  /*-------------------------------------------------------------------------
    NO_HEADER_COMPRESSION is always present in pid_table, hence start with
    1 here.
  -------------------------------------------------------------------------*/
  uint32 num_ul_pid_entries = 1;
  uint32 num_dl_pid_entries = 1;
  uint32  iphc_index = 0;
  uint32  rohc_index = 0;
  iphc_info_type iphc_info;
  rohc_info_type rohc_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*Validate subscription id*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DATA_MSG1_ERROR("Invalid Subs id(CM):%d in pdcp_validate_params_sub",subs_id);
    return FALSE;
  }

  DATA_MSG1_HIGH("pdcp_validate_params for (CM) subs id %d", subs_id);

  if ((pdcp_info.support_for_lossless == TRUE) && (pdcp_info.pdcp_pdu_hdr == ABSENT)) 
  {
    DATA_MSG0_ERROR("lossless support is true but pdcp_pdu_hdr is absent");
    return FALSE;
  }

  if ((pdcp_is_iphc_cfg(pdcp_info)) && (pdcp_info.pdcp_pdu_hdr == ABSENT))
  {
    DATA_MSG0_ERROR("IPHC is cfg but PDCP hdr is absent");
    return FALSE;
  }


  if (pdcp_is_iphc_cfg(pdcp_info)) 
  {
    iphc_index = pdcp_get_index_given_comp_eng(pdcp_info, PDCP_IPHC);

   if (iphc_index >= MAX_COMP_ENG_TYPES)
   {
     DATA_MSG0_ERROR("Invalid comp_eng_info passed");      
     return FALSE;
   }

   iphc_info = pdcp_info.comp_eng_info[iphc_index].iphc_info;
   if (iphc_info.expect_reorder == REORDERING_EXPECTED) 
   {
     DATA_MSG0_ERROR("IPHC does not support reordering");
     return FALSE;
   }
   
   if (iphc_info.iphc_f_max_period == 0 ||
       iphc_info.iphc_f_max_time == 0 || 
       iphc_info.iphc_max_header < 60 ||
       iphc_info.iphc_non_tcp_space < 3 || 
       iphc_info.iphc_tcp_space < 3) 
   {
     DATA_MSG0_ERROR("IPHC parameter = 0");
     return FALSE;
   }
  }

  if (pdcp_is_rohc_cfg(pdcp_info)) 
  {
    rohc_index = pdcp_get_index_given_comp_eng(pdcp_info, PDCP_ROHC);
    
    if (rohc_index >= MAX_COMP_ENG_TYPES)
    {
      DATA_MSG0_ERROR("Invalid comp_eng_info passed");      
      return FALSE;
    }

    rohc_info = pdcp_info.comp_eng_info[rohc_index].rohc_info;

    if (is_mcast_call == FALSE) 
    {
      if (rohc_info.rohc_dl_max_cid == 0 || 
          rohc_info.rohc_ul_max_cid == 0 )
      {
        DATA_MSG2_ERROR("ROHC parameter = %d %d", rohc_info.rohc_dl_max_cid, 
                  rohc_info.rohc_ul_max_cid);
        return FALSE;
      }
    }
    else
    {
      if (rohc_info.rohc_dl_max_cid == 0)
      {
        DATA_MSG1_ERROR("ROHC dl max cid = %d", rohc_info.rohc_dl_max_cid);
        return FALSE;
      }
    }
  }
  
  /*-------------------------------------------------------------------------
  check if num pid entries exceeds 32, fail the validation, otherwise returns
  success.
  -------------------------------------------------------------------------*/
  for ( index = 0; index < MAX_COMP_ENG_TYPES; index ++) 
  {
    comp_eng = pdcp_get_comp_eng_type (pdcp_info, index);
    if (comp_eng == PDCP_IPHC) 
    {
      num_dl_pid_entries += NUM_IPHC_ENTRIES;
      num_ul_pid_entries += NUM_IPHC_ENTRIES;
    }
    else if (comp_eng == PDCP_ROHC) 
    {
      /*-------------------------------------------------------------------------
      if UL / DL use rohc_pkt_fmt, increment num entries by 1 since rohc_pkt_fmt
      takes up only 1 pid entry.
      -------------------------------------------------------------------------*/
        num_dl_pid_entries += 1;
        num_ul_pid_entries += 1;
      }
    else if ( comp_eng == PDCP_NO_COMP )
    {
      break;
    }
  }

  /*-------------------------------------------------------------------------
  if either UL or DL num entries exceed 32, return FALSE.
  -------------------------------------------------------------------------*/
  if ((num_dl_pid_entries > MAX_PID_ENTRIES) ||
      (num_ul_pid_entries > MAX_PID_ENTRIES))
  {
    DATA_MSG2_ERROR("num pid entries exceed 32, dl_pid = %d, ul_pid = %d",
              num_dl_pid_entries, num_ul_pid_entries);
    return FALSE;
  }
  return ret_val;
} /* pdcp_validate_params_sub */

/*===========================================================================
FUNCTION pdcp_VALIDATE_PARAMS

DESCRIPTION
  This primitive is used by RRC to validate pdcp_info parameters 
  sent by UTRAN for a given pdcp_instance.

PARAMETERS
  pdcp_info - the parameters to be validated

DEPENDENCIES
  None

RETURN VALUE
              TRUE - if the parameters validated.
              FALSE - if validation or pid mapping table construction failed.
SIDE EFFECTS
  If this function returns FALSE, RRC reject RB_Setup/RB_Release/RB_Reconfig.

===========================================================================*/
boolean pdcp_validate_params
(
  pdcp_info_type pdcp_info,
  boolean        is_mcast_call
)
{
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return pdcp_validate_params_sub(subs_id, pdcp_info, is_mcast_call);
} /* pdcp_validate_params */


/*===========================================================================
FUNCTION pdcp_GET_NEXT_DL_RX_SN

DESCRIPTION
  This primitive is used by RRC to get the next_rx_sn on the DL when a 
  RLC PDU size change is received from UTRAN through an RB_RECONFIG message.
  This will be used only after RLC implements the PDU size change feature.
  
PARAMETERS
  pdcp_instance - Index into the pdcp control block
  next_rx_sn - parameter that will be filled with SN value

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS If next_dl_rx_sn retrieval succeeded.
  E_FAILURE If next_dl_rx_sn retrieval failed.

SIDE EFFECTS
  None

===========================================================================*/
errno_enum_type pdcp_get_next_dl_rx_sn
(
  uint8     pdcp_inst, 
  uint32    *next_rx_sn
)
{
  /*-------------------------------------------------------------------------
  Check to make sure RLC is in AM and pdcp is in UP state.
  -------------------------------------------------------------------------*/
  if ((pdcp_ctl_blk[pdcp_inst].rlc_state.lc_mode == UE_MODE_ACKNOWLEDGED) &&
      (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_UP))
  {
    /*-------------------------------------------------------------------------
    Whenever PDCP receives data, it increments next_rx_sn.
    -------------------------------------------------------------------------*/
    *next_rx_sn = pdcp_ctl_blk[pdcp_inst].next_rx_sn;
    DATA_MSG1_HIGH("pdcp_get_next_dl_rx_sn = %d", *next_rx_sn);
    return E_SUCCESS;
  }
  else
  {
    DATA_MSG2_ERROR("lc mode/pdcp_state check failed, lc_mode = %d, pdcp_state = %d",
              pdcp_ctl_blk[pdcp_inst].rlc_state.lc_mode,
              pdcp_ctl_blk[pdcp_inst].pdcp_state);
    return E_FAILURE;
  }
} /* pdcp_get_next_dl_rx_sn */



#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_TDSCDMA */


