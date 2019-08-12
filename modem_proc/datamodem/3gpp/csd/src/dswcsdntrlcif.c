/*===========================================================================

                 W C D M A   N O N - T R A N S P A R E N T 
             
                 C I R C U I T - S W I T C H E D   D A T A
                             
             R A D I O   L I N K   C O N T R O L   I N T E R F A C E

DESCRIPTION
  This file contains data and function definitions for WCDMA Non-Transparent
  Circuit-Switched Data to send and receive data from Radio Link Control.

    Copyright (c) 2003-2014 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dswcsdntrlcif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/12   ND      RCINIT changes.
04/01/11   TTV     Support for Q6 free floating.
03/04/09   SA      AU level CMI modifications.
11/15/07   AR      Use safety macros for watermark operations.
23/02/07   DA      Fixed high lint errors
09/01/06   AR      Add support for multiprocessor build.
02/09/06   AR      Lint corrections.
05/30/04   AR      Lint corrections.
08/05/03   TMR     Updated watermark callbacks for DSM 2
07/22/03   TMR     Added bit-reversal code of all WCDMA async data
03/25/03   DGY     Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#ifdef FEATURE_DATA_WCDMA_CS

#include "comdef.h"
#include "rex.h"
#include "task.h"
#include "dsm.h"
#include "dsmutil.h"
#include "dsgcsd_task_v.h"
#include "dsucsdnti.h"
#include "dsucsdrlp.h"
#include "dswcsdmshif.h"
#include "data_msg.h"
#include "rcinit_rex.h"

/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Buffers for storing Uplink and Downlink RLP data. Buffer size is 72 bytes.
---------------------------------------------------------------------------*/  
LOCAL uint8 dswcsd_nt_ul_rlp_buf[ DS_UMTS_CSD_FRAME_SIZE_576_BIT ] = {0};
LOCAL uint8 dswcsd_nt_dl_rlp_buf[ DS_UMTS_CSD_FRAME_SIZE_576_BIT ] = {0};
/*---------------------------------------------------------------------------
  TCB pointer for GCSD task
---------------------------------------------------------------------------*/
static rex_tcb_type* gcsd_tcb_ptr = NULL; 

/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================


FUNCTION dswcsd_nt_set_proc_ul_sig_cb


DESCRIPTION
  This function is used to set the signal DS_GCSD_PROC_UL_SIG in the GCSD
  TCB. It should be registered as a WM callback function. It is called 
  when RLC has sent the data in its Uplink watermark.


DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None


===========================================================================*/
/*ARGSUSED*/
void dswcsd_nt_set_proc_ul_sig_cb
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
)
#else
(
   void
)
#endif
{
   /*----------------------------------------------------------------------- 
    We get the gcsd tcb pointer locally.
   ----------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
   gcsd_tcb_ptr = rcinit_lookup_rextask("ds_gcsd"); 
   if ( RCINIT_NULL == gcsd_tcb_ptr)
   {
      DATA_ERR_FATAL("RCINIT Lookup for gcsd tcb returned NULL");
   }
#else
   gcsd_tcb_ptr = &Gcsd_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */
  (void)rex_set_sigs( gcsd_tcb_ptr, DS_GCSD_PROC_UL_SIG );
}


/*===========================================================================


FUNCTION dswcsd_nt_set_proc_dl_sig_cb


DESCRIPTION
  This function is used to set the signal DS_GCSD_PROC_DL_SIG in the GCSD
  TCB. It should be registered as a WM callback function. It is called 
  when RLC receives Downlink data.


DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None


===========================================================================*/
/*ARGSUSED*/
void dswcsd_nt_set_proc_dl_sig_cb
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
)
#else
(
   void
)
#endif
{
  /*----------------------------------------------------------------------- 
    We get the gcsd tcb pointer locally.
  ----------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  gcsd_tcb_ptr = rcinit_lookup_rextask("ds_gcsd"); 
  if ( RCINIT_NULL == gcsd_tcb_ptr)
  {
    DATA_ERR_FATAL("RCINIT Lookup for gcsd tcb returned NULL");
  }
#else
  gcsd_tcb_ptr = &Gcsd_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */
  (void)rex_set_sigs( gcsd_tcb_ptr, DS_GCSD_PROC_DL_SIG );
}


/*===========================================================================


FUNCTION dswcsd_nt_read_dl_ota_data_cb


DESCRIPTION
  Function used to read downlink OTA data from lower layers.  
  dl_data_ptr parameter is set by the callback function to 
  point to the block of downlink data.   

 
DEPENDENCIES
  None


RETURN VALUE
  Boolean value returned indicates whether or not downlink 
  data was successfully read or not.


SIDE EFFECTS
  None


===========================================================================*/
boolean dswcsd_nt_read_dl_ota_data_cb
(
  uint8 **dl_data_ptr               /* Pointer to a block of Downlink data */
)
{
  dsm_item_type *dsm_item_ptr;                    /* Pointer to a DSM item */
  register uint32 i;
  register uint8  tmp_octet;
  register uint8  *tmp_buf_ptr;
  dsm_watermark_type  *dswcsd_from_rlc_wm_ptr = NULL;
/*-------------------------------------------------------------------------*/
  if(dl_data_ptr == NULL)
  {
    DATA_MSG0_ERROR("dl_data_ptr is NULL");
    return FALSE;
  }

  dswcsd_from_rlc_wm_ptr = dswcsd_get_from_rlc_wm_ptr();

  /* Do nothing if watermark cleared */
  if( NULL == dswcsd_from_rlc_wm_ptr )
  {
    return ( FALSE );
  }
  
  /*-------------------------------------------------------------------------
    Get a DSM item from the Downlink RLC watermark.
  -------------------------------------------------------------------------*/
  dsm_item_ptr = DSM_DEQUEUE( dswcsd_from_rlc_wm_ptr );     

  if ( dsm_item_ptr != NULL ) 
  {
    /*-----------------------------------------------------------------------
      Copy the Downlink data in a chained DSM item into the Downlink
      RLP buffer.
    -----------------------------------------------------------------------*/
    (void)dsm_pkt_to_sdu_chain_bit( &dsm_item_ptr,
                                    (void *)dswcsd_nt_dl_rlp_buf );
   
    /*-----------------------------------------------------------------------
      Pass by reference the address of the Downlink RLP buffer.
    -----------------------------------------------------------------------*/
    *dl_data_ptr = (uint8 *)dswcsd_nt_dl_rlp_buf;
    
    /* Now perform bit reversal on all bytes of buffer */  
    tmp_buf_ptr = (uint8 *)dswcsd_nt_dl_rlp_buf;
    for (i=0; i < (uint32)DS_UMTS_CSD_FRAME_SIZE_576_BIT; i++,tmp_buf_ptr++)
    {
      tmp_octet = *tmp_buf_ptr;
      tmp_octet = DS_UCSD_REFLECT_BYTE (tmp_octet);
      *tmp_buf_ptr = tmp_octet;
    }  
    
    return ( TRUE );
  }
  else
  {
    return ( FALSE );
  }
} /* dswcsd_nt_read_dl_ota_data_cb() */


/*===========================================================================


FUNCTION dswcsd_nt_write_ul_ota_data_cb


DESCRIPTION
  Function used to write uplink data to the lower layers.
  ul_data_ptr points to the data block to be written.
 
  
DEPENDENCIES
  None


RETURN VALUE
  Boolean indicating whether or not write operation was successful.


SIDE EFFECTS
  None


===========================================================================*/
boolean dswcsd_nt_write_ul_ota_data_cb 
(
  uint8 *ul_data_ptr                  /* Pointer to a block of Uplink data */
)
{
  dsm_item_type *dsm_item_ptr;                    /* Pointer to a DSM item */
  register uint32 i;
  register uint8  tmp_octet;
  register uint8  *tmp_buf_ptr;
/*-------------------------------------------------------------------------*/

  if(ul_data_ptr == NULL)
  {
    DATA_MSG0_ERROR("ul_data_ptr is NULL");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Convert the Uplink data into an SDU, or a chained DSM item.
  -------------------------------------------------------------------------*/

  /* First, perform bit reversal on all bytes of incoming buffer */
  tmp_buf_ptr = ul_data_ptr;
  for (i=0; i < (uint32)DS_UMTS_CSD_FRAME_SIZE_576_BIT; i++,tmp_buf_ptr++)
  {
    tmp_octet = *tmp_buf_ptr;
    tmp_octet = DS_UCSD_REFLECT_BYTE (tmp_octet);
    *tmp_buf_ptr = tmp_octet;
  }
  
  dsm_item_ptr = dsm_sdu_to_pkt_chain_bit( 
                    (void *)ul_data_ptr, 
                    (word)( (word)DS_UMTS_CSD_FRAME_SIZE_576_BIT * 8 ) ); 

  if ( dsm_item_ptr != NULL )
  {
    if(dswcsd_get_to_rlc_wm_ptr() != NULL)
    {
      /*-----------------------------------------------------------------------
        Enqueue the SDU onto the Uplink RLC watermark.
      -----------------------------------------------------------------------*/
      DSM_ENQUEUE( dswcsd_get_to_rlc_wm_ptr(), &dsm_item_ptr );
   
      return ( TRUE );
    }
    else
    {
      dsm_free_packet(&dsm_item_ptr);
      DATA_MSG0_MED("UL enqueue failed, wm ptr NULL");
      return (FALSE);
    }
  }
  else
  {
    DATA_MSG0_MED("UL enqueue failed, dsm ptr NULL");
    return ( FALSE );
  }
} /* dswcsd_nt_write_ul_ota_data_cb() */


/*===========================================================================


FUNCTION dswcsd_nt_get_ul_ota_buf_cb


DESCRIPTION
  This function is used to return a buffer to be used to 
  for uplink data prior to it being written via the above
  function call.  Function returns the uplink data buffer
  via the ul_data_ptr parameter.
 
  
DEPENDENCIES
  None


RETURN VALUE
  Boolean indicating whether or not the operation was successful.


SIDE EFFECTS
  None


===========================================================================*/
boolean dswcsd_nt_get_ul_ota_buf_cb
(
  uint8 **ul_data_ptr                 /* Pointer to the Uplink data buffer */
)
{
/*-------------------------------------------------------------------------*/
  if(ul_data_ptr == NULL)
  {
    DATA_MSG0_ERROR("ul_data_ptr is NULL in get_ul_ota_buf_cb func");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Pass by reference the address of the Uplink RLP buffer.
  -------------------------------------------------------------------------*/
  *ul_data_ptr = (uint8 *)dswcsd_nt_ul_rlp_buf;

  return ( TRUE );
} /* dswcsd_nt_get_ul_ota_buf_cb() */


/*===========================================================================


FUNCTION dswcsd_nt_init_ota_cb


DESCRIPTION
  This function is used to initialize the OTA subsystem prior
  to starting up a call.  In the below function used to register
  this callback function, this function pointer may be set to 
  NULL if no initialization function is needed
  
  
DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None


===========================================================================*/
void dswcsd_nt_init_ota_cb( void )
{
}

#endif /* FEATURE_DATA_WCDMA_CS */




