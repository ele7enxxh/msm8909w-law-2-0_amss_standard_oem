/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   M A I N   C O N T R O L
               C A P A B I L I T Y   D Y N A M I C   F E A T U R E

GENERAL DESCRIPTION
  This file contains the MC Dynamic Feature functions. The Dynamic Feature
  is to control the feature capability via the NV item at run-time, rather
  than the feature flags during compile time. The Dynamic Feature includes
  optional features in Release 0 and A. When performing the capability
  reporting and service negotiation, it should consult the Dynamic Feature
  capability in this module, rather than rely on the feature flags.

  The Dynamic Feature item can be programmed using service programming
  tools. A verification routine is provided to ensure the Dynamic Feature
  item is programmed correctly.

  Upon power up, MC reads and verifies the Dynamic Feature item. If
  verifications fails, the mobile goes offline/FTM mode, prompting for
  service programming.

EXTERNALIZED FUNCTIONS

  mccap_df_verify -
    This function verifies the dynamic feature NV item based by checking the
    feature flags, hardware versions and other dependencies.

    This function is accessed both internally within this file, and
    externally by the DIAG/QPST when performing the service programming.

  mccap_df_init -
    If the NV item is inactive, it sets the default value and
    writes back to NV. If verification fails but use default value is
    preferred, it will do so and writes back to NV. It then updates the
    system capability with the dynamic feature fields.

    This function has to be called before calling the other functions in
    this file.

  Copyright (c) 2003 - 2014 Qualcomm Technologies, Inc.
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccapdf.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/22/13   bb      Changes to set and reset JCDMA related DFS released items
                   based on passport mode support
06/23/14   ppr     Graceful handling for NV Read/Write error in Card mode
05/06/14   gga     1X_CP F3 redistribution
02/25/14   agh     Moving protocol specific structures from mobile.c/h to 1x CP
02/11/14   gga     Fixed KW errors
08/08/13   agh     Replaced strlcpy with snprintf to handle buffer overflow
                   in mccap_df_create_discovery_string
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
07/29/13   fg      modified mccap_df_nv_write/read
12/06/12   trc     Fix secure stringl.h include
05/31/12   ssh     Added minimum length checks on Diag packet for DF items.
05/24/12   ppr     Feature Cleanup
03/12/12   jj      Turned OFF default support for SVD.
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
09/09/11   ppr     Replaced AEE APIs with the standard C string APIs.
11/15/10   jtm     Initial SU API split for CP.
11/02/09   jtm     Fixed compiler warnings.
08/02/09   jtm     Lint fixes.
05/25/09   pk      Corrected spelling of "FEATURE_IS2000_REL_C_FCS"
05/03/09   jj      CMI Phase-II update.
03/04/09   adw     Added mccdma_v.h for mcc_wait().
07/07/08   ag      Fixed a compiler error.
05/14/08   bb      Renaming AEESTD.h header file to AEEstd.h
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
                   Featurized FEATURE_IS2000_DYNAMIC_FEATURE.
12/14/07   an      Added Passport Mode
05/11/07   pg      More Lint Cleanup.
03/23/07   an      Fixed compiler warnings
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
03/15/07   an      Turn of CHS by default for JCDMA
03/08/07   an      Fixed Critical Lint Errors
10/12/06   pg      Default Dynamic Features: Reverse Pilot Gating and SDB to
                   FALSE under FEATURE_JCDMA_2.
08/09/06   fh      AKA/AES: renamed MCCAP_DF_AES_UI to MCCAP_DF_AES_UI_VOICE
08/04/06   fh      AKA/AES: Changed AES related DF items
07/13/06   fh      Added AES DF support under AES feature
04/26/06   an      Fixed Lint Errors
11/30/05   an      Mainlined FEATURE_IS2000
07/15/05   an      Set the REV_MAX_RATE in set_default and discovery_verify function correctly.
05/12/05   fc      Merged the following from MSMSHARED:
05/12/05   an       Changed function attributes to PACKED
04/05/05   fc      Merged the following from MSMSHARED:
04/05/05   an       Changed number of bits in FWD/REV max rates from 3 to 4
03/28/05   fc      Made FCS and RCNM DF available to Rel D also.
03/23/05   fc      Merged the following from MSMSHARED:
03/25/05   an       Fixed lint errors.
03/21/05   an       Changed the Dynamic Feature Implementation.
09/27/04   va      Merged the following from REL B/C archive:
07/15/04   fc        Removed the DF dependency between Sync ID and Reconnect
                     Message.
07/07/04   fc        Removed the tempory DF tool fix for fast call setup.
09/21/04   az      Changed feature name from FEATURE_REL_D_NEG_SCI to
                   FEATURE_IS2000_REL_D_NEG_SCI
08/23/04   az      Rel D Negative SCI - Complete changes for dynamic feature
08/17/04   az      Rel D Negative SCI - partial changes for making this dynamic
06/16/04   fc      Added DF support for Sync ID and Reconnect message.
06/04/04   bkm     Lint cleanup - added msg.h and err.h includes.
01/05/04   yll     Fixed a problem that in JCDMA, max data rate set by DS was
                   overwritten by MC. Separated the function of initialize
                   the DF item from NV and deciding if going offline.
                   mccap_df_init() is called with the MC task start. It
                   updates the system_capability variables in mccap.c.
                   Mccap_df_load() is called after task start to decide if
                   it needs to go offline due to DF verification failure.
11/03/03   yll     Added an option to continue online if DF verify fails.
05/15/03   yll     Create the module.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "1x_variation.h"
#include "target.h"
#include "comdef.h"

#include "msg.h"
#include "m1x_diag.h"
#include "err.h"
#include "bit.h"
#include "nv.h"
#include "mc.h"
#include "mc_i.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccap_v.h"
#include "mclog_v.h"
#include "mccapdf_v.h"
#include "ran.h"
#include "diagnv.h"
#include "stringl.h"
#include "onex_nv_rd_wt.h"


/*lint -e818 */

#define DF_VALUE(DF_TYPE)  mccap_df_values[DF_TYPE]

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR FILE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* The table contains whether the DF item is supported. */
/* Since the values are also referenced in system_capability even when
** dynamic_feature is not defined, it is defined outside of
** FEATURE_IS2000_DYNAMIC_FEATURE. */

uint8 mccap_df_values[MCCAP_DF_MAX] =
{
    /* MCCAP_DF_P_REV */
#if defined ( FEATURE_IS2000_REL_C )
    P_REV_IS2000_REL_C
#elif defined ( FEATURE_IS2000_REL_B )
    P_REV_IS2000_REL_B
#elif defined ( FEATURE_IS2000_REL_A )
    P_REV_IS2000_REL_A
#else
    P_REV_IS2000
#endif
  , /* MCCAP_DF_QPCH */
    TRUE
  , /* MCCAP_DF_SLOTTED_TIMER */
    TRUE
  , /* MCCAP_DF_CHS */
    FALSE
  , /* MCCAP_DF_SDB */
    FALSE
  , /* MCCAP_DF_CS */
    FALSE
  , /* MCCAP_DF_OTD */
#ifdef FEATURE_IS2000_REL_A
    TRUE
#else
    FALSE
#endif
  , /* MCCAP_DF_STS */
#ifdef FEATURE_IS2000_REL_A
    TRUE
#else
    FALSE
#endif
  , /* MCCAP_DF_DCCH */
    TRUE
  , /* MCCAP_DF_FOR_SCH */
    TRUE
  , /* MCCAP_DF_FOR_MAX_RATE */
#ifdef FEATURE_IS2000_REL_A
    (uint8) CAI_SCH_RATE_RS1_32X_RS2_18X
#else
    (uint8) CAI_SCH_RATE_RS1_16X_RS2_16X
#endif
  , /* MCCAP_DF_REV_SCH */
    TRUE
  , /* MCCAP_DF_REV_MAX_RATE */
#ifdef FEATURE_IS2000_REL_A
    (uint8) CAI_SCH_RATE_RS1_32X_RS2_18X
#else
    (uint8) CAI_SCH_RATE_RS1_16X_RS2_16X
#endif
  , /* MCCAP_DF_MUXPDU5 */
#ifdef FEATURE_IS2000_REL_A
    TRUE
#else
    FALSE
#endif
  , /* MCCAP_DF_REV_FCH_GATING */
    FALSE
  , /* MCCAP_DF_SYNC_ID */
#if defined(FEATURE_IS2000_REL_C)
    TRUE
#else
    FALSE
#endif /* FEATURE_IS2000_REL_C */
  , /* MCCAP_DF_RECONNECT_MSG */
#ifdef FEATURE_IS2000_REL_C
    TRUE
#else
    FALSE
#endif /* FEATURE_IS2000_REL_C */
  , /* MCCAP_DF_NEG_SCI */
    FALSE
  , /* MCCAP_DF_AEHO */
    TRUE
  , /* MCCAP_DF_APHO */
    TRUE
  , /* MCCAP_DF_AES_SIG */
    #ifdef FEATURE_IS2000_REL_A_AES
    TRUE
    #else
    FALSE
    #endif /* FEATURE_IS2000_REL_A_AES */
  , /* MCCAP_DF_AES_UI_VOICE */
    #ifdef FEATURE_IS2000_REL_A_AES
    TRUE
    #else
    FALSE
    #endif /* FEATURE_IS2000_REL_A_AES */
};


#define DF_STATUS(DF_TYPE) mccap_df_tbl[DF_TYPE].df_status

/* DF Items table. To add a DF item, insert the item at the end of this list, specifying
 * the DF status, no of bits, range, min p_rev and value */

LOCAL mccap_df_item_type mccap_df_tbl[] =
{
  {
    "P_REV",
    MCCAP_DF_P_REV,
    TRUE,
    8,
    P_REV_IS95B,
    {P_REV_IS95B,
#if defined ( FEATURE_IS2000_REL_C )
      P_REV_IS2000_REL_C
#elif defined ( FEATURE_IS2000_REL_B )
      P_REV_IS2000_REL_B
#elif defined ( FEATURE_IS2000_REL_A )
      P_REV_IS2000_REL_A
#else
      P_REV_IS2000
#endif
    }
  }
  ,
  {
    "QPCH",
    MCCAP_DF_QPCH,
    TRUE,
    1,
    P_REV_IS2000,
    {0, 1}
  }
  ,
  {
    "SLOTTED TIMER",
    MCCAP_DF_SLOTTED_TIMER,
    TRUE,
    1,
    P_REV_IS2000,
    {0, 1}
  }
  ,
  {
    "CHS",
    MCCAP_DF_CHS,
    TRUE,
    1,
    P_REV_IS2000,
    {0, 1}
  }
  ,
  {
    "SDB",
    MCCAP_DF_SDB,
    TRUE,
    1,
    P_REV_IS2000_REL_A,
    {0, 1}
  }
  ,
  {
    "CS",
    MCCAP_DF_CS,
#ifdef FEATURE_IS2000_REL_A_SVD
    TRUE,
#else
    FALSE,
#endif
    1,
    P_REV_IS2000_REL_A,
    {0, 1}
  }
  ,
  {
    "OTD",
    MCCAP_DF_OTD,
#ifdef FEATURE_IS2000_REL_A
    TRUE,
#else
    FALSE,
#endif
    1,
    P_REV_IS2000_REL_A,
    {0, 1}
  }
  ,
  {
    "STS",
    MCCAP_DF_STS,
#ifdef FEATURE_IS2000_REL_A
    TRUE,
#else
    FALSE,
#endif
    1,
    P_REV_IS2000_REL_A,
    {0, 1}
  }
  ,
  {
    "DCCH",
    MCCAP_DF_DCCH,

    TRUE,

    1,
    P_REV_IS2000,
    {0, 1}
  }
  ,
  {
    "FWD SCH",
    MCCAP_DF_FOR_SCH,
    TRUE,
    1,
    P_REV_IS2000,
    {0, 1}
  }
  ,
  {
    "FWD MAX RATE",
    MCCAP_DF_FOR_MAX_RATE,
    TRUE,
    4,
    P_REV_IS2000,
    {CAI_SCH_RATE_RS1_16X_RS2_16X, CAI_SCH_RATE_RS1_32X_RS2_18X}  /*lint !e641 */
  } /*lint !e641 */
  ,
  {
    "REV SCH",
    MCCAP_DF_REV_SCH,
    TRUE,
    1,
    P_REV_IS2000,
    {0, 1}
  }
  ,
  {
    "REV MAX RATE",
    MCCAP_DF_REV_MAX_RATE,
    TRUE,
    4,
    P_REV_IS2000,
    {CAI_SCH_RATE_RS1_16X_RS2_16X, CAI_SCH_RATE_RS1_32X_RS2_18X}  /*lint !e641 */
  } /*lint !e641 */
  ,
  {
    "MUXPDU5",
    MCCAP_DF_MUXPDU5,
#ifdef FEATURE_IS2000_REL_A
    TRUE,
#else
    FALSE,
#endif
    1,
    P_REV_IS2000_REL_A,
    {0, 1}
  }
  ,
  {
    "REV FCH GATING",
    MCCAP_DF_REV_FCH_GATING,
    TRUE,
    1,
    P_REV_IS2000,
    {0, 1}
  }
  ,
  {
    "SYNC ID",
    MCCAP_DF_SYNC_ID,
#if defined(FEATURE_IS2000_REL_C)
    TRUE,
#else
    FALSE,
#endif /* FEATURE_IS2000_REL_C */
    1,
    P_REV_IS2000_REL_C,
    {0, 1}
  }
  ,
  {
    "RECONNECT MSG",
    MCCAP_DF_RECONNECT_MSG,
#ifdef FEATURE_IS2000_REL_C
    TRUE,
#else
    FALSE,
#endif /* FEATURE_IS2000_REL_C */
    1,
    P_REV_IS2000_REL_C,
    {0, 1}
  }
  ,
  {
    "NEG SCI",
    MCCAP_DF_NEG_SCI,
    FALSE,
    1,
    P_REV_IS2000_REL_D,
    {0, 1}
  }
  ,
  {
    "AEHO",
    MCCAP_DF_AEHO,
    TRUE,
    1,
    P_REV_IS95B,
    {0, 1}
  }
  ,
  {
    "APHO",
    MCCAP_DF_APHO,
    TRUE,
    1,
    P_REV_IS95B,
    {0, 1}
  }
  ,
  {
    "AES SIG",
    MCCAP_DF_AES_SIG,
    #ifdef FEATURE_IS2000_REL_A_AES
    TRUE,
    #else
    FALSE,
    #endif /* FEATURE_IS2000_REL_A_AES */
    1,
    P_REV_IS2000_REL_A,
    {0, 1}
  },
  {
    "AES VOICE",
    MCCAP_DF_AES_UI_VOICE,
    #ifdef FEATURE_IS2000_REL_A_AES
    TRUE,
    #else
    FALSE,
    #endif /* FEATURE_IS2000_REL_A_AES */
    1,
    P_REV_IS2000_REL_A,
    {0, 1}
  }
};

/* Variable to store computed ESN hash */
LOCAL uint8 mccap_df_esn_hash;

/*===========================================================================

FUNCTION MCCAP_DF_SET_DEFAULT

  This function sets the default value for the Dynamic Feature items.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  mccap_df_init() has to be called before calling this function.

===========================================================================*/
void mccap_df_set_default()
{
  int i;                            /* Index to scroll through the DF items */

  /* If the current mobile p_rev is less than the supported p_rev for that
   * DF item, turn off the DF item. We set the DF status as well as the DF
   * value to FALSE. If for some reason the NV read is not successful, the
   * default DF value should be supported by the mobile p_rev */

  for (i=1; i<MCCAP_DF_MAX; i++)   /*lint !e641 */
  {
    if (mccap_df_tbl[i].df_min_p_rev > DF_VALUE(MCCAP_DF_P_REV))
    {
      DF_STATUS(i) = FALSE;
      if ((i != MCCAP_DF_FOR_MAX_RATE) && (i != MCCAP_DF_REV_MAX_RATE))  /*lint !e641 */
      {
        DF_VALUE(i) = FALSE;
      }
    }
    else
    {
      DF_STATUS(i) = TRUE;
    }
  }

  /* Handle dependencies between DF items */

  /* If forward SCH is supported, set the for max rate, depending upon whether MUXPDU5
   * is supported or not */
  if (DF_VALUE(MCCAP_DF_FOR_SCH))
  {
    if (DF_VALUE(MCCAP_DF_MUXPDU5))
    {
      /* Default max rate: 32x */
      DF_VALUE(MCCAP_DF_FOR_MAX_RATE) = CAI_SCH_RATE_RS1_32X_RS2_18X;  /*lint !e641 */
    }
    else
    {
      /* Default max rate: 16x */
      DF_VALUE(MCCAP_DF_FOR_MAX_RATE) = CAI_SCH_RATE_RS1_16X_RS2_16X;  /*lint !e641 */
    }
  }

  /* If reverse SCH is supported, set the rev max rate, depending upon whether MUXPDU5
   * is supported or not */
  if (DF_VALUE(MCCAP_DF_REV_SCH))
  {

    if (DF_VALUE(MCCAP_DF_MUXPDU5))
    {
      /* Default max rate: 32x */
      DF_VALUE(MCCAP_DF_REV_MAX_RATE) = CAI_SCH_RATE_RS1_32X_RS2_18X;  /*lint !e641 */
    }
    else
    {
      /* Default max rate: 16x */
      DF_VALUE(MCCAP_DF_REV_MAX_RATE) = CAI_SCH_RATE_RS1_16X_RS2_16X;  /*lint !e641 */
    }
  }

  /* If CHS is supported and DCCH is not supported, turn off CHS */
  if (DF_VALUE(MCCAP_DF_CHS) && !DF_VALUE(MCCAP_DF_DCCH))
  {
    DF_VALUE(MCCAP_DF_CHS) = FALSE;
  }

 /* Turn off SVD support, even if the SVD feature is enabled */
  if (DF_VALUE(MCCAP_DF_CS))
  {
    DF_VALUE(MCCAP_DF_CS) = FALSE;
  }

  /* set default values if JCDMA is not supported */
  if ( mcc_is_jcdma_supported() )
  {
    DF_VALUE(MCCAP_DF_CHS) = FALSE;
    DF_VALUE(MCCAP_DF_SDB) = FALSE;
    DF_VALUE(MCCAP_DF_REV_FCH_GATING) = FALSE;
  }
  else
  {
    DF_VALUE(MCCAP_DF_CHS) = TRUE;
    DF_VALUE(MCCAP_DF_SDB) = TRUE;
    DF_VALUE(MCCAP_DF_REV_FCH_GATING) = TRUE;    
  }

} /* mccap_df_set_default */

/*===========================================================================

FUNCTION MCCAP_DF_PACK

  This function xlates the Dynamic Feature item from internal to external
  format. The first byte in the DF NV is the number of DF items. Followed
  by the value of each DF item

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_df_pack
(
uint16                length,      /* The length of the buffer */
uint8                 *buf_ptr     /* The pointer to the buffer */
)
{
  uint16    data_pos;
  int i;

  if (length < MC_DYNAMIC_FEATURE_ITEM_BYTE ||!buf_ptr )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid parms");
    return FALSE;
  }

  data_pos = 0;

  /* Pack the number of DF items */
  b_packb( MCCAP_DF_MAX-1, buf_ptr, data_pos, 8);  /*lint !e641 */
  data_pos += 8;

  /* Pack the DF items */
  for (i=1; i < MCCAP_DF_MAX; i++)  /*lint !e641 */
  {
    b_packb( (byte)DF_VALUE(i),
             buf_ptr,
             data_pos,
             mccap_df_tbl[i].df_bits);
    data_pos += mccap_df_tbl[i].df_bits;
  }

  /* Pack the reserved bits */
  if (data_pos < MC_DYNAMIC_FEATURE_ITEM_BYTE * 8) //lint !e506 !e774
  {
    b_packb( 0, buf_ptr, data_pos,
             (MC_DYNAMIC_FEATURE_ITEM_BYTE * 8) - data_pos);
  }

  return TRUE;

} /* mccap_df_pack */

/*===========================================================================

FUNCTION MCCAP_DF_UNPACK

  This function xlates the Dynamic Feature item from external to internal
  format.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if successful.
  FALSE if failed due to invalid length.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_df_unpack
(
uint16                length,     /* The length of buffer */
uint8                 *buf_ptr    /* The pointer to buffer */
)
{

  word    data_pos;
  uint8     version_num, num_df_items;
  int i;
  /* Version map, to map the version number to number of DF items */
  uint8 mccap_df_version_map[] = {14,16,17};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (length < MC_DYNAMIC_FEATURE_ITEM_BYTE || !buf_ptr)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid parms");
    return FALSE;
  }

  data_pos = 0;

  /* Unpack the version number. If the version number is greater than 2, the field
   * denotes the number of DF items. If the version number is less than or equal to
   * 2, we map the version number to the number of DF items */
  version_num = b_unpackb( buf_ptr, data_pos, 8);
  data_pos += 8;
  if (version_num < 3)
  {
    num_df_items = mccap_df_version_map[version_num];
  }
  else
  {
    num_df_items = version_num;
  }

  if (num_df_items >= MCCAP_DF_MAX)  /*lint !e641 */
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Num DF items incorrect");
    return FALSE;
  }

  /* Unpack the DF items */
  for (i = 1; i<=num_df_items; i++)
  {
    DF_VALUE(i) = b_unpackb( buf_ptr, data_pos, mccap_df_tbl[i].df_bits);
    data_pos += mccap_df_tbl[i].df_bits;
  }

  return TRUE;

} /* mccap_df_unpack */

/*===========================================================================

FUNCTION MCCAP_DF_NV_READ
  This function reads the Dynamic Feature item from NV.

DEPENDENCIES
  NV task has to be started.
  This is a blocking function, waiting for NV response before return.

RETURN VALUE
  Status returned by nv_read procedure.

SIDE EFFECTS
  This is a blocking function, waiting for NV response before return.

===========================================================================*/
nv_stat_enum_type mccap_df_nv_read
(
  nv_cmd_type   *cmd_ptr
  /* Pointer to command to send to NV */
)
{
  onex_nv_read_write(NV_DF_I, &mcc_nv_data, NV_RD | NV_MCCAP, mcc_wait, cmd_ptr);

  return(cmd_ptr->status);

} /* mccap_df_nv_read */

/*===========================================================================

FUNCTION MCCAP_DF_NV_WRITE
  This function writes the Dynamic Feature item to NV.

DEPENDENCIES
  NV task has to be started.
  This is a blocking function, waiting for NV response before return.

RETURN VALUE
  Status returned by nv_write procedure.

SIDE EFFECTS
  This is a blocking function, waiting for NV response before return.

===========================================================================*/
nv_stat_enum_type mccap_df_nv_write
(
  nv_cmd_type *cmd_ptr
  /* Pointer to command to send to NV */
)
{
  onex_nv_read_write(NV_DF_I, &mcc_nv_data, NV_WRT | NV_MCCAP, mcc_wait, cmd_ptr);

  return(cmd_ptr->status);

} /* mccap_df_nv_write */

/*===========================================================================

FUNCTION MCCAP_DF_DISCOVERY

  This function discovers the DF items which are supported by the MS. First
  we set the default value of the DF items. Then we read the DF items from the
  NV and verify them and update the DF item table.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the DF items were successfully read.

SIDE EFFECTS
  None

===========================================================================*/
boolean mccap_df_discovery(void)
{
  boolean df_read_ok = TRUE;

  /* Read in the mob_cai_rev. Can't rely on cdma.mob_cai_rev */
  /* because  it may not be initialized yet.                 */
  mcc_nv_buf.cmd        = NV_READ_F;            /* We will do reads        */
  mcc_nv_buf.data_ptr   = &mcc_nv_data;         /* Initialize data pointer */
  mcc_nv_buf.item       = NV_MOB_CAI_REV_I;
  if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
  {
    DF_VALUE(MCCAP_DF_P_REV) = mcc_nv_data.mob_cai_rev; /*lint !e713*/
  }
  else
  {
    df_read_ok = FALSE;
    M1X_MSG( DCP, LEGACY_ERROR,
             "NV_MOB_CAI_REV_I read failed with status:%d", 
             mcc_nv_buf.status );
    return FALSE;
  }

  /* Set the default DF values. These values are compared with the values read from
   * the NV to find any inconsistency the DF item values */
  mccap_df_set_default();

  /* Read the DF items from NV */
  (void )mccap_df_nv_read(&mcc_nv_buf);


  switch (mcc_nv_buf.status)
  {
  case NV_NOTACTIVE_S:
    /* The Dynamic Feature item hasn't been init'ed.
     * Write the default values to the NV */
    M1X_MSG( DCP, LEGACY_MED,
      "NV_DF_I uninitialized, set default");
    mccap_df_print();
    /* Clear the mcc_nv_data buffer */
    memset((void *)&mcc_nv_data, 0, sizeof(mcc_nv_data));

    /* If DF NV is not active, pack the default DF values and write
     * them to the NV */
    (void)mccap_df_pack(MC_DYNAMIC_FEATURE_ITEM_BYTE,
                        (uint8 *)mcc_nv_data.df);
    (void)mccap_df_nv_write(&mcc_nv_buf);
    if(mcc_nv_buf.status != NV_DONE_S)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "NV_DF_I write failed with status:%d", 
               mcc_nv_buf.status );
    }

    break;

  case NV_BADPARM_S:
    /* Check if def val for pwrup nv err feature is enabled */
    if(cdma.use_def_val_for_pwrup_nv_err == TRUE)
    {
      /* The Dynamic Feature item has been tampered.  */
      M1X_MSG( DCP, LEGACY_ERROR,
               "NV_DF_I tampered! Srv prog req!" );
    }
    else
    {
      /* The Dynamic Feature item has been tampered.  */
      M1X_MSG( DCP, LEGACY_FATAL,
               "NV_DF_I tampered! Srv prog req!" );
    }
    df_read_ok = FALSE;
    break;

  case NV_DONE_S:
    /* The Dynamic Feature items have been read from  */
    /* NV successfully. Unpack the data, and verify */
    /* all the fields.                              */

    if (!mccap_df_unpack(MC_DYNAMIC_FEATURE_ITEM_BYTE,
                         (uint8 *)mcc_nv_data.df))
    {
      df_read_ok = FALSE;
      break;
    }

    /* Verify the NV items */
    (void )mccap_df_discovery_verify();

    /* Debug messages */
    mccap_df_print();

    break;

  default:
    /* Check if def val for pwrup nv err feature is enabled */
    if(cdma.use_def_val_for_pwrup_nv_err == TRUE)
    {
      /* The Dynamic Feature item has been tampered.  */
      M1X_MSG( DCP, LEGACY_ERROR,
               "DF NV READ FAIL with status:%d",
               mcc_nv_buf.status );
    }
    else
    {
      /* The Dynamic Feature item has been tampered.  */
      M1X_MSG( DCP, LEGACY_FATAL,
               "DF NV READ FAIL with status:%d",
               mcc_nv_buf.status );
    }
    df_read_ok = FALSE;
    break;

  }

  mccap_df_set_sys_cap ();

  return df_read_ok;

} /* mccap_df_discovery */

/*===========================================================================

FUNCTION MCCAP_DF_QUERY

  This function is called by the DIAG task to query to DF items.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the DF items were successfully read.

SIDE EFFECTS
  This function should be called only from the DIAG task context.

===========================================================================*/
boolean mccap_df_query (PACKED char *df_str, uint32 max_length)
{
  boolean df_read_ok = TRUE;

  if (df_str)
  {

    /* Update the P_REV */
    DF_VALUE(MCCAP_DF_P_REV) = cdma.mob_cai_rev; /*lint !e713*/

    /* Set the default DF values. These values are compared with the values read
     * from the NV to find any inconsistency the DF item values */
    mccap_df_set_default();

    /* Read the DF items from NV */
    if (diag_nv_read(NV_DF_I, &mcc_nv_data) != NV_DONE_S)
    {
      M1X_MSG( DCP, LEGACY_FATAL,
        "Unable to read DF NV items");
      return FALSE;
    }

    /* Unpack the DF NV items */
    if (!mccap_df_unpack(MC_DYNAMIC_FEATURE_ITEM_BYTE, (uint8 *)mcc_nv_data.df))
    {
      M1X_MSG( DCP, LEGACY_FATAL,
        "Unable to unpack the DF items");
      return FALSE;
    }

    /* Verify the NV items */
    (void )mccap_df_discovery_verify();

    /* Create the DF item string */
    mccap_df_create_discovery_string(df_str, max_length);
  }
  else
  {
    M1X_MSG( DCP, LEGACY_FATAL,
      "NULL DF String");
    return FALSE;
  }

  return df_read_ok;

} /* mccap_df_query */

/*===========================================================================

FUNCTION MCCAP_DF_PRINT

  This function prints out the DF fields.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccap_df_print()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  M1X_MSG( DCP, LEGACY_MED,
    " ====== START DYNAMIC FEATURE ====== ");
  M1X_MSG( DCP, LEGACY_MED,
    " mob_cai_rev %d qpch=%d",
    DF_VALUE(MCCAP_DF_P_REV),
    DF_VALUE(MCCAP_DF_QPCH));
  M1X_MSG( DCP, LEGACY_MED,
    " chs=%d sdb=%d cs=%d",
    DF_VALUE(MCCAP_DF_CHS),
    DF_VALUE(MCCAP_DF_SDB),
    DF_VALUE(MCCAP_DF_CS));
  M1X_MSG( DCP, LEGACY_MED,
    " otd=%d sts=%d dcch=%d",
    DF_VALUE(MCCAP_DF_OTD),
    DF_VALUE(MCCAP_DF_STS),
    DF_VALUE(MCCAP_DF_DCCH));
  M1X_MSG( DCP, LEGACY_MED,
    " f_sch=%d f_max_rate=%d r_sch=%d",
    DF_VALUE(MCCAP_DF_FOR_SCH),
    DF_VALUE(MCCAP_DF_FOR_MAX_RATE),
    DF_VALUE(MCCAP_DF_REV_SCH));
  M1X_MSG( DCP, LEGACY_MED,
    " r_max_rate=%d muxpdu5=%d 1/8_gating=%d",
    DF_VALUE(MCCAP_DF_REV_MAX_RATE),
    DF_VALUE(MCCAP_DF_MUXPDU5),
    DF_VALUE(MCCAP_DF_REV_FCH_GATING));
  M1X_MSG( DCP, LEGACY_MED,
    " access_entry_ho %d access_probe_ho %d",
    DF_VALUE(MCCAP_DF_AEHO),
    DF_VALUE(MCCAP_DF_APHO));
  M1X_MSG( DCP, LEGACY_MED,
    " sync_id=%d reconnect_msg=%d rel_d_neg_sci=%d",
    DF_VALUE(MCCAP_DF_SYNC_ID),
    DF_VALUE(MCCAP_DF_RECONNECT_MSG),
    DF_VALUE(MCCAP_DF_NEG_SCI));
  #ifdef FEATURE_IS2000_REL_A_AES
  M1X_MSG( DCP, LEGACY_MED,
    " aes siganling=%d aes ui=%d",
    DF_VALUE(MCCAP_DF_AES_SIG),
    DF_VALUE(MCCAP_DF_AES_UI_VOICE));
  #endif /* FEATURE_IS2000_REL_A_AES */
  M1X_MSG( DCP, LEGACY_MED,
    " ==== END DYNAMIC FEATURE ==== ");

} /* mccap_df_print */

/*===========================================================================

FUNCTION MCCAP_DF_DISCOVERY_VERIFY
  This function checks the dependencies between DF items, which the user may
  have violated.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  mccap_df_init() has to be called before calling this function.

===========================================================================*/
boolean mccap_df_discovery_verify()
{

  boolean nv_items_incompatible = FALSE;
  int i;
  /* For each DF item read from the NV, if the status of that
   * DF item is set to false, then the DF item is not supported in
   * the current build */

  for (i=1; i<MCCAP_DF_MAX; i++)  /*lint !e641 */
  {
    if (!DF_STATUS(i) && DF_VALUE(i))
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DF item %d not supported",
        i);
      if ((i != MCCAP_DF_FOR_MAX_RATE) && (i != MCCAP_DF_REV_MAX_RATE)) /*lint !e641 */
      {
        DF_VALUE(i) = FALSE;
      }
    }
  }

#ifndef FEATURE_IS2000_REL_A
  if (DF_VALUE(MCCAP_DF_FOR_SCH) &&
      DF_VALUE(MCCAP_DF_FOR_MAX_RATE) == CAI_SCH_RATE_RS1_32X_RS2_18X)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "F-SCH 32x not defined");
    /* Default the value */
    DF_VALUE(MCCAP_DF_FOR_MAX_RATE) = CAI_SCH_RATE_RS1_16X_RS2_16X;
    nv_items_incompatible = TRUE;
  }
#endif

#ifndef FEATURE_IS2000_REL_A
  if (DF_VALUE(MCCAP_DF_REV_SCH) &&
      DF_VALUE(MCCAP_DF_REV_MAX_RATE) == CAI_SCH_RATE_RS1_32X_RS2_18X)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "R-SCH 32x not defined");
    /* Default the value */
    DF_VALUE(MCCAP_DF_REV_MAX_RATE) = CAI_SCH_RATE_RS1_16X_RS2_16X;
    nv_items_incompatible = TRUE;
  }
#endif

  /* Turn off CHS if DCCH is not supported */
  if (DF_VALUE(MCCAP_DF_CHS) && !DF_VALUE(MCCAP_DF_DCCH))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CHS suported DCCH not");
    DF_STATUS(MCCAP_DF_CHS) = FALSE;
    DF_VALUE(MCCAP_DF_CHS) = FALSE;
    nv_items_incompatible = TRUE;
  }

  /* Turn on CHS if DCCH is supported */
  if (DF_STATUS(MCCAP_DF_DCCH) && DF_VALUE(MCCAP_DF_DCCH))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Turning on CHS, as DCCH supported");
    DF_STATUS(MCCAP_DF_CHS) = TRUE;
    nv_items_incompatible = TRUE;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DCCH not supported, turning off CHS");
    DF_STATUS(MCCAP_DF_CHS) = FALSE;
    nv_items_incompatible = TRUE;
  }

  /* Check the range of for and rev max rates. All other DF items are boolean
   * either 1 or 0 */
  if (DF_VALUE(MCCAP_DF_FOR_SCH) &&
      ((DF_VALUE(MCCAP_DF_FOR_MAX_RATE) < CAI_SCH_RATE_RS1_16X_RS2_16X) ||  /*lint !e641 */
       (DF_VALUE(MCCAP_DF_FOR_MAX_RATE) > CAI_SCH_RATE_RS1_32X_RS2_18X)))  /*lint !e641 */
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "for_max_rate can only be %d - %d",
              CAI_SCH_RATE_RS1_16X_RS2_16X,
      CAI_SCH_RATE_RS1_32X_RS2_18X);

    if (DF_VALUE(MCCAP_DF_MUXPDU5))
    {
      DF_VALUE(MCCAP_DF_FOR_MAX_RATE) = CAI_SCH_RATE_RS1_32X_RS2_18X;  /*lint !e641 */
    }
    else
    {
      DF_VALUE(MCCAP_DF_FOR_MAX_RATE) = CAI_SCH_RATE_RS1_16X_RS2_16X;  /*lint !e641 */
    }

    nv_items_incompatible = TRUE;
  }


  if (DF_VALUE(MCCAP_DF_REV_SCH) &&
      ((DF_VALUE(MCCAP_DF_REV_MAX_RATE) < CAI_SCH_RATE_RS1_16X_RS2_16X) || /*lint !e641 */
       (DF_VALUE(MCCAP_DF_REV_MAX_RATE) > CAI_SCH_RATE_RS1_32X_RS2_18X)))  /*lint !e641 */
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "for_max_rate can only be %d - %d",
              CAI_SCH_RATE_RS1_16X_RS2_16X,
      CAI_SCH_RATE_RS1_32X_RS2_18X);

    if (DF_VALUE(MCCAP_DF_MUXPDU5))
    {
      DF_VALUE(MCCAP_DF_REV_MAX_RATE) = CAI_SCH_RATE_RS1_32X_RS2_18X;  /*lint !e641 */
    }
    else
    {
      DF_VALUE(MCCAP_DF_REV_MAX_RATE) = CAI_SCH_RATE_RS1_16X_RS2_16X;  /*lint !e641 */
    }

    nv_items_incompatible = TRUE;
  }

  if (!DF_VALUE(MCCAP_DF_MUXPDU5))
  {
    if (DF_VALUE(MCCAP_DF_FOR_MAX_RATE) >= CAI_SCH_RATE_RS1_32X_RS2_18X)  /*lint !e641 */
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Need muxpdu5 to support 32x above");
      DF_VALUE(MCCAP_DF_FOR_MAX_RATE) = CAI_SCH_RATE_RS1_16X_RS2_16X;  /*lint !e641 */
      nv_items_incompatible = TRUE;
    }

    if (DF_VALUE(MCCAP_DF_REV_MAX_RATE) >= CAI_SCH_RATE_RS1_32X_RS2_18X)  /*lint !e641 */
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Need muxpdu5 to support 32x above");
      DF_VALUE(MCCAP_DF_REV_MAX_RATE) = CAI_SCH_RATE_RS1_16X_RS2_16X;  /*lint !e641 */
      nv_items_incompatible = TRUE;
    }
  }

  /* If For Sch or Rev Sch is false, then turn the status field of for max rate
   * and rev max rate to false */

  if (!DF_VALUE(MCCAP_DF_FOR_SCH))
  {
    DF_STATUS(MCCAP_DF_FOR_MAX_RATE) = FALSE;
  }
  else
  {
    DF_STATUS(MCCAP_DF_FOR_MAX_RATE) = TRUE;
  }

  if (!DF_VALUE(MCCAP_DF_REV_SCH))
  {
    DF_STATUS(MCCAP_DF_REV_MAX_RATE) = FALSE;
  }
  else
  {
    DF_STATUS(MCCAP_DF_REV_MAX_RATE) = TRUE;
  }

  /* Turn off SVD support, even if the SVD feature is enabled */
  if (DF_VALUE(MCCAP_DF_CS))
  {
    DF_VALUE(MCCAP_DF_CS) = FALSE;
  }

  /* Reset non supported JCDMA related fields */
  if ( mcc_is_jcdma_supported() )
  {
    DF_VALUE(MCCAP_DF_CHS) = FALSE;
    DF_VALUE(MCCAP_DF_SDB) = FALSE;
    DF_VALUE(MCCAP_DF_REV_FCH_GATING) = FALSE; 
  }
  

  return !nv_items_incompatible;

} /* mccap_df_discovery_verify */

/*===========================================================================

FUNCTION MCCAP_DF_CREATE_DISCOVERY_STRING

  This function updates the dynamic feature character string with the information
  contained in the df_ptr. The first byte in the character string is the number of
  DF items. The format of the string to be sent to the diag is as follows.

  Bytes       Content           Description                      Example
  ----------------------------------------------------------------------------------
  1            Item No.          Item No.                          1
  Len          DF String         The actual DF String              "QPCH"
  1            Status            Flag, whether DF is active        1
  1            Min               Min Value                         0
  1            Max               Max Value                         1
  1            Value             Value either a boolean or a byte   TRUE

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
void mccap_df_create_discovery_string(PACKED char *df_str, uint32 max_length)
{
  int i, j=0;

  /* Create the DF String */

  /* First item in the string is the number of DF items */
  df_str[j++] = MCCAP_DF_MAX;  /*lint !e641 */

  for (i=0; i<MCCAP_DF_MAX; i++)  /*lint !e641 */
  {
    /* Copy the DF item number */
    df_str[j++] = (char)i;
    /* Copy the DF String */
    (void)snprintf((char *)&df_str[j],
                    max_length-j, "%s",
                    mccap_df_tbl[i].df_str_name);

    j = j + (int)strlen(mccap_df_tbl[i].df_str_name) + 1;

    /* The value of j will increase after every iteration and the final value 
     * will be less than the max length received from string buffer. 
     * We are validating j to take care of Klocwork error */

    if(j < max_length - 4 )
    {
      /* Copy the Status */
      df_str[j++] = DF_STATUS(i);

      /* Copy the range */
      df_str[j++] = mccap_df_tbl[i].df_range.df_min;
      df_str[j++] = mccap_df_tbl[i].df_range.df_max;

      /* Copy the actual value */
      df_str[j++] = DF_VALUE(i);
    }/* if(j < max_length - 4 ) */
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
              "Buffer overflow for array df_str, j: %d", j );
      return;
    }/* else */
  }/* for (i=0; i<MCCAP_DF_MAX; i++) */

  mccap_df_esn_hash = df_str[j] = (char)hash(cdma.esn, 256, (word)(cdma.esn & 0xFFF));

}

/*===========================================================================

FUNCTION MCCAP_DF_INIT

  This function calls the mccap_df_discovery function, which loads the dynamic
  feature items from NV and verifies it.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This is a blocking function, waiting for NV response before return.
  system_capability variable will be updated.

===========================================================================*/
void mccap_df_init ( void )
{
  (void )mccap_df_discovery();

} /* mccap_df_init */

/*===========================================================================

FUNCTION MCCAP_DF_LOAD

  This function loads the dynamic feature item from NV and verifies it.
  If verification fails, it will return FALSE to prompt user for service
  programming.

DEPENDENCIES
  None.

RETURN VALUE
  If the NV item is active and the verification passes, it returns TRUE.
  Otherwise it returns FALSE to prompt user for service programming.

SIDE EFFECTS
  This is a blocking function, waiting for NV response before return.

===========================================================================*/
boolean mccap_df_load ( void )
{
  boolean  df_read_ok = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  df_read_ok = mccap_df_discovery();

  return df_read_ok;

} /* mccap_df_load */

/*===========================================================================

FUNCTION MCCAP_DF_VERIFY
  This function verifies a DF item. The request string contains information
  about the DF item to be verified. This function checks for dependency issues
  between DF items, and then returns a string indicating the DF items which
  changed their value, because of the verify command.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the DF item was successfully verified, FALSE otherwise.
SIDE EFFECTS
  None

===========================================================================*/
boolean mccap_df_verify (PACKED char *req_str, PACKED char *rsp_str)
{
  mccap_df_type df_item_type;
  uint8         df_item_value;
  uint8         df_esn_hash;
  uint8         df_cur_value[MCCAP_DF_MAX], df_cur_status[MCCAP_DF_MAX];
  int i, num_df_items_changed = 0, j=1;

  if (req_str == NULL || rsp_str == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "NULL String found");
    return FALSE;
  }

  /* Retrieve the values from the request string. The first byte in the req_str
   * is the df_item_type. The second byte is the DF item value followed by the esn
   * hash */

  df_item_type = (mccap_df_type)req_str[0];      /* DF item type */
  df_item_value = (uint8)req_str[1];             /* DF value */
  df_esn_hash = (uint8)req_str[2];               /* ESN hash */


  /* Validate the esn hash */

  if (df_esn_hash != mccap_df_esn_hash)
  {
    M1X_MSG( DCP, LEGACY_FATAL,
      "DF-ESN-HASH Mismatch. Cur = %d expected = %d",
      df_esn_hash,
      mccap_df_esn_hash);
    return FALSE;
  }

  /* Check the range of df_item_type */
  if (df_item_type >= MCCAP_DF_MAX)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "df_item_type %d beyond range %d",
      df_item_type,
      MCCAP_DF_MAX);
    return FALSE;
  }

  /* If the status of the DF item is set to false return without making any change */
  if (!DF_STATUS(df_item_type)) //lint !e661 Pointer out of bounds check done before
  {
    /* Should never come here */
    M1X_MSG( DCP, LEGACY_FATAL,
      "Status of item %d is false",
      df_item_type);
    return FALSE;
  }

  /* Check the range */
  //lint -e661 Pointer out of bounds check done before
  if (df_item_value < mccap_df_tbl[df_item_type].df_range.df_min ||
      df_item_value > mccap_df_tbl[df_item_type].df_range.df_max)
  //lint +e661
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DF item %d value %d outside range",
      df_item_type,
      df_item_value);
    return FALSE;
  }

  /* Additional check if the P_REV is changed */
  if (df_item_type == MCCAP_DF_P_REV)
  {
    if (!MOB_P_REV_IS_VALID (df_item_value))
    {
      M1X_MSG( DCP, LEGACY_FATAL,
        "P_REV not in range");
      return FALSE;
    }
  }

  /* Update the DF value in the DF table */
  DF_VALUE(df_item_type) = df_item_value;

  /* Remember the current DF item value and status */
  for (i=0; i<MCCAP_DF_MAX; i++)  /*lint !e641 */
  {
    df_cur_value[i] = DF_VALUE(i);
    df_cur_status[i] = DF_STATUS(i); /*lint !e661 !e713*/
  }

  /* Verify the DF item. If P_REV has been changed, enable or disable the status of DF
   * items. */

  if (df_item_type == MCCAP_DF_P_REV)
  {
    for (i=1; i<MCCAP_DF_MAX; i++)  /*lint !e641 */
    {

      /*If the current mobile p_rev is less than the supported p_rev for that
       * DF item, turn off the DF item */
      if (mccap_df_tbl[i].df_min_p_rev > df_item_value)
      {
        DF_STATUS(i) = FALSE;
        if ((i != MCCAP_DF_FOR_MAX_RATE) && (i != MCCAP_DF_REV_MAX_RATE))  /*lint !e641 */
        {
          DF_VALUE(i) = FALSE;
        }
      }
      else
      {
        DF_STATUS(i) = TRUE;
      }
    }
  }

  /* Verify the change made by the user by calling the DF discovery verify function */
  if (!mccap_df_discovery_verify())
  {
    M1X_MSG( DCP, LEGACY_MED,
      "One or more DF items affected by changing DF item %d",
      df_item_type);
  }

  /* Copy the changed DF items */
  for (i=1; i<MCCAP_DF_MAX; i++)  /*lint !e641 */
  {
    if (DF_VALUE(i) != df_cur_value[i] ||
        DF_STATUS(i) != df_cur_status[i])
    {
      num_df_items_changed++;
      /* Item No. */
      rsp_str[j++] = (char)i;

      /* Item Status */
      rsp_str[j++] = DF_STATUS(i);

      /* Item Range */
      rsp_str[j++] = mccap_df_tbl[i].df_range.df_min;
      rsp_str[j++] = mccap_df_tbl[i].df_range.df_max;

      /* Item Value */
      rsp_str[j++] = DF_VALUE(i);
    }
  }

  /* Copy the number of changed DF items */
  rsp_str[0] = (char)num_df_items_changed;

  /* Copy the esn hash. If no items have changed the second byte in the response string
   * is the esn hash */

  rsp_str[j] = mccap_df_esn_hash;


  return TRUE;

} /* mccap_df_verify */

/*===========================================================================

FUNCTION MCCAP_DF_WRITE
  This function writes the DF items to the NV
DEPENDENCIES
  This function should not be called within the MC task context.
RETURN VALUE
  Status whether the write was a SUCCESS or FAILURE
SIDE EFFECTS
  This function should not be called within the MC task context. This function
  makes a call to diag_nv_write which waits on a diag signal for the NV task to
  inform it about NV write status. This function is called as a result of a call
  back which is registered with the diag task, and should be called only within
  the context of diag task.
===========================================================================*/
boolean mccap_df_write (PACKED char *req_str, uint16 pkt_len)
{
  int num_df_items, i, j=0, df_item_no;
  uint8 esn_hash;

  /* This function should not be called within the MC task context. This function
   * makes a call to diag_nv_write which waits on a diag signal for the NV task to
   * inform it about NV write status. This function is called as a result of a call
   * back which is registered with the diag task, and should be called only within
   * the context of diag task.*/

  if ((req_str == NULL) || (pkt_len == sizeof(diagpkt_subsys_header_type)))
  {
    M1X_MSG( DCP, LEGACY_FATAL,
      "NULL String Found!!");
    return FALSE;
  }

  /* Extract the number of DF items */
  num_df_items = (int)req_str[j++];

  M1X_MSG( DCP, LEGACY_MED,
    "extracted num_df_items = %d",
    num_df_items);

  if (num_df_items >= MCCAP_DF_MAX)  /*lint !e641 */
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Num DF items incorrect");
    return FALSE;
  }

  if(pkt_len < sizeof(diagpkt_subsys_header_type) + (num_df_items * 2) + 2)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Incorrect packet length %d, the expected length is %d",
      pkt_len,
      sizeof(diagpkt_subsys_header_type) + (num_df_items * 2) + 2);
    return FALSE;
  }


  /* Validate the esn hash. Esn hash is stored last after all the DF item values.
   * Each DF entry includes the item number and the item value. The first byte
   * is the number of DF items */
  esn_hash = (uint8)req_str[(num_df_items * 2) + 1];
  if (esn_hash != mccap_df_esn_hash)
  {
    M1X_MSG( DCP, LEGACY_FATAL,
      "Incorrect ESN hash!!");
    return FALSE;
  }

  /* Copy the DF item values into the DF table */
  for (i=0; i<num_df_items; i++)
  {
    df_item_no = (int)req_str[j++];

    /* Validate the df item no */
    if (df_item_no >= MCCAP_DF_MAX)  /*lint !e641 */
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "df_item_no = %d incorrect",
        df_item_no);
      return FALSE;
    }

    DF_VALUE(df_item_no) = req_str[j++]; /*lint !e713 */
  }

  /* Verify the DF items. In the NV write command, we expect that the DF item values
   * are correct, and do not cause any dependeny issues. If we encounter a dependency
   * problem, we correct the values and write them to the NV */
  (void )mccap_df_discovery_verify();

  /* Set the system capability information */
  mccap_df_set_sys_cap ();

  /* Write the P_REV to NV */
  mcc_nv_data.mob_cai_rev = DF_VALUE(MCCAP_DF_P_REV); /*lint !e732 */
  if (diag_nv_write(NV_MOB_CAI_REV_I, &mcc_nv_data) != NV_DONE_S)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Unable to write mobile p_rev");
    return FALSE;
  }
  /* Write the DF items to NV */
  /* Clear the mcc_nv_data buffer */
  memset((void *)&mcc_nv_data, 0, sizeof(mcc_nv_data));
  (void)mccap_df_pack(MC_DYNAMIC_FEATURE_ITEM_BYTE,
                      (uint8 *)mcc_nv_data.df);

  return(diag_nv_write(NV_DF_I, &mcc_nv_data) == NV_DONE_S);

}

/*===========================================================================

FUNCTION MCCAP_PM_INIT

  This function initializes the compile time options for JCDMA at runtime
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccap_df_pm_init( void )
{
  /* Initialize the variables based on the MS region. If the feature is not
     defined then the value stays as initialized in the static table above */
  if(!mcc_passport_mode_is_jcdma())
  {
    DF_VALUE(MCCAP_DF_CHS) = TRUE;
    DF_VALUE(MCCAP_DF_SDB) = TRUE;
    DF_VALUE(MCCAP_DF_REV_FCH_GATING) = TRUE;
  }
  else
  {
    DF_VALUE(MCCAP_DF_CHS) = FALSE;
    DF_VALUE(MCCAP_DF_SDB) = FALSE;
    DF_VALUE(MCCAP_DF_REV_FCH_GATING) = FALSE;
  }

}
/*lint +e818 */
