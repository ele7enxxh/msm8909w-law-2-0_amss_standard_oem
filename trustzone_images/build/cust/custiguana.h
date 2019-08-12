#ifndef CUSTIGUANA_H
#define CUSTIGUANA_H
/*===========================================================================

            " C u s t - I G U A N A "   H E A D E R   F I L E

DESCRIPTION
  Configuration for All Iguana Feature.

  Copyright (c) 2004       by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custiguana.h#1 $
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/05   AMW     Added BUILD_JFLASH exception
09/21/05   AMW     Added test for building boot and tools
04/25/05   CEP     Removed FEATURE_IG_USB and FEATURE_L4LINUX_USB_TPOS, as
                   that functionality is now in APS, part of AMSS
03/31/05   CEP     Added FEATURE_IG_USB and FEATURE_L4LINUX_USB_TPOS

===========================================================================*/

#if !defined(BUILD_BOOT_CHAIN) && \
    !defined(BUILD_JNAND) && \
    !defined(BUILD_JFLASH) && \
    defined(FEATURE_IGUANA)

  /* Iguana based keypad driver and keypad server */
  #define FEATURE_IG_KEYPAD

  /*  Iguana based uart driver and serial server */
  #define FEATURE_IG_UART

  /*  Iguana based framebuffer driver and framebuffer server */
  #define FEATURE_IG_FB
  #define FEATURE_IG_FRAMEBUFFER

  /*  Iguana based Secure Digital Card Controller and sdio server */
  #define FEATURE_IG_SDIO


  #ifdef FEATURE_IG_UART
  /* The kdb in the pistachio kernel talks directly to the physcial UARTS, and
     uses this feature to decide which UART to use.

     The scons build system uses FEATURE_L4LINUX_UART1 and FEATURE_L4LINUX_UART2
     below to set up linux configuration variables CONFIG_FEATURE_L4LINUX_UART1
     and CONFIG_FEATURE_L4LINUX_UART2. It is not intelligent enough to identify
     "C" style comments. It searches for "#define 'single space'
     FEATURE_L4LINUX_UARTx". Thus take care how the two features below appear
     in this h file.

     SIMPLY COMMENTING OUT THE #DEFINE IS NOT ENOUGH TO DISABLE THE FEATURE!!!
    #define "delete this and uncomment line to define this feature" FEATURE_L4LINUX_UART1
    #define "delete this and uncomment line to define this feature" FEATURE_L4LINUX_UART2
  */

    #define FEATURE_L4LINUX_UART2
  #endif

#else

  #undef FEATURE_IGUANA

#endif /* !BUILD_BOOT_CHAIN & !BUILD_JNAND & !BUILD_JFLASH & FEATURE_IGUANA */

#endif /* CUSTIGUANA_H */

