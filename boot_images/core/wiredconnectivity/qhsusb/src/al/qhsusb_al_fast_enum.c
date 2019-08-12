/*=======================================================================*//**
  @file         qhsusb_al_fast_enum.c

  @brief        Adaptation layer for fast enumeration

  @details      
                In case of data card on PC, BIOS may want to enumerate all
                attached devices in order to check every device against its
                whitelist.
                Application starts stack, polls it until enumeration done,
                then shuts down stack.

                There are 3 flavors:
                - separate descriptor set
                - HDLC descriptor set
                - Charger descriptor set

                In case of HDLC or Charger, memory saving acheived, providing
                HDLC or Charger function already exist in the same image.
 
                Copyright (c) 2008 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/al/qhsusb_al_fast_enum.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $

  when        who       what, where, why
  ----------  ---      -------------------------------------------------------
  2008-10-19  vkondrat FD selection - HDLC, CHG, separate
  2008-09-28  vkondrat Initial version
  
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "qhsusb_dci_api.h"         // qhsusb_dci_disconnect
#include "qhsusb_dcd.h"
#include "qhsusb_fd_fast_enum.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static struct qhsusb_dcd_dsc_device* dev = NULL;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
void qhsusb_al_fast_enum_init(void)
{
   dev = qhsusb_fd_fast_enum_init();
}

void qhsusb_al_fast_enum_shutdown(void)
{
   qhsusb_fd_fast_enum_shutdown();

   qhsusb_dci_disconnect(dev->core_id); // @todo - The qhsusb_fd_fast_enum_shutdown() should call DCI
}


boolean qhsusb_al_fast_enum_poll(void)
{
   qhsusb_dcd_poll(dev);
   return (0 != dev->config_value);
}

void qhsusb_al_fast_enum_delay_usec(uint32 microseconds)
{
   uint32 core_id = dev->core_id;
   uint32 time_usec = 0;

   qhsusb_dci_stop_timer(core_id, QHSUSB_TIMER_1_ONE_SHOT); // stop before start

   qhsusb_dci_start_timer(core_id, QHSUSB_TIMER_1_ONE_SHOT,FALSE);

   do {
      time_usec = qhsusb_dci_get_time_usec(core_id, QHSUSB_TIMER_1_ONE_SHOT); 
   } while ( time_usec < microseconds );

   qhsusb_dci_stop_timer(core_id, QHSUSB_TIMER_1_ONE_SHOT); // stop before start
}

void qhsusb_al_do_fast_enum()
{
#ifdef FEATURE_QHSUSB_FAST_ENUMERATION
   qhsusb_al_fast_enum_init();
   
   while(!qhsusb_al_fast_enum_poll());
   
   qhsusb_al_fast_enum_shutdown();
#endif   
}


