/*===========================================================================

              GSM Circuit Switched Data Remap Module

DESCRIPTION
  This file contains the implementation of the GSM Circuit switched 
  data Remap functionality.

   Copyright (c) 2002 - 2011 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdremap.c_v   1.2   12 Feb 2002 16:51:12   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdremap.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
09/29/09   SA      Fixed KW errors.
07/09/09   BS      Fixed offtarget lint medium and low's.
08/20/07   AR      Ensure RLP v2 frames are procesed correctly.
05/05/06   AR      Incorporate changes from BoaApps interRAT effort.
02/09/06   AR      Lint corrections.
10/19/04   AR      Flag error handling mode on REMAP timeout.
10/08/04   AR      Use constant two bytes for extracting N(R) in REMAP frame.
                   Adjust diag message level.
05/17/04   AR      Use return code from ProcessXIDResponse().
03/04/04   AR      Updates to support W2G handover
07/15/03   TMR     Added support for RLP Version 2
04/21/03   AR      Add support for using DSM items inplace of static buffers
                   Lint corrections
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/


/*****************************************************************************
 *
 *  File:            /home/briers/gsmqc/ds/SCCS/s.m51310.c
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
 *  Description:    Remap Pending Buffer
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
#include "dsm.h"

#include "data_msg.h"
#include "dsucsdnti.h"
#include "dsucsdrlp.h"
#include "dsucsdv42.h"

#define FULL_THRESHOLD    100
#define RLP_REMAP_INFO_NR_SIZE 2

#ifdef BOA_APPS
/* Macro to change endianess for uint16 */
#define ENDIAN_SWAP_16_BIT(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
                  (((uint16)(x) & 0xff00) >> 8)))
#endif /* BOA_APPS */

/****************************************************************************
 * Module scope (private) variables
 ****************************************************************************/
static dsm_item_type * Buffer = NULL;
static dsm_item_type * RemapCommandPDU = NULL;

static short HandleREMAPResponse (dsm_item_type* RemapResponse);

/****************************************************************************
 * buffer routines
 ****************************************************************************/

void ds_write_remap_pending_buffer (dsm_item_type ** PDU_ptr)
{
  DS_UCSD_DEBUG_MSG ("In ds_write_remap_pending_buffer");
    
  /* Clear the status octet absent indicator if set */
  if ((*PDU_ptr)->app_field & RLP_DSM_APP_FIELD_NO_STATUS_BYTE)
  {
    (*PDU_ptr)->app_field &= ~RLP_DSM_APP_FIELD_NO_STATUS_BYTE;
    DATA_MSG0_LOW("Cleared app_field NO_STATUS flag");
  }

  /* Add passed PDU onto working buffer (if any) */
  dsm_append (&Buffer, PDU_ptr);
}

/*ARGSUSED*/
uint32
ds_read_remap_pending_buffer 
(
   dsm_item_type ** PDU_ptr,
   uint32           num_bytes_to_read
)
{
   word  PDUsize = current_rlp_version_is_2()?
                   RLP_CURRENT_MAX_DATA_BYTES : (RLP_CURRENT_MAX_DATA_BYTES - 1);
  
   ASSERT( NULL != PDU_ptr );
  
   DATA_MSG0_MED( "In ds_read_remap_pending_buffer");

   /* Check for buffer available */
   if (NULL != Buffer)
   {
     byte  PDU[RLP_PDU_MAX_USER_DATA_SIZE];
     word  size;
        
     /* Extract single PDU of data from buffer */
     dsm_item_type * RmPDU = ds_ucsd_UplinkPDUMgr_New();
     if(NULL == RmPDU)
     {
       return 0;
     }
     size = dsm_pullup( &Buffer, PDU, PDUsize );
     if( 0 < size)
     {
       DATA_MSG1_MED("Extracted data from Remap buffer: size=%d",size);

       if(size != dsm_pushdown_tail( &RmPDU, PDU, size, DSM_ITEM_POOL(RmPDU) ))
       {
         DATA_MSG1_ERROR("DSM pushdown size mismatch %d",size);
       }
       dsm_append (PDU_ptr, &RmPDU);  /* nulls RmPDU */
     }
   }
   else
   {
      DATA_MSG0_MED( "Remap buffer empty during read attempt!");
   }

   PDUsize = (word)(( NULL != *PDU_ptr )? dsm_length_packet(*PDU_ptr) : 0);
   MSG_MED ("Read Remap buffer returns: %d", PDUsize,0,0);
   return PDUsize;
}

boolean ds_check_full_remap_pending_buffer (void)
{
   return (FULL_THRESHOLD < ds_check_remap_pending_buffer())? TRUE : FALSE;
}

void ds_init_remap_pending_buffer (void)
{
   /* Free DSM item currently assigned (if any) */
   if (NULL != Buffer)
   {
      ds_ucsd_UplinkPDUMgr_Free (&Buffer);
   }
}

short ds_check_remap_pending_buffer (void)
{
   return (short)dsm_length_packet(Buffer);
}

boolean ds_check_empty_remap_pending_buffer (void)
{
   return ( (NULL == Buffer) ||
            (0 == ds_check_remap_pending_buffer()) ) ? TRUE : FALSE;
}


/****************************************************************************
 * Function name:  ActionMSRemapping
 * -------------
 * Description:    Action routine for state MS Remapping
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
/*ARGSUSED*/
void ActionMSRemapping (DSI_Event_T Event)
{
   short status;
   RLP_Frame_T *RxFrame = NULL;

   RxFrame = ds_ucsd_get_RxFrame();

   if (ds_ucsd_get_rlp_process_downlink()) 
   {
      status = ReadRLPFrame (RxFrame);
      
      if ((status == RLP_OK) && 
          ((ds_ucsd_get_FrameType()) == FRAMETYPE_UNNUMBERED))
      {
         switch (ds_ucsd_get_UnNumberedFrame())
         {
            case DISC: 
               STOP_TIMER (TIMERID_T1);
               STOP_TIMER (TIMERID_REMAP);
               ds_ucsd_set_UA_State(STATE_SEND);
               ds_ucsd_set_UA_FBit(ds_ucsd_get_PF());
               WRITE_EVENT_TO_L2R (RLP_DISC_IND);
               ds_ucsd_set_RlpCurrentState(RLPSTATE_ADM_ATTACHED);
               ds_ucsd_set_UplinkRemapped(FALSE);                  
               break;   
               
            case SABM:  
               STOP_TIMER (TIMERID_T1);
               STOP_TIMER (TIMERID_REMAP);
               WRITE_EVENT_TO_L2R (RLP_RESET_IND);
               ds_ucsd_set_RlpCurrentState(RLPSTATE_PENDING_RESET_INDICATION);
               ds_ucsd_set_UplinkRemapped(FALSE);                  
               break;   
               
            case REMAP:
               if (!ds_ucsd_get_CR())
               {
                  status = HandleREMAPResponse (RxFrame->Data);
                  if (status == RLP_OK)
                  {
                     STOP_TIMER (TIMERID_T1);
                     STOP_TIMER (TIMERID_REMAP);
                     WRITE_EVENT_TO_L2R (RLP_REMAP_CNF);
                     ds_ucsd_set_RlpCurrentState(RLPSTATE_CONNECTION_ESTABLISHED);
                     /* Reset remap flag for next channel coding change */
                     ds_ucsd_set_UplinkRemapped(FALSE);
                  }
               }
               break;
         
            case UI:    HandleUI    (RxFrame);  break;
            case _TEST: HandleTEST  (RxFrame);  break;  
            default:    break;    /* Making Lint happy */
         }
      }
   }

     
   /* IF Timeout Occurred, terminate the call */
   if (TIMER_EXPIRED (TIMERID_REMAP))
   {
         DS_UCSD_DEBUG_MSG ("RLP:REMAP T.out");
         ds_ucsd_set_rlp_error_handling(TRUE);
         
         STOP_TIMER (TIMERID_REMAP);
         ds_ucsd_set_RlpCurrentState(RLPSTATE_ADM_ATTACHED);
   }

   
   if ((ds_ucsd_get_rlp_process_uplink()) && (ds_HardwareTxReady()))
   {
      /* Now send any frames if required */ 
      status = Send_TXU();
      if (status == 0)
      {
         if (ds_ucsd_get_UA_State() == STATE_SEND) 
         {
            status = SendUnNumberedFrame (UA, FALSE, ds_ucsd_get_UA_FBit(), NULL);
            ds_ucsd_set_UA_State(STATE_IDLE); 
         }
         else
         {
            dsm_item_type * Remap_ptr = ds_ucsd_UplinkPDUMgr_New();
            if (NULL == Remap_ptr)
            {
              DATA_MSG0_ERROR("Failed to allocate dup DSM for remap cmd");
            }
            else
            {
              /* Copy REMAP cmd packet into new DSM item */
              /*FIXME: performance issue with multiple extracts */
              byte tmpbuf[RLP_PDU_MAX_USER_DATA_SIZE];
              word PDUsize;
              
              PDUsize = (word)dsm_length_packet(RemapCommandPDU);
              DSM_EXTRACT(RemapCommandPDU, 0, tmpbuf, PDUsize); /*lint !e666 */
              DSM_PUSHDOWN_TAIL (&Remap_ptr, tmpbuf, PDUsize); 
            }
            status = SendUnNumberedFrame (REMAP, TRUE, FALSE, Remap_ptr);
            Remap_ptr = NULL;
         }
      }
   }
}

/****************************************************************************
 * Function name:  MakeRemapFrame
 * -------------
 * Description:    Build a remap frame to send to remote RLP.
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
void MakeRemapFrame (short _Nr, XID_Params_T* pXID)
{
   DS_UCSD_DEBUG_MSG_1 ("MakeRemapFrame: Nr = %d", _Nr);

   /* Allocate PDU for Remap Frame */
   RemapCommandPDU = ds_ucsd_UplinkPDUMgr_New();
   if(NULL == RemapCommandPDU)
   {
     return;
   }

#ifndef BOA_APPS
   /* Put the Downlink Nr into the REMAP PDU (see 04.22 para 5.2.2.9) */
   DSM_PUSHDOWN_TAIL (&RemapCommandPDU, (short*)&_Nr, sizeof(short));
#else
   {
     short temp = ENDIAN_SWAP_16_BIT(_Nr);

     /* Put the Downlink Nr into the REMAP PDU (see 04.22 para 5.2.2.9) */
     DSM_PUSHDOWN_TAIL (&RemapCommandPDU, (short*)&temp, sizeof(short));
   }
#endif /* BOA_APPS */

   if (pXID)
   {
      ds_ucsd_set_Negotiate_XID_ADM_Values(TRUE);
     
      /* Put the XID parameters into the REMAP PDU (see 04.22 para 5.2.2.9) */
      XID_to_PDU (&RemapCommandPDU, pXID);
   }
   else
   {
      byte tmp = 0x00;
      
      /* Terminate the PDU parameter list */
      ASSERT ( NULL != RemapCommandPDU );
      DSM_PUSHDOWN_TAIL (&RemapCommandPDU, &tmp, 1);
   }
}


/****************************************************************************
 * Function name:  HandleREMAPResponse
 * -------------
 * Description:    This routine is called to handle a REMAP response
 * -----------     from the network. 
 *
 * Returns:        RLP_OK on successful processing; RLP_ERROR otherwise.
 * -------         
 ****************************************************************************/
static short HandleREMAPResponse (dsm_item_type * RemapResponse)
{
   short status = RLP_OK;
   short _Nr = 0;

   /* First two bytes give N(R) value */
   DSM_PULLUP (&RemapResponse, (byte*)&_Nr, RLP_REMAP_INFO_NR_SIZE); /*lint !e666 */
   
   DS_UCSD_DEBUG_MSG_1 ("In HandleREMAPResponse: Nr = %d", _Nr);

   if (!ds_ucsd_get_UplinkRemapped() && 
       (_Nr != ds_ucsd_get_UplinkRemappedLastNr()))
   {
     /* Remap the UL (Tx) PDU manager */
     status = ds_ucsd_UplinkPDUMgr_Remap (ds_ucsd_get_CurrentPDUType(), _Nr);
     if( status == RLP_OK )
     {
       ds_ucsd_set_UplinkRemapped(TRUE);
       ds_ucsd_set_UplinkRemappedLastNr(_Nr);

       /* Get the new values for the XID parameters */
       if ( FALSE == ProcessXIDResponse (RemapResponse, 
                                         ds_ucsd_get_current_xid()) )
       {
         status = RLP_ERROR;
       }

       /* Cleanup the Remap cmd packet*/
       dsm_free_packet(&RemapCommandPDU);

       DS_UCSD_DEBUG_MSG ("HandleREMAPResponse: initiated remap state");
     }
   }
   
   return status;
}





/****************************************************************************
 * The following routines provide the functionality of the Network end
 * of the RLP link, for the REMAP procedure. They will therefore be
 * conditionally compiled for the UNIX testharness only, and not the 
 * production MS code.
 ****************************************************************************/


#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */


