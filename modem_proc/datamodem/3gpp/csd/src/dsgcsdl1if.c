/*===========================================================================

              G C S D   P R O T O C O L   S T A C K    
                      S O U R C E   F I L E

DESCRIPTION
  This source file contains the GCSD transport protocol stack high level
  functions, task entry point, and command interface functions.  
  
  This software unit contains the task entry point and the top-level dispatch
  routines for the Data Services (DS) Task. Functions to manipulate the Data
  Services Task's command queue are also included here.

    Copyright (c) 1991-2014 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdl1if.c_v   1.9   25 Jun 2002 13:11:24   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdl1if.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/12   ND      RCINIT changes.
04/01/11   TTV     Support for Q6 free floating.
11/01/10   SA      Removing REX_DISABLE_PREMPTION/REX_ENABLE_PREMPTION.
04/02/09   SA      Removed multiple initialization of critical section for L1 interface.
03/04/09   SA      AU level CMI modifications.
11/07/08   DH      Porting code for Q6, off-target build.
07/29/08   SA      Limited the usage of KxMutex to only 8k and Windows mobile.
06/06/08   SA      Migrated INTLOCK to KxMutex for L1 Rx and Tx buffers.
05/30/04   AR      Lint corrections.
05/16/02   TMR     Fixed problem with M1 and M2 bit logging for 14400 frames
08/10/01   TMR     Initial release

===========================================================================*/




/*===========================================================================
Data definitions
===========================================================================*/


/*===========================================================================
Function definitions
===========================================================================*/ 

/*===========================================================================
Header files
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#undef SIMBA_BUILD

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)

#include "memory.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "cm.h"
#include "mn_cm_exp.h"
#include "rcinit_rex.h"
#include "data_msg.h"
/* Turn debugging off in this file */
#define DS_GCSD_FILE_DEBUG_OFF
#include "dsgcsdi.h"

#include "dsgcsd_task_v.h"
#include "dstask.h"
#include "sys_cnst.h"
#include "sys_stru.h"
#include "sys_type.h"
#include "l1_ds.h"

 /* Use rex INTLOCK replacements for QDSP and all other targets
 */
static rex_crit_sect_type ds_gcsd_l1_rx_buf_crit_sect = {{NULL}};
static rex_crit_sect_type ds_gcsd_l1_tx_buf_crit_sect = {{NULL}};

#define DS_GCSD_L1_RX_BUFF_LOCK() \
  REX_ISR_LOCK( &ds_gcsd_l1_rx_buf_crit_sect ); 

#define DS_GCSD_L1_RX_BUFF_UNLOCK() \
  REX_ISR_UNLOCK( &ds_gcsd_l1_rx_buf_crit_sect );

#define DS_GCSD_L1_TX_BUFF_LOCK() \
  REX_ISR_LOCK( &ds_gcsd_l1_tx_buf_crit_sect );

#define DS_GCSD_L1_TX_BUFF_UNLOCK() \
  REX_ISR_UNLOCK( &ds_gcsd_l1_tx_buf_crit_sect );
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

#if defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)

#define GCSD_L1_BUF_SIZE      38
#define GCSD_L1_NUM_TX_BUFS   2
#define GCSD_L1_NUM_RX_BUFS   3

typedef struct 
{
   uint32 read_index;
   uint32 write_index;
   uint32 count;
   uint32 size;
} gcsd_l1_rx_buf_mgr_type;

static gcsd_l1_rx_buf_mgr_type Gcsd_l1_rx_buf_mgr = {0,0,0,GCSD_L1_NUM_RX_BUFS};
static gcsd_l1_rx_buf_mgr_type Gcsd_l1_tx_buf_mgr = {0,0,0,GCSD_L1_NUM_TX_BUFS};
/*---------------------------------------------------------------------------
  TCB pointer for GCSD task
---------------------------------------------------------------------------*/
static rex_tcb_type* gcsd_tcb_ptr; 


LOCAL uint8 Gcsd_l1_tx_buf[GCSD_L1_NUM_TX_BUFS][GCSD_L1_BUF_SIZE] = {{0}};
LOCAL uint8 Gcsd_l1_default_tx_buf [GCSD_L1_BUF_SIZE] = 
     { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
       0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
       0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
       0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
       0xff,0xff,0xff,0xff,0xff,0xff};

LOCAL gcsd_buffer   Gcsd_l1_rx_buf[GCSD_L1_NUM_RX_BUFS] = {{NULL}};
LOCAL gcsd_buffer   Gcsd_l1_default_rx_buf = {NULL};


LOCAL boolean Ds_gcsd_first_write_call_made = FALSE;
LOCAL boolean Ds_gcsd_first_read_call_made  = FALSE;


#define BUF_IS_FULL(buf_mgr) (buf_mgr.count == (buf_mgr.size - 1))

#define BUF_IS_EMPTY(buf_mgr) (buf_mgr.count == 0)

#define INC_READ_INDEX(buf_mgr)  if (++buf_mgr.read_index >= buf_mgr.size) \
                                 {                                         \
                                    buf_mgr.read_index = 0;                \
                                 }                                         \
                                 buf_mgr.count--
                                 
#define INC_WRITE_INDEX(buf_mgr) if (++buf_mgr.write_index >= buf_mgr.size) \
                                 {                                          \
                                    buf_mgr.write_index = 0;                \
                                 }                                          \
                                 buf_mgr.count++

/* #define BUF_COUNT(buf_mgr) buf_mgr.count */


/* <EJECT> */
/*===========================================================================

FUNCTION  ds_gcsd_l1_init

DESCRIPTION
   Initializes the GSM protocol stack <-> Layer 1 interface subsystem
         
DEPENDENCIES
   None.
  
RETURN VALUE
   None.
    
SIDE EFFECTS
   Yes.

===========================================================================*/
void
ds_gcsd_l1_init (void)
{
   Ds_gcsd_first_write_call_made = Ds_gcsd_first_read_call_made = FALSE;
}


/*===========================================================================

FUNCTION GCSD_OTA_RX_CALLBACK

DESCRIPTION
  This callback function is called by L1 every 4th frame (after every 
  data message has been received).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void gcsd_ota_rx_callback
(
   gcsd_buffer_ptr      curr_buf, 
   gcsd_buffer_ptr      *next_buf, 
   gcsd_receive_status  status
)
{
  static uint32 prev_overrun = 0;
  register uint32 error = (status != GCSD_RX_NORMAL);

  if (error)
  {
     DATA_MSG1_ERROR("Error status on L1 RX callback=%d",status);
  }
   
  if (BUF_IS_FULL (Gcsd_l1_rx_buf_mgr))
  {
    *next_buf = &Gcsd_l1_default_rx_buf;
    if (!prev_overrun)
    {
      DATA_MSG0_ERROR("GCSD L1 RX overrun");
    }
    prev_overrun++;
  }
  else
  {
    DS_GCSD_L1_RX_BUFF_LOCK()
    if (!error)
    {
      INC_WRITE_INDEX (Gcsd_l1_rx_buf_mgr);
    }
    *next_buf = &Gcsd_l1_rx_buf[Gcsd_l1_rx_buf_mgr.write_index];
      
    DS_GCSD_L1_RX_BUFF_UNLOCK()
    if (prev_overrun)
    {
      DATA_MSG1_ERROR("GCSD L1 RX overrun cleared %d",prev_overrun);
      prev_overrun = 0;
    }
         
  }
   
  if (!error)
  {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
    gcsd_tcb_ptr = rcinit_lookup_rextask("ds_gcsd");
    if ( RCINIT_NULL == gcsd_tcb_ptr)
    {
      ASSERT(0);
    }
#else
    gcsd_tcb_ptr = &Gcsd_tcb;
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
    (void) rex_set_sigs(gcsd_tcb_ptr, DS_GCSD_PROC_DL_SIG);
  }
}


/*===========================================================================

FUNCTION GCSD_OTA_TX_CALLBACK

DESCRIPTION
  This callback function is called by L1 every 4th frame (before 
  every data message is sent).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void gcsd_ota_tx_callback
(
   uint8    **curr_buf, 
   uint8    *prev_buf, 
   boolean  tx_enabled
)
{   
   static uint32 prev_underrun = 0;   

   if (BUF_IS_EMPTY (Gcsd_l1_tx_buf_mgr))
   {
      *curr_buf = Gcsd_l1_default_tx_buf;
      if (!prev_underrun)
      {
         DATA_MSG0_ERROR("GCSD L1 TX underrun");
      }
      prev_underrun++;
   }
   else
   {
   
      DS_GCSD_L1_TX_BUFF_LOCK()
      *curr_buf = Gcsd_l1_tx_buf[Gcsd_l1_tx_buf_mgr.read_index];
      INC_READ_INDEX (Gcsd_l1_tx_buf_mgr);
      DS_GCSD_L1_TX_BUFF_UNLOCK()
      
      if (prev_underrun)
      {
         DATA_MSG1_ERROR("GCSD L1 TX underrun cleared=%d",prev_underrun);
         prev_underrun = 0;
      }
   }

#ifdef FEATURE_MODEM_RCINIT_PHASE2
   gcsd_tcb_ptr = rcinit_lookup_rextask("ds_gcsd"); 
   if ( RCINIT_NULL == gcsd_tcb_ptr)
   {
      ASSERT(0);
   }
#else
   gcsd_tcb_ptr = &Gcsd_tcb;
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
   (void) rex_set_sigs(gcsd_tcb_ptr, DS_GCSD_PROC_UL_SIG);

   
}



/* <EJECT> */
/*===========================================================================

FUNCTION  ds_gcsd_read_from_channel_decoder

DESCRIPTION
   Reads an SDU's worth of data from the Layer 1.  Pointer value passed in
   is set to point to the incoming data.  If no data is available, FALSE
   is returned by function and pointer value is undefined.
         
DEPENDENCIES
   ds_gcsd_l1_init() should have been previously called.
  
RETURN VALUE
   TRUE if data was available and radio_rx_data was made to point at RX data
   FALSE if no data was available, radio_rx_data is undefined.
    
SIDE EFFECTS
   Yes.

===========================================================================*/
boolean 
ds_gcsd_read_from_channel_decoder
(
   uint8 **radio_rx_data
)
{

   /* If we have no receive data, return FALSE */
   if (BUF_IS_EMPTY (Gcsd_l1_rx_buf_mgr))
   {
      return FALSE;
   }

   
   *radio_rx_data = (uint8 *) &Gcsd_l1_rx_buf[Gcsd_l1_rx_buf_mgr.read_index] + 
                   Gcsd_l1_rx_buf[Gcsd_l1_rx_buf_mgr.read_index].data_offset;
   
   DS_GCSD_L1_RX_BUFF_LOCK()
   INC_READ_INDEX (Gcsd_l1_rx_buf_mgr);
   DS_GCSD_L1_RX_BUFF_UNLOCK()
   

   if ((Ds_gcsd_first_read_call_made) && (!BUF_IS_EMPTY (Gcsd_l1_rx_buf_mgr)))
   {
      DS_UCSD_DEBUG_MSG ("STILL data in RX buf");
#ifdef FEATURE_MODEM_RCINIT_PHASE2
      gcsd_tcb_ptr = rcinit_lookup_rextask("ds_gcsd"); 
      if ( RCINIT_NULL == gcsd_tcb_ptr)
      {
         ASSERT(0);
      }
#else
      gcsd_tcb_ptr = &Gcsd_tcb;
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
      (void) rex_set_sigs(gcsd_tcb_ptr, DS_GCSD_PROC_DL_SIG);

   }
       
   Ds_gcsd_first_read_call_made = TRUE;
   return TRUE;

}



/* <EJECT> */
/*===========================================================================

FUNCTION  ds_gcsd_get_tx_buffer

DESCRIPTION
   Sets the radio_tx_data pointer to point to a buffer that should be used
   to put the next SDU's worth of data into and subsequently transmitted
   using the ds_gcsd_write_to_channel_coder() function. 
         
DEPENDENCIES
   ds_gcsd_l1_init() should have been previously called.
  
RETURN VALUE
   TRUE if a buffer was available and radio_tx_data is pointing at it.
   FALSE if no buffer was available, radio_tx_data is undefined.
    
SIDE EFFECTS
   Yes.

===========================================================================*/
boolean
ds_gcsd_get_tx_buffer 
(
   uint8 **radio_tx_data
)
{
   if (BUF_IS_FULL (Gcsd_l1_tx_buf_mgr))
   {
      DATA_MSG0_ERROR("ds_gcsd_write_to_channel_coder: Tx Overrun!");
      return FALSE;
   }
   *radio_tx_data = Gcsd_l1_tx_buf[Gcsd_l1_tx_buf_mgr.write_index];
   return TRUE;
  
}
   
      


/* <EJECT> */
/*===========================================================================

FUNCTION  ds_gcsd_write_to_channel_coder

DESCRIPTION
   Transmits the radio_tx_data buffer.  The transmitted buffer must be the
   one retrieved by the ds_gcsd_get_tx_buffer() call or the transmission
   will fail. 
         
DEPENDENCIES
   ds_gcsd_l1_init() should have been previously called. 
   ds_gcsd_get_tx_buffer() must have been used to retrieve the buffer.
  
RETURN VALUE
   TRUE if transmission was successfully queued to L1
   FALSE if not.
    
SIDE EFFECTS
   Yes.

===========================================================================*/
boolean 
ds_gcsd_write_to_channel_coder
(
   uint8 *radio_tx_data
)
{
   if (radio_tx_data != Gcsd_l1_tx_buf[Gcsd_l1_tx_buf_mgr.write_index])
   {
      DATA_MSG0_ERROR("Buffer pointer mismatch");
      return FALSE;
   }
   
   if (BUF_IS_FULL (Gcsd_l1_tx_buf_mgr))
   {
      DATA_MSG0_ERROR("GCSD TX buffer full");
      return FALSE;
   }
  
   DS_GCSD_L1_TX_BUFF_LOCK()
   INC_WRITE_INDEX (Gcsd_l1_tx_buf_mgr);
   DS_GCSD_L1_TX_BUFF_UNLOCK()
     
   if ((Ds_gcsd_first_write_call_made) && (!BUF_IS_FULL(Gcsd_l1_tx_buf_mgr)))
   {
      DS_UCSD_DEBUG_MSG ("DATA needed for TX buf");
#ifdef FEATURE_MODEM_RCINIT_PHASE2
      gcsd_tcb_ptr = rcinit_lookup_rextask("ds_gcsd"); 
      if ( RCINIT_NULL == gcsd_tcb_ptr)
      {
         ASSERT(0);
      }
#else
      gcsd_tcb_ptr = &Gcsd_tcb;
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
      (void) rex_set_sigs(gcsd_tcb_ptr, DS_GCSD_PROC_UL_SIG);

   }      

   Ds_gcsd_first_write_call_made = TRUE;
   return TRUE;
}
#endif /* FEATURE_GSM && FEATURE_DATA_GCSD */

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
/*===========================================================================

FUNCTION  DS_GCSD_GET_DS_GCSD_L1_RX_BUF_CRIT_SECT_PTR

DESCRIPTION
   Returns a pointer to the variable ds_gcsd_l1_rx_buf_crit_sect. 
         
DEPENDENCIES
   None 

RETURN VALUE
   rex_crit_sect_type* - pointer to ds_gcsd_l1_rx_buf_crit_sect
    
SIDE EFFECTS
   None

===========================================================================*/
rex_crit_sect_type*
ds_gcsd_get_l1_rx_buf_crit_sect_ptr(void)
{
  return &ds_gcsd_l1_rx_buf_crit_sect;
}

/*===========================================================================

FUNCTION  DS_GCSD_GET_DS_GCSD_L1_TX_BUF_CRIT_SECT_PTR

DESCRIPTION
   Returns a pointer to the variable ds_gcsd_l1_tx_buf_crit_sect. 
         
DEPENDENCIES
   None 

RETURN VALUE
   rex_crit_sect_type* - pointer to ds_gcsd_l1_tx_buf_crit_sect
    
SIDE EFFECTS
   None

===========================================================================*/
rex_crit_sect_type*
ds_gcsd_get_l1_tx_buf_crit_sect_ptr(void)
{
  return &ds_gcsd_l1_tx_buf_crit_sect;
}
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */


