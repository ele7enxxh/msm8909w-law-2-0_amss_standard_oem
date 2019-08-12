/*===========================================================================

              GSM Circuit Switched Data XXXX Module

DESCRIPTION
  This file contains the implementation of the GSM Circuit switched 
  data XXXX functionality.

Copyright (c) 2002 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdulpdu.c_v   1.3   21 Mar 2002 10:56:54   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdulpdu.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
03/05/10   SA      Ignore N(r) if it is acknowledging a 
                   frame whose 2nd half is not yet send.
10/21/09   SA      Fixed offtarget lint errors.
01/04/08   SA      Change the PDU state to SEND only if previous state was WAIT.
08/15/07   AR      Ensure VA matches VD before clearing old PDU list on REMAP.
02/23/07   DA      Fixed high lint errors
02/09/07   DA      Fixed critical lint errors
05/26/06   AR      Ensure OldList cleared on REMAP is queue empty.
02/09/06   AR      Lint corrections.
03/24/04   AR      Lint cleanup.
03/10/04   AR      Remove spurious flow control messages.
12/11/03   TMR     Added checks for 'while' loops to make sure we don't get 
                   into any infinite loops
11/18/03   AR      Added ds_ucsd_UplinkPDUMgr_GetType()
09/18/03   AR      Removed obsolete code block.
07/15/03   TMR     Added support for RLP Version 2
04/16/03   AR      Add suppport for DSM items inplace of local buffers
                   Lint corrections
01/20/03   TMR     Fixed bug in ds_ucsd_UplinkPDUMgr_ReadOutOfSeq() where 
                   incorrect frame was marked as PDU_STATE_WAIT 
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/


/*****************************************************************************
 *
 *  File:            /home/briers/gsmqc/ds/SCCS/s.m51306.c
 *  -----
 *
 *  SCCS Version:    2.3
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
 *  Description:    This is the L2R/RLP Uplink PDU manager.
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
#include "ds_3gppi_utils.h"

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)


/* Turn debugging off in this file */
/* #define DS_GCSD_FILE_DEBUG_OFF */
#include "dsucsdnti.h"

#include "dsucsdrlp.h"
#include "memory.h"
#include "amssassert.h"


/****************************************************************************
 * Module Scope variables 
 ****************************************************************************/
static PDU_List_T large_PDU_data_list = {NULL};
static PDU_List_T small_PDU_data_list = {NULL};

static Tx_PDU_list_T LargePDUList = {PDU_TYPE_INVALID, 0, 0, 0, 0, NULL};
static Tx_PDU_list_T SmallPDUList = {PDU_TYPE_INVALID, 0, 0, 0, 0, NULL};

static Tx_PDU_list_T* CurrentList = NULL;
static Tx_PDU_list_T* OldList = NULL;

#ifndef FEATURE_DS_CSNT_DYNAMIC_ALLOCATION
/* Statically allocate maximum storage required (WCDMA) for PDU Manager */
static dsm_item_type * UL_small_PDU_data_list[RLP_M_VER_2] = {NULL};
static dsm_item_type * UL_large_PDU_data_list[RLP_M_VER_2] = {NULL};

static byte UL_small_state_list[RLP_M_VER_2] = {0};
static byte UL_large_state_list[RLP_M_VER_2] = {0};
#endif /* FEATURE_DS_CSNT_DYNAMIC_ALLOCATION */


#ifdef DEBUG
char buf[100];
#endif

/****************************************************************************
 * Module Scope functions
 ****************************************************************************/
static void InitialiseList
(
   Tx_PDU_list_T* List,
   short          Index,
   boolean        Allocate
);


/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_RemapDataPending
 * -------------
 * Description:    This routine returns whether or not there is data
 * -----------     left in the OldList after a REMAP. The L2R must
 *                 retrieve this data and re-send it, it will then 
 *                 be sent over the RLP via the new CurrentList.
 * Returns:        TRUE/FALSE
 * -------         
 ****************************************************************************/
boolean ds_ucsd_UplinkPDUMgr_RemapDataPending (void)
{
   boolean RetVal = FALSE;

   if (OldList)
   {
      if (OldList->VA != OldList->VD)
      {
         RetVal = TRUE;
      }
      else
      {
        /* Once list is empty of REMAP frames, delete the list */
        OldList = NULL;
      }
   }
   
   /* DS_UCSD_DEBUG_MSG_1 ("ULPDUMgr_RemapDataPending: RetVal=",RetVal); */
   
   return RetVal;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_Retrieve
 * -------------
 * Description:    This routine retrives a PDU from the old list.
 * -----------     It is called by the L2R to retrieve a PDU after a REMAP.
 *                 The PDU is then decoded and re-encoded into the new
 *                 current PDU format.
 * Returns:        ERROR, or Type of PDU returned
 * -------         
 ****************************************************************************/
PDU_Type ds_ucsd_UplinkPDUMgr_Retrieve (dsm_item_type ** PDU_ptr)
{
   PDU_Type RetVal = PDU_TYPE_INVALID;

   if(PDU_ptr == NULL)
   {
     DATA_MSG0_ERROR("Null ptr to PDU in retrieve func");
     return PDU_TYPE_INVALID;
   }

   /* NOTE: When a REMAP occurs, the VA in the old list is set to 
    *       the value of the N(r) in the REMAP frame received from the
    *       remote node. We must send the I frames from this value onward. */

   if (OldList)
   {
      DS_UCSD_DEBUG_MSG_3 ("ULPDUMgr_Retrieve: OldList Type=%d VA=%d VD=%d",
                           OldList->Type, OldList->VA, OldList->VD);
      if (OldList->VA != OldList->VD)
      {
         if (OldList->State[OldList->VA] == PDU_STATE_IDLE)
         {
            DS_UCSD_DEBUG_MSG_1 (
               "ds_ucsd_UplinkPDUMgr_Retrieve:WARNING: State IDLE at VA=%d",
               OldList->VA);
         }

         /* Assign the source PDU from OLD list */
         *PDU_ptr =  ds_ucsd_UplinkPDUMgr_Release (OldList, OldList->VA);
         OldList->State[OldList->VA] = PDU_STATE_IDLE;
         
         OldList->VA = INC_SEQ_NUM (OldList->VA);
         RetVal = OldList->Type;

         /* IF all the old data read then delete the list */
         if (OldList->VA == OldList->VD)
         {
            DS_UCSD_DEBUG_MSG ("ULPDUMgr_Retrieve: Set OldList=NULL");
            OldList = NULL;
         }
      }
   }
   return RetVal;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_SendSlotsAvailable
 * -------------
 * Description:    This routine 
 * -----------     
 * Returns:        TRUE/FALSE
 * -------         
 ****************************************************************************/
boolean  ds_ucsd_UplinkPDUMgr_SendSlotsAvailable (void)
{
   int32 size;
   int32 threshold;
   boolean RetVal = FALSE;
   uint16  current_rlp_win_size;

   size = (CurrentList->VD)-(CurrentList->VA);
   if (size < 0)
      size += get_current_rlp_max_window_size();;

   current_rlp_win_size = get_current_rlp_neg_tx_window_size();   
   if (current_rlp_version_is_2())
   {
     threshold = MIN((current_rlp_win_size + 20), RLP_M_VER_2);
   }
   else
   {
     threshold = MIN(current_rlp_win_size, RLP_M_VER_0_1);
   }
      
   if (size < threshold)
   {
      RetVal = TRUE;
   }
   else
   {
      RetVal = FALSE;
   }
   
#ifdef DS_UCSD_DEBUG
   {
      static short old_vd=999;
      
      if (RetVal == FALSE)
      {      
         if (old_vd != CurrentList->VD)
         {
            DS_UCSD_DEBUG_MSG_3 ("RLP:No Tx slot! size=%d VD%d VA%d",
                                 size, CurrentList->VD, CurrentList->VA);
            old_vd = CurrentList->VD;
         }
      }
   }
#endif
   
   return RetVal;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_Write
 * -------------
 * Description:    This routine inserts the passed PDU into the current list
 *                 at the in-sequence position. The PDU state is set to SEND.
 *                 The input PDU pointer parameter is set to NULL.
 *                 The VD sequence index is incremented.
 * -----------     
 * Returns:        ERROR, PDU type written (if OK)
 * -------         
 ****************************************************************************/
PDU_Type ds_ucsd_UplinkPDUMgr_Write (dsm_item_type ** PDU_ptr)
{
   PDU_Type status = PDU_TYPE_INVALID;

   if(PDU_ptr == NULL)
   {
     DATA_MSG0_ERROR("Null ptr to PDU in write func");
     return PDU_TYPE_INVALID;
   }

   ASSERT (*PDU_ptr != NULL);

   if (CurrentList->State[CurrentList->VD] == PDU_STATE_IDLE)
   {
      dsm_item_type ** PDUList = *PDU_LIST_PTR(CurrentList);
      
      /*MSG_LOW ("Writing PDU at Uplink List: %d[%d] 0x%x",
                 CurrentList->Type, CurrentList->VD,*PDU_ptr); */

      /* Insert PDU into the current list to send */
      PDUList[CurrentList->VD] = *PDU_ptr;
      *PDU_ptr = NULL;
      CurrentList->State[CurrentList->VD] = PDU_STATE_SEND;

      /* Increment the state pointer */
      CurrentList->VD = INC_SEQ_NUM(CurrentList->VD);
    
      status = CurrentList->Type;
   }
   else
   {
      DS_UCSD_DEBUG_MSG_2 ("ULPDUMgr_Write error: VD=%d, State[VD]=%d",
                           CurrentList->VD,
                           CurrentList->State[CurrentList->VD]);
   }
   
   return status;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_DataToSend
 * -------------
 * Description:    This routine
 * -----------     
 * Returns:        TRUE/FALSE
 * -------         
 ****************************************************************************/
boolean ds_ucsd_UplinkPDUMgr_DataToSend (void)
{
   if (CurrentList->State[CurrentList->VS] == PDU_STATE_SEND)
      return TRUE;
   else
      return FALSE;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_Read
 * -------------
 * Description:    This routine reads next PDU from the Uplink list.
 * -----------     
 * Returns:        ERROR, or size of PDU read
 * -------         
 ****************************************************************************/
dsm_item_type *  ds_ucsd_UplinkPDUMgr_Read ( void )
{
   dsm_item_type * PDU_ptr = NULL;
   
   if (CurrentList->State[CurrentList->VS] == PDU_STATE_SEND)
   {
      dsm_item_type ** PDUList = *PDU_LIST_PTR(CurrentList);

      PDU_ptr = PDUList[CurrentList->VS];
      
      CurrentList->State[CurrentList->VS] = PDU_STATE_WAIT;
      
      CurrentList->VS = INC_SEQ_NUM (CurrentList->VS);

      /* DS_UCSD_DEBUG_MSG_1 ("ULPDUMgr_Read from CurrList:%d",CurrentList->VS); */
   }
   
   return PDU_ptr;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_ReadOutOfSeq
 * -------------
 * Description:    This routine reads from the tx list from a specified
 * -----------     index. 
 * Returns:        NULL, or ptr to PDU
 * -------         
 ****************************************************************************/
dsm_item_type * ds_ucsd_UplinkPDUMgr_ReadOutOfSeq (short Index)
{
   dsm_item_type * PDU_ptr = NULL;

   if (CurrentList->State[Index] == PDU_STATE_SEND)
   {
      dsm_item_type ** PDUList = *PDU_LIST_PTR(CurrentList);

      PDU_ptr = PDUList[Index];
      
      CurrentList->State[Index] = PDU_STATE_WAIT;
      
      DS_UCSD_DEBUG_MSG_2 ("ds_ucsd_UplinkPDUMgr_ReadOutOfSeq index=%d,ptr=%x",Index,PDU_ptr);
   }
   else
   {
      DS_UCSD_DEBUG_MSG_2 ("ReadOutOfSeq fail: idx=%d, st=%d",Index,CurrentList->State[Index]);
   }
   
   return PDU_ptr;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_Remap
 * -------------
 * Description:    This routine
 * -----------     
 * Returns:        ERROR, OK
 * -------         
 ****************************************************************************/
short ds_ucsd_UplinkPDUMgr_Remap (PDU_Type Type, short Index)
{
   short RetVal = RLP_OK;

   DS_UCSD_DEBUG_MSG_2 ("In ds_ucsd_UplinkPDUMgr_Remap: Type=%d Index=%d",
                        Type, Index);

   /* The supplied Index is the N(r) value received in the REMAP 
    * command from the remote node. We can then assume that the 
    * current VA value can be brought up to this value (if it isn't
    * already) and that the remote node requires sending from this
    * value forwards in the new format. */

   /* IF the requested PDU size is different to that currently being used */
   if (Type != CurrentList->Type)
   {
      if (OldList)
      {
         /* IF the old list is not empty then we have not finished
            with the last Remap! The RLP cannot REMAP without losing
            some data - it will issue a link reset */
         RetVal = RLP_ERROR;
         
         DS_UCSD_DEBUG_MSG ("ds_ucsd_UplinkPDUMgr_Remap : returning ERROR!");
      }
      else
      {
         /* Freeze the Current List */
         OldList = CurrentList;

         DS_UCSD_DEBUG_MSG_1 ("ds_ucsd_UplinkPDUMgr_Remap: Remap Index= %d", Index);
         DS_UCSD_DEBUG_MSG_3 ("ds_ucsd_UplinkPDUMgr_Remap: VA=%d VS=%d VD=%d",
                              OldList->VA, OldList->VS, OldList->VD);
         DS_UCSD_DEBUG_MSG_2 ("ds_ucsd_UplinkPDUMgr_Remap: Moving VA from %d to %d",
                              OldList->VA, Index);
         
         OldList->VA = Index;


         /* Point to the new list */
         if (Type == PDU_TYPE_SMALL)
         {
            CurrentList = &SmallPDUList;
            DS_UCSD_DEBUG_MSG ("ds_ucsd_UplinkPDUMgr_Remap: Going to SMALL PDUs");
         }
         else
         {
            CurrentList = &LargePDUList;
            DS_UCSD_DEBUG_MSG ("ds_ucsd_UplinkPDUMgr_Remap: Going to LARGE PDUs");
         }

         /* Start the new list from the current VR */
         InitialiseList (CurrentList, Index, TRUE);
         
         DS_UCSD_DEBUG_MSG_1 ("ds_ucsd_UplinkPDUMgr_Remap: Starting new list from: %d",Index);

         /* If all the old data acknowledged then delete the list */
         if (OldList->VD == OldList->VA)
         {
            DS_UCSD_DEBUG_MSG ("ds_ucsd_UplinkPDUMgr_Remap: Setting OldList=NULL");
            OldList = NULL;
         }
      }
   }
   
   return RetVal;
}


/****************************************************************************
 * Function name:  Decrease_VS_to_NR
 * -------------
 *
 * Description:    This routine rewinds the send pointer VS to the received 
 * -----------     NR. This has the effect of sending frames from the new
 *                 VS (i.e. re-sending some frames).
 *
 * Returns:        None
 * -------         
 ****************************************************************************/
void ds_ucsd_UplinkPDUMgr_DecreaseVStoNR (short _Nr)
{
   DS_UCSD_DEBUG_MSG ("RLP:Dec VS->NR");

   /* Make sure loop values are sane */
   if ((_Nr < 0) || (_Nr > get_current_rlp_max_frame_number()) ||
       (CurrentList->VS > get_current_rlp_max_frame_number()))
   {
      ERR ("ULPDUMgr: DecVStoNR, illegal N(r)=%d or VS=%d",
                              _Nr,CurrentList->VS,0);

      WRITE_EVENT_TO_L2R (RLP_ERROR_IND);
      return;
   }
   while (CurrentList->VS != _Nr)
   {
      CurrentList->VS = DEC_SEQ_NUM(CurrentList->VS);
      /* Change the PDU state to SEND only if previous state is WAIT */
      if (CurrentList->State[CurrentList->VS] == PDU_STATE_WAIT)
      {
        CurrentList->State[CurrentList->VS] = PDU_STATE_SEND;
      }
   }

   DS_UCSD_DEBUG_MSG_2 ("VS=%d Nr=%d", CurrentList->VS, _Nr);
           
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_AdvanceVAtoNR 
 * -------------
 * Description:    This routine advances the value of VA upto the latest
 * -----------     valid received Nr. This acknowledges the frames.
 * Returns:        None
 * -------
 ****************************************************************************/
void ds_ucsd_UplinkPDUMgr_AdvanceVAtoNR (short _Nr)
{

   short Nr_to_update = _Nr;

  /*MSG_LOW ("In ds_ucsd_UplinkPDUMgr_AdvanceVAtoNR: Nr=%d VA=%d",
             _Nr, CurrentList->VA,0); */

   /* Make sure loop values are sane */
   if ( (_Nr < 0) || (_Nr > get_current_rlp_max_frame_number()) ||
        (CurrentList->VA > get_current_rlp_max_frame_number()) )
   {
      DATA_MSG2_ERROR("ULPDUMgr: AdvanceVAtoNR, illegal N(r)=%d or VA=%d",
                              _Nr,CurrentList->VA);

      WRITE_EVENT_TO_L2R (RLP_ERROR_IND);
      return;
   }

   if( (_Nr == ds_ucsd_UplinkPDUMgr_VS()) && !(ds_HardwareTxReady()))
   { 
     if( DEC_SEQ_NUM(_Nr) >= CurrentList->VA )
     {
       /* If we recieve a frame which is not completely sent,
       ** Update PDU state and free upto (_Nr-1) 
       */
       Nr_to_update = DEC_SEQ_NUM(_Nr);
     }
     else
     {
       /* Suppress PDU release until next ACK received */
       Nr_to_update = CurrentList->VA;
     }
   }
   while (CurrentList->VA != Nr_to_update)
   {
      dsm_item_type * PDU = NULL;

      /* Release PDU buffer */
      PDU = ds_ucsd_UplinkPDUMgr_Release (CurrentList, CurrentList->VA);
      ds_ucsd_UplinkPDUMgr_Free (&PDU);

      CurrentList->State[CurrentList->VA] = PDU_STATE_IDLE;
      CurrentList->VA = INC_SEQ_NUM (CurrentList->VA);
   }
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_Initialise
 * -------------
 * Description:    This routine initialises the variables in the module
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
void ds_ucsd_UplinkPDUMgr_Initialise (PDU_Type Type, boolean Allocate)
{
   if (Type == PDU_TYPE_SMALL)
   {
      CurrentList = &SmallPDUList;
   }
   else
   {
      CurrentList = &LargePDUList;
   }

   OldList = NULL;

   InitialiseList (CurrentList, 0, Allocate);
   InitialiseList (OldList, 0, Allocate);
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_Empty
 * -------------
 * Description:    Empties the current list at it's current VS 
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
void ds_ucsd_UplinkPDUMgr_Empty (void)
{
   OldList = NULL;
   InitialiseList (CurrentList, CurrentList->VS, TRUE);
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_IsEmpty
 * -------------
 * Description:    TRUE if Current List empty
 * -----------     
 * Returns:        TRUE/FALSE
 * -------         
 ****************************************************************************/
boolean ds_ucsd_UplinkPDUMgr_IsEmpty (void)
{
   if (CurrentList->VA == CurrentList->VD)
      return TRUE;
   else
      return FALSE;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_AllFramesAcknowledged
 * -------------
 * Description:    TRUE if all frames Acknowledged.
 * -----------     
 * Returns:        TRUE/FALSE
 * -------         
 ****************************************************************************/
boolean ds_ucsd_UplinkPDUMgr_AllFramesAcknowledged (void)
{
   if (CurrentList->VA == CurrentList->VS)
   {
      return TRUE;
   }
   return FALSE;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_MarkForRetransmission
 * -------------
 * Description:    Mark a PDU for re-transmission.
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
void ds_ucsd_UplinkPDUMgr_MarkForRetransmission (short Index)
{
   if (CurrentList->State[Index] == PDU_STATE_WAIT) /* Added in v2.3 */
   {
      CurrentList->State[Index] = PDU_STATE_SEND;
      DS_UCSD_DEBUG_MSG_1 ("RLP:Txl[%d]=SEND!", Index);
   }
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_SREJDataToTransmit
 * -------------
 *
 * Description:    This routine returns whether there is a data PDU 
 * -----------     to send due to an SREJ from the remote node.
 *
 * Returns:        RLP_ERROR: Nothing to send
 * -------         short:     SeqNum of Frame to re-send
 ****************************************************************************/
short ds_ucsd_UplinkPDUMgr_SREJDataToTransmit (void)
{
   short RetVal = RLP_ERROR;
   short Index;
   boolean Loop = TRUE;

   if (CurrentList->VA != CurrentList->VS)
   {
      /* Make sure loop values are sane */
      if ( (CurrentList->VS > get_current_rlp_max_frame_number()) ||
           (CurrentList->VA > get_current_rlp_max_frame_number()) )
      {
         DATA_MSG2_ERROR("ULPDUMgr: SREJDataToTransmit, illegal VS=%d or VA=%d",
                                    CurrentList->VS,CurrentList->VA);

         WRITE_EVENT_TO_L2R (RLP_ERROR_IND);
      }
      else
      {
         Index = CurrentList->VA;
         while ((Index != CurrentList->VS) && (Loop))
         {
            if (CurrentList->State[Index] == PDU_STATE_SEND)    
            {
               RetVal = Index;
               Loop = FALSE;
            }
            Index = INC_SEQ_NUM (Index);
         }
      }
   }

   if (RetVal != RLP_ERROR)
   {
      DS_UCSD_DEBUG_MSG_1 ("SREJ data to Xmit=%d",RetVal);
   }
   return RetVal;
}


/****************************************************************************
 * Function name:  TransmitWindowOpen
 * -------------
 * Description:    This routine returns TRUE if the transmit window
 * -----------     is open (ie that means that the number of outstanding 
 *                 non-ack'd frames is less than the max window size).
 * Uses:
 * ----
 * Parameters:     None
 * ----------
 * Returns:        TRUE = Window Open/FALSE = Window Closed
 * -------
 ****************************************************************************/
boolean ds_ucsd_UplinkPDUMgr_TransmitWindowOpen (void)
{
   boolean RetVal = FALSE;
   short Size;
   XID_Params_T *Rlp_CurrentXID = NULL;

   Rlp_CurrentXID = ds_ucsd_get_current_xid();

   Size = CurrentList->VS-CurrentList->VA;
   if (Size < 0)
   {
      Size += get_current_rlp_max_window_size();;
   }

   if (Size < Rlp_CurrentXID->MStoIWFWindow) /*was <=, cetecom! */
   {
      RetVal = TRUE;
   }
   else
   {
      DS_UCSD_DEBUG_MSG_2 ("RLP:TxWnd Closed!(VS=%d, VA=%d)", CurrentList->VS, CurrentList->VA);
   }
   return RetVal;
}

/****************************************************************************
 * Function name:  void ds_ucsd_UplinkPDUMgr_ColdStart
 * -------------
 * Description:    Cold start the module.
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
void ds_ucsd_UplinkPDUMgr_ColdStart (PDU_Type Type)
{
   /* These fields in the PDU list structures are FIXED - they
      need to be initialised at start-up time. */
   SmallPDUList.Type       = PDU_TYPE_SMALL;
   LargePDUList.Type       = PDU_TYPE_LARGE;

   ds_ucsd_UplinkPDUMgr_Initialise (Type, TRUE);
}


/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_PwrUp_Init
 * -------------
 * Description:    This routine initialises the PDU Mgr at powerup.
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
void  ds_ucsd_UplinkPDUMgr_PwrUp_Init ( void )
{
   /* Required for dynamic memory allocation */
   /* Clear memory used by state tracking */
   memset ((void*)&SmallPDUList, 0x00, sizeof(Tx_PDU_list_T));
   memset ((void*)&LargePDUList, 0x00, sizeof(Tx_PDU_list_T));

   /* Initialize PDU list pointers */
   small_PDU_data_list = NULL;
   large_PDU_data_list = NULL;
}


/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_PwrUp_Cleanup
 * -------------
 * Description:    This routine cleans up list on call end.
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
void  ds_ucsd_UplinkPDUMgr_Cleanup ( void )
{
   /* Cleanup both PDU lists */
   ds_ucsd_UplinkPDUMgr_Initialise (PDU_TYPE_SMALL, FALSE);
   ds_ucsd_UplinkPDUMgr_Initialise (PDU_TYPE_LARGE, FALSE);
}


/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_VS
 * -------------
 * Description:    Returns the value of VS
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
uint16 ds_ucsd_UplinkPDUMgr_VS (void)
{
   return CurrentList->VS;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_VA
 * -------------
 * Description:    Returns the value of VA
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
uint16 ds_ucsd_UplinkPDUMgr_VA (void)
{
   return CurrentList->VA;
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_State
 * -------------
 * Description:    Returns state of specified PDU
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
byte ds_ucsd_UplinkPDUMgr_State (short Index)
{
   return (CurrentList->State[Index]);
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_GetType
 * -------------
 * Description:    Returns PDU type of currrent PDU list
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
PDU_Type ds_ucsd_UplinkPDUMgr_GetType (void)
{
   return (CurrentList->Type);
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_New
 * -------------
 * Description:    Allocates a new DSM item from small pool to be used as
 *                 a PDU buffer. For Uplink, the data buffer is offset to
 *                 leave room for RLP header to be added later.
 * -----------     
 * Returns:        DSM item pointer
 * -------         
 ****************************************************************************/
dsm_item_type * ds_ucsd_UplinkPDUMgr_New ( void )
{
   dsm_item_type * PDU_ptr = NULL;
   int32 free_cnt = DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL); /*lint !e10 !e26*/

   /* Check availability of DSM items.                   */
   /* If FEW threshold exceeded, enable Tx flow control. */
   if ( (!ds_ucsd_nt_get_DSM_Level_Low() ) &&
        ( RLP_DSM_MEM_LEVEL_FEW > free_cnt ) )
   {
      DS_UCSD_DEBUG_MSG_2 ("Enabling DSM level TX Flow Control: %d > %d",
                           RLP_DSM_MEM_LEVEL_FEW, free_cnt);
      ds_ucsd_nt_set_DSM_Level_Low(TRUE);
   }
   
   /* Allocate new DSM item */
   PDU_ptr = dsm_offset_new_buffer( DSM_DS_SMALL_ITEM_POOL,
                                    (uint16)(get_current_rlp_header_size() +
                                             L2R_MAX_HDR_SIZE));
   if ( NULL == PDU_ptr )
   {
     DS_3GPP_MSG0_ERROR ("No small DSM items available");
   }
   else
   {
     /* Clear out the app_field */
     PDU_ptr->app_field = 0;
  
     /* MSG_LOW ("Allocating Uplink DSM item 0x%x - available: %d",
                  PDU_ptr, DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),0);
     */
   }
   return PDU_ptr;
}


/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_Free
 * -------------
 * Description:    Frees the passed DSM item pointer.
 *                 Pointer is set to null.
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
void ds_ucsd_UplinkPDUMgr_Free 
(
   dsm_item_type ** PDU_ptr
)
{
   int32 free_cnt = DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL); /*lint !e10 !e26*/

   /* MSG_LOW ("In ds_ucsd_UplinkPDUMgr_Free - 0x%x",*PDU_ptr,0,0); */

   /* Check availability of DSM items.                       */
   /* If Tx flow control enabled and MANY threshold exceded, */
   /* disable Tx flow control.                               */
   if ( ( ds_ucsd_nt_get_DSM_Level_Low() ) &&
        ( RLP_DSM_MEM_LEVEL_MANY < free_cnt ))
   {
      DATA_MSG2_HIGH("Disabling TX Flow Control: %d < %d",
                RLP_DSM_MEM_LEVEL_MANY, free_cnt);
      ds_ucsd_nt_set_DSM_Level_Low(FALSE);
   }
   
   /* Release the DSM item chain */
   /* This sets PDU ptr to NULL */
   dsm_free_packet( PDU_ptr );
}

/****************************************************************************
 * Function name:  ds_ucsd_UplinkPDUMgr_Release
 * -------------
 * Description:    Releases the PDU at the specified position
 *                 in the indicated PDU list.
 * -----------     
 * Returns:        Pointer to released DSM item (caller must free it).
 * -------         
 ****************************************************************************/
dsm_item_type * ds_ucsd_UplinkPDUMgr_Release
(
   Tx_PDU_list_T* List,
   short Index
)
{
   dsm_item_type * result = NULL;
   
   /* Verify List provided */
   if (NULL != List)
   {
     dsm_item_type ** PDUList = *PDU_LIST_PTR(List);
           
     /* Release the PDU buffer */
     result = PDUList[Index];
     PDUList[Index] = NULL;

     /*MSG_LOW ("Releasing PDU at Uplink List: %d[%d] 0x%x",
                List->Type,Index,result); */
   }

   return result;
}

/****************************************************************************
 * Function name:  InitialiseList
 * -------------
 * Description:    This routine initialises a PDU list.
 *                 Note dynamic memory is allocated/freed.
 * -----------     
 * Dependencies:   ds_ucsd_UplinkPDUMgr_PwrUp_Init must have been called
 *                 once before calling this function.
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
static void InitialiseList
(
   Tx_PDU_list_T* List,
   short          Index,
   boolean        Allocate
)
{
   /* Initialize PDU list */
   if (List != NULL)
   {
      PDU_List_T * PDU_list_ptr = PDU_LIST_PTR(List);
      
      DATA_MSG1_LOW("Initializing Uplink List: %d",List->Type);
      
      List->VA = Index;
      List->VS = Index;
      List->VD = Index;

      
      /* First deallocate any previous dynamic memory (if any) */
      /* State tracking array */
      if (NULL != List->State)
      {
         List->State = NULL;
      }      
      
      /* DSM item storage array */
      if (NULL != *PDU_list_ptr)
      {
         /* Free any DSM items in previous list */
         word i;
         for (i=0; i<List->PDUCount; i++)
         {
            dsm_item_type * dsm_ptr = PDU_PTR(List,i);

            /* Check for registered DSM item */
            if ( NULL != dsm_ptr )
            {
               dsm_free_packet( &dsm_ptr );
               /* MSG_LOW ("Released PDU at List: %d[%d]",List->Type,Index,0); */
            }
         }
         *PDU_list_ptr = NULL;
      }

      
      /* Allocate dynamic memory items */
      if (Allocate)
      {

         List->State = (PDU_TYPE_SMALL == List->Type) ?
                       UL_small_state_list : UL_large_state_list;
         
         *PDU_list_ptr = (PDU_TYPE_SMALL == List->Type) ?
                         UL_small_PDU_data_list : UL_large_PDU_data_list;

         List->PDUCount = RLP_M_VER_2;
         memset((void*)List->State, PDU_STATE_IDLE, RLP_M_VER_2);
         memset( (void*)*PDU_list_ptr, 0x0, (RLP_M_VER_2 * sizeof(dsm_item_type*)) );
      }
   }
}




/* Test !*/
/*********
void print_pdu (byte* buf, short len)
{
   short i;

   printf ("PDU: ");
   for (i=0; i<len; i++)
   {
      printf ("%d ", buf[i]);
   }
   printf ("\n");
}

boolean L2R_send (byte* pdu)
{
   static byte remap_pending_buffer[100];
   static boolean pending_buffer_empty =TRUE;
   byte Type;
   boolean RetVal = FALSE;



   if (ds_ucsd_UplinkPDUMgr_RemapDataPending ())
   {
      / * Data in an old pdu list - retrieve it! **
      ds_ucsd_UplinkPDUMgr_Retrieve (remap_pending_buffer);
      ds_ucsd_UplinkPDUMgr_Write (remap_pending_buffer);
   }
   else
   {
      ds_ucsd_UplinkPDUMgr_Write (pdu);
      RetVal = TRUE;
   }
   return RetVal; 
}

void main (void)
{
   byte* pByte;
   byte buf1[25] = {1,2,3,4,5};
   byte buf1a[25] = {6,7,8,9,10};
   byte buf1b[25] = {11,12,13,14,15};
   byte buf1c[25] = {16,17,18,19,20};
   byte buf1d[25] = {21,22,23,24,25};

   byte buf2[100] = {0,0,0,0,0,0,0,0,0,0,0};
   
   
   ds_ucsd_UplinkPDUMgr_ColdStart ();


   / * Now a dummy L2R_transmit: **


   while (!L2R_send(buf1));
   while (!L2R_send(buf1a));
         
   ds_ucsd_UplinkPDUMgr_Remap (PDU_TYPE_LARGE, CurrentList->VA);

   while (!L2R_send(buf1b));
   while (!L2R_send(buf1c));

   ds_ucsd_UplinkPDUMgr_Remap (PDU_TYPE_SMALL, CurrentList->VA);

   while (!L2R_send(buf1d));



   ds_ucsd_UplinkPDUMgr_Read (buf2);
   print_pdu (buf2, PDU_SMALL_LENGTH);

   ds_ucsd_UplinkPDUMgr_Read (buf2);
   print_pdu (buf2, PDU_SMALL_LENGTH);

   ds_ucsd_UplinkPDUMgr_Read (buf2);
   print_pdu (buf2, PDU_SMALL_LENGTH);

   ds_ucsd_UplinkPDUMgr_Read (buf2);
   print_pdu (buf2, PDU_SMALL_LENGTH);

   ds_ucsd_UplinkPDUMgr_Read (buf2);
   print_pdu (buf2, PDU_SMALL_LENGTH);



}

*******/
#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */
