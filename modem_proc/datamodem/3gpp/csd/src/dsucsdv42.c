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

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42.c_v   1.4   25 Jun 2002 13:12:26   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdv42.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/11   TTV     Support for Q6 free floating.
08/31/04   AR      Simplify initialization sequence.
05/05/04   AR      Remove assigment of flush flag.
07/15/03   TMR     Added support for RLP Version 2
05/14/03   ar      Lint corrections.  Diag msg corrections.
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/


/*******************************************************************************
 *
 *  File:           /home/briers/gsmqc/ds/SCCS/s.m51320.c
 *  -----
 *
 *  SCCS Version:   2.3
 *  -------------
 *
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained herein
 *  is confidential.  The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Communications Consultants Worldwide Limited (C) 2000
 *
 *  Project:
 *  --------
 *  P63
 *
 *  Software Unit: 
 *  --------------
 *  V.42bis compression standard
 *  
 *  Description:
 *  ------------
 *  This module performs compression and decompression in accordance with V42bis
 *  standard.
 *
 *
 *  Functions Used:
 *  ---------------
 *
 *  Modification History:
 *  ---------------------
 *  Date         Vers  Auth  Details
 *  ----         ----  ----  -------
 *  12-Feb-1999  1.0   PGB   Created
 *  12-Apr-2000  2.1   RCB   Added direction negotiation code to V42_init.
 *                           Various small changes for integration with L2R.
 *  26-Jul-2000  2.2   RCB   Update file header
 *  26-Jul-2000  2.3   RCB   Update file header
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



#define EMPTY_FLUSH_THRESHOLD   100

static byte    EmptyCount = 0;
static boolean Flushed = FALSE;

static byte ds_v42_compress_control_flag = 0; /* to make requests of the encoder   */
static byte ds_v42_decode_control_flag = 0;   /* to make requests of the decoder   */

static byte ds_v42_compress_status_flag = 0;  /* for status reporting from encoder */
static byte ds_v42_decode_status_flag = 0;    /* for status reporting from decoder */

static boolean ds_v42_terminate_flag = FALSE;    /* for requesting termination        */
 
static v42_comp_direction_T compression_request = V42_NO_COMPRESSION; /* stores negotiated direction*/
static byte ds_v42_max_string_length = 0;     /* negotiated max string length      */
static word ds_v42_max_codewords = 0;         /* dictionary size                   */
static byte ds_v42_max_codeword_size = 0;     /* max codeword size in bits         */


/****************************************************************************
 *
 * Function name:  V42 main
 * -------------
 *
 * Description:    called by RLP to encode and/or decode data
 * -----------
 *
 * Returns:        -1 to indicate error, otherwise returns 0
 * -------
 ****************************************************************************/
short  V42_main (void)
{
   short   ret_value = V42_OK;
   boolean Rlp_process_uplink;
   boolean Rlp_process_downlink;

   Rlp_process_uplink   = ds_ucsd_get_rlp_process_uplink();
   Rlp_process_downlink = ds_ucsd_get_rlp_process_downlink();
   if (compression_request != V42_NO_COMPRESSION)
   {
      if (Rlp_process_uplink)
      {
         /* Flush the buffers if no data transmited for a while */
         if ((EmptyCount > EMPTY_FLUSH_THRESHOLD) && (!Flushed))
         {
            V42_flush();
            EmptyCount = 0;
            DATA_MSG0_MED("V42bis: Flushing...");
         }
         else
         {
            EmptyCount++;
         }
      }
      
      switch (compression_request)
      {
         case V42_BOTH_DIRECTIONS:
            if (Rlp_process_uplink)
            {
               compress();
            }
            if (Rlp_process_downlink)
            {
               v42decode();   /* local copy of ds_process_uplink */            
            }
            break;
            
         case V42_DECODE_ONLY:
            if (Rlp_process_downlink)
            {
               v42decode();
            }
            break;
            
         case V42_ENCODE_ONLY:
            if (Rlp_process_uplink)
            {
               compress();
            }
            break;
            
         default:
            break;
      }
      
      
      /* test for error in decoder */
      if (ds_v42_decode_status_flag == D_ERROR_IND)
      {
         ret_value = V42_ERROR;
         DATA_MSG0_ERROR("V42bis:Fatal Error!");
      }
   }
   
   return ret_value;
}


/****************************************************************************
 *
 * Function name:  V42 initialise
 * -------------
 *
 * Description:    initialises V.42bis
 * -----------
 *
 * Returns:        none
 * -------
 ****************************************************************************/
void V42_initialise
(
  byte max_str_len,
  word max_codes,
  v42_comp_direction_T direction
)
{
   register word temp = 1;

   DS_UCSD_DEBUG_MSG ("NT: Initialising V42bis");

   /* initialise control and status flags */
   ds_v42_compress_control_flag = C_INIT_REQ;
   ds_v42_compress_status_flag = C_OK;

   ds_v42_decode_control_flag = D_INIT_REQ;
   ds_v42_decode_status_flag = D_OK;

   /* assign compression parameters */
   compression_request = direction;
   ds_v42_max_string_length = max_str_len;
   ds_v42_max_codewords = max_codes;

   DS_UCSD_DEBUG_MSG_1 ("ds_v42_max_codewords:%d", ds_v42_max_codewords);

   /* get max codeword size, largest power of 2 within max_codes */
   ds_v42_max_codeword_size = 0;
   while (temp < max_codes)
   {
      temp <<= 1; /* temp *= 2; */
      ds_v42_max_codeword_size++;
   }

   /* clear terminate flag */
   ds_v42_terminate_flag = FALSE;

   EmptyCount = 0;
   Flushed = FALSE;
}





/****************************************************************************
 *
 * Function name:  V42 flush
 * -------------
 *
 * Description:    flushes the output bit buffer of the encoder
 * -----------
 *
 * Returns:        none
 * -------
 ****************************************************************************/

void V42_flush(void)
{
   ds_v42_compress_control_flag = C_FLUSH_REQ;
   ds_v42_compress_status_flag = C_OK;
}




/****************************************************************************
 *
 * Function name:  V42 terminate
 * -------------
 *
 * Description:    flushes the output bit buffer of the encoder only after 
 * -----------       the input buffer is empty
 *
 * Returns:        none
 * -------
 ****************************************************************************/

void V42_terminate(void)
{
   DS_UCSD_DEBUG_MSG ("V42bis: Terminate");
   ds_v42_compress_control_flag = C_FLUSH_REQ;
   ds_v42_compress_status_flag = C_OK;
   /* set flag to tell encoder to clear it's input buffer before final bit
    * buffer flush */
   ds_v42_terminate_flag = TRUE;

}



/****************************************************************************
 *
 * Function name:  V42 terminated
 * -------------
 *
 * Description:    gives indiction of completion of termination flush
 * -----------
 *
 * Returns:        1 if completed or 0 if not
 * -------
 ****************************************************************************/

short           V42_terminated(void)
{
   byte            ret = 0;

   if (compression_request == V42_NO_COMPRESSION)
   {
      ret = 1;
   }
   else
   {
      if (ds_v42_compress_status_flag == C_FLUSH_CNF)
      {
         /* flushing confirmed */
         /*      ds_v42_compress_status_flag = C_OK;*/
         ret = 1;
      }
      
      if (compression_request == V42_DECODE_ONLY)
      {
         /* decode side doens't flush so the above condition will never 
          * be met */
         ret = 1;
      }
   }
   return ret;
}

/****************************************************************************
 *
 * Function name:  V42_SET_COMPRESS_CONTROL_FLAG
 * -------------
 *
 * Description:    Sets the value to variable ds_v42_compress_control_flag
 * -----------
 *
 * Returns:        TRUE if success or FALSE if fail
 * -------
 ****************************************************************************/
void V42_set_compress_control_flag
(
  byte compress_control_flag
)
{
  ds_v42_compress_control_flag = compress_control_flag;
}

/****************************************************************************
 *
 * Function name:  V42_SET_DECODE_CONTROL_FLAG
 * -------------
 *
 * Description:    Sets the value to variable ds_v42_decode_control_flag
 * -----------
 *
 * Returns:        TRUE if success or FALSE if fail
 * -------
 ****************************************************************************/
void V42_set_decode_control_flag
(
  byte decode_control_flag
)
{
  ds_v42_decode_control_flag = decode_control_flag;
}

/****************************************************************************
 *
 * Function name:  V42_SET_COMPRESS_STATUS_FLAG
 * -------------
 *
 * Description:    Sets the value to variable ds_v42_compress_status_flag
 * -----------
 *
 * Returns:        TRUE if success or FALSE if fail
 * -------
 ****************************************************************************/
void V42_set_compress_status_flag
(
  byte compress_status_flag
)
{
  ds_v42_compress_status_flag = compress_status_flag;
}

/****************************************************************************
 *
 * Function name:  V42_SET_DECODE_STATUS_FLAG
 * -------------
 *
 * Description:    Sets the value to variable ds_v42_decode_status_flag
 * -----------
 *
 * Returns:        TRUE if success or FALSE if fail
 * -------
 ****************************************************************************/
void V42_set_decode_status_flag
(
  byte decode_status_flag
)
{
  ds_v42_decode_status_flag = decode_status_flag;
}

/****************************************************************************
 *
 * Function name:  V42_SET_TERMINATE_FLAG
 * -------------
 *
 * Description:    Sets the value to variable ds_v42_terminate_flag
 * -----------
 *
 * Returns:        None
 * -------
 ****************************************************************************/
void V42_set_terminate_flag
(
  boolean terminate_flag
)
{
  ds_v42_terminate_flag = terminate_flag;
}

/****************************************************************************
 *
 * Function name:  V42_SET_MAX_STRING_LENGTH
 * -------------
 *
 * Description:    Sets the value to variable ds_v42_max_string_length
 * -----------
 *
 * Returns:        None
 * -------
 ****************************************************************************/
void V42_set_max_string_length
(
  byte max_string_length
)
{
  ds_v42_max_string_length = max_string_length;
}

/****************************************************************************
 *
 * Function name:  V42_SET_MAX_CODEWORDS
 * -------------
 *
 * Description:    Sets the value to variable ds_v42_max_codewords
 * -----------
 *
 * Returns:        None
 * -------
 ****************************************************************************/
void V42_set_max_codewords
(
  word max_codewords
)
{
  ds_v42_max_codewords = max_codewords;
}

/****************************************************************************
 *
 * Function name:  V42_SET_MAX_CODEWORD_SIZE
 * -------------
 *
 * Description:    Sets the value to variable ds_v42_max_codeword_size
 * -----------
 *
 * Returns:        None
 * -------
 ****************************************************************************/
void V42_set_max_codeword_size
(
  byte max_codeword_size
)
{
  ds_v42_max_codeword_size = max_codeword_size;
}

/****************************************************************************
 *
 * Function name:  V42_SET_FLUSHED
 * -------------
 *
 * Description:    Sets the value to the boolean variable Flushed
 * -----------
 *
 * Returns:        None
 * -------
 ****************************************************************************/
void V42_set_flushed
(
  boolean flushed
)
{
  Flushed = flushed;
}

/****************************************************************************
 *
 * Function name:  V42_GET_COMPRESS_CONTROL_FLAG
 * -------------
 *
 * Description:    return the contents of variable ds_v42_compress_control_flag
 * -----------
 *
 * Returns:        TRUE if success or FALSE if fail
 * -------
 ****************************************************************************/
byte V42_get_compress_control_flag(void)
{
  return ds_v42_compress_control_flag;
}

/****************************************************************************
 *
 * Function name:  V42_GET_DECODE_CONTROL_FLAG
 * -------------
 *
 * Description:    return the contents of variable ds_v42_decode_control_flag
 * -----------
 *
 * Returns:        TRUE if success or FALSE if fail
 * -------
 ****************************************************************************/
byte V42_get_decode_control_flag(void)
{
  return ds_v42_decode_control_flag;
}

/****************************************************************************
 *
 * Function name:  V42_GET_COMPRESS_STATUS_FLAG
 * -------------
 *
 * Description:    return the contents of variable ds_v42_compress_status_flag
 * -----------
 *
 * Returns:        TRUE if success or FALSE if fail
 * -------
 ****************************************************************************/
byte V42_get_compress_status_flag(void)
{
  return ds_v42_compress_status_flag;
}

/****************************************************************************
 *
 * Function name:  V42_GET_DECODE_STATUS_FLAG
 * -------------
 *
 * Description:    return the contents of variable ds_v42_decode_status_flag
 * -----------
 *
 * Returns:        TRUE if success or FALSE if fail
 * -------
 ****************************************************************************/
byte V42_get_decode_status_flag(void)
{
  return ds_v42_decode_status_flag;
}

/****************************************************************************
 *
 * Function name:  V42_GET_TERMINATE_FLAG
 * -------------
 *
 * Description:    Returns the value to variable ds_v42_terminate_flag
 * -----------
 *
 * Returns:        terminate_flag value
 * -------
 ****************************************************************************/
boolean V42_get_terminate_flag(void)
{
  return ds_v42_terminate_flag;
}

/****************************************************************************
 *
 * Function name:  V42_GET_MAX_STRING_LENGTH
 * -------------
 *
 * Description:    Returns the value to variable ds_v42_max_string_length
 * -----------
 *
 * Returns:        max_string_length
 * -------
 ****************************************************************************/
byte V42_get_max_string_length(void)
{
  return ds_v42_max_string_length;
}

/****************************************************************************
 *
 * Function name:  V42_GET_MAX_CODEWORDS
 * -------------
 *
 * Description:    Returns the value to variable ds_v42_max_codewords
 * -----------
 *
 * Returns:        max_codewords
 * -------
 ****************************************************************************/
word V42_get_max_codewords(void)
{
  return ds_v42_max_codewords;
}

/****************************************************************************
 *
 * Function name:  V42_GET_MAX_CODEWORD_SIZE
 * -------------
 *
 * Description:    Returns the value to variable ds_v42_max_codeword_size
 * -----------
 *
 * Returns:        max_codeword_size
 * -------
 ****************************************************************************/
byte V42_get_max_codeword_size(void)
{
  return ds_v42_max_codeword_size;
}

/****************************************************************************
 *
 * Function name:  V42_GET_FLUSHED
 * -------------
 *
 * Description:    Returns the value to the boolean variable Flushed
 * -----------
 *
 * Returns:        None
 * -------
 ****************************************************************************/
boolean V42_get_flushed()
{
  return Flushed;
}

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */

