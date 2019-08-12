/*===========================================================================

              GPRS V42 bis Data compression Module

DESCRIPTION
  This file is part of the implementation of the GPRS V42 bis Data compression.
  The file is a modified version of the dsgcsdv42.c used in
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

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42.c_v   1.4   25 Jun 2002 13:12:26   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsnv42.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

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

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

#include "comdef.h"
#include "gsni.h"
#include "gsnv42.h"
#include "gsndef.h"
#include "err.h"

#define EMPTY_FLUSH_THRESHOLD   100

/****************************************************************************
 *
 * Function name:  gsn_v42_main
 * -------------
 *
 * Description:    called by RLP to encode and/or decode data
 * -----------
 *
 * Returns:        -1 to indicate error, otherwise returns 0
 * -------
 ****************************************************************************/
short  gsn_v42_main ( comp_ent_V42bis_t *comp_e)
{
   short   ret_value = V42_OK;

   if (comp_e->compression_request != GSN_V42_NO_COMPRESSION)
   {
      if (comp_e->process_uplink)
      {
         /* Flush the buffers if no data transmited for a while */
         if (gsn_v42_check_empty_tx_buffer(comp_e))
         {
            if ((comp_e->EmptyCount > EMPTY_FLUSH_THRESHOLD) && (!comp_e->Flushed))
            {
               gsn_V42_flush( comp_e );
               comp_e->EmptyCount = 0;
               comp_e->Flushed = TRUE;
               MSG_GERAN_LOW_0("GSN ""V42bis: Flushing...");
            }
            else
            {
               comp_e->EmptyCount++;
            }
         }
         else
         {
            comp_e->EmptyCount=0;
            comp_e->Flushed = FALSE;
         }
      }

      switch (comp_e->compression_request)
      {
         case GSN_V42_BOTH_DIR:
            if (comp_e->process_uplink)
            {
               gsn_v42_compress( comp_e );
            }
            if (comp_e->process_downlink)
            {
               gsn_v42_decode( comp_e );   /* local copy of ds_process_uplink */
            }
            break;

         case GSN_V42_SGSN_TO_MS:
            if (comp_e->process_downlink)
            {
               gsn_v42_decode( comp_e );
            }
            break;

         case GSN_V42_MS_TO_SGSN:
            if ( comp_e->process_uplink )
            {
               gsn_v42_compress( comp_e );
            }
            break;

         default:
            break;
      }

      /* test for error in decoder */
      if (comp_e->process_downlink)
      {
        if (comp_e->ds_v42_decode_status_flag == D_ERROR_IND)
        {
           ret_value = V42_ERROR;
           ERR ("V42bis:Fatal Error!",0,0,0);
        }
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
void gsn_V42_initialise
(
  comp_ent_V42bis_t *comp_e,
  byte              max_str_len,
  word              max_codes,
  uint8             direction,
  uint8             init_dir
)
{
  word temp = 1;

  MSG_GERAN_LOW_0("GSN ""Initialising V42bis");

  /* initialise control and status flags */
  comp_e->ds_v42_compress_control_flag = C_INIT_REQ;
  comp_e->ds_v42_decode_control_flag = D_INIT_REQ;
  comp_e->ds_v42_compress_status_flag = C_OK;

  comp_e->ds_v42_decode_status_flag = D_OK;

  /* assign compression parameters */
  comp_e->compression_request = direction;
  comp_e->ds_v42_max_string_length = max_str_len;
  comp_e->ds_v42_max_codewords = max_codes;

  MSG_GERAN_LOW_1("GSN "" V42bis max_codewords:%d", comp_e->ds_v42_max_codewords);

  /* get max codeword size, largest power of 2 within max_codes */
  comp_e->ds_v42_max_codeword_size = 0;
  while (temp < max_codes)
  {
    temp *= 2;
    comp_e->ds_v42_max_codeword_size++;
  }

  /* clear terminate flag */
  comp_e->ds_v42_terminate_flag = FALSE;

  comp_e->EmptyCount = 0;
  comp_e->Flushed = FALSE;

  /* Initialize encoder/decoder */
  if( init_dir == GSN_V42_BOTH_DIR )
  {
    gsn_init_compress( comp_e );
    gsn_v42_init_decoder( comp_e );
  }
  else if( init_dir == GSN_V42_MS_TO_SGSN )
  {
    gsn_init_compress( comp_e );
  }
  else if( init_dir == GSN_V42_SGSN_TO_MS )
  {
    gsn_v42_init_decoder( comp_e );
  }

  gsn_v42_init_tx_rx_buffs ( comp_e );
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

void gsn_V42_flush( comp_ent_V42bis_t * comp_e)
{
   comp_e->ds_v42_compress_control_flag = C_FLUSH_REQ;
   comp_e->ds_v42_compress_status_flag = C_OK;
}




/****************************************************************************
 *
 * Function name:  gsn_v42_terminate
 * -------------
 *
 * Description:    flushes the output bit buffer of the encoder only after
 * -----------       the input buffer is empty
 *
 * Returns:        none
 * -------
 ****************************************************************************/

void gsn_v42_terminate(comp_ent_V42bis_t * comp_e)
{
   MSG_GERAN_LOW_0("GSN ""V42bis: Terminate");
   comp_e->ds_v42_compress_control_flag = C_FLUSH_REQ;
   comp_e->ds_v42_compress_status_flag = C_OK;
   /* set flag to tell encoder to clear it's input buffer before final bit
    * buffer flush */
   comp_e->ds_v42_terminate_flag = TRUE;

}


#else
void feature_gsm_gprs_dummy(void) {}    /* To prevent compiler warning C3040E: no external declaration in translation unit */
#endif /* FEATURE_GSM_GPRS */
