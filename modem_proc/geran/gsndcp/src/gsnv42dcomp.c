/*===========================================================================

              GPRS V42 bis Data compression Module

DESCRIPTION
  This file is part of the implementation of the GPRS V42 bis Data compression.
  The file is a modified version of the dsgcsdv42d.c used in
  GSM Circuit Switched V42 bis implementation.

Copyright (c) 1991 - 2013 Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42d.c_v   1.2   12 Feb 2002 16:53:22   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsnv42dcomp.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03-07-16   tjw     Prevent compiler warning C3040E: no external declaration in translation unit
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
#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

#include "comdef.h"
#include "gsni.h"
#include "gsnv42.h"
#include "err.h"




/************************** Globals ***********************************/

/* various decoder states possible */
enum
{
   D_WAIT_FOR_DINIT,
   D_INIT_DECODE,
   D_ESCAPE,
   D_NORMAL
};


/* Forward prototypes */
word     gsn_v42_input_code(comp_ent_V42bis_t *);


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

void gsn_v42_init_decoder(comp_ent_V42bis_t *comp_e)
{
   comp_e->ds_v42_d_state = D_INIT_DECODE;
   comp_e->ds_v42_d_post_ETM_flag = FALSE;
   comp_e->ds_v42_d_stack_ptr = &comp_e->ds_v42_d_stack[0];
   comp_e->ds_v42_d_string_length = 1;
   comp_e->ds_v42_d_last_code = V42BIS_DELETION_MASK;
   comp_e->ds_v42_d_refill_hash_flag = FALSE;
   comp_e->ds_v42_d_mode = TRANSPARENT_MODE;
   comp_e->ds_v42_d_codeword_size = INITIAL_CODEWORD_SIZE;
   comp_e->ds_v42_d_max_code = (comp_e->ds_v42_max_codewords - 1);
   comp_e->ds_v42_d_stepup_threshold = ((1 << INITIAL_CODEWORD_SIZE) - 1);
   comp_e->ds_v42_d_escape_code = 0;
   comp_e->ds_v42_d_next_code = FIRST_CODE;  /* next available dictionary location */
   comp_e->ds_v42_d_stack_ptr = &comp_e->ds_v42_d_stack[0];
   comp_e->ds_v42_d_decode_pending_flag = FALSE;
   comp_e->ds_v42_d_data_pending = NULL_VALUE;
   comp_e->ds_v42_d_old_code = V42BIS_UNUSED_MASK;
   comp_e->ds_v42_d_string_code = V42BIS_UNUSED_MASK;
   comp_e->ds_v42_d_last_string_length = 1;
   comp_e->ds_v42_d_input_bit_count = 0;
   comp_e->ds_v42_d_input_bit_buffer = 0;
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

void gsn_v42_decode(comp_ent_V42bis_t *comp_e)
{
   word            new_code;     /* used to store current input code          */
   word            code;         /* temporary storage of codewords            */
   word            hash_index;   /* temporary store for returned hash location*/

   switch (comp_e->ds_v42_d_state)
   {
      case D_INIT_DECODE:

         gsn_v42_init_decoder( comp_e );
         comp_e->ds_v42_d_state = D_WAIT_FOR_DINIT;

      /* .. Drop through deliberately.
      ** .. No break statement
      */
      /*lint -fallthrough */

      case D_WAIT_FOR_DINIT:

         if (comp_e->ds_v42_decode_control_flag != D_INIT_REQ)
         {
            return;
         }
         else
         {
            gsn_v42_init_tables(
                                comp_e->ds_v42_d_hash_table,
                                comp_e->ds_v42_d_prefix_table,
                                comp_e->ds_v42_max_codewords,
                                comp_e->ds_v42_max_codewords
                               );

            comp_e->ds_v42_decode_control_flag = D_CLEAR;
            comp_e->ds_v42_decode_status_flag = D_INIT_CNF;
            comp_e->ds_v42_d_state = D_NORMAL;
         }
         break;

      case D_NORMAL:
         break;

      default:
         break;
   }

   /* check for INIT request from control function */
   if (comp_e->ds_v42_decode_control_flag == D_INIT_REQ)
   {
      /* re-initialise decoder */
      gsn_v42_init_decoder( comp_e );
      gsn_v42_init_tables(
                          comp_e->ds_v42_d_hash_table,
                          comp_e->ds_v42_d_prefix_table,
                          comp_e->ds_v42_max_codewords,
                          comp_e->ds_v42_max_codewords
                         );

      comp_e->ds_v42_decode_control_flag = D_CLEAR;
      comp_e->ds_v42_decode_status_flag = D_INIT_CNF;
   }


   /* write any pending data writes not made during last execution cycle */
   if (comp_e->ds_v42_d_data_pending != NULL_VALUE)
   {
      gsn_v42_write_to_rx_buffer(comp_e->ds_v42_d_data_pending, comp_e);

      /****
           if (ds_write_to_rx_buffer(comp_e->ds_v42_d_data_pending))
      {
         buffer full
         return;
      }
      ***/
      comp_e->ds_v42_d_data_pending = NULL_VALUE;
   }

   /* check for resume of a codeword decoding */
   else if (comp_e->ds_v42_d_decode_pending_flag)
   {
      /* output string */
      while (comp_e->ds_v42_d_string_length--)
      {

         gsn_v42_write_to_rx_buffer(*comp_e->ds_v42_d_stack_ptr, comp_e);

         if (*comp_e->ds_v42_d_stack_ptr-- == comp_e->ds_v42_d_escape_code)
         {
            comp_e->ds_v42_d_escape_code = (comp_e->ds_v42_d_escape_code + 51) % 256;
         }


      }
      comp_e->ds_v42_d_stack_ptr++;
      comp_e->ds_v42_d_decode_pending_flag = FALSE;
   }


   /* if refill flag has been set defrag the hash table to improve
    * store and retrieval performance                             */
   if (comp_e->ds_v42_d_refill_hash_flag)
   {
      gsn_v42_refill_hash
         (
            comp_e->ds_v42_max_codewords,
            comp_e->ds_v42_d_hash_ptr,
            comp_e->ds_v42_d_prefix_ptr,
            comp_e->ds_v42_d_app_char_ptr,
            comp_e->ds_v42_max_codewords
         );

      comp_e->ds_v42_d_refill_hash_flag = FALSE;
   }



   /************************ main loop *************************/
   /* this loop will continue processing bytes or codewords until
    * either the input buffer is empty or the max number of cycles
    * is reached, or the output buffer is full */
/*lint -e(716) Info -- while(1) ... */
   while (1)
   {

      /* limiter for no. of cycles OR output buffer full */
      if ( gsn_v42_check_full_rx_buffer(comp_e) )
      {
         return;
      }

      /* get a codeword or byte */
      if ((new_code = gsn_v42_input_code( comp_e )) == NULL_VALUE)
      {
         /* buffer empty */
         return;
      }

      /* test for invalid codeword */
      if (new_code > comp_e->ds_v42_d_max_code)
      {
         /* D_ERROR_IND */

         ERR ("D_ERROR_IND (code:%x max:%x)\n",new_code,comp_e->ds_v42_d_max_code,0);

         comp_e->ds_v42_decode_status_flag = D_ERROR_IND;
         return;
      }

      /*************** escape code handling **********************/

      /***************** transparent ds_v42_d_mode ***********************/

      if (comp_e->ds_v42_d_mode == TRANSPARENT_MODE)
      {


         if (comp_e->ds_v42_d_state == D_ESCAPE)
         {
            comp_e->ds_v42_d_state = D_NORMAL;
            if (new_code == EID)
            {
               /* cycle escape */
               new_code = comp_e->ds_v42_d_escape_code;
               comp_e->ds_v42_d_escape_code = (comp_e->ds_v42_d_escape_code + 51) % 256;

            }
            else
            {
               /* escape code sent */
               if (new_code == ECM)
               {
                  MSG_GERAN_LOW_0("GSN ""Decoder: entering compressed mode");
                  comp_e->ds_v42_d_mode = COMPRESSED_MODE;



                  comp_e->ds_v42_d_last_string_length = comp_e->ds_v42_d_string_length;
                  comp_e->ds_v42_d_old_code = comp_e->ds_v42_d_string_code;
                  continue;
               }
               else if (new_code == V42_RESET)
               {
                  MSG_GERAN_LOW_0("GSN ""Decoder: received reset");
                  gsn_v42_init_tables(
                                      comp_e->ds_v42_d_hash_table,
                                      comp_e->ds_v42_d_prefix_table,
                                      comp_e->ds_v42_max_codewords,
                                      comp_e->ds_v42_max_codewords
                                     );
                  gsn_v42_init_decoder( comp_e );
                  return;
               }
               else
               {
                  /* D_ERROR_IND */
                  MSG_GERAN_ERROR_0("GSN ""D_ERROR_IND 2");
                  comp_e->ds_v42_decode_status_flag = D_ERROR_IND;
                  return;
               }
            }
         }
         else if (new_code == comp_e->ds_v42_d_escape_code)
         {
            comp_e->ds_v42_d_state = D_ESCAPE;
            continue;
         }

      }


      /*********************** Compressed ds_v42_d_mode ***********************/

      if (comp_e->ds_v42_d_mode == COMPRESSED_MODE)
      {
         /* check for control code (0 - 2) */
         if (new_code < 3)
         {

            if (new_code == STEPUP)
            {
               /* increment codeword size */
               comp_e->ds_v42_d_codeword_size++;
               if (comp_e->ds_v42_d_codeword_size > comp_e->ds_v42_max_codeword_size)
               {
                  MSG_GERAN_ERROR_0("GSN ""decode stepup error - codeword size exceeded!");
                  MSG_GERAN_ERROR_0("GSN ""D_ERROR_IND #2");

                  comp_e->ds_v42_decode_status_flag = D_ERROR_IND;
                  return;
               }
               comp_e->ds_v42_d_stepup_threshold = (word)((1 << comp_e->ds_v42_d_codeword_size) - 1);
               continue;
            }


            if (new_code == FLUSH)
            {
               comp_e->ds_v42_d_input_bit_count = 0;
               comp_e->ds_v42_d_input_bit_buffer = 0;
               continue;
            }


            if (new_code == ETM)
            {
               /* ENTER transparent ds_v42_d_mode */
               MSG_GERAN_LOW_0("GSN ""Decoder: entering transparent mode");
               comp_e->ds_v42_d_mode = TRANSPARENT_MODE;
               comp_e->ds_v42_d_input_bit_count = 0;   /* reset input buffer
                                       * (cut off 0 padding) */
               comp_e->ds_v42_d_input_bit_buffer = 0;


               comp_e->ds_v42_d_string_code = comp_e->ds_v42_d_old_code;
               comp_e->ds_v42_d_post_ETM_flag = TRUE;
               continue;
            }

         }


         /* remove offset of asci codes */
         if ((new_code > 2) && (new_code < FIRST_CODE))
         {
            if (comp_e->ds_v42_d_mode == COMPRESSED_MODE)
            {
               new_code -= 3;
            }
         }



      }





      /******************* Decode section ***********************/



      /******************* Compresed ds_v42_d_mode ***********************
       * in this section the codewords are decoded and the bytes
       * they represent are outputted                             */

      if (comp_e->ds_v42_d_mode == COMPRESSED_MODE)
      {
         code = new_code;
         comp_e->ds_v42_d_string_length = 1;

         /* check for invalid codeword */
         if (code >= FIRST_CODE)
         {

            if (comp_e->ds_v42_d_prefix_ptr[code] == V42BIS_UNUSED_MASK)
            {
               MSG_GERAN_ERROR_1("GSN ""V42 code %d not found in dictionary", code);
               comp_e->ds_v42_decode_status_flag = D_ERROR_IND;
               return;
            }
         }


         /* decode codeword using codetable */
         while (code >= FIRST_CODE)
         {
            *comp_e->ds_v42_d_stack_ptr++ = comp_e->ds_v42_d_app_char_ptr[code];
            code = comp_e->ds_v42_d_prefix_ptr[code];
            comp_e->ds_v42_d_string_length++;

            /* check for invalid codetable entry */
            if ((code > comp_e->ds_v42_d_max_code) || (comp_e->ds_v42_d_string_length > comp_e->ds_v42_max_string_length))
            {

               MSG_GERAN_ERROR_3("GSN ""V42: D_ERROR_IND 5 %d %d %d",code, comp_e->ds_v42_d_max_code,comp_e->ds_v42_d_string_length);
               comp_e->ds_v42_decode_status_flag = D_ERROR_IND;
               return;
            }

         }

         /* put first byte of the codeword string on the ds_v42_d_stack */
         *comp_e->ds_v42_d_stack_ptr = (byte) code;
         /* store the first byte of the codeword */
         comp_e->ds_v42_d_character = code;



         /* ensure ds_v42_d_old_code is not empty as is after an INIT */
         if (comp_e->ds_v42_d_old_code != V42BIS_UNUSED_MASK)
         {
            /*
             * Add last character to last codeword
             * processed
             */
            if ( comp_e->ds_v42_d_last_string_length < comp_e->ds_v42_max_string_length )
            {
               hash_index = gsn_v42_find_match
                                 (
                                    comp_e->ds_v42_d_old_code,
                                    comp_e->ds_v42_d_character,
                                    comp_e->ds_v42_d_hash_ptr,
                                    comp_e->ds_v42_d_prefix_ptr,
                                    comp_e->ds_v42_d_app_char_ptr,
                                    &comp_e->ds_v42_d_refill_hash_flag,
                                    &comp_e->ds_v42_d_search_success,
                                    comp_e->ds_v42_max_codewords
                                 );

               /* only add if doesn't already exist */
               if (!comp_e->ds_v42_d_search_success)
               {
                  comp_e->ds_v42_d_last_code = comp_e->ds_v42_d_next_code;
                  //GCSD_DEBUG_MSG_3("Decode: comp-Add code %d, for code/char %d/%c",
                  //                                   comp_e->ds_v42_d_next_code,
                  //                                   comp_e->ds_v42_d_old_code,
                  //                                   comp_e->ds_v42_d_character);
                  gsn_v42_add_string
                     (
                        hash_index,
                        comp_e->ds_v42_d_old_code,
                        comp_e->ds_v42_d_character,
                        comp_e->ds_v42_d_max_code,
                        &comp_e->ds_v42_d_next_code,
                        comp_e->ds_v42_d_hash_ptr,
                        comp_e->ds_v42_d_prefix_ptr,
                        comp_e->ds_v42_d_app_char_ptr,
                        comp_e->ds_v42_d_leaf_count_ptr,
                        comp_e->ds_v42_max_codewords
                     );
               }
               else
               {
                  //GCSD_DEBUG_MSG_2("Decode: comp-match for code/char %d/%c" ,comp_e->ds_v42_d_old_code,
                  //                                   comp_e->ds_v42_d_character);
               }
            }
         }

         /* store length of current string */
         comp_e->ds_v42_d_last_string_length = comp_e->ds_v42_d_string_length;

         /* store current codeword for use on next iteration */
         comp_e->ds_v42_d_old_code = new_code;



         /* pop bytes off the codeword ds_v42_d_stack and output to buffer */
         while (comp_e->ds_v42_d_string_length--)
         {

            gsn_v42_write_to_rx_buffer(*comp_e->ds_v42_d_stack_ptr, comp_e);

            /* check for escape code in string */
            if (*comp_e->ds_v42_d_stack_ptr-- == comp_e->ds_v42_d_escape_code)
            {
               comp_e->ds_v42_d_escape_code = (comp_e->ds_v42_d_escape_code + 51) % 256;
            }


         }
         comp_e->ds_v42_d_stack_ptr++;


      }

      /******************* Transparent ds_v42_d_mode ***********************/

      else
      {
         /* bytes must be processed slightly differently following
          * entering transparent ds_v42_d_mode                               */
         if (comp_e->ds_v42_d_post_ETM_flag)
         {
            comp_e->ds_v42_d_post_ETM_flag = FALSE;

            /* search for code character combination */
            hash_index = gsn_v42_find_match
                                 (
                                    comp_e->ds_v42_d_string_code,
                                    new_code,
                                    comp_e->ds_v42_d_hash_ptr,
                                    comp_e->ds_v42_d_prefix_ptr,
                                    comp_e->ds_v42_d_app_char_ptr,
                                    &comp_e->ds_v42_d_refill_hash_flag,
                                    &comp_e->ds_v42_d_search_success,
                                    comp_e->ds_v42_max_codewords
                                 );

            /* if not found then add to codetable */
            if (!comp_e->ds_v42_d_search_success)
            {
               if (comp_e->ds_v42_d_last_string_length < comp_e->ds_v42_max_string_length)
               {
                  //GCSD_DEBUG_MSG_3("Decode: trans-Add code %d, for code/char %d/%c",
                  //                                   comp_e->ds_v42_d_next_code,
                  //                                   comp_e->ds_v42_d_string_code,
                  //                                   new_code);
                  comp_e->ds_v42_d_last_code = comp_e->ds_v42_d_next_code;

                  gsn_v42_add_string
                     (
                        hash_index,
                        comp_e->ds_v42_d_string_code,
                        new_code,
                        comp_e->ds_v42_d_max_code,
                        &comp_e->ds_v42_d_next_code,
                        comp_e->ds_v42_d_hash_ptr,
                        comp_e->ds_v42_d_prefix_ptr,
                        comp_e->ds_v42_d_app_char_ptr,
                        comp_e->ds_v42_d_leaf_count_ptr,
                        comp_e->ds_v42_max_codewords
                     );
                }

            }

            comp_e->ds_v42_d_string_code = new_code;
            comp_e->ds_v42_d_string_length = 1;
            /* output byte to tx buffer */

            gsn_v42_write_to_rx_buffer(new_code, comp_e);
            continue;
         }


         /* cast new_code to byte */
         comp_e->ds_v42_d_character = (byte)new_code;

         /*
          * Detect first character condition this occurs after
          * INIT and RESET
          */
         if (comp_e->ds_v42_d_string_code == V42BIS_UNUSED_MASK)
         {
            comp_e->ds_v42_d_string_code = comp_e->ds_v42_d_character;
            comp_e->ds_v42_d_string_length = 1;


            gsn_v42_write_to_rx_buffer(comp_e->ds_v42_d_character, comp_e);
            continue;
         }

         /* search codetable for current codeword and character */
         hash_index = gsn_v42_find_match
                           (
                              comp_e->ds_v42_d_string_code,
                              comp_e->ds_v42_d_character,
                              comp_e->ds_v42_d_hash_ptr,
                              comp_e->ds_v42_d_prefix_ptr,
                              comp_e->ds_v42_d_app_char_ptr,
                              &comp_e->ds_v42_d_refill_hash_flag,
                              &comp_e->ds_v42_d_search_success,
                              comp_e->ds_v42_max_codewords
                           );



         if (comp_e->ds_v42_d_search_success)   /* match found */
         {
            /* test for use of last code added */
            if ((comp_e->ds_v42_d_hash_ptr[hash_index]) == comp_e->ds_v42_d_last_code)
            {
               /* if so terminate string matching */
               comp_e->ds_v42_d_string_code = comp_e->ds_v42_d_character;
               comp_e->ds_v42_d_string_length = 1;
               //GCSD_DEBUG_MSG("Decode: trans-found last string");
               comp_e->ds_v42_d_last_code = V42BIS_UNUSED_MASK;

            }
            else   /* match ok */
            {
               /* increase matched string length */
               comp_e->ds_v42_d_string_code = (comp_e->ds_v42_d_hash_ptr[hash_index]);
               comp_e->ds_v42_d_string_length++;
            }

         }

         else   /* no match, add to dictionary */
         {
            /* check length is within the negotiated maximum */
            if (comp_e->ds_v42_d_string_length < comp_e->ds_v42_max_string_length)
            {
               /* store last code added */
               comp_e->ds_v42_d_last_code = comp_e->ds_v42_d_next_code;
               /* add string to codetable */
               //GCSD_DEBUG_MSG_3("Decode-trans no match, add %d for %d/%c", comp_e->ds_v42_d_next_code, comp_e->ds_v42_d_string_code ,comp_e->ds_v42_d_character);
               gsn_v42_add_string
                     (
                        hash_index,
                        comp_e->ds_v42_d_string_code,
                        comp_e->ds_v42_d_character,
                        comp_e->ds_v42_d_max_code,
                        &comp_e->ds_v42_d_next_code,
                        comp_e->ds_v42_d_hash_ptr,
                        comp_e->ds_v42_d_prefix_ptr,
                        comp_e->ds_v42_d_app_char_ptr,
                        comp_e->ds_v42_d_leaf_count_ptr,
                        comp_e->ds_v42_max_codewords
                     );
            }
            /* reset string */
            comp_e->ds_v42_d_string_code = comp_e->ds_v42_d_character;
            comp_e->ds_v42_d_string_length = 1;
         }

         /* output byte to buffer */

         gsn_v42_write_to_rx_buffer(comp_e->ds_v42_d_character, comp_e);
      }
   }/* end of while */
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

word  gsn_v42_input_code(comp_ent_V42bis_t *comp_e)
{
   word            return_value = 0;
   byte            character;
   word            RxWord;

   if (gsn_check_empty_V42_rx_buffer (comp_e))
   {
      return_value = NULL_VALUE;
   }
   else
   {
      if (comp_e->ds_v42_d_mode == TRANSPARENT_MODE)
      {
         /* read byte from buffer */
         (void)gsn_read_from_V42_rx_buffer(&RxWord, comp_e);
         character = (byte)RxWord;

         return_value = character;
      }
      else   /* compressed ds_v42_d_mode */
      {
         /* read in bytes until there are at least enough bits in the bit buffer
          * to make a codeword                                               */
         while (comp_e->ds_v42_d_input_bit_count < comp_e->ds_v42_d_codeword_size)
         {
            /* read byte from buffer */
            if (gsn_check_empty_V42_rx_buffer (comp_e))
            {
               /* buffer empty */
               return_value = NULL_VALUE;
               break;
            }
            else
            {
               (void)gsn_read_from_V42_rx_buffer(&RxWord, comp_e);
               character = (byte)RxWord;

               /* add byte to bit buffer */
               comp_e->ds_v42_d_input_bit_buffer |=
                  (unsigned long) character << (comp_e->ds_v42_d_input_bit_count);
               /* increment count of bits in the buffer */
               comp_e->ds_v42_d_input_bit_count += 8;
            }
         }

         if (return_value != NULL_VALUE)
         {
            /* mask codeword size bits to get inputted codeword */
            return_value = (word) (comp_e->ds_v42_d_input_bit_buffer & comp_e->ds_v42_d_stepup_threshold);
            /* shift bits out of bit buffer */
            comp_e->ds_v42_d_input_bit_buffer >>= comp_e->ds_v42_d_codeword_size;
            /* decrease input bit buffer count of bits */
            comp_e->ds_v42_d_input_bit_count -= comp_e->ds_v42_d_codeword_size;
         }
      }
   }

   /* MSG_GERAN_LOW_1("GSN ""decoder input code is %x", return_value);   */

   return (return_value);
}

#else
void feature_gsm_gprs_dcomp_dummy(void) {}    /* To prevent compiler warning C3040E: no external declaration in translation unit */
#endif /* FEATURE_GSM_GPRS */
