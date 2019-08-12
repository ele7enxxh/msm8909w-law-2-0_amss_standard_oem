/*===========================================================================

              GSM Circuit Switched Data XXXX Module

DESCRIPTION
  This file contains the implementation of the GSM CS Transparent data 
  RA1' rate adaption functionality.

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

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdra1.c_v   1.8   02 Jul 2002 18:09:42   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdra1.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/11   ttv     Support for Q6 free floating.
06/29/09   bs      Fixed off-target lint medium & lows.
02/06/08   sa      Updated RX Frame to include FCS error.
05/30/04   ar      Lint corrections.
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_3gppi_utils.h"

#if defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)

#include "memory.h"
#include "dsgcsdi.h"
#include "dsucsdlog.h"
#include "err.h"


/*coordinate map for V.110 60-bit frame*/
/*
enum V110_frame_60_coord
{
   D1_60, D2_60, D3_60, D4_60, D5_60, D6_60, S1_60,
   D7_60, D8_60, D9_60, D10_60, D11_60, D12_60, X1_60,
   D13_60, D14_60, D15_60, D16_60, D17_60, D18_60, S3_60,
   D19_60, D20_60, D21_60, D22_60, D23_60, D24_60, S4_60,
   E4_60, E5_60, E6_60, E7_60, D25_60, D26_60, D27_60,
   D28_60, D29_60, D30_60, S6_60, D31_60, D32_60, D33_60,
   D34_60, D35_60, D36_60, X2_60, D37_60, D38_60, D39_60,
   D40_60, D41_60, D42_60, S8_60, D43_60, D44_60, D45_60,
   D46_60, D47_60, D48_60, S9_60
};
*/

// This defines the number of non-sync frames that must be
// received before we report that a non-sync frame has
// been received.
#define GCSD_TRANS_SYNC_HYTERISIS_THRESHOLD  3



#define S1_BIT          0x00000001
#define X1_BIT          0x00000002
#define S3_BIT          0x00000004
#define S4_BIT          0x00000008
#define E4_BIT          0x00000010
#define E5_BIT          0x00000020
#define E6_BIT          0x00000040
#define E7_BIT          0x00000080
#define S6_BIT          0x00000100
#define X2_BIT          0x00000200
#define S8_BIT          0x00000400
#define S9_BIT          0x00000800


#define BITS_PER_UINT32          32

// Hysterisis counter for determining when we are no
// longer receiving sync frames
LOCAL uint32 GCSD_trans_sync_hysterisis_count;



/*
 * Function name :  ds_get_rx_status_36_14400()
 * -------------
 * Description :    function to read Rx status bits from received M1 and M2 bits
 * -----------
 * Uses : 
 * ----
 * Parameters:      radio_rx_data, rx_status
 * ----------
 * Returns:         None  
 * --------
 */

LOCAL void 
ds_gcsd_get_rx_status_14400
(
   boolean     init,
   uint32      M1,
   uint32      M2,
   rx_status_T *rx_status
)
{

#define SYNC_PATTERN_M1_THRESHOLD   25

   static uint32  multiframe_cycle = 0;
   static uint32  M1_on_count = 0;
   
   /* Variables used for synchronising: */
   static byte    input_M1_pattern[5] = {0};
   static byte    sync_try_count = 0; /*i.e. no of calls to function*/
   static boolean synchronised = FALSE;
   
   /*The last five 5 bytes of the following construct are simply the first 
    *five repeated, for ease of testing in the synchronisation code*/
   static const byte multiframe_pattern[36] = {0, 0, 0, 0,
                                               1, 0, 0, 1,  
                                               0, 1, 1, 0,
                                               0, 1, 1, 1,
                                               1, 1, 0, 0,
                                               0, 1, 1, 0,
                                               1, 1, 1, 0,
                                               1, 0, 1, 0,
                                               0, 0, 0, 1};
   /*Variables used once synchronised: */
   static byte    SB_history[2] = {0};
   static byte    X_history[2] = {0};
   static byte    SB_history_index = {0};
   static byte    X_history_index = {0};
   static byte    remembered_SB_bit = {0};
   static byte    remembered_X_bit = {0};

   uint32         pattern_index = 0;  /*for parsing multiframe_pattern*/
   

   if(init)
   {
      multiframe_cycle = 0;
      sync_try_count = 0;
      synchronised = FALSE;
      SB_history[0] = V24_CONTROL_OFF;
      SB_history[1] = V24_CONTROL_OFF;
      X_history[0] = V24_CONTROL_OFF;
      X_history[1] = V24_CONTROL_OFF;
      SB_history_index = 0;
      X_history_index = 0;
      remembered_SB_bit = V24_CONTROL_OFF;
      remembered_X_bit = V24_CONTROL_OFF;
      M1_on_count      = 0;
   }
   else
   {
      // Initialize the output status bits to the saved ones.
      rx_status[0].X1_status_rx = remembered_X_bit;
      rx_status[0].X2_status_rx = remembered_X_bit;
      rx_status[0].S4_status_rx = remembered_SB_bit;
      rx_status[0].S9_status_rx = remembered_SB_bit;
            
      if (synchronised)
      {
         
         /* Although we get to this point because we synchronised last time,
          * we may have just received a dud value for M1. So we test to see that
          * M1 is what we are expecting it to be and if not, we must
          * re-synchronise */
         if (multiframe_pattern[multiframe_cycle] == (byte) M1)
         {
            if (multiframe_cycle == 3 ||
                multiframe_cycle == 4 ||
                multiframe_cycle == 10 ||
                multiframe_cycle == 11 ||
                multiframe_cycle == 15 ||
                multiframe_cycle == 16 ||
                multiframe_cycle == 22 ||
                multiframe_cycle == 23 ||
                multiframe_cycle == 29 ||
                multiframe_cycle == 30 )
            {
               
               /* get SB status (DCD)*/
               /*2 byte array to hold current M2/SB bit and previous M2/SB bit*/
               SB_history[SB_history_index] = (byte) M2;
               SB_history_index ^= 0x01;
               
               
               /* If current M2/SB is the same as previous M2/SB, then 2 
                * consecutive 1's or 0's have been recieved. We therefore 
                * set rx_status to the same value as the current bit. */
               if ((SB_history[0] == SB_history[1]) && (remembered_SB_bit != (byte) M2))
               {
                  rx_status[0].S4_status_rx = (byte) M2;
                  rx_status[0].S9_status_rx = (byte) M2;
                  
                  remembered_SB_bit = (byte) M2;
                  
                  DS_UCSD_DEBUG_MSG_1 ("T-14.4: RX SB bit = %d",M2);
               }
               
            }
            else if (multiframe_cycle == 5 ||
                     multiframe_cycle == 9 ||
                     multiframe_cycle == 17 ||
                     multiframe_cycle == 21 )
            {
               
               /*get X status (CTS)*/
               /* 2 byte array to hold current M2/X bit and previous M2/X bit */
               X_history[X_history_index] = (byte) M2;
               X_history_index ^= 0x01;
               
               
               if ((X_history[0] == X_history[1]) && (remembered_X_bit != (byte)M2))
               {
                  rx_status[0].X1_status_rx = (byte) M2;
                  rx_status[0].X2_status_rx = (byte) M2;
                  remembered_X_bit = (byte) M2;
                  DS_UCSD_DEBUG_MSG_1 ("T-14.4: RX X bit = %d",M2);
               }
               
            }
         }
         else /* Lost synchronisation, try again */
         {
            DS_UCSD_DEBUG_MSG ("T-14.4: Lost RX sync");
            sync_try_count = 0;
            synchronised = FALSE;
         }
         
         
         multiframe_cycle++;
         
         if (multiframe_cycle > 30)
         {
            multiframe_cycle = 0;
         }
      }
      else /* Synchronise */
      {
         input_M1_pattern[sync_try_count] = (byte) M1;
         
         if (M1)
         {
            if (M1_on_count < SYNC_PATTERN_M1_THRESHOLD)
            {
               M1_on_count++;
               if (M1_on_count == SYNC_PATTERN_M1_THRESHOLD)
               {
                  DS_UCSD_DEBUG_MSG ("T-14.4: M1 cnt hit SYNC frame threshold");
               }
            }
         }
         else
         {
            M1_on_count = 0;
         }
         
         
         // If we seem to be gettng sync frames, turn DCD and CTS off
         if (M1_on_count == SYNC_PATTERN_M1_THRESHOLD)
         {
            rx_status[0].X1_status_rx = V24_CONTROL_OFF;
            rx_status[0].X2_status_rx = V24_CONTROL_OFF;
            rx_status[0].S4_status_rx = V24_CONTROL_OFF;
            rx_status[0].S9_status_rx = V24_CONTROL_OFF;
         }
         
         /*We will only have enough bytes to test against once func has 
          *been called 5 times (NB: First call, sync_try_count is 0)*/
         else if (sync_try_count == 4)
         {
            
            /*Locate 5 bit pattern in multiframe_pattern*/
            for (pattern_index = 0; pattern_index < 30; pattern_index++)
            {
               if (multiframe_pattern[pattern_index] == input_M1_pattern[0] &&
                   multiframe_pattern[pattern_index+1] == input_M1_pattern[1] &&
                   multiframe_pattern[pattern_index+2] == input_M1_pattern[2] &&
                   multiframe_pattern[pattern_index+3] == input_M1_pattern[3] &&
                   multiframe_pattern[pattern_index+4] == input_M1_pattern[4] )
               {
                  DS_UCSD_DEBUG_MSG ("T-14.4: Acquired RX sync");
                  synchronised = TRUE;
                  sync_try_count = 0;
                  
                  /*Set multiframe value ready for next call to to function.
                   *when synchronised will be true*/
                  if (pattern_index >= 26)
                  {
                     /*We cannot set multiframe_cycle to anything above 30
                      *so if pattern_index + 5 will be > 30, we adjust it to
                      *loop round and become one of the first 5 frames*/
                     multiframe_cycle = (byte) (pattern_index - 26);
                  }
                  else
                  {
                     multiframe_cycle = (byte) (pattern_index + 5);
                  }
                  break;
               }
            }
            
            /* If we cant sync, (e.g. because the 5 received bits do not match a
             * pattern within multiframe pattern,) then discard the values 
             * and try again */
            if (!synchronised)
            {
               sync_try_count = 0;
            }
         }
         else
         {
            /* not enough bytes received yet to calculate where we are
             * in multiframe_pattern */
            sync_try_count++;
         }
      }
               
      /* SA status (DSR) is not sent in 14400 frames, set to 0 */
      rx_status[0].S1_status_rx = 0;
      rx_status[0].S3_status_rx = 0;
      rx_status[0].S6_status_rx = 0;
      rx_status[0].S8_status_rx = 0;
      
      /*ignore E bits for transparent async data (no NIC)
       * but store them anyway*/
      
      rx_status[0].E4_status_rx = 1;
      rx_status[0].E5_status_rx = 1;
      rx_status[0].E6_status_rx = 1;
      rx_status[0].E7_status_rx = 1;
   }
}


/*
 * Function name :  ds_copy_rx_data_36_14400()
 * -------------
 * 
 * Description :    function to copy user data directly from radio_rx_data
 * -----------      to sync_rx_data
 *
 * Uses : 
 * ----
 *
 * Parameters:      radio_rx_data, sync_rx_data
 * ----------
 *
 * Returns:         None
 * --------
 */
static void ds_gcsd_get_rx_data_14400
(
   uint8 *radio_rx_data,
   sync_rx_data_T    *sync_rx_data,
   rx_status_T       *rx_status
)

{
   register uint32   tmp_radio_rx_data;
   register uint32   sync_shift = 0;
   register uint32   tmp_sync_rx_data = 0;
   register uint8    *tmp_radio_rx_data_ptr;
   register uint32   counter;
      
   uint32            M1=0, M2=0;
   
      

   /* Get M1 and M2 bits... */
   DS_UCSD_GET_M1_M2_BITS (radio_rx_data, M1);
   
   // Set tmp_radio_rx_data_ptr after DS_UCSD_GET_M1_M2_BITS() because 
   // the macro changes the pointer value
   tmp_radio_rx_data_ptr = radio_rx_data;
   
      
   for (counter = 0; counter < BYTES_PER_14400_20_MSEC; counter++)
   {
      tmp_radio_rx_data = *tmp_radio_rx_data_ptr++;
      tmp_sync_rx_data |= tmp_radio_rx_data << sync_shift;
      sync_shift += 8;
      
      if (sync_shift > 24)
      {
         *sync_rx_data++ = tmp_sync_rx_data;
         sync_shift = 0;
         tmp_sync_rx_data = 0;
      }
      
   }     
   /* Now get status bits */
   ds_gcsd_get_rx_status_14400 (FALSE, M1, M2, rx_status);
}

 

/*===========================================================================

FUNCTION  ds_gcsd_get_rx_status_60

DESCRIPTION
   Fills in the rx_status array based on input status bits   
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_gcsd_get_rx_status_60
(
   uint32         rx_status_data,
   rx_status_T    *rx_status
)

{
#ifdef DS_UCSD_DEBUG
   typedef struct
   {
      uint32 SB1;
      uint32 SB2;
      uint32 X1;
      uint32 X2;
   }Status_T;

   static Status_T Saved_status = {100,100,100,100};
   Status_T curr_status;
#endif

   register uint32 tmp_rx_status_data = rx_status_data;   
   register int frame;
   //register int frame_offset;
   
   for (frame = 0; frame < 4; frame++)
   {
      
      //frame_offset = frame * NUM_V110_STATUS_BITS_60;
      
      /*get SA status (DSR)*/
      rx_status[frame].S1_status_rx = (tmp_rx_status_data & S1_BIT) ? TRUE : FALSE;
      rx_status[frame].S3_status_rx = (tmp_rx_status_data & S3_BIT) ? TRUE : FALSE;
      rx_status[frame].S6_status_rx = (tmp_rx_status_data & S6_BIT) ? TRUE : FALSE;
      rx_status[frame].S8_status_rx = (tmp_rx_status_data & S8_BIT) ? TRUE : FALSE;
    
      /*get SB status (DCD)*/

      rx_status[frame].S4_status_rx = (tmp_rx_status_data & S4_BIT) ? TRUE : FALSE;
      rx_status[frame].S9_status_rx = (tmp_rx_status_data & S9_BIT) ? TRUE : FALSE;
      
      /*get X status (CTS)*/

      rx_status[frame].X1_status_rx = (tmp_rx_status_data & X1_BIT) ? TRUE : FALSE;
      rx_status[frame].X2_status_rx = (tmp_rx_status_data & X2_BIT) ? TRUE : FALSE;

#ifdef DS_UCSD_DEBUG
      curr_status.SB1 = rx_status[frame].S4_status_rx;
      curr_status.SB2 = rx_status[frame].S9_status_rx;
      curr_status.X1  = rx_status[frame].X1_status_rx;
      curr_status.X2  = rx_status[frame].X2_status_rx;
#endif

      /*ignore E bits for transparent async data (no NIC)
       * but store them anyway*/

      rx_status[frame].E4_status_rx = (tmp_rx_status_data & E4_BIT) ? TRUE : FALSE;
      rx_status[frame].E5_status_rx = (tmp_rx_status_data & E5_BIT) ? TRUE : FALSE;
      rx_status[frame].E6_status_rx = (tmp_rx_status_data & E6_BIT) ? TRUE : FALSE;
      rx_status[frame].E7_status_rx = (tmp_rx_status_data & E7_BIT) ? TRUE : FALSE;

#ifdef DS_UCSD_DEBUG
      if (memcmp(&Saved_status, &curr_status, sizeof (Status_T) != 0))
      {
         DS_UCSD_DEBUG_MSG_2("Rx status chg oldSB1=%d,SB1=%d",Saved_status.SB1,curr_status.SB1);
         DS_UCSD_DEBUG_MSG_2("oldSB12=%d,SB2=%d",Saved_status.SB2,curr_status.SB2);
         DS_UCSD_DEBUG_MSG_2("oldX1=%d,X1=%d",Saved_status.X1,curr_status.X1);
         DS_UCSD_DEBUG_MSG_2("oldX2=%d,X2=%d",Saved_status.X2,curr_status.X2);
         Saved_status = curr_status;
      }
#endif


   }

}


/*===========================================================================

FUNCTION  ds_gcsd_set_tx_data_9600

DESCRIPTION
   Retrieves the user and status data from V.110 frame format byte array
   passed in via the radio_rx_data   
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void
ds_gcsd_get_rx_data_9600 
(
   uint8             *radio_rx_data,   // OTA byte array
   uint32            *sync_rx_data,    // User data bits
   rx_status_T       *rx_status        // Status data
)
{
// Macro to get data from radio_rx_data and store in a uint32
#define GET_RADIO_RX_DATA(x) x = 0;                       \
                             x = *radio_rx_data++;        \
                             x |= *radio_rx_data++ << 8;  \
                             x |= *radio_rx_data++ << 16; \
                             x |= *radio_rx_data++ << 24
   
   // Local variables                         
   register uint32   tmp_sync_rx_data;
   register uint32   tmp_sync_rx_data_cnt;
   register uint32   tmp_radio_rx_data;
   register uint32   tmp_radio_rx_data_cnt;
   register uint32   tmp_data;
   register uint32   tmp_data_size;
   register uint16   i,j;
   register uint32   rx_status_bits;
   register uint32   rx_status_cnt;
   
   // Initialize locals
   tmp_radio_rx_data_cnt = 0;
   rx_status_bits        = 0;
   tmp_sync_rx_data      = 0;
   tmp_sync_rx_data_cnt  = 0;
   tmp_data              = 0;
   rx_status_cnt         = 0;
   
   
   // Read in radio_rx_data into tmp_radio_rx_data
   GET_RADIO_RX_DATA(tmp_radio_rx_data);
   
   // Loop for 4 60-bit frames
   for (i = 0; i < 4; i++)
   {
      /* For each frame, we want to loop 16 times.  The frame consists of 
      ** alternating data and status bits.  Each set of data bits is always
      ** 6 bits.  Status bits are 1 bit, with one case being 5 bits.  The 
      ** pattern is as follows:
      **
      **  3 * (6 data, 1 status)
      **  1 * (6 data, 5 status)
      **  4 * (6 data, 1 status)
      **
      **  for a total of 48 data bits and 12 status bits for each frame
      */ 
      for (j = 0; j < 16; j++)
      {
         // Get the number of bits that we need from radio_rx_data..
         
         // Status bit(s) size 
         if (j & 0x0001)
         {
            tmp_data_size = (j == 7) ? 5 : 1;
         }
         
         // Data bits size
         else
         { 
            tmp_data_size = 6;
         }
         
         // Store the requested number of bits in tmp_data
         tmp_data = tmp_radio_rx_data >> tmp_radio_rx_data_cnt;
         
         // Increment the radio_rx_data counter
         tmp_radio_rx_data_cnt += tmp_data_size;
         
         // If we needed more data than was available in tmp_radio_rx_data,
         // get more rx data and store in missing bits in tmp_data
         if (tmp_radio_rx_data_cnt >= BITS_PER_UINT32)
         {
            GET_RADIO_RX_DATA(tmp_radio_rx_data);
            tmp_radio_rx_data_cnt -= BITS_PER_UINT32;
            tmp_data |= tmp_radio_rx_data << (tmp_data_size - tmp_radio_rx_data_cnt);
         }
         
         // Now store the tmp_data in either the status or user data
         
         // Status bit
         if (j & 0x0001)
         {
            // Only store status data for last frame... we ignore the other
            // as it is not important.
            if (i == 3)
            {
               // mask off unused bits of tmp_data
               tmp_data &= (j == 7) ? 0x0000001F : 0x00000001;               
               rx_status_bits |= tmp_data << rx_status_cnt;
               rx_status_cnt += tmp_data_size;
            }
         }
         
         // Store in user data
         else
         {  
            tmp_data &= 0x0000003F;
            tmp_sync_rx_data |= tmp_data << tmp_sync_rx_data_cnt;
            tmp_sync_rx_data_cnt += 6;
            
            // If we hit uint32 boundary of user data, store data, and
            // save unsaved bits.
            if (tmp_sync_rx_data_cnt >= BITS_PER_UINT32)
            {
               *sync_rx_data++ = tmp_sync_rx_data;
               tmp_sync_rx_data_cnt -= BITS_PER_UINT32;
               tmp_sync_rx_data = tmp_data >> (6 - tmp_sync_rx_data_cnt);
            }
         }
      }
   }
   
   /* Now put the status data into the rx_status structure */  
   ds_gcsd_get_rx_status_60 (rx_status_bits, rx_status);
   
   // Store last accumulated data
   *sync_rx_data = tmp_sync_rx_data;
}


/***************************************************************
 * Function name :  ds_init_RA1d_adaption_rx
 * -------------
 *
 * Description :     
 * -----------       
 *
 * Parameters:   none
 * ----------
 * Returns:      None
 * --------
 ***************************************************************/
void
ds_gcsd_init_RA1d_adaption_rx (void)
{

   GCSD_trans_sync_hysterisis_count = 0;
   ds_gcsd_get_rx_status_14400 (TRUE, 0,0, NULL);

}



/*
 * Function name :  ds_RA1d_adaption_rx()
 * -------------
 *
 * Description :    function to perform RA1' rate adaption for Rx data
 * -----------
 *
 * Uses : 
 * ----
 * adapt V110 data frames from network, to appropriate synchronous
 * data stream for RA0
 * see  ETS 300 562 (GSM 04.21) ed3 sect. 8, Figs 5,6,7,8,9
 * ignore all E bits for async transparent data
 * there are always 12 status bits per V110 frame,
 * regardless of data rate
 *
 *
 * Parameters:  
 * ----------
 * current_call_mode
 * e_receive_sync_pattern
 *
 * Returns:    
 * --------
 * sync_rx_data
 * c_sync_pattern_received
 * rx_status
 */


boolean 
ds_gcsd_RA1d_adaption_rx
(
   boolean         e_receive_sync_pattern,
   sync_rx_data_T  *sync_rx_data,
   rx_status_T     *rx_status_bits,
   boolean         *rx_error
)
{
   /*locals*/
   ds_ucsd_data_rate_T           Gcsd_curr_data_rate;
   register boolean              tmp_rx_error = FALSE;
   register ds_ucsd_frame_size_T frame_size;
   register int                  i;
            boolean              c_sync_pattern_received = FALSE;
            uint8                *radio_rx_data;
            uint32               M1=0, M2=0;

   Gcsd_curr_data_rate = ds_gcsd_get_Gcsd_curr_data_rate();
   /*read all data bits from channel decoder */
  
      
   if (!(ds_gcsd_read_from_channel_decoder(&radio_rx_data)))
   {
      tmp_rx_error = TRUE;
   }

   // Handle case where we have an error.
   *rx_error = tmp_rx_error;
   if (tmp_rx_error)
   {
      return FALSE;
   }
   
   /* Determine the frame size for logging */
   if (Gcsd_curr_data_rate == DS_UCSD_RATE_14400)
   {
      frame_size = DS_UMTS_CSD_FRAME_SIZE_288_BIT;
   }
   else
   {
      frame_size = DS_UMTS_CSD_FRAME_SIZE_240_BIT;
   }
         
   /* Log the received data */
   ds_ucsd_log_rx_data (radio_rx_data, frame_size, FALSE);   

   /*check for reception of 1/OFF synchronisation bit pattern*/
   if (e_receive_sync_pattern)
   {
   
      /* Initialize c_sync_pattern to TRUE */
      c_sync_pattern_received = TRUE;
      
      /*check for 1/OFF sync pattern (all 1s), ignoring E bits*/

      switch (Gcsd_curr_data_rate)
      {
         case DS_UCSD_RATE_14400:
            
            /* Need to get M1 and M2 and determine 
            ** multi-frame syncronization 
            */
            DS_UCSD_GET_M1_M2_BITS (radio_rx_data, M1);
            
            ds_gcsd_get_rx_status_14400 (FALSE,M1,M2,rx_status_bits);
            
            
            /* Now check all data portion bytes of radio_rx_data and if they aren't
            ** all 1's we don't have the sync pattern
            */
            for (i = 0; i < BYTES_PER_14400_20_MSEC; i++)
            {
               if (radio_rx_data[i] != 0xFF)
               {
                  c_sync_pattern_received = FALSE;
                  break;
               }
            }
            break;         
            
         case DS_UCSD_RATE_9600:
         
            /* Log the received data */
            ds_ucsd_log_rx_data (radio_rx_data, DS_UMTS_CSD_FRAME_SIZE_240_BIT, FALSE);
            
            /*if all 56 bits other than E4 to E7 = 1 for 4 (best number TBD)
             *consecutive V110 frames, then must be 1/OFF sync pattern*/
             
            /* Turn E4 - E7 bits on in all 4 frames.  These are to be
            ** ignored in determining if we have a sync frame or not,
            ** so turning them on will make the test more efficient.
            */
            
            /* Bits 28 - 31 are E4 - E7 in first frame */
            /* radio_rx_data [3] |= 0xF0; */
            
            /* Bits 88 - 91 are E4 - D7 in second frame */
           /* radio_rx_data [11] |= 0x0F; */
            
             /* Bits 148 - 151 are E4 - E7 in third frame */
           /* radio_rx_data [18] |= 0xF0; */
            
            /* Bits 208 - 211 are E4 - D7 in fourth frame */
           /* radio_rx_data [26] |= 0x0F; */
            
            /* Now check for all 1's sync pattern.... */
            for (i = 0; i < BYTES_PER_9600_20_MSEC; i++)
            {
               if (radio_rx_data[i] != 0xFF)
               {
                  c_sync_pattern_received = FALSE;
                  break;
               }
            }
            break;             
  
         
         default:
            DS_3GPP_MSG1_ERROR ("GCSD RA1'RX: Unsupported user rate = %d",(int)Gcsd_curr_data_rate);
            break;
      }
      /* Use hysterisis for determining when sync pattern is no
      ** longer present.  This will help with the case when a 
      ** few bit errors could falsely report that we are no
      ** longer receiving sync frames
      */ 
      if (!c_sync_pattern_received)
      {
         // If we haven't gotten more than or equal to the threshold number of non-sync
         // frames, report the frame as a sync frame.
         if (++GCSD_trans_sync_hysterisis_count < GCSD_TRANS_SYNC_HYTERISIS_THRESHOLD)
         {
            c_sync_pattern_received = TRUE;
         }
      }
      else
      {
         GCSD_trans_sync_hysterisis_count = 0;
      }
          
   }

   else
   {
      switch (Gcsd_curr_data_rate)
      {         
         case DS_UCSD_RATE_9600:
            /*received at 12000 bit/s in 60-bit frames
             *copy all data bits from 4 consecutive frames*/

            
            /* split up user data and status bits from received radio data
            ** (4 X 60 bit V110 frames)
            */

            ds_gcsd_get_rx_data_9600 (radio_rx_data,
                                      sync_rx_data,
                                      rx_status_bits);
                              
            break;
            
         case DS_UCSD_RATE_14400:

            ds_gcsd_get_rx_data_14400 (radio_rx_data,
                                       sync_rx_data,
                                       rx_status_bits);

            break;
            

         default:
            DS_3GPP_MSG1_ERROR ("GCSD RA1'RX: Unsupported user rate = %d",
                                 (int)Gcsd_curr_data_rate);
            break;

      }
   }

   return c_sync_pattern_received;
}

/***************************************************************
 * Function name :  ds_set_status_36_14400()
 * -------------
 *
 * Description :     Sets up the M1 and M2 status bits for 14400
 * -----------       transparent Multiframe
 *
 * Parameters:   init, radio_tx_data
 * ----------
 * Returns:      None
 * --------
 ***************************************************************/
LOCAL void 
ds_gcsd_set_tx_status_14400 
(
   boolean     init, 
   tx_status_T tx_status,
   uint32      *M1,
   uint32      *M2
)
{
   static byte multiframe_cycle = 0;

   if (init)
   {
      multiframe_cycle = 0;
   }
   else
   {
   
      switch (multiframe_cycle)
      {
         /*load status registers with values outlined in GSM spec
          *GSM 04.21 version 7.03 section 8.1.1.1*/
          
         /* TODO:  Why are we not adding the X status bit in M2 of
         ** multiframes 5,9,17 and 21????   Also, the M2 for the
         ** NIC in multiframes 24-28 should be set to 1, not 0...
         */

         case 0: *M1=0; *M2=0; break;
         case 1: *M1=0; *M2=0; break;
         case 2: *M1=0; *M2=0; break;
         case 3: *M1=0; *M2=tx_status.SB_status_tx; break;
         case 4: *M1=1; *M2=tx_status.SB_status_tx; break;
         case 5: *M1=0; *M2=0; break;
         case 6: *M1=0; *M2=0; break;
         case 7: *M1=1; *M2=0; break;
         case 8: *M1=0; *M2=0; break;
         case 9: *M1=1; *M2=0; break;
         case 10: *M1=1; *M2=tx_status.SB_status_tx; break;
         case 11: *M1=0; *M2=tx_status.SB_status_tx; break;
         case 12: *M1=0; *M2=0; break;
         case 13: *M1=1; *M2=0; break;
         case 14: *M1=1; *M2=0; break;
         case 15: *M1=1; *M2=tx_status.SB_status_tx; break;
         case 16: *M1=1; *M2=tx_status.SB_status_tx; break;
         case 17: *M1=1; *M2=0; break;
         case 18: *M1=0; *M2=0; break;
         case 19: *M1=0; *M2=0; break;
         case 20: *M1=0; *M2=0; break;
         case 21: *M1=1; *M2=0; break;
         case 22: *M1=1; *M2=tx_status.SB_status_tx; break;
         case 23: *M1=0; *M2=tx_status.SB_status_tx; break;
         case 24: *M1=1; *M2=1; break;
         case 25: *M1=1; *M2=1; break;
         case 26: *M1=1; *M2=1; break;
         case 27: *M1=0; *M2=1; break;
         case 28: *M1=1; *M2=1; break;
         case 29: *M1=0; *M2=tx_status.SB_status_tx; break;
         case 30: *M1=1; *M2=tx_status.SB_status_tx; break;
	
         default: break;  /* make lint happy */
      }
      
      /* TODO:  Double check the 30 value */
      if (++multiframe_cycle > 30)
      {
         multiframe_cycle = 0;
      }
      
   }
}

/***************************************************************
 * Function name :  ds_copy_data_36_14400()
 * -------------
 *
 * Description :     Copies from sync_tx_data to radio_tx_data directly 
 * -----------       for 14400 bd
 *
 * Parameters:   sync_tx_data, radio_tx_data
 * ----------
 * Returns:      None
 * --------
 ***************************************************************/
static void ds_gcsd_set_tx_data_14400
(
   sync_tx_data_T      *sync_tx_data,
   tx_status_T tx_status,
   byte        *radio_tx_data
)

{
   register uint32   counter;
   register uint32   tmp_sync_tx_data;
   register uint32   sync_shift;
   register uint8    *tmp_radio_tx_data_ptr;
   
   uint32            M1,M2;

   /* Get M1 and M2 bits */
   ds_gcsd_set_tx_status_14400 (FALSE, tx_status, &M1, &M2);
   
   /* Set the M1 and M2 bits in the outgoing radio_tx_data */
   DS_UCSD_SET_M1_M2_BITS (radio_tx_data, M1, M2);

  
   /* If sync_tx_data is NULL, set data portion of radio_tx_data
   ** to all 1's
   */
   if (sync_tx_data == NULL)
   {
      for (counter = 0;
           counter < BYTES_PER_14400_20_MSEC;
           counter++)
      {
         *radio_tx_data = 0xFF;
         radio_tx_data++;
      }
   }
   
   /* If sync_tx_data is not NULL, we have actual user data in the form of
   ** an array, one byte per bit of tx data.  Fill in the radio_tx_data
   ** with each bit of user data
   */
   else
   {
      sync_shift = 0;
      
      tmp_sync_tx_data = *sync_tx_data++;
      
      tmp_radio_tx_data_ptr = radio_tx_data;
      
      
      for (counter = 0; counter < BYTES_PER_14400_20_MSEC; counter++)
      {
         if (sync_shift > 24)
         {
            tmp_sync_tx_data = *sync_tx_data++;
            sync_shift = 0;
         }
         
         *tmp_radio_tx_data_ptr++ = (uint8) (tmp_sync_tx_data >> sync_shift);
         sync_shift += 8;
      }      
   }/* else */
}



 
/*
 * Function name :  ds_set_status_60()
 * -------------
 *
 * Description :     sets status bits for RA1' rate adaption
 * -----------       using 60-bit V.110 frames
 *
 * Uses :
 * ----   
 *
 * Parameters:  
 * ----------
 * 
 * tx_status
 * V110_frame_60
 *
 * Returns:    
 * --------
 * none
 */
/*===========================================================================

FUNCTION  ds_gcsd_set_tx_status_60

DESCRIPTION
   Returns 12 status bits based on the input tx_status data.  
         
DEPENDENCIES
  None
  
RETURN VALUE
  uint32 - the 12 least significant bits of which represent the
           current status bits.
    
SIDE EFFECTS
  None

===========================================================================*/
uint32 ds_gcsd_set_tx_status_60
(
   tx_status_T tx_status
)
{
    // Always set E4, E5, E6 and E7
   uint32 status = E4_BIT | E5_BIT | E6_BIT | E7_BIT;  
      
   
   if (tx_status.SA_status_tx)
   {
      // if DTR status is set, set S1, S3, S6 and S8
      status |= S1_BIT | S3_BIT | S6_BIT | S8_BIT;;    
   }
   
   if (tx_status.SB_status_tx)
   {
      // if RTS status is set, set S4 and S9
      status |= S4_BIT | S9_BIT; 
   }
   
   return status;
}


/*===========================================================================

FUNCTION  ds_gcsd_set_tx_data_9600

DESCRIPTION
   Formats the user and status data into V.110 frame byte array format data 
   to be written to L1.
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void
ds_gcsd_set_tx_data_9600 
(
   uint32            *sync_tx_data,
   tx_status_T       tx_status,
   uint8             *radio_tx_data
)
{
   register uint32   tmp_sync_tx_data;
   register uint32   sync_tx_data_cnt;
   register uint32   tmp_radio_tx_data;
   register uint32   radio_tx_data_cnt;
   register uint32   tmp_data;
   register uint32   tmp_data_size;
   register uint16   i,j;
   register uint32   tx_status_bits;
   register uint32   tx_status_cnt;
   
   // Initialize locals
   tmp_radio_tx_data = 0;
   radio_tx_data_cnt = 0;
   sync_tx_data_cnt  = 0;
   
   
   // If sync_tx_data is NULL, send sync frame
   if (sync_tx_data == NULL)
   {
      for (i = 0;
           i < BYTES_PER_9600_20_MSEC;
           i++)
      {
         *radio_tx_data = 0xFF;
         radio_tx_data++;
      }
   }
   
   // Otherwise, send real data
   else
   {
      // Initialize tmp_sync_tx_data
      tmp_sync_tx_data  = *sync_tx_data++;
      
      // Get the status bits...
      tx_status_bits = ds_gcsd_set_tx_status_60 (tx_status);

      // Loop for 4 60-bit frames
      for (i = 0; i < 4; i++)
      {
         // Reset the status bit counter
         tx_status_cnt = 0;
         
         /* For each frame, we want to loop 16 times.  The frame consists of 
         ** alternating data and status bits.  Each set of data bits is always
         ** 6 bits.  Status bits are 1 bit, with one case being 5 bits.  The 
         ** pattern is as follows:
         **
         **  3 * (6 data, 1 status)
         **  1 * (6 data, 5 status)
         **  4 * (6 data, 1 status)
         **
         **  for a total of 48 data bits and 12 status bits for each frame
         */
         for (j = 0; j < 16; j++)
         {
            // Odd number loops are status bits 
            if (j & 0x0001)
            {
               // Get status bits from tx_status_bits
               // and make status bits the least significant of tmp_data
               tmp_data = (tx_status_bits >> tx_status_cnt);
               
               // 5 status bits case...
               if (j == 7)
               {
                  // remove non status bits from tmp_data
                  tmp_data &= 0x0000001F;
                  tmp_data_size = 5;
                  tx_status_cnt += 5;
               }
               else
               {
                  // remove non status bits from tmp_data
                  tmp_data &= 0x00000001;
                  tmp_data_size = 1;
                  tx_status_cnt++;
               }
            }
            
            // Data bits... get them from tmp_sync_tx_data and put them
            // into tmp_data 
            else
            {
               tmp_data = tmp_sync_tx_data >> sync_tx_data_cnt;
               sync_tx_data_cnt += 6;
               
               // If we cross a uint32 boundary of sync_tx_data, read
               // in next one and get data from it.
               if (sync_tx_data_cnt >= BITS_PER_UINT32)
               {
                  tmp_sync_tx_data = *sync_tx_data++;
                  sync_tx_data_cnt -= BITS_PER_UINT32;
                  tmp_data |= tmp_sync_tx_data << (6 - sync_tx_data_cnt); 
               }
               tmp_data &= 0x3F;
               tmp_data_size = 6;
            }
            
            // Now store the data in tmp_data into the radio_tx_data
            tmp_radio_tx_data |= (tmp_data << radio_tx_data_cnt);
            radio_tx_data_cnt += tmp_data_size;
            
            // If we hit a radio_tx_data uint32 boundary, write out
            // the stored data and store the remaining, unstored data.
            if (radio_tx_data_cnt >= BITS_PER_UINT32)
            {
               *radio_tx_data++ = (uint8) tmp_radio_tx_data;
               *radio_tx_data++ = (uint8)(tmp_radio_tx_data >> 8);
               *radio_tx_data++ = (uint8)(tmp_radio_tx_data >> 16);
               *radio_tx_data++ = (uint8)(tmp_radio_tx_data >> 24);
               radio_tx_data_cnt -= BITS_PER_UINT32;
               tmp_radio_tx_data = tmp_data >> (tmp_data_size - radio_tx_data_cnt);
            }
         }
      }
      
      // There will be 16 bits of data that had not yet been written to the
      // radio_tx_data yet 
      *radio_tx_data++ = (uint8)tmp_radio_tx_data;
      *radio_tx_data++ = (uint8)(tmp_radio_tx_data >> 8);
   }
}      

/***************************************************************
 * Function name :  ds_init_RA1d_adaption_tx
 * -------------
 *
 * Description :     
 * -----------       
 *
 * Parameters:   none
 * ----------
 * Returns:      None
 * --------
 ***************************************************************/
void
ds_gcsd_init_RA1d_adaption_tx (void)
{
   tx_status_T         tx_status = {1,1};
      
   ds_gcsd_set_tx_status_14400 (TRUE, tx_status, NULL, NULL);
}



/*
 * Function name :  ds_RA1d_adaption_tx()
 * -------------
 *
 * Description :     RA1' rate adaption function
 * -----------
 *
 * Uses : adapts synchronous Tx data to V.110 frame format (apart from
 * ----   14400 whch is sent directly) to be transmitted to network
 *
 * Parameters:  
 * ----------
 * sync_tx_data
 * tx_status
 * e_send_sync_pattern
 *
 * Returns:    
 * --------
 * none
 */
void
ds_gcsd_RA1d_adaption_tx
(
   uint32        *sync_tx_data,
   tx_status_T   tx_status,
   boolean       e_send_sync_pattern,
   uint8         *radio_tx_data
)
{
   register ds_ucsd_frame_size_T frame_size;
   ds_ucsd_data_rate_T           Gcsd_curr_data_rate;
   Gcsd_curr_data_rate = ds_gcsd_get_Gcsd_curr_data_rate();
   /*adapt synchronous output of RA0 to radio data frames
    *of either 14.5kbit/s, 12k bit/s, 6k bit/s or 3.6 kbit/s
    *see GSM 04.21 Figs. 5 thru 17*/
   if (e_send_sync_pattern)
   {
      /*send 1/OFF sync pattern (all 1s) to network*/
      switch (Gcsd_curr_data_rate)
      {
         case DS_UCSD_RATE_14400:
         
            frame_size = DS_UMTS_CSD_FRAME_SIZE_288_BIT;
            /*send 9 36-bit frames of all 1s with */

            /* TODO:  Is this right, or should be be sending 1's in the M1/M2
            ** bits as well??
            */


            tx_status.SA_status_tx = 1;
            tx_status.SB_status_tx = 1;
            
            ds_gcsd_set_tx_data_14400 (NULL, tx_status, radio_tx_data);
            break;


         case DS_UCSD_RATE_9600:
         
            frame_size = DS_UMTS_CSD_FRAME_SIZE_240_BIT;
            /*send 4 60-bit frames of all 1s*/

            ds_gcsd_set_tx_data_9600 (NULL, tx_status, radio_tx_data);
            break;

         default:
            /*unsupported data rate*/
            DS_3GPP_MSG1_ERROR ("GCSD RA1'TX: Unsupported user rate = %d",(int)Gcsd_curr_data_rate);
            break;
      }
   } 

   else /* Don't send sync pattern, send user data */
   {
      switch (Gcsd_curr_data_rate)
      {
         case DS_UCSD_RATE_14400:
            
            frame_size = DS_UMTS_CSD_FRAME_SIZE_288_BIT;
            
            /* Set the radio data */
            ds_gcsd_set_tx_data_14400 (sync_tx_data, tx_status, radio_tx_data);

            break;
            
         case DS_UCSD_RATE_9600:
            
            frame_size = DS_UMTS_CSD_FRAME_SIZE_240_BIT;
            /* Copy user data and status data to the radio_tx_data 
            ** buffer
            */

            ds_gcsd_set_tx_data_9600 (sync_tx_data, tx_status, radio_tx_data);
                      
            break;

         

         default:
            DS_3GPP_MSG1_ERROR ("GCSD RA1'TX: Unsupported user rate = %d",(int)Gcsd_curr_data_rate);
            break;
      }
   }

      /* Determine the frame size for logging */
   if (Gcsd_curr_data_rate == DS_UCSD_RATE_14400)
   {
      frame_size = DS_UMTS_CSD_FRAME_SIZE_288_BIT;
   }
   else
   {
      frame_size = DS_UMTS_CSD_FRAME_SIZE_240_BIT;
   }
         
   /* Log the transmit data */
   ds_ucsd_log_tx_data (radio_tx_data, frame_size);
   
   (void)ds_gcsd_write_to_channel_coder(radio_tx_data);
}

#endif /* defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD) */
