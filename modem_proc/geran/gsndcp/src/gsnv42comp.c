
/*===========================================================================

              GPRS V42 bis Data compression Module

DESCRIPTION
  This file is part of the implementation of the GPRS V42 bis Data compression.
  The file is a modified version of the dsgcsdv42c.c used in
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

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42c.c_v   1.3   21 Mar 2002 10:58:10   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsnv42comp.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

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
#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

#include "comdef.h"
#include "gsni.h"
#include "gsnv42.h"
#include "err.h"
#include "gsndef.h"



/**************************** Globals **********************************/

/* various states possible */
enum
{
   C_WAIT_FOR_CINIT,
   C_INIT_COMPRESS,
   C_NORMAL

};

/* Forward prototypes */
void     gsn_v42_exception_process_next_char( comp_ent_V42bis_t * );
void     gsn_v42_reset(comp_ent_V42bis_t *);
void     gsn_v42_test_compression(comp_ent_V42bis_t *);
void     gsn_v42_output_byte(byte, comp_ent_V42bis_t *);
void     gsn_v42_output_code(word, comp_ent_V42bis_t *);
void     gsn_v42_stepup(comp_ent_V42bis_t *);



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

void gsn_init_compress(comp_ent_V42bis_t *comp_e)
{

   comp_e->ds_v42_c_state = C_INIT_COMPRESS;   /* current state */
   comp_e->ds_v42_c_control_code_flag = FALSE;
   comp_e->ds_v42_c_next_code = FIRST_CODE;    /* 259, 256 + 3 control codes */
   comp_e->ds_v42_c_last_code = V42BIS_DELETION_MASK; /* indicating null */
   comp_e->ds_v42_c_string_length = 1;
   comp_e->ds_v42_c_codeword_size = INITIAL_CODEWORD_SIZE;
   comp_e->ds_v42_c_mode = TRANSPARENT_MODE;  /* always starts in transparent ds_v42_c_mode */
   comp_e->ds_v42_c_stepup_threshold = (word)((1 << comp_e->ds_v42_c_codeword_size) - 1);
   comp_e->ds_v42_c_max_code = comp_e->ds_v42_max_codewords - 1;
   comp_e->ds_v42_c_escape_code = 0;

   /* init compression test variables */
   comp_e->ds_v42_c_datain_count = (word) (RETEST_THRESHOLD / 1.2);
   comp_e->ds_v42_c_dataout_count = 0;
   comp_e->ds_v42_c_string_matches = (word) (comp_e->ds_v42_c_datain_count / 1.5);

   comp_e->ds_v42_c_string_code = V42BIS_UNUSED_MASK;
   comp_e->ds_v42_c_exception_char_next_flag = FALSE;
   comp_e->ds_v42_c_refill_hash_flag = FALSE;
   comp_e->ds_v42_c_output_bit_buffer = 0;
   comp_e->ds_v42_c_output_bit_count = 0;
   comp_e->ds_v42_c_buffer_write_error = FALSE;
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

void  gsn_v42_compress(comp_ent_V42bis_t *comp_e)
{

   register word   index;          /* temporary storage for index values */
   word            RxWord;



   switch (comp_e->ds_v42_c_state)
   {
     case C_INIT_COMPRESS:
         MSG_GERAN_LOW_0("GSN ""V42 Comp: INIT_COMPRESS");
         gsn_init_compress( comp_e );
         comp_e->ds_v42_c_state = C_WAIT_FOR_CINIT;

      /* .. Drop through deliberately.
      ** .. No break statement
      */
      /*lint -fallthrough */

      case C_WAIT_FOR_CINIT:
         MSG_GERAN_LOW_0("GSN ""V42 Comp: C_WAIT_FOR_CINIT");
         if (comp_e->ds_v42_compress_control_flag != C_INIT_REQ)
         {
            return;
         }
         else
         {
            gsn_v42_init_tables(
                                comp_e->ds_v42_c_hash_table,
                                comp_e->ds_v42_c_prefix_table,
                                comp_e->ds_v42_max_codewords,
                                comp_e->ds_v42_max_codewords
                               );

            comp_e->ds_v42_compress_control_flag = C_CLEAR;
            comp_e->ds_v42_compress_status_flag = C_INIT_CNF;
            comp_e->ds_v42_c_state = C_NORMAL;
            MSG_GERAN_LOW_0("GSN ""V42 Comp: C_NORMAL");
         }
         break;

      case C_NORMAL:
         break;

      default:
          MSG_GERAN_MED_1("GSN ""V42 Comp: state=%d",comp_e->ds_v42_c_state);
         break;
   }


   /* check for C_INIT request from control function */
   if (comp_e->ds_v42_compress_control_flag == C_INIT_REQ)
   {
      /* re-initialise compression function */
      gsn_init_compress( comp_e );

      gsn_v42_init_tables(
                          comp_e->ds_v42_c_hash_table,
                          comp_e->ds_v42_c_prefix_table,
                          comp_e->ds_v42_max_codewords,
                          comp_e->ds_v42_max_codewords
                         );

      comp_e->ds_v42_compress_control_flag = C_CLEAR;
      comp_e->ds_v42_compress_status_flag = C_INIT_CNF;
   }


   /* if the ds_v42_c_hash_table is congested this function will defrag it */
   if (comp_e->ds_v42_c_refill_hash_flag)
   {
      /* purge and refill hash table */

      gsn_v42_refill_hash
         (
            comp_e->ds_v42_max_codewords,
            comp_e->ds_v42_c_hash_ptr,
            comp_e->ds_v42_c_prefix_ptr,
            comp_e->ds_v42_c_app_char_ptr,
            comp_e->ds_v42_max_codewords
         );

      comp_e->ds_v42_c_refill_hash_flag = FALSE;
   }


   /*
    * this loop is ended when either the read buffer is empty, or
    * the maximum number of cycles has been reached
    */
/*lint -e(716) Info -- while(1) ... */
   while (1)
   {

      /*
       * limit the number of bytes processed to ensure time slice
       * of ISR is not over stretched
       */
      if ( gsn_check_full_V42_tx_buffer(comp_e) )
      {
         /* cycle limit reached */
         return;
      }

      /*
       * check there has been no errors writing to buffers This
       * should never occur as a check is made before executing
       * each cycle that sufficient space is available
       */
      if (comp_e->ds_v42_c_buffer_write_error)
      {
         comp_e->ds_v42_c_buffer_write_error = FALSE;
         ERR ("v42encode: buffer write error\n",0,0,0);
         return;
      }


      /* test for C_FLUSH_REQ from control function */
      if (comp_e->ds_v42_compress_control_flag == C_FLUSH_REQ)
      {
         /* check the input buffer is empty */
         /* not a requirement if terminate flag is not set */
         if (gsn_v42_check_empty_tx_buffer(comp_e) || (!comp_e->ds_v42_terminate_flag))
         {
            comp_e->ds_v42_terminate_flag = FALSE;
            /*
             * only performed if not already involved
             * with a flush or ds_v42_c_mode change
             */
            if (comp_e->ds_v42_c_exception_char_next_flag == FALSE)
            {
               gsn_flush(comp_e);   /* perform flush */
               /* ensure next char is handled by the exception algorithm */
               //ds_v42_c_exception_char_next_flag = TRUE;
               comp_e->ds_v42_compress_status_flag = C_FLUSH_CNF;

               comp_e->ds_v42_compress_control_flag = C_CLEAR;
               continue;
            }
            else
            {
               comp_e->ds_v42_compress_status_flag = C_FLUSH_CNF;
               comp_e->ds_v42_compress_control_flag = C_CLEAR;
               continue;
            }
         }
      }





      /* check for whether to test compression ratio */
      if (comp_e->ds_v42_c_datain_count > RETEST_THRESHOLD)
      {
         if (comp_e->ds_v42_c_exception_char_next_flag == FALSE)
         {
            gsn_v42_test_compression(comp_e);
            continue;
         }
      }
      /* read a byte from input buffer */
      if (gsn_v42_read_from_tx_buffer(&RxWord, comp_e))
      {
         /* return if it's empty */
         return;
      }
      comp_e->ds_v42_c_character = (byte)RxWord;


      /* compression test uses this */
      comp_e->ds_v42_c_datain_count++;


      /*
       * this will be set if a FLUSH ot ds_v42_c_mode change have occurred
       * the proceeding char will be processed slightly differently
       */
      if (comp_e->ds_v42_c_exception_char_next_flag)
      {
         gsn_v42_exception_process_next_char( comp_e );
         continue;
      }


      /* Detect first character condition */
      if (comp_e->ds_v42_c_string_code == V42BIS_UNUSED_MASK)
      {
         comp_e->ds_v42_c_string_code = comp_e->ds_v42_c_character;
         comp_e->ds_v42_c_string_length = 1;

         /* Assume transparent ds_v42_c_mode */
         gsn_v42_output_byte(comp_e->ds_v42_c_character, comp_e);

         /* escape code handling */
         if (comp_e->ds_v42_c_character == comp_e->ds_v42_c_escape_code)
         {
            /* output escape in data command code */
            gsn_v42_output_byte(EID, comp_e);
            /* cycle escape */
            comp_e->ds_v42_c_escape_code = (comp_e->ds_v42_c_escape_code + 51) % 256;
         }
         continue;
      }



      /* search for comp_e->ds_v42_c_string_code character combination in dictionary */
      index = gsn_v42_find_match
                  (
                     comp_e->ds_v42_c_string_code,
                     comp_e->ds_v42_c_character,
                     comp_e->ds_v42_c_hash_ptr,
                     comp_e->ds_v42_c_prefix_ptr,
                     comp_e->ds_v42_c_app_char_ptr,
                     &comp_e->ds_v42_c_refill_hash_flag,
                     &comp_e->ds_v42_c_search_success,
                     comp_e->ds_v42_max_codewords
                   );

      if (comp_e->ds_v42_c_search_success)   /* match found */
      {
         /* test for use of last code  */
         if ((comp_e->ds_v42_c_hash_ptr[index]) == comp_e->ds_v42_c_last_code)
         {
            if (comp_e->ds_v42_c_mode == COMPRESSED_MODE)
            {
               /* if neccesary STEPUP */
               if (comp_e->ds_v42_c_string_code > comp_e->ds_v42_c_stepup_threshold)
               {
                  gsn_v42_stepup(comp_e);
               }

               gsn_v42_output_code(comp_e->ds_v42_c_string_code, comp_e);
            }
            /* comp_e->ds_v42_c_last_code = V42BIS_UNUSED_MASK; */
            comp_e->ds_v42_c_string_code = comp_e->ds_v42_c_character;
            comp_e->ds_v42_c_string_length = 1;
            comp_e->ds_v42_c_last_code = V42BIS_UNUSED_MASK;

         }
         else   /* match ok */
         {
            comp_e->ds_v42_c_string_code = (comp_e->ds_v42_c_hash_ptr[index]);
            comp_e->ds_v42_c_string_length++;
            comp_e->ds_v42_c_string_matches++;
         }

      }

      else      /* no match, add to dictionary */
      {
         if (comp_e->ds_v42_c_string_length < comp_e->ds_v42_max_string_length)
         {
            comp_e->ds_v42_c_last_code = comp_e->ds_v42_c_next_code;

            gsn_v42_add_string
            (
               index,
               comp_e->ds_v42_c_string_code,
               comp_e->ds_v42_c_character,
               comp_e->ds_v42_c_max_code,
               &comp_e->ds_v42_c_next_code,
               comp_e->ds_v42_c_hash_ptr,
               comp_e->ds_v42_c_prefix_ptr,
               comp_e->ds_v42_c_app_char_ptr,
               comp_e->ds_v42_c_leaf_count_ptr,
               comp_e->ds_v42_max_codewords
            );
         }

         if (comp_e->ds_v42_c_mode == COMPRESSED_MODE)
         {
            /* if neccesary STEPUP */
            if (comp_e->ds_v42_c_string_code > comp_e->ds_v42_c_stepup_threshold)
            {
               gsn_v42_stepup(comp_e);
            }

            gsn_v42_output_code(comp_e->ds_v42_c_string_code, comp_e);

         }
         comp_e->ds_v42_c_string_code = comp_e->ds_v42_c_character;
         comp_e->ds_v42_c_string_length = 1;
      }




      if (comp_e->ds_v42_c_mode == TRANSPARENT_MODE)
      {
         gsn_v42_output_byte(comp_e->ds_v42_c_character, comp_e);
      }

      /* escape code handling */
      if (comp_e->ds_v42_c_character == comp_e->ds_v42_c_escape_code)
      {
         if (comp_e->ds_v42_c_mode == TRANSPARENT_MODE)
         {
            /* output escape in data command code */
            gsn_v42_output_byte(EID, comp_e);
         }
         /* cycle escape */
         comp_e->ds_v42_c_escape_code = (comp_e->ds_v42_c_escape_code + 51) % 256;
      }


   }  /* End of while loop */

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


void  gsn_v42_output_code(word code, comp_ent_V42bis_t *comp_e)
{

   /* MSG_GERAN_LOW_2("GSN ""encoder output code %d, %c", code, code); */

   /* offset ascii codes to allow for 3 control codes */
   if ((code < FIRST_CODE) && (!(comp_e->ds_v42_c_control_code_flag)))
   {
      code += 3;
   }

   /* add code to bit buffer */
   comp_e->ds_v42_c_output_bit_buffer |= ((unsigned long) code << comp_e->ds_v42_c_output_bit_count);
   comp_e->ds_v42_c_output_bit_count += comp_e->ds_v42_c_codeword_size;

   /* output any whole bytes in the bit buffer */
   while (comp_e->ds_v42_c_output_bit_count >= 8)
   {
      gsn_write_to_V42_tx_buffer((word) (comp_e->ds_v42_c_output_bit_buffer & 255), comp_e);

      /*** Old code: buffer check now done beforehand **
      if (gsn_write_to_V42_tx_buffer(comp_e->ds_v42_c_output_bit_buffer & 255) != 0)
      {
         printf("Error writing to tx comp buffer\n");
         comp_e->ds_v42_c_buffer_write_error = TRUE;
      }
      ********/

      /* shift byte out of bit buffer */
      comp_e->ds_v42_c_output_bit_buffer >>= 8;
      comp_e->ds_v42_c_output_bit_count -= 8;
      comp_e->ds_v42_c_dataout_count++;/* used in compression test */
   }
   /* reset control code flag */
   comp_e->ds_v42_c_control_code_flag = FALSE;
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


void  gsn_v42_output_byte(byte character, comp_ent_V42bis_t *comp_e)
{

   gsn_write_to_V42_tx_buffer(character, comp_e);

   /****** Old Code: buffer check now done beforehand *
   if (gsn_write_to_V42_tx_buffer(character) != 0)
   {
      comp_e->ds_v42_c_buffer_write_error = TRUE;
   }
   ******/

   /* MSG_GERAN_LOW_2("GSN ""encoder output byte %d, %c", character, character); */

   /* used in compression test */
   comp_e->ds_v42_c_dataout_count++;

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

void gsn_v42_test_compression(comp_ent_V42bis_t *comp_e)
{
   byte            temp;   /* used to store comp_e->ds_v42_c_codeword_size during octet
                            * align */



   if (comp_e->ds_v42_c_mode == TRANSPARENT_MODE)
   {
      if (comp_e->ds_v42_c_string_matches)
      {
         /* test for enough string matches to warrant change to compressed */

         if ((comp_e->ds_v42_c_datain_count / comp_e->ds_v42_c_string_matches) < ECM_THRESHOLD)
         {
            /* enter compressed ds_v42_c_mode */
            MSG_GERAN_LOW_0("GSN ""V42bis: enter compressed mode");
            /* output Enter compressed ds_v42_c_mode control code */
            gsn_v42_output_byte(comp_e->ds_v42_c_escape_code, comp_e);
            gsn_v42_output_byte(ECM, comp_e);

            /* change ds_v42_c_mode */
            comp_e->ds_v42_c_mode = COMPRESSED_MODE;

            /* clear bit buffer */
            comp_e->ds_v42_c_output_bit_count = 0;
            comp_e->ds_v42_c_output_bit_buffer = 0;

            comp_e->ds_v42_c_exception_char_next_flag = TRUE;

         }
      }

   }

   else         /* COMPRESSED_MODE */
   {
      if (comp_e->ds_v42_c_dataout_count)
      {

         /* check for poor compression ratio to warrant change to transparent */
         if ((comp_e->ds_v42_c_datain_count / comp_e->ds_v42_c_dataout_count) < ETM_THRESHOLD)
         {
            /* enter transparent ds_v42_c_mode */
            /* send curently matched string if there is one */
            if (comp_e->ds_v42_c_string_code != V42BIS_UNUSED_MASK)
            {
               /* if neccesary STEPUP */
               if (comp_e->ds_v42_c_string_code > comp_e->ds_v42_c_stepup_threshold)
               {

                  gsn_v42_stepup(comp_e);
               }

               gsn_v42_output_code(comp_e->ds_v42_c_string_code, comp_e);

            }

            /* send ETM (enter transparent ds_v42_c_mode) */
            comp_e->ds_v42_c_control_code_flag = TRUE;


            gsn_v42_output_code(ETM, comp_e);

            if (comp_e->ds_v42_c_output_bit_count)   /* if not octet aligned */
            {
               /* octet align by sending zeros */
               temp = comp_e->ds_v42_c_codeword_size;
               comp_e->ds_v42_c_codeword_size = 8 - comp_e->ds_v42_c_output_bit_count;
               comp_e->ds_v42_c_control_code_flag = TRUE;
               gsn_v42_output_code(0, comp_e);
               comp_e->ds_v42_c_codeword_size = temp;
               comp_e->ds_v42_c_output_bit_buffer = 0;
               comp_e->ds_v42_c_output_bit_count = 0;
            }

            comp_e->ds_v42_c_mode = TRANSPARENT_MODE;

            comp_e->ds_v42_c_exception_char_next_flag = TRUE;

            MSG_GERAN_LOW_0("GSN ""V42bis: enter transparent mode");
         }




      }
   }


   comp_e->ds_v42_c_datain_count = 0;
   comp_e->ds_v42_c_dataout_count = 0;
   comp_e->ds_v42_c_string_matches = 0;
}


/****************************************************************************
 *
 * Function name:   gsn_v42_reset
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


void gsn_v42_reset(comp_ent_V42bis_t *comp_e)
{
   byte            temp;
   if (comp_e->ds_v42_c_mode == COMPRESSED_MODE)
   {
      /* is string empty */
      if (comp_e->ds_v42_c_string_code != V42BIS_UNUSED_MASK)
      {
         /* if neccesary STEPUP */
         if (comp_e->ds_v42_c_string_code > comp_e->ds_v42_c_stepup_threshold)
         {
            gsn_v42_stepup(comp_e);
         }
         gsn_v42_output_code(comp_e->ds_v42_c_string_code, comp_e);
      }

      /* send ETM (enter transparent ds_v42_c_mode) */
      comp_e->ds_v42_c_control_code_flag = TRUE;
      gsn_v42_output_code(ETM, comp_e);

      if (comp_e->ds_v42_c_output_bit_count)   /* if not octet aligned */
      {
         /*
          * octet align by sending zeros
          */
         temp = comp_e->ds_v42_c_codeword_size;
         comp_e->ds_v42_c_codeword_size = 8 - comp_e->ds_v42_c_output_bit_count;
         comp_e->ds_v42_c_control_code_flag = TRUE;
         gsn_v42_output_code(0, comp_e);
         comp_e->ds_v42_c_codeword_size = temp;
         comp_e->ds_v42_c_output_bit_count = 0;
         comp_e->ds_v42_c_output_bit_buffer = 0;
      }

      comp_e->ds_v42_c_mode = TRANSPARENT_MODE;

   }
   /* send RESET */
   gsn_v42_output_byte(comp_e->ds_v42_c_escape_code, comp_e);
   gsn_v42_output_byte(V42_RESET, comp_e);

   /* initialise compression function */
   gsn_v42_init_tables(
                        comp_e->ds_v42_c_hash_table,
                        comp_e->ds_v42_c_prefix_table,
                        comp_e->ds_v42_max_codewords,
                        comp_e->ds_v42_max_codewords
                      );

   gsn_init_compress( comp_e );
   comp_e->ds_v42_c_last_code = V42BIS_UNUSED_MASK;
   comp_e->ds_v42_c_string_code = V42BIS_UNUSED_MASK;
}




/****************************************************************************
 *
 * Function name:   gsn_v42_exception_process_next_char
 * -------------
 *
 * Description:    This routine is called after a FLUSH or change of ds_v42_c_mode
 * -----------     it processes a character in a slightly different way.
 *
 * Returns:        none
 * -------
 *
 ****************************************************************************/

void gsn_v42_exception_process_next_char(comp_ent_V42bis_t *comp_e)
{
   word            hash_index;



   /* search for string character pair */
   hash_index = gsn_v42_find_match
                  (
                     comp_e->ds_v42_c_string_code,
                     comp_e->ds_v42_c_character,
                     comp_e->ds_v42_c_hash_ptr,
                     comp_e->ds_v42_c_prefix_ptr,
                     comp_e->ds_v42_c_app_char_ptr,
                     &comp_e->ds_v42_c_refill_hash_flag,
                     &comp_e->ds_v42_c_search_success,
                     comp_e->ds_v42_max_codewords
                  );

   if (comp_e->ds_v42_c_search_success == FALSE)
   {
      /* no match so add to dictionary */
      if (comp_e->ds_v42_c_string_length < comp_e->ds_v42_max_string_length)
      {
         comp_e->ds_v42_c_last_code = comp_e->ds_v42_c_next_code;
         gsn_v42_add_string
            (
               hash_index,
               comp_e->ds_v42_c_string_code,
               comp_e->ds_v42_c_character,
               comp_e->ds_v42_c_max_code,
               &comp_e->ds_v42_c_next_code,
               comp_e->ds_v42_c_hash_ptr,
               comp_e->ds_v42_c_prefix_ptr,
               comp_e->ds_v42_c_app_char_ptr,
               comp_e->ds_v42_c_leaf_count_ptr,
               comp_e->ds_v42_max_codewords
            );
      }
   }
   /* terminate string matching */
   comp_e->ds_v42_c_string_code = comp_e->ds_v42_c_character;
   comp_e->ds_v42_c_string_length = 1;

   if (comp_e->ds_v42_c_mode == TRANSPARENT_MODE)
   {
      gsn_v42_output_byte(comp_e->ds_v42_c_character, comp_e);
   }


   /* escape code handling */
   if (comp_e->ds_v42_c_character == comp_e->ds_v42_c_escape_code)
   {
      if (comp_e->ds_v42_c_mode == TRANSPARENT_MODE)
      {
         /* output escape in data command code */
         gsn_v42_output_byte(EID, comp_e);
      }
      /* cycle escape */
      comp_e->ds_v42_c_escape_code = (comp_e->ds_v42_c_escape_code + 51) % 256;
   }
   comp_e->ds_v42_c_exception_char_next_flag = FALSE;
}



/****************************************************************************
 *
 * Function name:   gsn_flush
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

void  gsn_flush(comp_ent_V42bis_t *comp_e)
{
   byte            temp;


   if (comp_e->ds_v42_c_mode == COMPRESSED_MODE)
   {
      /* send string code if there is one */
      if (comp_e->ds_v42_c_string_code != V42BIS_UNUSED_MASK)
      {
         /* if neccesary STEPUP */
         if (comp_e->ds_v42_c_string_code > comp_e->ds_v42_c_stepup_threshold)
         {
            gsn_v42_stepup(comp_e);
         }
         gsn_v42_output_code(comp_e->ds_v42_c_string_code, comp_e);
      }

      /* send FLUSH and octet align if not already aligned */
      if (comp_e->ds_v42_c_output_bit_count > 0)
      {
         /* send FLUSH */
         comp_e->ds_v42_c_control_code_flag = TRUE;
         gsn_v42_output_code(FLUSH, comp_e);
         /* octet align */
         if (comp_e->ds_v42_c_output_bit_count > 0)
         {
            temp = comp_e->ds_v42_c_codeword_size;
            comp_e->ds_v42_c_codeword_size = 8 - comp_e->ds_v42_c_output_bit_count;
            /* send zero bits to octet align */
            gsn_v42_output_code(0, comp_e);
            /* restore codeword size */
            comp_e->ds_v42_c_codeword_size = temp;
            /* clear output bit buffer */
            comp_e->ds_v42_c_output_bit_buffer = 0;
         }
      }
      comp_e->ds_v42_c_exception_char_next_flag = TRUE;
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

void gsn_v42_stepup(comp_ent_V42bis_t *comp_e)
{
   do
   {
      comp_e->ds_v42_c_control_code_flag = TRUE;
      gsn_v42_output_code(STEPUP, comp_e);
      comp_e->ds_v42_c_codeword_size++;
      comp_e->ds_v42_c_stepup_threshold = (word)((1 << comp_e->ds_v42_c_codeword_size) - 1);
   } while (comp_e->ds_v42_c_string_code > comp_e->ds_v42_c_stepup_threshold);
}

#else
void feature_gsm_gprs_comp_dummy(void) {}    /* To prevent compiler warning C3040E: no external declaration in translation unit */
#endif /* FEATURE_GSM_GPRS */
