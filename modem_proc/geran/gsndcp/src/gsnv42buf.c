
/*===========================================================================

               GPRS V42 bis Data compression Module

DESCRIPTION
  This file is part of the implementation of the GPRS V42 bis Data compression.
  The file is a modified version of the dsgcsdv42buf.c used in
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

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42buf.c_v   1.2   21 Mar 2002 10:57:58   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsnv42buf.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03-07-16   tjw     Prevent compiler warning C3040E: no external declaration in translation unit
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/


/*****************************************************************************
 *
 *  File:            /home/briers/gsmqc/ds/SCCS/s.m51308.c
 *  -----
 *
 *  SCCS Version:    2.2
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
 *  Description:    V24 Uplink Buffer
 *  ------------
 *
 *  Functions Used:
 *  ---------------
 *
 *  Modification History:
 *  ---------------------
 *  Date       Vers  Auth   Details
 *  ----       ----  ----   -------
 *  04-Apr-00  2.1   RCB    Original version
 *  26-Jul-00  2.2   RCB    Update file header
 *
 *****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

#include "comdef.h"
#include "gsndef.h"
#include "gsni.h"
#include "gsnv42.h"
#include "gprs_mem.h"
#include "memory.h"

/*===========================================================================

FUNCTION gsn_v42_read_from_tx_buffer

DESCRIPTION  reads user data words from tx_data_buffer


DEPENDENCIES
  None

RETURN VALUE
  0: OK, not 0: error

SIDE EFFECTS
  None

===========================================================================*/
int8 gsn_v42_read_from_tx_buffer
(
   word *tx_data_word, comp_ent_V42bis_t *comp_e
)
{
  int8 error_ind = 0;

  if (comp_e->tx_data_buffer.read_p == comp_e->tx_data_buffer.write_p)
  {
    /* buffer empty - indicate error */
    error_ind = -1;
    comp_e->tx_data_buffer.read_p = comp_e->tx_data_buffer.write_p = 0;
  }

  else
  {
    *tx_data_word = comp_e->tx_data_buffer.tx_data[comp_e->tx_data_buffer.read_p];
    comp_e->tx_data_buffer.read_p++;
    if( comp_e->tx_data_buffer.read_p >= V42COMP_BUF_LEN )
    {
      /* buffer overflow - indicate error */
      error_ind = -1;
      comp_e->tx_data_buffer.read_p = comp_e->tx_data_buffer.write_p = 0;
      MSG_GERAN_HIGH_0("GSN ""V42bis tx_data_buffer overflow");
    }
  }

  return error_ind;
}

/*===========================================================================

FUNCTION gsn_v42_check_empty_tx_buffer

DESCRIPTION  Checks empty buffer


DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean gsn_v42_check_empty_tx_buffer (comp_ent_V42bis_t *comp_e)
{
  if (comp_e->tx_data_buffer.write_p == comp_e->tx_data_buffer.read_p)
  {
    /* Initialise pointers ready for next N-PDU */
    comp_e->tx_data_buffer.write_p = comp_e->tx_data_buffer.read_p = 0;
    return TRUE;
  }
  else
    return FALSE;
}

/*===========================================================================================*/

/*===========================================================================

FUNCTION gsn_v42_write_to_rx_buffer

DESCRIPTION  writes user data words to rx_data_buffer


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gsn_v42_write_to_rx_buffer
(
   uint16 rx_data_word, comp_ent_V42bis_t *comp_e
)
{
  if (((comp_e->rx_data_buffer.write_p + 1) == comp_e->rx_data_buffer.read_p) ||
     (((comp_e->rx_data_buffer.write_p + 1) == V42COMP_BUF_LEN) &&
      (comp_e->rx_data_buffer.read_p == 0)))
  {
    /*error - buffer full*/
    MSG_GERAN_HIGH_0("GSN ""V42bis rx_data_buffer overflow");
  }

  else
  {
    comp_e->rx_data_buffer.rx_data[comp_e->rx_data_buffer.write_p] = (uint8)rx_data_word;
    comp_e->rx_data_buffer.write_p++;
  }
}

/*===========================================================================

FUNCTION gsn_v42_check_full_rx_buffer

DESCRIPTION  Checks buffer for over full threshold


DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean gsn_v42_check_full_rx_buffer (const comp_ent_V42bis_t *comp_e)
{
  int16 size;

  size =
    (int16)(comp_e->rx_data_buffer.write_p - comp_e->rx_data_buffer.read_p);

  if( (size < 0) || (size > V42COMP_BUF_LEN) )
    return TRUE;
  else
    return FALSE;

}

/*===========================================================================

FUNCTION gsn_v42_check_rx_buffer

DESCRIPTION  Checks number of chars in buffer


DEPENDENCIES
  None

RETURN VALUE
  Number of chars

SIDE EFFECTS
  None

===========================================================================*/
uint16 gsn_v42_check_rx_buffer (const comp_ent_V42bis_t *comp_e)
{
  int16 size;

  size =
    (int16)(comp_e->rx_data_buffer.write_p - comp_e->rx_data_buffer.read_p);

  if( (size <= 0) || (size > V42COMP_BUF_LEN) )
  {
    MSG_GERAN_ERROR_1("GSN "" Invalid rx_buffer size %d", size);
    size = 0;
  }

  return ( (uint16)size );
}

/****************************************************************************
 * TX buffer routines
 ****************************************************************************/

void gsn_write_to_V42_tx_buffer (word data, comp_ent_V42bis_t *comp_e)
{
   comp_e->TxBuffer.buf [comp_e->TxBuffer.write_p++] = (byte) data;

   if (comp_e->TxBuffer.write_p >= V42COMP_BUF_LEN)
   {
      MSG_GERAN_MED_1("GSN "" V42: tx_buffer full %d", comp_e->TxBuffer.write_p);

      /* Initialise the input tx buffer, read_p = write_p means no input data
      ** to the compressor, so that the compression cycle ends.
      */
      comp_e->tx_data_buffer.read_p = comp_e->tx_data_buffer.write_p = 0;
   }
}

boolean gsn_check_full_V42_tx_buffer (const comp_ent_V42bis_t *comp_e)
{
  int16 size  = (int16) (comp_e->TxBuffer.write_p - comp_e->TxBuffer.read_p);


  if( (size < 0) || (size > V42COMP_BUF_LEN) )
    return TRUE;
  else
    return FALSE;
}


word gsn_check_V42_tx_buffer (const comp_ent_V42bis_t *comp_e)
{
   int16 size  = (int16)(comp_e->TxBuffer.write_p - comp_e->TxBuffer.read_p);

   if( (size <= 0) || (size > V42COMP_BUF_LEN) )
   {
     MSG_GERAN_ERROR_1("GSN "" Invalid tx_buffer size %d", size);
     size = 0;
   }

   /*   size = (size*100)/BUF_LEN;*/

   return (word)size;
}


/****************************************************************************
 * RX buffer routines
 ****************************************************************************/

word  gsn_read_from_V42_rx_buffer (word* pData, comp_ent_V42bis_t *comp_e)
{
   if (comp_e->RxBuffer.write_p != comp_e->RxBuffer.read_p)
   {
      *pData = (word) comp_e->RxBuffer.buf[comp_e->RxBuffer.read_p++];
      if (comp_e->RxBuffer.read_p >= V42COMP_BUF_LEN)
      {
         comp_e->RxBuffer.read_p = comp_e->RxBuffer.write_p = 0;
         MSG_GERAN_HIGH_0("GSN "" V42: Rxbuffer overflow !");
      }

      return 1;
   }
   else
      return 0;

}

boolean gsn_check_empty_V42_rx_buffer (const comp_ent_V42bis_t *comp_e)
{
   if (comp_e->RxBuffer.write_p == comp_e->RxBuffer.read_p)
      return TRUE;
   else
      return FALSE;
}

/****************************************************************************
 *
 * Function name:  gsn_v42_init_tx_rx_buffs
 * -------------
 *
 * Description:    Initialises tx and rx buffers used in V42 bis
 * -----------
 *
 * Returns:        None
 * -------
 ****************************************************************************/
void gsn_v42_init_tx_rx_buffs ( comp_ent_V42bis_t *comp_e )
{
  comp_e->RxBuffer.write_p = 0;
  comp_e->RxBuffer.read_p  = 0;

  comp_e->TxBuffer.write_p = 0;
  comp_e->TxBuffer.read_p  = 0;

  comp_e->rx_data_buffer.write_p = 0;
  comp_e->rx_data_buffer.read_p  = 0;

  comp_e->tx_data_buffer.write_p = 0;
  comp_e->tx_data_buffer.read_p  = 0;

}


/****************************************************************************
 *
 * Function name:  gsn_v42_alloc_tables
 * -------------
 *
 * Description:    Allocates memory for code and hash tables of the encoder/decoder.
 * -----------
 *
 * Returns:        TRUE if available heap
 * -------
 ****************************************************************************/

boolean gsn_v42_alloc_tables( comp_ent_V42bis_t *comp_e, uint8 direction, uint16 len )
{

  boolean init_ok = TRUE;

  if( direction == GSN_V42_MS_TO_SGSN ||
      direction == GSN_V42_BOTH_DIR )
  {
    /* Allocate Compressor/encoder code and hash tables */
    comp_e->ds_v42_c_hash_table = GPRS_MEM_MALLOC( (sizeof(word)) * len );
    comp_e->ds_v42_c_prefix_table = GPRS_MEM_MALLOC( (sizeof(word)) * len );
    comp_e->ds_v42_c_app_char_table = GPRS_MEM_MALLOC( (sizeof(uint8)) * len );
    comp_e->ds_v42_c_leaf_count_table = GPRS_MEM_MALLOC( (sizeof(uint8)) * len );

    if( comp_e->ds_v42_c_hash_table &&
        comp_e->ds_v42_c_prefix_table &&
        comp_e->ds_v42_c_app_char_table &&
        comp_e->ds_v42_c_leaf_count_table )

    {
      /* pointers to the arrays used to avoid passing variables excessively  */
      comp_e->ds_v42_c_hash_ptr   = comp_e->ds_v42_c_hash_table;
      comp_e->ds_v42_c_prefix_ptr =
          (&comp_e->ds_v42_c_prefix_table[0] - FIRST_CODE);
      comp_e->ds_v42_c_app_char_ptr =
          (&comp_e->ds_v42_c_app_char_table[0] - FIRST_CODE);
      comp_e->ds_v42_c_leaf_count_ptr =
          (&comp_e->ds_v42_c_leaf_count_table[0] - FIRST_CODE);
    }
    else
      init_ok = FALSE;

  }

  if( direction == GSN_V42_SGSN_TO_MS ||
      direction == GSN_V42_BOTH_DIR )
  {

    /* Allocate Decompressor/decoder code and hash tables */
    comp_e->ds_v42_d_hash_table = GPRS_MEM_MALLOC( (sizeof(word)) * len );
    comp_e->ds_v42_d_prefix_table = GPRS_MEM_MALLOC( (sizeof(word)) * len );
    comp_e->ds_v42_d_app_char_table = GPRS_MEM_MALLOC( (sizeof(uint8)) * len );
    comp_e->ds_v42_d_leaf_count_table = GPRS_MEM_MALLOC( (sizeof(uint8)) * len );


    if( comp_e->ds_v42_d_hash_table &&
        comp_e->ds_v42_d_prefix_table &&
        comp_e->ds_v42_d_app_char_table &&
        comp_e->ds_v42_d_leaf_count_table )
    {
      comp_e->ds_v42_d_hash_ptr   = comp_e->ds_v42_d_hash_table;
      comp_e->ds_v42_d_prefix_ptr =
        (&comp_e->ds_v42_d_prefix_table[0] - FIRST_CODE);
      comp_e->ds_v42_d_app_char_ptr =
        (&comp_e->ds_v42_d_app_char_table[0] - FIRST_CODE);
      comp_e->ds_v42_d_leaf_count_ptr =
        (&comp_e->ds_v42_d_leaf_count_table[0] - FIRST_CODE);
    }
    else
      init_ok = FALSE;
  }

  return( init_ok );
}

/****************************************************************************
 *
 * Function name:  gsn_v42_dealloc_tables
 * -------------
 *
 * Description:    deallocates memory for code and hash tables of the encoder/decoder.
 * -----------
 *
 * Returns:        None
 * -------
 ****************************************************************************/

void gsn_v42_dealloc_tables( gas_id_t gas_id, const comp_ent_V42bis_t *comp_e, uint8 direction )
{

  if( direction == GSN_V42_MS_TO_SGSN ||
      direction == GSN_V42_BOTH_DIR )
  {
    /* deallocate Compressor/encoder code and hash tables */
    if( comp_e->ds_v42_c_hash_table )
    {
      GPRS_MEM_FREE( comp_e->ds_v42_c_hash_table);
    }
    if( comp_e->ds_v42_c_prefix_table )
    {
      GPRS_MEM_FREE( comp_e->ds_v42_c_prefix_table);
    }
    if( comp_e->ds_v42_c_app_char_table )
    {
      GPRS_MEM_FREE( comp_e->ds_v42_c_app_char_table);
    }
    if( comp_e->ds_v42_c_leaf_count_table )
    {
      GPRS_MEM_FREE( comp_e->ds_v42_c_leaf_count_table);
    }
  }

  if( direction == GSN_V42_SGSN_TO_MS ||
      direction == GSN_V42_BOTH_DIR )
  {

    if( comp_e->ds_v42_d_hash_table )
    {
      GPRS_MEM_FREE( comp_e->ds_v42_d_hash_table);
    }
    if( comp_e->ds_v42_d_prefix_table )
    {
      GPRS_MEM_FREE( comp_e->ds_v42_d_prefix_table);
    }
    if( comp_e->ds_v42_d_app_char_table )
    {
      GPRS_MEM_FREE( comp_e->ds_v42_d_app_char_table);
    }
    if( comp_e->ds_v42_d_leaf_count_table )
    {
      GPRS_MEM_FREE( comp_e->ds_v42_d_leaf_count_table);
    }
  }
}

#else
void feature_gsm_gprs_buf_dummy(void) {}    /* To prevent compiler warning C3040E: no external declaration in translation unit */
#endif /* FEATURE_GSM_GPRS */
