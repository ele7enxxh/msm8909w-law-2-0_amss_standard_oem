/*===========================================================================

              GSM Non-Transparent Data Downlink PDU Manager
              

DESCRIPTION
  This is the L2R/RLP Downlink PDU manager routines.

Copyright (c) 2001 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsddlpdu.c_v   1.2   12 Feb 2002 16:49:32   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsddlpdu.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/11   ND      Feature cleanup.
04/05/11   SS      Q6 free floating support.
09/29/09   SA      Fixed KW errors.
02/09/06   AR      Lint corrections.
07/23/04   AR      Lint corrections
06/07/04   AR      Toggle use for SREJ based on NV item.
03/10/03   AR      Remove spurious flow control messages.
12/11/03   TMR     Added checks for 'while' loops to make sure we don't get 
                   into any infinite loops
09/18/03   AR      Removed obsolete code block.
07/15/03   TMR     Added support for RLP Version 2
04/16/03   AR      Add suppport for DSM items inplace of local buffers
                   Lint corrections
04/16/03   AR      Fixed typo in call from ds_ucsd_DownlinkPDUMgr_ColdStart
01/27/03   TMR     Turned debugging on, and changed some debugs
08/10/01   TMR     Initial release

===========================================================================*/


 
/* <EJECT> */
/*===========================================================================
Header files
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_3gppi_utils.h"

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)


/* Turn debugging off in this file */
//#define DS_GCSD_FILE_DEBUG_OFF

#include "dsucsdnti.h"

#include "dsucsdrlp.h"
#include "memory.h"
#include "amssassert.h"


/* <EJECT> */
/*===========================================================================
Data definitions
===========================================================================*/


/****************************************************************************
 * Module Scope variables 
 ****************************************************************************/
static PDU_List_T large_PDU_data_list = {NULL};
static PDU_List_T small_PDU_data_list = {NULL};


static Rx_PDU_list_T LargePDUList = {PDU_TYPE_INVALID, 0, 0, 0, NULL};
static Rx_PDU_list_T SmallPDUList = {PDU_TYPE_INVALID, 0, 0, 0, NULL};

static Rx_PDU_list_T* CurrentList = &LargePDUList;
static Rx_PDU_list_T* OldList = NULL;

/* Statically allocate maximum storage required (WCDMA) for PDU Manager */
static dsm_item_type * DL_small_PDU_data_list[RLP_M_VER_2] = {NULL};
static dsm_item_type * DL_large_PDU_data_list[RLP_M_VER_2] = {NULL};

static byte DL_small_state_list[RLP_M_VER_2] = {0};
static byte DL_large_state_list[RLP_M_VER_2] = {0};


/****************************************************************************
 * Module function prototypes
 ****************************************************************************/
LOCAL void ds_ucsd_MarkMissingFramesForSREJ (short Ns);
LOCAL void ds_ucsd_InitialiseList
(
   Rx_PDU_list_T* List,
   short Index,
   boolean Allocate
);



/* <EJECT> */
/*===========================================================================
Function definitions
===========================================================================*/


/*===========================================================================

FUNCTION  ds_ucsd_DownlinkPDUMgr_DataAvailable

DESCRIPTION
   Return whether or not data available to read.
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if data is available, FALSE if not
    
SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_ucsd_DownlinkPDUMgr_DataAvailable (void)
{
   boolean RetVal = FALSE;

   if (OldList)
   {
      RetVal = TRUE;
   }
   else
   {
      if ((CurrentList) && (CurrentList->VR != CurrentList->VE) && 
          (CurrentList->State[CurrentList->VE] == PDU_STATE_RCVD))
      {
         RetVal = TRUE;
      }
   }
   
   //DS_UCSD_DEBUG_MSG_1 ("ds_ucsd_DownlinkPDUMgr_DataAvailable rc=",RetVal);

   return RetVal;
}

 
/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_DownlinkPDUMgr_IsEmpty

DESCRIPTION
     Return whether or not all lists empty
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if all lists are empty, FALSE if not
    
SIDE EFFECTS
  None

===========================================================================*/
boolean   
ds_ucsd_DownlinkPDUMgr_IsEmpty (void)
{
   boolean RetVal = FALSE;

   if (!OldList)
   {
      if ((CurrentList->VR == CurrentList->VE) && 
          (CurrentList->State[CurrentList->VE] == PDU_STATE_IDLE))
      {
         RetVal = TRUE;
      }
   }
   return RetVal;
}



/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_DownlinkPDUMgr_Read

DESCRIPTION
    Read a PDU from the current Downlink list.
         
DEPENDENCIES
  None
  
RETURN VALUE
  RLP_ERROR, or Type of PDU read
    
SIDE EFFECTS
  None

===========================================================================*/
PDU_Type 
ds_ucsd_DownlinkPDUMgr_Read 
(
   dsm_item_type ** PDU_ptr
)
{
   PDU_Type RetVal = PDU_TYPE_INVALID;
   
   ASSERT( NULL != PDU_ptr );
   *PDU_ptr = NULL;

   /* IF there is any old data left from before a REMAP, then send it */
   if (OldList)
   {
      if (OldList->State[OldList->VE] == PDU_STATE_RCVD)
      {
         DS_UCSD_DEBUG_MSG_1 ("ds_ucsd_DownlinkPDUMgr_Read from OldList:%d",
                              OldList->VE);

         /* Get PDU from old PDU list */
         *PDU_ptr = ds_ucsd_DownlinkPDUMgr_Release (OldList, OldList->VE);
         OldList->State[OldList->VE] = PDU_STATE_IDLE;

         OldList->VE = INC_SEQ_NUM (OldList->VE);
         RetVal = OldList->Type;
      }

      /* IF all the old data read then delete the list */
      if (OldList->VR == OldList->VE)
      {
         DS_UCSD_DEBUG_MSG ("ds_ucsd_DownlinkPDUMgr_Read: Setting OldList=NULL");
         OldList = NULL;
      }
   }
   else
   {
      if (CurrentList->State[CurrentList->VE] == PDU_STATE_RCVD)
      {
         
         //DS_UCSD_DEBUG_MSG_1 ("DownlinkPDUMgr_Read VE=%d",CurrentList->VE);
   
         /* Get PDU from current PDU list */
         *PDU_ptr = ds_ucsd_DownlinkPDUMgr_Release (CurrentList, CurrentList->VE);
         CurrentList->State[CurrentList->VE] = PDU_STATE_IDLE;
         
         CurrentList->VE = INC_SEQ_NUM (CurrentList->VE);
         RetVal = CurrentList->Type;
      }
      else
      {
         DS_UCSD_DEBUG_MSG_2 ("DLPDUMgr_Read error: VE=%d, State[VE]=%d",
                                                         CurrentList->VE,
                                                         CurrentList->State[CurrentList->VE]);
      }
   }
   
   //DS_UCSD_DEBUG_MSG_3 ("ds_ucsd_DownlinkPDUMgr_Read returned:%d CurrVR=%d CurrVE=%d ",
   //                     RetVal, CurrentList->VR, CurrentList->VE);

   if (OldList)
   {
      DS_UCSD_DEBUG_MSG_2 ("OldVR=%d OldVE=%d", OldList->VR, OldList->VE);
   }

   return RetVal;
}



/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_DownlinkPDUMgr_Write

DESCRIPTION
     Writes a PDU into the current downlink PDU list at the in-sequence
     position. The PDU state is set to RCVD. The input PDU pointer
     parameter is set to NULL. The VR sequence index is incremented.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_DownlinkPDUMgr_Write 
(
   dsm_item_type ** PDU_ptr
)
{
   dsm_item_type ** PDUList = *PDU_LIST_PTR(CurrentList);

   ASSERT( NULL != PDU_ptr );
   ASSERT( NULL != *PDU_ptr );
   
   //MSG_LOW ("Writing PDU at Downlink List: %d[%d] 0x%x",
        //    CurrentList->Type, CurrentList->VR, *PDU_ptr);
   
   /* Insert PDU into the current list to send */
   PDUList[CurrentList->VR] = *PDU_ptr;
   *PDU_ptr = NULL;
   CurrentList->State[CurrentList->VR] = PDU_STATE_RCVD;

   /* Increment the state pointer */
   if ( SREJisSuitable() )
   {
     while (CurrentList->State[CurrentList->VR] == PDU_STATE_RCVD)
     {

       CurrentList->VR = INC_SEQ_NUM (CurrentList->VR);
     }
   }
   else
   {
     CurrentList->VR = INC_SEQ_NUM (CurrentList->VR);
   }
}

/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_DownlinkPDUMgr_WriteOutOfSeq

DESCRIPTION
     Writes a PDU out of the normal sequence. The PDU state is set to RCVD.
     This is used when the RLP is configured to use SREJ recovery.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_DownlinkPDUMgr_WriteOutOfSeq 
(
   dsm_item_type ** PDU_ptr,
   short _Ns
)
{
   dsm_item_type ** PDUList = *PDU_LIST_PTR(CurrentList);

   ASSERT( NULL != PDU_ptr );
   ASSERT( NULL != *PDU_ptr );
   
   MSG_LOW ("Writing PDU OoS at Downlink List: %d[%d] 0x%x",
            CurrentList->Type, _Ns, *PDU_ptr);
   
   /* Check for overwrite condition */
   if (NULL != PDUList[_Ns])
   {
      //MSG_MED ("Overwriting PDU in Downlink List: %d[%d] 0x%x",
           //     CurrentList->Type, _Ns, PDUList[_Ns]);
      
      ds_ucsd_DownlinkPDUMgr_Free(&PDUList[_Ns]);
   }
   
   /* Insert PDU into the current list to send */
   PDUList[_Ns] = *PDU_ptr;
   *PDU_ptr = NULL; 
   CurrentList->State[_Ns] = PDU_STATE_RCVD;

   ds_ucsd_MarkMissingFramesForSREJ (_Ns);
}



/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_DownlinkPDUMgr_Remap

DESCRIPTION
     Remap to the requested PDU size
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
short 
ds_ucsd_DownlinkPDUMgr_Remap 
(
   PDU_Type Type
)
{
   short RetVal = RLP_OK;

   /* IF the requested PDU size is different to that currently being used */
   if (Type != CurrentList->Type)
   {
      if (OldList)
      {
         /* IF the old list is not empty then we have not finished
            with the last Remap! The RLP cannot REMAP without losing
            some data - it will issue a link reset */
         RetVal = RLP_ERROR;

         DS_UCSD_DEBUG_MSG ("ds_ucsd_DownlinkPDUMgr_Remap : returning ERROR!");
      }
      else
      {
         /* Save the Current List */
         OldList = CurrentList;
         
         /* Cancel all SREJ conditions - is this necessary!?!?!?*/
         /* TBD!!!*/

         /* Point to the new list */
         if (Type == PDU_TYPE_SMALL)
         {
            CurrentList = &SmallPDUList;
            DS_UCSD_DEBUG_MSG ("ds_ucsd_DownlinkPDUMgr_Remap: Going to SMALL PDUs");
         }
         else
         {
            CurrentList = &LargePDUList;
            DS_UCSD_DEBUG_MSG ("ds_ucsd_DownlinkPDUMgr_Remap: Going to LARGE PDUs");
         }

         /* Start the new list from the current VR */
         ds_ucsd_InitialiseList (CurrentList, OldList->VR, TRUE);

         DS_UCSD_DEBUG_MSG_1 ("ds_ucsd_DownlinkPDUMgr_Remap: Starting new list from: %d", OldList->VR);

         /* If all the old data read then delete the list */
         if (OldList->VR == OldList->VE)
         {
            DS_UCSD_DEBUG_MSG ("ds_ucsd_DownlinkPDUMgr_Read: Setting OldList=NULL");
            OldList = NULL;
         }
      }
   }
   return RetVal;
}



/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_DownlinkPDUMgr_DeleteUnAcknowledgedFrames

DESCRIPTION
   This routine deletes any frames that have been received 
   out of sequence. It is called in the case of us being
   polled by the remote node (but only if we are configured
   to use SREJ recovery), for a checkpoint recovery.
   This will only happen if we have tried SREJ recovery
   and it too has failed, so as a last resort the remote
   node is performing checkpoint recovery with a Poll.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_DownlinkPDUMgr_DeleteUnAcknowledgedFrames (void)
{
   short Index = CurrentList->VR;
   dsm_item_type * PDU;

   /* Make sure loop values are sane */
   if ( (CurrentList->VE > get_current_rlp_max_frame_number()) ||
        (CurrentList->VR > get_current_rlp_max_frame_number()) )
   {
      DATA_MSG2_ERROR("DLPDUMgr: DelUnACKFrames, illegal VE=%d or VR=%d",
                          CurrentList->VE,CurrentList->VR);

      WRITE_EVENT_TO_L2R (RLP_ERROR_IND);
      return;
   }

   do 
   {
      if (CurrentList->State[Index] == PDU_STATE_RCVD)
      {
         DS_UCSD_DEBUG_MSG_1 ("RLP:Del o.o.s IFrame:Rx[%d]=IDLE", Index);

         /* Release the PDU */
         PDU = ds_ucsd_DownlinkPDUMgr_Release (CurrentList,Index) ;
         CurrentList->State[Index] = PDU_STATE_IDLE;
         
         /* Free DSM item */
         ds_ucsd_DownlinkPDUMgr_Free (&PDU);
      }
      Index = INC_SEQ_NUM (Index);
   } while (Index != CurrentList->VE);
}


/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_DownlinkPDUMgr_ReceiveListFull

DESCRIPTION
   Check the receive list to see if it is full
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if list is full, FALSE if not.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_DownlinkPDUMgr_ReceiveListFull (void)
{
   register boolean ret_val;
   register int32 size;
   register int32 threshold;
   
   size = CurrentList->VR - CurrentList->VE;
   if (size < 0)
   {
      size += get_current_rlp_max_window_size();
   }
    
   if (current_rlp_version_is_2())
   {
      threshold = RECEIVE_FULL_THRESHOLD_V2;
   }
   else
   { 
      threshold = RECEIVE_FULL_THRESHOLD;
   }
     
   if (size >= threshold)
   {
      ret_val = TRUE;
   }
   else
   {
      ret_val = FALSE;
   }
   
   return ret_val;
}



/* <EJECT> */
/*===========================================================================

FUNCTIONS
   The following are a set of small utility functions for the downlink
   PDU manager 

DESCRIPTION
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
uint16
ds_ucsd_DownlinkPDUMgr_VE (void)
{
   return (CurrentList->VE);
}


uint16
ds_ucsd_DownlinkPDUMgr_VR (void)
{
   return (CurrentList->VR);
}


void 
ds_ucsd_DownlinkPDUMgr_IncVR (void)
{
   CurrentList->VR = INC_SEQ_NUM (CurrentList->VR);
}


byte
ds_ucsd_DownlinkPDUMgr_GetState (short Index)
{
   return (CurrentList->State[Index]);
}


void 
ds_ucsd_DownlinkPDUMgr_SetState (short Index, byte State)
{
   CurrentList->State[Index] = State;
}


void 
ds_ucsd_DownlinkPDUMgr_Initialise (PDU_Type Type, boolean Allocate)
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

   ds_ucsd_InitialiseList (CurrentList, 0, Allocate);
   ds_ucsd_InitialiseList (OldList, 0, Allocate);

}


void 
ds_ucsd_DownlinkPDUMgr_Empty (void)
{
   OldList = NULL;
   ds_ucsd_InitialiseList (CurrentList, CurrentList->VR, TRUE);
}




/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_DownlinkPDUMgr_ColdStart

DESCRIPTION
    PDU manager initialization function that needs to be called once
    at system startup time
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_DownlinkPDUMgr_ColdStart 
(
   PDU_Type Type
)
{
   /* These fields in the PDU list structures are FIXED - they
      need to be initialised at start-up time. */
   SmallPDUList.Type       = PDU_TYPE_SMALL;                             
   LargePDUList.Type       = PDU_TYPE_LARGE;

   ds_ucsd_DownlinkPDUMgr_Initialise (Type, TRUE);
}


/* <EJECT> */
/****************************************************************************
 * Function name:  ds_ucsd_DownlinkPDUMgr_PwrUp_Init
 * -------------
 * Description:    This routine initialises the PDU Mgr at powerup.
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
void
ds_ucsd_DownlinkPDUMgr_PwrUp_Init ( void )
{
   /* Required for dynamic memory allocation */
   /* Clear memory used by state tracking */
   memset ((void*)&SmallPDUList, 0x00, sizeof(Rx_PDU_list_T));
   memset ((void*)&LargePDUList, 0x00, sizeof(Rx_PDU_list_T));

   /* Initialize PDU list pointers */
   small_PDU_data_list = NULL;
   large_PDU_data_list = NULL;
}

/* <EJECT> */
/****************************************************************************
 * Function name:  ds_ucsd_DownlinkPDUMgr_Cleanup
 * -------------
 * Description:    This routine cleans up list on call end.
 * -----------     
 * Returns:        None
 * -------         
 ****************************************************************************/
void
ds_ucsd_DownlinkPDUMgr_Cleanup ( void )
{
   /* Cleanup both PDU lists */
   ds_ucsd_DownlinkPDUMgr_Initialise (PDU_TYPE_SMALL, FALSE);
   ds_ucsd_DownlinkPDUMgr_Initialise (PDU_TYPE_LARGE, FALSE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_DownlinkPDUMgr_ReceiveWindowOpen

DESCRIPTION
   This routine returns TRUE if the receive window
   is open (ie that means that the number of outstanding
   non-ack'd frames is less than the max window size).
         
DEPENDENCIES
  None
  
RETURN VALUE
   TRUE = Window Open/FALSE = Window Closed
    
SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_DownlinkPDUMgr_ReceiveWindowOpen (short _Ns)
{
   boolean RetVal = FALSE;
   short Size;
   XID_Params_T *current_xid_ptr = NULL;

   Size = _Ns-CurrentList->VR;
   if (Size < 0)
   {
      Size += get_current_rlp_max_window_size();
   }
   
   current_xid_ptr = ds_ucsd_get_current_xid(); 

   if (Size < current_xid_ptr->IWFtoMSWindow) /*was <=, cetecom!!*/
   {
      RetVal = TRUE;
   }
   else
   {
      DS_UCSD_DEBUG_MSG_2 ("RLP:Rx Wnd Closed!(Ns=%d, VR=%d)",
                           _Ns, CurrentList->VR);
   }

   return RetVal;
}


/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_MarkMissingFramesForSREJ

DESCRIPTION
   This routine marks any missing frames in the Rx list.
   These frames will be requested using a SREJ frame.
   (From VR to Ns-1 marked as SREJ)
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL 
void 
ds_ucsd_MarkMissingFramesForSREJ 
(
   short _Ns
)
{
   short Index = CurrentList->VR;

   /* Make sure loop values are sane */
   if ( (_Ns < 0) || 
        (_Ns > get_current_rlp_max_frame_number()) ||
        (CurrentList->VR > get_current_rlp_max_frame_number()) )
   {
      DATA_MSG2_ERROR("DLPDUMgr: MarkMissFrmForSREJ, illegal N(r)=%d or VR=%d",
                        _Ns,CurrentList->VR);

      WRITE_EVENT_TO_L2R (RLP_ERROR_IND);
      return;
   }
   while (Index != _Ns)
   {
      if (CurrentList->State[Index] == PDU_STATE_IDLE)
      {

         DS_UCSD_DEBUG_MSG_1 ("RLP: rxl[%d]=SREJ", Index);
         CurrentList->State[Index] = PDU_STATE_SREJ;
      }
      Index = INC_SEQ_NUM (Index);
   }
}


/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_DownlinkPDUMgr_CheckSREJToSend

DESCRIPTION
    This routine checks to see if an SREJ frame needs to be sent.
         
DEPENDENCIES
  None
  
RETURN VALUE
  RLP_ERROR - None to send, or Index (Ns) of SREJ to send
    
SIDE EFFECTS
  None

===========================================================================*/
short 
ds_ucsd_DownlinkPDUMgr_CheckSREJToSend (void)
{
   short Index = CurrentList->VR;

  DATA_MSG2_LOW("In ds_ucsd_DownlinkPDUMgr_CheckSREJToSend: VR=%d VE=%d",
            CurrentList->VR,CurrentList->VE);

   /* Make sure loop values are sane */
   if ( (CurrentList->VE > get_current_rlp_max_frame_number()) ||
        (CurrentList->VR > get_current_rlp_max_frame_number()) )
   {
      DATA_MSG2_ERROR("DLPDUMgr: CheckSREJToSend, illegal VE=%d or VR=%d",
                     CurrentList->VE,CurrentList->VR);
      WRITE_EVENT_TO_L2R (RLP_ERROR_IND);
   }
   else
   {
      do
      {
         if (CurrentList->State[Index] == PDU_STATE_SREJ)
         {
            return Index;
         }
         Index = INC_SEQ_NUM (Index);
      
      } while (Index != CurrentList->VE);
   }
   return RLP_ERROR;
}


/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_DownlinkPDUMgr_New

DESCRIPTION
    This routine allocates a new DSM item from small pool to be used as
    a PDU buffer.
         
DEPENDENCIES
  None
  
RETURN VALUE
  DSM item pointer
    
SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type * ds_ucsd_DownlinkPDUMgr_New ( void )
{
   dsm_item_type * PDU_ptr = NULL;
   int32 free_cnt = DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL); /*lint !e10 !e26*/

   /* Check availability of DSM items.                   */
   /* If FEW threshold exceeded, enable Tx flow control. */
   if ( ( !ds_ucsd_nt_get_DSM_Level_Low() ) &&
        ( RLP_DSM_MEM_LEVEL_FEW > free_cnt ) )
   {
      DS_UCSD_DEBUG_MSG_2 ("Enabling DSM level TX Flow Control: %d > %d",
                           RLP_DSM_MEM_LEVEL_FEW, free_cnt);
      ds_ucsd_nt_set_DSM_Level_Low(TRUE);
   }
   
   /* Allocate new DSM item */
   if ( NULL == (PDU_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)) )
   {
      DS_3GPP_MSG3_ERROR ("No small DSM items available",0,0,0);
   }
   else
   {

     /* MSG_LOW ("Allocating Downlink DSM item 0x%x - available: %d",
                 PDU_ptr,DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),0); */
        
     /* Clear out the app_field */
     PDU_ptr->app_field = 0;
   }
   return PDU_ptr;
}

/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_DownlinkPDUMgr_Free

DESCRIPTION
  This routine frees the passed DSM item pointer. Pointer is set to null.
         
DEPENDENCIES
  None
  
RETURN VALUE
  DSM item pointer
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_DownlinkPDUMgr_Free 
(
   dsm_item_type ** PDU_ptr
)
{
   int32 free_cnt = DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL); /*lint !e10 !e26*/

   /* MSG_LOW ("In ds_ucsd_DownlinkPDUMgr_Free - 0x%x",*PDU_ptr,0,0); */

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
   /* This sets PDU pptr to NULL */
   dsm_free_packet( PDU_ptr );
}

/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_DownlinkPDUMgr_Release

DESCRIPTION
    This routine releases the PDU found at the position
    in the indicated PDU list. 
         
DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to released DSM item (caller must free it).
    
SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *  ds_ucsd_DownlinkPDUMgr_Release 
(
   Rx_PDU_list_T* List,
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

     //MSG_LOW ("Releasing PDU at Downlink List: %d[%d] 0x%x",
          //    List->Type,Index,result);
   }

   return result;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_InitialiseList

DESCRIPTION
  This routine initialises a PDU list.
  Note dynamic memory is allocated/freed.
         
DEPENDENCIES
  ds_ucsd_UplinkPDUMgr_PwrUp_Init must have been called once before
  calling this function.
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_InitialiseList 
(
   Rx_PDU_list_T  * List, 
   short          Index,
   boolean        Allocate
)
{
   /* Initialize PDU list */
   if (List != NULL)
   {
      PDU_List_T * PDU_list_ptr = PDU_LIST_PTR(List);
      
      DATA_MSG1_LOW("Initializing Downlink List: %d",List->Type);
      
      List->VE = Index;
      List->VR = Index;

      /* First deallocate any previous dynamic memory (if any) */
      /* State tracking array */
      if (NULL != List->State)
      {         
         List->State = NULL;
      }
      
      /* DSM item storage array */
      if (NULL != *PDU_list_ptr)
      {
         word i;
         for (i=0; i<List->PDUCount; i++)
         {
            dsm_item_type * dsm_ptr = PDU_PTR(List,i);

            /* Check for registered DSM item */
            if ( NULL != dsm_ptr )
            {
               dsm_free_packet( &dsm_ptr );
            }
         }
         
         *PDU_list_ptr = NULL;
      }

      /* Allocate dynamic memory items */
      if (Allocate)
      {

         List->State = (PDU_TYPE_SMALL == List->Type) ?
                       DL_small_state_list : DL_large_state_list;
         
         *PDU_list_ptr = (PDU_TYPE_SMALL == List->Type) ?
                         DL_small_PDU_data_list : DL_large_PDU_data_list;
         
         List->PDUCount = RLP_M_VER_2;
         memset((void*)List->State, PDU_STATE_IDLE, RLP_M_VER_2);
         memset( (void*)*PDU_list_ptr, 0x0,
                 (RLP_M_VER_2 * sizeof(dsm_item_type*)) );
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

void main (void)
{
   byte* pByte;
   byte buf1[25] = {1,2,3,4,5};
   byte buf1a[25] = {6,7,8,9,10};
   byte buf1b[25] = {11,12,13,14,15};
   byte buf1c[25] = {16,17,18,19,20};

   byte buf2[100] = {0,0,0,0,0,0,0,0,0,0,0};
   
   ds_ucsd_DownlinkPDUMgr_ColdStart ();

   print_pdu (buf1, PDU_SMALL_LENGTH);
   print_pdu (buf2, PDU_SMALL_LENGTH);
   

   ds_ucsd_DownlinkPDUMgr_Write (buf1);
   ds_ucsd_DownlinkPDUMgr_Write (buf1a);
   ds_ucsd_DownlinkPDUMgr_Remap (PDU_TYPE_LARGE);
   ds_ucsd_DownlinkPDUMgr_Write (buf1b);
   ds_ucsd_DownlinkPDUMgr_Write (buf1c);


   

   if (ds_ucsd_DownlinkPDUMgr_DataAvailable ())
   {
      ds_ucsd_DownlinkPDUMgr_Read (buf2);
      print_pdu (buf2, PDU_SMALL_LENGTH);
   }
   
   if (ds_ucsd_DownlinkPDUMgr_DataAvailable ())
   {
      ds_ucsd_DownlinkPDUMgr_Read (buf2);
      print_pdu (buf2, PDU_SMALL_LENGTH);
   }
   
   if (ds_ucsd_DownlinkPDUMgr_DataAvailable ())
   {
      ds_ucsd_DownlinkPDUMgr_Read (buf2);
      print_pdu (buf2, PDU_SMALL_LENGTH);
   }
   
   if (ds_ucsd_DownlinkPDUMgr_DataAvailable ())
   {
      ds_ucsd_DownlinkPDUMgr_Read (buf2);
      print_pdu (buf2, PDU_SMALL_LENGTH);
   }
   
   if (ds_ucsd_DownlinkPDUMgr_DataAvailable ())
   {
      ds_ucsd_DownlinkPDUMgr_Read (buf2);
      print_pdu (buf2, PDU_SMALL_LENGTH);
   }
   

   ds_ucsd_DownlinkPDUMgr_Write (buf1);
   ds_ucsd_DownlinkPDUMgr_Write (buf1a);
   ds_ucsd_DownlinkPDUMgr_Remap (PDU_TYPE_SMALL);
   ds_ucsd_DownlinkPDUMgr_Write (buf1b);
   ds_ucsd_DownlinkPDUMgr_Write (buf1c);

   if (ds_ucsd_DownlinkPDUMgr_DataAvailable ())
   {
      ds_ucsd_DownlinkPDUMgr_Read (buf2);
      print_pdu (buf2, PDU_SMALL_LENGTH);
   }
   
   if (ds_ucsd_DownlinkPDUMgr_DataAvailable ())
   {
      ds_ucsd_DownlinkPDUMgr_Read (buf2);
      print_pdu (buf2, PDU_SMALL_LENGTH);
   }
   
   if (ds_ucsd_DownlinkPDUMgr_DataAvailable ())
   {
      ds_ucsd_DownlinkPDUMgr_Read (buf2);
      print_pdu (buf2, PDU_SMALL_LENGTH);
   }
   
   if (ds_ucsd_DownlinkPDUMgr_DataAvailable ())
   {
      ds_ucsd_DownlinkPDUMgr_Read (buf2);
      print_pdu (buf2, PDU_SMALL_LENGTH);
   }
   
   if (ds_ucsd_DownlinkPDUMgr_DataAvailable ())
   {
      ds_ucsd_DownlinkPDUMgr_Read (buf2);
      print_pdu (buf2, PDU_SMALL_LENGTH);
   }
}
*******/
#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */
