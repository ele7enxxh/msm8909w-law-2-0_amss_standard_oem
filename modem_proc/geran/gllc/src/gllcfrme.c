/*****************************************************************************
***
*** TITLE  gllcfrme.c
***
***  GPRS LLC Layer Frame Assembly & Disassembly Functions
***
***
*** DESCRIPTION
***
***  Provides a suite of LLC frame assembly/disassembly functions and
***  formatting macros and definitions for the translation of LLC frame
***  information between 'internal' and 'external' formats.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  gllc_assemble_frame_hdr()
***  gllc_assemble_frame_info_and_fcs()
***
***  gllc_disassemble_frame_hdr()
***  gllc_disassemble_frame_info_and_fcs()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  None.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/src/gllcfrme.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz     changes
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
#include "gprs_pdu.h"
#include "gllci.h"
#include "msg.h"
#include "string.h"
#include "stringl.h"


/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/* N202 lookup table to be indexed by LLC Version number. */

LOCAL const uint16 gllc_n202_lookup[16] =
{
  4, /* LLC Version 0 */

  4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
};


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

/* Frame acceptance utility prototypes. */

LOCAL boolean gllc_accept_u_frame_ctrl_field( gllc_u_frame_t *u_frame );


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

/* LLE to SAPI address translator. */

const uint8 gllc_sapi_addr_lookup[ GLLC_LL_NUM_SAPIS ] =
{
  1,    /* GLLC_LL_SAPI_1_GMM   */
  3,    /* GLLC_LL_SAPI_3_LL3   */
  5,    /* GLLC_LL_SAPI_5_LL5   */
  7,    /* GLLC_LL_SAPI_7_SMS   */
  9,    /* GLLC_LL_SAPI_9_LL9   */
  11    /* GLLC_LL_SAPI_11_LL11 */
};


/* SAPI address to LLE translator. */

const gllc_ll_sapi_t gllc_lle_lookup[ 16 ] =
{
  GLLC_LL_SAPI_RESERVED,    /*  0 */
  GLLC_LL_SAPI_1_GMM,       /*  1 */
  GLLC_LL_SAPI_RESERVED,    /*  2 */
  GLLC_LL_SAPI_3_LL3,       /*  3 */
  GLLC_LL_SAPI_RESERVED,    /*  4 */
  GLLC_LL_SAPI_5_LL5,       /*  5 */
  GLLC_LL_SAPI_RESERVED,    /*  6 */
  GLLC_LL_SAPI_7_SMS,       /*  7 */
  GLLC_LL_SAPI_RESERVED,    /*  8 */
  GLLC_LL_SAPI_9_LL9,       /*  9 */
  GLLC_LL_SAPI_RESERVED,    /* 10 */
  GLLC_LL_SAPI_11_LL11,     /* 11 */
  GLLC_LL_SAPI_RESERVED,    /* 12 */
  GLLC_LL_SAPI_RESERVED,    /* 13 */
  GLLC_LL_SAPI_RESERVED,    /* 14 */
  GLLC_LL_SAPI_RESERVED     /* 15 */
};


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/

/* ===========================================================================
   LL PDU Frame Structure Manipulation Macros

   Utility definitions for use in the construction of LL PDU frame headers
   and for use in the parsing mechanisms which must be applied in LL PDU
   frame header decomposition.
   ======================================================================== */

/*lint -save -e798 Stop complaints about redundant '\' characters */

/*
   LLC frame address field manipulation macros.

                                Bit
            7     6     5     4     3     2     1     0    Octet
         +-----+-----+-----+-----+-----+-----+-----+-----+
         | PD  | C/R |  x  |  x  |         SAPI          |   0
         +-----+-----+-----+-----+-----+-----+-----+-----+
*/

/* ........................................................................ */

#define LLC_FRAME_SET_PD( bufp, pd )                                        \
                                                                            \
        { bufp[0] = (bufp[0] & ~0x80) | ((uint8)(pd << 7) & 0x80); }

#define LLC_FRAME_GET_PD( bufp )                                            \
                                                                            \
        ( (uint8)((bufp[0] & 0x80) >> 7) )

/* ........................................................................ */

  #define LLC_FRAME_SET_CR( bufp, cr )                                      \
                                                                            \
          { bufp[0] = (bufp[0] & ~0x40) | ((uint8)(cr << 6) & 0x40); }

  #define LLC_FRAME_GET_CR( bufp )                                          \
                                                                            \
          ( (uint8)((bufp[0] & 0x40) >> 6) )

/* ........................................................................ */

#define LLC_FRAME_SET_SAPI( bufp, sapi )                                    \
                                                                            \
        { bufp[0] = (bufp[0] & ~0x0F) |                                     \
          (gllc_sapi_addr_lookup[sapi & 0x0F] & 0x0F); }

#define LLC_FRAME_GET_SAPI( bufp )                                          \
                                                                            \
        ( gllc_lle_lookup[ bufp[0] & 0x0F ] )

/* ------------------------------------------------------------------------ */

/*
   LLC frame header format field manipulation macros.

                                Bit
  Frame     7     6     5     4     3     2     1     0    Octet
         +-----+-----+-----+-----+-----+-----+-----+-----+
   I+S   |  0  |                                         |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+

         +-----+-----+-----+-----+-----+-----+-----+-----+
   S     |  1  |  0  |                                   |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+

         +-----+-----+-----+-----+-----+-----+-----+-----+
   UI    |  1  |  1  |  0  |                             |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+

         +-----+-----+-----+-----+-----+-----+-----+-----+
   U     |  1  |  1  |  1  |                             |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+
*/

/* ........................................................................ */

#define LLC_FRAME_SET_FORMAT_I_S( bufp )                                    \
                                                                            \
        { bufp[1] &= ~0x80; }

#define LLC_FRAME_SET_FORMAT_S( bufp )                                      \
                                                                            \
        { bufp[1]  = (bufp[1] & ~0xC0) | 0x80; }

#define LLC_FRAME_SET_FORMAT_UI( bufp )                                     \
                                                                            \
        { bufp[1]  = (bufp[1] & ~0xE0) | 0xC0; }

#define LLC_FRAME_SET_FORMAT_U( bufp )                                      \
                                                                            \
        { bufp[1] |= 0xE0; }


#define LLC_FRAME_GET_FORMAT( bufp )                                        \
        (gllc_frame_format_t)                                               \
        (                                                                   \
          ( ((bufp[1] & 0x80) == 0x00) ? (uint8)GLLC_I_S_FORMAT:(uint8)GLLC_NO_FORMAT ) | \
          ( ((bufp[1] & 0xC0) == 0x80) ? (uint8)GLLC_S_FORMAT  :(uint8)GLLC_NO_FORMAT ) | \
          ( ((bufp[1] & 0xE0) == 0xC0) ? (uint8)GLLC_UI_FORMAT :(uint8)GLLC_NO_FORMAT ) | \
          ( ((bufp[1] & 0xE0) == 0xE0) ? (uint8)GLLC_U_FORMAT  :(uint8)GLLC_NO_FORMAT )   \
        )

/* ........................................................................ */

/*
   LLC I plus S frame header manipulation macros.

                                Bit
            7     6     5     4     3     2     1     0    Octet
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |  0  |  A  |  x  |             N(S)            |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |          N(S)         |  x  |       N(R)      |   2
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |                N(R)               |  S1 |  S2 |   3
         +-----+-----+-----+-----+-----+-----+-----+-----+


   I plus S frame header for S1S2 = SACK

                                Bit
            7     6     5     4     3     2     1     0    Octet
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |  0  |  A  |  x  |             N(S)            |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |          N(S)         |  x  |       N(R)      |   2
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |                N(R)               |  S1 |  S2 |   3
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |  x  |  x  |  x  |              K              |   4
         +-----+-----+-----+-----+-----+-----+-----+-----+
         | R(1)| R(2)| R(3)| R(4)| R(5)| R(6)| R(7)| R(8)|   5
         +-----+-----+-----+-----+-----+-----+-----+-----+
         | R(9)|R(10)|R(11)|R(12)|R(13)|R(14)|R(15)|R(16)|   6
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |                      ...                      |   :
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |R(249|R(250|R(251|R(252|R(253|R(254|R(255|  x  |   36 (max)
         +-----+-----+-----+-----+-----+-----+-----+-----+
*/

/* ........................................................................ */

#define LLC_I_S_FRAME_SET_A( bufp, a )                                      \
                                                                            \
        { bufp[1] = (bufp[1] & ~0x40) | ((uint8)(a << 6) & 0x40); }

#define LLC_I_S_FRAME_GET_A( bufp )                                         \
                                                                            \
        ( (uint8)((bufp[1] & 0x40) >> 6) )

/* ........................................................................ */

#define LLC_I_S_FRAME_SET_N_S( bufp, n_s )                                  \
        {                                                                   \
          bufp[1] = (bufp[1] & ~0x1F) | ((uint8)(n_s >> 4) & 0x1F);         \
          bufp[2] = (bufp[2] & ~0xF0) | ((uint8)(n_s << 4) & 0xF0);         \
        }

#define LLC_I_S_FRAME_GET_N_S( bufp )                                       \
                                                                            \
        ( (uint16)((bufp[1] & 0x1F) << 4) | (uint16)((bufp[2] & 0xF0) >> 4) )

/* ........................................................................ */

#define LLC_I_S_FRAME_SET_N_R( bufp, n_r )                                  \
        {                                                                   \
          bufp[2] = (bufp[2] & ~0x07) | ((uint8)(n_r >> 6) & 0x07);         \
          bufp[3] = (bufp[3] & ~0xFC) | ((uint8)(n_r << 2) & 0xFC);         \
        }

#define LLC_I_S_FRAME_GET_N_R( bufp )                                       \
                                                                            \
        ( (uint16)((bufp[2] & 0x07) << 6) | (uint16)((bufp[3] & 0xFC) >> 2) )

/* ........................................................................ */

#define LLC_I_S_FRAME_SET_S1S2( bufp, s1s2 )                                \
                                                                            \
        { bufp[3] = (bufp[3] & ~0x03) | (s1s2 & 0x03); }

#define LLC_I_S_FRAME_GET_S1S2( bufp )                                      \
                                                                            \
        ( (gllc_supervisory_t)(bufp[3] & 0x03) )

/* ........................................................................ */

#define LLC_I_S_FRAME_SET_K( bufp, k )                                      \
                                                                            \
        { bufp[4] = (bufp[4] & ~0x1F) | (k & 0x1F); }

#define LLC_I_S_FRAME_GET_K( bufp )                                         \
                                                                            \
        ( bufp[4] & 0x1F )

/* ------------------------------------------------------------------------ */

/*
   LLC S frame header manipulation macros.

                                Bit
            7     6     5     4     3     2     1     0    Octet
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |  1  |  0  |  A  |  x  |  x  |       N(R)      |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |                N(R)               |  S1 |  S2 |   2
         +-----+-----+-----+-----+-----+-----+-----+-----+


   S frame header for S1S2 = SACK

                                Bit
            7     6     5     4     3     2     1     0    Octet
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |  1  |  0  |  A  |  x  |  x  |       N(R)      |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |                N(R)               |  S1 |  S2 |   2
         +-----+-----+-----+-----+-----+-----+-----+-----+
         | R(1)| R(2)| R(3)| R(4)| R(5)| R(6)| R(7)| R(8)|   3
         +-----+-----+-----+-----+-----+-----+-----+-----+
         | R(9)|R(10)|R(11)|R(12)|R(13)|R(14)|R(15)|R(16)|   4
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |                      ...                      |   :
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |R(249|R(250|R(251|R(252|R(253|R(254|R(255|  x  |   34 (max)
         +-----+-----+-----+-----+-----+-----+-----+-----+
*/

/* ........................................................................ */

#define LLC_S_FRAME_SET_A( bufp, a )                                        \
                                                                            \
        { bufp[1] = (bufp[1] & ~0x20) | ((uint8)(a << 5) & 0x20); }

#define LLC_S_FRAME_GET_A( bufp )                                           \
                                                                            \
        ( (uint8)((bufp[1] & 0x20) >> 5) )

/* ........................................................................ */

#define LLC_S_FRAME_SET_N_R( bufp, n_r )                                    \
        {                                                                   \
          bufp[1] = (bufp[1] & ~0x07) | ((uint8)(n_r >> 6) & 0x07);         \
          bufp[2] = (bufp[2] & ~0xFC) | ((uint8)(n_r << 2) & 0xFC);         \
        }

#define LLC_S_FRAME_GET_N_R( bufp )                                         \
                                                                            \
        ( (uint16)((bufp[1] & 0x07) << 6) | (uint16)((bufp[2] & 0xFC) >> 2) )

/* ........................................................................ */

#define LLC_S_FRAME_SET_S1S2( bufp, s1s2 )                                  \
                                                                            \
        { bufp[2] = (bufp[2] & ~0x03) | (s1s2 & 0x03); }

#define LLC_S_FRAME_GET_S1S2( bufp )                                        \
                                                                            \
        ( (gllc_supervisory_t)(bufp[2] & 0x03) )

/* ------------------------------------------------------------------------ */

/*
   LLC UI frame header manipulation macros.

                                Bit
            7     6     5     4     3     2     1     0    Octet
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |  1  |  1  |  0  |  x  |  x  |       N(U)      |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |                N(U)               |  E  |  PM |   2
         +-----+-----+-----+-----+-----+-----+-----+-----+
*/

/* ........................................................................ */

#define LLC_UI_FRAME_SET_N_U( bufp, n_u )                                   \
        {                                                                   \
          bufp[1] = (bufp[1] & ~0x07) | ((n_u >> 6) & 0x07);                \
          bufp[2] = (bufp[2] & ~0xFC) | ((n_u << 2) & 0xFC);                \
        }

#define LLC_UI_FRAME_GET_N_U( bufp )                                        \
                                                                            \
        ( (uint16)((bufp[1] & 0x07) << 6) | (uint16)((bufp[2] & 0xFC) >> 2) )

/* ........................................................................ */

#define LLC_UI_FRAME_SET_E( bufp, e )                                       \
                                                                            \
        { bufp[2] = (bufp[2] & ~0x02) | ((uint8)(e << 1) & 0x02); }

#define LLC_UI_FRAME_GET_E( bufp )                                          \
                                                                            \
        ( (bufp[2] & 0x02) >> 1 )

/* ........................................................................ */

#define LLC_UI_FRAME_SET_PM( bufp, pm )                                     \
                                                                            \
        { bufp[2] = (bufp[2] & ~0x01) | (pm & 0x01); }

#define LLC_UI_FRAME_GET_PM( bufp )                                         \
                                                                            \
        ( bufp[2] & 0x01 )

/* ------------------------------------------------------------------------ */

/*
   LLC U frame header manipulation macros.

                                Bit
            7     6     5     4     3     2     1     0    Octet
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |  1  |  1  |  1  | P/F |  M4 |  M3 |  M2 |  M1 |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+
*/

/* ........................................................................ */

#define LLC_U_FRAME_SET_PF( bufp, pf )                                      \
                                                                            \
        { bufp[1] = (bufp[1] & ~0x10) | ((uint8)(pf << 4) & 0x10); }

#define LLC_U_FRAME_GET_PF( bufp )                                          \
                                                                            \
        ( (uint8)((bufp[1] & 0x10) >> 4) )

/* ........................................................................ */

#define LLC_U_FRAME_SET_M4_M1( bufp, m4_m1 )                                \
                                                                            \
        { bufp[1] = (bufp[1] & ~0x0F) | (m4_m1 & 0x0F); }

#define LLC_U_FRAME_GET_M4_M1( bufp )                                       \
                                                                            \
        ( (gllc_unnumbered_t)(bufp[1] & 0x0F) )

/* ------------------------------------------------------------------------ */

/*
   LLC U frame xid information field manipulation macros.

                                Bit
            7     6     5     4     3     2     1     0    Octet
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |  XL |             TYPE            |  LENGTH   |   0
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |          LENGTH                   |  x  |  x  |   1
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |           XID Param High Order Octet          | 1 or 2
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |                      ...                      |  ...
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |           XID Param Low Order Octet           |   n
         +-----+-----+-----+-----+-----+-----+-----+-----+
*/

/* ........................................................................ */

#define LLC_U_FRAME_SET_XID_XL( xidp, xl )                                  \
                                                                            \
        { xidp[0] = (xidp[0] & ~0x80) | ((uint8)(xl << 7) & 0x80); }

#define LLC_U_FRAME_GET_XID_XL( xidp )                                      \
                                                                            \
        ( (uint8)((xidp[0] & 0x80) >> 7) )

/* ........................................................................ */

#define LLC_U_FRAME_SET_XID_TYPE( xidp, type )                              \
                                                                            \
        { xidp[0] = (bufp[0] & ~0x7C) | ((uint8)(type << 2) & 0x7C); }

#define LLC_U_FRAME_GET_XID_TYPE( xidp )                                    \
                                                                            \
        ( (uint8)((xidp[0] & 0x7C) >> 2) )

/* ........................................................................ */

#define LLC_U_FRAME_SET_XID_LEN_2BIT( xidp, len )                           \
                                                                            \
        { xidp[0] = (xidp[0] & ~0x03) | (len & 0x03); }

#define LLC_U_FRAME_GET_XID_LEN_2BIT( xidp )                                \
                                                                            \
        ( xidp[0] & 0x03 )

/* ........................................................................ */

#define LLC_U_FRAME_SET_XID_LEN_8BIT( xidp, len )                           \
        {                                                                   \
          xidp[0] = (xidp[0] & ~0x03) | ((uint8)(len >> 6) & 0x03);         \
          xidp[1] = (xidp[1] & ~0xFC) | ((uint8)(len << 2) & 0xFC);         \
        }

#define LLC_U_FRAME_GET_XID_LEN_8BIT( xidp )                                \
                                                                            \
        ( (uint8)((xidp[0] & 0x03) << 6) | (uint8)((xidp[1] & 0xFC) >> 2) )

/* ------------------------------------------------------------------------ */

/*
   LLC U frame FRMR information field manipulation macros.

                                Bit
            7     6     5     4     3     2     1     0    Octet
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |                                               |   0
         |                                               |
         |                                               |   1
         |                Rejected Frame                 |
         |                                               |   2
         |                Control Field                  |
         |                                               |   3
         |                (1st 6 octets)                 |
         |                                               |   4
         |                                               |
         |                                               |   5
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |  x  |  x  |  x  |  x  |          V(S)         |   6
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |             V(S)            |  x  |    V(R)   |   7
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |                   V(R)                  | C/R |   8
         +-----+-----+-----+-----+-----+-----+-----+-----+
         |  x  |  x  |  x  |  x  |  W4 |  W3 |  W2 |  W1 |   9
         +-----+-----+-----+-----+-----+-----+-----+-----+
*/

/* ........................................................................ */

#define LLC_U_FRAME_SET_FRMR_V_S( frmrp, v_s )                              \
        {                                                                   \
          frmrp[6] = (frmrp[6] & ~0x0F) | ((uint8)(v_s >> 5) & 0x0F);       \
          frmrp[7] = (frmrp[7] & ~0xF8) | ((uint8)(v_s << 3) & 0xF8);       \
        }

#define LLC_U_FRAME_GET_FRMR_V_S( frmrp )                                   \
                                                                            \
        ( (uint16)((frmrp[6] & 0x0F) << 5) | (uint16)((frmrp[7] & 0xF1) >> 3) )

/* ........................................................................ */

#define LLC_U_FRAME_SET_FRMR_V_R( frmrp, v_r )                              \
        {                                                                   \
          frmrp[7] = (frmrp[7] & ~0x07) | ((uint8)(v_r >> 7) & 0x07);       \
          frmrp[8] = (frmrp[8] & ~0xFE) | ((uint8)(v_r << 1) & 0xFE);       \
        }

#define LLC_U_FRAME_GET_FRMR_V_R( frmrp )                                   \
                                                                            \
        ( (uint16)((frmrp[7] & 0x07) << 7) | (uint16)((frmrp[8] & 0xFE) >> 1) )

/* ........................................................................ */


#define LLC_U_FRAME_SET_FRMR_CR( frmrp, cr )                                \
                                                                            \
        { frmrp[8] = (frmrp[8] & ~0x01) | (cr & 0x01); }

#define LLC_U_FRAME_GET_FRMR_CR( frmrp )                                    \
                                                                            \
        ( (gllc_cr_t)(frmrp[8] & 0x01) )

/* ........................................................................ */


#define LLC_U_FRAME_SET_FRMR_W4_W1( frmrp, w4_w1 )                          \
                                                                            \
        { frmrp[9] = (frmrp[9] & ~0x0F) | (w4_w1 & 0x0F); }

#define LLC_U_FRAME_GET_FRMR_W4_W1( frmrp )                                 \
                                                                            \
        ( frmrp[9] & 0x0F )

/*lint -restore*/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_ACCEPT_U_FRAME_CTRL_FIELD()
===
===  DESCRIPTION
===
===    Verifies that the U frame control field contains a valid command or
===    response.
===
===  DEPENDENCIES
===
===    TRUE if acceptable, FALSE otherwise.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

LOCAL boolean gllc_accept_u_frame_ctrl_field( gllc_u_frame_t *u_frame )
{
  if ( (u_frame -> cmd_res_M4_M1 == GLLC_SABM   ) &&
       (u_frame -> cmd_or_res_CR == GLLC_COMMAND)    )
  {
    return ( TRUE );
  }
  else if ( (u_frame -> cmd_res_M4_M1 == GLLC_UA      ) &&
            (u_frame -> cmd_or_res_CR == GLLC_RESPONSE)    )
  {
    return ( TRUE );
  }
  else if ( (u_frame -> cmd_res_M4_M1 == GLLC_DISC   ) &&
            (u_frame -> cmd_or_res_CR == GLLC_COMMAND)    )
  {
    return ( TRUE );
  }
  else if ( (u_frame -> cmd_res_M4_M1 == GLLC_DM      ) &&
            (u_frame -> cmd_or_res_CR == GLLC_RESPONSE)    )
  {
    return ( TRUE );
  }
  else if ( (u_frame -> cmd_res_M4_M1 == GLLC_FRMR    ) &&
            (u_frame -> cmd_or_res_CR == GLLC_RESPONSE)    )
  {
    return ( TRUE );
  }
  else if ( (u_frame -> cmd_res_M4_M1 == GLLC_NULL   ) &&
            (u_frame -> cmd_or_res_CR == GLLC_COMMAND)    )
  {
    return ( FALSE ); /* A NULL command is transmitted by the MS only. */
  }
  else if ( u_frame -> cmd_res_M4_M1 == GLLC_XID )
    return ( TRUE );

  else
    return ( FALSE );

} /* end of gllc_accept_u_frame_ctrl_field() */


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_ASSEMBLE_FRAME_HDR()
===
===  DESCRIPTION
===
===    Formats an appropriate frame header from the information collated in
===    the frame element of the LLC frame assembly working structure,
===    and appends it to the LL PDU element of the GRR message field of the
===    working structure.
===
===  DEPENDENCIES
===
===    Frame assembly working structure must contain all pertinent
===    information.
===
===    Must be called BEFORE gllc_assemble_frame_info() and BEFORE
===    gllc_assemble_frame_fcs().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_assemble_frame_hdr( gas_id_t gas_id )
{
  uint8 buf[37]    = {0}; /* Max frame size is for I+S frame SACK format */
  uint8 hdr_length = 0;   /* Assembled header length in octets.          */

  register gllc_frame_t *frame_p = gllc_assy_info[gas_id].frame;

  /* --------------------------------------------------------------------- */

  /* Clear relevent buffer workspace and set address field parameters. */

  LLC_FRAME_SET_PD( buf, 0 );

  LLC_FRAME_SET_SAPI( buf, (uint32)gllc_assy_info[gas_id].lle );

  /* --------------------------------------------------------------------- */

  switch ( frame_p -> format )
  {
  /* Information plus supervisory frame format. */

  case GLLC_I_S_FORMAT:

    if ( frame_p -> i_s_frame.cmd_or_res_CR == GLLC_COMMAND )
    {
      LLC_FRAME_SET_CR( buf, 0 );
    }
    else
    {
      LLC_FRAME_SET_CR( buf, 1 );
    }
    LLC_FRAME_SET_FORMAT_I_S( buf );
    LLC_I_S_FRAME_SET_A( buf, frame_p -> i_s_frame.ack_req_A );
    LLC_I_S_FRAME_SET_N_S( buf, frame_p -> i_s_frame.send_seq_num_N_S );
    LLC_I_S_FRAME_SET_N_R( buf, frame_p -> i_s_frame.recv_seq_num_N_R );
    LLC_I_S_FRAME_SET_S1S2( buf, (uint8)frame_p -> i_s_frame.cmd_res_S1S2 );

    if ( frame_p -> i_s_frame.cmd_res_S1S2 == GLLC_SACK )
    {
      /* An I + S frame header with a SACK command/response carries a
         selective acknowledgement bitmap which must be embodied within
         the header under assembly from the assembly info. */

      LLC_I_S_FRAME_SET_K( buf, frame_p -> i_s_frame.sack_bitmap_len_K );

      /* Locations 0 to 4 of buf are used by header. Remainder is (sizeof(buf) - 5). */
      (void)memscpy
      (
        &buf[5],
        ( sizeof(buf) - 5 ),
        gllc_assy_info[gas_id].sack_bitmap_R_p,
        frame_p -> i_s_frame.sack_bitmap_len_K + 1
      );

      hdr_length = 5 + (frame_p -> i_s_frame.sack_bitmap_len_K + 1);
    }
    else
      hdr_length = 4;
    break;

  /* --------------------------------------------------------------------- */

  /* Supervisory frame format. */

  case GLLC_S_FORMAT:

    if ( frame_p -> s_frame.cmd_or_res_CR == GLLC_COMMAND )
    {
      LLC_FRAME_SET_CR( buf, 0 );
    }
    else
    {
      LLC_FRAME_SET_CR( buf, 1 );
    }
    LLC_FRAME_SET_FORMAT_S( buf );
    LLC_S_FRAME_SET_A( buf, frame_p -> s_frame.ack_req_A );
    LLC_S_FRAME_SET_N_R( buf, frame_p -> s_frame.recv_seq_num_N_R );
    LLC_S_FRAME_SET_S1S2( buf, (uint8)frame_p -> s_frame.cmd_res_S1S2 );

    if ( frame_p -> s_frame.cmd_res_S1S2 == GLLC_SACK )
    {

      /* Locations 0 to 2 of buf are used by header. Remainder is (sizeof(buf) - 3). */

      (void)memscpy
      (
        &buf[3],
        (sizeof(buf) - 3),
        gllc_assy_info[gas_id].sack_bitmap_R_p,
        frame_p -> s_frame.sack_bitmap_len_K + 1
      );
      hdr_length = 3 + (frame_p -> s_frame.sack_bitmap_len_K + 1);
    }
    else
      hdr_length = 3;
    break;

  /* --------------------------------------------------------------------- */

  /* Unconfirmed information frame format. */

  case GLLC_UI_FORMAT:

    if ( frame_p -> ui_frame.cmd_or_res_CR == GLLC_COMMAND )
    {
      LLC_FRAME_SET_CR( buf, 0 );
    }
    else
    {
      LLC_FRAME_SET_CR( buf, 1 );
    }
    LLC_FRAME_SET_FORMAT_UI( buf );
    LLC_UI_FRAME_SET_N_U( buf, frame_p -> ui_frame.send_seq_num_N_U );
    LLC_UI_FRAME_SET_E( buf, frame_p -> ui_frame.encrypted_E );
    LLC_UI_FRAME_SET_PM( buf, frame_p -> ui_frame.protected_mode_PM );

    hdr_length = 3;
    break;

  /* --------------------------------------------------------------------- */

  /* Unnumbered frame format. */

  case GLLC_U_FORMAT:

    if ( frame_p -> u_frame.cmd_or_res_CR == GLLC_COMMAND )
    {
      LLC_FRAME_SET_CR( buf, 0 );
    }
    else
    {
      LLC_FRAME_SET_CR( buf, 1 );
    }
    LLC_FRAME_SET_FORMAT_U( buf );
    LLC_U_FRAME_SET_PF( buf, frame_p -> u_frame.poll_final_PF );
    LLC_U_FRAME_SET_M4_M1( buf, (uint8)frame_p -> u_frame.cmd_res_M4_M1 );

    hdr_length = 2;
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_NO_FORMAT:
  default:

    MSG_GERAN_ERROR_1_G( "GLC GRR Ass. Hdr: Invalid fr fmt: %d",
      frame_p -> format);
    break;

  /* --------------------------------------------------------------------- */
  }

  /* Calculate partial FCS and append the header to the message body. */

  gllc_ass_frame_header_fcs( gas_id, buf, hdr_length );

  (void)gprs_pdu_append_head(
    &gllc_assy_info[gas_id].grr_ul_mes -> pdu_ptr, buf, hdr_length );

  gllc_assy_info[gas_id].fr_hdr_length = hdr_length;

  /* --------------------------------------------------------------------- */

} /* end of gllc_assemble_frame_hdr() */


/*===========================================================================
===
===  FUNCTION      GLLC_ASSEMBLE_FRAME_INFO_AND_FCS()
===
===  DESCRIPTION
===
===    Formats an appropriate frame information field from the information
===    collated in the frame element of the LLC frame assembly working
===    structure, calculates an FCS and enciphers if required. The
===    information and FCS fields are then appended to the LL PDU element
===    of the GRR message field of the working structure.
===
===  DEPENDENCIES
===
===    Frame assembly working structure must contain all pertinent
===    information.
===
===    Must be called AFTER gllc_assemble_frame_hdr().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_assemble_frame_info_and_fcs( gas_id_t gas_id )
{
  uint8         buf[ 38 ];    /* Maximum size of an XID information field. */
  uint8         *bufp;
  uint16        fcs_length   = 0;
  dsm_item_type *l3_pdu_temp = NULL;

  register gllc_frame_t     *frame_p = gllc_assy_info[gas_id].frame;
  register gllc_xid_info_t  *xid_info_p;
  register gllc_frmr_info_t *frmr_info_p;
  register uint32           gea_input_seed;

  /* --------------------------------------------------------------------- */

  switch ( frame_p -> format )
  {
  /* Information plus supervisory frame format. */

  case GLLC_I_S_FORMAT:

    /* Determine whether this frame requires enciphering. */

    if ( (gllc_llme[gas_id].gea != GMMLLC_GEA_NONE     ) &&
         (gllc_llme[gas_id].gea != GMMLLC_GEA_UNDEFINED)    )
    {
      /* Enciphering is specified, so firstly calculate the encryption
         algorithm initialisation seed (termed INPUT).

         Input = (IOV_UI + LFN + OC) Mod32 */

      gea_input_seed =
      (uint32)
      (
        gllc_lle[gas_id][ gllc_assy_info[gas_id].lle ].ack.input_offset_IOV_I
        +
        frame_p -> i_s_frame.send_seq_num_N_S                 /* LFN */
        +
        frame_p -> i_s_frame.cipher_oc                        /* OC  */
      );

      /* A new copy chain must be created, as part of a combined
         FCS/encipher/copy operation with FCS append. */

      gllc_ass_frame_info_fcs_and_encipher
      (
        gas_id,
        frame_p -> i_s_frame.l3_pdu,
        &l3_pdu_temp,
        gllc_assy_info[gas_id].fr_info_length,
        gllc_assy_info[gas_id].fr_info_length,
        gea_input_seed
      );
    }
    else
    {
      /* Enciphering is not required and so a duplicate copy chain must be
         created, followed by an FCS calculation and FCS append operation. */

      l3_pdu_temp = gprs_pdu_duplicate(
        frame_p -> i_s_frame.l3_pdu, 0, gllc_assy_info[gas_id].fr_info_length );

      gllc_ass_frame_info_fcs( gas_id, &l3_pdu_temp, gllc_assy_info[gas_id].fr_info_length );
    }

    /* Append the information and FCS fields to the header. */

    gprs_pdu_augment_tail
    (
      &gllc_assy_info[gas_id].grr_ul_mes -> pdu_ptr,
      &l3_pdu_temp
    );
    break;

  /* --------------------------------------------------------------------- */

  /* Supervisory frame format. */

  case GLLC_S_FORMAT:

    /* The LLC supervisory frame contains no information field, so only
       an FCS field append operation is required. */

    gllc_assy_info[gas_id].fr_info_length = 0;
    gllc_ass_frame_info_fcs( gas_id, &l3_pdu_temp, gllc_assy_info[gas_id].fr_info_length );

    /* Append the FCS field to the header. */

    gprs_pdu_augment_tail
    (
      &gllc_assy_info[gas_id].grr_ul_mes -> pdu_ptr,
      &l3_pdu_temp
    );
    break;

  /* --------------------------------------------------------------------- */

  /* Unconfirmed information frame format. */

  case GLLC_UI_FORMAT:

    /* Determine the information field FCS calculation length. */

    if ( !frame_p -> ui_frame.protected_mode_PM )
    {
      fcs_length = gllc_n202_lookup[ gllc_llme[gas_id].llc_version ];

      if ( fcs_length > gllc_assy_info[gas_id].fr_info_length )
        fcs_length = gllc_assy_info[gas_id].fr_info_length;
    }
    else
      fcs_length = gllc_assy_info[gas_id].fr_info_length;

    /* ................................................................... */

    /* Determine whether this frame requires enciphering. */

    if ( frame_p -> ui_frame.encrypted_E )
    {
      /* Enciphering is specified, so firstly calculate the encryption
         algorithm initialisation seed (termed INPUT).

         Input = ((IOV_UI xor ((2^27 * SAPI) + 2^31)) + LFN + OC) Mod32 */

      gea_input_seed =
      (uint32)
      (
        (
          gllc_llme[gas_id].input_offset_IOV_UI
          ^
          (
            (
              gllc_sapi_addr_lookup[gllc_assy_info[gas_id].lle] *   /* SAPI */
              0x08000000UL                                  /* 2^27 */
            )
            +
            0x80000000UL                                    /* 2^31 */
          )
        )
        +
        frame_p -> ui_frame.send_seq_num_N_U                /* LFN */
        +
        frame_p -> ui_frame.cipher_oc                       /* OC  */
      );

      /* Now perform a combined FCS/encipher/copy operation, and FCS append.
         The free copy operation protects any duplicate references of upper
         layer protocols from corruption due to the effects of enciphering. */

      gllc_ass_frame_info_fcs_and_encipher
      (
        gas_id,
        frame_p -> ui_frame.l3_pdu,
        &l3_pdu_temp,
        gllc_assy_info[gas_id].fr_info_length,
        fcs_length,
        gea_input_seed
      );

      gprs_pdu_augment_tail(
        &gllc_assy_info[gas_id].grr_ul_mes -> pdu_ptr, &l3_pdu_temp );

      GPRS_PDU_FREE( &frame_p -> ui_frame.l3_pdu );
    }

    /* ................................................................... */

    else
    {
      /* No enciphering is specified so perform an FCS calculation and FCS
         append operation only. */

      gllc_ass_frame_info_fcs( gas_id, &frame_p -> ui_frame.l3_pdu, fcs_length );

      gprs_pdu_augment_tail
      (
        &gllc_assy_info[gas_id].grr_ul_mes -> pdu_ptr,
        &frame_p -> ui_frame.l3_pdu
      );
    }
    break;

  /* --------------------------------------------------------------------- */

  /* Unnumbered frame format. */

  case GLLC_U_FORMAT:

    if ( (frame_p -> u_frame.cmd_res_M4_M1 == GLLC_SABM) ||
         (frame_p -> u_frame.cmd_res_M4_M1 == GLLC_UA  ) ||
         (frame_p -> u_frame.cmd_res_M4_M1 == GLLC_XID )    )
    {
      /* These commands/responses may carry an XID parameter negotiation
         information field. */

      if ( frame_p -> u_frame.xid != GLLC_XID_NONE )
      {
        xid_info_p = &frame_p -> u_frame.info_field.xid_info;

        #define XID_SHORT_HEADER    1
        #define XID_LONG_HEADER     2

        /* Prepare working buffer and associated variables. */

        (void)memset( buf, 0, 38 );
        bufp = buf;

        /* For each XID parameter involved in the negotiation, build an
           information field element comprising an XID paramater header
           and an XID parameter body. */

        /* ............................................................... */

        /* Append any RESET XID parameters - *** MUST BE FIRST *** */

        if ( xid_info_p -> contents & GLLC_RESET_XID   )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_RESET_XID_TYPE );
          LLC_U_FRAME_SET_XID_LEN_2BIT( bufp, 0 );

          bufp += XID_SHORT_HEADER;
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_VERSION_XID )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_VERSION_XID_TYPE );
          LLC_U_FRAME_SET_XID_LEN_2BIT( bufp, GLLC_VERSION_XID_SIZE );

          bufp[1] = xid_info_p -> version_xid;

          bufp += (XID_SHORT_HEADER + GLLC_VERSION_XID_SIZE);
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_IOV_UI_XID  )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 1 );    /* 8 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_IOV_UI_XID_TYPE );
/*lint -save -e572 -e778 Quieten lint for this particular usage of this macro */
          LLC_U_FRAME_SET_XID_LEN_8BIT( bufp, GLLC_IOV_UI_XID_SIZE );
/*lint -restore*/

          bufp[2] = (uint8)(xid_info_p -> iov_ui_xid >> 24);
          bufp[3] = (uint8)(xid_info_p -> iov_ui_xid >> 16);
          bufp[4] = (uint8)(xid_info_p -> iov_ui_xid >>  8);
          bufp[5] = (uint8)(xid_info_p -> iov_ui_xid >>  0);

          bufp += (XID_LONG_HEADER + GLLC_IOV_UI_XID_SIZE);
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_IOV_I_XID   )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 1 );    /* 8 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_IOV_I_XID_TYPE );
/*lint -save -e572 -e778 Quieten lint for this particular usage of this macro */
          LLC_U_FRAME_SET_XID_LEN_8BIT( bufp, GLLC_IOV_I_XID_SIZE );
/*lint -restore*/

          bufp[2] = (uint8)(xid_info_p -> iov_i_xid >> 24);
          bufp[3] = (uint8)(xid_info_p -> iov_i_xid >> 16);
          bufp[4] = (uint8)(xid_info_p -> iov_i_xid >>  8);
          bufp[5] = (uint8)(xid_info_p -> iov_i_xid >>  0);

          bufp += (XID_LONG_HEADER + GLLC_IOV_I_XID_SIZE);
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_T200_XID    )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_T200_XID_TYPE );
          LLC_U_FRAME_SET_XID_LEN_2BIT( bufp, GLLC_T200_XID_SIZE );

          bufp[1] = (uint8)(xid_info_p -> t200_xid >> 8);
          bufp[2] = (uint8)(xid_info_p -> t200_xid >> 0);

          bufp += (XID_SHORT_HEADER + GLLC_T200_XID_SIZE);
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_N200_XID    )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_N200_XID_TYPE );
          LLC_U_FRAME_SET_XID_LEN_2BIT( bufp, GLLC_N200_XID_SIZE );

          bufp[1] = xid_info_p -> n200_xid;

          bufp += (XID_SHORT_HEADER + GLLC_N200_XID_SIZE);
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_N201_U_XID  )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_N201_U_XID_TYPE );
          LLC_U_FRAME_SET_XID_LEN_2BIT( bufp, GLLC_N201_U_XID_SIZE );

          bufp[1] = (uint8)(xid_info_p -> n201_u_xid >> 8);
          bufp[2] = (uint8)(xid_info_p -> n201_u_xid >> 0);

          bufp += (XID_SHORT_HEADER + GLLC_N201_U_XID_SIZE);
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_N201_I_XID  )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_N201_I_XID_TYPE );
          LLC_U_FRAME_SET_XID_LEN_2BIT( bufp, GLLC_N201_I_XID_SIZE );

          bufp[1] = (uint8)(xid_info_p -> n201_i_xid >> 8);
          bufp[2] = (uint8)(xid_info_p -> n201_i_xid >> 0);

          bufp += (XID_SHORT_HEADER + GLLC_N201_I_XID_SIZE);
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_mD_XID      )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_mD_XID_TYPE );
          LLC_U_FRAME_SET_XID_LEN_2BIT( bufp, GLLC_mD_XID_SIZE );

          bufp[1] = (uint8)(xid_info_p -> md_xid >> 8);
          bufp[2] = (uint8)(xid_info_p -> md_xid >> 0);

          bufp += (XID_SHORT_HEADER + GLLC_mD_XID_SIZE);
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_mU_XID      )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_mU_XID_TYPE );
          LLC_U_FRAME_SET_XID_LEN_2BIT( bufp, GLLC_mU_XID_SIZE );

          bufp[1] = (uint8)(xid_info_p -> mu_xid >> 8);
          bufp[2] = (uint8)(xid_info_p -> mu_xid >> 0);

          bufp += (XID_SHORT_HEADER + GLLC_mU_XID_SIZE);
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_kD_XID      )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_kD_XID_TYPE );
          LLC_U_FRAME_SET_XID_LEN_2BIT( bufp, GLLC_kD_XID_SIZE );

          bufp[1] = xid_info_p -> kd_xid;

          bufp += (XID_SHORT_HEADER + GLLC_kD_XID_SIZE);
        }

        /* ............................................................... */

        if ( xid_info_p -> contents & GLLC_kU_XID      )
        {
          LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
          LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_kU_XID_TYPE );
          LLC_U_FRAME_SET_XID_LEN_2BIT( bufp, GLLC_kU_XID_SIZE );

          bufp[1] = xid_info_p -> ku_xid;

          bufp += (XID_SHORT_HEADER + GLLC_kU_XID_SIZE);
        }

        /* ............................................................... */

        /* Append any Layer 3 XID parameters - *** MUST BE LAST *** */

        if ( xid_info_p -> contents & GLLC_LAY3_XID    )
        {
          if ( xid_info_p -> l3_xid.length <= 3 )
          {
            LLC_U_FRAME_SET_XID_XL( bufp, 0 );    /* 2 bit length field. */
            LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_LAY3_XID_TYPE );
            LLC_U_FRAME_SET_XID_LEN_2BIT(
              bufp, xid_info_p->l3_xid.length );

            bufp += XID_SHORT_HEADER;
          }
          else
          {
            LLC_U_FRAME_SET_XID_XL( bufp, 1 );    /* 8 bit length field. */
            LLC_U_FRAME_SET_XID_TYPE( bufp, GLLC_LAY3_XID_TYPE );
            LLC_U_FRAME_SET_XID_LEN_8BIT(
              bufp, xid_info_p->l3_xid.length );

            bufp += XID_LONG_HEADER;
          }

          /* Load the current working buffer contents to a PDU chain, and
             augment this with a duplicate copy of the layer 3 XID
             parameter PDU chain. */

          l3_pdu_temp = gllc_pdu_construct( buf, (uint16)(bufp - buf) );

          if ( xid_info_p -> l3_xid.length != 0 )
          {
            dsm_item_type *xid_dup = gprs_pdu_duplicate(
              xid_info_p -> l3_xid.params, 0, xid_info_p -> l3_xid.length );

            gprs_pdu_augment_tail( &l3_pdu_temp, &xid_dup );
          }

          gllc_assy_info[gas_id].fr_info_length =
            (uint16)(bufp - buf + xid_info_p -> l3_xid.length);
        }

        /* ............................................................... */

        else
        {
          /* Load the current working buffer contents to PDU chain. */

          l3_pdu_temp = gllc_pdu_construct( buf, (uint16)(bufp - buf) );
          gllc_assy_info[gas_id].fr_info_length = (uint16)(bufp - buf);
        }
      }
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    else if ( frame_p -> u_frame.cmd_res_M4_M1 == GLLC_FRMR )
    {
      /* This response carries a frame reject status information field. */

      frmr_info_p = &frame_p -> u_frame.info_field.frmr_info;

      /* Prepare working buffer and associated variables. */

      (void)memset( buf, 0, 10 );

      /* Construct the FRMR information field in the working buffer. */

      (void)memscpy( buf, sizeof(buf), frmr_info_p -> rej_fr_ctrl_field, 6 );

      LLC_U_FRAME_SET_FRMR_V_S( buf, frmr_info_p -> send_state_var_V_S );
      LLC_U_FRAME_SET_FRMR_V_R( buf, frmr_info_p -> recv_state_var_V_R );
      LLC_U_FRAME_SET_FRMR_W4_W1( buf, frmr_info_p -> reject_status_W4_W1);

      if ( frmr_info_p -> rejected_frame_CR == GLLC_COMMAND )
      {
        LLC_U_FRAME_SET_FRMR_CR( buf, 0 );
      }
      else
      {
        LLC_U_FRAME_SET_FRMR_CR( buf, 1 );
      }

      /* Load the current working buffer contents to PDU chain. */

      l3_pdu_temp = gllc_pdu_construct( buf, 10 );
      gllc_assy_info[gas_id].fr_info_length = 10;
    }

    /* Perform an FCS calculation and FCS append operation, before appending
       the information and FCS fields to the header. */

    gllc_ass_frame_info_fcs( gas_id, &l3_pdu_temp, gllc_assy_info[gas_id].fr_info_length );
    gprs_pdu_augment_tail(
      &gllc_assy_info[gas_id].grr_ul_mes -> pdu_ptr, &l3_pdu_temp );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_NO_FORMAT:
  default:

    MSG_GERAN_ERROR_1_G( "GLC GRR Ass. Info: Invalid fr fmt: %d",
      frame_p -> format);
    break;

  }

  /* --------------------------------------------------------------------- */

} /* end of gllc_assemble_frame_info_and_fcs() */


/*===========================================================================
===
===  FUNCTION      GLLC_DISASSEMBLE_FRAME_HDR()
===
===  DESCRIPTION
===
===    Parses the LL PDU header of a received GRR message and extracts the
===    header fields to the various elements of the given working structure.
===
===  DEPENDENCIES
===
===    Given working structure must contain all pertinent information.
===
===    Must be called BEFORE gllc_disassemble_frame_info().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_disassemble_frame_hdr( gas_id_t gas_id )
{
  uint8         buf[37]; /* Max frame size is for the I+S frame SACK format*/
  dsm_item_type **ll_pdu_ptr;
  uint16        temp;

  /* --------------------------------------------------------------------- */

  ll_pdu_ptr = &gllc_disassy_info[gas_id].grr_dl_mes -> ll_pdu;

  /* Make sure the first 7 octets of the receiving buffer are cleared to
     ensure a consistent frame reject control field is prepared. */

  (void)memset( buf, 0, 7 );

  /* Strip off the minimum frame header size (equivalent to a U frame) in
     order to establish the frame format and address field parameters. */

  if ( !gprs_pdu_strip_head( ll_pdu_ptr, &buf[0], 2 ) )
    gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_SHORT_ERR;

  /* Prepare a frame reject ctrl field block in case of a FRMR condition. */

  gllc_disassy_info[gas_id].rej_fr_ctrl_field[0] = buf[1];  /* Exclude addr field. */

  /* Disassemble frame header. */

  gllc_disassy_info[gas_id].lle = LLC_FRAME_GET_SAPI(buf);

  if ( (gllc_disassy_info[gas_id].lle   == GLLC_LL_SAPI_RESERVED) ||
       (LLC_FRAME_GET_PD( buf ) == 1                    )    )
  {
    gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_ADDR_ERR;
  }
  gllc_disassy_info[gas_id].frame.format = LLC_FRAME_GET_FORMAT( buf );

  switch ( gllc_disassy_info[gas_id].frame.format )
  {
  /* ..................................................................... */

  case GLLC_I_S_FORMAT:           /* Information plus supervisory format.  */

    /* Strip two more octets from the LL PDU to complete the I+S frame
       header (excluding any SACK bitmap that might be present), and
       continue preparing a frame reject control field block in case of a
       FRMR condition. */

    if ( !gprs_pdu_strip_head( ll_pdu_ptr, &buf[2], 2 ) )
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_SHORT_ERR;

    gllc_disassy_info[gas_id].rej_fr_ctrl_field[1] = buf[2];/* Exclude addr field. */
    gllc_disassy_info[gas_id].rej_fr_ctrl_field[2] = buf[3];

    if ( LLC_FRAME_GET_CR( buf ) == 1 )
      gllc_disassy_info[gas_id].frame.i_s_frame.cmd_or_res_CR = GLLC_COMMAND;
    else
      gllc_disassy_info[gas_id].frame.i_s_frame.cmd_or_res_CR = GLLC_RESPONSE;

    gllc_disassy_info[gas_id].rejected_frame_CR =
      gllc_disassy_info[gas_id].frame.i_s_frame.cmd_or_res_CR; /* In case of FRMR. */

    gllc_disassy_info[gas_id].frame.i_s_frame.ack_req_A =
      LLC_I_S_FRAME_GET_A( buf );
    gllc_disassy_info[gas_id].frame.i_s_frame.send_seq_num_N_S =
      LLC_I_S_FRAME_GET_N_S( buf );
    gllc_disassy_info[gas_id].frame.i_s_frame.recv_seq_num_N_R =
      LLC_I_S_FRAME_GET_N_R( buf );
    gllc_disassy_info[gas_id].frame.i_s_frame.cmd_res_S1S2 =
      LLC_I_S_FRAME_GET_S1S2( buf );

    /* Extract any SACK bitmap that may be present and calculate partial
       FCS over the header. */

    if ( gllc_disassy_info[gas_id].frame.i_s_frame.cmd_res_S1S2 == GLLC_SACK )
    {
      /* A selective acknowledgement bitmap is attached to the header so
         establish the bitmap length from the K indicator, and extract the
         bitmap. */

      if ( !gprs_pdu_strip_head( ll_pdu_ptr, &buf[4], 1 ) )
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_SHORT_ERR;

      gllc_disassy_info[gas_id].rej_fr_ctrl_field[3] = buf[4];
      gllc_disassy_info[gas_id].frame.i_s_frame.sack_bitmap_len_K =
        LLC_I_S_FRAME_GET_K( buf );

      if
      (
        !gprs_pdu_strip_head
        (
          ll_pdu_ptr, &buf[5],
          (uint16)(gllc_disassy_info[gas_id].frame.i_s_frame.sack_bitmap_len_K + 1)
        )
      )
      {
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_SHORT_ERR;
      }

      /* Continue preparing frame reject control field block just in case. */

      gllc_disassy_info[gas_id].rej_fr_ctrl_field[4] = buf[5];
      gllc_disassy_info[gas_id].rej_fr_ctrl_field[5] = buf[6];

      /* Copy the SACK bitmap to it's intended destination. */

      (void)memscpy
      (
        gllc_disassy_info[gas_id].frame.i_s_frame.sack_bitmap_R,
        sizeof(gllc_disassy_info[gas_id].frame.i_s_frame.sack_bitmap_R),
        &buf[5],
        gllc_disassy_info[gas_id].frame.i_s_frame.sack_bitmap_len_K + 1
      );

      gllc_dis_frame_header_fcs
      (
        gas_id,
        buf,
        (uint16)(5 + gllc_disassy_info[gas_id].frame.i_s_frame.sack_bitmap_len_K + 1)
      );
    }
    else
      gllc_dis_frame_header_fcs( gas_id, buf, 4 );
    break;

  /* ..................................................................... */

  case GLLC_S_FORMAT:            /* Supervisory format.                    */

    /* Strip one more octet from the LL PDU to complete the S frame
       header (excluding any SACK bitmap that might be present), and
       continue preparing a frame reject control field block in case of a
       FRMR condition. */

    if ( !gprs_pdu_strip_head( ll_pdu_ptr, &buf[2], 1 ) )
    {
      /* Supervisory frame has an incorrect length - too short. */

      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FRMR_ERR;
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_FRMR_W1_INVALID_I_FIELD;
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_FRMR_W3_INVALID_CTRL_FIELD;
    }

    gllc_disassy_info[gas_id].rej_fr_ctrl_field[1] = buf[2];/* Exclude addr field. */

    if ( LLC_FRAME_GET_CR( buf ) == 1 )
      gllc_disassy_info[gas_id].frame.s_frame.cmd_or_res_CR = GLLC_COMMAND;
    else
      gllc_disassy_info[gas_id].frame.s_frame.cmd_or_res_CR = GLLC_RESPONSE;

    gllc_disassy_info[gas_id].rejected_frame_CR =
      gllc_disassy_info[gas_id].frame.s_frame.cmd_or_res_CR; /* In case of FRMR. */

    gllc_disassy_info[gas_id].frame.s_frame.ack_req_A =
      LLC_S_FRAME_GET_A( buf );
    gllc_disassy_info[gas_id].frame.s_frame.recv_seq_num_N_R =
      LLC_S_FRAME_GET_N_R( buf );
    gllc_disassy_info[gas_id].frame.s_frame.cmd_res_S1S2 =
      LLC_S_FRAME_GET_S1S2( buf );

    /* Extract any SACK bitmap that may be present and calculate partial
       FCS over the header. */

    if ( gllc_disassy_info[gas_id].frame.s_frame.cmd_res_S1S2 == GLLC_SACK )
    {
      /* A selective acknowledgement bitmap is attached to the header so
         establish length (accounting for 3 FCS octets) and strip. */

      temp = GPRS_PDU_LENGTH( *ll_pdu_ptr );

      if ( temp < (1 + 3) )
      {
        /* Supervisory frame has an incorrect length - too short. */

        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FRMR_ERR;
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_FRMR_W1_INVALID_I_FIELD;
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_FRMR_W3_INVALID_CTRL_FIELD;

        gllc_disassy_info[gas_id].frame.s_frame.sack_bitmap_len_K = 0;
      }
      else if ( temp > (GLLC_SACK_BMP_LEN + 3) )
      {
        /* Supervisory frame has an incorrect length - too long. */

        gllc_disassy_info[gas_id].frame.s_frame.sack_bitmap_len_K =
          GLLC_SACK_BMP_LEN - 1;
        (void)gprs_pdu_strip_head( ll_pdu_ptr, &buf[3], GLLC_SACK_BMP_LEN );
      }
      else
      {
        /* Supervisory frame has an in range length. */

        gllc_disassy_info[gas_id].frame.s_frame.sack_bitmap_len_K = (uint8)(temp - 3 - 1);
        (void)gprs_pdu_strip_head( ll_pdu_ptr, &buf[3], (uint16)(temp - 3) );
      }

      /* Continue preparing frame reject control field block just in case. */

      gllc_disassy_info[gas_id].rej_fr_ctrl_field[2] = buf[3];
      gllc_disassy_info[gas_id].rej_fr_ctrl_field[3] = buf[4];
      gllc_disassy_info[gas_id].rej_fr_ctrl_field[4] = buf[5];
      gllc_disassy_info[gas_id].rej_fr_ctrl_field[5] = buf[6];

      /* Copy the SACK bitmap to it's intended destination. */

      (void)memscpy
      (
        gllc_disassy_info[gas_id].frame.s_frame.sack_bitmap_R,
        sizeof(gllc_disassy_info[gas_id].frame.s_frame.sack_bitmap_R),
        &buf[3],
        (gllc_disassy_info[gas_id].frame.s_frame.sack_bitmap_len_K + 1)
      );

      gllc_dis_frame_header_fcs
      (
        gas_id,
        buf,
        (uint16)(3 + gllc_disassy_info[gas_id].frame.s_frame.sack_bitmap_len_K + 1)
      );
    }
    else
      gllc_dis_frame_header_fcs( gas_id, buf, 3 );
    break;

  /* ..................................................................... */

  case GLLC_UI_FORMAT:           /* Unconfirmed information format.        */

    /* Strip one more octet from the LL PDU to complete the UI frame
       header, and continue preparing a frame reject control field block in
       case of a FRMR condition. */

    if ( !gprs_pdu_strip_head( ll_pdu_ptr, &buf[2], 1 ) )
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_SHORT_ERR;

    gllc_disassy_info[gas_id].rej_fr_ctrl_field[1] = buf[2]; /* Exclude addr field. */

    if ( LLC_FRAME_GET_CR( buf ) == 1 )
      gllc_disassy_info[gas_id].frame.ui_frame.cmd_or_res_CR = GLLC_COMMAND;
    else
      gllc_disassy_info[gas_id].frame.ui_frame.cmd_or_res_CR = GLLC_RESPONSE;

    gllc_disassy_info[gas_id].rejected_frame_CR =
      gllc_disassy_info[gas_id].frame.ui_frame.cmd_or_res_CR; /* In case of FRMR. */

    gllc_disassy_info[gas_id].frame.ui_frame.send_seq_num_N_U  =
      LLC_UI_FRAME_GET_N_U( buf );
    gllc_disassy_info[gas_id].frame.ui_frame.encrypted_E =
      LLC_UI_FRAME_GET_E( buf );
    gllc_disassy_info[gas_id].frame.ui_frame.protected_mode_PM =
      LLC_UI_FRAME_GET_PM( buf );

    gllc_dis_frame_header_fcs( gas_id, buf, 3 );
    break;

  /* ..................................................................... */

  case GLLC_U_FORMAT:            /* Unnumbered format.                     */

    if ( LLC_FRAME_GET_CR( buf ) == 1 )
      gllc_disassy_info[gas_id].frame.u_frame.cmd_or_res_CR = GLLC_COMMAND;
    else
      gllc_disassy_info[gas_id].frame.u_frame.cmd_or_res_CR = GLLC_RESPONSE;

    gllc_disassy_info[gas_id].rejected_frame_CR =
      gllc_disassy_info[gas_id].frame.u_frame.cmd_or_res_CR; /* In case of FRMR. */

    gllc_disassy_info[gas_id].frame.u_frame.poll_final_PF =
      LLC_U_FRAME_GET_PF( buf );
    gllc_disassy_info[gas_id].frame.u_frame.cmd_res_M4_M1 =
      LLC_U_FRAME_GET_M4_M1( buf );

    /* Verify the validity of the U frame control field and flag a frame
       reject condition if invalid. */

    if ( !gllc_accept_u_frame_ctrl_field(&gllc_disassy_info[gas_id].frame.u_frame) )
    {
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FRMR_ERR;
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_FRMR_W3_INVALID_CTRL_FIELD;
    }

    gllc_dis_frame_header_fcs( gas_id, buf, 2 );
    break;

  /* ..................................................................... */

  case GLLC_NO_FORMAT:
  default:

    /* Flag a frame reject condition. */

    gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FRMR_ERR;
    gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_FRMR_W3_INVALID_CTRL_FIELD;
    break;
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_disassemble_frame_hdr() */


/*===========================================================================
===
===  FUNCTION      GLLC_DISASSEMBLE_FRAME_INFO_AND_FCS()
===
===  DESCRIPTION
===
===    Deciphers, strips and checks FCS, and extracts the remaining
===    information field from the LL PDU to the frame disassembly working
===    structure.
===
===  DEPENDENCIES
===
===    Frame disassembly working structure must contain all pertinent
===    information.
===
===    Must be called AFTER gllc_disassemble_frame_hdr().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_disassemble_frame_info_and_fcs( gas_id_t gas_id )
{
  uint8           buf[ 255 ];   /* Maximum size of an XID information field */
  uint8           xid_ctr;
  boolean         xid_xl;
  uint8           xid_length;
  uint8           xid_type;
  boolean         frmr_condition = FALSE;
  uint32          gea_input_seed;
  uint16          fcs_length;
  uint16          temp;
  uint32          decipher_oc;
  uint32          decipher_reattempt_countdown = 0;
  dsm_item_type   *ll_pdu_copy_ptr = NULL;
  dsm_item_type   *ll_pdu_dup_ptr  = NULL;

  register dsm_item_type    **ll_pdu_ptr;
  register gllc_xid_info_t  *xid_info_p;

  /* ===================================================================== */

  /* Establish remaining information field length. Account for 3 FCS
     octets. */

  ll_pdu_ptr  = &gllc_disassy_info[gas_id].grr_dl_mes -> ll_pdu;
  temp        = GPRS_PDU_LENGTH( *ll_pdu_ptr );

  if ( temp >= 3 )
  {
    gllc_disassy_info[gas_id].fr_info_length = temp - 3;

    /* Obtain a debug sample of the first few octets of the information
       field. This may be usefull in debugging ciphering problems later. */

    temp = gllc_disassy_info[gas_id].fr_info_length;

    if ( temp > sizeof(gllc_disassy_info[gas_id].fr_info_debug_sample) )
      temp = sizeof(gllc_disassy_info[gas_id].fr_info_debug_sample);

    ll_pdu_dup_ptr = gprs_pdu_duplicate( *ll_pdu_ptr, 0, temp );
    (void)gprs_pdu_strip_head(
      &ll_pdu_dup_ptr, gllc_disassy_info[gas_id].fr_info_debug_sample, temp );
  }
  else
  {
    /* Insufficient FCS octets. No point continuing with this frame. */

    gllc_disassy_info[gas_id].fr_info_length  = 0;
    gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_SHORT_ERR;
    return;                                                  /*** RETURN ***/
  }


  /* ===================================================================== */

  /* If a PDP is not active on the SAPI indicated in DL frame, then DL UI  
     frames on that SAPI should not be accepted given that indicated SAPI is
     either SAPI 3, 5, 9, or 11. Report an ADDR error and return. */

  if ( GLLC_LLE_IS_3_5_9_11( gllc_disassy_info[gas_id].lle )) 
  {
    if ( !(gllc_lle[gas_id][gllc_disassy_info[gas_id].lle].gen.pdp_active  ) &&
          (gllc_disassy_info[gas_id].frame.format == GLLC_UI_FORMAT)    )
    {
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_ADDR_ERR;
      return;                                                /*** RETURN ***/
    }
  }

  /* ===================================================================== */

  switch ( gllc_disassy_info[gas_id].frame.format )
  {
  case GLLC_I_S_FORMAT:           /* Information plus supervisory format.  */

    /* If GMM has specified an encryption algorithm for this TLLI, then
       calculate the cipher algorithm input seed in preparation for frame
       deciphering, FCS calculation and strip, and test for FCS error. */

    if ( (gllc_llme[gas_id].gea != GMMLLC_GEA_NONE     ) &&
         (gllc_llme[gas_id].gea != GMMLLC_GEA_UNDEFINED)    )
    {
      decipher_oc = gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].ack.decipher_oc;

      /* Examine the frame's N(S) count to see if the peer has wrapped it's
         send state variable modulo with respect to the receive state
         variable V(R). If so just increment the LOCAL version of decipher_oc
         by 512 as the frame has not yet been FCS verified. */

      if ( gllc_disassy_info[gas_id].frame.i_s_frame.send_seq_num_N_S <
           gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].ack.recv_state_var_V_R )
      {
        decipher_oc = (uint32)(decipher_oc + 512);
      }

      /* Input = (IOV_UI + LFN + OC) Mod32 */

      gea_input_seed =
      (uint32)
      (
        gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].ack.input_offset_IOV_I
        +
        gllc_disassy_info[gas_id].frame.i_s_frame.send_seq_num_N_S       /* LFN */
        +
        decipher_oc                                              /* OC  */
      );

      if
      (
        !gllc_dis_frame_info_fcs_and_decipher
        (
          gas_id, ll_pdu_ptr, gllc_disassy_info[gas_id].fr_info_length,
          gllc_disassy_info[gas_id].fr_info_length, gea_input_seed
        )
      )
      {
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FCS_ERR;
      }
    }

    /* ................................................................... */

    /* else GMM has not specified an encryption algorithm for this TLLI,
       so just perform FCS calculation and strip, and test for FCS error. */

    else if
    (
      !gllc_dis_frame_info_fcs(
        gas_id, ll_pdu_ptr, gllc_disassy_info[gas_id].fr_info_length )
    )
    {
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FCS_ERR;
    }

    /* ................................................................... */

    /* Check information field length against permitted maximum for I
       frames. */

    if ( !gllc_disassy_info[gas_id].fr_dis_err_map )
    {
#ifdef DEBUG_GSM_GPRS_LLC_AGILENT8960_WORKAROUND
      #error code not present
#else
      uint16 max_info_octets_N201_I =
        gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].ack.max_info_octets_N201_I;
#endif

      /* If a negotiation for increased N201-I is in progress or pending,
         then permit the acceptance of frames consistent with that pending
         negotiation, until the negotiation explicitly concludes. */

      if ( gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unnum.xid_pending &
           GLLC_N201_I_XID                                       )
      {
        if
        ( 
           gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unnum.xid_req.n201_i_xid >
           max_info_octets_N201_I
        )
        {
          max_info_octets_N201_I =
            gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unnum.xid_req.n201_i_xid;
        }
      }
      
      if ( gllc_disassy_info[gas_id].fr_info_length > max_info_octets_N201_I )
      {
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FRMR_ERR;
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_FRMR_W2_I_FIELD_LENGTH_ERR;
      }
      else if ( gllc_disassy_info[gas_id].fr_info_length == 0 )
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_SHORT_ERR;
    }

    /* ................................................................... */

    /* De-link the remaining L3 PDU item chain and link it into the
       disassembly working structure. */

    if ( !gllc_disassy_info[gas_id].fr_dis_err_map )
    {
      if ( *ll_pdu_ptr != NULL )
      {
        gllc_disassy_info[gas_id].frame.i_s_frame.l3_pdu  = *ll_pdu_ptr;
        *ll_pdu_ptr                               = NULL;
      }
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_S_FORMAT:            /* Supervisory format.                    */

    /* Perform FCS calculation and strip, and test for FCS error. */

    if
    (
      !gllc_dis_frame_info_fcs(
        gas_id, ll_pdu_ptr, gllc_disassy_info[gas_id].fr_info_length )
    )
    {
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FCS_ERR;
    }

    /* ................................................................... */

    /* No information content should be attached to a supervisory frame. */

    if ( !gllc_disassy_info[gas_id].fr_dis_err_map )
    {
      if ( *ll_pdu_ptr != NULL )
        frmr_condition = TRUE;   /* Supervisory frame too long. */
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_UI_FORMAT:           /* Unconfirmed information format.        */

    /* If unprotected mode then FCS is calculated over the header and first
       N202 octets of the information field. */

    fcs_length = gllc_disassy_info[gas_id].fr_info_length;

    if ( (gllc_disassy_info[gas_id].frame.format == GLLC_UI_FORMAT  ) &&
        !(gllc_disassy_info[gas_id].frame.ui_frame.protected_mode_PM)    )
    {
      if ( fcs_length > gllc_n202_lookup[gllc_llme[gas_id].llc_version] )
        fcs_length = gllc_n202_lookup[gllc_llme[gas_id].llc_version];
    }

    /* ................................................................... */

    /* If GMM has specified an encryption algorithm for this TLLI and the
       frame is shown as enciphered, then calculate the cipher algorithm
       input seed in preparation for frame deciphering, FCS calculation and
       strip, and test for FCS error. */

    if ( (gllc_llme[gas_id].gea != GMMLLC_GEA_NONE            ) &&
         (gllc_llme[gas_id].gea != GMMLLC_GEA_UNDEFINED       ) &&
         (gllc_disassy_info[gas_id].frame.ui_frame.encrypted_E)    )
    {
      /* Examine the frames N(U) count to see if it is in sequence as
         expected, or if it has wrapped it's modulo normally with V(UR)
         at zero. */
      if 
      (
        ( gllc_disassy_info[gas_id].frame.ui_frame.send_seq_num_N_U !=
          gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unack.recv_state_var_V_UR )
        ||
        ( gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unack.recv_state_var_V_UR == 0 )
      )
      {
        /* Deciphering may require multiple attempts. As the deciphering
           process modifies the source PDU, a backup copy of the PDU must be
           retained for each deciphering pass. */

        ll_pdu_copy_ptr = gprs_pdu_copy( *ll_pdu_ptr );
        decipher_reattempt_countdown = GLLC_NUM_DECIPHER_REATTEMPTS;
      }

      decipher_oc =
        gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unack.decipher_oc;

      /* Commence the single/multipass deciphering process with combined
         FCS calculation. */
      do
      {
        /* Input = ((IOV_UI xor ((2^27 * SAPI) + 2^31)) + LFN + OC) Mod32 */

        gea_input_seed =
        (uint32)
        (
          (
            gllc_llme[gas_id].input_offset_IOV_UI
            ^
            (
              (
                gllc_sapi_addr_lookup[gllc_disassy_info[gas_id].lle] *    /* SAPI */
                0x08000000UL                                      /* 2^27 */
              )
              +
              0x80000000UL                                        /* 2^31 */
            )
          )
          +
          gllc_disassy_info[gas_id].frame.ui_frame.send_seq_num_N_U       /* LFN */
          +
          decipher_oc                                             /* OC  */
        );

        if
        (
          !gllc_dis_frame_info_fcs_and_decipher
          (
            gas_id, ll_pdu_ptr, gllc_disassy_info[gas_id].fr_info_length,
            fcs_length, gea_input_seed
          )
        )
        {
          /* Frame has not been successfully deciphered. */
          gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FCS_ERR;

          if ( decipher_reattempt_countdown != 0 )
          {
            if ( decipher_oc ==
                 gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unack.decipher_oc )
            {
              /* Deciphering using the current value of the decipher OC
                 counter has failed. */

              MSG_GERAN_HIGH_3_G
              (
                "GLC LL %d: Info UI U:%d FCS Err with Decipher OC:%d",
                gllc_sapi_addr_lookup[ gllc_disassy_info[gas_id].lle ],
                gllc_disassy_info[gas_id].frame.ui_frame.send_seq_num_N_U,
                decipher_oc
              );
            }

            /* Advance the value of decipher OC and re-attempt after
               reinstating the PDU to be deciphered. */

            decipher_oc = (uint32)(decipher_oc + 512);
            GPRS_PDU_FREE( ll_pdu_ptr );
            *ll_pdu_ptr = gprs_pdu_copy( ll_pdu_copy_ptr );

            MSG_GERAN_HIGH_3_G
            (
              "GLC LL %d: Info UI U:%d Re-attempt Decipher OC:%d",
              gllc_sapi_addr_lookup[ gllc_disassy_info[gas_id].lle ],
              gllc_disassy_info[gas_id].frame.ui_frame.send_seq_num_N_U,
              decipher_oc
            ); 
          }
        }
        else
        {
          /* Frame has been successfully deciphered. */
          gllc_disassy_info[gas_id].fr_dis_err_map &= ~GLLC_DIS_FCS_ERR;

          /* Force an update of decipher OC if decipher OC is found to be
             mis-synchonised through a decipher re-attempt. */

          if ( decipher_oc !=
               gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unack.decipher_oc )
          {
            MSG_GERAN_HIGH_3_G
            (
              "GLC LL %d: Info - Decipher OC Update %d to %d",
              gllc_sapi_addr_lookup[ gllc_disassy_info[gas_id].lle ],
              gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unack.decipher_oc,
              decipher_oc
            ); 

            gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unack.decipher_oc =
              decipher_oc;

            gllc_debug_print_lle_cipher_params( gas_id, gllc_disassy_info[gas_id].lle );

          }
          break;
        }
      }
      while ( decipher_reattempt_countdown-- != 0 );

      if ( ll_pdu_copy_ptr != NULL )
        GPRS_PDU_FREE( &ll_pdu_copy_ptr );
    }

    /* ................................................................... */

    /* else GMM has not specified an encryption algorithm for this TLLI,
       so just perform FCS calculation and strip, and test for FCS error. */

    else if ( !gllc_dis_frame_info_fcs( gas_id, ll_pdu_ptr, fcs_length ) )
    {
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FCS_ERR;
    }

    /* ................................................................... */

    /* Check information field length against permitted maximum for UI
       frames. */

    if ( !gllc_disassy_info[gas_id].fr_dis_err_map )
    {
#ifdef DEBUG_GSM_GPRS_LLC_AGILENT8960_WORKAROUND
      #error code not present
#else
      uint16 max_info_octets_N201_U =
        gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unack.max_info_octets_N201_U;
#endif

      /* If a negotiation for increased N201-U is in progress or pending,
         then permit the acceptance of frames consistent with that pending
         negotiation, until the negotiation explicitly concludes. */

      if ( gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unnum.xid_pending &
           GLLC_N201_U_XID                                       )
      {
        if
        ( 
           gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unnum.xid_req.n201_u_xid >
           max_info_octets_N201_U
        )
        {
          max_info_octets_N201_U =
            gllc_lle[gas_id][ gllc_disassy_info[gas_id].lle ].unnum.xid_req.n201_u_xid;
        }
      }
      
      if ( gllc_disassy_info[gas_id].fr_info_length > max_info_octets_N201_U )
      {
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FRMR_ERR;
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_FRMR_W2_I_FIELD_LENGTH_ERR;
      }
      else if ( gllc_disassy_info[gas_id].fr_info_length == 0 )
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_SHORT_ERR;
    }

    /* ................................................................... */

    /* De-link the remaining L3 PDU item chain and link it into the
       disassembly working structure. */

    if ( !gllc_disassy_info[gas_id].fr_dis_err_map )
    {
      if ( *ll_pdu_ptr != NULL )
      {
        gllc_disassy_info[gas_id].frame.ui_frame.l3_pdu = *ll_pdu_ptr;
        *ll_pdu_ptr                             = NULL;
      }
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_U_FORMAT:            /* Unnumbered format.                     */

    /* Initialise relevent fields of the disassembly working structure. */

    xid_info_p = &gllc_disassy_info[gas_id].frame.u_frame.info_field.xid_info;

    gllc_disassy_info[gas_id].frame.u_frame.xid = GLLC_XID_NONE;
    xid_info_p -> contents              = 0;
    xid_info_p -> errors                = 0;

    /* Perform FCS calculation and strip, and test for FCS error. */

    if
    (
      !gllc_dis_frame_info_fcs(
        gas_id, ll_pdu_ptr, gllc_disassy_info[gas_id].fr_info_length )
    )
    {
      gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FCS_ERR;
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    else if
    (
      (gllc_disassy_info[gas_id].frame.u_frame.cmd_res_M4_M1 == GLLC_SABM) ||
      (gllc_disassy_info[gas_id].frame.u_frame.cmd_res_M4_M1 == GLLC_UA  ) ||
      (gllc_disassy_info[gas_id].frame.u_frame.cmd_res_M4_M1 == GLLC_XID )
    )
    {
      /* These commands/responses may carry an XID parameter negotiation
         information field. */

      if ( gllc_disassy_info[gas_id].frame.u_frame.cmd_or_res_CR == GLLC_COMMAND )
        gllc_disassy_info[gas_id].frame.u_frame.xid = GLLC_XID_REQ;
      else
        gllc_disassy_info[gas_id].frame.u_frame.xid = GLLC_XID_NEG;

      if ( *ll_pdu_ptr != NULL )
      {
        /* An XID information field is present. Extract each XID param block
           in turn from the XID info field. */

        xid_ctr = 0;

        while ( (*ll_pdu_ptr != NULL) && !frmr_condition )
        {
          if ( !gprs_pdu_strip_head( ll_pdu_ptr, buf, 1 ) )
            frmr_condition = TRUE;

          /* Establish length of XID parameter field. */

          xid_xl   = LLC_U_FRAME_GET_XID_XL( buf );
          xid_type = LLC_U_FRAME_GET_XID_TYPE( buf );

          if ( xid_xl )
          {
            if ( !gprs_pdu_strip_head( ll_pdu_ptr, &buf[1], 1 ) )
              frmr_condition = TRUE;
            xid_length = LLC_U_FRAME_GET_XID_LEN_8BIT( buf );
          }
          else
            xid_length = LLC_U_FRAME_GET_XID_LEN_2BIT( buf );

          /* A layer 3 XID parameter cannot be stripped to the local buffer
             due to its potential length, and must be handled seperately. */

          if ( xid_type != GLLC_LAY3_XID_TYPE )
          {
            if ( !gprs_pdu_strip_head( ll_pdu_ptr, buf, xid_length ) )
              frmr_condition = TRUE;

            else switch ( xid_type )
            {
            /* ............................................................. */

            case GLLC_VERSION_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_VERSION_XID) )
              {
                if ( xid_length == GLLC_VERSION_XID_SIZE )
                {
                  xid_info_p -> contents    |= GLLC_VERSION_XID;
                  xid_info_p -> version_xid  = buf[xid_length-1];
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_IOV_UI_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_IOV_UI_XID) )
              {
                if ( xid_length == GLLC_IOV_UI_XID_SIZE )
                {
                  xid_info_p -> contents   |= GLLC_IOV_UI_XID;
                  xid_info_p -> iov_ui_xid  = 0;
                  xid_info_p -> iov_ui_xid |= (uint32)(buf[xid_length-4] << 24);
                  xid_info_p -> iov_ui_xid |= (uint32)(buf[xid_length-3] << 16);
                  xid_info_p -> iov_ui_xid |= (uint32)(buf[xid_length-2] <<  8);
                  xid_info_p -> iov_ui_xid |= (uint32)(buf[xid_length-1] <<  0);
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_IOV_I_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_IOV_I_XID) )
              {
                if ( xid_length == GLLC_IOV_I_XID_SIZE )
                {
                  xid_info_p -> contents  |= GLLC_IOV_I_XID;
                  xid_info_p -> iov_i_xid  = 0;
                  xid_info_p -> iov_i_xid |= (uint32)(buf[xid_length-4] << 24);
                  xid_info_p -> iov_i_xid |= (uint32)(buf[xid_length-3] << 16);
                  xid_info_p -> iov_i_xid |= (uint32)(buf[xid_length-2] <<  8);
                  xid_info_p -> iov_i_xid |= (uint32)(buf[xid_length-1] <<  0);
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_T200_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_T200_XID) )
              {
                if ( xid_length == GLLC_T200_XID_SIZE )
                {
                  xid_info_p -> contents |= GLLC_T200_XID;
                  xid_info_p -> t200_xid  = 0;
                  xid_info_p -> t200_xid |= (uint16)(buf[xid_length-2] <<  8);
                  xid_info_p -> t200_xid |= (uint16)(buf[xid_length-1] <<  0);
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_N200_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_N200_XID) )
              {
                if ( xid_length == GLLC_N200_XID_SIZE )
                {
                  xid_info_p -> contents |= GLLC_N200_XID;
                  xid_info_p -> n200_xid = buf[xid_length-1];
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_N201_U_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_N201_U_XID) )
              {
                if ( xid_length == GLLC_N201_U_XID_SIZE )
                {
                  xid_info_p -> contents   |= GLLC_N201_U_XID;
                  xid_info_p -> n201_u_xid  = 0;
                  xid_info_p -> n201_u_xid |= (uint16)(buf[xid_length-2] <<  8);
                  xid_info_p -> n201_u_xid |= (uint16)(buf[xid_length-1] <<  0);
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_N201_I_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_N201_I_XID) )
              {
                if ( xid_length == GLLC_N201_I_XID_SIZE )
                {
                  xid_info_p -> contents   |= GLLC_N201_I_XID;
                  xid_info_p -> n201_i_xid  = 0;
                  xid_info_p -> n201_i_xid |= (uint16)(buf[xid_length-2] <<  8);
                  xid_info_p -> n201_i_xid |= (uint16)(buf[xid_length-1] <<  0);
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_mD_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_mD_XID) )
              {
                if ( xid_length == GLLC_mD_XID_SIZE )
                {
                  xid_info_p -> contents |= GLLC_mD_XID;
                  xid_info_p -> md_xid    = 0;
                  xid_info_p -> md_xid   |= (uint16)(buf[xid_length-2] <<  8);
                  xid_info_p -> md_xid   |= (uint16)(buf[xid_length-1] <<  0);
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_mU_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_mU_XID) )
              {
                if ( xid_length == GLLC_mU_XID_SIZE )
                {
                  xid_info_p -> contents |= GLLC_mU_XID;
                  xid_info_p -> mu_xid    = 0;
                  xid_info_p -> mu_xid   |= (uint16)(buf[xid_length-2] <<  8);
                  xid_info_p -> mu_xid   |= (uint16)(buf[xid_length-1] <<  0);
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_kD_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_kD_XID) )
              {
                if ( xid_length == GLLC_kD_XID_SIZE )
                {
                  xid_info_p -> contents |= GLLC_kD_XID;
                  xid_info_p -> kd_xid    = buf[xid_length-1];
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_kU_XID_TYPE:

              if ( !(xid_info_p -> contents & GLLC_kU_XID) )
              {
                if ( xid_length == GLLC_kU_XID_SIZE )
                {
                  xid_info_p -> contents |= GLLC_kU_XID;
                  xid_info_p -> ku_xid    = buf[xid_length-1];
                }
                else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                  xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
              }
              else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
              break;

            /* ............................................................. */

            case GLLC_RESET_XID_TYPE:

              /* Reset must be the first XID parameter. */

              if ( (xid_length == GLLC_RESET_XID_SIZE) && (xid_ctr == 0) )
                xid_info_p -> contents |= GLLC_RESET_XID;
              else
                xid_info_p -> errors |= GLLC_XID_RESET_NOT_FIRST;
              break;

            /* ............................................................. */

            default:

              if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_TYPE;
              break;
            }
          }

          else /* xid_type == GLLC_LAY3_XID_TYPE */
          {
            if ( !(xid_info_p -> contents & GLLC_LAY3_XID) )
            {
              if ( xid_length > 0 )
              {
                if ( GPRS_PDU_LENGTH( *ll_pdu_ptr ) < xid_length )
                {
                  if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
                    xid_info_p -> errors |= GLLC_XID_UNSUPPORTED_LEN;
                }
                else
                {
                  xid_info_p -> contents      |= GLLC_LAY3_XID;
                  xid_info_p -> l3_xid.length  = xid_length;
                  xid_info_p -> l3_xid.params  =
                    gprs_pdu_segment_head( ll_pdu_ptr, xid_length );
                }
              }
              else
              {
                xid_info_p -> contents      |= GLLC_LAY3_XID;
                xid_info_p -> l3_xid.length  = 0;
                xid_info_p -> l3_xid.params  = NULL;
              }
            }
            else if ( gllc_disassy_info[gas_id].frame.u_frame.xid == GLLC_XID_NEG )
              xid_info_p -> errors |= GLLC_XID_MULTI_INSTANCE;
          }

          xid_ctr++;

        } /* while ( (*ll_pdu_ptr != NULL) && !frmr_condition ) */
      }
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    else if ( gllc_disassy_info[gas_id].frame.u_frame.cmd_res_M4_M1 == GLLC_FRMR )
    {
      /* This response carries a frame reject status 10 octet information
         field. */

      if ( !gprs_pdu_strip_head( ll_pdu_ptr, buf, 10 ) )
        frmr_condition = TRUE; /* The U frame FRMR res. is itself reject. */
      else
      {
        /* Extract the rejected frame control field as a 6 octet block. */

        (void)memscpy
        (
          gllc_disassy_info[gas_id].frame.u_frame.
            info_field.frmr_info.rej_fr_ctrl_field,
          sizeof(gllc_disassy_info[gas_id].frame.u_frame.
            info_field.frmr_info.rej_fr_ctrl_field),
          buf,
          6
        );

        /* Extract the remaining frame reject parameters. */

        gllc_disassy_info[gas_id].frame.u_frame.info_field.frmr_info.send_state_var_V_S =
          LLC_U_FRAME_GET_FRMR_V_S( buf );
        gllc_disassy_info[gas_id].frame.u_frame.info_field.frmr_info.recv_state_var_V_R =
          LLC_U_FRAME_GET_FRMR_V_R( buf );
        gllc_disassy_info[gas_id].frame.u_frame.info_field.frmr_info.rejected_frame_CR =
          LLC_U_FRAME_GET_FRMR_CR( buf );
        gllc_disassy_info[gas_id].frame.u_frame.info_field.frmr_info.reject_status_W4_W1 =
          LLC_U_FRAME_GET_FRMR_W4_W1( buf );
      }
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_NO_FORMAT:
  default:

    /* This error case should have been handled within
       gllc_disassemble_hdr(). */
    break;
  }

  /* --------------------------------------------------------------------- */

  /* If a frame reject condition has been detected then flag it up. */

  if ( frmr_condition )
  {
    gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_FRMR_ERR;
    gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_FRMR_W1_INVALID_I_FIELD;
    gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_FRMR_W3_INVALID_CTRL_FIELD;
  }

  /* ===================================================================== */
} /* end of gllc_disassemble_frame_info_and_fcs() */



/*** EOF: don't remove! ***/

