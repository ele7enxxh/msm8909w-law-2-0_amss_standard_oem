/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A

                               U P L I N K

DESCRIPTION
  This file contains data and function definitions for WCDMA Circuit-Switched 
  Data Uplink. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 - 2015 by Qualcomm Technologies, Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsdul.c_v   1.14   16 Jul 2002 14:40:56   degangy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dswcsdul.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/11   TTV     Fixed an issue of leaving critical section correctly.
04/05/11   SS      Q6 free floating support.
11/01/10   SA      Removing REX_DISABLE_PREMPTION/REX_ENABLE_PREMPTION.
03/08/10   SA      Added changes to send data to RLC watermark for sync call
                   until the watermark count is HIGH.
09/29/09   SA      Fixed KW errors.
07/09/09   BS      Fixed offtarget lint medium and low's.
03/04/09   SA      AU level CMI modifications.
01/06/09   SA      Added NULL check for set_data_counters_cb function.
03/13/08   SA      Fixing Klocwork issues.
11/15/07   AR      Use safety macros for watermark operations.
23/02/07   DA      Fixed high lint errors
01/25/07   AR      Move flow control flag before flow enable for race cond.
02/09/06   AR      Lint corrections.
06/01/04   AR      Adjust test for data threshold to include partial DSM.
05/28/04   AR      Add logging of synch CSD packet.
04/26/04   AR      Enable SIO flow only when FROM_SIO_Q_LO_CNT reached.  
                   Lint corrections.
10/08/03   TMR     Discard SIO RX data when we reach FROM_SIO_Q_MAX_CNT limit
07/24/03   dwp     Handle case when the DSM item might become empty after 
                   V.80 processing.
07/23/03   TMR     Removed reference to DSM_DS_SMALL_ITEM_SIZ and replaced
                   with UL_DATA_BUF_SIZ
06/20/03   dgy     Pulled up data up to the buffer size in 
                   dswcsd_build_ul_sdu().
04/15/03   dgy     Added support for UMTS CS Data handlers. Removed 
                   FEATURE_DATA_WCDMA_ASYNC_T_CS and
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA. Moved some data
                   definition here from dswcsdultask.c.
03/07/03   dwp     Make sync test app runtime configurable. Restructure 
                   tx_data_to_rlc() to use both build_ul_sdu() and 
                   build_testapp_ul_sdu() based on sync test app feature.
02/28/03 dgy/dwp   Fix dswcsd_rx_data_from_sio() for non V.80 calls when
                   FEATURE_DATA_V80 is defined.
02/12/03   dwp     Add initial V.80 support. Update copyright.
01/23/03   dgy     Used ds3g_siolib_set_inbound_flow() instead of 
                   ds3g_siolib_inbound_flow_enable() and 
                   ds3g_siolib_inbound_flow_disable().
11/18/02   dgy     Renamed a define.
10/01/02   dgy     Added multi-mode changes.
07/15/02   dgy     Added dswcsd_at_esc_seq_ul_cleanup().
06/14/02   dgy     Mainlined the Simple Rate Adaption under 
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA.
05/06/02   dgy     Computed SDU size and character length on a per-call basis
                   for Asynchronous CS Data.
04/29/02   dgy     Replaced static variables with global variables.
03/15/02   dgy     Added Synchronous Transparent CS Data test application 
                   under FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP.
03/12/02   dgy     Featurized Asynchronous Transparent CS Data under 
                   FEATURE_DATA_WCDMA_ASYNC_T_CS. Cleaned up the triggering
                   mechanism for Synchronous CS Data.
02/24/02   dgy     Set the UL WM callback functions in 
                   dswcsd_call_connected_hdlr() instead of dswcsd_dl_cleanup().
02/21/02   dgy     Added Synchronous CS Data and a run-time switch to switch 
                   between Asynchronous and Synchronous CS Data.
01/11/02   dgy     Renamed some functions.
12/17/01   dgy     Ensured the inbound SIO flow was enabled at call release. 
10/30/01   dgy     Fixed RA0 adaption code so that 0x0 could be sent.
09/20/01   dgy     Modified callback funtion for the UL WM. Added and modified
                   critical sections. Modified thresholds for SIO flow control.
08/31/01   dgy     Changed include file and CM data type names.
08/29/01   dgy     Changed FEATURE_WCDMA_CS_DATA to FEATURE_DATA_WCDMA_CS.
                   Removed the code that reset the UL WM q_ptr.
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
#include "rex.h"
#include "task.h"
#include "memory.h"
#include "err.h"
#include "dsmutil.h"
#include "ds3gsiolib.h"
#include "ds3gcfgmgr.h"
#include "ds3gsiolib_ex.h"
#include "dsucsdlog.h"
#include "dswcsdi.h"
#include "dswcsdultask.h"
#include "dswcsdultask_v.h"
#include "dswcsdmshif.h"
#include "dsutil.h"
#include "data_msg.h"
#include "modem_mem.h"
#include "dswcsdul.h"


#ifdef FEATURE_DATA_V80
#include "dswcsdv80.h"
#endif /* FEATURE_DATA_V80 */



/*---------------------------------------------------------------------------
  Pointer to the Uplink RLC watermark
---------------------------------------------------------------------------*/  

/*---------------------------------------------------------------------------
  Pointer to DSM item from SIO
---------------------------------------------------------------------------*/  

static dsm_item_type *dswcsd_sio_dsm_item_ptr = NULL;          

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP


/*---------------------------------------------------------------------------
  Known data pattern for Sync CS Data test application
---------------------------------------------------------------------------*/  

static uint8 dswcsd_known_pattern[ UL_DATA_BUF_SIZ ] = { 0 };

/*---------------------------------------------------------------------------
  The UL SDU count is used to compute the first byte in a UL SDU. It repeats
  from 0 to ( FIRST_BYTE_VAL_RANGE - 1 ). 
---------------------------------------------------------------------------*/  

static uint8 dswcsd_ul_sdu_cnt = 0;

/*---------------------------------------------------------------------------
  First byte in a UL SDU, which repeats from 'a' to 'z' in ASCII.
---------------------------------------------------------------------------*/  

static uint8 dswcsd_ul_first_byte = 0;
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

/*---------------------------------------------------------------------------
  Size of DSM item payload, in bytes
---------------------------------------------------------------------------*/  

static uint16 dswcsd_dsm_payload_size = 0;

extern boolean ds_ucsd_is_csvt_call;

/*---------------------------------------------------------------------------
  QRD CSVT call NV item.
---------------------------------------------------------------------------*/  
uint8 dswcsd_is_qmss_enabled = 2;

/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/


#define MAX_NUM_SDU_PER_TTI      4            /* Maximum of 4 SDUs per TTI */

/*---------------------------------------------------------------------------
  When internally-queued SIO data reaches the high/low counts, disable/enable
  the inbound SIO data flow.
---------------------------------------------------------------------------*/  

uint32 FROM_SIO_Q_HI_CNT  = 3200;                           
uint32 FROM_SIO_Q_LO_CNT  = 1280;                           
uint32 FROM_SIO_Q_MAX_CNT = 3200 * 18;     

#if 0
#define FROM_SIO_Q_HI_CNT  3200                           /* 64 bytes * 50 */
#define FROM_SIO_Q_LO_CNT  1280                           /* 64 bytes * 20 */
#define FROM_SIO_Q_MAX_CNT (FROM_SIO_Q_HI_CNT * 3)                           
#endif 
/*---------------------------------------------------------------------------
  Queue for SIO data
---------------------------------------------------------------------------*/  

static q_type dswcsd_from_sio_q;

/*---------------------------------------------------------------------------
  Number of data bytes from SIO
---------------------------------------------------------------------------*/  

static uint16 dswcsd_from_sio_q_cnt = 0;

/*---------------------------------------------------------------------------
  Flag indicates whether to send UL data to RLC. Used only for Synchronous
  CS Data.
---------------------------------------------------------------------------*/  

static boolean dswcsd_data_to_send = FALSE;

/*---------------------------------------------------------------------------
  Flag indicating if the inbound SIO data flow is enabled
---------------------------------------------------------------------------*/  

static boolean dswcsd_sio_inbound_flow_enabled = TRUE;

/*---------------------------------------------------------------------------
  Global variable used for WCDMA UL SIO data path critical section
---------------------------------------------------------------------------*/
static rex_crit_sect_type dswcsd_ul_sio_crit_sect;


/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/
dsm_item_type* dswcsd_build_ul_sdu_with_sync_flag(void)
{
  uint16 sdu_byte_cnt = 0;
  uint16 alloc_size;
  uint16 octets_left_for_sdu;
  uint8 *sio_data_buf = NULL;
  dsm_item_type *sdu_ptr = NULL;
  dswcsd_call_info_T  dswcsd_call_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memscpy(&dswcsd_call_info, sizeof(dswcsd_call_info_T),
          dswcsd_get_call_info_ptr(), 
         sizeof(dswcsd_call_info_T));

  sio_data_buf = (uint8*) modem_mem_alloc((1024 * sizeof(uint8)), 
                                          MODEM_MEM_CLIENT_DATA_CRIT);
  if(sio_data_buf == NULL)
  {
    DATA_MSG0_ERROR("memory allocation failed");
    return NULL;
  }

  while (sdu_byte_cnt < dswcsd_call_info.bytes_per_sdu)
  {
    if (dswcsd_sio_dsm_item_ptr == NULL)
    {
      dswcsd_sio_dsm_item_ptr = (dsm_item_type *) q_get( &dswcsd_from_sio_q );
      if(dswcsd_sio_dsm_item_ptr != NULL)
      {
        REX_DISABLE_PREMPTION();
        dswcsd_from_sio_q_cnt -= (uint16)dsm_length_packet( dswcsd_sio_dsm_item_ptr );
        REX_ENABLE_PREMPTION();
      }
      else
      {
        
        if ( (dswcsd_sio_dsm_item_ptr 
                = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)) == NULL)
        {
          DATA_MSG2_ERROR("out of memory: small=%d large=%d",
              DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
              DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL));
          dsm_free_packet(&sdu_ptr);
          modem_mem_free(sio_data_buf, MODEM_MEM_CLIENT_DATA_CRIT);
          return NULL;
        }

        memset ( dswcsd_sio_dsm_item_ptr->data_ptr, 
                 0x78,
                 dswcsd_sio_dsm_item_ptr->size );

        dswcsd_sio_dsm_item_ptr->used = dswcsd_sio_dsm_item_ptr->size;
      }
      dswcsd_dsm_payload_size = (uint16)dsm_length_packet( dswcsd_sio_dsm_item_ptr );
    }

    if ( ( sdu_byte_cnt + dswcsd_dsm_payload_size ) >
         dswcsd_call_info.bytes_per_sdu )
    {
      octets_left_for_sdu = dswcsd_call_info.bytes_per_sdu - sdu_byte_cnt;
    }
    else
    {
      octets_left_for_sdu = dswcsd_dsm_payload_size;
    }

    memset((void *)sio_data_buf, 0, (1024 * sizeof(uint8)));

    (void)dsm_pullup( &dswcsd_sio_dsm_item_ptr, sio_data_buf, octets_left_for_sdu );

    dswcsd_dsm_payload_size -= octets_left_for_sdu;

    if ( ( alloc_size = dsm_pushdown_tail( &sdu_ptr,
                                           sio_data_buf,
                                           octets_left_for_sdu,
                                           DSM_DS_SMALL_ITEM_POOL ) ) !=
         octets_left_for_sdu)
    {
      dsm_free_packet(&sdu_ptr);
      sdu_ptr = NULL;
      DATA_MSG0_ERROR("Failed to obtain enough memory");
    }
    else
    {
      sdu_byte_cnt += octets_left_for_sdu;
      ASSERT (NULL != sdu_ptr );
      sdu_ptr->app_field += ( alloc_size * NUM_BITS_PER_BYTE );
    }

    if ( dswcsd_dsm_payload_size == 0 )
    {
      dswcsd_sio_dsm_item_ptr = NULL;
    }
  }
  modem_mem_free(sio_data_buf, MODEM_MEM_CLIENT_DATA_CRIT);
  return (sdu_ptr);
}


/*===========================================================================

FUNCTION DSWCSD_INIT_FROM_SIO_Q

DESCRIPTION
  This function initializes the queue for the incoming UL SIO data.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_init_from_sio_q( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Initialize the REX critical section
  -------------------------------------------------------------------------*/ 

  memset(&dswcsd_ul_sio_crit_sect, 0, sizeof(rex_crit_sect_type));
  DS_INIT_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);

  /*-------------------------------------------------------------------------
    Initialize the queue for the incoming UL SIO data and the data count.
  -------------------------------------------------------------------------*/  
  
  memset(&dswcsd_from_sio_q, 0, sizeof(q_type));
  (void)q_init( &dswcsd_from_sio_q );
  
  dswcsd_from_sio_q_cnt = 0;

  /*-------------------------------------------------------------------------
    Initialize SDU packaging workspace.
  -------------------------------------------------------------------------*/  
  dswcsd_sio_dsm_item_ptr = NULL;
  dswcsd_dsm_payload_size = 0;
  
} /* dswcsd_init_from_sio_q() */


/*===========================================================================

FUNCTION DSWCSD_AT_ESC_SEQ_UL_CLEANUP

DESCRIPTION
  This function is invoked to clean up the relevant UL variables and data 
  structures and enable inbound SIO data flow if necessary when the AT escape 
  sequence is detected.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_at_esc_seq_ul_cleanup( void )
{
  dsm_item_type            *dsm_item_ptr = NULL; /* Pointer to an DSM item */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);

  /*-------------------------------------------------------------------------
    Clear the queue for SIO data and return buffers to the DSM item pool.
  -------------------------------------------------------------------------*/  
  
  while ( ( dsm_item_ptr = 
            (dsm_item_type *) q_get( &dswcsd_from_sio_q ) ) != NULL )
  {
    dsm_free_packet( &dsm_item_ptr );
  }

  dswcsd_from_sio_q_cnt = 0;

  dswcsd_data_to_send = FALSE;

  /*-------------------------------------------------------------------------
    Clear the SDU packaging workspace.
  -------------------------------------------------------------------------*/  
  if ( NULL != dswcsd_sio_dsm_item_ptr )
  {
    DATA_MSG1_MED("Freeing DSM with data on cleanup: %d", 
             dsm_length_packet(dswcsd_sio_dsm_item_ptr));
    dsm_free_packet( &dswcsd_sio_dsm_item_ptr );
  }
  dswcsd_dsm_payload_size = 0;

  /*-------------------------------------------------------------------------
    If the inbound SIO data flow is disabled, enable the data flow.  
  -------------------------------------------------------------------------*/  

  if ( !dswcsd_sio_inbound_flow_enabled )
  {
    ds3g_siolib_ex_set_inbound_flow( DS_FLOW_WCDMACS_MASK, DS_FLOW_ENABLE,
                                      ds3g_siolib_ex_get_csd_port());

    dswcsd_sio_inbound_flow_enabled = TRUE;

    DATA_MSG0_MED( "Enabled inbound SIO flow");
  }
  
  DS_LEAVE_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
} /* dswcsd_at_esc_seq_ul_cleanup() */


/*===========================================================================

FUNCTION DSWCSD_UL_CLEANUP

DESCRIPTION
  This function cleans up the UL variables and data structures and enables
  inbound SIO data flow if necessary.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_ul_cleanup( void )
{
  dsm_item_type            *dsm_item_ptr = NULL; /* Pointer to an DSM item */
  dsm_watermark_type       *dswcsd_to_rlc_wm_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MSG0_HIGH("CS Data UL clean_up");

  DS_ENTER_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);

  /*-------------------------------------------------------------------------
    Clear the queue for SIO data and return buffers to the DSM item pool.
  -------------------------------------------------------------------------*/  
  
  while ( ( dsm_item_ptr = 
            (dsm_item_type *) q_get( &dswcsd_from_sio_q ) ) != NULL )
  {
    dsm_free_packet( &dsm_item_ptr );
  }

  dswcsd_from_sio_q_cnt = 0;

  /*-------------------------------------------------------------------------
    Clear the SDU packaging workspace.
  -------------------------------------------------------------------------*/  
  if ( NULL != dswcsd_sio_dsm_item_ptr )
  {
    DATA_MSG1_MED("Freeing DSM with data on cleanup: %d", 
             dsm_length_packet(dswcsd_sio_dsm_item_ptr));
    dsm_free_packet( &dswcsd_sio_dsm_item_ptr );
  }
  dswcsd_dsm_payload_size = 0;

  dswcsd_to_rlc_wm_ptr = dswcsd_get_to_rlc_wm_ptr();
  
  if( NULL != dswcsd_to_rlc_wm_ptr )
  {
    /*-----------------------------------------------------------------------
      Clear the watermark function pointers.
    -----------------------------------------------------------------------*/  
    dswcsd_to_rlc_wm_ptr->gone_empty_func_ptr = NULL;
    dswcsd_to_rlc_wm_ptr->lowater_func_ptr    = NULL;
    dswcsd_to_rlc_wm_ptr->non_empty_func_ptr  = NULL;
    dswcsd_to_rlc_wm_ptr->hiwater_func_ptr    = NULL;

    /*-----------------------------------------------------------------------
      Clear the queue for the watermark and return buffers to the DSM
      item pool.
    -----------------------------------------------------------------------*/  
    EMPTY_WATERMARK( dswcsd_to_rlc_wm_ptr );
  }
  else
  {
    DATA_MSG0_HIGH("dswcsd_to_rlc_wm_ptr is NULL ");
  }

  dswcsd_data_to_send = FALSE;

  /*-------------------------------------------------------------------------
    If the inbound SIO data flow is disabled, enable the data flow.  
  -------------------------------------------------------------------------*/  

  if ( !dswcsd_sio_inbound_flow_enabled )
  {
    ds3g_siolib_ex_set_inbound_flow( DS_FLOW_WCDMACS_MASK, DS_FLOW_ENABLE,
                                  ds3g_siolib_ex_get_csd_port());

    dswcsd_sio_inbound_flow_enabled = TRUE;

    DATA_MSG0_MED("Enabled inbound SIO flow");
  }
  
  DS_LEAVE_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
} /* dswcsd_ul_cleanup() */


#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
/*===========================================================================

FUNCTION DSWCSD_BUILD_TESTAPP_UL_SDU

DESCRIPTION
  This function builds UL SDUs for Synchronous CS Data when running the 
  Sync Test App.

DEPENDENCIES
  dswcsd_from_sio_q must have been previously initialized.

RETURN VALUE
  Pointer to an SDU.

SIDE EFFECTS
  None

===========================================================================*/

dsm_item_type *dswcsd_build_testapp_ul_sdu( void )
{
  uint8  sio_data_buf[ UL_DATA_BUF_SIZ ];           /* Buffer for SIO data */
  uint8  sdu_byte_cnt;                         /* Number of SDU data bytes */
  uint16 sio_data_size;            /* Number of SIO data bytes to retrieve */
  uint16 alloc_size;            /* Number of bytes pushed into an DSM item */
  dsm_item_type        *sdu_ptr = NULL;                     /* SDU pointer */
    dswcsd_call_info_T  dswcsd_call_info;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memscpy(&dswcsd_call_info, sizeof(dswcsd_call_info_T), dswcsd_get_call_info_ptr(), 
         sizeof(dswcsd_call_info_T));
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Number of bytes per sdu should be always <= UL_DATA_BUF_SIZ */
  if ( dswcsd_call_info.bytes_per_sdu > UL_DATA_BUF_SIZ )
  {
    DATA_MSG1_ERROR(" Number of bytes_per_sdu for this call info is invalid %d",
         dswcsd_call_info.bytes_per_sdu);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Initialize the byte count for UL data.
  -------------------------------------------------------------------------*/

  sdu_byte_cnt = 0;

  /*-------------------------------------------------------------------------
    Build UL SDU.
  -------------------------------------------------------------------------*/

  while ( sdu_byte_cnt < dswcsd_call_info.bytes_per_sdu )
  {
    /*-----------------------------------------------------------------------
      Application to test Sync CS Data: send a known data pattern on the UL
      and compare the received data with the known pattern on the DL. The
      first byte in the pattern is from 'a' to 'z' in ASCII. The second-to-last
      byte in the pattern is Carriage Return. The last byte is Line Feed.
    -----------------------------------------------------------------------*/

    sio_data_size = dswcsd_call_info.bytes_per_sdu;

    /*-----------------------------------------------------------------------
      Copy the known data pattern into a buffer.
    -----------------------------------------------------------------------*/

    memscpy( sio_data_buf, sizeof(sio_data_buf), dswcsd_known_pattern,
            sio_data_size );

    /*-----------------------------------------------------------------------
      Set the first byte in the SDU, which can be from 'a' to 'z' in ASCII.
    -----------------------------------------------------------------------*/

    sio_data_buf[ 0 ] = dswcsd_ul_first_byte;

    if ( ( alloc_size = dsm_pushdown_tail( &sdu_ptr,
                                           sio_data_buf,
                                           sio_data_size,
                                           DSM_DS_SMALL_ITEM_POOL ) ) !=
         sio_data_size)
    {
      /*---------------------------------------------------------------------
        Failed to allocate needed DSM items. Release the DSM
        item being allocated and set the SDU pointer to NULL.
      ---------------------------------------------------------------------*/

      dsm_free_packet(&sdu_ptr);

      sdu_ptr = NULL;

      DATA_MSG0_ERROR("Failed to obtain enough memory");
    }
    else
    {
      /*---------------------------------------------------------------------
        Successfully inserted data into the SDU. Update the SDU byte count
        and the app_field.
      ---------------------------------------------------------------------*/

      sdu_byte_cnt += (uint8)sio_data_size;

      ASSERT ( NULL != sdu_ptr );
      sdu_ptr->app_field += ( alloc_size * NUM_BITS_PER_BYTE );

      /*---------------------------------------------------------------------
        Update the SDU count, which can be from 0 to
        ( FIRST_BYTE_VAL_RANGE - 1 ).
      ---------------------------------------------------------------------*/

      dswcsd_ul_sdu_cnt = ( dswcsd_ul_sdu_cnt + 1 ) % FIRST_BYTE_VAL_RANGE;

      /*---------------------------------------------------------------------
        Compute the first byte in the next SDU, which can be from 'a' to 'z'
        in ASCII.
      ---------------------------------------------------------------------*/

      dswcsd_ul_first_byte = dswcsd_ul_sdu_cnt + FIRST_BYTE_START_VAL;
    }

  } /* while loop */

  return ( sdu_ptr );

} /* dswcsd_build_testapp_ul_sdu() */
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */


/*===========================================================================

FUNCTION DSWCSD_BUILD_UL_SDU

DESCRIPTION
  This function builds UL SDUs for Synchronous CS Data calls and V.80.
  Sync Test App UL SDUs are built w/dswcsd_build_testapp_ul_sdu().

DEPENDENCIES
  dswcsd_from_sio_q must have been previously initialized.

RETURN VALUE
  Pointer to an SDU.

SIDE EFFECTS
  None

===========================================================================*/

dsm_item_type *dswcsd_build_ul_sdu( void )
{
  uint8  sio_data_buf[ UL_DATA_BUF_SIZ ];     /* Buffer for SIO data */
  uint8  sdu_byte_cnt;                         /* Number of SDU data bytes */
  uint16 sio_data_size;            /* Number of SIO data bytes to retrieve */
  uint16 alloc_size;            /* Number of bytes pushed into an DSM item */
  dsm_item_type        *sdu_ptr = NULL;                     /* SDU pointer */

#ifdef FEATURE_DATA_V80
  boolean item_became_chain = FALSE; /* single DSM might grow in V.80 mode */
#endif
  dswcsd_call_info_T  dswcsd_call_info;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memscpy(&dswcsd_call_info, sizeof(dswcsd_call_info_T), dswcsd_get_call_info_ptr(), 
         sizeof(dswcsd_call_info_T));
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the byte count for UL data.
  -------------------------------------------------------------------------*/

  sdu_byte_cnt = 0;

  /*-------------------------------------------------------------------------
    Build UL SDU.
  -------------------------------------------------------------------------*/

  while ( sdu_byte_cnt < dswcsd_call_info.bytes_per_sdu )
  {
    /*-----------------------------------------------------------------------
      Take account of unfinished DSM item from the previous SDU.
    -----------------------------------------------------------------------*/

    if ( dswcsd_sio_dsm_item_ptr == NULL )
    {
      /*---------------------------------------------------------------------
        There is no unfinished DSM item, retrieve queued SIO data.
      ---------------------------------------------------------------------*/

      dswcsd_sio_dsm_item_ptr = (dsm_item_type *) q_get( &dswcsd_from_sio_q );

      if ( dswcsd_sio_dsm_item_ptr != NULL )
      {
        /*-------------------------------------------------------------------
          Decrease the count for queued SIO data.
        -------------------------------------------------------------------*/

        DS_ENTER_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);

        dswcsd_from_sio_q_cnt -= (uint16)dsm_length_packet( dswcsd_sio_dsm_item_ptr );

        DS_LEAVE_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);

#ifdef FEATURE_DATA_V80

        /*-------------------------------------------------------------------
          In general, the V.80 UL data stream can only get smaller or stay
          the same size.

          However, on a per DSM item basis, a single item can get larger. The
          one case when this can happens is:

          1.) A V.80 insert Transparency Character command gets split across
              two DSM items.
          2.) The second DSM item containing the insert Transparency Character
              command is full.

          So this single item passed in for V.80 processing will come back
          as a pkt chain.

          Note, this V.80 call must be done after the above update to
          dswcsd_from_sio_q_cnt. dswcsd_from_sio_q_cnt is for unprocessed
          data from the DTE. dswcsd_v80_from_dte() may alter the size of
          the DSM item, and this would make dswcsd_from_sio_q_cnt incorrect.
        -------------------------------------------------------------------*/
        if(dswcsdv80_get_ul_r_iface_mode_is_v80() == TRUE)
        {
          item_became_chain = dswcsd_v80_from_dte(&dswcsd_sio_dsm_item_ptr);
 
          /*-----------------------------------------------------------------
            In the UL, it's possible a DSM item becomes empty after V.80 
            processing. In this case, dswcsd_v80_from_dte() will free the 
            item and dswcsd_sio_dsm_item_ptr will be NULL. When this happens,
            go back to the top of the loop to get a new DSM item to process.
          -----------------------------------------------------------------*/
          if(dswcsd_sio_dsm_item_ptr == NULL)
          {
            DATA_MSG0_MED("Item became empty");
            continue;
          }        
        }
#endif /* FEATURE_DATA_V80 */

        /*-------------------------------------------------------------------
          Get the size of the DSM item payload.
        -------------------------------------------------------------------*/

        dswcsd_dsm_payload_size = (uint16)dsm_length_packet( dswcsd_sio_dsm_item_ptr );
      }
      else
      {
        DATA_MSG1_HIGH("dswcsd_sio_dsm_item_ptr is null and SIO_q has no data resetting dswcsd_from_sio_q_cnt to 0 earlier value %d",
                  dswcsd_from_sio_q_cnt);
        DS_ENTER_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
        dswcsd_from_sio_q_cnt = 0;
        DS_LEAVE_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
        break;
      }
    }

    /*-----------------------------------------------------------------------
      Assemble UL SDU: compute how many bytes to pull out of the DSM item,
      pull the bytes out of the DSM item, and insert the bytes into the SDU.
    -----------------------------------------------------------------------*/

    if ( ( sdu_byte_cnt + dswcsd_dsm_payload_size ) > 
         dswcsd_call_info.bytes_per_sdu ) 
    {
      /*---------------------------------------------------------------------
        DSM item payload is more then required for this SDU.
        Take out just what is required to finish this SDU.
      ---------------------------------------------------------------------*/
      sio_data_size = dswcsd_call_info.bytes_per_sdu - sdu_byte_cnt;
    }
    else
    {
      /*---------------------------------------------------------------------
        This entire DSM item will fit into this SDU.
      ---------------------------------------------------------------------*/
      sio_data_size = dswcsd_dsm_payload_size;
    }

    /*-----------------------------------------------------------------------
      Do not pull up data more than the local buffer size.
    -----------------------------------------------------------------------*/
    sio_data_size = MIN( sio_data_size, sizeof( sio_data_buf ) );

    /*-----------------------------------------------------------------------
      Pull the data out of the single DSM item received from SIO. This can
      be either all of the item or just some. It depends on how much more
      is needed for this SDU.
    -----------------------------------------------------------------------*/
    (void)dsm_pullup( &dswcsd_sio_dsm_item_ptr, sio_data_buf, sio_data_size );

    dswcsd_dsm_payload_size -= sio_data_size;

    /*-----------------------------------------------------------------------
      Now, build the SDU up. dsm_pushdown_tail() will chain DSM items as
      needed should we have more data then one DSM item. The SDU will be
      a DSM packet chain.
    -----------------------------------------------------------------------*/
    if ( ( sio_data_size == 0 ) ||
         ( alloc_size = dsm_pushdown_tail( &sdu_ptr,
                                           sio_data_buf,
                                           sio_data_size,
                                           DSM_DS_SMALL_ITEM_POOL ) ) !=
         sio_data_size)
    {
      /*---------------------------------------------------------------------
        Failed to allocate needed DSM items. Release the DSM
        item being allocated and set the SDU pointer to NULL.
      ---------------------------------------------------------------------*/

      dsm_free_packet(&sdu_ptr);

      sdu_ptr = NULL;

      DATA_MSG0_ERROR("Failed to obtain enough memory");
    }
    else
    {
      /*---------------------------------------------------------------------
        Successfully inserted data into the SDU. Update the SDU byte count
        and the app_field.
      ---------------------------------------------------------------------*/

      sdu_byte_cnt += (uint8)sio_data_size;

      ASSERT( NULL != sdu_ptr);
      sdu_ptr->app_field += ( alloc_size * NUM_BITS_PER_BYTE );

    }

    if ( dswcsd_dsm_payload_size == 0 )
    {
#ifdef FEATURE_DATA_V80
      if(item_became_chain == TRUE)
      {
        /*-------------------------------------------------------------------
          When we have finished copying this DSM item into an SDU,
          see if this item was chained. This will only be if there was
          some data from SIO. V.80 can make a single item become at most
          two. If it had been chained, dswcsd_sio_dsm_item_ptr is
          pointing at the second item. Now make dswcsd_dsm_payload_size
          correspond to the same item.

          Otherwise, the item is empty, so set dswcsd_sio_dsm_item_ptr
          back to NULL.
        -------------------------------------------------------------------*/
        item_became_chain = FALSE;
        dswcsd_dsm_payload_size = (uint16)dsm_length_packet( dswcsd_sio_dsm_item_ptr );
        #ifdef V80_DEBUG
          DATA_MSG1_ERROR("V80 chained the item in UL, pkt_ptr->size=%d",
                     dswcsd_dsm_payload_size);
        #endif
      }
      else
#endif /* FEATURE_DATA_V80 */
      {
        dswcsd_sio_dsm_item_ptr = NULL;
      }

    } /* dswcsd_dsm_payload_size == 0 */

  } /* while loop */

  return ( sdu_ptr );

} /* dswcsd_build_ul_sdu() */


/*===========================================================================

FUNCTION DSWCSD_TX_DATA_TO_RLC

DESCRIPTION
  This function builds and sends UL SDUs to RLC.

DEPENDENCIES
  dswcsd_to_rlc_wm_ptr must have been previously initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_tx_data_to_rlc( void )
{
  dsm_item_type *sdu_ptrs[ MAX_NUM_SDU_PER_TTI ];          /* SDU pointers */
  uint8          sdu_cnt;                                     /* SDU count */
  dswcsd_call_info_T  dswcsd_call_info;
  dsm_watermark_type  *dswcsd_to_rlc_wm_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memscpy(&dswcsd_call_info, sizeof(dswcsd_call_info_T), dswcsd_get_call_info_ptr(), 
         sizeof(dswcsd_call_info_T));

  memset ((void*)sdu_ptrs, NULL, MAX_NUM_SDU_PER_TTI);
  
  /*-------------------------------------------------------------------------
    Reset the SDU count.
  -------------------------------------------------------------------------*/

  sdu_cnt = 0;
  dswcsd_to_rlc_wm_ptr = dswcsd_get_to_rlc_wm_ptr();
  
  DATA_MSG2_MED("CS tx status: siocnt=%d, rlccnt=%d", dswcsd_from_sio_q_cnt/80, 
              dsm_queue_cnt(dswcsd_to_rlc_wm_ptr));

  /*-------------------------------------------------------------------------
    Check high count is reached.
  -------------------------------------------------------------------------*/
  if( dswcsd_to_rlc_wm_ptr->hi_watermark <= dsm_queue_cnt(dswcsd_to_rlc_wm_ptr) )
  {
    DATA_MSG0_HIGH("Don't post to RLC watermark");
    /* This will filter further SIO signalling,
       It will be set to false when RLC de-queues.
    */
    dswcsd_data_to_send = TRUE;
    return;
  }
  /*-------------------------------------------------------------------------
    Synchronous CS Data.
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP

  if(dswcsd_is_sync_test_app_enabled())
  {
    /*-----------------------------------------------------------------------
      Sync Test App call. Build SDUs w/known fill pattern.
    -----------------------------------------------------------------------*/

    for ( sdu_cnt = 0; sdu_cnt < dswcsd_call_info.sdus_per_tti; sdu_cnt++ ) 
    {
      /*---------------------------------------------------------------------
        Build UL SDU.
      ---------------------------------------------------------------------*/

      sdu_ptrs[ sdu_cnt ] = dswcsd_build_testapp_ul_sdu();
    }
  }
  else

#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

  {
    /*-----------------------------------------------------------------------
      Non Sync Test App call. Check if there is 
      enough data to send for a TTI.
    -----------------------------------------------------------------------*/

    if ( (uint32)( dswcsd_from_sio_q_cnt + dswcsd_dsm_payload_size ) >=
         (uint32)( dswcsd_call_info.sdus_per_tti * dswcsd_call_info.bytes_per_sdu) )
    {
      /*---------------------------------------------------------------------
        There is data for a TTI. Build UL SDUs.
      ---------------------------------------------------------------------*/

      dswcsd_data_to_send = TRUE;


      for ( sdu_cnt = 0; 
            sdu_cnt < dswcsd_call_info.sdus_per_tti; 
            sdu_cnt++ )
      {
        /*-------------------------------------------------------------------
          Build UL SDU.
        -------------------------------------------------------------------*/

        sdu_ptrs[ sdu_cnt ] = dswcsd_build_ul_sdu();
      }

      /*---------------------------------------------------------------------
        Enable the inbound SIO data flow if it is disabled and the queue
        low count has been reached.
      ---------------------------------------------------------------------*/

      DS_ENTER_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);

      if ( ( !dswcsd_sio_inbound_flow_enabled ) &&
           ( dswcsd_from_sio_q_cnt <= FROM_SIO_Q_LO_CNT ) )
      {
        dswcsd_sio_inbound_flow_enabled = TRUE;

        ds3g_siolib_ex_set_inbound_flow( DS_FLOW_WCDMACS_MASK,
                                         DS_FLOW_ENABLE,
                                         ds3g_siolib_ex_get_csd_port());

        DATA_MSG0_MED("Enabled inbound SIO flow");
      }

      DS_LEAVE_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
    }
    else
    {

#ifdef FEATURE_DATA_V80
      
      /*---------------------------------------------------------------------
        There is not enough data to send. For V.80, we fill the frame(s)
        with SYN1. Otherwise, synchronous will send nothing, and wait
        for more data to arrive.
      ---------------------------------------------------------------------*/

      if(dswcsdv80_get_ul_r_iface_mode_is_v80() == TRUE)
      {
        /*-------------------------------------------------------------------
          Build partial or full SDUs using SYN1
        -------------------------------------------------------------------*/
        for ( sdu_cnt = 0; sdu_cnt < dswcsd_call_info.sdus_per_tti; sdu_cnt++ )
        {
          sdu_ptrs[ sdu_cnt ] = dswcsdv80_build_ul_sdu_with_flag();
        }
      }
      else

#endif /* FEATURE_DATA_V80 */
      {
        /*-------------------------------------------------------------------
                      Wait for enough data to send.
        -------------------------------------------------------------------*/

        DATA_MSG2_MED("Not enough data to tx: cnt=%d < %d",
                 dswcsd_from_sio_q_cnt,
                 ( dswcsd_call_info.sdus_per_tti * 
                   dswcsd_call_info.bytes_per_sdu ));

        dswcsd_data_to_send = FALSE;
        /*-------------------------------------------------------------------
          Adding extra(remaining) bytes of data if user data is not sufficient.
          Eexempt from Queueing data if QMSS NV is not enabled.
        -------------------------------------------------------------------*/
        if(ds_ucsd_is_csvt_call && (dswcsd_is_qmss_enabled == 1) )
        {
          /* adding SYNC flag, if incoming data is not suffficent */
          for ( sdu_cnt = 0; sdu_cnt < dswcsd_call_info.sdus_per_tti; sdu_cnt++ )
          {
            sdu_ptrs[ sdu_cnt ] = dswcsd_build_ul_sdu_with_sync_flag();
          }
        }
      }

    } /* else not enough data to send */

  } /* !dswcsd_sync_test_app_enabled */

  /*-------------------------------------------------------------------------
    Send UL SDUs to RLC.
  -------------------------------------------------------------------------*/

  if ( sdu_cnt == dswcsd_call_info.sdus_per_tti ) 
  {
    for ( sdu_cnt = 0; sdu_cnt < dswcsd_call_info.sdus_per_tti; sdu_cnt++ ) 
    {
      if ( sdu_ptrs[ sdu_cnt ] == NULL )
      {
        DATA_MSG0_ERROR("Empty UL packet chain?");
      }
      else
      {
        uint8 logbuf[ UL_DATA_BUF_SIZ ];  /* Log data buffer */

        /*-------------------------------------------------------------------
          Log data in UL SDU
        -------------------------------------------------------------------*/
        if (dswcsd_call_info.bytes_per_sdu !=
            dsm_extract( sdu_ptrs[ sdu_cnt ], 0,
                         logbuf, dswcsd_call_info.bytes_per_sdu))
        {
          DATA_MSG1_MED("Could not extract %d bytes from UL SDU log",
                    dswcsd_call_info.bytes_per_sdu);
        }
        ds_ucsd_log_tx_data( logbuf,
                             (ds_ucsd_frame_size_T)dswcsd_call_info.bytes_per_sdu );

        DATA_MSG1_MED( "CS Data UL: send %d bytes to RLC",
                 dsm_length_packet( sdu_ptrs[ sdu_cnt ] ));

        /*-------------------------------------------------------------------
          Enqueue the SDU onto the DS/RLC watermark.
        -------------------------------------------------------------------*/
        DSM_ENQUEUE( dswcsd_to_rlc_wm_ptr, &sdu_ptrs[ sdu_cnt ] );

        sdu_ptrs[ sdu_cnt ] = NULL;
      }
    }
  } /* if(sdu_cnt == dswcsd_call_info.sdus_per_tti) */

} /* dswcsd_tx_data_to_rlc() */


/*===========================================================================

FUNCTION DSWCSD_RX_DATA_FROM_SIO

DESCRIPTION
  This function receives UL data from SIO. It queues the data on a local queue,
  dswcsd_from_sio_q.
  
DEPENDENCIES
  dswcsd_from_sio_q must have been previously initialized.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_rx_data_from_sio
( 
  dsm_item_type **dsm_item_ptr       /* Address of a pointer to a DSM item */
)
{
  dswcsd_call_info_T  dswcsd_call_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(dsm_item_ptr != NULL);

  memscpy(&dswcsd_call_info, sizeof(dswcsd_call_info_T), dswcsd_get_call_info_ptr(), 
         sizeof(dswcsd_call_info_T));

  /*-------------------------------------------------------------------------
    Queue the incoming SIO data on a local queue, dswcsd_from_sio_q.  
  -------------------------------------------------------------------------*/  
  
  if ( *dsm_item_ptr != NULL ) 
  {
    /* Determine if we already have reached the max queued count... if so,
    ** start discarding the data (this should only happen if TE is not
    ** obeying flow control indications)
    */ 
    if (dswcsd_from_sio_q_cnt >= FROM_SIO_Q_MAX_CNT)
    {
      dsm_free_packet (dsm_item_ptr);
      DATA_MSG2_ERROR("CSD SIO Tx queue full, dropping packet: %d >= %d",
                 dswcsd_from_sio_q_cnt, FROM_SIO_Q_MAX_CNT);
    }
    else
    {
      uint32  pkt_len = dsm_length_packet( (*dsm_item_ptr) );
      
      /*-----------------------------------------------------------------------
        Queue the incoming data and increase the queued data count.
      -----------------------------------------------------------------------*/  
      
      DS_ENTER_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);

      q_put( &dswcsd_from_sio_q, &(*dsm_item_ptr)->link );
      dswcsd_from_sio_q_cnt += (uint16)pkt_len;
      
      DATA_MSG1_MED("CSD SIO Tx queue cnt=%d", dswcsd_from_sio_q_cnt);
      
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
      if(!dswcsd_is_sync_test_app_enabled())
#endif
      {
        /*---------------------------------------------------------------------
          For synchronous CS Data, if there is enough data to send for the next
          TTI, signal the CS Data UL task.
             Do not signal if it's a V.80 call, or a sync test app call.
        ---------------------------------------------------------------------*/  
      
        if ( 
#ifdef FEATURE_DATA_V80
             ( dswcsdv80_get_ul_r_iface_mode_is_v80() == FALSE ) &&
#endif
             !dswcsd_data_to_send &&
             ( dswcsd_from_sio_q_cnt >= 
               ( dswcsd_call_info.sdus_per_tti * 
                 dswcsd_call_info.bytes_per_sdu ) ) )
        {
          DATA_MSG1_MED("CSD SIO TX post signal SIO q cnt %d", dswcsd_from_sio_q_cnt);

#ifdef FEATURE_DSM_WM_CB      
          dswcsd_set_ul_tx_data_sig(NULL,NULL);
#else
          dswcsd_set_ul_tx_data_sig();
#endif
        }
      } /* !dswcsd_sync_test_app_enabled */
      
      /*-----------------------------------------------------------------------
        If the queued SIO data reaches the high count and the inbound SIO data
        flow is enabled, disable the data flow.  
      -----------------------------------------------------------------------*/  
      
      if ( dswcsd_sio_inbound_flow_enabled &&
          ( dswcsd_from_sio_q_cnt >= FROM_SIO_Q_HI_CNT ) )
      {
        ds3g_siolib_ex_set_inbound_flow( DS_FLOW_WCDMACS_MASK,
                                         DS_FLOW_DISABLE,
                                         ds3g_siolib_ex_get_csd_port());
     
        dswcsd_sio_inbound_flow_enabled = FALSE;
     
        DATA_MSG0_MED("Disabled inbound SIO flow");
      }
      
      DS_LEAVE_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);

      if( dswcsd_call_info.set_data_counters_cb != NULL )
      {
        /* Update call instance traffic counter for SIO interface */
        (void)dswcsd_call_info.set_data_counters_cb( dswcsd_call_info.cm_call_id,
                                                     TRUE, 0, pkt_len );
      }
      else
      {
        DATA_MSG1_ERROR( "Problem updating data traffic counters: call ID=%d",
                    dswcsd_call_info.cm_call_id);
      }
    } /* dswcsd_from_sio_q_cnt < FROM_SIO_Q_MAX_CNT */
  } /* *dsm_item_ptr != NULL */
} /* dswcsd_rx_data_from_sio() */

/*===========================================================================

FUNCTION DSWCSD_GET_SIO_DSM_ITEM_PTR

DESCRIPTION
  This function returns the dswcsd_sio_dsm_item_ptr.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  dsm_item_type *

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type * dswcsd_get_sio_dsm_item_ptr(void)
{
  return dswcsd_sio_dsm_item_ptr;
}

/*===========================================================================

FUNCTION DSWCSD_SET_SIO_DSM_ITEM_PTR

DESCRIPTION
  This function sets the dswcsd_sio_dsm_item_ptr.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_sio_dsm_item_ptr(dsm_item_type * item_ptr)
{
  dswcsd_sio_dsm_item_ptr = item_ptr;
}

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
/*===========================================================================

FUNCTION DSWCSD_SET_KNOWN_PATTERN

DESCRIPTION
  This function sets the value corresponding to the index in 
  dswcsd_known_pattern array.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  int

SIDE EFFECTS
  None

===========================================================================*/
int dswcsd_set_known_pattern
(
  uint8 index, 
  uint8 value
)
{
  if (index < UL_DATA_BUF_SIZ)
  {
    dswcsd_known_pattern[index] = value;
  }
  else
  {
    DATA_MSG1_ERROR("Index: %d passed for dswcsd_known_pattern is invalid",
               index);
    return -1;
  }

  return 0;
}

/*===========================================================================

FUNCTION DSWCSD_SET_UL_SDU_CNT

DESCRIPTION
  This function sets the value of dswcsd_ul_sdu_cnt
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_ul_sdu_cnt(uint8 value)
{
  dswcsd_ul_sdu_cnt = value;
}

/*===========================================================================

FUNCTION DSWCSD_SET_UL_FIRST_BYTE

DESCRIPTION
  This function sets the value of dswcsd_ul_first_byte
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_ul_first_byte(uint8 value)
{
  dswcsd_ul_first_byte = value;
}

#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

/*===========================================================================

FUNCTION DSWCSD_GET_DSM_PAYLOAD_SIZE

DESCRIPTION
  This function returns the value of dswcsd_dsm_payload_size
  
DEPENDENCIES
  None.
  
RETURN VALUE
  uint16.

SIDE EFFECTS
  None

===========================================================================*/
uint16 dswcsd_get_dsm_payload_size(void)
{
  uint16  payload_size;
  DS_ENTER_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
  payload_size = dswcsd_dsm_payload_size;
  DS_LEAVE_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
  return payload_size;
}

/*===========================================================================

FUNCTION DSWCSD_SET_DSM_PAYLOAD_SIZE

DESCRIPTION
  This function sets the value of dswcsd_dsm_payload_size
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_dsm_payload_size(uint16 value)
{
  DS_ENTER_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
  dswcsd_dsm_payload_size = value;
  DS_LEAVE_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
}

/*===========================================================================

FUNCTION DSWCSD_GET_FROM_SIO_Q_PTR

DESCRIPTION
  This function returns the pointer to dswcsd_from_sio_q
  
DEPENDENCIES
  None.
  
RETURN VALUE
  q_type *.

SIDE EFFECTS
  None

===========================================================================*/
q_type * dswcsd_get_from_sio_q_ptr(void)
{
  return &dswcsd_from_sio_q;
}

/*===========================================================================

FUNCTION DSWCSD_GET_FROM_SIO_Q_CNT

DESCRIPTION
  This function returns the queue count for dswcsd_from_sio_q
  
DEPENDENCIES
  None.
  
RETURN VALUE
  uint16.

SIDE EFFECTS
  None

===========================================================================*/
uint16 dswcsd_get_from_sio_q_cnt(void)
{
  uint16 sio_q_cnt;
  DS_ENTER_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
  sio_q_cnt = dswcsd_from_sio_q_cnt;
  DS_LEAVE_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
  return sio_q_cnt;
}

/*===========================================================================

FUNCTION DSWCSD_UPDATE_FROM_SIO_Q_CNT

DESCRIPTION
  This function updates the queue count for dswcsd_from_sio_q
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_update_from_sio_q_cnt(uint16 value)
{
  DS_ENTER_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
  dswcsd_from_sio_q_cnt = value;
  DS_LEAVE_CRIT_SECTION(&dswcsd_ul_sio_crit_sect);
}

/*===========================================================================

FUNCTION DS_WCSD_GET_CSVTI_NV_QMSS_ENABLED

DESCRIPTION
  This function reads from NV whether or not CSVT NV for QRD device is set or not.
  It must be called in the DSGCSD task.
   
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void ds_wcsd_get_csvti_nv_qmss_enabled
(
  sys_modem_as_id_e_type subs_id
)
{
  #define DSWCSD_QMSS_ENABLE_NV_PATH \
                               (char*)"/nv/item_files/modem/mmode/qmss_enabled"

  nv_stat_enum_type nv_status = NV_DONE_S;
    /* status var for reading from NV */

/*-------------------------------------------------------------------------*/
  
  /* Read from NV whether or not csvti nv qmss is enabled*/

  nv_status =
    ds3gcfgmgr_read_efs_nv_ex(DSWCSD_QMSS_ENABLE_NV_PATH,
                   (void *)(&dswcsd_is_qmss_enabled), 
                   sizeof(uint8),
                   subs_id);

  if(nv_status == NV_DONE_S)
  {
    DATA_MSG1_HIGH("NV_DATA_QMSS_CSVT_ENABLE is set to %d",
             dswcsd_is_qmss_enabled);
  }
  else
  {
    /* If problem reading from NV, default to FALSE.
    */
    dswcsd_is_qmss_enabled = 0;
    DATA_MSG1_ERROR("NV_DATA_QMSS_CSVT_ENABLE NV not set, Default %d",
               dswcsd_is_qmss_enabled);
  }
} /* ds_ucsd_get_csvti_nv_qmss_enabled() */

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


