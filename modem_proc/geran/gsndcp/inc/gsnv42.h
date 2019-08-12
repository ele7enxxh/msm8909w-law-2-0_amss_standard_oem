#ifndef GSNV42_H
#define GSNV42_H
/*===========================================================================

              GPRS V42 bis Data compression Module

DESCRIPTION
  This file is part of the implementation of the GPRS V42 bis Data compression.
  The file is a modified version of the dsgcsdv42.h used in
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

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42.h_v   1.2   21 Mar 2002 10:57:50   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/inc/gsnv42.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

#include "comdef.h"
#include "gsndef.h"
#include <stdio.h>


#define V42_DEBUG

#define V42_OK        0
#define V42_ERROR     (-1)

/************* implementation specific values ************/
#define HASHTABLE_SIZE   512    /* maximum permited, given memory restrictions */
#define CODETABLE_SIZE   253    /* 512 - 256 - 3 (control codes)            */
#define V42BIS_UNUSED_MASK      2048    /* bit 12 of the codewords indicates unused
                                  * locations in hash and code tables         */
#define NULL_LOCATION    2049    /* used to represent unassigned location     */
#define NULL_VALUE       2049    /* used to represent unassigned value        */

#define V42BIS_DELETION_MASK    16384   /* msb indicates previously used in hashtable*/

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
#define V42_RESET  2

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


/* Added by ED. Although the maximum SNDCP N-PDU is 1502, we need to cater for 
** negative compression gain in SNDCP ACK mode (RC 1/2). Since the compression entity 
** is not reset after each N-PDU as in UNACK mode, we have to cater for compressed N=PDUs
** which are grater in size that the uncompressed N-PDU (negative compression gain)    
*/
#define V42COMP_BUF_LEN    1700

typedef struct 
{
   byte buf[V42COMP_BUF_LEN];
   word read_p;
   word write_p;
} FIFO_BUFFER;

typedef struct
{
   byte tx_data[V42COMP_BUF_LEN];
   word write_p;
   word read_p;
} tx_data_buffer_T;

typedef struct
{
   byte rx_data[V42COMP_BUF_LEN];
   word write_p;
   word read_p;
} rx_data_buffer_T;


/* Holds compression state info of the entity */
typedef struct
{
  byte    EmptyCount;
  boolean Flushed;
  byte    ds_v42_compress_control_flag;  /* to make requests of the encoder   */
  byte    ds_v42_decode_control_flag;    /* to make requests of the decoder   */
  byte    ds_v42_compress_status_flag;   /* for status reporting from encoder */
  byte    ds_v42_decode_status_flag;     /* for status reporting from decoder */
  byte    ds_v42_terminate_flag;         /* for requesting termination        */
  uint8   compression_request;   /* stores negotiated direction       */
  byte    ds_v42_max_string_length;     /* negotiated max string length      */
  word    ds_v42_max_codewords;         /* dictionary size                   */
  byte    ds_v42_max_codeword_size;     /* max codeword size in bits         */
  boolean process_uplink;
  boolean process_downlink;

  /* find_match return flag */
  byte     ds_v42_c_search_success;
  /* flag to indicate hashtable needs refilling */
  byte     ds_v42_c_refill_hash_flag;
  /* needed in add function to store next free code table entry */
  word     ds_v42_c_next_code;
  word     ds_v42_c_max_code;           /* current max code,initially 512 */
  word     ds_v42_c_stepup_threshold;   /* the codesize in bits for next stepup */
  byte     ds_v42_c_mode;             /* can be transparent or compressed */
  byte     ds_v42_c_codeword_size;    /* current code size in bits        */  
  byte     ds_v42_c_escape_code;      /* used in transparent ds_v42_c_mode         */

  /* for compression testing */
  word     ds_v42_c_datain_count;
  word     ds_v42_c_string_matches;
  word     ds_v42_c_dataout_count;

  /* current string and character, need to be global for the exception routine */
  word     ds_v42_c_string_code;
  byte     ds_v42_c_character;

  /* used in compress function and test_compression */
  byte     ds_v42_c_string_length;   /* used for keeping within N7 string length
                                  * limit */

  /* used for octet alignment by flush and switch */
  byte     ds_v42_c_output_bit_count;   /* stores the number of bits in the
                                       * output buffer */

  /* flag to indicate a control code is to be sent */
  byte     ds_v42_c_control_code_flag;   /* used in output_code */

  /*
   * used in compress and exception functions, it stores the last code added to
   * the dictionary used to ensure an entry is not sent before it is defined in
   * the decoder
  */
  word     ds_v42_c_last_code;

  byte     ds_v42_c_state;   /* current state */

  byte     ds_v42_c_exception_char_next_flag;   /* indicates whether the
                                             * exception function is
                                             * called on next loop  */

  /* stores the bits to be sent as codewords to decoder */
  unsigned long ds_v42_c_output_bit_buffer;

  /* used to indicate an error writing to the output buffer */
  byte     ds_v42_c_buffer_write_error;

  /* Decoder */
  word     ds_v42_d_max_code;        /* normally 2047             */
  byte     ds_v42_d_mode;            /* transparent or compressed */
  byte     ds_v42_d_codeword_size;   /* normally 11               */
  byte     ds_v42_d_escape_code;
  word     ds_v42_d_stepup_threshold;   /* code value to go to next codeword size */

  /* needed in add function */
  word     ds_v42_d_next_code;   /* points to next available dictionary entry */
  /* d_find_match return flags */
  byte     ds_v42_d_search_success;

  /* flag to indicate hashtable needs refilling */
  byte     ds_v42_d_refill_hash_flag;

  /* holds number of bits left in buffer to be transfered */
  byte     ds_v42_d_input_bit_count;        /* reset by decode function after ETM */

  unsigned long ds_v42_d_input_bit_buffer;  /* codeword bit buffer                */
  byte     ds_v42_d_state;   /* current state          */
  word     ds_v42_d_last_code;                 /* last code added        */
  byte     ds_v42_d_stack[MAX_STRING_LENGTH];  /* for string reversing   */
  byte     ds_v42_d_post_ETM_flag;             /* flag set after ETM     */
  byte    *ds_v42_d_stack_ptr;                 /* for reversing strings  */
  byte     ds_v42_d_string_length;         /* current string length              */
  byte     ds_v42_d_decode_pending_flag;   /* for resuming of unfinished decodes */
  word     ds_v42_d_old_code;              /* holds last code decoded            */
  word     ds_v42_d_string_code;           /* current code word                  */
  word     ds_v42_d_character;             /* current appended byte              */
  byte     ds_v42_d_last_string_length;    /* length of previous string          */
  word     ds_v42_d_data_pending;          /* store for pending tx buffer writes */
  
  /* Buffers used by the compressor/encoder */
  tx_data_buffer_T  tx_data_buffer; /* Input buffer */
  FIFO_BUFFER       TxBuffer; /* output buffer */

  /* Buffers used by the decompressor/decoder */
  FIFO_BUFFER       RxBuffer; /* Input buffer */
  rx_data_buffer_T  rx_data_buffer; /* output buffer */

  /* Compressor/encoder code and hash tables */
  word     *ds_v42_c_hash_table;
  word     *ds_v42_c_prefix_table;
  uint8    *ds_v42_c_app_char_table;
  uint8    *ds_v42_c_leaf_count_table;

  /* pointers to the arrays used to avoid passing variables excessively  */
  word    *ds_v42_c_hash_ptr;
  word    *ds_v42_c_prefix_ptr;
  uint8   *ds_v42_c_app_char_ptr;
  uint8   *ds_v42_c_leaf_count_ptr;

  /* Decompressor/decoder code and hash tables */
  word     *ds_v42_d_hash_table;
  word     *ds_v42_d_prefix_table;
  uint8    *ds_v42_d_app_char_table;
  uint8    *ds_v42_d_leaf_count_table;


  /* pointers to the arrays used to avoid passing variables excessively  */
  word    *ds_v42_d_hash_ptr;
  word    *ds_v42_d_prefix_ptr;
  uint8   *ds_v42_d_app_char_ptr;
  uint8   *ds_v42_d_leaf_count_ptr;

}comp_ent_V42bis_t;


/******************* Prototypes *********************/

/* In control module */
void  gsn_V42_initialise( comp_ent_V42bis_t*, byte, word, uint8, uint8);
short gsn_v42_main( comp_ent_V42bis_t * );
void  gsn_v42_terminate(comp_ent_V42bis_t *);

void  gsn_V42_flush (comp_ent_V42bis_t *);

/* in v42encode.c */
void     gsn_init_compress(comp_ent_V42bis_t *);
void     gsn_v42_compress(comp_ent_V42bis_t *);
void     gsn_flush(comp_ent_V42bis_t *);

/* in v42decode.c */
void     gsn_v42_init_decoder(comp_ent_V42bis_t *);
void     gsn_v42_decode(comp_ent_V42bis_t *);

/* in dsgcsdv42buf.c */

word     gsn_read_from_V42_rx_buffer (word*, comp_ent_V42bis_t *);
boolean  gsn_check_empty_V42_rx_buffer (const comp_ent_V42bis_t *);
void     gsn_write_to_V42_tx_buffer (word, comp_ent_V42bis_t * );
boolean  gsn_check_full_V42_tx_buffer (const comp_ent_V42bis_t *);
word    gsn_check_V42_tx_buffer (const comp_ent_V42bis_t *);
int8 gsn_v42_read_from_tx_buffer( word *, comp_ent_V42bis_t *);
boolean  gsn_v42_check_empty_tx_buffer (comp_ent_V42bis_t *);
void gsn_v42_write_to_rx_buffer( word, comp_ent_V42bis_t * );
uint16 gsn_v42_check_rx_buffer (const comp_ent_V42bis_t *);
boolean gsn_v42_check_full_rx_buffer (const comp_ent_V42bis_t *);
void gsn_v42_init_tx_rx_buffs ( comp_ent_V42bis_t *comp_e );
void gsn_v42_dealloc_tables( gas_id_t gas_id, const comp_ent_V42bis_t *, uint8 );
boolean gsn_v42_alloc_tables( comp_ent_V42bis_t *, uint8, uint16 );



/********************** Functions from dsgcsdv42iram.c file ***************/
word gsn_v42_find_match
(
   uint32   codeword, 
   uint32   character,
   word     *ds_v42_hash_ptr,
   word     *ds_v42_prefix_ptr,
   uint8    *ds_v42_app_char_ptr,
   uint8    *ds_v42_refill_hash_flag,
   uint8    *ds_v42_search_success,
   uint16   table_size 
);

void gsn_v42_add_string
(
   uint32   hash_index, 
   uint32   codeword, 
   uint32   app_char,
   uint32   ds_v42_max_code,
   word     *ds_v42_next_code,
   word     *ds_v42_hash_ptr,   
   word     *ds_v42_prefix_ptr,
   uint8    *ds_v42_app_char_ptr,
   uint8    *ds_v42_leaf_count_ptr,
   uint16   table_size
);

void gsn_v42_refill_hash
(
   uint32   ds_v42_max_code,
   word     *ds_v42_hash_ptr,   
   word     *ds_v42_prefix_ptr,
   uint8    *ds_v42_app_char_ptr,
   uint16   table_size
);

void gsn_v42_init_tables
(
   word     *ds_v42_hash_table,   
   word     *ds_v42_prefix_table,
   uint16   hash_table_size,
   uint16   code_table_size

);
/************************* Global Variables *******************************/


#endif /* FEATURE_GSM_GPRS */
#endif /* GSNV42_H */
