#ifndef CUSTUI_H
#define CUSTUI_H
/*===========================================================================

DESCRIPTION
  Configuration for UI

  Copyright (c) 2002  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custui.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
 Came from //depot/asic/msm7600/7200_main/build/cust/custui.h#2

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/24/04   pa      Enabled PBM task.
11/11/02   cah     Added GSM features.
05/01/02   jct     Created
===========================================================================*/

/* -----------------------------------------------------------------------
** User Interface
** ----------------------------------------------------------------------- */

/* Support BSP LCD/KEYPAD API
*/
#define FEATURE_BSPUI_API

/* Support of 5GP UI Board
*/
#define FEATURE_ODIE_FONT

/* UI To Indicate on Display if Auto Answer is Enabled
*/
#undef  FEATURE_UI_AUTO_ANS_INDICATE

/* UI "case" screen to use "A->a" (otherwize "X->x")
*/
#undef  FEATURE_UI_CASE_USE_A

/* UI To Support Sending PIN when requested
*/
#undef  FEATURE_UI_SEND_PIN

/* UI To Show "Dropped Call"
*/
#define FEATURE_UI_SHOW_DROP_CALL

/* UI Provides some Alternate Behaviors for the Power Key on Power On and
** Power Off.
*/
#define  FEATURE_UI_PWR_KEY_ALT

/* In alpha edit, the second line will prompt "Enter Name: "
*/
#undef  FEATURE_UI_ENTER_NAME_PROMPT

/* Save the redial number through a power-cycle
*/
#undef  FEATURE_UI_SAVE_REDIAL_BUFFER

/* When this feature is turned on, Old Markov Call is supported.
*/
#undef  FEATURE_UI_OLD_MARKOV

/* Envelope icon is used for voice mail also, just like text messages
*/
#undef  FEATURE_UI_ENVELOPE_VM_ICON

/* Default the IMSI to all 0's is okay
*/
#undef  FEATURE_UI_IMSI_ALL_0_OK

/* Voice Privacy Alert if the Service Alert is enabled.  The first entry
** into enhanced mode per call is ignored so that is doesn't beep in every
** call.  From then on beep every time it toggles.
*/
#undef  FEATURE_UI_PRIVACY_ALERT

/*  Shows preferred Roaming List Version
*/
#define FEATURE_UI_PRL_VER

/* handle forward DTMF */
/* #define FEATURE_HANDLE_FWD_DTMF  -abp */

/* Reorganize Menus when using FEATURE_AUDIO_AGC.
** Puts Audio AGC in the sounds menu rather than features menu.
*/
#define FEATURE_MENU_REORG

/* Enable Battery and Charger Displays
*/
#undef FEATURE_UI_LOW_BATT_W_EXT_PWR

#ifdef FEATURE_BUILD_CDMA_TRIMODE
   /* UI To Support "Force Call" in its.
   */
   #define FEATURE_UI_FORCE_CALL
   #define FEATURE_UI_FORCE_MODE_SEL
#endif

/* Enables the Phone Book Manager interface with existing UI menus 
*/
#define FEATURE_UI_PBM

/* Enable dynamic memory management
*/
#define FEATURE_REX_DYNA_MEM_UI
  
#ifdef FEATURE_GSM

  #ifdef FEATURE_UI_PBM
    /* Enables PhoneBook Manager for ATCOP
    */
    #define FEATURE_ETSI_PBM
    /* Enables the support for Phonebook in GSDI
    */
    #define FEATURE_GSDI_PHONEBOOK_SUPPORT
  #endif

  /* Enables improved GSDI cache access
  */
  #define FEATURE_GSDI_OPTIMIZATION

  /* Enables AOC feature 
  */
  #define FEATURE_AOC

  #ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
    /* Enables +CPIN for ATCOP
	*/
    #define FEATURE_DATA_ETSI_PIN
  #endif /* FEATURE_GSDI_MULTICLIENT_SUPPORT */

#endif /* FEATURE_GSM */
/* Define the dual-display feature.  */
#if (defined(T_MSM6550) && (defined(T_SURF) || defined(T_FFA6550_SS))) || \
     defined(T_MSM7500)
  #if defined FEATURE_MDP
    #define FEATURE_SECONDARY_DISPLAY
  #endif
#endif

/* P1 P2 merge */
/*---------------------------------------------------------------------------
                             User Interface
---------------------------------------------------------------------------*/

/* UI To Indicate on Display if Auto Answer is Enabled
*/
#undef  FEATURE_UI_AUTO_ANS_INDICATE

/* UI "case" screen to use "A->a" (otherwize "X->x")
*/
#undef  FEATURE_UI_CASE_USE_A

/* UI To Show "Dropped Call"
*/
#define FEATURE_UI_SHOW_DROP_CALL

/* UI Provides some Alternate Behaviors for the Power Key on Power On and
** Power Off.
*/
#define  FEATURE_UI_PWR_KEY_ALT

/* Save the redial number through a power-cycle
*/
#undef  FEATURE_UI_SAVE_REDIAL_BUFFER

/* Envelope icon is used for voice mail also, just like text messages
*/
#undef  FEATURE_UI_ENVELOPE_VM_ICON

/* Enable Battery and Charger Displays
*/
#undef FEATURE_UI_LOW_BATT_W_EXT_PWR

/* Define to enable new MSM3300 menu ordering.
*/
#define FEATURE_MENU_REORG

/* Define to indicate to UI usage of new DSM buffer format.
*/
#define FEATURE_NEW_DSM

/* Support BSP LCD/KEYPAD API
*/
#define FEATURE_BSPUI_API

/* Defines Multiple Incoming Call Ringing Support
*/
#define FEATURE_MULTIPLE_RINGER_TYPES

/* Enables the Phone Book Manager interface with existing UI menus 
*/
#ifdef FEATURE_GSM //DNN
#define FEATURE_UI_PBM

#ifdef FEATURE_UI_PBM
  /* Enables PhoneBook Manager for ATCOP
  */
  #define FEATURE_ETSI_PBM
  /* Enables the support for Phonebook in GSDI
  */
  #define FEATURE_GSDI_PHONEBOOK_SUPPORT
#endif
#endif //#ifdef FEATURE_GSM //DNN

/* Enables improved GSDI cache access
*/
#define FEATURE_GSDI_OPTIMIZATION

/* Enables AOC feature 
*/
#define FEATURE_AOC

/* Enables PBM task
*/
#define FEATURE_PBM_TASK

/* disable FEATURE_SECONDARY_DISPLAY on 7200A for CR 130634*/
#if (defined(T_MSM7600)&& defined(T_FFA))
  #undef FEATURE_SECONDARY_DISPLAY
#endif

#endif /* CUSTUI_H */

