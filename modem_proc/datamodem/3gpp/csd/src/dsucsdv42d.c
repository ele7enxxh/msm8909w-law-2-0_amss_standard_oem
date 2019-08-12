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

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42d.c_v   1.2   12 Feb 2002 16:53:22   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdv42d.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/11   TTV     Support for Q6 free floating.
03/17/11   TTV     Fixed compilation warnings.
02/24/11   TTV     Fixed compilation warnings.
09/29/09   SA      Fixed KW errors.
02/12/07   AR      Resolve RVCT compiler build messages.
02/09/06   AR      Lint corrections.
03/14/05   AR      Ensure OutputRxBuffer is NULL on init.
08/31/04   AR      Simplify initialization sequence.
05/05/04   AR      Adjust flow control on Rx buffer
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
 *  File:         /home/briers/gsmqc/ds/SCCS/s.m51322.c
 *  -----
 *
 *  SCCS Version: 2.3
 *  -------------
 *
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained herein
 *  is confidential.  The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Communications Consultants Worldwide Limited (C) 2000
 *
 *  Project:  P60
 *  --------
 *
 *  Software Unit:   V42bis compression standard  
 *  --------------
 *
 *  Description:
 *  ------------
 *  This module performs decompression in accordance with V42bis standard.
 *
 *
 *  Functions Used:
 *  ---------------
 *
 *  Modification History:
 *  ---------------------
 *  Date         Vers  Auth  Details
 *  ----         ----  ----  -------
 *  24-Feb-1999  1.0   PGB   Created
 *  12-Apr-2000  2.1   RCB   Various small changes for integration with L2R.
 *  26-Jul-2000  2.2   RCB   Update file header
 *  04-Sep-2000  2.3   RCB   Reinstate exit after MAX_DECODE_CYCLES
 *
 ******************************************************************************/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_3gppi_utils.h"

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)

#include "dsucsdnti.h"
#include "dsucsdrlp.h"
#include "dsucsdv42.h"
#include "err.h"
#include "modem_mem.h"



/************************** Globals ***********************************/

/* This is main pointer, below four pointers are allocated from this.*/
static uint8   *ds_v42_d_main_ptr;
/* code and hash tables */
LOCAL word     *ds_v42_d_hash_table_ptr;
LOCAL word     *ds_v42_d_prefix_table_ptr;
LOCAL uint8    *ds_v42_d_app_char_table_ptr;
LOCAL uint8    *ds_v42_d_leaf_count_table_ptr;


/* pointers to the tables used for efficiency */
LOCAL word  *ds_v42_d_hash_ptr;   
#define ds_v42_d_prefix_ptr                 ds_v42_d_prefix_table_ptr
#define ds_v42_d_app_char_ptr               ds_v42_d_app_char_table_ptr
#define ds_v42_d_leaf_count_ptr             ds_v42_d_leaf_count_table_ptr



LOCAL word     ds_v42_d_max_code = 0;        /* normally 2047             */
LOCAL byte     ds_v42_d_mode = 0;            /* transparent or compressed */
LOCAL byte     ds_v42_d_codeword_size = 0;   /* normally 11               */
LOCAL byte     ds_v42_d_escape_code = 0;
LOCAL word     ds_v42_d_stepup_threshold = 0;   /* code value to go to next codeword size */


/* needed in add function */
LOCAL word     ds_v42_d_next_code = 0;   /* points to next available dictionary entry */

/* d_find_match return flags */
LOCAL byte     ds_v42_d_search_success = 0;

/* flag to indicate hashtable needs refilling */
LOCAL byte     ds_v42_d_refill_hash_flag = 0;


/* holds number of bits left in buffer to be transfered */
LOCAL byte     ds_v42_d_input_bit_count = 0;        /* reset by decode function after ETM */

LOCAL unsigned long ds_v42_d_input_bit_buffer = 0;  /* codeword bit buffer                */




/* various decoder states possible */
typedef enum
{
/* D_WAIT_FOR_DINIT, */
   D_INIT_DECODE,
   D_ESCAPE,
   D_NORMAL
} ds_v42_d_states_T;


LOCAL ds_v42_d_states_T  ds_v42_d_state = D_INIT_DECODE;   /* current state          */
LOCAL word     ds_v42_d_last_code = 0;                         /* last code added        */
LOCAL byte     ds_v42_d_stack[MAX_STRING_LENGTH] = {0};          /* for string reversing   */
LOCAL byte     ds_v42_d_post_ETM_flag = 0;                     /* flag set after ETM     */
LOCAL byte    *ds_v42_d_stack_ptr = NULL;                         /* for reversing strings  */
LOCAL byte     ds_v42_d_string_length = 0;         /* current string length              */
LOCAL byte     ds_v42_d_decode_pending_flag = 0;   /* for resuming of unfinished decodes */
LOCAL word     ds_v42_d_old_code = 0;              /* holds last code decoded            */
LOCAL word     ds_v42_d_string_code = 0;           /* current code word                  */
LOCAL byte     ds_v42_d_character = 0;             /* current appended byte              */
LOCAL byte     ds_v42_d_last_string_length = 0;    /* length of previous string          */
LOCAL word     ds_v42_d_data_pending = 0;          /* store for pending tx buffer writes */
LOCAL byte     ds_v42_d_trans_cycle_limit = 0;     /* limit cycles for transparent mode  */

/* Used to hold output data from downlink chain.  Buffer is filled
 * with bytes from decompressor output.  Contents are flushed to
 * serial interface each decompress cycle. */
static dsm_item_type * OutputRxBuffer = NULL;


/* Forward prototypes */
word     input_code(void);


/****************************************************************************
 * Function name:  init decoder
 * -------------
 *
 * Description:    This routine returns all variables to initial state, run 
 * -----------     upon first call and D_INIT requests
 *    
 * Returns:        none
 * -------         
 ****************************************************************************/

void            init_decoder(void)
{
   DS_UCSD_DEBUG_MSG("V42D: INIT_DECODE");
  
   ds_v42_d_main_ptr = (uint8*)modem_mem_alloc
      ( ( HASHTABLE_SIZE*sizeof(word)  + 
          CODETABLE_SIZE*sizeof(word)  + 
          CODETABLE_SIZE*sizeof(uint8) +
          CODETABLE_SIZE*sizeof(uint8) ),
        MODEM_MEM_CLIENT_DATA
      );

   if( ds_v42_d_main_ptr == NULL ) 
   {
     DS_3GPP_MSG2_ERROR( "Heap Memory Allocation failed for client: %d size: %d" ,
                (int)MODEM_MEM_CLIENT_DATA_CRIT,
                 HASHTABLE_SIZE*sizeof(word) + 
                 CODETABLE_SIZE*(sizeof(word)+2));
   }

   ds_v42_d_hash_table_ptr       = (word*)ds_v42_d_main_ptr;
   ds_v42_d_prefix_table_ptr     = (word*)(ds_v42_d_hash_table_ptr + HASHTABLE_SIZE);
   ds_v42_d_app_char_table_ptr   = (uint8*)(ds_v42_d_prefix_table_ptr + CODETABLE_SIZE);
   ds_v42_d_leaf_count_table_ptr = (uint8*)(ds_v42_d_app_char_table_ptr + CODETABLE_SIZE);
   ds_v42_d_hash_ptr             = ds_v42_d_hash_table_ptr;
  
   ds_v42_d_state = D_INIT_DECODE;
   ds_v42_d_post_ETM_flag = FALSE;
   ds_v42_d_stack_ptr = &ds_v42_d_stack[0];
   ds_v42_d_string_length = 1;
   ds_v42_d_last_code = V42BIS_DELETION_MASK;
   ds_v42_d_refill_hash_flag = FALSE;
   ds_v42_d_mode = TRANSPARENT_MODE;
   ds_v42_d_codeword_size = INITIAL_CODEWORD_SIZE;
   ds_v42_d_max_code = (V42_get_max_codewords() - 1);
   ds_v42_d_stepup_threshold = ((1 << INITIAL_CODEWORD_SIZE) - 1);
   ds_v42_d_escape_code = 0;
   ds_v42_d_next_code = FIRST_CODE;  /* next available dictionary location */
   ds_v42_d_stack_ptr = &ds_v42_d_stack[0];
   ds_v42_d_decode_pending_flag = FALSE;
   ds_v42_d_data_pending = NULL_VALUE;
   ds_v42_d_old_code = V42BIS_UNUSED_MASK;
   ds_v42_d_string_code = V42BIS_UNUSED_MASK;
   ds_v42_d_last_string_length = 1;
   ds_v42_d_input_bit_count = 0;   
   ds_v42_d_input_bit_buffer = 0;

   /* Set processing limit based on call mode */
   ds_v42_d_trans_cycle_limit = (SYS_SYS_MODE_GSM == ds_ucsd_nt_get_curr_mode())?
                                MAX_USER_BYTES_PER_20_MSEC :
                                MAX_USER_BYTES_PER_10_MSEC;
   
   /* Clear data buffer */
   if ( NULL != OutputRxBuffer )
   {
     ds_ucsd_DownlinkPDUMgr_Free (&OutputRxBuffer);
   }
   
   /* main buffers are initialised in the control function */
}



/****************************************************************************
 * Function name:  decode
 * -------------
 *
 * Description:    This is called by the control function to decode data
 * -----------     from it's rx buffer
 *    
 * Returns:        -1 for error, otherwise 0
 * -------         
 ****************************************************************************/

void           v42decode()
{
   word  new_code;        /* used to store current input code          */
   word  code;            /* temporary storage of codewords            */
   word  hash_index;      /* temporary store for returned hash location*/
   byte  exit_count = 0;      /* counter to limit decoding cycles      */
   word  *tmp_ds_v42_d_prefix_ptr   = ds_v42_d_prefix_ptr   - FIRST_CODE; 
   uint8 *tmp_ds_v42_d_app_char_ptr = ds_v42_d_app_char_ptr - FIRST_CODE;

   /* check for INIT request from control function */
   if (D_INIT_REQ == V42_get_decode_control_flag())
   {
      /* Initialise decoder */
      init_decoder();
      init_tables (ds_v42_d_hash_table_ptr, ds_v42_d_prefix_table_ptr);
                  
      ds_v42_d_state = D_NORMAL;
      DS_UCSD_DEBUG_MSG("V42D: D_NORMAL");
     
      V42_set_decode_control_flag(D_CLEAR);
      V42_set_decode_status_flag(D_INIT_CNF);
   }

   /* setup output buffer */
   if (NULL == OutputRxBuffer)
   {
      OutputRxBuffer = ds_ucsd_DownlinkPDUMgr_New(); 
      if (NULL == OutputRxBuffer)
      {
        return;
      }
   }


   /* write any pending data writes not made during last execution cycle */
   if (ds_v42_d_data_pending != NULL_VALUE)
   {
      DSM_PUSHDOWN_TAIL (&OutputRxBuffer, (byte*)&ds_v42_d_data_pending, 1);
      
      /****
           if (ds_write_to_rx_buffer(ds_v42_d_data_pending))
      {
         buffer full 
         return;
      }
      ***/
      ds_v42_d_data_pending = NULL_VALUE;
   }

   /* check for resume of a codeword decoding */
   else if (ds_v42_d_decode_pending_flag)
   {
      /* output string */
      while ( (ds_v42_d_string_length--) && (NULL != OutputRxBuffer) )
      {
         DSM_PUSHDOWN_TAIL (&OutputRxBuffer, ds_v42_d_stack_ptr, 1);

         if (*ds_v42_d_stack_ptr-- == ds_v42_d_escape_code)
         {
            ds_v42_d_escape_code = (ds_v42_d_escape_code + 51) % 256;
         }
      }
      ds_v42_d_stack_ptr++;
      ds_v42_d_decode_pending_flag = FALSE;
   }


   /* if refill flag has been set defrag the hash table to improve
    * store and retrieval performance                             */
   if (ds_v42_d_refill_hash_flag)
   {
      refill_hash
         (
            V42_get_max_codewords(),
            ds_v42_d_hash_ptr,   
            ds_v42_d_prefix_ptr,
            ds_v42_d_app_char_ptr
         );
         
      ds_v42_d_refill_hash_flag = FALSE;
      return;
   }


   /************************ main loop *************************/
   /* this loop will continue processing bytes or codewords until 
    * either the input buffer is empty or the max number of cycles
    * is reached, or the output buffer is full */
   while (1)   /*lint !e716 */
   {

      /* limiter for no. of cycles OR output buffer full */
      
      if (ds_v42_d_mode == COMPRESSED_MODE)
      {
         if (exit_count > MAX_DECODE_CYCLES)
         {
            break;  /* exit while loop */
         }
      }
      else // Transparent mode
      {
         if (exit_count > ds_v42_d_trans_cycle_limit)
         {
            break;  /* exit while loop */
         }
      }
      
      ++exit_count;

      /* get a codeword or byte */
      if ((new_code = input_code()) == NULL_VALUE)
      {
         /* buffer empty */
         break;  /* exit while loop */
      }

      /* test for invalid codeword */
      if (new_code > ds_v42_d_max_code)
      {
         /* D_ERROR_IND */

         DATA_MSG2_ERROR("D_ERROR_IND (code:%x max:%x)",new_code,ds_v42_d_max_code);
         
         V42_set_decode_status_flag(D_ERROR_IND);
         break;  /* exit while loop */
      }

      /*************** escape code handling **********************/

      /***************** transparent ds_v42_d_mode ***********************/

      if (ds_v42_d_mode == TRANSPARENT_MODE)
      {
         if (ds_v42_d_state == D_ESCAPE)
         {
            ds_v42_d_state = D_NORMAL;
            if (new_code == EID)
            {
               /* cycle escape */
               new_code = ds_v42_d_escape_code;
               ds_v42_d_escape_code = (ds_v42_d_escape_code + 51) % 256;

            }
            else
            {
               /* escape code sent */
               if (new_code == ECM)
               {
                  DS_UCSD_DEBUG_MSG ("Decoder: entering compressed mode");
                  ds_v42_d_mode = COMPRESSED_MODE;



                  ds_v42_d_last_string_length = ds_v42_d_string_length;
                  ds_v42_d_old_code = ds_v42_d_string_code;
                  continue;
               }
               else if (new_code == RESET)
               {
                  DS_UCSD_DEBUG_MSG ("Decoder: received reset");
                  init_tables (ds_v42_d_hash_table_ptr, ds_v42_d_prefix_table_ptr);
                  init_decoder();
                  break;  /* exit while loop */
               }
               else
               {
                  /* D_ERROR_IND */
		  ERR ("D_ERROR_IND 2 - codes: new=0x%X old=0x%X esc=0x%X",
		       new_code,ds_v42_d_old_code,ds_v42_d_escape_code);
                  V42_set_decode_status_flag(D_ERROR_IND);
                  break;  /* exit while loop */
               }
            }
         }
         else if (new_code == ds_v42_d_escape_code)
         {
            //DS_UCSD_DEBUG_MSG ("Decoder: Got escape code");
            ds_v42_d_state = D_ESCAPE;
            continue;
         }

      }


      /*********************** Compressed ds_v42_d_mode ***********************/

      if (ds_v42_d_mode == COMPRESSED_MODE)
      {
         /* check for control code (0 - 2) */
         if (new_code < 3)
         {
            
            if (new_code == STEPUP)
            {
               /* increment codeword size */
               ds_v42_d_codeword_size++;
               //DS_UCSD_DEBUG_MSG ("Decoder: Got STEPUP");
               if ( ds_v42_d_codeword_size > V42_get_max_codeword_size() )
               {
                  DATA_MSG0_ERROR("decode: stepup error - codeword size exceeded!. D_ERROR_IND 2");
                 
                  V42_set_decode_status_flag(D_ERROR_IND);
                  break;  /* exit while loop */
               }
               ds_v42_d_stepup_threshold = (word)((1 << ds_v42_d_codeword_size) - 1);
               continue;
            }


            if (new_code == FLUSH)
            {
               //DS_UCSD_DEBUG_MSG ("Decoder: received flush in compressed mode");
               ds_v42_d_input_bit_count = 0;
               ds_v42_d_input_bit_buffer = 0;
               continue;
            }


            if (new_code == ETM)
            {
               /* ENTER transparent ds_v42_d_mode */
               DS_UCSD_DEBUG_MSG ("Decoder: entering transparent mode");
               ds_v42_d_mode = TRANSPARENT_MODE;
               ds_v42_d_input_bit_count = 0;   /* reset input buffer
                                       * (cut off 0 padding) */
               ds_v42_d_input_bit_buffer = 0;

               ds_v42_d_string_code = ds_v42_d_old_code;
               ds_v42_d_post_ETM_flag = TRUE;
               continue;
            }
         }


         /* remove offset of asci codes */
         if ((new_code > 2) && (new_code < FIRST_CODE))
         {
            if (ds_v42_d_mode == COMPRESSED_MODE)
            {
               new_code -= 3;
            }
         }
      }



      /******************* Decode section ***********************/



      /******************* Compresed ds_v42_d_mode ***********************
       * in this section the codewords are decoded and the bytes 
       * they represent are outputted                             */

      if (ds_v42_d_mode == COMPRESSED_MODE)
      {
         code = new_code;
         ds_v42_d_string_length = 1;

         /* check for invalid codeword */
         if (code >= FIRST_CODE)
         {

            if (tmp_ds_v42_d_prefix_ptr[code] == V42BIS_UNUSED_MASK)
            {    
               DATA_MSG1_ERROR("V42error:code 0x%X not found in dictionary", code);
               V42_set_decode_status_flag( (byte)D_ERROR_IND );
               break;  /* exit while loop */
            }
         }
         

         /* decode codeword using codetable */
         while (code >= FIRST_CODE)
         {
            *ds_v42_d_stack_ptr++ = tmp_ds_v42_d_app_char_ptr[code];
            code = tmp_ds_v42_d_prefix_ptr[code];
            ds_v42_d_string_length++;

            /* check for invalid codetable entry */
            if ((code > ds_v42_d_max_code) ||
                 (ds_v42_d_string_length > V42_get_max_string_length()) )
            {
              ERR ("V42: D_ERROR_IND 5 %d %d %d",
                   code, ds_v42_d_max_code,ds_v42_d_string_length);
              V42_set_decode_status_flag(D_ERROR_IND);
              break;  /* exit inner while loop */
            }

         }
         if ( D_ERROR_IND == V42_get_decode_status_flag() )
         {
            break;  /* exit while loop */
         }
         
         /* put first byte of the codeword string on the ds_v42_d_stack */
         *ds_v42_d_stack_ptr = (byte) code;
         /* store the first byte of the codeword */ 
         ds_v42_d_character = (byte) code;



         /* ensure ds_v42_d_old_code is not empty as is after an INIT */
         if (ds_v42_d_old_code != V42BIS_UNUSED_MASK)
         {
            /*
             * Add last character to last codeword
             * processed
             */
            if ( ds_v42_d_last_string_length < V42_get_max_string_length() )
            {
               hash_index = find_match
                                 (
                                    ds_v42_d_old_code,
                                    ds_v42_d_character,
                                    ds_v42_d_hash_ptr,
                                    ds_v42_d_prefix_ptr,
                                    ds_v42_d_app_char_ptr,
                                    &ds_v42_d_refill_hash_flag,
                                    &ds_v42_d_search_success
                                 );
                                 
               /* only add if doesn't already exist */
               if (!ds_v42_d_search_success)
               {
                  ds_v42_d_last_code = ds_v42_d_next_code;
                  //DS_UCSD_DEBUG_MSG_3("Decode: comp-Add code %d, for code/char %d/%c",
                  //                                   ds_v42_d_next_code,
                  //                                   ds_v42_d_old_code,
                  //                                   ds_v42_d_character);
                  add_string
                     (
                        hash_index, 
                        ds_v42_d_old_code, 
                        ds_v42_d_character,
                        ds_v42_d_max_code,
                        &ds_v42_d_next_code,
                        ds_v42_d_hash_ptr,   
                        ds_v42_d_prefix_ptr,
                        ds_v42_d_app_char_ptr,
                        ds_v42_d_leaf_count_ptr 
                     );
               }
               else
               {
                  //DS_UCSD_DEBUG_MSG_2("Decode: comp-match for code/char %d/%c" ,ds_v42_d_old_code,
                  //                                   ds_v42_d_character);
               }
            }
         }

         /* store length of current string */
         ds_v42_d_last_string_length = ds_v42_d_string_length;

         /* store current codeword for use on next iteration */
         ds_v42_d_old_code = new_code;



         /* pop bytes off the codeword ds_v42_d_stack and output to buffer */
         while (ds_v42_d_string_length--)
         {
            ASSERT( NULL != OutputRxBuffer);
            DSM_PUSHDOWN_TAIL (&OutputRxBuffer, ds_v42_d_stack_ptr, 1);
            
            /* check for escape code in string */
            if (*ds_v42_d_stack_ptr-- == ds_v42_d_escape_code)
            {
               ds_v42_d_escape_code = (ds_v42_d_escape_code + 51) % 256;
            }
         }
         ds_v42_d_stack_ptr++;
      }

      /******************* Transparent ds_v42_d_mode ***********************/

      else
      {
         /* bytes must be processed slightly differently following 
          * entering transparent ds_v42_d_mode                               */
         if (ds_v42_d_post_ETM_flag)
         {
            ds_v42_d_post_ETM_flag = FALSE;
            
            /* search for code character combination */
            hash_index = find_match
                                 (
                                    ds_v42_d_string_code,
                                    new_code,
                                    ds_v42_d_hash_ptr,
                                    ds_v42_d_prefix_ptr,
                                    ds_v42_d_app_char_ptr,
                                    &ds_v42_d_refill_hash_flag,
                                    &ds_v42_d_search_success
                                 );

            /* if not found then add to codetable */
            if (!ds_v42_d_search_success)
            {
               if ( ds_v42_d_last_string_length < V42_get_max_string_length() )
               {
                  //DS_UCSD_DEBUG_MSG_3("Decode: trans-Add code %d, for code/char %d/%c",
                  //                                   ds_v42_d_next_code,
                  //                                   ds_v42_d_string_code,
                  //                                   new_code);
                  ds_v42_d_last_code = ds_v42_d_next_code;
                 
                  add_string
                     (
                        hash_index, 
                        ds_v42_d_string_code, 
                        new_code,
                        ds_v42_d_max_code,
                        &ds_v42_d_next_code,
                        ds_v42_d_hash_ptr,   
                        ds_v42_d_prefix_ptr,
                        ds_v42_d_app_char_ptr,
                        ds_v42_d_leaf_count_ptr 
                     );
               }
            }
            else
            {
               //DS_UCSD_DEBUG_MSG_2("Decode: trans-match for code/char %d/%c" ,ds_v42_d_string_code,
               //                                      new_code);
            }
            ds_v42_d_string_code = new_code;
            ds_v42_d_string_length = 1;
            /* output byte to tx buffer */
            ASSERT( NULL != OutputRxBuffer);
            DSM_PUSHDOWN_TAIL (&OutputRxBuffer, &new_code, 1);
            continue;
         }


         /* cast new_code to byte */
         ds_v42_d_character = (byte)new_code;

         /*
          * Detect first character condition this occurs after
          * INIT and RESET
          */
         if (ds_v42_d_string_code == V42BIS_UNUSED_MASK)
         {
            ds_v42_d_string_code = ds_v42_d_character;
            ds_v42_d_string_length = 1;
            ASSERT( NULL != OutputRxBuffer);
            DSM_PUSHDOWN_TAIL (&OutputRxBuffer, &ds_v42_d_character, 1);
            continue;
         }

         /* search codetable for current codeword and character */
         hash_index = find_match
                           (
                              ds_v42_d_string_code,
                              ds_v42_d_character,
                              ds_v42_d_hash_ptr,
                              ds_v42_d_prefix_ptr,
                              ds_v42_d_app_char_ptr,
                              &ds_v42_d_refill_hash_flag,
                              &ds_v42_d_search_success
                           );
                      


         if (ds_v42_d_search_success)   /* match found */
         {
            //DS_UCSD_DEBUG_MSG_2("Decode-trans-found match for %d/%c",ds_v42_d_string_code,ds_v42_d_character);
            /* test for use of last code added */
            if ((ds_v42_d_hash_ptr[hash_index]) == ds_v42_d_last_code)
            {
               /* if so terminate string matching */
               
               ds_v42_d_string_code = ds_v42_d_character;
               ds_v42_d_string_length = 1;
               //DS_UCSD_DEBUG_MSG("Decode: trans-found last string");
               ds_v42_d_last_code = V42BIS_UNUSED_MASK;

            }
            else   /* match ok */
            {
               /* increase matched string length */
               ds_v42_d_string_code = (ds_v42_d_hash_ptr[hash_index]);
               ds_v42_d_string_length++;
            }

         }

         else   /* no match, add to dictionary */
         {
            /* check length is within the negotiated maximum */
            if ( ds_v42_d_string_length < V42_get_max_string_length() )
            {
               /* store last code added */
               ds_v42_d_last_code = ds_v42_d_next_code;
               /* add string to codetable */
               //DS_UCSD_DEBUG_MSG_3("Decode-trans no match, add %d for %d/%c", ds_v42_d_next_code, ds_v42_d_string_code ,ds_v42_d_character);
               add_string
                     (
                        hash_index, 
                        ds_v42_d_string_code, 
                        ds_v42_d_character,
                        ds_v42_d_max_code,
                        &ds_v42_d_next_code,
                        ds_v42_d_hash_ptr,   
                        ds_v42_d_prefix_ptr,
                        ds_v42_d_app_char_ptr,
                        ds_v42_d_leaf_count_ptr 
                     );
            }
            /* reset string */
            ds_v42_d_string_code = ds_v42_d_character;
            ds_v42_d_string_length = 1;
         }
         ASSERT( NULL != OutputRxBuffer);
         /* output byte to buffer */
         DSM_PUSHDOWN_TAIL (&OutputRxBuffer, &ds_v42_d_character, 1);
      }

   }         /* end of while */

   /* send output buffer to downstream stage */
   ds_ucsd_V24_interface_driver_byte_oriented(&OutputRxBuffer);
}







/****************************************************************************
 *
 * Function name:  input code
 * -------------
 *
 * Description:    reads a code from the input buffer
 * -----------     
 *    
 * Returns:        codeword or NULL_VALUE if buffer is empty
 * -------         
 ****************************************************************************/

word            input_code(void)
{
   word            return_value = 0;
   byte            character;
   byte            RxWord;

   if (ds_check_empty_V42_rx_buffer ())
   {
      return_value = NULL_VALUE;
   }
   else
   {
      if (ds_v42_d_mode == TRANSPARENT_MODE)
      {
         /* read byte from buffer */
         (void)ds_read_from_V42_rx_buffer(&RxWord);
         character = RxWord;

         return_value = character;
      }
      else   /* compressed ds_v42_d_mode */
      {
         /* read in bytes until there are at least enough bits in the bit buffer
          * to make a codeword
                                                        */
         while (ds_v42_d_input_bit_count < ds_v42_d_codeword_size)
         {
            /* read byte from buffer */
            if (ds_check_empty_V42_rx_buffer ())
            {
               /* buffer empty */
               return_value = NULL_VALUE;
               break;
            }
            else
            {
               (void)ds_read_from_V42_rx_buffer(&RxWord);
               character = RxWord;
               /* add byte to bit buffer */
               ds_v42_d_input_bit_buffer |=
                  (unsigned long) character << (ds_v42_d_input_bit_count);
               /* increment count of bits in the buffer */
               //DS_UCSD_DEBUG_MSG_3("bit_count=%d,char=%x, buf=%x", ds_v42_d_input_bit_count, RxWord,ds_v42_d_input_bit_buffer);
               ds_v42_d_input_bit_count += 8;
            }
         }
         
         if (return_value != NULL_VALUE)
         {
            /* mask codeword size bits to get inputted codeword */
            return_value = (word) (ds_v42_d_input_bit_buffer & ds_v42_d_stepup_threshold);
            /* shift bits out of bit buffer */
            ds_v42_d_input_bit_buffer >>= ds_v42_d_codeword_size;
            /* decrease input bit buffer count of bits */
            ds_v42_d_input_bit_count -= ds_v42_d_codeword_size;
         }
      }
   }
   
   //if (return_value != NULL_VALUE)
   //   DS_UCSD_DEBUG_MSG_1 ("decoder->input code is %d", return_value);

   return (return_value);
}




#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */

