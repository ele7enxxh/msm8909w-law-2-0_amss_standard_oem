/*==============================================================================

                                boot_apt_test.c

GENERAL DESCRIPTION
  This file contains the function definitions for invoking APT Test Framework.

Copyright 2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE

when       who        what, where, why
--------   ---        ----------------------------------------------------------
05/03/13   jihok      First release
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/
#include "boot_apt_test.h"
#include "boot_logger.h"
#include "boot_apt_test_asm.h"
#include <stdio.h>
#include <string.h>

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

  /* These variables are modified by Trace32 */

static volatile boolean apt_test_run_always = FALSE;
static volatile boolean test_dep_stack = FALSE;
static volatile boolean test_dep_global = FALSE;
static volatile boolean test_dep_code = FALSE;
static volatile boolean test_stack_protection_search = FALSE;
static volatile boolean test_stack_protection_verify_dynamic = FALSE;
static volatile boolean test_stack_protection_overwrite = FALSE;

extern unsigned int __stack_chk_guard[];
static char globalVar[8];

static void apt_canary_overwrite()
{
   int  dummy = 0;
   char *sp_pointer = (char *)&dummy;
   int  limit = 0;
   int  ret = 0;
   char a[8];

   // Search for canary
   for (limit=0; limit < 60; limit++, sp_pointer++) {
      a[7] = 213;
      if (0 == memcmp(sp_pointer, (char *)__stack_chk_guard, 4)) {
         ret = 1;
         break;
      }
   }

   if (ret && a[7] == 213) {
      boot_log_message("Canary found");
      // Overwrite Canary
      *sp_pointer = 0;
   } else {
      boot_log_message("Canary NOT found");
   }
}

static void lennyFunc()
{
   int i;
   for (i=0; i<10; i++) {
      boot_log_message("DENTAL PLAN!");
   }
}

/* =============================================================================
**  Function : apt_single_test
** =============================================================================
*/
/*!
*   @brief
*   This function invokes APT Test API's one time.
*
*
*   @return
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @sa
*   None
*/
static void apt_single_test()
{
   if (test_dep_stack)
   {
      char stackVar[8];

      // Copy function op codes into stack data region
      memcpy(stackVar, apt_execute, 8);

      // Try execution of code on stack variable
      apt_jump(stackVar);

      boot_log_message("TEST FAILURE. DEP did not prevent execution.");
   }
   else if (test_dep_global)
   {
      // Copy function op codes into global data region
      memcpy(globalVar, apt_execute, 8);

      // Try execution of code on global variable
      apt_jump(globalVar);

      boot_log_message("TEST FAILURE. DEP did not prevent execution.");
   }
   else if (test_dep_code)
   {
      // Copy function op codes into code section
      memcpy(lennyFunc, apt_execute, 8);
      boot_log_message("TEST FAILURE. Code overwritten");
   }
   else if (test_stack_protection_search)
   {
      int  dummy = 0;
      char *sp_pointer = (char *)&dummy;
      int  limit = 0;
      int  ret = 0;
      char a[8];

      // Search for canary
      for (limit=0; limit < 60; limit++, sp_pointer++) {
         a[7] = 213;
         if (0 == memcmp(sp_pointer, (char *)__stack_chk_guard, 4)) {
            ret = 1;
            break;
         }
      }

      if (ret && a[7] == 213) {
         boot_log_message("Canary found");
      } else {
         boot_log_message("TEST FAILURE. Canary not found");
      }
   }
   else if (test_stack_protection_verify_dynamic)
   {
      boot_log_message((char*)__stack_chk_guard);
   }
   else if (test_stack_protection_overwrite)
   {
      apt_canary_overwrite();
      boot_log_message("TEST FAILURE. Canary overwritten");
   }
} /* apt_single_test */

/* =============================================================================
**  Function : apt_test
** =============================================================================
*/
/*!
*   @brief
*   This function invokes APT Test Framework.
*
*
*   @return
*   None
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @sa
*   None
*/
void apt_test()
{
   do
   {
      /* loop as long as the test script sets the run always flag to do so */
      apt_single_test();
   } while(apt_test_run_always == TRUE);

} /*apt_test*/
