
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     TIME PROFILE SERVICE
                     -- HDR definitions

GENERAL DESCRIPTION
  The file defined further code parition, events defintions and macros
  definitions in the hdr timeline profiling section; In the HDR section, 
  in addition [15:14] as toplevel tag, bits [13:12] are used for further 
  code partition

EXTERNALIZED FUNCTIONS
  none

INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/03/13   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
03/19/12   ljl     Fixed Dime compiler errors.
04/28/11   kss     Fixed include/compiler warning.
01/13/10   wsh     Merged in definitions from profile_hdr.h 
05/10/05   jyw     created the file

===========================================================================*/
#ifndef FEATURE_HDR_DIME_MODEM
#include "hdr_variation.h"
#include "timetest.h"
#endif /* FEATURE_HDR_DIME_MODEM */

#define HDR_PROFILE_VAL_MASK 0xfff
  /* the HDR used 12 bits for the events output */

/* bits [13:12] is used for sub TAG */
typedef enum
{
  HDR_PROFILE_SUBTAG_SRCH              = 0x0000,
    /* sub TAG [13:8] = 0b __00 xxxx xxxx xxxx for the HDR SRCH */

  /* --- add here for the new sub TAG ---- */

} hdr_profile_subtag_type;

#ifdef FEATURE_HDR_DIME_MODEM
#define HDR_PROFILE( sub_tag,val )
#else
#define HDR_PROFILE( sub_tag,val ) \
  PROFILE_TAG_OUT( PROFILE_TAG_HDR, \
     ( sub_tag )|(( val ) & HDR_PROFILE_VAL_MASK ))
#endif /* FEATURE_HDR_DIME_MODEM */

/* ================ HDR SRCH Profiling Sub Section =============== */

/* Events definitions:
   The number of events has to be less then PROFILE_HDR_VAL_MASK
   see the definitions in the profile_hdr.h. The event 
   HDRSRCH_PROFILE_CC_SLOTS is ORed with the number of
   decoding slot at the profiling output */
typedef enum
{
  HDRSRCH_PROFILE_WAKEUP               = 0xB00,
  HDRSRCH_PROFILE_REACQ_INIT,
  HDRSRCH_PROFILE_REACQ_TUNE_DONE, 
  HDRSRCH_PROFILE_REACQ_SRCH_SCHED,
  HDRSRCH_PROFILE_REACQ_DUMP,

  HDRSRCH_PROFILE_TIME_VALID           = 0xC00,
  HDRSRCH_PROFILE_DEC_INTR ,
  HDRSRCH_PROFILE_CC_SLOTS             = 0xC00,

  HDRSRCH_PROFILE_SLP_CMD              = 0xD00,
  HDRSRCH_PROFILE_SLP_START,
  HDRSRCH_PROFILE_SLP_INIT,
  HDRSRCH_PROFILE_SLEEP,

  HDRSRCH_PROFILE_OFS_START            = 0xE00,
  HDRSRCH_PROFILE_OFS_TUNE_DONE,
  HDRSRCH_PROFILE_OFS_DUMP
} hdrsrch_profile_event_type;

#define HDRSRCH_PROFILE( event ) \
  HDR_PROFILE( HDR_PROFILE_SUBTAG_SRCH, HDRSRCH_PROFILE_##event )

