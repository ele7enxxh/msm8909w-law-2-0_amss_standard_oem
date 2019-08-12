/*============================================================================
  rr_seg_load.c

  This file implements the RR-FEMTO module.
  See comments below for more detail.

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_seg_load.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/** 
  @file rr_seg_load.c 
   
  This module contains definitions for use by the WCDMA, 
  TD-SCDMA segmented loading feature. 
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "rr_seg_load.h"

#ifdef FEATURE_SEGMENT_LOADING
/*----------------------------------------------------------------------------
 * Typedefs
 * -------------------------------------------------------------------------*/
typedef struct
{
  rr_geran_seg_load_t seg_load[RR_SEG_LOAD_NUM_RATS];
} rr_seg_load_data_t;


/*----------------------------------------------------------------------------
 * Private Data
 * -------------------------------------------------------------------------*/
/*no need to data space this as segmented loading applies 
  to both subs*/
rr_seg_load_data_t rr_seg_load_data;

rr_geran_seg_load_t *rr_seg_load_ptr = rr_seg_load_data.seg_load;

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION rr_seg_load_map_rr_to_client

DESCRIPTION
  Maps a RAT to a client id for segment loading.
 
PARAMS
  rat

RETURN VALUE
  clsid_t

===========================================================================*/
static clsid_t rr_seg_load_map_rr_to_client(rr_geran_seg_load_rat_t rat)
{
  clsid_t client_id = 0;

  switch (rat)
  {
    case RR_SEG_LOAD_WCDMA: 
      client_id = CLSID_WCDMA; 
    break;

    case RR_SEG_LOAD_TDSCDMA: 
      client_id = CLSID_TDSCDMA; 
    break;

    default:
      MSG_ERROR("RR seg mem unknown rat:%d",rat,0,0);
    break;
  }

  return client_id;
}

/*===========================================================================

FUNCTION rr_initialise_seg_load

DESCRIPTION
  Populates the segmented loading database for all RAT's that are segment
  loaded.
 
PARAMS
  none

RETURN VALUE
  none

===========================================================================*/
void rr_initialise_seg_load()
{
  rr_geran_seg_load_rat_t rat = 0;

  for (rat=0; rat < RR_SEG_LOAD_NUM_RATS; rat++)
  {
    if (create_instance( rr_seg_load_map_rr_to_client(rat), &(rr_seg_load_ptr[rat].rat_interface_ptr)) == E_SUCCESS)
    {
      if (rr_seg_load_ptr[rat].rat_interface_ptr == NULL)
      {
        MSG_ERROR("create_instance returns E_SUCCESS but interface is NULL",0,0,0);
        rr_seg_load_ptr[rat].rat_loaded = FALSE;
      }
      else
      {
        rr_seg_load_ptr[rat].rat_loaded = TRUE;
      }
    }
    else
    {
      rr_seg_load_ptr[rat].rat_loaded = FALSE;
      rr_seg_load_ptr[rat].rat_interface_ptr = NULL;
    }

    MSG_HIGH("seg load rat:%d loaded:%d",rat,rr_seg_load_ptr[rat].rat_loaded,0);
  }
}
#endif /*FEATURE_SEGMENT_LOADING*/

/*===========================================================================

FUNCTION rr_is_rat_segment_loaded

DESCRIPTION
  Query if a RAT is segment loaded.
 
PARAMS
  rat

RETURN VALUE
  boolean - TRUE - RAT loaded, FALSE - RAT not loaded

===========================================================================*/
boolean rr_is_rat_segment_loaded(rr_geran_seg_load_rat_t rat)
{
#ifdef FEATURE_SEGMENT_LOADING
  boolean rat_segment_loaded = FALSE;

  if (rat >= RR_SEG_LOAD_NUM_RATS)
  {
    MSG_ERROR("RR invalid RAT:%d",rat,0,0);
  }
  else
  {
    rat_segment_loaded = rr_seg_load_ptr[rat].rat_loaded;
  }

  return rat_segment_loaded;
#else
  return TRUE;
#endif /*FEATURE_SEGMENT_LOADING*/
}
/* EOF */

