#ifndef DSUCSDV42_H
#define DSUCSDV42_H
/*===========================================================================

              GSM Circuit Switched Data XXXX Header File

DESCRIPTION
  This file contains the definitions used in the GSM Circuit switched 
  data XXXX functionality.

Copyright (c) 2002 - 2004 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42.h_v   1.2   21 Mar 2002 10:57:50   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdv42.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/11   TTV     Support for Q6 free floating.
02/24/11   TTV     Fixed compilation warnings.
10/16/06   AR      Reduce MAX_ENCODE_CYCLES to improved CPU utilization.
05/12/04   AR      Tune encode/decode limits for WCDMA.
07/15/03   TMR     Added support for RLP Version 2
04/30/03   AR      Add suppport for DSM items inplace of local buffers.
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/


/*******************************************************************************
 *
 *  File:          /home/briers/gsmqc/ds/SCCS/s.v42bis.h
 *  -----
 *
 *  SCCS Version:  2.4
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
 *    P63
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
 *  24-Feb-1999  1.0   PGB   Created
 *  04-May-2000  2.1   RCB   Changes for integration with DSISR
 *  26-Jul-00    2.2   RCB   Update file header 
 *
 ******************************************************************************/


#define V42_DEBUG

#define V42_OK        0
#define V42_ERROR     (-1)

/************* implementation specific values ************/

#define HASHTABLE_SIZE   4096    /* maximum permited, given memory -- MUST REMAIN
                                 ** AN EVEN POWER OF 2 NUMBER FOR CODE TO WORK
                                 ** CORRECTLY!!!!!!!!!!
                                 */
                                 
#define CODETABLE_SIZE   1789    /* 2048 - 256 - 3 (control codes)            */
#define V42BIS_UNUSED_MASK 2048  /* bit 12 of the codewords indicates unused
                                  * locations in hash and code tables         */
#define NULL_LOCATION    2049    /* used to represent unassigned location     */
#define NULL_VALUE       2049    /* used to represent unassigned value        */

#define V42BIS_DELETION_MASK    16384
                                 /* msb indicates previously used in hashtable*/


/* time slice limiting */ 
#define MAX_DECODE_CYCLES  70 /* max bytes processed by decoder (was 30) */
#define MAX_ENCODE_CYCLES 150 /* max bytes processed by encoder (was 96) */
#define MAX_TX_BUF_BYTES  MAX_ENCODE_CYCLES /* max bytes read from SIO */

/******************* performance tuning *****************/

/** for speed **/

#define REFILL_THRESHOLD 20   /* allowable rehashes per character before a
                                * refill (hash defrag)                     */

/** for compress ratio **/

/* compression test parameters */
#define RETEST_THRESHOLD  200   /* test compression every retest_threshold   */
#define ETM_THRESHOLD     1     /* ratio of datain:dataout, default 1 lower
                                 * value is less likely to switch            */
#define ECM_THRESHOLD     3     /* ratio of datain:string matches, default
                                 * 3.4 higher value is less likely to switch */



/******************* capabilities ***********************/

#define MAX_STRING_LENGTH    250      /* N7 from the spec                 */
#define MAX_DICTIONARY_SIZE 2048      /* reqd for neqotiation             */
#define MAX_CODEWORD_SIZE     11      /* limits the dictionary size of this
                                      * implementation                    */

/****************** negotiation defaults ****************/
#define PREFERED_STRING_LENGTH       6      /* N7 from the spec           */
#define PREFERED_DICTIONARY_SIZE  2048      /* req'd for negotiation       */

/***************** V42bis definitions *****************/

#define FIRST_CODE 259          /* 3 - 259 are ascii characters, 0 - 2
                                 * are control code words                 */

#define INITIAL_CODEWORD_SIZE 9 /* increments with STEPUP control code to 
                                 *  maximum size negotiated               */

/* compression mode */
enum
{
   COMPRESSED_MODE,
   TRANSPARENT_MODE
};


/** special codes **/

/* Compressed mode control code words */
#define ETM    0
#define FLUSH  1
#define STEPUP 2


/* Transparent mode Command codes */
#define ECM    0
#define EID    1
#define RESET  2

/* negotiated parameters */


/* assigned to ds_v42_compress_control_flag to make requests of the compressor */
enum
{
   C_CLEAR,
   C_INIT_REQ,
   C_FLUSH_REQ
};


/* used by compress_status to confirm requests to control function */
enum
{
   C_OK,
   C_FLUSH_CNF,
   C_INIT_CNF
};

/* assigned to ds_v42_decode_control_flag to make requests of the decoder */
enum
{
   D_CLEAR,
   D_INIT_REQ
};

/* used by decode_status to confirm requests to control function */
enum
{
   D_OK,
   D_INIT_CNF,
   D_ERROR_IND
};


/******************* Prototypes *********************/

/* In control module */
void       V42_flush      (void);

/* in v42encode.c */
void     init_compress(void);
void     compress(void);


/* in v42decode.c */
void     init_decoder(void);
void     v42decode(void);


/* in dsgcsdv42buf.c */
void     ds_write_V42_rx_buffer        (dsm_item_type**);
short    ds_read_V42_rx_buffer         (dsm_item_type**);
short    ds_read_from_V42_rx_buffer    (byte*);
short    ds_check_V42_rx_buffer        (void);
boolean  ds_check_full_V42_rx_buffer   (void);
boolean  ds_check_empty_V42_rx_buffer  (void);
void     ds_init_V42_rx_buffer         (void);

void     ds_write_to_V42_tx_buffer     (byte);
void     ds_write_V42_tx_buffer        (dsm_item_type**);
uint32   ds_read_V42_tx_buffer         (dsm_item_type**,uint32);
boolean  ds_check_full_V42_tx_buffer   (void);
boolean  ds_check_empty_V42_tx_buffer  (void);
short    ds_check_V42_tx_buffer        (void);
void     ds_init_V42_tx_buffer         (void);

/* Buffer Routines for Remap Pending Buffer... in dsgcsdremap.c */
void     ds_write_remap_pending_buffer       (dsm_item_type**);
uint32    ds_read_remap_pending_buffer       (dsm_item_type**,uint32);
boolean  ds_check_full_remap_pending_buffer  (void);
boolean  ds_check_empty_remap_pending_buffer (void);
short    ds_check_remap_pending_buffer       (void);
void     ds_init_remap_pending_buffer        (void);



/********************** Functions from dsgcsdv42iram.c file ***************/
word            
find_match
(
   uint32   codeword, 
   uint32   character,
   word     *ds_v42_hash_ptr,
   word     *ds_v42_prefix_base_ptr,
   uint8    *ds_v42_app_char_base_ptr,
   uint8    *ds_v42_refill_hash_flag,
   uint8    *ds_v42_search_success 
);

void       
add_string
(
   uint32   hash_index, 
   uint32   codeword, 
   uint32   app_char,
   uint32   ds_v42_max_code,
   word     *ds_v42_next_code,
   word     *ds_v42_hash_ptr,   
   word     *ds_v42_prefix_base_ptr,
   uint8    *ds_v42_app_char_base_ptr,
   uint8    *ds_v42_leaf_count_base_ptr 
);

void       
refill_hash
(
   uint32   ds_v42_max_code,
   word     *ds_v42_hash_ptr,   
   word     *ds_v42_prefix_base_ptr,
   uint8    *ds_v42_app_char_base_ptr
);

void       
init_tables
(
   word     *ds_v42_hash_table,   
   word     *ds_v42_prefix_table 
);

/*---------------------------------------------------------------------------

                       Function Declarations                                 

---------------------------------------------------------------------------*/

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
byte V42_get_compress_control_flag(void);

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
byte V42_get_decode_control_flag(void);

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
byte V42_get_compress_status_flag(void);

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
byte V42_get_decode_status_flag(void);

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
boolean V42_get_terminate_flag(void);

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
byte V42_get_max_string_length(void);

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
word V42_get_max_codewords(void);

/****************************************************************************
 *
 * Function name:  V42_GET_MAX_CODEWORDS_SIZE
 * -------------
 *
 * Description:    Returns the value to variable ds_v42_max_codeword_size
 * -----------
 *
 * Returns:        max_codeword_size
 * -------
 ****************************************************************************/
byte V42_get_max_codeword_size(void);

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
boolean V42_get_flushed(void);

#endif /* DSUCSDV42_H */
