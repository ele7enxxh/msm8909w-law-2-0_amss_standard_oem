/*===========================================================================

             L O O P B A C K   D A T A Working

                               
DESCRIPTION
  This file contains data and function definitions Looback data.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/loopback.c_v   1.14   26 Jun 2002 18:51:34   madiraju  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/tc/src/tc_loopback.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
 
03/28/12   us       Compilation warnings fixed 
10/04/06   rk      replaced code to free dsm packet as a whole instead of freeing byte by byte.
07/11/06   rk      loopback.c/h is renamed to tc_loopback.c/h.
01/24/06   hj      Emptying the DSM WM when SDU size is set to 0, for that 
                   logical channel.
10/25/04   mks     Tuning the UL RLC sdu size appropriately based on the 
                   loopback configuration received in CLOSE_UE_TEST_LOOP 
                   message. 
10/13/04   mks     Added support for handling loopback configuration with 
                   radio bearers mapped to multiple RABs
08/31/04   mks     Added support for handling multiple radio bearers during
                   loopback call setup
08/19/04   mks     Per Table A.7.1 of TS34.109, truncate the DL SDU to match 
                   the UL RLC SDU size for test loop mode 1
07/02/04   mks     Now handling variable length DL RLC SDU sizes during test
                   loop closure. Avoided the hard coding of maximum DL SDU size.
12/05/03   mks     Fixed DL WM overflow issues when network requests larger 
                   SDU size while closing the test loop during RLC testing.
06/26/02   mks     Fixed DL WM overflow issues when network sends user data even 
                   before UE test loop is closed
04/17/02   mks     Tune the length of tx rlc sdu size for test loop mode 1 & 2
03/29/02   mks     Added support for test loop mode 2, 
                   Added lb_mode1_rx_handler () & lb_mode2_rx_handler () 
03/25/02   mks     Message set for internal messages has been set to MS_TC_TC
03/20/02   mks     Fixed DL WM overflow issue 
02/07/02   mks     Fixed issues with RLC UL/DL water mark registration
02/07/02   ks      Packing RLC SDU size date in lb_tx_data_to_rlc.
02/06/02   ks      Register UL/DL water marks with RLC
09/28/01   ks      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

//NPR
#include "mmcp_variation.h"
#include "customer.h"
#ifdef FEATURE_TC

#define FEATURE_DSM_WM_CB
#include "tc_loopback.h"
#include "tctask.h"
#include "tctask_v.h"
#include "dsmutil.h"
#include "wcdmamvsif.h"
#include "rrcdata.h"
#include "msg.h"
#include "tc.h"
#include "err.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#include "mm.h"
#include "rlcrrcif.h"
#ifdef FEATURE_TDSCDMA
#include "tdsrrcdata.h"
#include "tdsl2external.h"
#endif /* FEATURE_TDSCDMA */
#include <stringl/stringl.h>
#define THRESHOLD_TC_DSM_DUP 10

#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

#ifdef FEATURE_MBMS
#error code not present
#endif
// Place holder for received DSM item length on the DL
uint16 dsm_item_len = 0;


/*---------------------------------------------------------------------------
  RLC registration service types for UL/DL Watermarks.
---------------------------------------------------------------------------*/
rlc_ul_register_srvc_type lb_rlc_ul_reg_params;
  /* Uplink parameters to be registered with RLC */

rlc_dl_register_srvc_type lb_rlc_dl_reg_params;
  /* Downlink parameters to be registered with RLC */

#ifdef FEATURE_TDSCDMA
tdsl2_ul_service_register_type lb_tdsrlc_ul_reg_params;
  /* Uplink parameters to be registered with RLC */

tdsl2_dl_service_register_type lb_tdsrlc_dl_reg_params;
  /* Downlink parameters to be registered with RLC */
#endif /* FEATURE_TDSCDMA */

extern boolean post_self_cmd_flag[MAX_RAB_TO_SETUP][UE_MAX_DL_LOGICAL_CHANNEL];

/*---------------------------------------------------------------------------
  RRC User plane LC info type, used for quering the LC info for a RAB.
---------------------------------------------------------------------------*/

/*===========================================================================

                      EXTERNAL DATA DECLARATIONS

===========================================================================*/
extern boolean dummy_dcch_enabled;

/*===========================================================================

                      EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
extern boolean is_tc_test_loop_closed(void);

#ifndef FEATURE_SEGMENT_LOADING
extern void rlc_ul_register_srvc_handler 
(
  rlc_ul_register_srvc_type *reg_ptr      /* Ptr to WM registration info   */
);

extern void rlc_dl_register_srvc_handler
(
  rlc_dl_register_srvc_type *reg_ptr      /* Ptr to WM registration Info   */
);
#endif
/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Buffer for DL data from RLC 
---------------------------------------------------------------------------*/  

/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION LB_GET_WM_ID

DESCRIPTION
  This function generates an appropriate watermark ID for loopback data
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 lb_get_key (byte rab_id, uint32 chan_id)
{
  uint32 key = (rab_id * MAX_RAB_TO_SETUP) + chan_id;
  return (key);
}

/*===========================================================================

FUNCTION LB_GET_RB_ID_FROM_KEY

DESCRIPTION
  This function extracts the RB ID from LB watermark Key
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 lb_get_chan_id_from_key (uint32 wm_key)
{
  return (wm_key % MAX_RAB_TO_SETUP);
}

/*===========================================================================

FUNCTION LB_GET_RAB_ID_FROM_KEY

DESCRIPTION
  This function extracts the RAB ID from LB Key
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

uint32 lb_get_rab_id_from_key (uint32 wm_key)
{
  return (wm_key / MAX_RAB_TO_SETUP);
}

/*===========================================================================

FUNCTION LB_INIT_DLRLC_WM

DESCRIPTION
  This function initializes the DL RLC/DS watermark.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lb_init_dlrlc_wm( byte rab_id )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the DL RLC/DS watermark.
  -------------------------------------------------------------------------*/  
  uint32 index1;

  for (index1=0; index1<UE_MAX_DL_LOGICAL_CHANNEL; index1++)
  {
      (void) dsm_queue_init (&lb_rab_list[rab_id]->lb_rlc_dl_wm[index1], (int) 1, 
                                                &lb_rab_list[rab_id]->lb_dl_rlc_q[index1] );
  
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].q_ptr                  = &(lb_rab_list[rab_id]->lb_dl_rlc_q[index1]);
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].lo_watermark           = MAX_LB_SDU_SIZE;
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].hi_watermark           = MAX_LB_SDU_SIZE * 17;
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].gone_empty_func_ptr    = NULL; 
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].lowater_func_ptr       = NULL;
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].non_empty_func_ptr     = NULL;
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].non_empty_func_data    = (void *) index1;
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].each_enqueue_func_ptr  = NULL;
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].each_enqueue_func_data = (void *) index1;
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].hiwater_func_ptr       = NULL;
    lb_rab_list[rab_id]->lb_rlc_dl_wm[index1].current_cnt            = 0;  
  }
} /* lb_init_dlrlc_wm() */

/*===========================================================================

FUNCTION LB_INIT_ULRLC_WM

DESCRIPTION
  This function initializes the UL DS/RLC watermark.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lb_init_ulrlc_wm( byte rab_id )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the DL RLC/DS watermark.
  -------------------------------------------------------------------------*/  
  uint32 index1;

  for (index1=0; index1<UE_MAX_UL_LOGICAL_CHANNEL; index1++)
  {
  
      (void) dsm_queue_init (&lb_rab_list[rab_id]->lb_rlc_ul_wm[index1], (int) (MAX_LB_SDU_SIZE * 20), 
                                                &lb_rab_list[rab_id]->lb_ul_rlc_q[index1] );

    lb_rab_list[rab_id]->lb_rlc_ul_wm[index1].lo_watermark           = 0;
    lb_rab_list[rab_id]->lb_rlc_ul_wm[index1].hi_watermark           = MAX_LB_SDU_SIZE * 17;
    lb_rab_list[rab_id]->lb_rlc_ul_wm[index1].gone_empty_func_ptr    = NULL; 
    lb_rab_list[rab_id]->lb_rlc_ul_wm[index1].lowater_func_ptr       = NULL;
    lb_rab_list[rab_id]->lb_rlc_ul_wm[index1].non_empty_func_ptr     = NULL;
    lb_rab_list[rab_id]->lb_rlc_ul_wm[index1].each_enqueue_func_ptr  = NULL;
    lb_rab_list[rab_id]->lb_rlc_ul_wm[index1].hiwater_func_ptr       = NULL;
    lb_rab_list[rab_id]->lb_rlc_ul_wm[index1].current_cnt            = 0;  
  }
} /* lb_init_ulrlc_wm() */

#ifdef FEATURE_WCDMA
/*===========================================================================
FUNCTION LB_REGISTER_RLC_WM

DESCRIPTION
  This function registers the UL/UL watermarks with the RLC for data 
  transfer.
  
DEPENDENCIES
  This function should be called only after a successful RAB establishment.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void lb_register_rlc_wm( byte rab_id )
{
  uint8 i;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t* ptr_w = NULL;
  
  ptr_w = get_wcdma_interface();
  if (ptr_w == NULL)
  {
     ERR_FATAL("Segment loading error\n", 0,0,0);
  }
#endif  
  MSG_HIGH_DS_1(TC_SUB,"TC: Registering RAB Id=%d", rab_id);

  lb_rab_list[rab_id]->lc_info_ptr.num_ul_lc_ids_for_rab = 0;
  lb_rab_list[rab_id]->lc_info_ptr.num_dl_lc_ids_for_rab = 0;
  lb_rab_list[rab_id]->lc_info_ptr.rab_id                = rab_id;
  lb_rab_list[rab_id]->rab_id                            = rab_id;
  
  for ( i = 0; i < RRC_MAX_LC_ID_PER_RAB; i++ )
  {
    lb_rab_list[rab_id]->lc_info_ptr.ul_lc_id[ i ] = 0;
    lb_rab_list[rab_id]->lc_info_ptr.dl_lc_id[ i ] = 0;
  }
#ifdef FEATURE_SEGMENT_LOADING  
  if ( IWCDMA_rrc_return_lc_info_for_rab(ptr_w,  &(lb_rab_list[rab_id]->lc_info_ptr) ) == RRC_RAB_NOT_FOUND )
  {
    ERR( "Invalid RAB id %d", rab_id, 0, 0 ); 
  }
#else
  if ( rrc_return_lc_info_for_rab( &(lb_rab_list[rab_id]->lc_info_ptr) ) == RRC_RAB_NOT_FOUND )
  {
    ERR( "Invalid RAB id %d", rab_id, 0, 0 ); 
  }
#endif
  
  if ( ( lb_rab_list[rab_id]->lc_info_ptr.num_ul_lc_ids_for_rab > 1 ) ||
       ( lb_rab_list[rab_id]->lc_info_ptr.num_dl_lc_ids_for_rab > 1 ) )
  {
    MSG_HIGH_DS_2( TC_SUB, "More than 1 LC id: UL %d; DL %d",
         lb_rab_list[rab_id]->lc_info_ptr.num_ul_lc_ids_for_rab,
         lb_rab_list[rab_id]->lc_info_ptr.num_dl_lc_ids_for_rab
         ); 
  }

  /* ------------------------------------------------------------
  * Set up the RLC uplink registration parameters
  * -----------------------------------------------------------*/

  lb_rlc_ul_reg_params.nchan = lb_rab_list[rab_id]->lc_info_ptr.num_ul_lc_ids_for_rab;
  /* Number of uplink logical channels to register */
  lb_rlc_ul_reg_params.nchan = MIN(lb_rlc_ul_reg_params.nchan, RRC_MAX_LC_ID_PER_RAB);
  for (i=0; i<lb_rlc_ul_reg_params.nchan; i++)
  {

    lb_rlc_ul_reg_params.rlc_data[ i ].lc_id = lb_rab_list[rab_id]->lc_info_ptr.ul_lc_id[ i ];
       /* Uplink RLC logical channel ID */
                                                         
    lb_rlc_ul_reg_params.rlc_data[ i ].ul_wm_ptr = &(lb_rab_list[rab_id]->lb_rlc_ul_wm[ i ]);
    /* Pointer to the watermark for storing uplink data */
    MSG_HIGH_DS_2(TC_SUB,"TC: UL LC ID= %d, UL LC ID Index = %d", lb_rab_list[rab_id]->lc_info_ptr.ul_lc_id[ i ], i);

  }
  
  /* ------------------------------------------------------------
  * Set up the RLC downlink registration parameters
  * -----------------------------------------------------------*/
 
  lb_rlc_dl_reg_params.nchan = lb_rab_list[rab_id]->lc_info_ptr.num_dl_lc_ids_for_rab;
  /* Number of downlink logical channels to register */
  lb_rlc_dl_reg_params.nchan = MIN(lb_rlc_dl_reg_params.nchan, RRC_MAX_LC_ID_PER_RAB);
  for (i=0; i<lb_rlc_dl_reg_params.nchan; i++)
  {

    lb_rlc_dl_reg_params.rlc_data[ i ].lc_id = lb_rab_list[rab_id]->lc_info_ptr.dl_lc_id[ i ];
    /* Downlink RLC logical channel ID */

    //lb_rab_list[rab_id].lb_rlc_dl_wm[i].non_empty_func_ptr     = lb_rx_data_from_rlc;
    lb_rab_list[rab_id]->lb_rlc_dl_wm[i].non_empty_func_data    = (void *) lb_get_key(rab_id, i);
    //lb_rab_list[rab_id].lb_rlc_dl_wm[i].each_enqueue_func_ptr  = lb_rx_data_from_rlc;
    lb_rab_list[rab_id]->lb_rlc_dl_wm[i].each_enqueue_func_data = (void *) lb_get_key(rab_id,i);
    MSG_HIGH_DS_3(TC_SUB, "TC: Allocated wm_id = %d for DL LC ID= %d, DL LC ID Index = %d", 
      lb_get_key(rab_id, i), lb_rab_list[rab_id]->lc_info_ptr.dl_lc_id[ i ], i);

    lb_rlc_dl_reg_params.rlc_data[ i ].dl_wm_ptr = &(lb_rab_list[rab_id]->lb_rlc_dl_wm[ i ]);
    /* Pointer to the watermark for storing downlink data */

    lb_rlc_dl_reg_params.rlc_data[ i ].context = FALSE;
    /* Task context */

    lb_rlc_dl_reg_params.rlc_data[ i ].rlc_post_rx_proc_func_ptr = NULL;
    /* Set the RLC callback function to NULL because we specified the
       callback function in the DS/RLC watermark */
  }

  
  // for debugging purpose later disable

  for (i=0; i<lb_rlc_ul_reg_params.nchan; i++)
  {
    MSG_HIGH_DS_1(TC_SUB,"Registering UL  ul_lc_id = %d", lb_rlc_ul_reg_params.rlc_data[ i ].lc_id);
  }
  for (i=0; i<lb_rlc_dl_reg_params.nchan; i++)
  {
    MSG_HIGH_DS_1(TC_SUB,"Registering DL  dl_lc_id = %d", lb_rlc_dl_reg_params.rlc_data[ i ].lc_id);
  }
  
  /* Register the DS/RLC watermarks with RLC */
#ifdef FEATURE_SEGMENT_LOADING
  IWCDMA_rlc_ul_register_srvc_handler( ptr_w, &lb_rlc_ul_reg_params );
  IWCDMA_rlc_dl_register_srvc_handler( ptr_w, &lb_rlc_dl_reg_params );
#else
  rlc_ul_register_srvc_handler( &lb_rlc_ul_reg_params );
  rlc_dl_register_srvc_handler( &lb_rlc_dl_reg_params );
#endif
}

#endif

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION LB_REGISTER_TDSRLC_WM

DESCRIPTION
  This function registers the UL/UL watermarks with the TD-SCDMA RLC for data 
  transfer.
  
DEPENDENCIES
  This function should be called only after a successful RAB establishment.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void lb_register_tdsrlc_wm( byte rab_id )
{
  uint8 i;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_T = NULL;
  
  ptr_T = get_tdscdma_interface();
  if (ptr_T == NULL)
  {     
      ERR_FATAL("Segment Loading Error, interface pointer NULL", 0,0,0);
  }
#endif  
  MSG_HIGH_DS_1(TC_SUB,"TC: Registering RAB Id=%d", rab_id);

  lb_rab_list[rab_id]->tdslc_info_ptr.num_ul_lc_ids_for_rab = 0;
  lb_rab_list[rab_id]->tdslc_info_ptr.num_dl_lc_ids_for_rab = 0;
  lb_rab_list[rab_id]->tdslc_info_ptr.rab_id                = rab_id;
  lb_rab_list[rab_id]->rab_id                               = rab_id;
  
  for ( i = 0; i < RRC_MAX_LC_ID_PER_RAB; i++ )
  {
    lb_rab_list[rab_id]->tdslc_info_ptr.ul_lc_id[ i ] = 0;
    lb_rab_list[rab_id]->tdslc_info_ptr.dl_lc_id[ i ] = 0;
  }
#ifdef FEATURE_SEGMENT_LOADING  
  if (ITDSCDMA_tdsrrc_return_lc_info_for_rab( ptr_T, &(lb_rab_list[rab_id]->tdslc_info_ptr) ) ==
        TDSRRC_RAB_NOT_FOUND)
  {
    ERR( "Invalid RAB id %d", rab_id, 0, 0 ); 
  }
#else
  if (tdsrrc_return_lc_info_for_rab( &(lb_rab_list[rab_id]->tdslc_info_ptr) ) ==
        TDSRRC_RAB_NOT_FOUND)
  {
    ERR( "Invalid RAB id %d", rab_id, 0, 0 ); 
  }
#endif
  
  if ( ( lb_rab_list[rab_id]->tdslc_info_ptr.num_ul_lc_ids_for_rab > 1 ) ||
       ( lb_rab_list[rab_id]->tdslc_info_ptr.num_dl_lc_ids_for_rab > 1 ) )
  {
    MSG_HIGH_DS_2(TC_SUB, "More than 1 LC id: UL %d; DL %d",
         lb_rab_list[rab_id]->tdslc_info_ptr.num_ul_lc_ids_for_rab,
         lb_rab_list[rab_id]->tdslc_info_ptr.num_dl_lc_ids_for_rab
         ); 
  }

  /* ------------------------------------------------------------
  * Set up the RLC uplink registration parameters
  * -----------------------------------------------------------*/
  lb_tdsrlc_ul_reg_params.service = TRUE;  /* TRUE = Register */
  lb_tdsrlc_ul_reg_params.nchan =
    lb_rab_list[rab_id]->tdslc_info_ptr.num_ul_lc_ids_for_rab;
  /* Number of uplink logical channels to register */
  lb_tdsrlc_ul_reg_params.nchan = MIN(lb_tdsrlc_ul_reg_params.nchan,RRC_MAX_LC_ID_PER_RAB);
  for (i=0; i < lb_tdsrlc_ul_reg_params.nchan; i++)
  {

    lb_tdsrlc_ul_reg_params.rlc_id[i] =
      lb_rab_list[rab_id]->tdslc_info_ptr.ul_lc_id[ i ];
       /* Uplink RLC logical channel ID */
                                                         
    lb_tdsrlc_ul_reg_params.ul_wm_ptr[i] =
      &(lb_rab_list[rab_id]->lb_rlc_ul_wm[ i ]);
    /* Pointer to the watermark for storing uplink data */
    MSG_HIGH_DS_2(TC_SUB,"TC: UL LC ID= %d, UL LC ID Index = %d", lb_rab_list[rab_id]->tdslc_info_ptr.ul_lc_id[ i ], i);
  }
  
  /* ------------------------------------------------------------
  * Set up the RLC downlink registration parameters
  * -----------------------------------------------------------*/
  lb_tdsrlc_dl_reg_params.service = TRUE;  /* TRUE = Register */ 
  lb_tdsrlc_dl_reg_params.nchan =
    lb_rab_list[rab_id]->tdslc_info_ptr.num_dl_lc_ids_for_rab;
  /* Number of downlink logical channels to register */
  lb_tdsrlc_dl_reg_params.nchan = MIN(lb_tdsrlc_dl_reg_params.nchan,RRC_MAX_LC_ID_PER_RAB);
  for (i=0; i < lb_tdsrlc_dl_reg_params.nchan; i++)
  {
    lb_tdsrlc_dl_reg_params.rlc_id[i] =
      lb_rab_list[rab_id]->tdslc_info_ptr.dl_lc_id[ i ];
    /* Downlink RLC logical channel ID */

    lb_rab_list[rab_id]->lb_rlc_dl_wm[i].non_empty_func_data    = (void *) lb_get_key(rab_id, i);
    lb_rab_list[rab_id]->lb_rlc_dl_wm[i].each_enqueue_func_data = (void *) lb_get_key(rab_id,i);
    MSG_HIGH_DS_3(TC_SUB, "TC: Allocated wm_id = %d for DL LC ID= %d, DL LC ID Index = %d", 
      lb_get_key(rab_id, i), lb_rab_list[rab_id]->tdslc_info_ptr.dl_lc_id[ i ], i);

    lb_tdsrlc_dl_reg_params.dl_wm_ptr[i] = &(lb_rab_list[rab_id]->lb_rlc_dl_wm[ i ]);
    /* Pointer to the watermark for storing downlink data */

    lb_tdsrlc_dl_reg_params.context[i] = FALSE;
    /* Task context */

    lb_tdsrlc_dl_reg_params.rlc_post_rx_proc_func_ptr[i] = NULL;
    /* Set the RLC callback function to NULL because we specified the
       callback function in the DS/RLC watermark */
  }

  
  // for debugging purpose later disable

  for (i=0; i < lb_tdsrlc_ul_reg_params.nchan; i++)
  {
    MSG_HIGH_DS_1(TC_SUB,"Registering UL  ul_lc_id = %d", lb_tdsrlc_ul_reg_params.rlc_id[i]);
  }
  for (i=0; i < lb_tdsrlc_dl_reg_params.nchan; i++)
  {
    MSG_HIGH_DS_1(TC_SUB,"Registering DL  dl_lc_id = %d", lb_tdsrlc_dl_reg_params.rlc_id[i]);
  }
  
#ifdef FEATURE_SEGMENT_LOADING   
  /* Register the DS/RLC watermarks with RLC */
  (void) ITDSCDMA_tdsl2_ul_service_register( ptr_T, &lb_tdsrlc_ul_reg_params );
  (void) ITDSCDMA_tdsl2_dl_service_register( ptr_T, &lb_tdsrlc_dl_reg_params );
#else
  /* Register the DS/RLC watermarks with RLC */
  (void) tdsl2_ul_service_register( &lb_tdsrlc_ul_reg_params );
  (void) tdsl2_dl_service_register( &lb_tdsrlc_dl_reg_params );
#endif  
}
#endif /* FEATURE_TDSCDMA */

/*===========================================================================
FUNCTION LB_REGISTER_DL_RLC_WM

DESCRIPTION
  This function registers the DL watermarks for a specific LC-IDwith the RLC for data 
  transfer.
  
DEPENDENCIES
  This function should be called only after a successful RAB establishment.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void lb_register_dl_lc_rlc_wm(byte rab_id, uint8 dl_lc_id)
{
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = NULL;

  ptr_W = get_wcdma_interface();
  if (ptr_W == NULL)
  {     
      ERR_FATAL("Segment Loading Error, interface pointer NULL", 0,0,0);
  }
#endif
  lb_rlc_dl_reg_params.nchan = 1;
  
  lb_rlc_dl_reg_params.rlc_data[ 0 ].lc_id = dl_lc_id;
  /* Downlink RLC logical channel ID */

  lb_rab_list[rab_id]->lb_rlc_dl_wm[0].non_empty_func_ptr     = lb_rx_data_from_rlc;
  lb_rab_list[rab_id]->lb_rlc_dl_wm[0].non_empty_func_data    = (void *) lb_get_key(rab_id, 0);
  lb_rab_list[rab_id]->lb_rlc_dl_wm[0].each_enqueue_func_ptr  = lb_rx_data_from_rlc;
  lb_rab_list[rab_id]->lb_rlc_dl_wm[0].each_enqueue_func_data = (void *) lb_get_key(rab_id,0);
  MSG_HIGH_DS_2(TC_SUB, "MBMS: Allocated wm_id = %d, formed from RAB ID=%d, DL LC index=%d",
    lb_get_key(rab_id, 0), rab_id);

  lb_rlc_dl_reg_params.rlc_data[ 0 ].dl_wm_ptr = &(lb_rab_list[rab_id]->lb_rlc_dl_wm[ 0 ]);
  /* Pointer to the watermark for storing downlink data */

  lb_rlc_dl_reg_params.rlc_data[ 0 ].context = FALSE;
  /* Task context */

  lb_rlc_dl_reg_params.rlc_data[ 0 ].rlc_post_rx_proc_func_ptr = NULL;
  /* Set the RLC callback function to NULL because we specified the
     callback function in the DS/RLC watermark */
#ifdef FEATURE_SEGMENT_LOADING
  IWCDMA_rlc_dl_register_srvc_handler( ptr_W, &lb_rlc_dl_reg_params );
#else
  rlc_dl_register_srvc_handler( &lb_rlc_dl_reg_params );
#endif
}


/*===========================================================================
FUNCTION LB_CLEANUP

DESCRIPTION
  This function cleans up the DL/UL variables and data structures.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lb_cleanup( void )
{
  dsm_item_type *dsm_item_ptr = NULL;            /* Pointer to an DSM item */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Reset the DL watermark function pointers.
  -------------------------------------------------------------------------*/  
  uint32 index1, index2;

  for (index1=0; index1<MAX_RAB_TO_SETUP ; index1++)
  {
    if (lb_rab_list[index1] != NULL)
    {
      for (index2=0; index2<UE_MAX_DL_LOGICAL_CHANNEL; index2++)
      {
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].gone_empty_func_ptr = NULL;
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].lowater_func_ptr    = NULL;
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].non_empty_func_ptr  = NULL;
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].hiwater_func_ptr    = NULL;
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].dont_exceed_cnt        = 1;
  
        /*-------------------------------------------------------------------------
          Clear the queue for the watermark and return buffers to the DSM item pool.
        -------------------------------------------------------------------------*/    
        while ( ( dsm_item_ptr = dsm_dequeue( &(lb_rab_list[index1]->lb_rlc_dl_wm[index2]) ) ) != NULL )
        {
          dsm_free_packet( &dsm_item_ptr );
        }
        post_self_cmd_flag[index1][index2] = FALSE;
        /*-------------------------------------------------------------------------
        Set the DL watermark function pointers.
        -------------------------------------------------------------------------*/
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].gone_empty_func_ptr = NULL; 
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].lowater_func_ptr    = NULL;
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].non_empty_func_ptr  = NULL;
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].non_empty_func_data = (void *) index2;
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].hiwater_func_ptr    = NULL;
        lb_rab_list[index1]->lb_rlc_dl_wm[index2].current_cnt         = 0;

        (void)dsm_queue_destroy(&lb_rab_list[index1]->lb_rlc_dl_wm[index2]);
      }
  
        /*-------------------------------------------------------------------------
          Clear the UL watermark function pointers..
        -------------------------------------------------------------------------*/  
      for (index2=0; index2<UE_MAX_UL_LOGICAL_CHANNEL; index2++)
      {
        lb_rab_list[index1]->lb_rlc_ul_wm[index2].gone_empty_func_ptr = NULL;
        lb_rab_list[index1]->lb_rlc_ul_wm[index2].lowater_func_ptr    = NULL;
        lb_rab_list[index1]->lb_rlc_ul_wm[index2].non_empty_func_ptr  = NULL;
        lb_rab_list[index1]->lb_rlc_ul_wm[index2].hiwater_func_ptr    = NULL;
        lb_rab_list[index1]->ul_rlc_sdu_size[index2]                  = 0;
        lb_rab_list[index1]->ul_rlc_sdu_size_valid[index2]            = TRUE;
  
        /*-------------------------------------------------------------------------
          Clear the queue for the watermark and return buffers to the DSM item pool.
        -------------------------------------------------------------------------*/    
        while ( ( dsm_item_ptr = dsm_dequeue( &lb_rab_list[index1]->lb_rlc_ul_wm[index2] ) ) != NULL )
        {
          dsm_free_packet( &dsm_item_ptr );
        }
  
        /*-------------------------------------------------------------------------
        Set the UL watermark function pointers.
        -------------------------------------------------------------------------*/
        lb_rab_list[index1]->lb_rlc_ul_wm[index2].gone_empty_func_ptr = NULL;
        lb_rab_list[index1]->lb_rlc_ul_wm[index2].lowater_func_ptr    = NULL;
        lb_rab_list[index1]->lb_rlc_ul_wm[index2].non_empty_func_ptr  = NULL;
        lb_rab_list[index1]->lb_rlc_ul_wm[index2].hiwater_func_ptr    = NULL;
        lb_rab_list[index1]->lb_rlc_ul_wm[index2].current_cnt         = 0;   

        (void)dsm_queue_destroy(&lb_rab_list[index1]->lb_rlc_ul_wm[index2]);
      }
  
      #ifdef FEATURE_MODEM_HEAP       
      modem_mem_free(lb_rab_list[index1], MODEM_MEM_CLIENT_NAS);
      #endif
    
      lb_rab_list[index1] = NULL;  
    }

  }
}

/*===========================================================================
FUNCTION LB_RAB_CLEANUP

DESCRIPTION
  This function cleans up the DL/UL variables and data structures.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lb_rab_cleanup( byte rab_id )
{
  dsm_item_type *dsm_item_ptr = NULL;            /* Pointer to an DSM item */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Reset the DL watermark function pointers.
  -------------------------------------------------------------------------*/  
  uint32 index1;

    for (index1=0; index1<UE_MAX_DL_LOGICAL_CHANNEL; index1++)
    {
      /*-------------------------------------------------------------------------
        Clear the queue for the watermark and return buffers to the DSM item pool.
      -------------------------------------------------------------------------*/    
      while ( ( dsm_item_ptr = dsm_dequeue( &(lb_rab_list[rab_id]->lb_rlc_dl_wm[index1]) ) ) != NULL )
      {
        dsm_free_packet( &dsm_item_ptr );
      }
      post_self_cmd_flag[rab_id][index1] = FALSE;
      (void)dsm_queue_destroy(&lb_rab_list[rab_id]->lb_rlc_dl_wm[index1]);
    }

      /*-------------------------------------------------------------------------
        Clear the UL watermark function pointers..
      -------------------------------------------------------------------------*/  
    for (index1=0; index1<UE_MAX_UL_LOGICAL_CHANNEL; index1++)
    {
      /*-------------------------------------------------------------------------
        Clear the queue for the watermark and return buffers to the DSM item pool.
      -------------------------------------------------------------------------*/    
      while ( ( dsm_item_ptr = dsm_dequeue( &lb_rab_list[rab_id]->lb_rlc_ul_wm[index1] ) ) != NULL )
      {
        dsm_free_packet( &dsm_item_ptr );
      }
      (void)dsm_queue_destroy(&lb_rab_list[rab_id]->lb_rlc_ul_wm[index1]);
    }

    #ifdef FEATURE_MODEM_HEAP       
    modem_mem_free(lb_rab_list[rab_id], MODEM_MEM_CLIENT_NAS);
    #endif
  
    lb_rab_list[rab_id] = NULL;  

} /* LB_cleanup() */

/*===========================================================================

FUNCTION LB_RX_DATA_FROM_RLC

DESCRIPTION
  This function sends a command to the TC task, which tells the TC task to
  receive data from RLC.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lb_rx_data_from_rlc(   
  struct dsm_watermark_type_s *wm,
  /*lint -esym(715,wm) */
  void* callback_data
)
{
  tc_cmd_type *cmd_ptr = NULL;                 /* Pointer to a command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  uint32 wm_id = (uint32) callback_data;

  uint32 rab_id  = lb_get_rab_id_from_key (wm_id);
  uint32 lc_id   = lb_get_chan_id_from_key (wm_id);

  if (post_self_cmd_flag[rab_id][lc_id] == TRUE)
  {
#ifdef FEATURE_DEBUG_TC
    MSG_HIGH_DS_1(TC_SUB, "LB_PROC_DL_RLC_DATA_CMD already in Queue for wm-id=%d", wm_id); //TC_PENDING to removet this print stmt
#endif
    return; // This Cmd is already in Queue
  }
  else
  {
    MSG_HIGH_DS_1(TC_SUB, "LB_PROC_DL_RLC_DATA_CMD posting Cmd to Queue for wm-id=%d", wm_id); //TC_PENDING to removet this print stmt
    post_self_cmd_flag[rab_id][lc_id] = TRUE;
  }

  MSG_HIGH_DS_1(TC_SUB, "LB_PROC_DL_RLC_DATA_CMD posting Cmd to Queue for wm-id=%d", wm_id); //TC_PENDING to removet this print stmt
  /*-----------------------------------------------------------------------
    Get a TC task command buffer and send a command to the TC task.
    Note: the LB_PROC_DL_RLC_DATA_CMD is used for task context switch.
  -----------------------------------------------------------------------*/      
  if (( cmd_ptr = (tc_cmd_type *) tc_get_cmd_buf() ) != NULL )
  {
    // Send internal command LB_PROC_DL_RLC_DATA_CMD to TC task
    cmd_ptr->header.message_set = MS_TC_TC;
    cmd_ptr->header.cmd_id = LB_PROC_DL_RLC_DATA_CMD;
    cmd_ptr->cmd.lb_proc_dl_rlc_data_ind.wm_id = wm_id;

    tc_put_cmd( cmd_ptr );
  }
  else
  {
    ERR_FATAL( "No TC command buffer", 0, 0, 0 );
  }
} /* lb_rx_data_from_rlc() */

/*===========================================================================

FUNCTION LB_MODE1_RX_HANDLER

DESCRIPTION
  This function is called by the TC task for processing the received RLC
  data in test loop mode 1.
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_TC_HSPA_OPTIMIZATION

void lb_mode1_rx_handler (uint32 wm_key)
{
  uint16 ul_max_sdu_size_bytes, padded_bytes=0, i, j;
  uint32 rab_id, wm_id;
  uint8 *dsm_data = NULL; 
  dsm_item_type *dsm_item_ptr = NULL; // Pointer to a DSM item from the WM
  dsm_item_type *temp_dup_ptr, *head_dup_ptr, *last_dup_ptr;
  dsm_item_type *dsm_previous_ptr, *dsm_previous_ptr_itr;
  #ifdef FEATURE_DEBUG_TC
  dsm_item_type *dsm_item_ptr_print;
  #endif
#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
  uint32 app_ptr;
#endif
  uint16 num_dsm_dup_required=0, num_mem_copies=1, total_dsm_data_length;
#ifdef FEATURE_TC_DEBUG_DUMP_DL_UL_SDU
  #error code not present
#endif

  rab_id = lb_get_rab_id_from_key(wm_key);
  wm_id = lb_get_chan_id_from_key(wm_key)%RRC_MAX_LC_ID_PER_RAB;

  // No processing if UL RLC SDU size is zero, just empty the watermark
  if (lb_rab_list[rab_id]->ul_rlc_sdu_size_valid[wm_id] == FALSE)
  {
    MSG_HIGH_DS_2(TC_SUB, "TC: LB1 Discarding data from wm id %d as UL RLC SDU is 0, Q_cnt %d ", 
               wm_key, q_cnt(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id].q_ptr));
    while ((dsm_item_ptr = dsm_dequeue(&lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id])) != NULL)
    {
      dsm_free_packet(&dsm_item_ptr);
    }
    post_self_cmd_flag[rab_id][wm_id] = FALSE;
    return;  
  }

  ul_max_sdu_size_bytes = ((lb_rab_list[rab_id]->ul_rlc_sdu_size[wm_id] + 7) >> 3);
  dsm_item_len = 0;           // Length of DSM item payload 

  dsm_item_ptr = dsm_dequeue(&(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id]));
  if (dsm_item_ptr == NULL)
  {
    MSG_ERROR_DS_3(TC_SUB, "TC: LB1 No DSM item on RLC/DL wm_id %d, RAB %d, DL LC index %d",
               wm_key, rab_id, wm_id);
    post_self_cmd_flag[rab_id][wm_id] = FALSE;
    return;  
  }
  
  MSG_HIGH_DS_3(TC_SUB, "TC: LB1 Handler for wm id %d, RAB %d, Q_cnt %d", 
           wm_key, rab_id, q_cnt(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id].q_ptr));
  
  while (dsm_item_ptr != NULL)
  {
    dsm_item_len = (uint16) dsm_length_packet(dsm_item_ptr);

#ifdef FEATURE_DEBUG_TC
    MSG_MED_DS_2(TC_SUB, "TC: DL SDU size %d, UL SDU size %d", 
             dsm_item_len, ul_max_sdu_size_bytes);
#endif
    
#ifdef FEATURE_TC_DEBUG_DUMP_DL_UL_SDU
#error code not present
#endif

    if ((ul_max_sdu_size_bytes == 0) ||
        (ul_max_sdu_size_bytes == dsm_item_len ) ||
        (dsm_item_len > ul_max_sdu_size_bytes) )
    {
      temp_dup_ptr = NULL;
      if ((dsm_dup_packet(&temp_dup_ptr, dsm_item_ptr, 0, dsm_item_len)) != dsm_item_len)
      {
        ERR_FATAL("TC: Not able to DUP requested length %d", dsm_item_len, 0, 0);
      }

      #ifdef FEATURE_TC_CONSTANT_DELAY_ENH
      if(temp_dup_ptr == NULL)
      {
        ERR_FATAL(" Not able to DUP of dsm item %x", dsm_item_ptr, 0, 0);
      }
      temp_dup_ptr->app_ptr = dsm_item_ptr->app_ptr;
      #endif
      /* Free the original DSM Pointer received in DL WM Q 
       * Assign the dup ptr to dsm_item_ptr
       */
      dsm_free_packet(&dsm_item_ptr);
      dsm_item_ptr = temp_dup_ptr;
      // app_field is used only in TM Mode
      if ((tc_rat_rab_established_on == SYS_RAT_UMTS_RADIO_ACCESS) &&
          (lb_rab_list[rab_id]->lc_info_ptr.ul_lc_id[ wm_id ] > 7)) 
      {
        dsm_item_ptr->app_field = 0;
      } 
#ifdef FEATURE_TDSCDMA
      else if ((tc_rat_rab_established_on == SYS_RAT_TDS_RADIO_ACCESS) &&
          (lb_rab_list[rab_id]->tdslc_info_ptr.ul_lc_id[ wm_id ] > 7)) 
      {
        dsm_item_ptr->app_field = 0;
      } 
#endif /* FEATURE_TDSCDMA */
      else
      {
        dsm_item_ptr->app_field = lb_rab_list[rab_id]->ul_rlc_sdu_size[wm_id];
      }      
      if ((ul_max_sdu_size_bytes == 0) ||
          (ul_max_sdu_size_bytes == dsm_item_len ) )
      {
        /* 
         * If the UL and DL SDU Sizes are same 
         * -- enqueue the same DSM rcvd in DL to UL Queue
         * If the UL SDU size is not received, then as per spec UL SDU Size is assumed as DL SDU Size
         * -- Again enqueue the same DL DSM item in UL Queue
         */
#ifdef FEATURE_TC_DEBUG_DUMP_DL_UL_SDU
          #error code not present
#endif

        lb_tx_data_to_rlc(wm_key, dsm_item_ptr);
      } else if (dsm_item_len > ul_max_sdu_size_bytes)
      {
        /* Per Table A.7.1 of TS34.109, truncate the SDU to UL SDU size */
  #ifdef FEATURE_DEBUG_TC
        MSG_HIGH_DS_0(TC_SUB,"Trimming bytes ");
  #endif
        dsm_trim_packet (&dsm_item_ptr, ul_max_sdu_size_bytes) ;

#ifdef FEATURE_TC_DEBUG_DUMP_DL_UL_SDU
        #error code not present
#endif

        lb_tx_data_to_rlc(wm_key, dsm_item_ptr);
      }
    }
    else if (dsm_item_len < ul_max_sdu_size_bytes) 
    {
      num_dsm_dup_required = ul_max_sdu_size_bytes/dsm_item_len;
      num_mem_copies = 1;
      while (num_dsm_dup_required > THRESHOLD_TC_DSM_DUP)
      {
          num_mem_copies = (uint16)(num_mem_copies * 2);
          num_dsm_dup_required = (uint16)(num_dsm_dup_required >> 1);
      }

      total_dsm_data_length = (uint16)(dsm_item_len*num_mem_copies);
      padded_bytes = (uint16)(ul_max_sdu_size_bytes -(total_dsm_data_length * num_dsm_dup_required) );
      
#ifdef FEATURE_MODEM_HEAP      
          dsm_data = modem_mem_alloc(total_dsm_data_length * sizeof(uint8), MODEM_MEM_CLIENT_NAS);
#else
          dsm_data = mem_malloc (&tmc_heap, total_dsm_data_length * sizeof (uint8));
#endif
      if (dsm_data != NULL)
      {
        #ifdef FEATURE_TC_CONSTANT_DELAY_ENH
        app_ptr = (uint32) dsm_item_ptr->app_ptr;
        #endif
        // memset((void*)dsm_data, 0, total_dsm_data_length);
        if ( dsm_pullup( &dsm_item_ptr, dsm_data, dsm_item_len ) != dsm_item_len )
        {
          ERR( "Invalid DL RLC SDU", 0, 0, 0);
        }
        for (j=1 ; num_mem_copies!=1 ; j=j*2)
        {
          memscpy (dsm_data+dsm_item_len*j, (total_dsm_data_length - j*dsm_item_len), dsm_data, dsm_item_len*j);
          num_mem_copies = num_mem_copies >> 1;
        }
        dsm_item_ptr = dsm_sdu_to_pkt_chain_bit( dsm_data, (uint16)(total_dsm_data_length * 8));
        if(dsm_item_ptr == NULL)
        {
          ERR("Unable allocate memory for dsm_item_ptr",0,0,0);
          if (dsm_data != NULL)
#ifdef FEATURE_MODEM_HEAP      
            modem_mem_free(dsm_data, MODEM_MEM_CLIENT_NAS);
#else
            mem_free (&(tmc_heap), dsm_data);
#endif
          return;
        }

        #ifdef FEATURE_TC_CONSTANT_DELAY_ENH
        dsm_item_ptr->app_ptr = (void *) app_ptr;
        #endif

        // app_field is used only in TM Mode
        if ((tc_rat_rab_established_on == SYS_RAT_UMTS_RADIO_ACCESS) &&
            (lb_rab_list[rab_id]->lc_info_ptr.ul_lc_id[ wm_id ] > 7)) 
        {
          dsm_item_ptr->app_field = 0;
        } 
#ifdef FEATURE_TDSCDMA
        else if ((tc_rat_rab_established_on == SYS_RAT_TDS_RADIO_ACCESS) &&
            (lb_rab_list[rab_id]->tdslc_info_ptr.ul_lc_id[ wm_id ] > 7)) 
        {
          dsm_item_ptr->app_field = 0;
        } 
#endif /* FEATURE_TDSCDMA */
        else
        {
          dsm_item_ptr->app_field = lb_rab_list[rab_id]->ul_rlc_sdu_size[wm_id];
        }      
      }
      else
      {
        ERR_FATAL( "TC: Cannot allocate Memory", 0, 0, 0);
      }

      if (dsm_data != NULL)
      {
#ifdef FEATURE_MODEM_HEAP      
            modem_mem_free(dsm_data, MODEM_MEM_CLIENT_NAS);
#else
            mem_free (&(tmc_heap), dsm_data);
#endif
      }

      /* Dup the original DSM item by sep_repeat_count number of times */
      head_dup_ptr = dsm_item_ptr;

      dsm_previous_ptr = dsm_item_ptr;
      dsm_previous_ptr_itr = dsm_item_ptr;
      
      for (; dsm_previous_ptr_itr->pkt_ptr != NULL ; dsm_previous_ptr_itr=dsm_previous_ptr_itr->pkt_ptr)
      {
        dsm_previous_ptr = dsm_previous_ptr_itr->pkt_ptr;
      }
      last_dup_ptr = dsm_previous_ptr;

#ifdef FEATURE_DEBUG_TC
      dsm_item_ptr_print = dsm_item_ptr;
      for (; dsm_item_ptr_print != NULL; dsm_item_ptr_print=dsm_item_ptr_print->pkt_ptr)
      {
        MSG_HIGH_DS_2(TC_SUB,"TC: DL Data size %d, Used %d", dsm_item_ptr_print->size, dsm_item_ptr_print->used);
      }
#endif

      for (i = 1; i < num_dsm_dup_required; i++)
      {
#ifdef FEATURE_DEBUG_TC
         MSG_HIGH_DS_2(TC_SUB,"TC: Duping item counter %d, count: %d", i, current_reference_count);
#endif
        temp_dup_ptr = NULL;
        if ((dsm_dup_packet(&temp_dup_ptr, dsm_item_ptr, 0, total_dsm_data_length)) != total_dsm_data_length)
        {
          ERR_FATAL("TC_ERR: Not able to DUP requested length %d", total_dsm_data_length, 0, 0);
        }

        if(temp_dup_ptr == NULL)
        {
          ERR("Unable allocate memory for temp_dup_ptr",0,0,0);
          return;
        }

        last_dup_ptr->pkt_ptr = temp_dup_ptr;
        dsm_previous_ptr = temp_dup_ptr;
        dsm_previous_ptr_itr = temp_dup_ptr;
        for (; dsm_previous_ptr_itr->pkt_ptr != NULL ; dsm_previous_ptr_itr=dsm_previous_ptr_itr->pkt_ptr)
        {
          dsm_previous_ptr = dsm_previous_ptr_itr->pkt_ptr;
        }
        last_dup_ptr = dsm_previous_ptr;
      }
      
      /* Dup the original item by padded_bytes amount of data and chain to the last_dup_ptr */
      if (padded_bytes > 0)
      {
        temp_dup_ptr = NULL;
#ifdef FEATURE_DEBUG_TC
        MSG_HIGH_DS_2(TC_SUB,"TC: Padded bytes %d, count: %d", padded_bytes, current_reference_count);
#endif
        if ((dsm_dup_packet(&temp_dup_ptr, dsm_item_ptr, 0, padded_bytes)) != padded_bytes)
        {
          ERR_FATAL("TC: Not able to DUP requested length %d", padded_bytes, 0, 0);
        }
        last_dup_ptr->pkt_ptr = temp_dup_ptr;
      }
      
      if (head_dup_ptr != NULL)
      {
#ifdef FEATURE_TC_DEBUG_DUMP_DL_UL_SDU
          #error code not present
#endif

        lb_tx_data_to_rlc(wm_key, head_dup_ptr);
      }
    }
    
    /* Get the next entry from the DL WM Queue */
    dsm_item_ptr = dsm_dequeue(&lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id]);
  }
  
  post_self_cmd_flag[rab_id][wm_id] = FALSE;
  
} /* lb_mode1_rx_handler() */

#else

void lb_mode1_rx_handler (uint32 wm_key)
{
  uint32 i = 0;
  uint32 j = 0;
  dsm_item_type *dsm_item_ptr = NULL; // Pointer to a DSM item from the WM
  uint8 *dsm_data = NULL; 
  uint16 seq_repeat_count = 1;
  uint32 padded_bytes = 0;
  uint16 max_sdu_size; 
  uint8 *lb_from_rlc_data = NULL; 
  // Place holder for loopback transmit item size in bits
  uint16 lb_tx_item_size = 0;
  uint32 rab_id  = lb_get_rab_id_from_key (wm_key);
  uint32 wm_id   = lb_get_chan_id_from_key (wm_key)%RRC_MAX_LC_ID_PER_RAB;

#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
  uint32 app_ptr;
#endif

  max_sdu_size = (lb_rab_list[rab_id]->ul_rlc_sdu_size[wm_id]+ 7) >> 3;

  MSG_HIGH_DS_3(TC_SUB, "TC: Mode1 Handler for wm_id = %d, for RAB = %d, DL LC index=%d",wm_key,rab_id,wm_id);

  // No processing if UL RLC SDU size is zero, just empty the watermark
  if (lb_rab_list[rab_id]->ul_rlc_sdu_size_valid[wm_id] == FALSE)
  {
    // Retrieve the data from DSM WM 
    MSG_HIGH_DS_2(TC_SUB,"Emptying data from wm_id = %d for RAB = %d because RLC SDU size = 0",wm_id,rab_id);
    for (dsm_item_ptr = dsm_dequeue( &lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id]);
         dsm_item_ptr;
         dsm_item_ptr = dsm_dequeue(&(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id])) )
    {
      dsm_free_packet( &dsm_item_ptr );
    }
    post_self_cmd_flag[rab_id][wm_id] = FALSE;
    return;  
  }

  dsm_item_len = 0;           // Length of DSM item payload 

  MSG_HIGH_DS_1(TC_SUB, "LB DL: processing DL data in test loop mode 1, UL RLC SDU size: %d", lb_rab_list[rab_id]->ul_rlc_sdu_size[wm_id]);

  /*-------------------------------------------------------------------------
  Get a DSM item from the RLC WM.
  -------------------------------------------------------------------------*/
  dsm_item_ptr = dsm_dequeue( &lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id] );

  if ( dsm_item_ptr == NULL )
  {
    MSG_HIGH_DS_0(TC_SUB, "TC: No DSM item on RLC/DL WM?");
  }

  /*-------------------------------------------------------------------------
  Retrieve all the data on the RLC/DS WM, and process for Loopback.
  -------------------------------------------------------------------------*/
  while ( dsm_item_ptr != NULL )
  {
    seq_repeat_count = 1;
    padded_bytes = 0;

#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
    app_ptr = (uint32) dsm_item_ptr->app_ptr;
#endif
    /*---------------------------------------------------------------------
    Copy the data in the DSM item into the array dswcsd_from_rlc_data.
    --------------------------------------------------------------------*/
    dsm_item_len = (uint16) dsm_length_packet( dsm_item_ptr );
    if (lb_rab_list[rab_id]->ul_rlc_sdu_size[wm_id] == 0)
    {
      max_sdu_size = dsm_item_len;
      MSG_MED_DS_2(TC_SUB, "TC: Data Received/Loopback for RB IE missing in CLOSE_UE_TEST_LOOP for rabid=%d, lcid=%d", rab_id, wm_id);
    }

    if (dsm_item_len > max_sdu_size )
    {
       MSG_MED_DS_3(TC_SUB, "Truncated DL sdu = %d to match UL sdu = %d, wm_id = %d", 
                 dsm_item_len, max_sdu_size, wm_id);
    }
    else if (dsm_item_len < max_sdu_size) /* Per Table A.7.1 of TS34.109, truncate the SDU to UL SDU size */
    {
       seq_repeat_count = max_sdu_size/dsm_item_len;
       /* lint -e734 */
       padded_bytes = max_sdu_size % (dsm_item_len*seq_repeat_count);
       /* lint +e734 */
    }

	MSG_LOW_DS_4(TC_SUB,"dl sdu sz = %d  ul sdu sz = %d seq repeat = %d extra = %d", dsm_item_len, max_sdu_size, seq_repeat_count, padded_bytes);
	
    /* Allocate enough memory from tmc heap */
#ifdef FEATURE_MODEM_HEAP      
    lb_from_rlc_data = (uint8*) modem_mem_alloc(max_sdu_size * sizeof(uint8), MODEM_MEM_CLIENT_NAS);
    dsm_data = (uint8*) modem_mem_alloc(dsm_item_len * sizeof(uint8), MODEM_MEM_CLIENT_NAS);
#else
    lb_from_rlc_data = (uint8*) mem_malloc (&tmc_heap, max_sdu_size * sizeof (uint8));
    dsm_data = (uint8*) mem_malloc (&tmc_heap, dsm_item_len * sizeof (uint8));
#endif

    if ( lb_from_rlc_data != NULL && dsm_data != NULL)
    {
      memset((void*)lb_from_rlc_data, 0, max_sdu_size);
      memset((void*)dsm_data, 0, dsm_item_len);

      if ( dsm_pullup( &dsm_item_ptr, dsm_data, dsm_item_len ) != dsm_item_len )
      {
        ERR( "Invalid DL RLC SDU", 0, 0, 0);
      }
      else if (dsm_item_len <= max_sdu_size)
      {
        for (i=0; i<seq_repeat_count; i++)
        {
          memscpy((lb_from_rlc_data + dsm_item_len*i), (max_sdu_size - i*dsm_item_len), dsm_data, dsm_item_len);
        }
      }
      else
      {
        memscpy(lb_from_rlc_data, max_sdu_size, dsm_data, max_sdu_size);
      }

      
      if (dsm_item_len > 2)
      {
        MSG_MED_DS_3(TC_SUB, "rx data = %d, %d, %d", lb_from_rlc_data[0], lb_from_rlc_data[1], lb_from_rlc_data[2]);
      }

      for (i=padded_bytes, j=0; j<padded_bytes; i--)
      {
         lb_from_rlc_data[ max_sdu_size - i ] = lb_from_rlc_data[j++];
      }
     
      /*---------------------------------------------------------------------
      Get a DSM item from the RLC/DS WM.
      ---------------------------------------------------------------------*/
      dsm_item_ptr = dsm_dequeue(&(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id]));

      // Set the loopback transmit item length in bits
      lb_tx_item_size = (word) lb_rab_list[rab_id]->ul_rlc_sdu_size[wm_id];
      if (lb_rab_list[rab_id]->ul_rlc_sdu_size[wm_id] == 0)
      {
        lb_tx_item_size = (word) (dsm_item_len * 8); // Convert dsm_item_len to bits this is only for SDUs of size ZERO
      }


      // Loopback the data
#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
      lb_tx_data_to_rlc(wm_key, lb_from_rlc_data, lb_tx_item_size, app_ptr);
#else
      lb_tx_data_to_rlc(wm_key, lb_from_rlc_data, lb_tx_item_size);
#endif
 
      // Return the allocated memory to tmc heap */
#ifdef FEATURE_MODEM_HEAP      
      modem_mem_free(lb_from_rlc_data, MODEM_MEM_CLIENT_NAS);
      modem_mem_free(dsm_data, MODEM_MEM_CLIENT_NAS);
#else
      mem_free (&(tmc_heap), lb_from_rlc_data);
      mem_free (&(tmc_heap), dsm_data);
#endif
    }
    else
    {
      ERR ("Failed to allocate memory - discarding data", 0, 0, 0);
      if (lb_from_rlc_data != NULL)
      {
#ifdef FEATURE_MODEM_HEAP      
        modem_mem_free(lb_from_rlc_data, MODEM_MEM_CLIENT_NAS);
#else
        mem_free (&(tmc_heap), lb_from_rlc_data);
#endif
      }
      if (dsm_data != NULL)
      {
#ifdef FEATURE_MODEM_HEAP      
        modem_mem_free(dsm_data, MODEM_MEM_CLIENT_NAS);
#else
        mem_free (&(tmc_heap), dsm_data);
#endif
      }
    }
  }
  post_self_cmd_flag[rab_id][wm_id] = FALSE;

} /* lb_mode1_rx_handler() */
            
#endif
/*===========================================================================

FUNCTION LB_MODE2_RX_HANDLER

DESCRIPTION
  This function is called by the TC task for processing the received RLC
  data in test loop mode 2.
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_TC_HSPA_OPTIMIZATION

void lb_mode2_rx_handler (uint32 wm_key)
{
  uint16 lb_tx_item_size = 0;
  dsm_item_type *temp_dup_ptr, *dsm_item_ptr = NULL; 
  uint32 rab_id  = lb_get_rab_id_from_key (wm_key);
  uint32 wm_id   = lb_get_chan_id_from_key (wm_key)%RRC_MAX_LC_ID_PER_RAB;
#ifdef FEATURE_TC_DEBUG_DUMP_DL_UL_SDU
  #error code not present
#endif

  MSG_HIGH_DS_3(TC_SUB, "TC: LB2 Handler for wm_id %d, RAB %d, Q_cnt %d", 
                     wm_key,rab_id,q_cnt(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id].q_ptr));

  /*-------------------------------------------------------------------------
  Get a DSM item from the RLC WM.
  -------------------------------------------------------------------------*/
  dsm_item_ptr = dsm_dequeue( &(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id]) );

  if ( dsm_item_ptr == NULL )
  {
    MSG_HIGH_DS_0(TC_SUB, "TC: LB2, No DSM item on RLC/DS WM");
  }

  /*-------------------------------------------------------------------------
  Retrieve all the data on the RLC/DS WM, and process for Loopback.
  -------------------------------------------------------------------------*/
  while ( dsm_item_ptr != NULL )
  {
    dsm_item_len = (uint16) dsm_length_packet(dsm_item_ptr);
    lb_tx_item_size = dsm_bit_length_packet(dsm_item_ptr);
    
#ifdef FEATURE_TC_DEBUG_DUMP_DL_UL_SDU
    #error code not present
#endif


    temp_dup_ptr = NULL;
    if ((dsm_dup_packet(&temp_dup_ptr, dsm_item_ptr, 0, dsm_item_len)) != dsm_item_len)
    {
      ERR_FATAL("TC: Not able to DUP requested length %d", dsm_item_len, 0, 0);
    }
    if(temp_dup_ptr == NULL)
    {
      ERR("Unable allocate memory for temp_dup_ptr",0,0,0);
      return;
    }
    #ifdef FEATURE_TC_CONSTANT_DELAY_ENH
    temp_dup_ptr->app_ptr = dsm_item_ptr->app_ptr;
    #endif
    /* Free the original DSM Pointer received in DL WM Q 
     * Assign the dup ptr to dsm_item_ptr
     */
    dsm_free_packet(&dsm_item_ptr);
    dsm_item_ptr = temp_dup_ptr;

      /* Update app_field with UL RLC SDU size. app_field holds value in bits
      * This is used ONLY in TM mode. RLC ignores this values for other modes.
      */
    dsm_item_ptr->app_field = lb_tx_item_size;

    MSG_HIGH_DS_2(TC_SUB,"TC: dsm_item_len = %d, app_field=%d ", dsm_item_len, dsm_item_ptr->app_field);

      // Loopback the data
      lb_tx_data_to_rlc(wm_key, dsm_item_ptr);

      /*---------------------------------------------------------------------
      Get a DSM item from the RLC/DS WM.
      ---------------------------------------------------------------------*/
      dsm_item_ptr = dsm_dequeue(&(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id]));
  }

  post_self_cmd_flag[rab_id][wm_id] = FALSE;
} /* lb_mode2_rx_handler() */

#else

void lb_mode2_rx_handler (uint32 wm_key)
{
  dsm_item_type *dsm_item_ptr = NULL; 
  uint8 *lb_from_rlc_data = NULL; 
  // Place holder for loopback transmit item size in bits
  uint16 lb_tx_item_size = 0;
  uint32 rab_id  = lb_get_rab_id_from_key (wm_key);
  uint32 wm_id   = lb_get_chan_id_from_key (wm_key)%RRC_MAX_LC_ID_PER_RAB;

#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
  uint32 app_ptr;
#endif
  MSG_HIGH_DS_3(TC_SUB, "TC: processing DL data, Mode2 Handler for wm_id = %d for RAB = %d DL LC Index=%d",wm_key,rab_id,wm_id);
  
  /*-------------------------------------------------------------------------
  Get a DSM item from the RLC WM.
  -------------------------------------------------------------------------*/
  dsm_item_ptr = dsm_dequeue( &(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id]) );

  if ( dsm_item_ptr == NULL )
  {
    MSG_HIGH_DS_0(TC_SUB, "No DSM item on RLC/DS WM?");
  }

  /*-------------------------------------------------------------------------
  Retrieve all the data on the RLC/DS WM, and process for Loopback.
  -------------------------------------------------------------------------*/
  while ( dsm_item_ptr != NULL )
  {
    /*---------------------------------------------------------------------
    Copy the data in the DSM item into the array lb_from_rlc_data
    --------------------------------------------------------------------*/
    dsm_item_len = (uint16) dsm_length_packet( dsm_item_ptr );

#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
    app_ptr = (uint32) dsm_item_ptr->app_ptr;
#endif
    // Set the loopback transmit item length in bits
    lb_tx_item_size = dsm_bit_length_packet(dsm_item_ptr);

    /* Allocate enough memory from tmc heap */
#ifdef FEATURE_MODEM_HEAP      
    lb_from_rlc_data = (uint8*) modem_mem_alloc(dsm_item_len * sizeof(uint8), MODEM_MEM_CLIENT_NAS);
#else
    lb_from_rlc_data = (uint8*) mem_malloc (&tmc_heap, dsm_item_len * sizeof (uint8));
#endif

    if ( lb_from_rlc_data != NULL )
    {
      memset((void*)lb_from_rlc_data, 0, dsm_item_len);

      if ( dsm_pullup( &dsm_item_ptr, lb_from_rlc_data, dsm_item_len ) != dsm_item_len )
      {
        ERR( "Invalid DL RLC SDU", 0, 0, 0);
      }
      else
      {
        if (dsm_item_len > 2)
        {
           MSG_MED_DS_3(TC_SUB, "rx data = %d, %d, %d", lb_from_rlc_data[0], lb_from_rlc_data[1], lb_from_rlc_data[2]);
        }
      }

      /*---------------------------------------------------------------------
      Get a DSM item from the RLC/DS WM.
      ---------------------------------------------------------------------*/
      dsm_item_ptr = dsm_dequeue(&(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id]));

      // Loopback the data
#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
      lb_tx_data_to_rlc(wm_key, lb_from_rlc_data, lb_tx_item_size, app_ptr);
#else
      lb_tx_data_to_rlc(wm_key, lb_from_rlc_data, lb_tx_item_size);
#endif

      // Return the allocated memory to tmc heap */
#ifdef FEATURE_MODEM_HEAP      
      modem_mem_free(lb_from_rlc_data, MODEM_MEM_CLIENT_NAS);
#else
      mem_free (&(tmc_heap), lb_from_rlc_data);
#endif
    }
    else
    {
      ERR ("Failed to allocate memory - discarding data", 0, 0, 0);
    }
  }

  post_self_cmd_flag[rab_id][wm_id] = FALSE;
} /* lb_mode2_rx_handler() */

#endif

#ifdef FEATURE_MBMS
#error code not present
#endif

/*===========================================================================

FUNCTION LB_RX_PROCESS_DATA

DESCRIPTION
  This function is called by the TC task for processing the received RLC
  data.
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lb_rx_process_data( uint32 wm_key)
{
   dsm_item_type *dsm_item_ptr = NULL; /* Pointer to a DSM item sent to RLC */
   uint32 rab_id  = lb_get_rab_id_from_key (wm_key);
   uint32 wm_id   = lb_get_chan_id_from_key (wm_key);

   if (lb_rab_list[rab_id] == NULL)
   {
      ERR("lb_rab_list for %d is NULL ", rab_id, 0, 0);
      post_self_cmd_flag[rab_id][wm_id] = FALSE;
   }
   else if (wm_id >= UE_MAX_DL_LOGICAL_CHANNEL)
   {
     ERR( "Invalid DL WM Key = %d, watermark ID = %d, mapped for RAB ID = %d", wm_key, wm_id, rab_id);
   }
   else if (is_tc_test_loop_closed() == FALSE)
   {
      ERR("UE test loop not closed, discarding data", 0, 0, 0);
      for (dsm_item_ptr = dsm_dequeue( &lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id]);
           dsm_item_ptr;
           dsm_item_ptr = dsm_dequeue(&(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id])) )
      {
        dsm_free_packet( &dsm_item_ptr );
      }
      post_self_cmd_flag[rab_id][wm_id] = FALSE;
   }
   else if (tc_current_state == TC_LOOPBACK_MODE1)
   {
      lb_mode1_rx_handler(wm_key);
   }   
   else if (tc_current_state == TC_LOOPBACK_MODE2)
   {
      lb_mode2_rx_handler(wm_key);
   }   
#ifdef FEATURE_MBMS
   #error code not present
#endif
   else 
   {
      ERR("Unknown UE mode : %d", (byte)tc_current_state, 0, 0);
      if (lb_rab_list[rab_id] != NULL)
      {
        for (dsm_item_ptr = dsm_dequeue( &lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id]);
             dsm_item_ptr;
             dsm_item_ptr = dsm_dequeue(&(lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id])) )
        {
          dsm_free_packet( &dsm_item_ptr );
        }
      }
      post_self_cmd_flag[rab_id][wm_id] = FALSE;
   }
} /* lb_rx_process_data() */

/*===========================================================================

FUNCTION LB_TX_DATA_TO_RLC

DESCRIPTION
  This function sends UL data to RLC.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_TC_HSPA_OPTIMIZATION

void lb_tx_data_to_rlc(uint32 wm_key, dsm_item_type *dsm_item_ptr)
{
  uint32 rab_id  = lb_get_rab_id_from_key (wm_key);
  uint32 wm_id   = lb_get_chan_id_from_key (wm_key);
#ifdef FEATURE_DEBUG_TC
  uint16 dsm_item_length = (uint16) dsm_length_packet(dsm_item_ptr);
#endif
  
  /*-------------------------------------------------------------------------
  Convert the UL data into a chained DSM item and queue it on
  the DS/RLC watermark.
  -------------------------------------------------------------------------*/  

  if ( dsm_item_ptr == NULL ) 
  {
    ERR( "TC: Empty UL packet chain?", 0, 0, 0 );
  }
  else
  {
#ifdef FEATURE_DEBUG_TC
    MSG_HIGH_DS_2(TC_SUB, "TC: Tx UL data wm_key %d, DSM item length %d", 
                        wm_key, dsm_item_length);
#endif
    dsm_enqueue( &(lb_rab_list[rab_id]->lb_rlc_ul_wm[wm_id]), &dsm_item_ptr);
  }
} /* lb_tx_data_to_rlc() */

#else

void lb_tx_data_to_rlc(uint32 wm_key, uint8 *lb_from_rlc_data, uint16 lb_tx_item_size
#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
, uint32 app_ptr
#endif
)
{
  dsm_item_type *dsm_item_ptr = NULL; /* Pointer to a DSM item sent to RLC */

  uint32 rab_id  = lb_get_rab_id_from_key (wm_key);
  uint32 wm_id   = lb_get_chan_id_from_key (wm_key);

  /*-------------------------------------------------------------------------
  Convert the UL data into a chained DSM item and queue it on
  the DS/RLC watermark.
  -------------------------------------------------------------------------*/  

  dsm_item_ptr = dsm_sdu_to_pkt_chain_bit( lb_from_rlc_data, lb_tx_item_size);

  MSG_MED_DS_5(TC_SUB, "wm_key = %d, rab_id = %d, lc_index = %d UL DNE = %d, DL DNE = %d",wm_key, rab_id, wm_id,lb_rab_list[rab_id]->lb_rlc_ul_wm[wm_id].dont_exceed_cnt, lb_rab_list[rab_id]->lb_rlc_dl_wm[wm_id].dont_exceed_cnt);
  MSG_LOW_DS_4(TC_SUB, "tx = %d %d %d" tx item len = %d bits, lb_from_rlc_data[0], lb_from_rlc_data[1], lb_from_rlc_data[3], lb_tx_item_size);

  if ( dsm_item_ptr == NULL ) 
  {
    ERR( "Empty UL packet chain?", 0, 0, 0 );
  }
  else
  {
    MSG_MED_DS_0(TC_SUB, "LB UL: UL data sent to RLC");
#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
    dsm_item_ptr->app_ptr = (void *) app_ptr;
#endif
    dsm_enqueue( &(lb_rab_list[rab_id]->lb_rlc_ul_wm[wm_id]), &dsm_item_ptr);
  }
} /* lb_tx_data_to_rlc() */

#endif

/*===========================================================================

FUNCTION LB_TX_DUMMY_DCCH_MSG

DESCRIPTION
  This function sends a dummy DCCH message when UE is in loopback mode
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lb_tx_dummy_dcch_msg(void)
{
   switch (tc_current_state)
   {
      case TC_LOOPBACK_MODE1:
      case TC_LOOPBACK_MODE2:
         if (dummy_dcch_enabled)
         {
            MSG_MED_DS_0(TC_SUB,"Sending DUMMY DCCH message");
         }
         break;

      default:
         break;
   }
}
      
#endif /* FEATURE_TC */
