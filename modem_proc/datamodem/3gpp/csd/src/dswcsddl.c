/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A

                               D O W N L I N K

DESCRIPTION
  This file contains data and function definitions for WCDMA Circuit-Switched 
  Data Downlink. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2014 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsddl.c_v   1.11   14 Jun 2002 13:08:54   degangy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dswcsddl.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
07/09/09   bs      Fixed offtarget lint medium and low's.
01/06/09   sa      Added NULL check for set_data_counters_cb function.
02/06/08   sa      Updated RX Frame to include FCS error.
11/15/07   ar      Use safety macros for watermark operations.
02/09/06   ar      Lint corrections.
05/28/04   ar      Add logging of synch CSD packet.
04/15/03   dgy     Added support for UMTS CS Data handlers. Removed 
                   FEATURE_DATA_WCDMA_ASYNC_T_CS and
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA.
03/07/03   dwp     Make sync test app runtime configurable.
01/01/03   dwp     Add initial V.80 support. Update copyright.
11/18/02   dgy     Renamed a define.
10/01/02   dgy     Added multi-mode changes.
08/22/02   dgy     Changed two F3 messages under 
                   FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP from ERR to MSG_MED.
06/14/02   dgy     Mainlined the Simple Rate Adaption under 
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA.
05/06/02   dgy     Computed SDU size and character length on a per-call basis
                   for Asynchronous CS Data.
03/15/02   dgy     Added Synchronous Transparent CS Data test application 
                   under FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP. For 
                   Synchronous CS Data, unchained the DSM items from RLC.
03/12/02   dgy     Featurized Asynchronous Transparent CS Data under 
                   FEATURE_DATA_WCDMA_ASYNC_T_CS.
02/24/02   dgy     Set the DL WM callback functions in 
                   dswcsd_call_connected_hdlr() instead of dswcsd_dl_cleanup().
02/21/02   dgy     Added Synchronous CS Data and a run-time switch to switch 
                   between Asynchronous and Synchronous CS Data.
01/11/02   dgy     Renamed some functions.
12/10/01   dgy     Modified the message level of a message.
09/20/01   dgy     Modified watermarks and callback funtions for the DL WM.
                   Dequeued all the data on the DL WM every time 
                   dswcsd_tx_data_to_sio() was invoked. Removed 
                   dswcsd_rx_data_from_rlc(). 
08/31/01   dgy     Changed include file and CM data type names. Dequeued DL
                   data in dswcsd_tx_data_to_sio() instead of 
                   dswcsd_rx_data_from_rlc().
08/29/01   dgy     Changed FEATURE_WCDMA_CS_DATA to FEATURE_DATA_WCDMA_CS.
                   Removed the code that reset the DL WM q_ptr.
08/17/01   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include <stringl/stringl.h>


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)

#ifdef FEATURE_DATA_WCDMA_CS
#include "data_msg.h"
#include "err.h"
#include "dsmutil.h"
#include "ds3gsiolib.h"
#include "ds3gsiolib_ex.h"
#include "dsucsdlog.h"
#include "dswcsdi.h"
#include "dswcsdmshif.h"

#ifdef FEATURE_DATA_V80
#include "dswcsdv80.h"
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
/*---------------------------------------------------------------------------
  The DL SDU count is used to compute the first byte expected in a DL SDU. 
  It repeats from 0 to ( FIRST_BYTE_VAL_RANGE - 1 ). 
---------------------------------------------------------------------------*/  

static uint8 dswcsd_dl_sdu_cnt = 0;

/*---------------------------------------------------------------------------
  First byte expected in a DL SDU, which should repeat from 'a' to 'z' in 
  ASCII.
---------------------------------------------------------------------------*/  

static uint8 dswcsd_dl_expected_first_byte = 0;
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSWCSD_DL_CLEANUP

DESCRIPTION
  This function cleans up the DL variables and data structures.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_dl_cleanup( void )
{
  dsm_watermark_type  *dswcsd_from_rlc_wm_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MSG0_HIGH("Tearing down CS Data DL");
  dswcsd_from_rlc_wm_ptr = dswcsd_get_from_rlc_wm_ptr();

  if(dswcsd_from_rlc_wm_ptr != NULL)
  {
    /*-------------------------------------------------------------------------
      Clear the watermark function pointers.
    -------------------------------------------------------------------------*/  
    dswcsd_from_rlc_wm_ptr->gone_empty_func_ptr = NULL;
    dswcsd_from_rlc_wm_ptr->lowater_func_ptr    = NULL;
    dswcsd_from_rlc_wm_ptr->non_empty_func_ptr  = NULL;
    dswcsd_from_rlc_wm_ptr->hiwater_func_ptr    = NULL;

    /*-------------------------------------------------------------------------
      Clear the queue for the watermark and return buffers to the DSM item pool.
    -------------------------------------------------------------------------*/
    EMPTY_WATERMARK( dswcsd_from_rlc_wm_ptr );
  }
  else
  {
    DATA_MSG0_HIGH("dswcsd_from_rlc_wm_ptr is NULL ");
  }
} /* dswcsd_dl_cleanup() */


/*===========================================================================

FUNCTION DSWCSD_TX_DATA_TO_SIO

DESCRIPTION
  This function retrieves all the data from the RLC/DS watermark, processes 
  it, and sends the DL data to SIO.
  
DEPENDENCIES
  dswcsd_from_rlc_wm_ptr must have been previously initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_tx_data_to_sio( void )
{
  dsm_item_type *dsm_item_ptr = NULL; /* Pointer to a DSM item from the WM */
  ds3g_siolib_status_e_type sio_status;                      /* SIO status */
  uint16 dsm_item_len = 0;         /* Length of DSM item payload, in bytes */
  uint8 logbuf[ SDU_SIZE_FOR_64K ];                      /* Log data buffer */
  dswcsd_call_info_T  dswcsd_call_info;
  dsm_watermark_type  *dswcsd_from_rlc_wm_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MSG0_MED("CS Data DL: processing DL data");

  memscpy(&dswcsd_call_info, sizeof(dswcsd_call_info_T), dswcsd_get_call_info_ptr(), 
         sizeof(dswcsd_call_info_T));

  /*-------------------------------------------------------------------------
    Get a DSM item from the RLC/DS WM.
  -------------------------------------------------------------------------*/  
  dswcsd_from_rlc_wm_ptr = dswcsd_get_from_rlc_wm_ptr();
  dsm_item_ptr = DSM_DEQUEUE( dswcsd_from_rlc_wm_ptr );     
  
  if ( dsm_item_ptr == NULL ) 
  {
    DATA_MSG0_MED("No DSM item on RLC/DS WM?");
  }

  /*-------------------------------------------------------------------------
    Retrieve all the data from the RLC/DS WM, process it, and send the 
    results to SIO.
  -------------------------------------------------------------------------*/  
  
  while ( dsm_item_ptr != NULL ) 
  {

    /*-------------------------------------------------------------------
      Log data in DL SDU
    -------------------------------------------------------------------*/
    if (dswcsd_call_info.bytes_per_sdu !=
        dsm_extract( dsm_item_ptr, 0, logbuf,
                     dswcsd_call_info.bytes_per_sdu))
    {
      MSG_MED ("Could not extract %d bytes from DL SDU log",
               dswcsd_call_info.bytes_per_sdu,0,0);
    }
    ds_ucsd_log_rx_data( logbuf,
                         (ds_ucsd_frame_size_T)dswcsd_call_info.bytes_per_sdu,
                         FALSE);

        
    /*-----------------------------------------------------------------------
      Synchronous CS Data. Get the payload length in the chained DSM item.
    -----------------------------------------------------------------------*/  

    if ( ( dsm_item_len = (uint16)dsm_length_packet( dsm_item_ptr ) ) != 0 )
    {
      DATA_MSG1_MED("CS Data DL: send %d bytes to SIO", dsm_item_len);
    }

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
    if(dswcsd_is_sync_test_app_enabled())
    {
      /*---------------------------------------------------------------------
        Check if we received the right amount of data.
      ---------------------------------------------------------------------*/

      if ( dsm_item_len != dswcsd_call_info.bytes_per_sdu ) 
      {
        DATA_MSG2_MED("%d bytes expected %d rx'ed", 
                 dswcsd_call_info.bytes_per_sdu, dsm_item_len);
      }

      /*---------------------------------------------------------------------
        Check if the first byte received is what is expected.
      ---------------------------------------------------------------------*/

      if ( dsm_item_ptr->data_ptr[ 0 ] != dswcsd_dl_expected_first_byte ) 
      {
        DATA_MSG2_MED("First byte: expected %d actual %d", 
                 dswcsd_dl_expected_first_byte, 
                 dsm_item_ptr->data_ptr[ 0 ]);
      }

      /*---------------------------------------------------------------------
        Update the SDU count, which can be from 0 to 
        ( FIRST_BYTE_VAL_RANGE - 1 ).
      ---------------------------------------------------------------------*/

      dswcsd_dl_sdu_cnt = ( dswcsd_dl_sdu_cnt + 1 ) % FIRST_BYTE_VAL_RANGE;

      /*---------------------------------------------------------------------
        Compute the first byte expected in the next SDU, which can be from 
        'a' to 'z' in ASCII.
      ---------------------------------------------------------------------*/

      dswcsd_dl_expected_first_byte = dswcsd_dl_sdu_cnt + FIRST_BYTE_START_VAL;
    }
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

#ifdef FEATURE_DATA_V80
    /*-----------------------------------------------------------------------
      If we're in V.80 mode, check the pkt chain for any Transparency
      Characters. All such TCs will be processed in the pkt chain.
    -----------------------------------------------------------------------*/
    if(dswcsdv80_get_dl_r_iface_mode_is_v80() == TRUE)
    {
      dswcsdv80_dl_inband_escape(&dsm_item_ptr);
    }
#endif /* FEATURE_DATA_V80 */

    if(dswcsd_call_info.set_data_counters_cb != NULL)
    {
      /* Update call instance traffic counter for SIO interface */
      (void)dswcsd_call_info.set_data_counters_cb( dswcsd_call_info.cm_call_id,
                                                   TRUE,
                                                   dsm_length_packet(dsm_item_ptr),
                                                   0 );
    }
    else
    {
      DATA_MSG1_ERROR("Problem updating data traffic counters: call ID=%d",
                    dswcsd_call_info.cm_call_id);
    }

    /*-----------------------------------------------------------------------
      Send pkt chain or single item to SIO. SIO can accept either.
    -----------------------------------------------------------------------*/

    if ( ( sio_status = ds3g_siolib_ex_setup_tx( dsm_item_ptr, FALSE,
                                   ds3g_siolib_ex_get_csd_port()) )
           != DS3G_SIOLIB_OK )
    {
      DATA_MSG1_ERROR("SIO is not open: status=%d", sio_status );
    }
    
    /*-----------------------------------------------------------------------
      Get a DSM item from the RLC/DS WM.
    -----------------------------------------------------------------------*/  
    dsm_item_ptr = DSM_DEQUEUE( dswcsd_from_rlc_wm_ptr );     
  
  } /* top/outer while(dsm_item_ptr != NULL) - dequeue from the WM */

} /* dswcsd_tx_data_to_sio() */

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
/*===========================================================================

FUNCTION DSWCSD_SET_DL_SDU_CNT

DESCRIPTION
  This function sets the value of dswcsd_dl_sdu_cnt.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_dl_sdu_cnt(uint8 value)
{
  dswcsd_dl_sdu_cnt = value;
}

/*===========================================================================

FUNCTION DSWCSD_SET_DL_EXPECTED_FIRST_BYTE

DESCRIPTION
  This function sets the value of dswcsd_dl_expected_first_byte.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_dl_expected_first_byte
(
  uint8 value
)
{
  dswcsd_dl_expected_first_byte = value;
}

#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */



