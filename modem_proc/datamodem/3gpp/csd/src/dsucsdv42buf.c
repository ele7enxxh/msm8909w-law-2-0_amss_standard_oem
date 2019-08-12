/*===========================================================================

              GSM Circuit Switched Data XXXX Module

DESCRIPTION
  This file contains the implementation of the GSM Circuit switched 
  data XXXX functionality.

Copyright (c) 2002 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42buf.c_v   1.2   21 Mar 2002 10:57:58   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdv42buf.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
21/06/07   DA      Correct possible buffer overrun vulnerability
23/02/07   DA      High lint error corrections
02/09/06   AR      Lint corrections.
02/03/06   AR      Remove Tx buffer append on read to avoid DSM free issue.
05/05/04   AR      Fixed TX buffer read PDU size and return value issues.
                   Removed obsolete code.  Lint corrections.
07/15/03   TMR     Added support for RLP Version 2
04/30/03   AR      Add suppport for DSM items inplace of local buffers
                   Lint corrections
02/05/03   TMR     Added watermarking to ds_check_full_V42_rx_buffer() to 
                   control the RX flow of data to keep processing times
                   down
                   
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/


/*****************************************************************************
 *
 *  File:            /home/briers/gsmqc/ds/SCCS/s.m51308.c
 *  -----
 *
 *  SCCS Version:    2.2
 *  -------------
 *
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained herein
 *  is confidential.  The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Communications Consultants Worldwide Limited (C) 2000
 *
 *  Project:        P60
 *  --------
 *
 *  Software Unit:  GSM/MS/DS
 *  --------------
 *
 *  Description:    V24 Uplink Buffer
 *  ------------
 *
 *  Functions Used:
 *  ---------------
 *
 *  Modification History:
 *  ---------------------
 *  Date       Vers  Auth   Details
 *  ----       ----  ----   -------
 *  04-Apr-00  2.1   RCB    Original version
 *  26-Jul-00  2.2   RCB    Update file header
 *
 *****************************************************************************/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)

#include "amssassert.h"

#include "dsucsdnti.h"
#include "dsucsdrlp.h"
#include "dsucsdv42.h"
#include "data_msg.h"

/* Flow control watermark levels */
#define TX_FULL_THRESHOLD    ((SYS_SYS_MODE_GSM == ds_ucsd_nt_get_curr_mode())? \
                              (MAX_USER_BYTES_PER_20_MSEC+10) : \
                              (MAX_USER_BYTES_PER_10_MSEC+10))
#define RX_FULL_THRESHOLD    ((SYS_SYS_MODE_GSM == ds_ucsd_nt_get_curr_mode())? \
                              (2*MAX_BUFFERED_USER_BYTES) : \
                              (2*MAX_BUFFERED_USER_BYTES_WCDMA))
#define RX_EMPTY_THRESHOLD   (MAX_BUFFERED_USER_BYTES)

/****************************************************************************
 * Module scope (private) variables
 ****************************************************************************/
static dsm_item_type * TxBuffer = NULL;
static dsm_item_type * RxBuffer = NULL;
static boolean ds_gcsd_v42_rx_buffer_full_flag = FALSE;



/****************************************************************************
 * TX buffer routines
 ****************************************************************************/

void ds_write_V42_tx_buffer (dsm_item_type ** PDU_ptr)
{
   /* Add passed PDU to working buffer (if any) */
   dsm_append (&TxBuffer, PDU_ptr);
}

void ds_write_to_V42_tx_buffer (byte code)
{
   /* Allocate DSM item if required */
   if (NULL == TxBuffer)
   {
      TxBuffer = ds_ucsd_UplinkPDUMgr_New();
   }
   if (NULL != TxBuffer )
   {
     DSM_PUSHDOWN_TAIL(&TxBuffer, &code, 1);
   }
   else
   {
     DS_UCSD_DEBUG_MSG ("V42 TX buffer allocation failed");
   }
}


uint32
ds_read_V42_tx_buffer 
(
   dsm_item_type ** PDU_ptr,
   uint32           num_bytes_to_read
)
{
  if(PDU_ptr == NULL)
  {
    DATA_MSG0_ERROR("Input to ds_read_V42_tx_buffer() is NULL");
    return 0;
  }

  /* Check for buffer available */
  if (NULL != TxBuffer)
  {
    byte  PDU[RLP_PDU_MAX_USER_DATA_SIZE];
    dsm_item_type * TxPDU = NULL;
    short buf_size = ds_check_V42_tx_buffer();
    short bytes_to_read = MIN((short)num_bytes_to_read,buf_size);
     
    /* Check for L2R status bytes required (WCDMA may have none) */
    if ( (RLP_CURRENT_MAX_DATA_BYTES != bytes_to_read) &&
         ((RLP_CURRENT_MAX_DATA_BYTES - L2R_MAX_STATUS_BYTES) < bytes_to_read) )
    {
      /* Worst case is for large PDU with 2 bytes status header and
      ** one trailing status byte.  Caller tells us only about the
      ** first header byte.
      */
      bytes_to_read = (short)(RLP_CURRENT_MAX_DATA_BYTES - L2R_MAX_STATUS_BYTES);
    }
     
    /* Extract single PDU from buffer */
    TxPDU = ds_ucsd_UplinkPDUMgr_New();
    /*lint -e666 */
    if(bytes_to_read > RLP_PDU_MAX_USER_DATA_SIZE)
    {
      DSM_PULLUP (&TxBuffer, PDU, RLP_PDU_MAX_USER_DATA_SIZE);
    }
    else
    {
      DSM_PULLUP (&TxBuffer, PDU, bytes_to_read);
    }
    if(NULL != TxPDU)
    {
      DSM_PUSHDOWN_TAIL (&TxPDU, PDU, (uint16)bytes_to_read);
      /*lint +e666 */
      dsm_append (PDU_ptr, &TxPDU);
    }
    else
    {
      DS_UCSD_DEBUG_MSG ("TxPDU DSM allocation failed");
    }
    /* Return actual number of bytes read */
    return dsm_length_packet(*PDU_ptr);
  }
  return 0;
}


boolean ds_check_full_V42_tx_buffer (void)
{
   return (TX_FULL_THRESHOLD < ds_check_V42_tx_buffer()) ? TRUE : FALSE;
}

boolean ds_check_empty_V42_tx_buffer (void)
{
   return ( (NULL == TxBuffer) ||
            (0 == ds_check_V42_tx_buffer()) ) ? TRUE : FALSE;
}

short ds_check_V42_tx_buffer (void)
{
   return (short)dsm_length_packet(TxBuffer);
}


void ds_init_V42_tx_buffer (void)
{
   /* Free DSM item currently assigned (if any) */
   if (NULL != TxBuffer)
   {
      ds_ucsd_UplinkPDUMgr_Free (&TxBuffer);
   }
}




/****************************************************************************
 * RX buffer routines
 ****************************************************************************/

void ds_write_V42_rx_buffer (dsm_item_type ** PDU_ptr)
{
   if(PDU_ptr == NULL)
   {
     DATA_MSG0_ERROR("Input to ds_write_V42_rx_buffer() is NULL");
     return;
   }

   ASSERT( NULL != *PDU_ptr );
  
   /* Add passed PDU to working buffer (if any) */
   dsm_append (&RxBuffer, PDU_ptr);
   *PDU_ptr = NULL;
}

short ds_read_from_V42_rx_buffer (byte * code)
{
   DSM_PULLUP (&RxBuffer, code, 1); /*lint !e666 */
   return 0;
}

short ds_check_V42_rx_buffer (void)
{
   return (short)dsm_length_packet(RxBuffer);
}


boolean ds_check_full_V42_rx_buffer (void)
{
   int size  = ds_check_V42_rx_buffer();
   ds_ucsd_nt_cb_type *ds_ucsd_nt_cb_info = NULL;

   ds_ucsd_nt_cb_info = ds_ucsd_nt_get_cb_info();

   // Do size management with watermarking
   if (ds_gcsd_v42_rx_buffer_full_flag)
   {
      if (size < RX_EMPTY_THRESHOLD)
      {
         DS_UCSD_DEBUG_MSG ("V42 RX buffer empty");
         ds_gcsd_v42_rx_buffer_full_flag = FALSE;
      }
   }
   
   else if (size > RX_FULL_THRESHOLD)
   {
      DS_UCSD_DEBUG_MSG ("V42 RX buffer full");
      ds_gcsd_v42_rx_buffer_full_flag = TRUE;
   }

   // If the V42 buffer isn't full, return whether or not the
   // RX buffer is filling up.  This will handle the case where
   // we are in online command mode but receiving data OTA
   if (ds_gcsd_v42_rx_buffer_full_flag)
   {
      return TRUE; //ds_gcsd_v42_rx_buffer_full_flag;
   }
   else
   {
     return (ds_ucsd_nt_cb_info->is_sio_tx_q_full_cb());
   }
}


boolean ds_check_empty_V42_rx_buffer (void)
{
   return ( (NULL == RxBuffer) ||
            (0 == ds_check_V42_rx_buffer()) ) ? TRUE : FALSE;
}

void ds_init_V42_rx_buffer (void)
{
   /* Free DSM item currently assigned (if any) */
   if (NULL != RxBuffer)
   {
      ds_ucsd_UplinkPDUMgr_Free (&RxBuffer);
   }
   ds_gcsd_v42_rx_buffer_full_flag = FALSE;
}

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */

