/*===========================================================================

              GSM Circuit Switched Data XXXX Module

DESCRIPTION
  This file contains the implementation of the GSM Circuit switched 
  data XXXX functionality.

   Copyright (c) 2002 - 2014 by Qualcomm Technologies INCORPORATED.
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

  $PVCSPath:$
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdrlphw.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/12   SA      Fix to avoid crash due to dsm extract problem.
04/05/11   SS      Q6 free floating support.
09/29/09   SA      Fixed KW errors.
07/09/09   BS      Fixed offtarget lint medium and low's.
03/13/08   SA      Fixing Klocwork issues.
02/06/08   SA      Updated RX Frame to include FCS error.
23/02/07   DA      Fixed high lint errors
02/09/07   DA      Fixed critical lint errors
08/14/06   AR      Clear RLP interface buffers after handover.
02/09/06   AR      Lint corrections.
02/02/06   AR      Check DSM item size before chaining Rx items
08/16/04   AR      Use user data fill pattern defined in Ds_FillPattern.
05/30/04   AR      Lint corrections.
09/18/03   AR      Cleared dangling DSM item reference.
09/17/03   TMR     Added support for TX frame suppression
07/15/03   TMR     Added support for RLP Version 2
04/21/03   AR      Add support for using DSM items inplace of static buffers
                   Lint corrections
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/




/******************************************************************************* *
 *  File:         /home/briers/gsmqc/ds/SCCS/s.m51303.c
 *  -----
 *
 *  SCCS Version: 2.5
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
 *  Description:    RLP Frame Hardware interface functions
 *  ------------
 *
 *  Functions Used:
 *  ---------------
 *
 *  Modification History:
 *  ---------------------
 *  Date       Vers  Auth   Details
 *  ----       ----  ----   -------
 *  01-Jun-99  2.1   RCB    Original version
 *  22-Jun-00  2.2   RCB    Changes for Phase2+
 *  26-Jul-00  2.3   RCB    Update file header
 *  17-Aug-00  2.4   RCB    Mods to use ghdi for radio interface
 *  06-Sep-00  2.5   RCB    Bugs to ghdi i/face fixed during testing
 *
 *****************************************************************************/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)


/* Turn debugging off in this file */
#define DS_GCSD_FILE_DEBUG_OFF
#include "dsucsdnti.h"
#include "dsucsdlog.h"
#include "data_msg.h"
#include "dsucsdrlp.h"
#include "memory.h"
#include "err.h"
#include "amssassert.h"

static const uint32 ds_ucsd_fcs_transform[256] =
{
  0x00000000L, 0x00BBA1B5L, 0x00CCE2DFL, 0x0077436AL, 
  0x0022640BL, 0x0099C5BEL, 0x00EE86D4L, 0x00552761L, 
  0x0044C816L, 0x00FF69A3L, 0x00882AC9L, 0x00338B7CL, 
  0x0066AC1DL, 0x00DD0DA8L, 0x00AA4EC2L, 0x0011EF77L, 
  0x0089902CL, 0x00323199L, 0x004572F3L, 0x00FED346L, 
  0x00ABF427L, 0x00105592L, 0x006716F8L, 0x00DCB74DL, 
  0x00CD583AL, 0x0076F98FL, 0x0001BAE5L, 0x00BA1B50L, 
  0x00EF3C31L, 0x00549D84L, 0x0023DEEEL, 0x00987F5BL, 
  0x00A881EDL, 0x00132058L, 0x00646332L, 0x00DFC287L, 
  0x008AE5E6L, 0x00314453L, 0x00460739L, 0x00FDA68CL, 
  0x00EC49FBL, 0x0057E84EL, 0x0020AB24L, 0x009B0A91L, 
  0x00CE2DF0L, 0x00758C45L, 0x0002CF2FL, 0x00B96E9AL, 
  0x002111C1L, 0x009AB074L, 0x00EDF31EL, 0x005652ABL, 
  0x000375CAL, 0x00B8D47FL, 0x00CF9715L, 0x007436A0L, 
  0x0065D9D7L, 0x00DE7862L, 0x00A93B08L, 0x00129ABDL, 
  0x0047BDDCL, 0x00FC1C69L, 0x008B5F03L, 0x0030FEB6L, 
  0x00EAA26FL, 0x005103DAL, 0x002640B0L, 0x009DE105L, 
  0x00C8C664L, 0x007367D1L, 0x000424BBL, 0x00BF850EL, 
  0x00AE6A79L, 0x0015CBCCL, 0x006288A6L, 0x00D92913L, 
  0x008C0E72L, 0x0037AFC7L, 0x0040ECADL, 0x00FB4D18L, 
  0x00633243L, 0x00D893F6L, 0x00AFD09CL, 0x00147129L, 
  0x00415648L, 0x00FAF7FDL, 0x008DB497L, 0x00361522L, 
  0x0027FA55L, 0x009C5BE0L, 0x00EB188AL, 0x0050B93FL, 
  0x00059E5EL, 0x00BE3FEBL, 0x00C97C81L, 0x0072DD34L, 
  0x00422382L, 0x00F98237L, 0x008EC15DL, 0x003560E8L, 
  0x00604789L, 0x00DBE63CL, 0x00ACA556L, 0x001704E3L, 
  0x0006EB94L, 0x00BD4A21L, 0x00CA094BL, 0x0071A8FEL, 
  0x00248F9FL, 0x009F2E2AL, 0x00E86D40L, 0x0053CCF5L, 
  0x00CBB3AEL, 0x0070121BL, 0x00075171L, 0x00BCF0C4L, 
  0x00E9D7A5L, 0x00527610L, 0x0025357AL, 0x009E94CFL, 
  0x008F7BB8L, 0x0034DA0DL, 0x00439967L, 0x00F838D2L, 
  0x00AD1FB3L, 0x0016BE06L, 0x0061FD6CL, 0x00DA5CD9L, 
  0x006EE56BL, 0x00D544DEL, 0x00A207B4L, 0x0019A601L, 
  0x004C8160L, 0x00F720D5L, 0x008063BFL, 0x003BC20AL, 
  0x002A2D7DL, 0x00918CC8L, 0x00E6CFA2L, 0x005D6E17L, 
  0x00084976L, 0x00B3E8C3L, 0x00C4ABA9L, 0x007F0A1CL, 
  0x00E77547L, 0x005CD4F2L, 0x002B9798L, 0x0090362DL, 
  0x00C5114CL, 0x007EB0F9L, 0x0009F393L, 0x00B25226L, 
  0x00A3BD51L, 0x00181CE4L, 0x006F5F8EL, 0x00D4FE3BL, 
  0x0081D95AL, 0x003A78EFL, 0x004D3B85L, 0x00F69A30L, 
  0x00C66486L, 0x007DC533L, 0x000A8659L, 0x00B127ECL, 
  0x00E4008DL, 0x005FA138L, 0x0028E252L, 0x009343E7L, 
  0x0082AC90L, 0x00390D25L, 0x004E4E4FL, 0x00F5EFFAL, 
  0x00A0C89BL, 0x001B692EL, 0x006C2A44L, 0x00D78BF1L, 
  0x004FF4AAL, 0x00F4551FL, 0x00831675L, 0x0038B7C0L, 
  0x006D90A1L, 0x00D63114L, 0x00A1727EL, 0x001AD3CBL, 
  0x000B3CBCL, 0x00B09D09L, 0x00C7DE63L, 0x007C7FD6L, 
  0x002958B7L, 0x0092F902L, 0x00E5BA68L, 0x005E1BDDL, 
  0x00844704L, 0x003FE6B1L, 0x0048A5DBL, 0x00F3046EL, 
  0x00A6230FL, 0x001D82BAL, 0x006AC1D0L, 0x00D16065L, 
  0x00C08F12L, 0x007B2EA7L, 0x000C6DCDL, 0x00B7CC78L, 
  0x00E2EB19L, 0x00594AACL, 0x002E09C6L, 0x0095A873L, 
  0x000DD728L, 0x00B6769DL, 0x00C135F7L, 0x007A9442L, 
  0x002FB323L, 0x00941296L, 0x00E351FCL, 0x0058F049L, 
  0x00491F3EL, 0x00F2BE8BL, 0x0085FDE1L, 0x003E5C54L, 
  0x006B7B35L, 0x00D0DA80L, 0x00A799EAL, 0x001C385FL, 
  0x002CC6E9L, 0x0097675CL, 0x00E02436L, 0x005B8583L, 
  0x000EA2E2L, 0x00B50357L, 0x00C2403DL, 0x0079E188L, 
  0x00680EFFL, 0x00D3AF4AL, 0x00A4EC20L, 0x001F4D95L, 
  0x004A6AF4L, 0x00F1CB41L, 0x0086882BL, 0x003D299EL, 
  0x00A556C5L, 0x001EF770L, 0x0069B41AL, 0x00D215AFL, 
  0x008732CEL, 0x003C937BL, 0x004BD011L, 0x00F071A4L, 
  0x00E19ED3L, 0x005A3F66L, 0x002D7C0CL, 0x0096DDB9L, 
  0x00C3FAD8L, 0x00785B6DL, 0x000F1807L, 0x00B4B9B2L
};

/* ----------------------------------------------------------------------- */

/* Octet reflection lookup table for remapping FCS octets to achieve the
   required field mapping. */

static const uint8 Ds_ucsd_byte_reflection_table[256] =
{
  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
  0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
  0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
  0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
  0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
  0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
  0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
  0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
  0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
  0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
  0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
  0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
  0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
  0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
  0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
  0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
  0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
  0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
  0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
  0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
  0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
  0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
  0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
  0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
  0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
  0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
  0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};


/* ----------------------------------------------------------------------- */

#define GLLC_FCS_SEED     0x00FFFFFFL

#define RX_FCS_OK_RESULT  0x006D8930L

/* ----------------------------------------------------------------------- */


/****************************************************************************
 * Module scope variables
 ****************************************************************************/
LOCAL uint32 rlp_rx_frame_available  = FALSE;
LOCAL uint32 rlp_tx_ready            = TRUE;

  
LOCAL dsm_item_type *        TxFrameBuf = NULL;
LOCAL dsm_item_type *        RxFrameBuf = NULL;
LOCAL uint32                 TxCycle = 0;
LOCAL uint32                 RxCycle = 0;


LOCAL const byte on_bit_masks[8] =  {0x01,
                                      0x02,
                                      0x04,
                                      0x08,
                                      0x10,
                                      0x20,
                                      0x40,
                                      0x80};

LOCAL const byte off_bit_masks[8] = {0xfe,
                                      0xfd,
                                      0xfb,
                                      0xf7,
                                      0xef,
                                      0xdf,
                                      0xbf,
                                      0x7f};

/****************************************************************************
 * Module scope functions
 ****************************************************************************/
 

/****************************************************************************
 * Function name:  copy_tx_data_60
 * -------------
 * Description:    Copies 60-bit frames from a buffer to a radio_tx_data 
 * -----------     
 * Parameters:     radio_tx_data ptr, RLP frame ptr
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
LOCAL void  
copy_tx_data_60 
(
   uint8 *radio_tx_data,
   dsm_item_type ** rlp_frame_buf
)
{
   if(rlp_frame_buf == NULL)
   {
     DATA_MSG0_ERROR("NULL ptr input to copy_tx_data_60()");
     return;
   }
   ASSERT (NULL != *rlp_frame_buf);
   
   /* Copy flattened RLP frame data to the tx_data_ptr */
   DSM_EXTRACT (*rlp_frame_buf,                   /* DSM item */
                0,                                /* Offset   */
                (byte*)radio_tx_data,             /* Out Buf  */
                (uint16)BYTES_PER_9600_20_MSEC);  /* Length   */ /*lint !e666*/
}

   
/****************************************************************************
 * Function name:  copy_rx_data_60
 * -------------
 * Description:    Copies 60-bit frames in radio_rx_data to RLP frmae
 * -----------     
 * Parameters:     radio_tx_data ptr, RLP frame ptr
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
LOCAL void 
copy_rx_data_60 
(
   uint8    *radio_rx_data,
   dsm_item_type **rlp_frame_buf
)
{
   if(rlp_frame_buf == NULL)
   {
     DATA_MSG0_ERROR("NULL ptr input to copy_rx_60()");
     return;
   }

   ASSERT (NULL != *rlp_frame_buf);
   ASSERT (0 == (*rlp_frame_buf)->used);  /* Ensure clean packet */  
   
   /* Copy the header and data portion to the rlp_frame */
   DSM_PUSHDOWN_TAIL (rlp_frame_buf, 
                      (byte*)radio_rx_data,
                      BYTES_PER_9600_20_MSEC); 
}




/****************************************************************************
 * Function name:  copy_tx_data_36
 * -------------
 * Description:    Copies 36-bit frames from a buffer to a radio_tx_data 
 * -----------     
 * Parameters:     radio_tx_data ptr, RLP frame ptr, frame size
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
/*ARGSUSED*/
LOCAL void  
copy_tx_data_36 
(
   uint8                *radio_tx_data,
   dsm_item_type      **rlp_frame_buf,
   uint32               second_half_of_frame,
   ds_ucsd_frame_size_T frame_size
)   
{
   uint16 offset = (second_half_of_frame)? (uint16)frame_size : 0;

   if(rlp_frame_buf == NULL)
   {
     DATA_MSG0_ERROR("NULL ptr input to copy_tx_data_36()");
     return;
   }
   
   ASSERT (NULL != *rlp_frame_buf);
                     
   /* Copy flattened RLP frame data to the tx_data_ptr */
   if (frame_size != dsm_extract(*rlp_frame_buf, offset, (void*)radio_tx_data, (uint16)frame_size))
   {
     ERR("DSM extract problem ",0,0,0);
     *rlp_frame_buf = NULL;
   }
}

/****************************************************************************
 * Function name:  copy_rx_data_36
 * -------------
 * Description:    Copies 36-bit frames in radio_rx_data to RLP frmae
 * -----------     
 * Parameters:     radio_tx_data ptr, RLP frame ptr, frame size
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
/*ARGSUSED*/
LOCAL void  
copy_rx_data_36 
(
   uint8                *radio_rx_data,
   dsm_item_type       **rlp_frame_buf,
   uint32               second_half_of_frame,
   ds_ucsd_frame_size_T frame_size
)
{
   if(rlp_frame_buf == NULL)
   {
     DATA_MSG0_ERROR("NULL ptr input to copy_rx_data_36()");
     return;
   }

   ASSERT (NULL != *rlp_frame_buf);
   
   /* Put buffer in designated half of frame */
   if (second_half_of_frame)
   {
      /* Trim any chained packets */
      dsm_trim_packet (&(*rlp_frame_buf)->pkt_ptr, 0);
      
      /* Set load into second half of packet. */
      (*rlp_frame_buf)->used = (word)frame_size;
   
      /* Copy the input buffer into end of packet */
      DSM_PUSHDOWN_TAIL (rlp_frame_buf,
                         (byte*)radio_rx_data,
                         (uint16)frame_size);
   }
   else
   {
      /* Adjust DSM item offset for GSM 14400 case */
      if (DS_UMTS_CSD_FRAME_SIZE_288_BIT == frame_size)
      {
         /* Advance packet pointer to second half of packet. */
         /* Modeled after "dsm_offset_new_buffer()"          */
         (*rlp_frame_buf)->data_ptr += (uint32)frame_size;
         (*rlp_frame_buf)->used = (word)((*rlp_frame_buf)->size -
                                         (uint16)frame_size);
         (*rlp_frame_buf)->size -= (word)frame_size;
      }
      
      /* Copy the input buffer into front of packet */
      DSM_PUSHDOWN (rlp_frame_buf,
                    (byte*)radio_rx_data,
                    (uint16)frame_size);
   }

}   

/****************************************************************************
 * Function name:  dispose_tx_buffer
 * -------------
 * Description:    Dispose of the Tx Frame buffer.  Information frames must
 *                 be retained until ACK'd. Other frame types may be
 *                 purged after transmission.
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
LOCAL void  
dispose_tx_buffer ( void )
{
   //MSG_LOW ("In dispose_tx_buffer",0,0,0);
   uint8 garbage [4];
   uint16 rlp_hdr_size;

  if( TxFrameBuf != NULL )
  {   
    /* Test for frame type */
    if (TxFrameBuf->app_field & RLP_DSM_APP_FIELD_INFO_FRAME) 
    {
       /* Strip Header and FCS from packet as they are reapplied on Tx */
 
       /* Remove RLP header data */
       rlp_hdr_size = get_current_rlp_header_size();
       DSM_PULLUP (&TxFrameBuf, garbage, rlp_hdr_size);
 
       /* Strip FCS bytes at tail */
       DSM_PULLUP_TAIL (&TxFrameBuf, garbage, RLP_FCS_SIZE);  /*lint !e666*/
 
    }
    else
    {
       /* Unnumbered or Supervisory frame */
       /* Release DSM item */
       ds_ucsd_UplinkPDUMgr_Free (&TxFrameBuf);
    }
    /* Set TxFrameBuf pointer to NULL */
    TxFrameBuf = NULL;
  }
}


/****************************************************************************
 * Function name:  ds_HardwareInitialise
 * -------------
 * Description:    Initialise the RLP Hardware interface module.
 *                 The Tx and Rx modules can be initialised independently
 *                 to support clearing Rx chain on HO.
 * -----------     
 * Parameters:     initTx flag, initRx flag
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
void ds_HardwareInitialise
(
  boolean initTx,
  boolean initRx
)
{
  RLP_Frame_T *RxFrame = NULL;
  RLP_Frame_T *TxFrame = NULL;
  
  RxFrame = ds_ucsd_get_RxFrame();
  TxFrame = ds_ucsd_get_TxFrame();
  /* Clear Tx elements */  
  if( initTx )
  {
    rlp_tx_ready = TRUE;
    TxCycle = 0;
     
    /* Clear the Frame buffers */
    ds_ucsd_UplinkPDUMgr_Free ( &(TxFrame->Data) );
    if ((NULL != TxFrameBuf) && (NULL != TxFrameBuf->pkt_ptr))
    {
      ds_ucsd_UplinkPDUMgr_Free (&(TxFrameBuf->pkt_ptr));
      TxFrameBuf->pkt_ptr = NULL;
    }
    ds_ucsd_UplinkPDUMgr_Free (&TxFrameBuf);
  }
   
  /* Clear Rx elements */  
  if( initRx )
  {
    rlp_rx_frame_available = FALSE;
    RxCycle = 0;
     
    /* Clear the Frame buffers */
    ds_ucsd_DownlinkPDUMgr_Free ( &(RxFrame->Data) );
    if ((NULL != RxFrameBuf) && (NULL != RxFrameBuf->pkt_ptr))
    {
      ds_ucsd_DownlinkPDUMgr_Free (&(RxFrameBuf->pkt_ptr));
      RxFrameBuf->pkt_ptr = NULL;
    }
    ds_ucsd_DownlinkPDUMgr_Free (&RxFrameBuf);
  }
}


/****************************************************************************
 * Function name:  ds_HardwareRLPTx
 * -------------
 * Description:    Send data to the ghdi.  The frame buffer DSM item is
 *                 released.
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
void ds_HardwareRLPTx (void)
{

   uint8           *radio_tx_data;
   ds_ucsd_nt_cb_type * ds_ucsd_nt_cb_info = NULL;
   ds_ucsd_frame_size_T Ds_ucsd_nt_bytes_per_sdu;
   
   /* We use a tmp variable for two reasons.  First, it is register, and
   ** thus faster.  Second, the value of the pointer will be changed if 
   ** the DS_UCSD_SET_M1_M2_BITS macro is called, so we will use the
   ** original pointer for logging and sending the data, and the tmp 
   ** pointer for getting the data
   */
   register uint8  *tmp_radio_tx_data;
   register uint32 M1,M2;

   //MSG_LOW ("In ds_HardwareRLPTx: TxFrameBuf=0x%x  rlp_tx_info_frame=%d",
        //    TxFrameBuf,TxFrameBuf.app_field & RLP_DSM_APP_FIELD_INFO_FRAME,0);

   ds_ucsd_nt_cb_info = ds_ucsd_nt_get_cb_info();
   Ds_ucsd_nt_bytes_per_sdu = ds_ucsd_nt_get_bytes_per_sdu();
   
   /* Get the buffer from L1 to add the transmit data to */
   if (!(ds_ucsd_nt_cb_info->get_ul_ota_buf_cb(&radio_tx_data)))
   {
      DATA_MSG0_ERROR("UCSD NT TX: Ds_ucsd_nt_get_ul_ota_buf_cb failed");
      return;
   }
   
   /* Set the tmp pointer */
   tmp_radio_tx_data = radio_tx_data;
   
   /* If a frame has been sent from the RLP */
   if (!rlp_tx_ready)
   {
      
      switch (Ds_ucsd_nt_bytes_per_sdu)
      {
         /* GSM 9600 case */   
         case DS_UMTS_CSD_FRAME_SIZE_240_BIT:
         
            copy_tx_data_60 (tmp_radio_tx_data, 
                             &TxFrameBuf);

            rlp_tx_ready = TRUE;            
            dispose_tx_buffer();
            break;
            
            
         
         /* GSM 14400 case */
         case DS_UMTS_CSD_FRAME_SIZE_288_BIT:
            
            /* Set the M1 flag accoding to the Tx Cycle (see 04.21 p11.2) 
            ** Also always clear the M2 flag: this is DTX (see 04.21 p11.2) 
            */
            M1 = (TxCycle) ? 0x01 : 0x00;
            M2 = 0x00;
                        
            /* Set the M1 and M2 bits */
            DS_UCSD_SET_M1_M2_BITS (tmp_radio_tx_data, M1, M2);
            
            /* Copy the 288 data bits as 8*36 bits in the radio data */
            copy_tx_data_36 (tmp_radio_tx_data,
                             &TxFrameBuf,
                             TxCycle,
                             Ds_ucsd_nt_bytes_per_sdu);
                        
            /* Set rlp_tx_ready according to TxCycle (i.e TxCycle == 1 means 
            ** the rlp_tx_cycle is TRUE because we just finished sending
            ** a frame
            */   
            rlp_tx_ready = TxCycle;
            if (TRUE == rlp_tx_ready)
            {
               dispose_tx_buffer ();
            }
            
            /* Toggle the TxCycle state */
            TxCycle ^= 0x01;
            break;
            
            
            
         /* WCDMA >= 14400 case */
         case DS_UMTS_CSD_FRAME_SIZE_576_BIT:
            
            /* Copy the 576 data bits into the radio data */
            copy_tx_data_36 (tmp_radio_tx_data,
                             &TxFrameBuf,
                             FALSE,
                             Ds_ucsd_nt_bytes_per_sdu);

            rlp_tx_ready = TRUE;            
            dispose_tx_buffer();
            break;                 
                             
         default:
         
            DATA_MSG0_ERROR("Invalid UMTS NT TX frame size");
            break;

      }

      /* Log and send the TX data */
      ds_ucsd_log_tx_data (radio_tx_data, 
                           Ds_ucsd_nt_bytes_per_sdu);
      (void)(ds_ucsd_nt_cb_info->write_ul_ota_data_cb(radio_tx_data));

   }
   /* no frame ready to be sent and not suppressing TX frames, send
   ** blank (garbage) frame 
   */  
   else if (!ds_ucsd_nt_get_suppress_duplicate_tx_frames())
   {
      /* Set all the unused bits to the fill pattern */
      memscpy (tmp_radio_tx_data, MAX_USER_BYTES,
              ds_ucsd_nt_get_Ds_FillPattern(), 
              (word)Ds_ucsd_nt_bytes_per_sdu);

      /* Log and send the TX data */
      ds_ucsd_log_tx_data (radio_tx_data, 
                           Ds_ucsd_nt_bytes_per_sdu);
      (void)(ds_ucsd_nt_cb_info->write_ul_ota_data_cb(radio_tx_data));
   }
}

/****************************************************************************
 * Function name:  ds_HardwareRLPRx
 * -------------
 * Description:    Receive data from the ghdi.  A new DMS item is allocated
 *                 for the frame buffer.
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        TRUE if an RX frame was sucessfully read, FALSE if not
 * --------
 ****************************************************************************/
boolean
ds_HardwareRLPRx (void)
{
   uint8    *radio_rx_data;
   uint32   M1; 
   boolean  rc;
   ds_ucsd_nt_cb_type *ds_ucsd_nt_cb_info = NULL;
   ds_ucsd_frame_size_T Ds_ucsd_nt_bytes_per_sdu;

   ds_ucsd_nt_cb_info = ds_ucsd_nt_get_cb_info();
   Ds_ucsd_nt_bytes_per_sdu = ds_ucsd_nt_get_bytes_per_sdu();

   //MSG_LOW ("In ds_HardwareRLPRx: RxFrameBuf=0x%x",RxFrameBuf,0,0);

   /* IF a frame hasn't already been rx'd, and one is successfully read */
   if ((!rlp_rx_frame_available) && 
        (ds_ucsd_nt_cb_info->read_dl_ota_data_cb(&radio_rx_data)))
   {
      rc = TRUE;
            
      /* Allocate new DSM item (if required) */
      if (NULL == RxFrameBuf)
      {
         RxFrameBuf = ds_ucsd_DownlinkPDUMgr_New ();
         if (NULL == RxFrameBuf )
         {
           return FALSE;
         }
         /* If PDU size exceeds single DSM item size, allocate second
          * DSM item to hopefully get consecutive memory blocks */
         if ( (uint16)Ds_ucsd_nt_bytes_per_sdu > RxFrameBuf->size )
         {
            dsm_item_type * tmp_ptr = ds_ucsd_DownlinkPDUMgr_New ();
            dsm_append(&RxFrameBuf, &tmp_ptr);
         }
      }
      
      switch (Ds_ucsd_nt_bytes_per_sdu)
      {
         
         /* GSM 9600 case */   
         case DS_UMTS_CSD_FRAME_SIZE_240_BIT:
         
            copy_rx_data_60 (radio_rx_data, &RxFrameBuf);
                             
            rlp_rx_frame_available = TRUE;            

            break;
            
        

         /* GSM 14400 case */
         case DS_UMTS_CSD_FRAME_SIZE_288_BIT:
         
            
            /* Get the M1 bit (This is Rx Cycle: See 04.21 p11.2) */
            DS_UCSD_GET_M1_M2_BITS (radio_rx_data, M1);
                        
            /* If the M1 != RxCycle, re-sync the RxCycle */
            if (M1 != RxCycle)
            {
               /* Safe to do this, even if only half a frame is received,
               ** it will fail the CRC check and do no harm */
               RxCycle = (byte) M1;
               rlp_rx_frame_available = FALSE;
            }
            else
            {
               rlp_rx_frame_available = RxCycle;
            }
            
            /* Copy data to the RxFrameBuf */
            copy_rx_data_36 (radio_rx_data, 
                             &RxFrameBuf,
                             RxCycle,
                             Ds_ucsd_nt_bytes_per_sdu);
            
            /* Toggle the expected RxCycle bit */  
            RxCycle ^= 0x01;
            break;
            
            
         /* WCDMA >= 14400 case */
         case DS_UMTS_CSD_FRAME_SIZE_576_BIT:
         
            rlp_rx_frame_available = TRUE;
            
            /* Copy data to the RxFrameBuf */
            copy_rx_data_36 (radio_rx_data, 
                             &RxFrameBuf,
                             FALSE,
                             Ds_ucsd_nt_bytes_per_sdu);
            break;
            
         default:
         
            DATA_MSG0_ERROR("Invalid UMTS NT RX frame size");
            break;
       
      }
   }
   else
   {
      rc = FALSE;
   }
   return rc;  
}


/****************************************************************************
 * Function name:  ds_HardwareTxReady 
 * -------------
 * Description:    Checks to see if the transmitter is ready to take
 * -----------     a frame.
 * Uses:
 * ----
 * Parameters:     None
 * ----------
 * Returns:        TRUE/FALSE
 * --------
 ****************************************************************************/
boolean ds_HardwareTxReady (void)
{
   return (boolean) rlp_tx_ready;
}


/****************************************************************************
 * Function name:  RLPFrameAvailable
 * -------------
 * Description:    Checks to see if a frame is available from the Hardware
 * -----------     which has been received  over the air.
 * Uses:
 * ----
 * Parameters:     None
 * ----------
 * Returns:        TRUE/FALSE
 * --------
 ****************************************************************************/
boolean ds_HardwareFrameAvailable (void)
{
   return (boolean) rlp_rx_frame_available;
}

/****************************************************************************
 * Function name:  ds_HardwareReceiveRLPFrame
 * -------------
 * Description:    Receives a Frame from the Hardware which has been 
 * -----------     received  over the air.
 * Parameters:     None
 * ----------
 * Returns:        status OK/ERROR
 * --------
 ****************************************************************************/
short ds_HardwareReceiveRLPFrame (RLP_Frame_T* Frame)
{
  short status = RLP_OK;
  register uint32 fcs;
  register uint32    i;
  register uint32 frame_size;
  ds_ucsd_frame_size_T Ds_ucsd_nt_bytes_per_sdu;

  //MSG_LOW ("In ds_HardwareReceiveRLPFrame",0,0,0);
  Ds_ucsd_nt_bytes_per_sdu = ds_ucsd_nt_get_bytes_per_sdu();

  if(Frame == NULL)
  {
    DATA_MSG0_ERROR("Null ptr to Rx RLP Frame");
    return RLP_ERROR;
  }

  /* We process whole frames (either 240 or 576 bit) here, no half frames */ 
  if (Ds_ucsd_nt_bytes_per_sdu
      == DS_UMTS_CSD_FRAME_SIZE_240_BIT)
  {
    frame_size = (uint32) DS_UMTS_CSD_FRAME_SIZE_240_BIT;
  }
  else
  {
    frame_size = (uint32) DS_UMTS_CSD_FRAME_SIZE_576_BIT;
  }

  if (rlp_rx_frame_available)
  {
    /* Use temporary buffer for byte work */
    byte  PDU[DS_UMTS_CSD_FRAME_SIZE_640_BIT];
    /* Variable to store FCS error status */
    boolean fcs_err = TRUE;
    ASSERT (NULL != RxFrameBuf);

    /* Copy data from DSM packet to temp buffer */
    DSM_EXTRACT (RxFrameBuf, 0, PDU, (uint16)frame_size); /*lint !e666*/

    /* Check the FCS */
    /* Loop through all the bytes of the frame... */
    fcs = GLLC_FCS_SEED;
    for (i=0; i < frame_size; i++)
    {
      fcs = ds_ucsd_fcs_transform[((fcs >> (24-8)) & 0xFF) ^ 
                                   DS_UCSD_REFLECT_BYTE(PDU[i])] ^ (fcs << 8);
    }

    /* Check to see if the fcs matches the secret decoder ring pattern */
    /* if then,clear FCS error flag */
    if ((fcs & 0x00FFFFFF) == RX_FCS_OK_RESULT)
    {
      fcs_err = FALSE;
    }
    /* Log the RX data */
    ds_ucsd_log_rx_data (PDU, 
                         Ds_ucsd_nt_bytes_per_sdu, fcs_err);

    /* If there is no FCS error */
    if (FALSE == fcs_err)
    {
      uint16  header_size = 0;
      /* Populate Frame structure */
      Frame->Header = 0;
      status = RLP_OK;

      /* Process Header bytes */
      /* N.B. Unnumbered frames have 2 byte header regardless of
       * RLP version.  Use Ns to get frame type assuming 2 byte header.
       */
      Frame->Header  = RxFrameBuf->data_ptr[0];
      Frame->Header |= RxFrameBuf->data_ptr[1] << 8;
      header_size = ( NS_UNNUMBEREDFRAME == GET_NS(Frame) )?
                     RLP_HDR_SIZE_VER_0_1 : get_current_rlp_header_size();

#ifdef BOA_APPS
      {
        byte garbage[4];
        switch ( header_size  )
        {
          case RLP_HDR_SIZE_VER_0_1:
            break;
          case RLP_HDR_SIZE_VER_2:
            Frame->Header |= RxFrameBuf->data_ptr[2] << 16;
            break;
          default:
            DATA_MSG1_ERROR("Bad RLP Header Size (%d)", header_size);
            break;
        }
        DSM_PULLUP (&RxFrameBuf, garbage, header_size);
      }
#else
      DSM_PULLUP (&RxFrameBuf, (byte*)&Frame->Header,
                   header_size); /*lint !e666*/
#endif

      /* Process FCS bytes */
      DSM_PULLUP_TAIL(&RxFrameBuf, Frame->FCS, RLP_FCS_SIZE); /*lint !e666*/

      Frame->Data = RxFrameBuf;

#ifdef DS_UCSD_DEBUG
         //MSG_LOW ("RLP RX Frame: Hdr=%x",Frame->Header,0,0);
         //MSG_LOW ("RLP RX Frame: FCS=%x %x %x",
              //    Frame->FCS[0],Frame->FCS[1],Frame->FCS[2]);
#endif /* DS_UCSD_DEBUG */

    }
    else /* FCS error is present */
    {
      DATA_MSG1_ERROR("RLP: RX FCS failure, fs=%d",frame_size);
      /* Flag frame was corrupt */
      status = RLP_ERROR;

      /* Purge DSM item */
      ds_ucsd_DownlinkPDUMgr_Free (&RxFrameBuf);
    }
    RxFrameBuf = NULL;
    rlp_rx_frame_available = FALSE; 
  }
  return status;
}

/****************************************************************************
 * Function name:  ds_HardwareSendRLPFrame
 * -------------
 * Description:    Sends a Frame to the Hardware to be transmitted on air.
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        status OK/ERROR
 * --------
 ****************************************************************************/
short 
ds_HardwareSendRLPFrame 
(
   RLP_Frame_T* Frame,
   uint32       rlp_hdr_size
)
{
   short status = RLP_OK;
   register uint32 i,fcs;
   register uint32 frame_size_minus_fcs;
   uint16  idata_size;
   uint16  ifull_size;
   
   /* Use temporary buffer for byte work */
   byte  PDU[DS_UMTS_CSD_FRAME_SIZE_576_BIT];

   if(Frame == NULL)
   {
     DATA_MSG0_ERROR("Null ptr to Tx RLP Frame");
     return RLP_ERROR;       
   }

   ASSERT (NULL != Frame->Data);
   
   /* We process whole frames (either 240 or 576 bit) here, no half frames */ 
   if (ds_ucsd_nt_get_bytes_per_sdu()
       == DS_UMTS_CSD_FRAME_SIZE_240_BIT)
   {
      frame_size_minus_fcs = 
                      (uint32) DS_UMTS_CSD_FRAME_SIZE_240_BIT - RLP_FCS_SIZE;
   }
   else
   {
      frame_size_minus_fcs = 
                      (uint32) DS_UMTS_CSD_FRAME_SIZE_576_BIT - RLP_FCS_SIZE;
   }
   
   /* Format the Frame structure for transmission              */
   /* The Frame structure is collapsed into a flat byte buffer */
   
   TxFrameBuf = Frame->Data;
   Frame->Data = NULL;

   /* Fill rest of information field with empty fill pattern.  Some
    * test equipment uses the pattern of the information field to
    * detect data transfer initiation. DSM item data packets are not
    * initialized by default. */
   ifull_size = (uint16)(frame_size_minus_fcs - rlp_hdr_size);
   if ( ifull_size >
       (idata_size = (uint16)dsm_length_packet(TxFrameBuf)) )
   {
     DSM_PUSHDOWN_TAIL (&TxFrameBuf, ds_ucsd_nt_get_Ds_FillPattern(),
                        (ifull_size - idata_size));
   }

   ASSERT (NULL != TxFrameBuf);
   /* Format Frame Header for byte array                       */
   /* Note the DSM item data buffer was offset to accomodate   */
   /* the Header field when DSM was initially allocated.       */

#ifdef BOA_APPS
   /* Process Header bytes */
   {
      uint8 tmp[4];
      switch ( rlp_hdr_size )
      {
        case RLP_HDR_SIZE_VER_0_1:
          tmp[0] = Frame->Header & 0xFF;
          tmp[1] = (Frame->Header >> 8) & 0xFF;
          break;
        case RLP_HDR_SIZE_VER_2:
          tmp[0] = Frame->Header & 0xFF;
          tmp[1] = (Frame->Header >> 8) & 0xFF;
          tmp[2] = (Frame->Header >> 16) & 0xFF;
          break;
        default:
          DATA_MSG1_ERROR("Bad RLP Header Size (%d)", rlp_hdr_size);
          break;
      }
   
      DSM_PUSHDOWN (&TxFrameBuf, tmp, rlp_hdr_size);  /*lint !e666*/
   }
#else
   DSM_PUSHDOWN (&TxFrameBuf, &Frame->Header, (uint16)rlp_hdr_size); 
#endif

   ASSERT (NULL != TxFrameBuf);

   /* Copy data from DSM packet to temp buffer */
   DSM_EXTRACT (TxFrameBuf, 0, PDU, (uint16)frame_size_minus_fcs); /*lint !e666*/
   
   /* Generate the FCS */
   /* Loop through all the header and user bytes of the frame.... */
   fcs = GLLC_FCS_SEED;
   for (i=0; i < frame_size_minus_fcs; i++)
   {
      fcs = ds_ucsd_fcs_transform[((fcs >> (24-8)) & 0xFF) ^ 
                                    DS_UCSD_REFLECT_BYTE(PDU[i])] ^ (fcs << 8);
   } 

   /* Now calculate the FCS */
   fcs     = ~fcs;
   Frame->FCS[2]  = DS_UCSD_REFLECT_BYTE(fcs & 0xFF);
   fcs   >>= 8;
   
   Frame->FCS[1]  = DS_UCSD_REFLECT_BYTE(fcs & 0xFF);
   fcs   >>= 8;
   
   Frame->FCS[0]  = DS_UCSD_REFLECT_BYTE(fcs & 0xFF);

   /* Put FCS at end of Frame buffer */
   DSM_PUSHDOWN_TAIL (&TxFrameBuf, Frame->FCS, RLP_FCS_SIZE);

   rlp_tx_ready = FALSE;
   TxCycle = 0;
   
   return status;
}

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
uint8 ds_Hardware_access_byte_reflection_table(uint8 index)
{
  return (Ds_ucsd_byte_reflection_table[index]);
}

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */
