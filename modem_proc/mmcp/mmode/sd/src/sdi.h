#ifndef SDI_H
#define SDI_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

               I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions used internally by the System
  Determination, as well as, providing internal interface to the SD.C file.

  This header file should NOT be included by any source file outside the
  System Determination module.

  Copyright (c) 2011 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/11/13   xs      SD send acq fail to CM during HDR silent redial
09/12/12   skk     Do not send internal service lost or limited service during uptime.
09/01/12   xs      LTE telescopic BSR in SVLTE support
07/23/12   xs      Added support of location based BSR
03/25/12   cl      Change SD_N_MIN_VOICE_SCANS_DEFAULT_VALUE to 0
01/01/12   cl      n_min_voice feature for SVLTE out of service algorithm
07/24/11   aj      SC SVLTE 1.0 merge to mainline
03/25/11   gm      LTE (Lmtd) to DO resel support
03/15/11   cl      Remove is_active indication from service indication
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/14/11   sv      Replace BOOLEAN with SD_BOOLEAN to avoid conflicts
01/21/11   rm      CMI-4: Moving global/internal data to private files
12/10/10   rk      MO/MT 1XCSFB call successful scenarios support
10/21/10   gm      Changed default values for MCC & IMSI_11_12
06/23/10   ay      Added EOOS support
02/11/10   fj      Added support for MCC-MNC system record.
01/18/10   ay      Removed the fix that disables HICCPS if UOOS feature is on
12/21/09   ak      Removing UOOS
12/03/09   ak      Enabling FEATURE_HICPS_STACK_SYNC_UP on target by default.
06/10/09   fj      Added macros SD_B_UNPACKD, SD_B_PACKB, SD_B_PACKW, and
                   SD_B_PACKD.
04/03/09   aj      Added support for emergency callback exit on sys lost
09/15/08   pk      Added support for WLAN UOoS
03/19/08   cl      Fixing UOOS Time to support Auto Mode Builds.
03/17/08   vs      Added definitions for additional system info
03/12/08   aj      Add header to file reflecting p4 path and revision
12/19/07   ka      Changes to support 16k PRL
09/04/07   cl      Change the interface that detect HDR Support
08/30/07   cl      Added CM critical section support. Also replaced INTLOCK /
                   INTFREE with rex_enter_crit_sect() / rex_leave_crit_sect()
06/19/07   cl      Added in FEATURE_SD_DETECT_HDR_HW
06/06/07   pk      UOOS enhancements...
03/29/07   pk      Added new GW UOOS wakeup time for GCF fix.
05/02/07   jqi     DO not turn off the redirection afer IHO if the expected
                   SID/NID of the redirected system is WILDCARD.
                   Expanded 1x system reselection timer value from 16-bit to
                   32-bit.
03/28/07   sk      Added new GW UOOS wakeup time for GCF fix.
11/15/06   pk      Added support for HYBR_GW
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
12/12/05   jqi     Lint fixes
11/08/05   jqi     Added support for 1x acquisition throttling for BCMCS
                   activity.
11/02/05   sk      Fixed all Lint and compiler warnings.
06/26/05   ic      Lint and compiler warnings cleanup
04/18/05   jqi     Lint error 737(Loss of sign in promotion from int to
                   unsigned long long)
04/04/05   jqi     Lint fixes.
03/28/05   RC      Added macros to extract the low/high 32bits of a QWORD
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/28/04   sj      Moved macros for mode/band pref to sd.h
03/24/04   jqi     Added band and more bit oprt macro.
03/17/04   jqi     Fixed compiler warning.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
12/08/03   SJ      Added support for multistage full srv reselection.
05/22/03   RC      Added definition for magic value that indicates a new SID.
01/16/03   SJ      Added defines to indicate if SD uses DB or CAI.
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
09/07/01   RC      Added support for T53 and JCDMA features.
02/26/01   RC      Changes to support dual-mode targets.
02/01/01   RC      Changes per 8th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
11/27/00   RC      Added sd_ss_act_is_equal().
11/17/00   RC      Initial release.

===========================================================================*/


#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "sd.h"       /* External header file */
#include "sd_v.h"     /* Internal header file */
#include "prl_v.h"

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================== COMMON ===================================
=============================================================================
=============================================================================
===========================================================================*/

/* Max. number of System Records that can be stored in the System Record
** Table. For non 1x targets use a small table size to save on RAM.
*/
#if( !defined( FEATURE_MMODE_CDMA_800 ) && \
     !defined( FEATURE_MMODE_CDMA_1900 ) && \
     !defined( FEATURE_ACP ) && \
     !defined( FEATURE_HDR ) && \
     !defined (FEATURE_SD_LTE))  /* this needs to be removed when GWL dflt PRL is supported */

  #define   PRL_TBL_MAX         20
  #define   PRL_DEFAULT_ENTRIES  0

#else

  #define   PRL_TBL_MAX          414
  #define   PRL_DEFAULT_ENTRIES  14

#endif

/* This values are used for n-min voice scans feature that required the phone
** to scans for n-times for voices systems before starting data.
** The legacy behavior is to scan 2 times PRL/TOT before starting data. These
** 2 scans are composed by 2 different sections: [scan1]+[scan 2], where
** 1st portion [scan 1] is repeated for n-times, and 2nd portion is kept same.
** Thus, for Legacy value, setting n = 1 will reserve the same behavior.
** For SLVTE, after [scan 1], we trigger Data scans. Thus, to be consistent
** with legacy behavior, we shall set default value n=2, which means we shall
** scan 2 times (consistent with legacy behavior).
*/

#define SD_N_MIN_VOICE_SCANS_LEGACY_VALUE 1
#define SD_N_MIN_VOICE_SCANS_DEFAULT_VALUE 0
#define SD_LOC_BASE_BSR_IN_USE_DFT_VAL         0
#define SD_LTE_TELESCOPE_BSR_STAGE_DFT_VAL   1
#define SD_LTE_TELESCOPE_BSR_RPT_DFT_VAL 1
#define SD_LTE_TELESCOPE_BSR_TIME_DFT_VAL 180
#define SD_MAX_ACQ_DB_SCAN_TIME 5
#define SD_T_ACQ_SCAN 5
#define SD_ACQ_SCAN_DURATION 2

/* <EJECT> */
/*---------------------------------------------------------------------------
                         MISCELLANEOUS TYPES AND MACROS
---------------------------------------------------------------------------*/

/* Define to indicate whether SD should use DB services or not.
*/
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) || \
    defined(FEATURE_ACP)      || defined(FEATURE_HDR)
#define SD_USES_DB_SERVICES
#endif


/* Define to indicate whether SD should use CDMA air interface defines
** or not.
*/
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) || \
    defined(FEATURE_ACP)      || defined(FEATURE_HDR)
#define SD_USES_CAI
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* SD value for a wildcard system table index.
*/
#define SD_WILDCARD_SYS_IDX   ((word) (-1))

/* SD value for no change system table index.
*/
#define SD_NO_CHANGE_SYS_IDX  ((word) (-2))

/* SD magic value that indicates a new SID.
*/
#define SD_NEW_SID            (0xFFFF)


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Definition for the additional system info
*/

#define   SDSS_ASSN_TAG_NONE   (0xFF)
#define   SDSS_GEO_OFFSET_NONE (-1)
#define   SDSS_GEO_COUNT_NONE  (0)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SD 1X Scan Time Offset for UOOS. This value is the offset
** between the actual value that writen in NV and the value in real time.
*/
#define SD_UOOS_SCAN_TIME_OFFSET  1


#ifdef FEATURE_EOOS
#error code not present
#endif /* FEATURE_EOOS_UMTS */


/* Following are values for SDEFS_ENHANCED_F3_DBG_RULE
** as of now, it can take maximum 16 bits
*/

#define SD_ENHANCED_F3_DBG_SCRIPT          BM(0)
/* To enable debugs in non-cond constructs of scripts */

#define SD_ENHANCED_F3_DBG_EQPRL           BM(1)
/* To enable debugs in construction of PRL */

#define SD_ENHANCED_F3_DBG_RF_VALID        BM(2)
/* To enable debugs in PRL validation of channels with RF */

#define SD_ENHANCED_F3_DBG_LIST_PRINT      BM(3)
/* To enable debugs in list printing */


/* HDR UOOS scan time, in milliseconds
*/
#define SD_UOOS_ACQ_HYBR_HDR_TIME       850
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to check whether val is between min and max
** (not include min and max).
*/
#define BETWEEN( val, min, max )  ( (val) > (min) && (val) < (max) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to check whether val is in the range of min to max
** (including min and max).
*/
#define INRANGE( val, min, max )  \
/*lint -e568 -e641*/ (((int32)val) >= ((int32)min) && ((int32)val) <= ((int32)max)) \
/*lint +e568 +e641*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to safely assign to a value to a boolean variable.
*/
#ifndef SD_BOOLEAN
#define SD_BOOLEAN( val )  ( (val) ? TRUE : FALSE )
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to convert two values into a word.
*/
#define TO_WORD( word_hi, word_lo ) \
             ( (((word) (word_hi))<<8) | \
               (((word) ((byte) (word_lo)))) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to convert two bytes into a word.
** Not as safe as TO_WORD(), but faster.
*/
#define BYTES_TO_WORD( word_hi, word_lo ) \
             ( (((word) (word_hi))<<8) | \
               ((word) (word_lo)) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to convert four bytes into a dword.
** Not as safe as TO_DWORD(), but faster.
*/
#define BYTES_TO_DWORD( b3, b2, b1, b0 ) \
             ( (((dword) (b3))<<24) | \
               (((dword) (b2))<<16) | \
               (((dword) (b1))<<8)  | \
               ((dword) (b0)) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macros to extract the low/high 32bits of a QWORD (64 bits).
*/
#define DWORD_LOW( dword )  ( (word) ((uint32) dword & (uint32) 0xFFFF))
#define DWORD_HIGH( dword ) ( (word) ((uint32) dword >> 16) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macros to extract 4 bytes of a DWORD (32 bits) from high to low.
e.g.  DWORD_BYTE_3(0x01020304) = 0x01
      DWORD_BYTE_2(0x01020304) = 0x02
      DWORD_BYTE_1(0x01020304) = 0x03
      DWORD_BYTE_0(0x01020304) = 0x04
*/
#define DWORD_BYTE_3( dword ) ( (byte) (((uint32) dword >> 24) & (uint32) 0xFF) )
#define DWORD_BYTE_2( dword ) ( (byte) (((uint32) dword >> 16) & (uint32) 0xFF) )
#define DWORD_BYTE_1( dword ) ( (byte) (((uint32) dword >> 8) & (uint32) 0xFF))
#define DWORD_BYTE_0( dword ) ( (byte) ((uint32) dword & (uint32) 0xFF) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to convert two words into a dword.
*/
#define WORD_TO_DWORD( dword_hi, dword_lo ) \
             ( (((dword) (dword_hi))<<16) | \
               (((dword) ((dword) (dword_lo)))) )


/*---------------------------------------------------------------------------
                               Bit operation MACROS
---------------------------------------------------------------------------*/


/* Macro to check if e1 contains e0.
*/
#define SD_IS_BIT_CONTAIN( e1, e0 ) ( (SD_BOOLEAN ((dword) e1 & (dword) e0)) )
/* Macro to return bits that are set either in e1 or in e0.
*/
#define SD_BIT_ADD( e1, e0 )        ( (word) e1 | (word) e0 )
/* Macro to get common bits in both e1 and e0.
*/
#define SD_BIT_COMMON( e1, e0 )     ( (word) e1 & (word) e0 )
/* Macro to take the bit set in e0 out of e1.
*/
#define SD_BIT_DELETE( e1, e0 )     ( (word) e1 & ~(word) e0 )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to correctly align function definitions (cosmetic).
*/
#define EXTERN


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* define for invalid SS timer.
*/
#define SD_SS_TIMER_INVALID          (dword)(900)

/* <EJECT> */
/*---------------------------------------------------------------------------
                               STACK MACROS
---------------------------------------------------------------------------*/


/* Macros to operate on a stack of elements that is implemented as a
** contiguous array of elements in memory.
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to define a stack of a specified type, name and size.
*/
#define STACK_DEF( type, name, size )  struct { unsigned int  cnt;       \
                                                type          arr[size]; \
                                              } name


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to initialize the a stack.
*/
#define STACK_INIT( s )   (s.cnt = 0)


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the size of a stack.
*/
#define STACK_SIZE( s )   ARR_SIZE( s.arr )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the count (i.e. the number of elements) of a stack.
*/
#define STACK_CNT( s )    ( s.cnt )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to push an item onto a stack.
** Return TRUE if item is successfully pushed on the stack, else return
** FALSE.
*/
#define STACK_PUSH( s, item )                                               \
 ( s.cnt >= STACK_SIZE(s) ? FALSE : (  s.arr[s.cnt] = item,                 \
                                       s.cnt++,                             \
                                       TRUE ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return a pointer to the item at the top of a stack.
** If stack is empty a NULL is returned.
*/
#define STACK_TOP( s ) ( s.cnt == 0 ? NULL : &s.arr[s.cnt-1] )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to pop an item of the top of a stack.
** Return TRUE if item is successfully popped, else return FALSE.
*/
#define STACK_POP( s )  ( s.cnt == 0 ? FALSE : (s.cnt--, TRUE) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Helps overcome limitation of pos being word in b_unpackx functions.
** Done by moving pointer to the closest byte and giving offset of bit
** within that byte.
*/
#define SD_B_UNPACKB(buf_ptr, offset, len) b_unpackb(&(buf_ptr[offset/8]), \
                                                     (word) (offset%8), len)

#define SD_B_UNPACKW(buf_ptr, offset, len) b_unpackw(&(buf_ptr[offset/8]), \
                                                     (word) (offset%8), len)

#define SD_B_UNPACKD(buf_ptr, offset, len) b_unpackd(&(buf_ptr[offset/8]), \
                                                     (word) (offset%8), len)

#define SD_B_PACKB(src, dst_ptr, offset, len) b_packb(src, \
                                                 &(dst_ptr[offset/8]), \
                                                 (word) (offset%8), \
                                                 len)

#define SD_B_PACKW(src, dst_ptr, offset, len) b_packw(src, \
                                                 &(dst_ptr[offset/8]), \
                                                 (word) (offset%8), \
                                                 len)

#define SD_B_PACKD(src, dst_ptr, offset, len) b_packd(src, \
                                                 &(dst_ptr[offset/8]), \
                                                 (word) (offset%8), \
                                                 len)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Table associating PCS frequency blocks with their set of
  ** preferred CDMA channels.
  ** Please refer standard TIA/EIA-97-D
  ** Table 3.1.2-4. CDMA Preferred Set of Frequency Assignments for Band Class 1
  ** Table 3.1.5-4. CDMA Preferred Set of Frequency Assignments for Band Class 4
  ** Table 3.1.6-4. CDMA Preferred Set of Frequency Assignments for Band Class 5

  */
  static const struct {

      byte      block;
          /* PCS frequency block */

      word      chan;
          /* Associated CDMA channel number */

  } prl_pcs_block_chan_tbl[] = {

    /* US PCS block A preferred channels
    */
    { PRL_BLOCK_A,    25 },
    { PRL_BLOCK_A,    50 },
    { PRL_BLOCK_A,    75 },
    { PRL_BLOCK_A,    100 },
    { PRL_BLOCK_A,    125 },
    { PRL_BLOCK_A,    150 },
    { PRL_BLOCK_A,    175 },
    { PRL_BLOCK_A,    200 },
    { PRL_BLOCK_A,    225 },
    { PRL_BLOCK_A,    250 },
    { PRL_BLOCK_A,    275 },

    /* US PCS block B preferred channels.
    */
    { PRL_BLOCK_B,    425 },
    { PRL_BLOCK_B,    450 },
    { PRL_BLOCK_B,    475 },
    { PRL_BLOCK_B,    500 },
    { PRL_BLOCK_B,    525 },
    { PRL_BLOCK_B,    550 },
    { PRL_BLOCK_B,    575 },
    { PRL_BLOCK_B,    600 },
    { PRL_BLOCK_B,    625 },
    { PRL_BLOCK_B,    650 },
    { PRL_BLOCK_B,    675 },

    /* US PCS block C preferred channels.
    */
    { PRL_BLOCK_C,    925 },
    { PRL_BLOCK_C,    950 },
    { PRL_BLOCK_C,    975 },
    { PRL_BLOCK_C,    1000 },
    { PRL_BLOCK_C,    1025 },
    { PRL_BLOCK_C,    1050 },
    { PRL_BLOCK_C,    1075 },
    { PRL_BLOCK_C,    1100 },
    { PRL_BLOCK_C,    1125 },
    { PRL_BLOCK_C,    1150 },
    { PRL_BLOCK_C,    1175 },

    /* US PCS block D preferred channels.
    */
    { PRL_BLOCK_D,    325 },
    { PRL_BLOCK_D,    350 },
    { PRL_BLOCK_D,    375 },

    /* US PCS block E preferred channels.
    */
    { PRL_BLOCK_E,    725 },
    { PRL_BLOCK_E,    750 },
    { PRL_BLOCK_E,    775 },

    /* US PCS block F preferred channels.
    */
    { PRL_BLOCK_F,    825 },
    { PRL_BLOCK_F,    850 },
    { PRL_BLOCK_F,    875 }

  };

/* The BCO preferred set to channels for sub band class 1 , 2 and 3
** The preferred channels of subbandclass 0 are read from NV.
*/
 static const struct {

      prl_bc0_subclass_e_type      sub_bandclass;
          /* Associated sub-bandclass*/

      prl_ab_type                  a_b;
          /* System designator A or B */

      prl_pri_sec_type             pri_sec;
          /* Primary or Secondary channel */

      word                         chan;
          /* Associated CDMA channel number */

  } prl_bc0_subband_chan_tbl[] = {

    {PRL_BC0_SUBCLASS0, PRL_AB_SYSTEM_A, PRL_PRISEC_PRIMARY,   283},
    {PRL_BC0_SUBCLASS0, PRL_AB_SYSTEM_A, PRL_PRISEC_SECONDARY, 691},
    {PRL_BC0_SUBCLASS0, PRL_AB_SYSTEM_B, PRL_PRISEC_PRIMARY,   384},
    {PRL_BC0_SUBCLASS0, PRL_AB_SYSTEM_B, PRL_PRISEC_SECONDARY, 777},

    {PRL_BC0_SUBCLASS1, PRL_AB_SYSTEM_A, PRL_PRISEC_PRIMARY,   779},
    {PRL_BC0_SUBCLASS1, PRL_AB_SYSTEM_A, PRL_PRISEC_SECONDARY, 738},
    {PRL_BC0_SUBCLASS1, PRL_AB_SYSTEM_B, PRL_PRISEC_PRIMARY,   486},
    {PRL_BC0_SUBCLASS1, PRL_AB_SYSTEM_B, PRL_PRISEC_SECONDARY, 568},

    {PRL_BC0_SUBCLASS2, PRL_AB_SYSTEM_A, PRL_PRISEC_PRIMARY,    40},
    {PRL_BC0_SUBCLASS2, PRL_AB_SYSTEM_A, PRL_PRISEC_SECONDARY,1022}

    //  Channel 1273 is not supported in BC0 currently.
    //  Channel 40 already added in subclass 2. So both commented out at present
    //,{PRL_BC0_SUBCLASS3, PRL_AB_SYSTEM_A, PRL_PRISEC_PRIMARY,  1273}
    //,{PRL_BC0_SUBCLASS3, PRL_AB_SYSTEM_A, PRL_PRISEC_SECONDARY,  40}

  };
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================ SERVICE INDICATORS =============================
=============================================================================
=============================================================================
===========================================================================*/


/* Enumeration of service update criterias.
*/
typedef enum {

  SD_SI_UPDATE_ACQ,   /* Update due to successful acquisition */
  SD_SI_UPDATE_OH,    /* Update due to overhead information */
  SD_SI_UPDATE_REG,   /* Update due to successful registration */

  SD_SI_UPDATE_MAX
} sd_si_update_e_type;


/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_srv_off

DESCRIPTION
  Inform the System Indication component that the service indication should
  be turned off.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_srv_off(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */
        boolean                   is_not_true_srv_loss
        /* Indicates whether it is internal or actual service loss */
);

 /*===========================================================================

FUNCTION sd_si_resel_hybr_bsr_state_update

DESCRIPTION
  Notify the registered client (i.e., CM) of change in resel state
  from/to HYBR_BSR states.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void                      sd_si_resel_hybr_bsr_state_update(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR */
        boolean                   is_hybr_bsr
            /* If the new resel state is HYBR_BSR */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_pwr_save

DESCRIPTION
  Inform the System Indication component that the service indication
  is pwr_save.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_srv_pwr_save(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_srv_off_until

DESCRIPTION
  Inform the System Indication component that the service indication should
  be turned off until the specified service update criteria is met.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_srv_off_until(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_si_update_e_type       si_update_criteria
            /* Service update criteria - indicate the minimum criteria for
            ** updating the service indicators.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_acq_fail

DESCRIPTION
  Notify the registered client (i.e., CM) that acquisition over all systems
  has failed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_acq_fail(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);

/*===========================================================================

FUNCTION sd_si_opt_sr_hdr_acq_fail

DESCRIPTION
  Notify the registered client (i.e., CM) that HDR acquisition over all systems
  has failed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_opt_sr_hdr_acq_fail(void);
/*===========================================================================

FUNCTION sd_si_hybr_bsr_to_hdr

DESCRIPTION
  Notify the CM to start lte to hdr hybrid bsr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_hybr_bsr_to_hdr(
	sd_ss_e_type			  ss,

  sd_hybr_bsr_to_hdr_s_type hybr_bsr_to_hdr_status
);

/*===========================================================================

FUNCTION sd_si_srv_lost_cb

DESCRIPTION
  Notify the registered client (i.e., CM) that service has been lost

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_srv_lost_cb(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);

/*===========================================================================

FUNCTION sd_si_full_srv_lost_cb

DESCRIPTION
  Notify the registered client (i.e., CM) that full service has been lost

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_full_srv_lost_cb(
        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);

/*===========================================================================

FUNCTION sdcmd_cdma_bsr_in_progress_cb

DESCRIPTION
  Notify the registered client (i.e., CM) that BSR status has changed

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdcmd_cdma_bsr_in_progress_cb(

       boolean bsr_status
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_emerg_cb_mode

DESCRIPTION
  Notify the registered client (e.g., CM) that SD entered or exited
  emergency callback mode of operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_emerg_cb_mode(
          sd_emerg_cb_mode_e_type          emerg_cb_mode
            /* Callback mode - enter or exit
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_autonam_change

DESCRIPTION
  Notify the registered client (e.g., CM) that auto-NAM change is
  appropriate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_autonam_change(

        sd_nam_e_type             nam
          /* The NAM that is proposed by the auto-NAM functionality.
          */
);


/*===========================================================================

FUNCTION sd_si_service_transfer_complete

DESCRIPTION
  Notify the registered client (i.e., CM) that IRAT has completed in SGLTE
  where we need to collapse 2 stacks into single stack. This collapse happens
  either:
  1) Service Transfer Timer expires and Main has no service.
  2) IRAT from L -> G.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_service_transfer_complete(void);


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== SYSTEM SELECTION =============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sd_ss_act_is_equal

DESCRIPTION
  Indicate whether most recent measurement request is done being serviced by
  Search.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if most recent measurement request is done being serviced by Search.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean          sd_ss_meas_is_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_is_get_net_done

DESCRIPTION
  Indicate whether network list was received for the last get networks
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean          sd_ss_is_get_net_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_update_get_net_done

DESCRIPTION
  Update the get network done flag.
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void             sd_ss_update_get_net_done(

        sd_ss_e_type     ss,
          /* System selection - MAIN or HDR.
          */
        boolean          flag

);

/*===========================================================================

FUNCTION sd_ss_get_timer

DESCRIPTION
  Gets the SS timer

EPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  dword                      sd_ss_get_timer(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);
/* <EJECT> */
/*===========================================================================

FUNCTION sd_rex_enter_crit_sect

DESCRIPTION
  Initialize SD critical section.
  This function must be called before any other part of SD is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void sd_rex_enter_crit_sect(void);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_rex_leave_crit_sect

DESCRIPTION
  Initialize SD critical section.
  This function must be called before any other part of SD is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void sd_rex_leave_crit_sect(void);

/*===========================================================================

FUNCTION sd_filter_out_overlap_band

DESCRIPTION
  Get band preference complies to MS radio access capability.Clear PGSM band
  pref if EGSM band pref is present. Clear both EGSM and PGSM band pref if
  RGSM band pref is present.

  SEMANTIC RULE : Among the three Access Type Technologies GSM 900-P,
  GSM 900-E and GSM 900-R only one shall be  present.

  Refer to 3GPP TS 05.05 Frequency in MHz
             Mobile Tx     Base Station Tx
  PGSM       890-915       935-960
  EGSM       880-915       925-960
  RGSM       876-915       921-960

DEPENDENCIES
  None

RETURN VALUE
  band preference enum type

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_band_pref_e_type    sd_filter_out_overlap_band(

        sd_ss_band_pref_e_type    band_pref
       /* Enumeration of system selection band and sub-band preferences.*/
);

/*===========================================================================

FUNCTION sd_si_get_srv_status

DESCRIPTION
  Get srv status which was reported to client
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_srv_status_e_type                 sd_si_get_srv_status(
  sd_ss_e_type              ss
      /* System selection - MAIN or HDR.
      */

);

/*===========================================================================

FUNCTION sd_print_lte_band_mask

DESCRIPTION
Print the LTE band mask.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
void sd_print_lte_band_mask(sys_lte_band_mask_e_type lte_band_pref);
#endif /* SDI_H */


