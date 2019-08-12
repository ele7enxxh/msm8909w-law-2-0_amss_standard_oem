#ifndef com_iei_v_h
#define com_iei_v_h
/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited..

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/com_iei.h_v   1.3   19 Mar 2001 17:42:58   tschwarz  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/com_iei_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------



===========================================================================*/

#include "com_iei.h"


/*
 * Defininitions corresponding to the SMS domain selection feature
 */

#define CS_PERMANENT               0xF1
#define PS_PERMANENT               0xF2
#define CS_PS_PERMANENT            0xF3
#define CS_TEMPORARY               0xE1
#define PS_TEMPORARY               0xE2
#define CS_PS_TEMPORARY            0xE3


#define GET_PD(val)   (((byte)val) & (byte)(0x0f))

#define GET_TI(val)   ( (byte)((byte)val & (byte)0xf0) >> 4 )


/*
 * Definitions of the Information Element Identifiers
 * --------------------------------------------------
 */

#define IEI_LAI                     0x13
#define IEI_MOBILE_ID               0x17     /* not in GSM 4.08 */

#ifdef FEATURE_NAS_REL10
#define  IEI_PER_MS_3212            0x35      /* Defined in TS24.008 */
#endif

#ifdef FEATURE_NAS_REL10
#define  IEI_PER_T3246_VALUE        0x36      /* Defined in TS24.008 */
#endif

#define IEI_MS_CLASSMARK_3          0x20

#define IEI_CIPHER_KEY_SEQ_NUMBER   0xf0     /* not in GSM 4.08 */
#define IEI_MS_CLASSMARK_2          0x1f     /* not in GSM 4.08 */
#define IEI_MS_CLASSMARK_1          0x18     /* not in GSM 4.08 */


/*********************************************************************
 * Location area identification ie
 */

typedef struct ie_lai_tag
{
   byte  Iei;
   byte  MccDigit2_1;
   byte  MccDigit3;
   byte  MncDigit2_1;
   byte  LacHi;
   byte  LacLo;
} ie_lai_T;


/*********************************************************************
 * mobile id IE
 */

typedef struct ie_mobile_id_tag
{
   byte  Iei;
   byte  Length;
   byte  Octet3;     /* type of identity, digit 1, odd/even indication */
   byte  DigitPair[7];
} ie_mobile_id_T;


/*********************************************************************
 * mobile station classmark 3 IE
 */

typedef struct ie_ms_classmark_3_tag
{
   byte  Iei;
   byte  Length;
   byte  AlgorithmSupport;
   byte  Spare[11];
} ie_ms_classmark_3_T;

/*
 * Algorithm support bit mapping - in the folowing, if the bit is set to 
 * 1, the algorithm is suppported, otherwise it is not available.
 */

#define MS_CM3_ALGORITHM_A5_4_MASK     0x01
#define MS_CM3_ALGORITHM_A5_5_MASK     0x02
#define MS_CM3_ALGORITHM_A5_6_MASK     0x03
#define MS_CM3_ALGORITHM_A5_7_MASK     0x04

#endif


