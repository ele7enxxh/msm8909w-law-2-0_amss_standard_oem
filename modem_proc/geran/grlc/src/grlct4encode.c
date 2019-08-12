/*****************************************************************************
***
*** TITLE
***
***  EGPRS RLC T.4 Encoder Module
***
***
*** DESCRIPTION
***
***  This module implements T.4 encoding as per 3GPP TS 04.60 V8.21.0
***
*** 
*** EXTERNALIZED FUNCTIONS
*** 
***  boolean grlc_t4_encode ()
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  none
***
*** Copyright (c) 2004-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlct4encode.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 07/23/04    hv     Created
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */



#include "comdef.h"
#include "grlct4encode.h"
#include "grlcmsgenum.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/* As per Table 9.1.10.1/2 of TS 04.60
*/
#define TCODE_TABLE_LEN       64
#define MCODE_TABLE_LEN       15
#define MAX_MCODE_RUN_LENGTH  960

/* Show debug info if defined
*/
#define ERLD_SHOW_T4_ENCODE
/* #undef  ERLD_SHOW_T4_ENCODE */


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

/*-------------------------------------------------------------------------
** Generic structure to define a T.4 table element. This shall be used for 
** the Terminating Code Table as well as the Make-up Code Table
**-------------------------------------------------------------------------
*/
typedef struct
{
  uint16  zero_cw;
  uint16  zero_cw_len;    /* length of zero code word in bits */

  uint16  one_cw;
  uint16  one_cw_len;     /* length of one code word in bits */

} t4_encode_table_t;


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*-------------------------------------------------------------
** Terminating code table. The index represents the run length
**-------------------------------------------------------------
*/
static const t4_encode_table_t   tcode_tab[TCODE_TABLE_LEN] =
{
  /* rlen   hexZero lenZero,   hexOne, lenOne,              binZero       binOne */

  /*  0 */  { 0x037,     10,    0x035,      8, }, /*   00 0011 0111    0011 0101 */
  /*  1 */  { 0x002,      2,    0x007,      6, }, /*             10     00  0111 */
  /*  2 */  { 0x003,      2,    0x007,      4, }, /*             11         0111 */
  /*  3 */  { 0x002,      3,    0x008,      4, }, /*            010         1000 */
  /*  4 */  { 0x003,      3,    0x00B,      4, }, /*            011         1011 */
  /*  5 */  { 0x003,      4,    0x00C,      4, }, /*           0011         1100 */
  /*  6 */  { 0x002,      4,    0x00E,      4, }, /*           0010         1110 */
  /*  7 */  { 0x003,      5,    0x00F,      4, }, /*         0 0011         1111 */
  /*  8 */  { 0x005,      6,    0x013,      5, }, /*        00 0101       1 0011 */
  /*  9 */  { 0x004,      6,    0x014,      5, }, /*        00 0100       1 0100 */
  /* 10 */  { 0x004,      7,    0x007,      5, }, /*       000 0100       0 0111 */
  /* 11 */  { 0x005,      7,    0x008,      5, }, /*       000 0101       0 1000 */
  /* 12 */  { 0x007,      7,    0x008,      6, }, /*       000 0111      00 1000 */
  /* 13 */  { 0x004,      8,    0x003,      6, }, /*      0000 0100      00 0011 */
  /* 14 */  { 0x007,      8,    0x034,      6, }, /*      0000 0111      11 0100 */
  /* 15 */  { 0x018,      9,    0x035,      6, }, /*    0 0001 1000      11 0101 */
  /* 16 */  { 0x017,     10,    0x02A,      6, }, /*   00 0001 0111      10 1010 */
  /* 17 */  { 0x018,     10,    0x02B,      6, }, /*   00 0001 1000      10 1011 */
  /* 18 */  { 0x008,     10,    0x027,      7, }, /*   00 0000 1000     010 0111 */
  /* 19 */  { 0x067,     11,    0x00C,      7, }, /*  000 0110 0111     000 1100 */
  /* 20 */  { 0x068,     11,    0x008,      7, }, /*  000 0110 1000     000 1000 */
  /* 21 */  { 0x06C,     11,    0x017,      7, }, /*  000 0110 1100     001 0111 */
  /* 22 */  { 0x037,     11,    0x003,      7, }, /*  000 0011 0111     000 0011 */
  /* 23 */  { 0x028,     11,    0x004,      7, }, /*  000 0010 1000     000 0100 */
  /* 24 */  { 0x017,     11,    0x028,      7, }, /*  000 0001 0111     010 1000 */
  /* 25 */  { 0x018,     11,    0x02B,      7, }, /*  000 0001 1000     010 1011 */
  /* 26 */  { 0x0CA,     12,    0x013,      7, }, /* 0000 1100 1010     001 0011 */
  /* 27 */  { 0x0CB,     12,    0x024,      7, }, /* 0000 1100 1011     010 0100 */
  /* 28 */  { 0x0CC,     12,    0x018,      7, }, /* 0000 1100 1100     001 1000 */
  /* 29 */  { 0x0CD,     12,    0x002,      8, }, /* 0000 1100 1101    0000 0010 */
  /* 30 */  { 0x068,     12,    0x003,      8, }, /* 0000 0110 1000    0000 0011 */
  /* 31 */  { 0x069,     12,    0x01A,      8, }, /* 0000 0110 1001    0001 1010 */
  /* 32 */  { 0x06A,     12,    0x01B,      8, }, /* 0000 0110 1010    0001 1011 */
  /* 33 */  { 0x06B,     12,    0x012,      8, }, /* 0000 0110 1011    0001 0010 */
  /* 34 */  { 0x0D2,     12,    0x013,      8, }, /* 0000 1101 0010    0001 0011 */
  /* 35 */  { 0x0D3,     12,    0x014,      8, }, /* 0000 1101 0011    0001 0100 */
  /* 36 */  { 0x0D4,     12,    0x015,      8, }, /* 0000 1101 0100    0001 0101 */
  /* 37 */  { 0x0D5,     12,    0x016,      8, }, /* 0000 1101 0101    0001 0110 */
  /* 38 */  { 0x0D6,     12,    0x017,      8, }, /* 0000 1101 0110    0001 0111 */
  /* 39 */  { 0x0D7,     12,    0x028,      8, }, /* 0000 1101 0111    0010 1000 */
  /* 40 */  { 0x06C,     12,    0x029,      8, }, /* 0000 0110 1100    0010 1001 */
  /* 41 */  { 0x06D,     12,    0x02A,      8, }, /* 0000 0110 1101    0010 1010 */
  /* 42 */  { 0x0DA,     12,    0x02B,      8, }, /* 0000 1101 1010    0010 1011 */
  /* 43 */  { 0x0DB,     12,    0x02C,      8, }, /* 0000 1101 1011    0010 1100 */
  /* 44 */  { 0x054,     12,    0x02D,      8, }, /* 0000 0101 0100    0010 1101 */
  /* 45 */  { 0x055,     12,    0x004,      8, }, /* 0000 0101 0101    0000 0100 */
  /* 46 */  { 0x056,     12,    0x005,      8, }, /* 0000 0101 0110    0000 0101 */
  /* 47 */  { 0x057,     12,    0x00A,      8, }, /* 0000 0101 0111    0000 1010 */
  /* 48 */  { 0x064,     12,    0x00B,      8, }, /* 0000 0110 0100    0000 1011 */
  /* 49 */  { 0x065,     12,    0x052,      8, }, /* 0000 0110 0101    0101 0010 */
  /* 50 */  { 0x052,     12,    0x053,      8, }, /* 0000 0101 0010    0101 0011 */
  /* 51 */  { 0x053,     12,    0x054,      8, }, /* 0000 0101 0011    0101 0100 */
  /* 52 */  { 0x024,     12,    0x055,      8, }, /* 0000 0010 0100    0101 0101 */
  /* 53 */  { 0x037,     12,    0x024,      8, }, /* 0000 0011 0111    0010 0100 */
  /* 54 */  { 0x038,     12,    0x025,      8, }, /* 0000 0011 1000    0010 0101 */
  /* 55 */  { 0x027,     12,    0x058,      8, }, /* 0000 0010 0111    0101 1000 */
  /* 56 */  { 0x028,     12,    0x059,      8, }, /* 0000 0010 1000    0101 1001 */
  /* 57 */  { 0x058,     12,    0x05A,      8, }, /* 0000 0101 1000    0101 1010 */
  /* 58 */  { 0x059,     12,    0x05B,      8, }, /* 0000 0101 1001    0101 1011 */
  /* 59 */  { 0x02B,     12,    0x04A,      8, }, /* 0000 0010 1011    0100 1010 */
  /* 60 */  { 0x02C,     12,    0x04B,      8, }, /* 0000 0010 1100    0100 1011 */
  /* 61 */  { 0x05A,     12,    0x032,      8, }, /* 0000 0101 1010    0011 0010 */
  /* 62 */  { 0x066,     12,    0x033,      8, }, /* 0000 0110 0110    0011 0011 */
  /* 63 */  { 0x067,     12,    0x034,      8, }, /* 0000 0110 0111    0011 0100 */

}; /* tcode_tab[] */


/*-----------------------------------------------------------------------
** Make-up code table. The index represents the run length according to
** the formula: 
**         run_length = 64 * (index+1). 
**       eg.       64         0
**                128         1
**                192         2
**                    ...
**                960        14
**-----------------------------------------------------------------------
*/
static const t4_encode_table_t   mcode_tab[MCODE_TABLE_LEN] =
{
  /* ind  rlen     hexZero lenZero   hexOne lenOne                binZero          binOne */

  /*   0    64 */  { 0x00F,     10,  0x001B,     5, }, /*     00 0000 1111         1 1011 */
  /*   1   128 */  { 0x0C8,     12,  0x0012,     5, }, /*   0000 1100 1000         1 0010 */
  /*   2   192 */  { 0x0C9,     12,  0x0017,     6, }, /*   0000 1100 1001        01 0111 */
  /*   3   256 */  { 0x05B,     12,  0x0037,     7, }, /*   0000 0101 1011       011 0111 */
  /*   4   320 */  { 0x033,     12,  0x0036,     8, }, /*   0000 0011 0011      0011 0110 */
  /*   5   384 */  { 0x034,     12,  0x0037,     8, }, /*   0000 0011 0100      0011 0111 */
  /*   6   448 */  { 0x035,     12,  0x0064,     8, }, /*   0000 0011 0101      0110 0100 */
  /*   7   512 */  { 0x06C,     13,  0x0065,     8, }, /* 0 0000 0110 1100      0110 0101 */
  /*   8   576 */  { 0x06D,     13,  0x0068,     8, }, /* 0 0000 0110 1101      0110 1000 */
  /*   9   640 */  { 0x04A,     13,  0x0067,     8, }, /* 0 0000 0100 1010      0110 0111 */
  /*  10   704 */  { 0x04B,     13,  0x00CC,     9, }, /* 0 0000 0100 1011    0 1100 1100 */
  /*  11   768 */  { 0x04C,     13,  0x00CD,     9, }, /* 0 0000 0100 1100    0 1100 1101 */
  /*  12   832 */  { 0x04D,     13,  0x00D2,     9, }, /* 0 0000 0100 1101    0 1101 0010 */
  /*  13   896 */  { 0x072,     13,  0x00D3,     9, }, /* 0 0000 0111 0010    0 1101 0011 */
  /*  14   960 */  { 0x073,     13,  0x00D4,     9, }, /* 0 0000 0111 0011    0 1101 0100 */

}; /* mcode_tab[] */


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/

/*--------------------------------------------------------------------------
** Macro to obtain an index to the Make-up code table given a run length
** from 64 to 960. index = (run_length/64) - 1. Donot use this macro unless
** the run length value is at least 64
**--------------------------------------------------------------------------
*/
#define GET_MCODE_TABLE_INDEX(rl)             (((rl) >> 6) - 1)

/*--------------------------------------------------------------------------
** When a run length exceeds 64 the result shall be a make-up code followed
** by a terminating code. The macro below calculates the index to the 
** (remaining) terminating code
**--------------------------------------------------------------------------
*/
#define GET_REMAINING_TCODE_TABLE_INDEX(rl)   ((rl) % TCODE_TABLE_LEN)


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/



/*===========================================================================
===
===  FUNCTION    GRLC_T4_ENCODE()
===
===  DESCRIPTION
===
===    This function performs T.4 encoding on the given run and run length specified.
===    The function returns TRUE if the given parameters are valid and the result
===    is returned in the passed data structure. Depending on the run length value
===    the result may contain a make-up/terminating code word pair or just the 
===    a terminating code word. If the make-up code word does not exist then its
===    run length is set to 0.
===
===  DEPENDENCIES
===
===    none
===
===  PARAMS
===
===    run: 0 or 1
===    run_length: from 0 to 960
===    res_ptr: results stored back in the passed structure. 
===         If run and run_length are valid tcode and tcode_len always contain 
===         meaningful values. mcode_len is zet to 0 if make-up code does not exist
===  
===  RETURN VALUE
===
===    Boolean TRUE if given params are valid, FALSE otherwise
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean grlc_t4_encode
(
  t4_encode_res_t   *res_ptr,
  uint16            run,
  uint16            run_length
)
{
  register uint16   ind = 0;

  /*--------------------------------------------------------------
  ** Return with no results if input parameters are out if range.
  ** May make this section conditional to debug mode only.
  **--------------------------------------------------------------
  */
  if (run > 1 ||                            /* expect run = 0 or 1 only */
      run_length > MAX_MCODE_RUN_LENGTH
     )
  {
    MSG_GRLC_ERROR_2(GRLC_F3_MSG_ENUM_303, "ERLD t4enc OOR run=%u run_length=%u", run, run_length);
  
    /* set the code lengths to 0 to limit damage
    */
    res_ptr->mcode_len = 0;
    res_ptr->tcode_len = 0;

    return FALSE;
  }

  if (run_length < TCODE_TABLE_LEN)
  {
    /*-----------------------
    ** Terminating code only
    **-----------------------
    */

    /* Index to the Terminating codes table is the run length itself
    */
    ind = run_length;

    /* Set make-up code length to 0 as make-up code does not exist
    */
    res_ptr->mcode_len = 0;
    res_ptr->mcode     = 0;
  }
  else
  {
    /*----------------------------------------
    ** Make-up code and terminating code pair
    **----------------------------------------
    */

    /* Get index to the make-up code table
    */
    ind = GET_MCODE_TABLE_INDEX(run_length);
    
    /* Fill in the make-up code and its length
    */
    if (run == 0)
    {
      res_ptr->mcode     = mcode_tab[ind].zero_cw;
      res_ptr->mcode_len = mcode_tab[ind].zero_cw_len;
    }
    else
    {
      res_ptr->mcode     = mcode_tab[ind].one_cw;
      res_ptr->mcode_len = mcode_tab[ind].one_cw_len;
    }
    
    /* Get the index to the terminating code table
    */
    ind = GET_REMAINING_TCODE_TABLE_INDEX(run_length);
  }

  /*---------------------------------------------
  ** Fill in the terminating code and its length
  **---------------------------------------------
  */
  if (run == 0)
  {
    res_ptr->tcode     = tcode_tab[ind].zero_cw;
    res_ptr->tcode_len = tcode_tab[ind].zero_cw_len;
  }
  else
  {
    res_ptr->tcode     = tcode_tab[ind].one_cw;
    res_ptr->tcode_len = tcode_tab[ind].one_cw_len;
  }

#ifdef  ERLD_SHOW_T4_ENCODE
  MSG_GRLC_LOW_2(GRLC_F3_MSG_ENUM_304, "ERLD t4enc run=%u run_length=%u", run, run_length);
  MSG_GRLC_LOW_2(GRLC_F3_MSG_ENUM_305, "ERLD       mc=x%03x mclen=%u", res_ptr->mcode, res_ptr->mcode_len);
  MSG_GRLC_LOW_2(GRLC_F3_MSG_ENUM_306, "ERLD       tc=x%03x tclen=%u", res_ptr->tcode, res_ptr->tcode_len);
#endif /* ERLD_SHOW_T4_ENCODE */

  return TRUE;

} /* grlc_t4_encode */

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/



/*** EOF: don't remove! ***/
