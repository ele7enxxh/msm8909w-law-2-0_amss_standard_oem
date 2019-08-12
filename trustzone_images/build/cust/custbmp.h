#ifndef CUSTBMP_H
#define CUSTBMP_H
/*===========================================================================

            " C u s t - B M P "   H E A D E R   F I L E

DESCRIPTION
  Configuration for the Bit-Mapped UI card.

  Copyright (c) 2002 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/custbmp.h_v   1.1   21 Mar 2002 20:16:14   swasson  $
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custbmp.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/06   bfc     Removed FEATURE_WALLPAPER for the new UI effort
11/14/03   ht      Enabled FEATURE_ICM and FEATURE_APP_DATASTATS
08/20/02   lz      Enabled FEATURE_UI_CORE
03/15/02   lz      Enabled FEATURE_BREW_LITE
03/16/02    ro     Initial revision.

===========================================================================*/

/*---------------------------------------------------------------------------
                            Bitmap support
---------------------------------------------------------------------------*/

/* Define to enable bitmap display support
*/
#define FEATURE_BITMAP

#ifdef FEATURE_BITMAP

  #define FEATURE_AUXSETTINGS_APP

  /* Define to clk_busy_wait limit on MSM6050
  */
  #define FEATURE_CLK_BUSY_WAIT_LIMIT_65535

  /* Define to use Navigational keypad
  */
  #define FEATURE_NAV_KEYPAD

  /* The power and end keys are combined on the bit-mapped UI card
  */
  #define FEATURE_COMBINED_PWR_END_KEYS

  /* Define to enable REX Heap Manager support
  */
  #define FEATURE_REX_DYNA_MEM

  /* Define to enable ANSI C File API support
  */
  #define FEATURE_ANSIC_FILE_API

  #if defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW)

    /* Define to enable new Overlay Support */

    #ifdef FEATURE_MDP
      #define FEATURE_OVERLAY
    #endif

    /* Define to enable 3G UI Core */
    #define FEATURE_UI_CORE

    #ifdef FEATURE_UI_CORE
      /* The following apps should be removed in HDK builds. */
      #ifndef FEATURE_UIONE_HDK
      /* Contacts (Phonebook) application.
      */
      #define FEATURE_APP_CONTACT
      /* Recent Calls (call history) application.
      */
      #define FEATURE_APP_RECENTCALLS
      /* Dialer (Lite) application.
      */
      #define FEATURE_APP_DIALER

        #define FEATURE_ANNUNCIATOR

      #endif /* FEATURE_UIONE_HDK */

      #define FEATURE_IVECTOR
      /* Service Programming application.
      */
      #define FEATURE_APP_SVCPRG
      /* Field Debug application.
      */
      #define FEATURE_APP_FLDDBG
      #define FEATURE_ICM
      #define FEATURE_APP_DATASTATS
      #define FEATURE_NETSETTINGS_APP
      #ifdef FEATURE_BT
             #define FEATURE_APP_BLUETOOTH
      #endif
    #endif

  #endif  /* FEATURE_BREW_LITE || FEATURE_BREW */
#endif /* FEATURE_BITMAP */

#endif /* CUSTBMP_H */


