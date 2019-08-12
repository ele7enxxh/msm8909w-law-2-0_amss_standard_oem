/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the HDR related AT modem commands for the IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 1995-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707hdrctab_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
			
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc     Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsat707hdr.h"
#include "dsat707hdrctab.h"
#include "dsatparm.h"
#include "msg.h"

#ifdef FEATURE_HDR

/*=========================================================================*/
/* Defaults & Limits for HDR commands */
/*=========================================================================*/
/*---------------------------------------------------------------------------
  Set fixed HDR DRC value.  Default is variable rate.
---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_REVB_ENH_RATES
const dsat_string_item_type dsat707_hdrc_valstr [][8] =
{ "0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F",
  "10","11","12","13","14","15","16","17","18","19","1A","1B" };

LOCAL const dsat_string_item_type dsat707_hdrc_tststr[] =
{ "\n  0 = NULL          \n  1 = 38.4         \n  2 = 76.8"
  "\n  3 = 153.6         \n  4 = 307.2  short \n  5 = 307.2  long" 
  "\n  6 = 614.4  short  \n  7 = 614.4  long  \n  8 = 921.6"
  "\n  9 = 1228.8 short  \n  A = 1228.8 long  \n  B = 1843.2"
  "\n  C = 2457.6        \n  D = 1536.0       \n  E = 3072.0"
  "\n  F = VarRateNonOverride \n  10 = 460.8   \n  11 = 614.4(4slot)"
  "\n  12 = 768.0        \n  13 = 921.6(4slot) \n  14 = 1075.2"
  "\n  15 = 1228.8(4slot) \n  16 = 1843.2(2slot) \n  17 = 2150.4"
  "\n  18 = 2457.6(2slot) \n  19 = 3686.4        \n  1A = 4300.8"
  "\n  1B = 4915.2 \n "
};
LOCAL def_list_type dsat707_qchdrc_list = 
{                                        
  15,  /* Default Value: F = VarRateNonOverride */  
  dsat707_hdrc_valstr,                
  dsat707_hdrc_tststr                 
} ;

#else 
const dsat_string_item_type dsat707_hdrc_valstr [][8] =
{ "0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F" };

LOCAL const dsat_string_item_type dsat707_hdrc_tststr[] =
{ "\n  0 = NULL          \n  1 = 38.4         \n  2 = 76.8"
  "\n  3 = 153.6         \n  4 = 307.2  short \n  5 = 307.2  long" 
  "\n  6 = 614.4  short  \n  7 = 614.4  long  \n  8 = 921.6"
  "\n  9 = 1228.8 short  \n  A = 1228.8 long  \n  B = 1843.2"
  "\n  C = 2457.6        \n  D = 1536.0       \n  E = 3072.0"
  "\n  F = VarRateNonOverride"
};

LOCAL def_list_type dsat707_qchdrc_list = 
{                                        
  15,  /*  Variable Rate */  
  dsat707_hdrc_valstr,                
  dsat707_hdrc_tststr                 
} ;

#endif 

/*---------------------------------------------------------------------------
  Set fixed reverse rate indicator for HDR.  Default is variable rate.
---------------------------------------------------------------------------*/
const dsat_string_item_type dsat707_hdrr_valstr [][8] =
{ "1","2","3","4","5","6","7","8","9","A","B","C","F" };

LOCAL const dsat_string_item_type dsat707_hdrr_tststr[] =
{ "\n  Rev-0 1 = 9.6     \n  Rev-0 2 = 19.2   \n  Rev-0 3 = 38.4"
  "\n  Rev-0 4 = 76.8    \n  Rev-0 5 = 153.6  \n  Rev-0 F = VarRate" 
  "\n  Rev-A 1 = 96      \n  Rev-A 2 = 224    \n  Rev-A 3 = 480" 
  "\n  Rev-A 4 = 736     \n  Rev-A 5 = 992    \n  Rev-A 6 = 1504" 
  "\n  Rev-A 7 = 2016    \n  Rev-A 8 = 3040   \n  Rev-A 9 = 4064" 
  "\n  Rev-A A = 6112    \n  Rev-A B = 8160   \n  Rev-A C = 12256"
  "\n  Rev-A F = VarRate"
};

LOCAL def_list_type dsat707_qchdrr_list = 
{                                        
  12,  /*  Variable Rate */  
  dsat707_hdrr_valstr,                
  dsat707_hdrr_tststr                 
} ;

/*---------------------------------------------------------------------------
  Toggle AT early termination.
---------------------------------------------------------------------------*/
dflm_type dsat707_qchdrt_dflm [] =
{
  { 1  , 0  , 1    },
} ;

/*---------------------------------------------------------------------------
  HDR Reverse link early termination configuration.
---------------------------------------------------------------------------*/
dflm_type dsat707_qchdret_dflm [] =
{
  { 0, 0, 6 },
} ;


/*=========================================================================*/
/* IS-707 HDR related AT command table */
/*=========================================================================*/

/*lint -save -e785 */
#ifdef FEATURE_HDR_AT_TEST_ONLY
const dsati_cmd_type dsat707_hdr_table [] =
{

/*-------------------------------------------------------------------------*/
  { "$QCHDRT", 
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_QCHDRT,
      1, 
      DSAT707_HDR_QCHDRT_IDX,
      &dsat707_qchdrt_dflm[0] },
/*-------------------------------------------------------------------------*/  
  { "$QCHDRC", 
      EXTENDED | CONFIG | LIST | LOCAL_TEST,
      SPECIAL_QCHDRC,
      1,
      DSAT707_HDR_QCHDRC_IDX,
      &dsat707_qchdrc_list },
/*-------------------------------------------------------------------------*/  
  { "$QCHDRR",
      EXTENDED | CONFIG | LIST | LOCAL_TEST,
      SPECIAL_QCHDRR,
      1,
      DSAT707_HDR_QCHDRR_IDX,
      &dsat707_qchdrr_list },
/*-------------------------------------------------------------------------*/
  { "$QCHDRET", 
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_QCHDRET,
      1,
      DSAT707_HDR_QCHDRET_IDX,
      &dsat707_qchdret_dflm[0] }

};

const unsigned int dsat707_hdr_table_size = ARR_SIZE( dsat707_hdr_table );
#endif /* FEATURE_HDR_AT_TEST_ONLY */
/*lint -restore */


#endif /* FEATURE_HDR */

#endif /* FEATURE_DATA_IS707 */
