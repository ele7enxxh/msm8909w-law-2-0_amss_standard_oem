#ifndef DSUCSDNTI_H
#define DSUCSDNTI_H
/*===========================================================================

            U M T S   N O N - T R A N S P A R E N T   C I R C U I T
                          S W I T C H E D   D A T A      
                            H E A D E R   F I L E

DESCRIPTION
   This header file contains the interface the the GSM/WCDMA 
   Non-transparent Circuit Switched data modeule. 
  
Copyright (c) 2003 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdnti.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
02/09/06   AR      Lint corrections.
08/16/04   AR      Added export of Ds_FillPattern.
05/10/04   AR      Relocate PDU_SIZE macro to internal header file. Lint fix
03/17/04   AR      Added ds_ucsd_remote_fc_enabled prototype
03/10/04   AR      Added ds_ucsd_UnpackPDU prototype
07/15/03   TMR     Added support for RLP Version 2
04/30/03   AR      Add suppport for DSM items inplace of local buffers.
02/17/03   TMR     Created

===========================================================================*/


/*===========================================================================
Header files
===========================================================================*/
#include "datamodem_variation.h"
#include "dsucsdnt.h"
#include "dsucsdi.h"
#include "dsm.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**********************************************************
** Date that is initialized via the 
** ds_ucsd_nt_init_call_data() function call
***********************************************************/

/* Types to be used for pointers to buffer functions */
typedef void      (*FPTR_WRITE_TO_BUFFER)   (dsm_item_type**);
typedef uint32    (*FPTR_READ_FROM_BUFFER)  (dsm_item_type**,uint32);
typedef boolean   (*FPTR_CHECK_FULL_BUFFER) (void);
typedef boolean   (*FPTR_CHECK_EMPTY_BUFFER)(void);
typedef void      (*FPTR_INIT_BUFFER)       (void);
typedef short     (*FPTR_CHECK_BUFFER)      (void);


/* Flag to indicate DSM item pool is low; used for Tx flow control */
#define RLP_DSM_MEM_LEVEL_FEW   80    /* Tx Flow control enabled */
#define RLP_DSM_MEM_LEVEL_MANY  90    /* Tx Flow control disabled */

/* Care must be taken to make sure the following don't 
** intersect with ds_ucsd_event_T.  But it is easier to translate
** L2R/RLP event numbers if they are explicitly initialized
*/

#define START_L2R_EVENT_ID 20

/*#if START_L2R_EVENT_ID < DS_UMTS_CSD_NUM_EVENTS
#error L2R/RLP events clash with ds_ucsd_event_T
#endif
*/

enum
{   
   /* Events internal to L2R (20-29) */
   L2R_DTE_BREAK_IND    = START_L2R_EVENT_ID,
   L2R_BREAK_ACK_IND    = START_L2R_EVENT_ID + 1,   
   L2R_REMAP_REQ        = START_L2R_EVENT_ID + 2,
   L2R_FLUSH_DONE       = START_L2R_EVENT_ID + 3,
   L2R_FLUSH_TIMEOUT    = START_L2R_EVENT_ID + 4,
   L2R_FLUSH_ABORT      = START_L2R_EVENT_ID + 5,
   L2R_RX_INFO_FRAME    = START_L2R_EVENT_ID + 6,

   /* Events to/from RLP and L2R, or internal to RLP ( >30)  */
   RLP_CONNECT_REQ      = START_L2R_EVENT_ID + 10,
   RLP_CONNECT_IND      = START_L2R_EVENT_ID + 11, 
   RLP_CONNECT_RES      = START_L2R_EVENT_ID + 12, 
   RLP_CONNECT_RES_NEG  = START_L2R_EVENT_ID + 13, 
   RLP_CONNECT_CNF      = START_L2R_EVENT_ID + 14, 
   RLP_CONNECT_CNF_NEG  = START_L2R_EVENT_ID + 15,
   RLP_RESET_REQ        = START_L2R_EVENT_ID + 16, 
   RLP_RESET_CNF        = START_L2R_EVENT_ID + 17, 
   RLP_RESET_IND        = START_L2R_EVENT_ID + 18, 
   RLP_RESET_RES        = START_L2R_EVENT_ID + 19, 
   RLP_DISC_REQ         = START_L2R_EVENT_ID + 20, 
   RLP_DISC_IND         = START_L2R_EVENT_ID + 21, 
   RLP_UI_REQ           = START_L2R_EVENT_ID + 22, 
   RLP_UI_IND           = START_L2R_EVENT_ID + 23, 
   RLP_ATTACH_REQ       = START_L2R_EVENT_ID + 24, 
   RLP_DETACH_REQ       = START_L2R_EVENT_ID + 25, 
   RLP_TEST_REQ         = START_L2R_EVENT_ID + 26, 
   RLP_XID_REQ          = START_L2R_EVENT_ID + 27, 
   RLP_XID_CNF          = START_L2R_EVENT_ID + 28, 
   RLP_XID_IND          = START_L2R_EVENT_ID + 29, 
   RLP_ERROR_IND        = START_L2R_EVENT_ID + 30, 
   RLP_TEST_CNF         = START_L2R_EVENT_ID + 31, 
   RLP_TEST_CNF_NEG     = START_L2R_EVENT_ID + 32, 
   RLP_REMAP_REQ        = START_L2R_EVENT_ID + 33, 
   RLP_REMAP_CNF        = START_L2R_EVENT_ID + 34, 
   RLP_REMAP_IND        = START_L2R_EVENT_ID + 35, 
   RLP_REMAP_RES        = START_L2R_EVENT_ID + 36,
   RLP_REMAP_END_IND    = START_L2R_EVENT_ID + 37

};

/************************************************************
* Event Queue declarations 
*************************************************************/

/* Byte reflection stuff */
#define DS_UCSD_REFLECT_BYTE(x) ds_Hardware_access_byte_reflection_table(x)

/* Macros to read/write events */
#ifdef DS_UCSD_DEBUG

#define WRITE_EVENT_TO_RLP(x) ds_ucsd_write_event(ds_ucsd_nt_get_Evq_To_RLP(), x); \
                              DS_UCSD_DEBUG_MSG_1 ("WRITE_EVENT to RLP=%d",x)
                              
#define WRITE_EVENT_TO_L2R(x) ds_ucsd_write_event(ds_ucsd_nt_get_Evq_To_L2R(), x); \
                              DS_UCSD_DEBUG_MSG_1 ("WRITE_EVENT to L2R=%d",x)
#else /* DS_UCSD_DEBUG */

#define WRITE_EVENT_TO_RLP(x) ds_ucsd_write_event(ds_ucsd_nt_get_Evq_To_RLP(), x)
                              
#define WRITE_EVENT_TO_L2R(x) ds_ucsd_write_event(ds_ucsd_nt_get_Evq_To_L2R(), x)

#endif /* DS_UCSD_DEBUG */

#define READ_EVENT_TO_RLP()      ds_ucsd_read_event (ds_ucsd_nt_get_Evq_To_RLP());
#define READ_EVENT_TO_L2R()      ds_ucsd_read_event (ds_ucsd_nt_get_Evq_To_L2R());

#define PDU_PAYLOAD_SIZE() \
((ds_ucsd_nt_get_bytes_per_sdu() == DS_UMTS_CSD_FRAME_SIZE_240_BIT) ? \
                                    DS_UMTS_CSD_FRAME_SIZE_240_BIT :  \
                                    DS_UMTS_CSD_FRAME_SIZE_576_BIT)
                                            

extern DSI_Event_T 
ds_ucsd_read_event  
(
   DSISR_EventQueue_T   *Queue
);

extern void 
ds_ucsd_write_event 
(
   DSISR_EventQueue_T   *Queue, 
   DSI_Event_T          event
);


uint32
ds_ucsd_V24_interface_handler_byte_oriented
(
   dsm_item_type  **PDU_ptr,
   uint32         num_bytes_needed  
);

void
ds_ucsd_V24_interface_driver_byte_oriented
(
   dsm_item_type ** PDU_ptr
);


/*===========================================================================

FUNCTION
   ds_ucsd_UnpackPDU

DESCRIPTION
   This routine decodes the PDUs (for Phase2).
   The data bytes are read from the PDU and written to 
   the appropriate rx buffer (depending on whether or not
   rx compression is being used). The rx status info 
   is stored.
         
DEPENDENCIES
  None
  
RETURN VALUE
  Number of bytes in PDU 
    
SIDE EFFECTS
  None

===========================================================================*/
extern uint32
ds_ucsd_UnpackPDU 
(
   dsm_item_type ** pdu_ptr,
   PDU_Type PDU_type,
   boolean useStatusBytes
);

  
/*===========================================================================

FUNCTION
   ds_ucsd_remote_fc_enabled()

DESCRIPTION
  This routine reports the flow control state of the remote entity
  as decoded from teh L2R status bits.
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE it CTS signal deasserted, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
extern boolean
ds_ucsd_remote_fc_enabled ( void );

/****************************************************************************
 * Function name:  ds_Hardware_access_byte_reflection_table
 * -------------
 * Description:    Returns a value in the byte reflection table corresponding
                   to the index.
 * -----------     
 * Parameters:     index
 * ----------
 * Returns:        uint8
 * --------
 ****************************************************************************/
uint8 ds_Hardware_access_byte_reflection_table(uint8 index);

#ifdef __cplusplus
}
#endif

#endif /* DSUCSDNTI_H */
