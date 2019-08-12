/*===========================================================================

              GSM/WCDMA RA0 RATE ADAPTION FUNCTIONS

DESCRIPTION
  This file contains the implementation of the GSM CS Transparent data 
  RA0 rate adaption functionality.
  
  This code has been ported from code written by Communication Consultants 
  Worldwide (CCWW) and subsequently purchased by Qualcomm Technologies Inc.

   Copyright (c) 2001 - 2014 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdra0.c_v   1.4   25 Jun 2002 13:11:42   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdra0.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/09   ms      Removal of private header file.
06/29/09   BS      Fixed off-target lint medium & lows.
03/07/09   MS      CMI Integration fixes
07/08/07   DA      Added user data inactivity timer functionality
23/02/07   DA      High lint errors fixed
09/01/06   AR      Add support for multiprocessor build.
05/30/04   AR      Lint corrections.
06/07/02   TMR     Completely dumped the old stuff and re-wrote to make it
                   better, faster, stronger.... well..... faster, anyway. It 
                   isn't pretty code, and not something I'm particularly proud
                   of, but it does seem to be significantly faster than the
                   old stuff.
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

#if defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)

#include "dsgcsdi.h"
#define BITS_PER_UINT32          32


static uint32 Apply_RA0 = 0;
static uint32 Character_length = 0;
static uint32 Num_data_bits = 0;
static uint32 All_stop_bits_character = 0;
static uint32 Bits_per_block = 0;
static uint32 Stop_bits = 0;
static uint32 Rx_data_mask = 0;


// Data value information saved between frames
static uint32 Tx_saved_num_bits_remaining = 0;
static uint32 Tx_saved_data_value = 0;
static uint32 Rx_saved_num_saved_data_bits = 0;
static uint32 Rx_saved_serial_data = 0;
static uint32 Rx_saved_valid_data_flag = 0;


// Flag to indicate whether or not Tx data was read from
// the serial port
static uint32 Tx_data_from_serial_port = 0;


// static uint32 Rx_frame_bytes;
// static uint32 Rx_total_bytes;

/*===========================================================================

FUNCTION  ds_gcsd_calculate_number_of_bits

DESCRIPTION
   Calculates the number of bits required for each character that is 
   transmitted based on bearer capabilities.  This code hasn't been 
   touched much from CCWW   
         
DEPENDENCIES
  None
  
RETURN VALUE
  The number of bits (start, stop, data, parity) that each character
  transmitted OTA will require.
    
SIDE EFFECTS
  None

===========================================================================*/
uint32 
ds_gcsd_calculate_number_of_bits
(
  const cm_bearer_capability_s_type *bearer_cap_ptr,
   current_call_mode_T             current_call_mode,
   uint32                          *number_of_data_bits,
   uint32                          *number_of_stop_bits,
   uint32                          *number_of_parity_bits
)
{
   uint32  character_length;
   uint8   parity_code = NAS_PARITY_NONE;
   uint8   data_bits_code = 0;
   uint8   stop_bits_code = 0;

   ASSERT( NULL != bearer_cap_ptr );

   /*pick current mode (1st or 2nd) negotiated_capability*/

   if (current_call_mode == MODE_1)
   {
      parity_code    = bearer_cap_ptr->parity;
      data_bits_code = bearer_cap_ptr->number_of_data_bits;
      stop_bits_code = bearer_cap_ptr->number_of_stop_bits;
   }
#if 0   /* Dual Mode Bearer supprot unused at this time */
   else
   {
      parity_code = negotiated_capability->bearer_capability_2.parity;
      data_bits_code = negotiated_capability->
         bearer_capability_2.number_of_data_bits;
      stop_bits_code =
         negotiated_capability->bearer_capability_2.number_of_stop_bits;
   }
#endif /* if 0 */

   if (parity_code != NAS_PARITY_NONE)
   {
      *number_of_parity_bits = 1;
   }

   else
   {
      *number_of_parity_bits = 0;
   }
   /*convert number of data bits*/

   switch (data_bits_code)
   {
      case _7_BITS_PER_CHAR:
         *number_of_data_bits = 7;
         break;

      case _8_BITS_PER_CHAR:
         *number_of_data_bits = 8;
         break;
	 
      default:
	 break;  /* make lint happy */
   }

   /*convert number of stop bits*/

   switch (stop_bits_code)
   {
      case _1_STOP_BIT:
         *number_of_stop_bits = 1;
         break;

      case _2_STOP_BITS:
         *number_of_stop_bits = 2;
         break;
	 
      default:
	 break;  /* make lint happy */
   }

   /*calculate character length in bits*/

   character_length =
      *number_of_data_bits +
      *number_of_parity_bits +
      *number_of_stop_bits +
      1;        /*start bit*/

   return character_length;
}


/*===========================================================================

FUNCTION ds_gcsd_init_RA0

DESCRIPTION
   Initializes the static global variables associated with the RA0 
   protocol stack layer.  These include some variables associated
   with character size/type and also whether or not to apply RA0 to the
   TX/RX data.  In the case of FAX calls, we don't apply RA0 to the data
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Sets up static local variables that are used by the RA0 TX/RX layers.

===========================================================================*/
void
ds_gcsd_init_RA0
(
  const cm_bearer_capability_s_type *bearer_cap_ptr,
   ds_ucsd_data_rate_T             data_rate 
)
{
   uint32 i;
   uint32 num_stop_bits, num_parity_bits;
   
   ASSERT( NULL != bearer_cap_ptr );
   
   //Initialize the TX and RX statics 
   Tx_saved_num_bits_remaining  = 0;
   Rx_saved_num_saved_data_bits = 0;
   Rx_saved_valid_data_flag     = FALSE;
   Rx_saved_serial_data         = 0;
   
   // Rx_frame_bytes = Rx_total_bytes = 0;

   // Initialize Tx_data_from_serial_port global.
   Tx_data_from_serial_port = FALSE;
   
   // Initialize the Bits_per_block value 
   switch (data_rate)
   {
      case DS_UCSD_RATE_14400: 
         Bits_per_block = BITS_PER_BLOCK_14400;
         break;

      case DS_UCSD_RATE_9600: 
         Bits_per_block = BITS_PER_BLOCK_9600;
         break;
         
      default:
         DS_3GPP_MSG1_ERROR ("Unsupported user rate=%d",(int)data_rate);
         break;
   }
   
   // Initialze data format parameters  
   if( bearer_cap_ptr->sync_async == SYNCHRONOUS )
   {
      Apply_RA0 = FALSE;
      
      if( bearer_cap_ptr->number_of_data_bits == _7_BITS_PER_CHAR )
      {
         Character_length = 7;
         Num_data_bits = 7;
      }
      else
      {
         Character_length = 8;
         Num_data_bits = 8; 
      }
      
      Stop_bits = 0;
      num_stop_bits = 0;
      num_parity_bits = 0;
   }
   else
   {
      Apply_RA0 = TRUE;
      
      // Figure out the character length and the length of components   
      Character_length = ds_gcsd_calculate_number_of_bits ( 
                              bearer_cap_ptr,
                              MODE_1,
                              &Num_data_bits,
                              &num_stop_bits,
                              &num_parity_bits);
                              
      // Initialize Stop_bits value
      if (num_stop_bits == 1)
      {
         Stop_bits = (0x01 << (Character_length - num_stop_bits));
      }
      else
      {
         Stop_bits = (0x03 << (Character_length - num_stop_bits));
      }
         
   }
                                
   // Initialize the all stop bit character to be all 1's of character length
   All_stop_bits_character = 0;                        
   for (i = 0; i < Character_length; i++)
   {
      All_stop_bits_character <<= 1;
      All_stop_bits_character |= 0x00000001;
   }
   
   // Initialize the Rx_data_mask to be all 1's of num_data_bits length.
   Rx_data_mask = 0;
   for (i = 0; i < Num_data_bits; i++)
   {
      Rx_data_mask <<= 1;
      Rx_data_mask |= 0x00000001;
   }
}


/*===========================================================================

FUNCTION ds_gcsd_RA0_tx_data

DESCRIPTION
   Performs RA0 processing on transmit (uplink) data.   
  Parameter is pointer to data being returned to caller.
         
DEPENDENCIES
  ds_gcsd_init_RA0 must have been called first.  ALSO NOTE, THIS ALGORITHM
  DEPENDS ON THE FACT THAT THE NUMBER OF DATA BITS IS AN EVEN MULTIPLE OF
  32.  IF THAT EVER CHANGES, THIS WILL NEED TO BE TWEAKED.
  
RETURN VALUE
  Number of data bytes processed
    
SIDE EFFECTS
  None

===========================================================================*/
uint32
ds_gcsd_RA0_tx_data
(
   uint32 *tx_data
)
{
   uint8    tmp_serial_data;
   
   register uint32 uint32_count;
   register uint32 tmp_tx_data_bits;
   register uint32 serial_data = 0;
   register uint32 tmp_character_length;
   register uint32 real_data_flag = FALSE;
   register int32  remaining_bit_count;
   register uint32 tx_data_count = 0;
   
   // Initialize locals
   remaining_bit_count = (int32)Bits_per_block;
   uint32_count = 0;
   tmp_character_length = Character_length;
   
   
   // First, add bits that were left over from previous data value, if any
   if (Tx_saved_num_bits_remaining)
   {
      remaining_bit_count -= (int32)Tx_saved_num_bits_remaining;
      uint32_count        += Tx_saved_num_bits_remaining;
      tmp_tx_data_bits     = Tx_saved_data_value >> 
                                 (tmp_character_length - Tx_saved_num_bits_remaining);
   }
   else
   {
      tmp_tx_data_bits = 0;
   }
   
   
   // Loop until we fill up the buffers with enough user data bits
   while (remaining_bit_count > 0)
   {
      
      // Read data from serial port and add in start and stop bits.
      if (ds_gcsd_read_from_serial_port (&tmp_serial_data))
      {
         serial_data = tmp_serial_data;
         real_data_flag = TRUE;
         tx_data_count++;
         if (Apply_RA0)
         {
            /*add start bit by shifting in a 0 for start bit */
            serial_data <<= 1;
            
            /* Add stop bits */
            serial_data |= Stop_bits;
         }
      }
      
      // No data available from serial port... just use all stop bits
      else
      {
         real_data_flag = FALSE;
         serial_data = All_stop_bits_character;
      }
      
      // Add the data to the tmp_tx_data_bits and adjust the count
      tmp_tx_data_bits |= (serial_data << uint32_count);
      uint32_count += tmp_character_length;
      
      // If we have filled up the current tmp_tx_data_bits value...
      if (uint32_count >= BITS_PER_UINT32)
      {
         // Save tmp_tx_data_bits to the tx_data array
         *tx_data = tmp_tx_data_bits;
         tx_data++;
         
         //Subtract 32 from the count 
         uint32_count -= BITS_PER_UINT32;
         
         // Add bits that were not part of saved tx_data into the
         // next one
         if (uint32_count)
         {
            tmp_tx_data_bits = serial_data >> (tmp_character_length - uint32_count);
         }
         else
         {
            tmp_tx_data_bits = 0;
         }
      }
      
      // Subtract the character length from the remaining bit count
      remaining_bit_count -= (int32)tmp_character_length;
   }
   
   // If we were processing data from the serial port, save the
   // data value and the number of bits remaining, otherwise,
   // don't bother, it is just stop bits filled data 
   if (real_data_flag)
   {
      Tx_saved_num_bits_remaining = (uint32)-remaining_bit_count;
      Tx_saved_data_value = serial_data;
   }
   else
   {
      Tx_saved_num_bits_remaining = 0;
   }
   /* Update user data inactivity timer */
   ds_gcsd_s30_timer_control(tx_data_count);
   
   // Set global
   Tx_data_from_serial_port = (tx_data_count > 0) ? TRUE : FALSE;

   return tx_data_count;
}
      
      
/*===========================================================================

FUNCTION  ds_gcsd_RA0_rx_data

DESCRIPTION
   Performs RA0 processing or received (downlink) data.  The received
   bits are scanned for user data characters, and the characters are 
   written to the serial port if any are found.
   Parameter is pointer to array of data passed by caller.   
         
DEPENDENCIES
  None
  
RETURN VALUE
  Number of data bytes processed.
    
SIDE EFFECTS
  None

===========================================================================*/
uint32
ds_gcsd_RA0_rx_data
(
   uint32 *rx_data
)
{    
   register uint32 uint32_count;
   register uint32 tmp_rx_data_bits;
   register uint32 serial_data;
   register uint32 valid_data_flag;
   register uint32 num_saved_data_bits;
   register int32  remaining_bit_count;
   register uint32 tmp_num_data_bits;
   register uint32 rx_data_count = 0;


   // Initialize locals
   remaining_bit_count  = (int32)Bits_per_block;
   uint32_count         = 0;
   tmp_num_data_bits    = Num_data_bits;
   
   // Rx_frame_bytes = 0;
      
   // First, add bits that were left over from previous received data frame, if any.
   if (Apply_RA0)
   {
      num_saved_data_bits = Rx_saved_num_saved_data_bits;
      valid_data_flag     = Rx_saved_valid_data_flag;
      serial_data         = Rx_saved_serial_data;
   }
   else
   {
      serial_data         = 0;
      num_saved_data_bits = 0;
      valid_data_flag     = TRUE;
   }
   
      
   // get the first 32 received data bits and store in register variable.
   tmp_rx_data_bits = *rx_data++;
   
   // This decodes the input bit stream and writes any found data 
   // characters to SIO
   while (remaining_bit_count > 0)
   {
      // Get the next 32 bits of input data if necessary
      if (uint32_count >= BITS_PER_UINT32)
      {
         tmp_rx_data_bits = *rx_data++;
         uint32_count = 0;
      }

      // If we have valid data to decode...
      if (valid_data_flag)
      {
         // This case comes into play if we saved one or more bits from the
         // previous uint32 of this frame or from a previous frame
         if (num_saved_data_bits)
         {
            serial_data |= ((tmp_rx_data_bits >> uint32_count) << num_saved_data_bits) 
                                                                       & Rx_data_mask;
            uint32_count += tmp_num_data_bits - num_saved_data_bits;
         }
         
         // Save the bits to serial_data and up the count of bits scanned from
         // the uint32_count;
         else
         {
            serial_data |= (tmp_rx_data_bits >> uint32_count) & Rx_data_mask;
            uint32_count += tmp_num_data_bits;
         }
         
         // If the count is greater than 32, then we only saved part of the 
         // received data character.   
         if (uint32_count > BITS_PER_UINT32)
         {
            /* Store the number of data bits actually saved */
	    num_saved_data_bits = tmp_num_data_bits -
	                           (uint32_count - BITS_PER_UINT32);
            remaining_bit_count -= (int32)num_saved_data_bits;
         }
         
         // Got the whole data character, write it out to the serial port.
         else
         {
            // Rx_frame_bytes++;
            (void)ds_gcsd_write_to_serial_port((uint8)serial_data, TRUE, FALSE);
            serial_data = 0;
            remaining_bit_count -= (int32)(tmp_num_data_bits - num_saved_data_bits);
            num_saved_data_bits = 0;
            rx_data_count++;
            
            // If we are applying RA0, the we set the valid_data_flag to 
            // FALSE so that we again scan for start bits.
            if (Apply_RA0)
            {
               valid_data_flag = FALSE;
            }
         }
      }
      
      // We don't currently have valid data to be received, so keep scanning
      // for start bits.
      else
      {
         // If we find a start bit, set the valid data flag to TRUE
         if (!(tmp_rx_data_bits & (0x00000001 << uint32_count)))
         {
            valid_data_flag = TRUE;
         }
         uint32_count++;
         remaining_bit_count--;
      }
   }
   
   // Flush out the written serial port data
   (void)ds_gcsd_write_to_serial_port(0, FALSE, TRUE);
   
   // Save the state of the RA0 RX processing for next frame
   if (Apply_RA0)
   {
      Rx_saved_num_saved_data_bits =  num_saved_data_bits;
      Rx_saved_serial_data = serial_data;
      Rx_saved_valid_data_flag = valid_data_flag;
   }
   /* Update user data inactivity timer */
   ds_gcsd_s30_timer_control(rx_data_count);
   
   // Rx_total_bytes += Rx_frame_bytes;
   // DS_UCSD_DEBUG_MSG_2("T-data RX: frame=%d, total=%d",Rx_frame_bytes, Rx_total_bytes);

   return rx_data_count;
}     
 
 

boolean
ds_gcsd_tx_transparent_data_read_from_serial_port (void)
{
   return (boolean) Tx_data_from_serial_port;
}  

#endif /* defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD) */
