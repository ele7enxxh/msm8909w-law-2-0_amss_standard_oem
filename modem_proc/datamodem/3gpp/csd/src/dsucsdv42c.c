
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

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42c.c_v   1.3   21 Mar 2002 10:58:10   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdv42c.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/11   TTV     Support for Q6 free floating.
02/24/11   TTV     Fixed compilation warnings.
02/12/07   AR      Resolve RVCT compiler build messages.
08/25/06   AR      Use termination flag to stop reading UL data.
02/09/06   AR      Lint corrections.
03/14/05   AR      Ensure InputTxBuffer is NULL on init.
08/31/04   AR      Simplify initialization sequence.
11/10/03   AR      Remove premature return on empty input buffer.
07/15/03   TMR     Added support for RLP Version 2
04/30/03   AR      Add suppport for DSM items inplace of local buffers
                   Lint corrections
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/


/*******************************************************************************
 *
 *  File:         /home/briers/gsmqc/ds/SCCS/s.m51321.c
 *  -----
 *
 *  SCCS Version: 2.4
 *  -------------
 *
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained herein
 *  is confidential.  The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Communications Consultants Worldwide Limited (C) 2000
 *
 *  Project:   P60
 *  --------
 *
 *  Software Unit: V.42bis compression standard
 *  --------------
 *
 *  Description:
 *  ------------
 *  This module performs compression in accordance with V.42bis standard.
 *
 *
 *  Functions Used:
 *  ---------------
 *
 *  Modification History:
 *
 *  ---------------------
 *  Date         Vers  Auth  Details
 *  ----         ----  ----  -------
 *  12-Feb-1999  1.0   PGB   Created
 *  12-Apr-2000  2.1   RCB   Various small changes for integration with L2R.
 *  26-Jul-2000  2.2   RCB   Update file header
 *  27-Jul-2000  2.3   RCB   Update file header
 *  04-Sep-2000  2.4   RCB   Reinstate exit after MAX_ENCODE_CYCLES
 *
 ******************************************************************************/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)

#include "dsucsdnti.h"
#include "dsucsdrlp.h"
#include "dsucsdv42.h"
#include "err.h"
#include "data_msg.h"



/**************************** Globals **********************************/

/* codetable and hashtable */
LOCAL word     ds_v42_c_hash_table[HASHTABLE_SIZE]       = {0};
LOCAL word     ds_v42_c_prefix_table[CODETABLE_SIZE]     = {0};
LOCAL uint8    ds_v42_c_app_char_table[CODETABLE_SIZE]   = {0};
LOCAL uint8    ds_v42_c_leaf_count_table[CODETABLE_SIZE] = {0};


/* pointers to the arrays used to avoid passing variables excessively  */
LOCAL  word    *ds_v42_c_hash_ptr          = ds_v42_c_hash_table;   
#define ds_v42_c_prefix_ptr                  ds_v42_c_prefix_table
#define ds_v42_c_app_char_ptr                ds_v42_c_app_char_table
#define ds_v42_c_leaf_count_ptr              ds_v42_c_leaf_count_table

/* needed in add function to store next free code table entry */
LOCAL word     ds_v42_c_next_code = 0;

 
/* find_match return flag */
LOCAL byte     ds_v42_c_search_success = 0;

/* flag to indicate hashtable needs refilling */
LOCAL byte     ds_v42_c_refill_hash_flag = 0;

LOCAL word     ds_v42_c_max_code = 0;           /* current max code,initially 512 */
LOCAL word     ds_v42_c_stepup_threshold = 0;   /* the codesize in bits for next
                                     * stepup                         */
LOCAL byte     ds_v42_c_mode = 0;             /* can be transparent or compressed */
LOCAL byte     ds_v42_c_codeword_size = 0;    /* current code size in bits        */
LOCAL byte     ds_v42_c_escape_code = 0;      /* used in transparent ds_v42_c_mode         */


/* for compression testing */
LOCAL word     ds_v42_c_datain_count   = 0;
LOCAL word     ds_v42_c_string_matches = 0;
LOCAL word     ds_v42_c_dataout_count  = 0;

/* current string and character, need to be global for the exception routine */
LOCAL word     ds_v42_c_string_code = 0;
LOCAL byte     ds_v42_c_character = 0;


/* used in compress function and test_compression */
LOCAL byte     ds_v42_c_string_length = 0;   /* used for keeping within N7 string length
                                  * limit */

/* used for octet alignment by flush and switch */
LOCAL byte     ds_v42_c_output_bit_count = 0;   /* stores the number of bits in the
                                     * output buffer */


/* flag to indicate a control code is to be sent */
LOCAL byte     ds_v42_c_control_code_flag = 0;   /* used in output_code */


/*
 * used in compress and exception functions, it stores the last code added to
 * the dictionary used to ensure an entry is not sent before it is defined in
 * the decoder
 */
LOCAL word     ds_v42_c_last_code = 0;



/* various states possible */
typedef enum
{
/* C_WAIT_FOR_CINIT, */
   C_INIT_COMPRESS,
   C_NORMAL
} ds_v42_c_states_T;

LOCAL ds_v42_c_states_T  ds_v42_c_state = C_INIT_COMPRESS; /* current state */


LOCAL byte     ds_v42_c_exception_char_next_flag = 0;   /* indicates whether the
                                             * exception function is
                                             * called on next loop  */


/* stores the bits to be sent as codewords to decoder */
LOCAL unsigned long ds_v42_c_output_bit_buffer = 0;

/* used to indicate an error writing to the output buffer */
LOCAL byte     ds_v42_c_buffer_write_error = 0;

/* Used to hold input data for uplink chain.  Buffer is filled with
 * bytes received from serial interface.  Compressor reads single byte
 * on each cycle to form codes. */
static dsm_item_type * InputTxBuffer = NULL;


/* Forward prototypes */
void     exception_process_next_char(void);
void     flush(void);
void     test_compression(void);
void     output_byte(byte data);
void     output_code(word code);
void     stepup(void);
#if 0
void     reset(void);
#endif


/****************************************************************************
 * Function name:  initialise compression
 * -------------
 *
 * Description:    This routine returns all variables to initial state, run
 * -----------     upon first call and C_INIT requests
 *
 * Returns:        none
 * -------
 ****************************************************************************/

void init_compress(void)
{
   DS_UCSD_DEBUG_MSG("V42C: INIT_COMPRESS");
  
   ds_v42_c_state = C_INIT_COMPRESS;   /* current state */
   ds_v42_c_control_code_flag = FALSE;
   ds_v42_c_next_code = FIRST_CODE;    /* 259, 256 + 3 control codes */
   ds_v42_c_last_code = V42BIS_DELETION_MASK; /* indicating null */
   ds_v42_c_string_length = 1;
   ds_v42_c_codeword_size = INITIAL_CODEWORD_SIZE;
   ds_v42_c_mode = TRANSPARENT_MODE;  /* always starts in transparent ds_v42_c_mode */
   ds_v42_c_stepup_threshold = (word)((1 << ds_v42_c_codeword_size) - 1);
   ds_v42_c_max_code = V42_get_max_codewords() - 1;
   ds_v42_c_escape_code = 0;

   /* init compression test variables */
   ds_v42_c_datain_count = (word) (RETEST_THRESHOLD / 1.2);
   //ds_v42_c_datain_count = 0; 
   ds_v42_c_dataout_count = 0;
   ds_v42_c_string_matches = (word) (ds_v42_c_datain_count / 1.5);

   ds_v42_c_string_code = V42BIS_UNUSED_MASK;
   ds_v42_c_exception_char_next_flag = FALSE;
   ds_v42_c_refill_hash_flag = FALSE;
   ds_v42_c_output_bit_buffer = 0;
   ds_v42_c_output_bit_count = 0;
   ds_v42_c_buffer_write_error = FALSE;
   
   /* Clear data buffer */
   if ( NULL != InputTxBuffer )
   {
     ds_ucsd_UplinkPDUMgr_Free (&InputTxBuffer);
   }
}




/****************************************************************************
 * Function name:  compress
 * -------------
 *
 * Description:    This is called by the control function to compress data
 * -----------     
 *
 * Returns:        none
 * -------
 ****************************************************************************/

void       compress(void)
{

   register word   index;          /* temporary storage for index values */
   register uint32 exit_count = 0; /* counter to exit function */
   word            RxWord;

   /* check for C_INIT request from control function */
   if ( C_INIT_REQ == V42_get_compress_control_flag() )
   {
      /* Initialise compression function */
      init_compress();
      
      init_tables (ds_v42_c_hash_table, ds_v42_c_prefix_table);

      ds_v42_c_state = C_NORMAL;
      DS_UCSD_DEBUG_MSG_1("V42C: C_NORMAL(%d)",ds_v42_c_state);
               
      V42_set_compress_control_flag(C_CLEAR);
      V42_set_compress_status_flag(C_INIT_CNF);
   }


   /* if the ds_v42_c_hash_table is congested this function will defrag it */
   if (ds_v42_c_refill_hash_flag)
   {
      /* purge and refill hash table */
     
      refill_hash
         (
            V42_get_max_codewords(),
            ds_v42_c_hash_ptr,   
            ds_v42_c_prefix_ptr,
            ds_v42_c_app_char_ptr
         );
         
      ds_v42_c_refill_hash_flag = FALSE;
      return; /* too time consuming to continue any with encode cycles */
   }

   /* pull in data from upstream stage */
   if( !V42_get_terminate_flag() )
   {
     if (0 ==
         ds_ucsd_V24_interface_handler_byte_oriented(&InputTxBuffer,
                                                     MAX_TX_BUF_BYTES) )
     {
       DATA_MSG0_LOW("V42 compress input buffer empty");
     }
   }
   
   /*
    * this loop is ended when either the read buffer is empty, or 
    * the maximum number of cycles has been reached
    */
   while (1)  /*lint !e716 */
   {

      /*
       * limit the number of bytes processed to ensure time slice
       * of ISR is not over stretched
       */
      if ( ( exit_count++ > MAX_ENCODE_CYCLES ) || 
           ( ds_check_full_V42_tx_buffer() ) )
      {
         /* cycle limit reached */
         return;
      }

      /*
       * check there has been no errors writing to buffers This
       * should never occur as a check is made before executing
       * each cycle that sufficient space is available
       */
      if (ds_v42_c_buffer_write_error)
      {
         ds_v42_c_buffer_write_error = FALSE;
         DATA_MSG0_ERROR("v42encode: buffer write error");
         return;
      }


      /* test for C_FLUSH_REQ from control function */
      if (V42_get_compress_control_flag() == C_FLUSH_REQ)
      {
         /*
          * only performed if not already involved
          * with a flush or ds_v42_c_mode change
          */
         if (ds_v42_c_exception_char_next_flag == FALSE)
         {
            flush();   /* perform flush */
            /* ensure next char is handled by the exception algorithm */
            //ds_v42_c_exception_char_next_flag = TRUE;
            V42_set_compress_status_flag(C_FLUSH_CNF);
            V42_set_compress_control_flag(C_CLEAR);
            continue;
         }
         else
         {
            V42_set_compress_status_flag(C_FLUSH_CNF);
            V42_set_compress_control_flag(C_CLEAR);
            continue;
         }
      }

      /* check for whether to test compression ratio */
      if (ds_v42_c_datain_count > RETEST_THRESHOLD)
      {   
         if (ds_v42_c_exception_char_next_flag == FALSE)
         {
            test_compression();
            continue;
         }
      }
      
      /* read a byte from input buffer */
      if ( 1 != dsm_pullup(&InputTxBuffer, &RxWord, 1) )
      {
         /* return if it's empty */
         DATA_MSG0_LOW("V42 compress exiting - input buffer empty");
         return;
      }
      ds_v42_c_character = (byte)RxWord;
      

      /* compression test uses this */
      ds_v42_c_datain_count++;


      /*
       * this will be set if a FLUSH or ds_v42_c_mode change have occurred
       * the proceeding char will be processed slightly differently
       */
      if (ds_v42_c_exception_char_next_flag)
      {
         exception_process_next_char();
         continue;
      }


      /* Detect first character condition */
      if (ds_v42_c_string_code == V42BIS_UNUSED_MASK)
      {
         ds_v42_c_string_code = ds_v42_c_character;
         ds_v42_c_string_length = 1;

         /* Assume transparent ds_v42_c_mode */
         output_byte(ds_v42_c_character);

         /* escape code handling */
         if (ds_v42_c_character == ds_v42_c_escape_code)
         {
            /* output escape in data command code */
            output_byte(EID);
            /* cycle escape */
            ds_v42_c_escape_code = (ds_v42_c_escape_code + 51) % 256;
         }
         continue;
      }



      /* search for ds_v42_c_string_code character combination in dictionary */
      index = find_match
                  (
                     ds_v42_c_string_code,
                     ds_v42_c_character,
                     ds_v42_c_hash_ptr,
                     ds_v42_c_prefix_ptr,
                     ds_v42_c_app_char_ptr,
                     &ds_v42_c_refill_hash_flag,
                     &ds_v42_c_search_success
                   );

      if (ds_v42_c_search_success)   /* match found */
      {
         /* test for use of last code  */
         if ((ds_v42_c_hash_ptr[index]) == ds_v42_c_last_code)
         {
            if (ds_v42_c_mode == COMPRESSED_MODE)
            {
               /* if neccesary STEPUP */
               if (ds_v42_c_string_code > ds_v42_c_stepup_threshold)
               {
                  stepup();
               }
         
               output_code(ds_v42_c_string_code);
            }
            /* ds_v42_c_last_code = V42BIS_UNUSED_MASK; */
            ds_v42_c_string_code = ds_v42_c_character;
            ds_v42_c_string_length = 1;
            ds_v42_c_last_code = V42BIS_UNUSED_MASK;

         }
         else   /* match ok */
         {
            ds_v42_c_string_code = (ds_v42_c_hash_ptr[index]);
            ds_v42_c_string_length++;
            ds_v42_c_string_matches++;
         }

      }

      else      /* no match, add to dictionary */
      {
         if ( ds_v42_c_string_length < V42_get_max_string_length() )
         {
            ds_v42_c_last_code = ds_v42_c_next_code;
            
            add_string
            (
               index, 
               ds_v42_c_string_code, 
               ds_v42_c_character,
               ds_v42_c_max_code,
               &ds_v42_c_next_code,
               ds_v42_c_hash_ptr,   
               ds_v42_c_prefix_ptr,
               ds_v42_c_app_char_ptr,
               ds_v42_c_leaf_count_ptr 
            );
         }

         if (ds_v42_c_mode == COMPRESSED_MODE)
         {
            /* if neccesary STEPUP */
            if (ds_v42_c_string_code > ds_v42_c_stepup_threshold)
            {
               stepup();
            }

            output_code(ds_v42_c_string_code);
         }
         ds_v42_c_string_code = ds_v42_c_character;
         ds_v42_c_string_length = 1;
      }




      if (ds_v42_c_mode == TRANSPARENT_MODE)
      {
         output_byte(ds_v42_c_character);
      }

      /* escape code handling */
      if (ds_v42_c_character == ds_v42_c_escape_code)
      {
         if (ds_v42_c_mode == TRANSPARENT_MODE)
         {
            /* output escape in data command code */
            output_byte(EID);
         }
         /* cycle escape */
         ds_v42_c_escape_code = (ds_v42_c_escape_code + 51) % 256;
      }


   }         /* End of while loop */

}



/****************************************************************************
 * Function name:  output code
 * -------------
 *
 * Description:    This routine adds a codeword to the output bit buffer,
 * -----------     any whole bytes are then sent to the output buffer
 *
 * Returns:        none
 * -------
 ****************************************************************************/


void       output_code(word code)
{
   
   //DS_UCSD_DEBUG_MSG_2 ("encoder->output code %d, %c", code, code);

   /* offset ascii codes to allow for 3 control codes */
   if ((code < FIRST_CODE) && (!(ds_v42_c_control_code_flag)))
   {
      code += 3;
   }

   /* add code to bit buffer */
   ds_v42_c_output_bit_buffer |=
      ((unsigned long) code << ds_v42_c_output_bit_count);
   ds_v42_c_output_bit_count += ds_v42_c_codeword_size;

   /* output any whole bytes in the bit buffer */
   while (ds_v42_c_output_bit_count >= 8)
   {
      ds_write_to_V42_tx_buffer((byte) (ds_v42_c_output_bit_buffer & 255));
      V42_set_flushed(FALSE);

      /*** Old code: buffer check now done beforehand **
      if (ds_write_to_V42_tx_buffer(ds_v42_c_output_bit_buffer & 255) != 0)
      {
         printf("Error writing to tx comp buffer");
         ds_v42_c_buffer_write_error = TRUE;
      }
      ********/

      /* shift byte out of bit buffer */
      ds_v42_c_output_bit_buffer >>= 8;
      ds_v42_c_output_bit_count -= 8;
      ds_v42_c_dataout_count++;/* used in compression test */
   }
   /* reset control code flag */
   ds_v42_c_control_code_flag = FALSE;
}





/****************************************************************************
 * Function name:  output byte
 * -------------
 *
 * Description:    used in transparent ds_v42_c_mode to send a byte
 * -----------
 *
 * Returns:        none
 * -------
 *
 ****************************************************************************/


void       output_byte(byte character)
{
 
   ds_write_to_V42_tx_buffer(character);
   V42_set_flushed(FALSE);

   /****** Old Code: buffer check now done beforehand *
   if (ds_write_to_V42_tx_buffer(character) != 0)
   {
      ds_v42_c_buffer_write_error = TRUE;
   }
   ******/

   //DS_UCSD_DEBUG_MSG_2 ("encoder->output byte %d, %c", character, character);
   
   /* used in compression test */
   ds_v42_c_dataout_count++;

}




/****************************************************************************
 *
 * Function name:   test compression
 * -------------
 *
 * Description:    This routine check the compression ratio and determines
 * -----------     whether a change of ds_v42_c_mode is necessary
 *
 * Returns:        none
 * -------
 *
 ****************************************************************************/

void       test_compression(void)
{
   byte            temp;   /* used to store ds_v42_c_codeword_size during octet
                            * align */
    


   if (ds_v42_c_mode == TRANSPARENT_MODE)
   {
      if (ds_v42_c_string_matches)
      {
         /* test for enough string matches to warrant change to compressed */
         
         if ((ds_v42_c_datain_count / ds_v42_c_string_matches) < ECM_THRESHOLD)
         {
            /* enter compressed ds_v42_c_mode */
            DS_UCSD_DEBUG_MSG ("V42bis: enter compressed mode");
            /* output Enter compressed ds_v42_c_mode control code */
            output_byte(ds_v42_c_escape_code);
            output_byte(ECM);

            /* change ds_v42_c_mode */
            ds_v42_c_mode = COMPRESSED_MODE;

            /* clear bit buffer */
            ds_v42_c_output_bit_count = 0;
            ds_v42_c_output_bit_buffer = 0;

            ds_v42_c_exception_char_next_flag = TRUE;

         }
      }

   }

   else         /* COMPRESSED_MODE */
   {
      if (ds_v42_c_dataout_count)
      {

         /* check for poor compression ratio to warrant change to transparent */
         if ((ds_v42_c_datain_count / ds_v42_c_dataout_count) < ETM_THRESHOLD)
         {
            /* enter transparent ds_v42_c_mode */
            /* send curently matched string if there is one */
            if (ds_v42_c_string_code != V42BIS_UNUSED_MASK)
            {
               /* if neccesary STEPUP */
               if (ds_v42_c_string_code > ds_v42_c_stepup_threshold)
               {

                  stepup();
               }

               output_code(ds_v42_c_string_code);

            }

            /* send ETM (enter transparent ds_v42_c_mode) */
            ds_v42_c_control_code_flag = TRUE;


            output_code(ETM);

            if (ds_v42_c_output_bit_count)   /* if not octet aligned */
            {
               /* octet align by sending zeros */
               temp = ds_v42_c_codeword_size;
               ds_v42_c_codeword_size = 8 - ds_v42_c_output_bit_count;
               ds_v42_c_control_code_flag = TRUE;
               output_code(0);
               ds_v42_c_codeword_size = temp;
               ds_v42_c_output_bit_buffer = 0;
               ds_v42_c_output_bit_count = 0;
            }

            ds_v42_c_mode = TRANSPARENT_MODE;

            ds_v42_c_exception_char_next_flag = TRUE;
            
            DS_UCSD_DEBUG_MSG ("V42bis: enter transparent mode");
         }




      }
   }


   ds_v42_c_datain_count = 0;
   ds_v42_c_dataout_count = 0;
   ds_v42_c_string_matches = 0;
}

#if 0
/****************************************************************************
 *
 * Function name:   Reset
 * -------------
 *
 * Description:    This routine is used to RESET the dictionary, which may be
 * -----------     beneficial to performance. In this implementation however
 *                   this feature is not used due to the small dictionary size
 *
 * Returns:        none
 * -------
 *
 ****************************************************************************/


void       reset(void)
{
   byte            temp;
   if (ds_v42_c_mode == COMPRESSED_MODE)
   {
      /* is string empty */
      if (ds_v42_c_string_code != V42BIS_UNUSED_MASK)
      {
         /* if neccesary STEPUP */
         if (ds_v42_c_string_code > ds_v42_c_stepup_threshold)
         {
            stepup();
         }
         output_code(ds_v42_c_string_code);
      }

      /* send ETM (enter transparent ds_v42_c_mode) */
      ds_v42_c_control_code_flag = TRUE;
      output_code(ETM);

      if (ds_v42_c_output_bit_count)   /* if not octet aligned */
      {
         /*
          * octet align by sending zeros
          */
         temp = ds_v42_c_codeword_size;
         ds_v42_c_codeword_size = 8 - ds_v42_c_output_bit_count;
         ds_v42_c_control_code_flag = TRUE;
         output_code(0);
         ds_v42_c_codeword_size = temp;
         ds_v42_c_output_bit_count = 0;
         ds_v42_c_output_bit_buffer = 0;
      }

      ds_v42_c_mode = TRANSPARENT_MODE;

   }
   /* send RESET */
   output_byte(ds_v42_c_escape_code);
   output_byte(RESET);

   /* initialise compression function */
   init_tables (ds_v42_c_hash_table, ds_v42_c_prefix_table);
               
   init_compress();
   ds_v42_c_last_code = V42BIS_UNUSED_MASK;
   ds_v42_c_string_code = V42BIS_UNUSED_MASK;
}
#endif



/****************************************************************************
 *
 * Function name:   exception_process_next_char
 * -------------
 *
 * Description:    This routine is called after a FLUSH or change of ds_v42_c_mode
 * -----------     it processes a character in a slightly different way.
 *
 * Returns:        none
 * -------
 *
 ****************************************************************************/

void            exception_process_next_char(void)
{
   word            hash_index;



   /* search for string character pair */
   hash_index = find_match
                  (
                     ds_v42_c_string_code,
                     ds_v42_c_character,
                     ds_v42_c_hash_ptr,
                     ds_v42_c_prefix_ptr,
                     ds_v42_c_app_char_ptr,
                     &ds_v42_c_refill_hash_flag,
                     &ds_v42_c_search_success
                  );

   if (ds_v42_c_search_success == FALSE)
   {
      /* no match so add to dictionary */
      if (ds_v42_c_string_length < V42_get_max_string_length() )
      {
         ds_v42_c_last_code = ds_v42_c_next_code;
         add_string
            (
               hash_index, 
               ds_v42_c_string_code, 
               ds_v42_c_character,
               ds_v42_c_max_code,
               &ds_v42_c_next_code,
               ds_v42_c_hash_ptr,   
               ds_v42_c_prefix_ptr,
               ds_v42_c_app_char_ptr,
               ds_v42_c_leaf_count_ptr 
            );
      }
   }
   /* terminate string matching */
   ds_v42_c_string_code = ds_v42_c_character;
   ds_v42_c_string_length = 1;

   if (ds_v42_c_mode == TRANSPARENT_MODE)
   {
      output_byte(ds_v42_c_character);
   }


   /* escape code handling */
   if (ds_v42_c_character == ds_v42_c_escape_code)
   {
      if (ds_v42_c_mode == TRANSPARENT_MODE)
      {
         /* output escape in data command code */
         output_byte(EID);
      }
      /* cycle escape */
      ds_v42_c_escape_code = (ds_v42_c_escape_code + 51) % 256;
   }
   ds_v42_c_exception_char_next_flag = FALSE;
}



/****************************************************************************
 *
 * Function name:   flush
 * -------------
 *
 * Description:    This routine performs a flush, called normally at the end
 * -----------     of a transmission to clear out any leftover bits in the
 *                   buffer.
 *
 * Returns:        none
 * -------
 *
 ****************************************************************************/

void       flush(void)
{
   byte            temp;


   if (ds_v42_c_mode == COMPRESSED_MODE)
   {
      /* send string code if there is one */
      if (ds_v42_c_string_code != V42BIS_UNUSED_MASK)
      {
         /* if neccesary STEPUP */
         if (ds_v42_c_string_code > ds_v42_c_stepup_threshold)
         {
            stepup();
         }
         output_code(ds_v42_c_string_code);
      }

      /* send FLUSH and octet align if not already aligned */
      if (ds_v42_c_output_bit_count > 0)
      {   
         /* send FLUSH */
         ds_v42_c_control_code_flag = TRUE;
         output_code(FLUSH);
         /* octet align */
         if (ds_v42_c_output_bit_count > 0)
         {
            temp = ds_v42_c_codeword_size;
            ds_v42_c_codeword_size = 8 - ds_v42_c_output_bit_count;
            /* send zero bits to octet align */
            output_code(0);
            /* restore codeword size */
            ds_v42_c_codeword_size = temp;
            /* clear output bit buffer */
            ds_v42_c_output_bit_buffer = 0;
         }
         V42_set_flushed(TRUE);
      }
      ds_v42_c_exception_char_next_flag = TRUE;
   }
   else         /* TRANSPARENT_MODE */
   {
      /* no action necessary */
   }

}


/****************************************************************************
 *
 * Function name:   stepup
 * -------------
 *
 * Description:   This function increments codeword size and sends the STEPUP
 * -----------    code to the encoder
 *                
 *
 * Returns:        none
 * -------
 *
 ****************************************************************************/

void stepup(void)
{
   do
   {
      ds_v42_c_control_code_flag = TRUE;
      output_code(STEPUP);
      ds_v42_c_codeword_size++;
      ds_v42_c_stepup_threshold = (word)((1 << ds_v42_c_codeword_size) - 1);
   } while (ds_v42_c_string_code > ds_v42_c_stepup_threshold);
}

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */
