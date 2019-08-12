/*****************************************************************************
***
*** TITLE
***
***  EGPRS RLC T.4 Decoder
***
*** DESCRIPTION
***
***  This module also contains a set of utilities which:
***  - Generates T4 binary trees
***  - Verifies that the generated binary trees are correct by comparing the 
***    decoded output with the run length stored in the raw tables
***  - Tests the T4 decoder using real compressed bitmap ..
*** 
*** EXTERNALIZED FUNCTIONS
*** 
***  void grlc_t4_decode ()
***
*** UTILITY FUNCTIONS (host build only)
***
***  void T4CreateTables (void)
***  The generated binary trees are shown on screen after T4CreateTables(). The text
***  must be pasted back to produce the tables for use with the target code.
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  void grlc_t4_decode_init ()
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlct4decode.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 23/04/04    hv     Created
*** 24/09/04    hv     Binary Trees produced and verified
*** 09/19/12    hv     Added dual data space functionality
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */


#include  "comdef.h"

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #error code not present
#endif

#include "grlct4decode.h"
#include "grlcdli.h"                  /* ? */

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/* Only define switch below to enable code which creates ROM tables and to test
** the tables
*/
#undef  GENERATE_T4_ROM_TABLES

/*-------------------------------------------------------------------------------------
** Basic element of the T4 One/Zero Run Length Binary Tree.
**
** The left index and righ index point to the next index in the table (array) when
** a '0' or '1' is matched. The run length (key), if valid, contains the run length of
** a matched sequence.
**-------------------------------------------------------------------------------------
*/
typedef struct
{
  uint16  left_index;
  uint16  right_index;
  uint16  run_len;              /* 0..63, 64,..,960 (65535 if not valid) */
} t4_binary_tree_node_t;

/*----------------------------------------------------------------------------------
** When a node is NOT a leaf there is no run length (key) associated with that node
**----------------------------------------------------------------------------------
*/
#define INVALID_RUN_LEN        (65535)

/*-----------------------------------------------------------------------------------
** The raw table is expressed as an array of nodes. When a branch of a node does not
** exist the branch (left or right index) takes on this invalid index value
**-----------------------------------------------------------------------------------
*/
#define INVALID_INDEX          (255)

/*------------------------------------------------------------------------------
** Maximum codeword length for one run length is 12 and 13 for zero run length.
** If a codeword is not found after T4_MAX_XRL_CODEWORD_LEN then the CRBB is 
** assumed to contain errors
**------------------------------------------------------------------------------
*/
#define T4_MAX_ORL_CODEWORD_LEN   12
#define T4_MAX_ZRL_CODEWORD_LEN   13

/*------------------------------------------------------------------------
** T4 maximum number of terminating codewords for both one run length and
** zero run length
**------------------------------------------------------------------------
*/

#define T4_MAX_TERMINATING_CODEWORDS  64


/*=============== BEGIN T4 TABLES GENERATION & VERIFICATION UTILITY =================*/

#ifdef GENERATE_T4_ROM_TABLES

/*----------------------------------------------------------------------
** Number of nodes (items) generated for the one run length binary tree
**----------------------------------------------------------------------
*/
#define T4_ORL_BIN_TABLE_LEN   (163)

/*-----------------------------------------------------------------------
** Number of nodes (items) generated for the zero run length binary tree
**-----------------------------------------------------------------------
*/
#define T4_ZRL_BIN_TABLE_LEN   (163)

/*---------------------------------------------------------------------------------
** Abbreviations
**
** ORL: one run length
** ZRL: zero run length
** Binary Table: binary tree where each node is represented by an element of an 
**    array (table). Left index and right index are used to point to the next node 
**    (in the table) instead of pointers
**---------------------------------------------------------------------------------
*/

/*--------------------------------------------------------------------------------
** Basic element of a raw T4 terminating code or make-up code table as 
** per TS 04.60 Table 9.1.10.1/2. Note the codewords are kept as strings to avoid
** having to represent their lengths.
**--------------------------------------------------------------------------------
*/
typedef struct
{
  uint16  uRunLen;
  char    *cpCw;        /* ptr to codeword eg. "00110101" corresponds to 0 one run 
                        ** length 
                        */
} stT4RawTableElement;

/*----------------------------------------------------------------------------
** Raw T4 One Run Length table as per TS 04.60 Table 9.1.10.1/2
** This table is only used to generate the One Run Length Binary Tree for the 
** target code
**----------------------------------------------------------------------------
*/
stT4RawTableElement  T4RawORLTable[] =
{
/* Run    Terminating
** Length Codeword 
*/
  {    0, "00110101", },
  {    1, "000111",   },
  {    2, "0111",     },
  {    3, "1000",     },
  {    4, "1011",     },
  {    5, "1100",     },
  {    6, "1110",     },
  {    7, "1111",     },
  {    8, "10011",    },
  {    9, "10100",    },
  {   10, "00111",    },
  {   11, "01000",    },
  {   12, "001000",   },
  {   13, "000011",   },
  {   14, "110100",   },
  {   15, "110101",   },
  {   16, "101010",   },
  {   17, "101011",   },
  {   18, "0100111",  },
  {   19, "0001100",  },
  {   20, "0001000",  },
  {   21, "0010111",  },
  {   22, "0000011",  },
  {   23, "0000100",  },
  {   24, "0101000",  },
  {   25, "0101011",  },
  {   26, "0010011",  },
  {   27, "0100100",  },
  {   28, "0011000",  },
  {   29, "00000010", },
  {   30, "00000011", },
  {   31, "00011010", },
  {   32, "00011011", },
  {   33, "00010010", },
  {   34, "00010011", },
  {   35, "00010100", },
  {   36, "00010101", },
  {   37, "00010110", },
  {   38, "00010111", },
  {   39, "00101000", },
  {   40, "00101001", },
  {   41, "00101010", },
  {   42, "00101011", },
  {   43, "00101100", },
  {   44, "00101101", },
  {   45, "00000100", },
  {   46, "00000101", },
  {   47, "00001010", },
  {   48, "00001011", },
  {   49, "01010010", },
  {   50, "01010011", },
  {   51, "01010100", },
  {   52, "01010101", },
  {   53, "00100100", },
  {   54, "00100101", },
  {   55, "01011000", },
  {   56, "01011001", },
  {   57, "01011010", },
  {   58, "01011011", },
  {   59, "01001010", },
  {   60, "01001011", },
  {   61, "00110010", },
  {   62, "00110011", },
  {   63, "00110100", },
/* Run    Make-up
** Length Codeword 
*/
  {   64, "11011",    },
  {  128, "10010",    },
  {  192, "010111",   },
  {  256, "0110111",  },
  {  320, "00110110", },
  {  384, "00110111", },
  {  448, "01100100", },
  {  512, "01100101", },
  {  576, "01101000", },
  {  640, "01100111", },
  {  704, "011001100",},
  {  768, "011001101",},
  {  832, "011010010",},
  {  896, "011010011",},
  {  960, "011010100",},
};  /* T4RawORLTable */

/*------------------------------------------------------------------------------------
** Number of elements in the raw one run length table. 64 for the terminating code
** table and 15 for the make-up code table. The two make up the ORL Table. This macro
** allows this table to be expanded if required
**------------------------------------------------------------------------------------
*/
#define RAW_TABLE_LEN() (sizeof(T4RawORLTable)/sizeof(stT4RawTableElement))

/*-----------------------------------------------------------------------------
** Raw T4 Zero Run Length table as per TS 04.60 Table 9.1.10.1/2
** This table is only used to generate the Zero Run Length Binary Tree for the 
** target code
**-----------------------------------------------------------------------------
*/
stT4RawTableElement  T4RawZRLTable[] =
{
/* Run    Terminating
** Length Codeword 
*/
  {    0, "0000110111",     },
  {    1, "10",             },
  {    2, "11",             },
  {    3, "010",            },
  {    4, "011",            },
  {    5, "0011",           },
  {    6, "0010",           },
  {    7, "00011",          },
  {    8, "000101",         },
  {    9, "000100",         },
  {   10, "0000100",        },
  {   11, "0000101",        },
  {   12, "0000111",        },
  {   13, "00000100",       },
  {   14, "00000111",       },
  {   15, "000011000",      },
  {   16, "0000010111",     },
  {   17, "0000011000",     },
  {   18, "0000001000",     },
  {   19, "00001100111",    },
  {   20, "00001101000",    },
  {   21, "00001101100",    },
  {   22, "00000110111",    },
  {   23, "00000101000",    },
  {   24, "00000010111",    },
  {   25, "00000011000",    },
  {   26, "000011001010",   },
  {   27, "000011001011",   },
  {   28, "000011001100",   },
  {   29, "000011001101",   },
  {   30, "000001101000",   },
  {   31, "000001101001",   },
  {   32, "000001101010",   },
  {   33, "000001101011",   },
  {   34, "000011010010",   },
  {   35, "000011010011",   },
  {   36, "000011010100",   },
  {   37, "000011010101",   },
  {   38, "000011010110",   },
  {   39, "000011010111",   },
  {   40, "000001101100",   },
  {   41, "000001101101",   },
  {   42, "000011011010",   },
  {   43, "000011011011",   },
  {   44, "000001010100",   },
  {   45, "000001010101",   },
  {   46, "000001010110",   },
  {   47, "000001010111",   },
  {   48, "000001100100",   },
  {   49, "000001100101",   },
  {   50, "000001010010",   },
  {   51, "000001010011",   },
  {   52, "000000100100",   },
  {   53, "000000110111",   },
  {   54, "000000111000",   },
  {   55, "000000100111",   },
  {   56, "000000101000",   },
  {   57, "000001011000",   },
  {   58, "000001011001",   },
  {   59, "000000101011",   },
  {   60, "000000101100",   },
  {   61, "000001011010",   },
  {   62, "000001100110",   },
  {   63, "000001100111",   },
/* Run    Make-up
** Length Codeword 
*/
  {   64, "0000001111",     },
  {  128, "000011001000",   },
  {  192, "000011001001",   },
  {  256, "000001011011",   },
  {  320, "000000110011",   },
  {  384, "000000110100",   },
  {  448, "000000110101",   },
  {  512, "0000001101100",  },
  {  576, "0000001101101",  },
  {  640, "0000001001010",  },
  {  704, "0000001001011",  },
  {  768, "0000001001100",  },
  {  832, "0000001001101",  },
  {  896, "0000001110010",  },
  {  960, "0000001110011",  },
};  /* T4RawZRLTable */

/*------------------------------------------------------------------------------------
** T4GeneratedOneRunLengthBinaryTree[] is generated by the utility function 
** T4CreateORLBinaryTree(). The values are shown by T4ShowORLBinaryTree() on screen. 
** The text is then copied and reformatted to produce the one run length binary tree 
** t4_orl_binary_tree[] below as a ROM-able table for the target code. The length of 
** this table is defined as T4_ORL_BIN_TABLE_LEN
**------------------------------------------------------------------------------------
*/
t4_binary_tree_node_t   T4GeneratedOneRunLengthBinaryTree[T4_ORL_BIN_TABLE_LEN];

/*------------------------------------------------------------------------------------
** T4GeneratedZeroRunLengthBinaryTree[] is generated by the utility function 
** T4CreateZRLBinaryTrees(). The values are shown by T4ShowZRLBinaryTree() on screen.
** The text is then copied and reformatted to produce the zero run length binary tree 
** t4_zrl_binary_tree[] below as a ROM-able table for the target code. The length of 
** this table is defined as T4_ZRL_BIN_TABLE_LEN
**------------------------------------------------------------------------------------
*/
t4_binary_tree_node_t   T4GeneratedZeroRunLengthBinaryTree[T4_ZRL_BIN_TABLE_LEN];

/*================ END T4 TABLES GENERATION & VERIFICATION UTILITY =================*/

#endif /* GENERATE_T4_ROM_TABLES */

static const t4_binary_tree_node_t   t4_orl_binary_tree[] =
{
    /* Index   Left  Right  Runlength */
  { /*   0 */    1,    16, 65535, },
  { /*   1 */    2,    13, 65535, },
  { /*   2 */    9,     3, 65535, },
  { /*   3 */   36,     4, 65535, },
  { /*   4 */    5,    32, 65535, },
  { /*   5 */   75,     6, 65535, },
  { /*   6 */    7,   141, 65535, },
  { /*   7 */  133,     8, 65535, },
  { /*   8 */  255,   255,     0, },
  { /*   9 */   39,    10, 65535, },
  { /*  10 */   54,    11, 65535, },
  { /*  11 */   52,    12, 65535, },
  { /*  12 */  255,   255,     1, },
  { /*  13 */   33,    14, 65535, },
  { /*  14 */  137,    15, 65535, },
  { /*  15 */  255,   255,     2, },
  { /*  16 */   17,    22, 65535, },
  { /*  17 */   18,    20, 65535, },
  { /*  18 */   19,    28, 65535, },
  { /*  19 */  255,   255,     3, },
  { /*  20 */   30,    21, 65535, },
  { /*  21 */  255,   255,     4, },
  { /*  22 */   23,    25, 65535, },
  { /*  23 */   24,    42, 65535, },
  { /*  24 */  255,   255,     5, },
  { /*  25 */   26,    27, 65535, },
  { /*  26 */  255,   255,     6, },
  { /*  27 */  255,   255,     7, },
  { /*  28 */  135,    29, 65535, },
  { /*  29 */  255,   255,     8, },
  { /*  30 */   31,    46, 65535, },
  { /*  31 */  255,   255,     9, },
  { /*  32 */  255,   255,    10, },
  { /*  33 */   34,    65, 65535, },
  { /*  34 */   35,    49, 65535, },
  { /*  35 */  255,   255,    11, },
  { /*  36 */   37,    57, 65535, },
  { /*  37 */   38,    71, 65535, },
  { /*  38 */  255,   255,    12, },
  { /*  39 */   60,    40, 65535, },
  { /*  40 */   63,    41, 65535, },
  { /*  41 */  255,   255,    13, },
  { /*  42 */   43,   134, 65535, },
  { /*  43 */   44,    45, 65535, },
  { /*  44 */  255,   255,    14, },
  { /*  45 */  255,   255,    15, },
  { /*  46 */   47,    48, 65535, },
  { /*  47 */  255,   255,    16, },
  { /*  48 */  255,   255,    17, },
  { /*  49 */   73,    50, 65535, },
  { /*  50 */  255,    51, 65535, },
  { /*  51 */  255,   255,    18, },
  { /*  52 */   53,    81, 65535, },
  { /*  53 */  255,   255,    19, },
  { /*  54 */   55,    87, 65535, },
  { /*  55 */   56,    84, 65535, },
  { /*  56 */  255,   255,    20, },
  { /*  57 */   94,    58, 65535, },
  { /*  58 */  101,    59, 65535, },
  { /*  59 */  255,   255,    21, },
  { /*  60 */   77,    61, 65535, },
  { /*  61 */  104,    62, 65535, },
  { /*  62 */  255,   255,    22, },
  { /*  63 */   64,   107, 65535, },
  { /*  64 */  255,   255,    23, },
  { /*  65 */   66,   119, 65535, },
  { /*  66 */   67,    69, 65535, },
  { /*  67 */   68,   110, 65535, },
  { /*  68 */  255,   255,    24, },
  { /*  69 */  113,    70, 65535, },
  { /*  70 */  255,   255,    25, },
  { /*  71 */  116,    72, 65535, },
  { /*  72 */  255,   255,    26, },
  { /*  73 */   74,   127, 65535, },
  { /*  74 */  255,   255,    27, },
  { /*  75 */   76,   130, 65535, },
  { /*  76 */  255,   255,    28, },
  { /*  77 */  255,    78, 65535, },
  { /*  78 */   79,    80, 65535, },
  { /*  79 */  255,   255,    29, },
  { /*  80 */  255,   255,    30, },
  { /*  81 */   82,    83, 65535, },
  { /*  82 */  255,   255,    31, },
  { /*  83 */  255,   255,    32, },
  { /*  84 */   85,    86, 65535, },
  { /*  85 */  255,   255,    33, },
  { /*  86 */  255,   255,    34, },
  { /*  87 */   88,    91, 65535, },
  { /*  88 */   89,    90, 65535, },
  { /*  89 */  255,   255,    35, },
  { /*  90 */  255,   255,    36, },
  { /*  91 */   92,    93, 65535, },
  { /*  92 */  255,   255,    37, },
  { /*  93 */  255,   255,    38, },
  { /*  94 */   95,    98, 65535, },
  { /*  95 */   96,    97, 65535, },
  { /*  96 */  255,   255,    39, },
  { /*  97 */  255,   255,    40, },
  { /*  98 */   99,   100, 65535, },
  { /*  99 */  255,   255,    41, },
  { /* 100 */  255,   255,    42, },
  { /* 101 */  102,   103, 65535, },
  { /* 102 */  255,   255,    43, },
  { /* 103 */  255,   255,    44, },
  { /* 104 */  105,   106, 65535, },
  { /* 105 */  255,   255,    45, },
  { /* 106 */  255,   255,    46, },
  { /* 107 */  108,   109, 65535, },
  { /* 108 */  255,   255,    47, },
  { /* 109 */  255,   255,    48, },
  { /* 110 */  111,   112, 65535, },
  { /* 111 */  255,   255,    49, },
  { /* 112 */  255,   255,    50, },
  { /* 113 */  114,   115, 65535, },
  { /* 114 */  255,   255,    51, },
  { /* 115 */  255,   255,    52, },
  { /* 116 */  117,   118, 65535, },
  { /* 117 */  255,   255,    53, },
  { /* 118 */  255,   255,    54, },
  { /* 119 */  120,   136, 65535, },
  { /* 120 */  121,   124, 65535, },
  { /* 121 */  122,   123, 65535, },
  { /* 122 */  255,   255,    55, },
  { /* 123 */  255,   255,    56, },
  { /* 124 */  125,   126, 65535, },
  { /* 125 */  255,   255,    57, },
  { /* 126 */  255,   255,    58, },
  { /* 127 */  128,   129, 65535, },
  { /* 128 */  255,   255,    59, },
  { /* 129 */  255,   255,    60, },
  { /* 130 */  131,   132, 65535, },
  { /* 131 */  255,   255,    61, },
  { /* 132 */  255,   255,    62, },
  { /* 133 */  255,   255,    63, },
  { /* 134 */  255,   255,    64, },
  { /* 135 */  255,   255,   128, },
  { /* 136 */  255,   255,   192, },
  { /* 137 */  144,   138, 65535, },
  { /* 138 */  149,   139, 65535, },
  { /* 139 */  255,   140, 65535, },
  { /* 140 */  255,   255,   256, },
  { /* 141 */  142,   143, 65535, },
  { /* 142 */  255,   255,   320, },
  { /* 143 */  255,   255,   384, },
  { /* 144 */  255,   145, 65535, },
  { /* 145 */  146,   152, 65535, },
  { /* 146 */  147,   148, 65535, },
  { /* 147 */  255,   255,   448, },
  { /* 148 */  255,   255,   512, },
  { /* 149 */  150,   160, 65535, },
  { /* 150 */  151,   157, 65535, },
  { /* 151 */  255,   255,   576, },
  { /* 152 */  154,   153, 65535, },
  { /* 153 */  255,   255,   640, },
  { /* 154 */  155,   156, 65535, },
  { /* 155 */  255,   255,   704, },
  { /* 156 */  255,   255,   768, },
  { /* 157 */  158,   159, 65535, },
  { /* 158 */  255,   255,   832, },
  { /* 159 */  255,   255,   896, },
  { /* 160 */  161,   255, 65535, },
  { /* 161 */  162,   255, 65535, },
  { /* 162 */  255,   255,   960, },
}; /* t4_orl_binary_tree */

static const t4_binary_tree_node_t   t4_zrl_binary_tree[] =
{
    /* Index   Left  Right  Runlength */
  { /*   0 */    1,    11,  65535, },
  { /*   1 */    2,    14,  65535, },
  { /*   2 */    3,    17,  65535, },
  { /*   3 */    4,    20,  65535, },
  { /*   4 */   29,     5,  65535, },
  { /*   5 */   25,     6,  65535, },
  { /*   6 */    7,    28,  65535, },
  { /*   7 */   35,     8,  65535, },
  { /*   8 */   51,     9,  65535, },
  { /*   9 */   54,    10,  65535, },
  { /*  10 */  255,   255,      0, },
  { /*  11 */   12,    13,  65535, },
  { /*  12 */  255,   255,      1, },
  { /*  13 */  255,   255,      2, },
  { /*  14 */   15,    16,  65535, },
  { /*  15 */  255,   255,      3, },
  { /*  16 */  255,   255,      4, },
  { /*  17 */   19,    18,  65535, },
  { /*  18 */  255,   255,      5, },
  { /*  19 */  255,   255,      6, },
  { /*  20 */   22,    21,  65535, },
  { /*  21 */  255,   255,      7, },
  { /*  22 */   24,    23,  65535, },
  { /*  23 */  255,   255,      8, },
  { /*  24 */  255,   255,      9, },
  { /*  25 */   26,    27,  65535, },
  { /*  26 */  255,   255,     10, },
  { /*  27 */  255,   255,     11, },
  { /*  28 */  255,   255,     12, },
  { /*  29 */   43,    30,  65535, },
  { /*  30 */   31,    33,  65535, },
  { /*  31 */   32,    37,  65535, },
  { /*  32 */  255,   255,     13, },
  { /*  33 */   40,    34,  65535, },
  { /*  34 */  255,   255,     14, },
  { /*  35 */   36,    48,  65535, },
  { /*  36 */  255,   255,     15, },
  { /*  37 */   59,    38,  65535, },
  { /*  38 */  128,    39,  65535, },
  { /*  39 */  255,   255,     16, },
  { /*  40 */   41,    56,  65535, },
  { /*  41 */   42,   106,  65535, },
  { /*  42 */  255,   255,     17, },
  { /*  43 */  255,    44,  65535, },
  { /*  44 */   45,    65,  65535, },
  { /*  45 */   46,    62,  65535, },
  { /*  46 */   47,   113,  65535, },
  { /*  47 */  255,   255,     18, },
  { /*  48 */   69,    49,  65535, },
  { /*  49 */   73,    50,  65535, },
  { /*  50 */  255,   255,     19, },
  { /*  51 */   52,    86,  65535, },
  { /*  52 */   53,    83,  65535, },
  { /*  53 */  255,   255,     20, },
  { /*  54 */   55,    96,  65535, },
  { /*  55 */  255,   255,     21, },
  { /*  56 */   76,    57,  65535, },
  { /*  57 */   93,    58,  65535, },
  { /*  58 */  255,   255,     22, },
  { /*  59 */   60,    99,  65535, },
  { /*  60 */   61,   110,  65535, },
  { /*  61 */  255,   255,     23, },
  { /*  62 */  125,    63,  65535, },
  { /*  63 */  134,    64,  65535, },
  { /*  64 */  255,   255,     24, },
  { /*  65 */   66,   119,  65535, },
  { /*  66 */   67,   116,  65535, },
  { /*  67 */   68,   146,  65535, },
  { /*  68 */  255,   255,     25, },
  { /*  69 */  142,    70,  65535, },
  { /*  70 */   71,    72,  65535, },
  { /*  71 */  255,   255,     26, },
  { /*  72 */  255,   255,     27, },
  { /*  73 */   74,    75,  65535, },
  { /*  74 */  255,   255,     28, },
  { /*  75 */  255,   255,     29, },
  { /*  76 */   77,    80,  65535, },
  { /*  77 */   78,    79,  65535, },
  { /*  78 */  255,   255,     30, },
  { /*  79 */  255,   255,     31, },
  { /*  80 */   81,    82,  65535, },
  { /*  81 */  255,   255,     32, },
  { /*  82 */  255,   255,     33, },
  { /*  83 */   84,    85,  65535, },
  { /*  84 */  255,   255,     34, },
  { /*  85 */  255,   255,     35, },
  { /*  86 */   87,    90,  65535, },
  { /*  87 */   88,    89,  65535, },
  { /*  88 */  255,   255,     36, },
  { /*  89 */  255,   255,     37, },
  { /*  90 */   91,    92,  65535, },
  { /*  91 */  255,   255,     38, },
  { /*  92 */  255,   255,     39, },
  { /*  93 */   94,    95,  65535, },
  { /*  94 */  255,   255,     40, },
  { /*  95 */  255,   255,     41, },
  { /*  96 */   97,    98,  65535, },
  { /*  97 */  255,   255,     42, },
  { /*  98 */  255,   255,     43, },
  { /*  99 */  100,   103,  65535, },
  { /* 100 */  101,   102,  65535, },
  { /* 101 */  255,   255,     44, },
  { /* 102 */  255,   255,     45, },
  { /* 103 */  104,   105,  65535, },
  { /* 104 */  255,   255,     46, },
  { /* 105 */  255,   255,     47, },
  { /* 106 */  107,   138,  65535, },
  { /* 107 */  108,   109,  65535, },
  { /* 108 */  255,   255,     48, },
  { /* 109 */  255,   255,     49, },
  { /* 110 */  111,   112,  65535, },
  { /* 111 */  255,   255,     50, },
  { /* 112 */  255,   255,     51, },
  { /* 113 */  114,   123,  65535, },
  { /* 114 */  115,   154,  65535, },
  { /* 115 */  255,   255,     52, },
  { /* 116 */  148,   117,  65535, },
  { /* 117 */  151,   118,  65535, },
  { /* 118 */  255,   255,     53, },
  { /* 119 */  120,   141,  65535, },
  { /* 120 */  121,   255,  65535, },
  { /* 121 */  122,   160,  65535, },
  { /* 122 */  255,   255,     54, },
  { /* 123 */  157,   124,  65535, },
  { /* 124 */  255,   255,     55, },
  { /* 125 */  126,   132,  65535, },
  { /* 126 */  127,   255,  65535, },
  { /* 127 */  255,   255,     56, },
  { /* 128 */  129,   136,  65535, },
  { /* 129 */  130,   131,  65535, },
  { /* 130 */  255,   255,     57, },
  { /* 131 */  255,   255,     58, },
  { /* 132 */  255,   133,  65535, },
  { /* 133 */  255,   255,     59, },
  { /* 134 */  135,   255,  65535, },
  { /* 135 */  255,   255,     60, },
  { /* 136 */  137,   145,  65535, },
  { /* 137 */  255,   255,     61, },
  { /* 138 */  139,   140,  65535, },
  { /* 139 */  255,   255,     62, },
  { /* 140 */  255,   255,     63, },
  { /* 141 */  255,   255,     64, },
  { /* 142 */  143,   144,  65535, },
  { /* 143 */  255,   255,    128, },
  { /* 144 */  255,   255,    192, },
  { /* 145 */  255,   255,    256, },
  { /* 146 */  255,   147,  65535, },
  { /* 147 */  255,   255,    320, },
  { /* 148 */  149,   150,  65535, },
  { /* 149 */  255,   255,    384, },
  { /* 150 */  255,   255,    448, },
  { /* 151 */  152,   153,  65535, },
  { /* 152 */  255,   255,    512, },
  { /* 153 */  255,   255,    576, },
  { /* 154 */  155,   156,  65535, },
  { /* 155 */  255,   255,    640, },
  { /* 156 */  255,   255,    704, },
  { /* 157 */  158,   159,  65535, },
  { /* 158 */  255,   255,    768, },
  { /* 159 */  255,   255,    832, },
  { /* 160 */  161,   162,  65535, },
  { /* 161 */  255,   255,    896, },
  { /* 162 */  255,   255,    960, },
}; /* t4_zrl_binary_tree */

/*-----------------------------------------------------------------------
** Array of mask values to obtain one bit of the CRBB given a bit number
**  0..31  ind = bit div 32, mask32[bit % 32]
** 32..63
** 64..95
** 96..127
**-----------------------------------------------------------------------
*/
static const uint32 mask32[] = 
{
  /*  0 */    0x00000001,
  /*  1 */    0x00000002,
  /*  2 */    0x00000004,
  /*  3 */    0x00000008,
  /*  4 */    0x00000010,
  /*  5 */    0x00000020,
  /*  6 */    0x00000040,
  /*  7 */    0x00000080,
  /*  8 */    0x00000100,
  /*  9 */    0x00000200,
  /* 10 */    0x00000400,
  /* 11 */    0x00000800,
  /* 12 */    0x00001000,
  /* 13 */    0x00002000,
  /* 14 */    0x00004000,
  /* 15 */    0x00008000,
  /* 16 */    0x00010000,
  /* 17 */    0x00020000,
  /* 18 */    0x00040000,
  /* 19 */    0x00080000,
  /* 20 */    0x00100000,
  /* 21 */    0x00200000,
  /* 22 */    0x00400000,
  /* 23 */    0x00800000,
  /* 24 */    0x01000000,
  /* 25 */    0x02000000,
  /* 26 */    0x04000000,
  /* 27 */    0x08000000,
  /* 28 */    0x10000000,
  /* 29 */    0x20000000,
  /* 30 */    0x40000000,
  /* 31 */    0x80000000,
}; /* mask32[] */

/*------------------------------------------------------------------------------------
** Macro to return '1' or '0' from the CRBB given the bit number (0..127) and the 
** pointer to the first element of the CRBB. The CRBB is an array of 4 32-bit numbers
**------------------------------------------------------------------------------------
*/
#define GET_BIT(bit,crbb_ptr)    (((uint32)(crbb_ptr[(bit)/32]) & \
                                   (uint32)(mask32[(bit)%32])     \
                                  ) ==                            \
                                  ((uint32)(mask32[(bit)%32]))    \
                                 )

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*=============== BEGIN T4 TABLES GENERATION & VERIFICATION UTILITY =================*/

#ifdef GENERATE_T4_ROM_TABLES

/*-----------------------------------------------------------------------------------
** Index to the latest item within the T4 Binary Table. This points to the node last
** visited during a search. This value should be reset to 0 (root node) at the 
** beginning of a search. This value is used for searching either one/zero table and 
** is only used to search one table at a time. This is fine as the search alternates 
** between the tables following a starting colour code value
**-----------------------------------------------------------------------------------
*/
static uint16  t4_cur_node = 0;

/*===========================================================================
===
===  FUNCTION      T4SeekFreeNode
===
===  DESCRIPTION
===       Seek the given binary table for a free item starting from the current
===       node plus 1
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===    Index of table
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
uint16 T4SeekFreeNode
(
  t4_binary_tree_node_t   *pstTree,
  uint16                  uStart
)
{
  uint16  u=0;
  
  if (uStart+1 < MAX(T4_ORL_BIN_TABLE_LEN,T4_ZRL_BIN_TABLE_LEN))
  {
    for (u=uStart+1; u<MAX(T4_ORL_BIN_TABLE_LEN,T4_ZRL_BIN_TABLE_LEN); u++)
    {
      if (pstTree[u].left_index == INVALID_INDEX && 
          pstTree[u].right_index == INVALID_INDEX &&
          pstTree[u].run_len == INVALID_RUN_LEN
         )
      {
        return u;
      }
    }  
  }  
  MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_279, "T4SeekFreeNode Bin Table Full");

  return 0;

} /* T4SeekFreeNode */

/*===========================================================================
===
===  FUNCTION      T4BuildBinaryTree
===
===  DESCRIPTION
===       Seek out the given element in the T4 Binary Table, given the params:
===       - uFirstSeek: 0,1. If 1 then the seek is reset to the root node. 
===         If 0 the seek continues from the previous seek.
===       - c: is the character beeing sought '0' or '1'; anything else is an error
===       - uCharInd: index to the character is being sought from left to right.
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4BuildBinaryTree
(
  t4_binary_tree_node_t *pstTree,

  stT4RawTableElement   *pstRawTable,
  uint16                uRawInd,
  uint16                uCharInd
)
{  
  char    c;
  uint16  run_len;
  uint16  uNewInd;

  c = pstRawTable[uRawInd].cpCw[uCharInd];
  
  /* Last char if char index is 1 behind the length of the string since char index 
  ** is 0-based
  */
  run_len = pstRawTable[uRawInd].uRunLen;

  /* Set current index to point to the root node at the start of a run
  */
  if (uCharInd == 0)
    t4_cur_node = 0;

  if (c == '0')   /* look at left index */
  {
    if (pstTree[t4_cur_node].left_index != INVALID_INDEX)
    {  
      /* Found item. Update Current index to point to this new item
      */
      t4_cur_node = pstTree[t4_cur_node].left_index;
    }
    else
    {
      /* create a new item (at t4_cur_node), then update the left and right 
      */
      uNewInd = T4SeekFreeNode (pstTree, t4_cur_node);
      pstTree[t4_cur_node].left_index = uNewInd;
      t4_cur_node = uNewInd;
    }  
  } /* c = '0' */
  else /* c == '1' */
  {
    if (pstTree[t4_cur_node].right_index != INVALID_INDEX)
    {  
      /* Found item. Update Current index to point to this new item
      */
      t4_cur_node = pstTree[t4_cur_node].right_index;
    }
    else
    {
      /* create a new item (at t4_cur_node), then update the left and right indices
      */
      uNewInd = T4SeekFreeNode (pstTree, t4_cur_node);
      pstTree[t4_cur_node].right_index = uNewInd;
      t4_cur_node = uNewInd;
    }
  }  
  
  /* Assign the run length to the current node if the character is the last in the 
  ** codeword
  */
  if ((uCharInd+1) == (uint16)strlen(pstRawTable[uRawInd].cpCw))
  {
    pstTree[t4_cur_node].run_len = run_len;
  }
} /* T4BuildBinaryTree */

/*===========================================================================
===
===  FUNCTION      T4ShowORLBinaryTree()
===
===  DESCRIPTION
===                Show the elements in the T4 Binary Table of One Run Length
===                in a format suitable for producing a ROM-able table
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4ShowORLBinaryTree (void)
{
  uint16  u;
  
  MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_280, "T4 one run length binary tree");
  MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_281, "     Index   Left  Right  Runlength");
  for (u=0; u<T4_ORL_BIN_TABLE_LEN; u++)
  {
    MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_282, "{ /* %3u */  %3u,   %3u, ",
                u,
                T4GeneratedOneRunLengthBinaryTree[u].left_index,
                T4GeneratedOneRunLengthBinaryTree[u].right_index
           );
    
    /* Show 4th value on the next line
    */
    MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_283, "                           %5u, },", 
            T4GeneratedOneRunLengthBinaryTree[u].run_len,
            0,
            0);
  }
} /* T4ShowORLBinaryTree */

/*===========================================================================
===
===  FUNCTION      T4ShowZRLBinaryTree()
===
===  DESCRIPTION
===                Show the elements in the T4 Binary Table of Zero Run Length
===                in a format suitable for producing a ROM-able table
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4ShowZRLBinaryTree (void)
{
  uint16  u;
  
  MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_284, "T4 zero run length binary tree");
  MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_285, "     Index   Left  Right  Runlength");
  for (u=0; u<T4_ZRL_BIN_TABLE_LEN; u++)
  {
    MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_286, "{ /* %3u */  %3u,   %3u, ",
                u,
                T4GeneratedZeroRunLengthBinaryTree[u].left_index,
                T4GeneratedZeroRunLengthBinaryTree[u].right_index
           );
    
    /* Show 4th value on the next line
    */
    MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_287, "                           %5u, },", 
            T4GeneratedZeroRunLengthBinaryTree[u].run_len,
            0,
            0);
  }
} /* T4ShowZRLBinaryTree */

/*===========================================================================
===
===  FUNCTION      T4InitBanryTrees()
===
===  DESCRIPTION
===
===           Fill the T4 tables with invalid values to start with
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4InitBinaryTrees (void)
{
  uint16  u;

  /*-------------------------------------
  ** Init T4 One Run Length Binary Table
  **-------------------------------------
  */
  for (u=0; u<T4_ORL_BIN_TABLE_LEN; u++)
  {
    T4GeneratedOneRunLengthBinaryTree[u].left_index = INVALID_INDEX;
    T4GeneratedOneRunLengthBinaryTree[u].right_index = INVALID_INDEX;
    T4GeneratedOneRunLengthBinaryTree[u].run_len = INVALID_RUN_LEN;
  }

  /*--------------------------------------
  ** Init T4 Zero Run Length Binary Table
  **--------------------------------------
  */
  for (u=0; u<T4_ZRL_BIN_TABLE_LEN; u++)
  {
    T4GeneratedZeroRunLengthBinaryTree[u].left_index = INVALID_INDEX;
    T4GeneratedZeroRunLengthBinaryTree[u].right_index = INVALID_INDEX;
    T4GeneratedZeroRunLengthBinaryTree[u].run_len = INVALID_RUN_LEN;
  }
} /* T4InitBinaryTrees */

/*===========================================================================
===
===  FUNCTION      T4
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4CreateORLBinaryTree (void)
{
  uint16  uCharInd;   /* indexes char of the codeword from left to rigt and 0-based */
  uint16  uInd;       /* indexes the raw table */
  uint16  uLen;       /* length of codeword */

  /* Loop through the length of the codeword. For each char '0' or '1' seek an 
  ** element in the Binary Table
  */

  /* Seek the very first item
  */
  for (uInd=0; uInd<RAW_TABLE_LEN(); uInd++)
  {
    uLen = strlen(T4RawORLTable[uInd].cpCw);
    for (uCharInd=0; uCharInd<uLen; uCharInd++)
    {
      T4BuildBinaryTree (&(T4GeneratedOneRunLengthBinaryTree[0]),
                         &(T4RawORLTable[0]),
                         uInd,
                         uCharInd
                        );
    }
  }  
} /* T4CreateORLBinaryTree */

/*===========================================================================
===
===  FUNCTION      T4CreateZRLBinaryTree()
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4CreateZRLBinaryTree (void)
{
  uint16  uCharInd;   /* indexes the codeword from left to rigt */
  uint16  uInd;       /* indexes the raw table */
  uint16  uLen;       /* length of codeword */

  /* Loop through the length of the codeword. For each char '0' or '1' seek an 
  ** element in the Binary Table
  */

  /* Seek the very first item
  */
  for (uInd=0; uInd<RAW_TABLE_LEN(); uInd++)
  {
    uLen = strlen(T4RawZRLTable[uInd].cpCw);
    for (uCharInd=0; uCharInd<uLen; uCharInd++)
    {
      T4BuildBinaryTree (&(T4GeneratedZeroRunLengthBinaryTree[0]),
                         &(T4RawZRLTable[0]),
                         uInd,
                         uCharInd
                        );
    }
  }  
} /* T4CreateZRLBinaryTree */

/*===========================================================================
===
===  FUNCTION      T4VerifyORLTable
===
===  DESCRIPTION
===
===    This function uses the raw One Run Length Table (T4RawORLTable[]) to
===    feed codewords into the T4Decoder function and checks if the decoder
===    output are correct and matche the values in the raw table. If the test
===    passes it means the T4ORLTable[] generated is correct and the decoder
===    function is also correct.
===
===    This function emits debug messages to indicate PASS or FAILED
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4VerifyORLTable (void)
{
  uint16  u;
  uint16  uLen;       /* length of codeword */
  uint16  uInd;       /* index to binary table */
  uint16  uRawIndex;  /* index to the raw ORL table */

  /* Loop through each codeword in the raw T4 ORL table feeding the
  ** compressed codeword to the T4 decoder
  */
  for (uRawIndex = 0; uRawIndex<RAW_TABLE_LEN(); uRawIndex++)
  {
    uLen = strlen(T4RawORLTable[uRawIndex].cpCw);
    uInd = 0;

    /* Decode loop
    */
    for (u=0; u<uLen; u++)
    {
      /* get new ind
      */
      if (T4RawORLTable[uRawIndex].cpCw[u] == '0')
        uInd = t4_orl_binary_tree[uInd].left_index;
      else
        uInd = t4_orl_binary_tree[uInd].right_index;
    
      if (uInd == INVALID_INDEX)
      {
        MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_288, "T4DecodeORLTest failed at char %u uLen = %u", u, uLen);
        break;
      }
      else
      {
        if (t4_orl_binary_tree[uInd].run_len != INVALID_RUN_LEN)
        {
          break;
        }
      }  
    } /* end decode loop. Decoded run length is in t4_orl_binary_tree[uInd].run_len */

    /*---------------------------------------------------------------------------------
    ** Check the decoder output run length in t4_orl_binary_tree[uInd].run_len against
    ** the value in the raw table and emit PASS/FAILED messages
    **---------------------------------------------------------------------------------
    */
    if (T4RawORLTable[uRawIndex].uRunLen != t4_orl_binary_tree[uInd].run_len)
    {
      MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_289, "T4VerifyORLTable FAILED raw ind = %3u, RL = %3u decoded RL = %3u",
                uRawIndex, 
                T4RawORLTable[uRawIndex].uRunLen, 
                t4_orl_binary_tree[uInd].run_len
             );
      break;
    }
    else
    {
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_290, "T4VerifyORLTable PASS raw ind %3u, RL = %3u decoded RL = %3u",
                uRawIndex, 
                T4RawORLTable[uRawIndex].uRunLen, 
                t4_orl_binary_tree[uInd].run_len
             );
    }  
  } /* end loop through raw table */
} /* T4VerifyORLTable */

/*===========================================================================
===
===  FUNCTION      T4VerifyZRLTable
===
===  DESCRIPTION
===
===    This function uses the raw Zero Run Length Table (T4RawZRLTable[]) to
===    feed codewords into the T4Decoder function and checks if the decoder
===    output are correct and matche the values in the raw table. If the test
===    passes it means the T4ZRLTable[] generated is correct and the decoder
===    function is also correct.
===
===    This function emits debug messages to indicate PASS or FAILED
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4VerifyZRLTable (void)
{
  uint16  u;
  uint16  uLen;       /* length of codeword */
  uint16  uInd;       /* index to binary table */
  uint16  uRawIndex;  /* index to the raw ORL table */

  /* Loop through each codeword in the raw T4 ZRL table feeding the
  ** compressed codeword to the T4 decoder
  */
  for (uRawIndex = 0; uRawIndex<RAW_TABLE_LEN(); uRawIndex++)
  {
    uLen = strlen(T4RawZRLTable[uRawIndex].cpCw);
    uInd = 0;

    /* Decode loop
    */
    for (u=0; u<uLen; u++)
    {
      /* get new ind
      */
      if (T4RawZRLTable[uRawIndex].cpCw[u] == '0')
        uInd = t4_zrl_binary_tree[uInd].left_index;
      else
        uInd = t4_zrl_binary_tree[uInd].right_index;
    
      if (uInd == INVALID_INDEX)
      {
        MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_291, "T4DecodeZRLTest failed at char %u uLen = %u", u, uLen);
        break;
      }
      else
      {
        if (t4_zrl_binary_tree[uInd].run_len != INVALID_RUN_LEN)
        {
          break;
        }
      }  
    } /* end decode loop. Decoded run length is in t4_zrl_binary_tree[uInd].run_len */

    /*---------------------------------------------------------------------------
    ** Check the decoder output run length in t4_zrl_binary_tree[uInd].run_len against
    ** the value in the raw table and emit PASS/FAILED messages
    **---------------------------------------------------------------------------
    */
    if (T4RawZRLTable[uRawIndex].uRunLen != t4_zrl_binary_tree[uInd].run_len)
    {
      MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_292, "T4VerifyZRLTable FAILED raw ind = %3u, RL = %3u decoded RL = %3u",
                uRawIndex, 
                T4RawZRLTable[uRawIndex].uRunLen, 
                t4_zrl_binary_tree[uInd].run_len
             );
      break;
    }
    else
    {
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_293, "T4VerifyZRLTable PASS raw ind %3u, RL = %3u decoded RL = %3u",
                uRawIndex, 
                T4RawZRLTable[uRawIndex].uRunLen, 
                t4_zrl_binary_tree[uInd].run_len
             );
    }  
  } /* end loop through raw table */
} /* T4VerifyZRLTable */

/*===========================================================================
===
===  FUNCTION      T4DecodeORLTest()
===
===  DESCRIPTION
===      Test the t4 decoder by decoding the run length of the string given
===      by cpTestStr[]. Print PASS or FAILED message
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4DecodeORLTest (void)
{
  uint16  u;
  uint16  uLen;                               /* length of codeword */
  uint16  uInd;                             /* index to binary table */
  char *cpTestStr = "011010011";
  /*  char *cpTestStr = "0000000000";  bad string for negative test */
  
  uInd = 0;
  uLen = strlen(cpTestStr);

  for (u=0; u<uLen; u++)
  {
    /* get new ind
    */
    if (cpTestStr[u] == '0')
      uInd = t4_orl_binary_tree[uInd].left_index;
    else
      uInd = t4_orl_binary_tree[uInd].right_index;
    
    if (uInd == INVALID_INDEX)
    {
      MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_294, "T4DecodeORLTest failed at char %u uLen = %u", u, uLen);
      break;
    }
    else
    {
      if (t4_orl_binary_tree[uInd].run_len != INVALID_RUN_LEN)
      {
        MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_295, "T4DecodeORLTest ORL = %u at char %u of uLen = %u",
                  t4_orl_binary_tree[uInd].run_len, 
                  u, 
                  uLen);
        break;
      }
    }  
  }

  if (u != (uLen-1))
  {
    MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_296, "T4DecodeORLTest ORL failed u = %u uLen = %u", u, uLen);
  }
} /* T4DecodeORLTest */

/*===========================================================================
===
===  FUNCTION      T4ActionFunction()
===
===  DESCRIPTION
===
===       Function passed to 
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
boolean T4ActionFunction
(
  uint16  uPosRelSSN,
  uint16  uAckValue
)
{
  MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_297, "T4AFn Pos = %u Ack = %u", uPosRelSSN, uAckValue);

  /* Assume V(A) needs updating
  */
  return TRUE;

} /* T4ActionFunction */

/*===========================================================================
===
===  FUNCTION      T4DecoderTest()
===
===  DESCRIPTION
===
===       Function to present some CRBB scenarios to the decoder. All the blocks
===       acknowledged and dis-acknowledged are shown on screen for visual inspection
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4DecoderTest (void)
{
  uint16  bits_decoded = 0;

  /*------------------------------------------------------------------
  ** Random test codewords. Enable one of
  **------------------------------------------------------------------
  */

#if 0
  uint32  test_crbb[] =
  {
    0x0003439A, /* 0'001 1'010 (32x1), 0'0011 (7x0), '1001'1 (8x1), 010 (3x0) */
    0xffffffff,
    0xffffffff,
    0xffffffff,
  };
  uint16  uLen = 8+5+5+3;
#endif

#if 0
  uint32  test_crbb[] =
  {
    0x173503E7, /* 01'0111' (192x1) '0011'0101' (0x1), '0000'0011'11 (64x0) 
                ** 10' (1x0), '0111 (2x1)
                ** .. summary: 192x1, 65x0, 2x1
                */
    0xffffffff,
    0xffffffff,
    0xffffffff,
  };
  uint16  uLen = 30;
  uint16  cbscc = 1;
#endif

#if 0
  uint32  test_crbb[] =
  {
    0x173503E7, /* 01'0111' (192x1) '0011'0101' (0x1), '0000'0011'11 (64x0) 
                ** 10' (1x0), '0111 (2x1)
                ** .. summary: 192x1, 65x0, 2x1
                ** 0'0111 (10x1), '0001'00.. (9x0)
                */
    0x00000071,
    0xffffffff,
    0xffffffff,
  };
  uint16  uLen = 41;
  uint16  cbscc = 1;
#endif

#if 1
  uint32  test_crbb[] =
  {
    0x00032333, /* 00'0011'0010'00 (128x0) 11' (2x0), '0011'0011' (62x1) */
    0x00000000,
    0xffffffff,
    0xffffffff,
  };
  uint16  uLen = 22;
  uint16  cbscc = 0;
#endif

  (void)grlc_t4_decode (gas_id, &(test_crbb[0]), T4ActionFunction, cbscc, uLen, &bits_decoded);

} /* T4DecoderTest */

/*===========================================================================
===
===  FUNCTION      T4CreateTables()
===
===  DESCRIPTION
===
===       Shell function to initialise, create and verify the one run length
===       and zero run length binary trees. The generated tables are shown on
===       screen for pasting and re-formatting into suitable ROM tables for the
===       target code
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void T4CreateTables (void)
{
  /*------------------------------------------------------
  ** Initialise and create the one run length binary tree
  **------------------------------------------------------
  */
  T4InitBinaryTrees();
  T4CreateORLBinaryTree();

  /*---------------------------------------------------------------------------
  ** Show the contents of the one binary tree in the format that resembles the
  ** table required for ROM-ing the table. The pasted result should be named a
  ** const table for inlusion in the target code
  **---------------------------------------------------------------------------
  */
  T4ShowORLBinaryTree();

  /*---------------------------------------------------------------------------------
  ** Perform a quick test on the one run length table by typing in a code and verify
  ** the result is correct
  **---------------------------------------------------------------------------------
  */
  T4DecodeORLTest();

  /*----------------------------------------------------------------------------------
  ** Exhaustively verifies the one run length table by presenting the raw code to the
  ** decoder and compare the decoder's output with the run length stored in the raw 
  ** table. Prints a message for every code verified.
  **----------------------------------------------------------------------------------
  */
  T4VerifyORLTable();
  
  /*---------------------------------------------------
  ** Create, show and verify the zero run length table
  **---------------------------------------------------
  */
  T4CreateZRLBinaryTree();
  T4ShowZRLBinaryTree();
  T4VerifyZRLTable();

  /*--------------------------------------------------------------------------
  ** Perform T4 decoding tests by presenting the a few scenarios of the CRBB. 
  ** The results must be visually inspected
  **--------------------------------------------------------------------------
  */
  T4DecoderTest();

} /* T4CreateTables */

/*================ END T4 TABLES GENERATION & VERIFICATION UTILITY =================*/

#endif /* GENERATE_T4_ROM_TABLES */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_T4_DECODE
===
===  DESCRIPTION
===
===    Perform T.4 decoding and invoke the action function which carry out
===    actions to positively or negatively acknowledge a data block.
===    This function returns PASS if no errors were found in the compressed
===    bitmap or FAIL otherwise. This function does not return an expanded
===    bitmap as it takes time to format it here and to decode the bitmap from
===    the calling function. Instead the caller passes a pointer to an action 
===    function. When a valid run length is found, this function invokes the
===    action function to take appropriate actions for each block represented
===    by the run length relative to the SSN (starting sequence number) value.
===
===    If an error is encountered during the search the actions carried out
===    by the action function for the previous valid run length can not be 
===    reverted, and the function returns at that point.
===
===  PARAMETERS
===
===    - crbb_ptr: pointer to the first element of the compressed bitmap array.
===            There are 4 elements each of which is 32 bits long
===
===                  b31 ..........................................b0
===      crbb[0] :  [...word5-LSB,MSB-codeword6-high-order-BSNs-LSB]  low-address
===      crbb[1] :  [MSB-codeword3-LSB,MSB-codeword4-LSB,MSB-code..]
===      crbb[2] :  [MSB-makeup-codeword2-LSB,MSB-term-codeword2LSB]
===      crbb[3] :  [un-used bits, MSB-codeword1-low-order-BSNs-LSB]  high-address
===                      
===    - cbscc: compressed bitmap starting colour code
==     - Lc: number of bits in CRBB. eg if Lc = 96 then only crbb[0,1,2] are used
===          and crbb[3] is not used
===    - act_p: pointer to action function. This can be NULL if no actions are required
===    - bits_decoded: actual number of bits decoded. This may be the same as lc
===      or less than lc -- when an error is encountered.
===
===    The orientation of the codewords are such that the low-order BSNs (at SSN) are at
===    higher elements of the crbb array and the highest-order BSNs are always at crbb[0]
===
===    Also the make-up codeword is at the higher order than the associated terminating 
===    codeword, eg. see crbb[2]
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===    FALSE if CRBB contains errors, TRUE otherwise
===
===  SIDE EFFECTS
===
===    t4_current_node changes during the search
===  
===========================================================================*/

boolean grlc_t4_decode
(
  gas_id_t  gas_id,

  /* pointer to uint32 representing the (first uint32 of the ) CRBB array */
  const uint32 *crbb_ptr,
  
  /* pointer to action function taking a uint16 to specify the position of 
  ** the acknowledgement relative to the beginning of the compressed bitmap 
  ** and an uint8 specifying the actual value of acknowledgement value (0 or 1) 
  ** and returning a boolean
  */
  void (*action_func) (gas_id_t gas_id, uint16 position, uint16 ack_value),

  /* compressed bitmap colour code (0 or 1) */
  uint16  cbscc,
  
  /* number of bits in the CRBB */
  uint16  lc,

  /* number of bits actually decoded */
  uint16  *bits_decoded
)
{
  uint16                  bit_ind;          /* value 127 is the MSB of CRBB[3] 
                                            ** value 0 is the LSB of the CRBB[0]
                                            */
  t4_binary_tree_node_t   const *tree_ptr;  /* pointer to a node of a binary tree */
  uint16                  bit;              /* value of a bit read from the CRBB */
  uint16                  bit_cnt;          /* ensure bit count per word is not exceeded.
                                            ** set to max which is 12/13 then counts down
                                            */
  uint16                  u;                /* action function loop count */
  uint16                  tot_run_len = 0;  /* sum of all run lengths found so far */
  
  uint16                  t4_current_node = 0;  /* use local var to ensure re-entrency */

  if (lc == 0 ||
      crbb_ptr == NULL
     )
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_298, "ERLU bad CRBB length 0");
    return FALSE;
  }

  /* Set index to root node
  */
  t4_current_node = 0;

  /* point to the one or zero binary tree according to the starting colour code
  */
  tree_ptr = (cbscc == 1) ? &(t4_orl_binary_tree[0]) : &(t4_zrl_binary_tree[0]);
  
  /* bit count within a codeword search
  */
  bit_cnt = 0;
  
  /* Index is 0-based and start at the highest bit order
  */
  bit_ind = lc;
 
  *bits_decoded = 0;

  do
  {
    /* Get bit, from the MSBit position first
    */
    bit = GET_BIT((bit_ind-1), crbb_ptr);

    /* Increase bit count for the current word -- not the whole bitmap
    */
    ++bit_cnt;
    
    /* Get index to the next node depending on the incoming data bit
    */
    t4_current_node = (bit) ? 
                  tree_ptr[t4_current_node].right_index : 
                  tree_ptr[t4_current_node].left_index;

    /* Is new node a valid node
    */
    if (t4_current_node != INVALID_INDEX)
    {
      /* ..with a valid run length
      */
      if (tree_ptr[t4_current_node].run_len != INVALID_RUN_LEN)
      {
        /* Codeword found
        */
        MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_299, "ERLU t4_dec run length=%u #bits=%u bit=%u",
                tree_ptr[t4_current_node].run_len, 
                bit_cnt, 
                cbscc);

        /* Execute the action function 'run_len' times supplying the cumulative run
        ** lengths thus far and the value of 1 or 0 of the run length.
        **
        ** The calling function should provide tot_run_len+u and add SSN to it ..
        */
        for (u=0; u<tree_ptr[t4_current_node].run_len; u++)
        {
          (*action_func)(gas_id, (uint16)(tot_run_len + u), cbscc);
        }

        /*----------------------------------------
        ** Tally up the run length so far.
        ** Reset bit count for the next codeword.
        **----------------------------------------
        */
        tot_run_len += tree_ptr[t4_current_node].run_len;
        bit_cnt = 0;

        /* Only flip the 'running' colour code if the current run length is less than 64,
        ** ie. the current codeword is a TERMINATING codeword
        ** Point to the next tree.
        */
        if (tree_ptr[t4_current_node].run_len < T4_MAX_TERMINATING_CODEWORDS)
        {
          cbscc ^= 1;
          tree_ptr = (cbscc == 1) ? &(t4_orl_binary_tree[0]) : &(t4_zrl_binary_tree[0]);
        }

        /* Reset index to the root node
        */
        t4_current_node = 0;
      }
    }
    else
    {
      MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_300, "ERLU bad node CurInd=%u bit=%u bit_cnt=%u",
                t4_current_node, 
                bit-1, 
                bit_cnt);

      *bits_decoded = tot_run_len;
      return FALSE;
    }    

    if (bit_cnt >= ( (cbscc == 1) ? T4_MAX_ORL_CODEWORD_LEN : T4_MAX_ZRL_CODEWORD_LEN))
    {
      MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_301, "ERLU t4_dec search failed at node %u after %u bits", 
                t4_current_node, 
                bit_cnt, 
                0);

      *bits_decoded = tot_run_len;
      return FALSE;
    }
    
    --bit_ind;

  } while (bit_ind);

  *bits_decoded = tot_run_len;

  MSG_GRLC_LOW_1_G(GRLC_F3_MSG_ENUM_302, "#decoded = %u",*bits_decoded);
  return TRUE;

} /* grlc_t4_decode */


                       
/*** EOF: don't remove! ***/
