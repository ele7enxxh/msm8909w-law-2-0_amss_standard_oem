#ifndef DOG_HB_REX_H
#define DOG_HB_REX_H
/**
  @file dog_hb.h
  @brief This file contains external procedure declarations necessary for
  accessing procedures contained within the watchdog task file.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The watchdog_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and LaTeX. To edit or
      update any of the file/group text in the PDF, edit the
      watchdog_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the dog group
      description in the watchdog_mainpage.dox file.
=============================================================================*/
/*=============================================================================
  Copyright (c) 1990, 1992-1995, 1998-2010 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

            W A T C H D O G    T A S K    H E A D E R    F I L E

DESCRIPTION
  This file contains external procedure declarations necessary for
  accessing procedures contained within the watchdog task file.


===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/debugtools/dog_hb_rex.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

===========================================================================*/

#include "rex.h" // this api uses rex types
#include "dog.h" // this api uses dog types

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/** @addtogroup dog_hb_api
@{ */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
  Dynamically registers tasks for watchdog services. A replacement for hard coded
  timeouts in dog.c for common modem interface (CMI) compliance.

  @param[in] signal_m rex signal mask

  @return
  Handle for tasks to be identified with the watchdog.

  @dependencies
  None.
*/
dog_report_type dog_hb_register_rex(rex_sigs_type signal_m);

/**
  Reports to the watchdog task whether the monitored task is
  functioning properly.

  @param[in]     handle             Dog handle used to tell the watchdog which task is reporting.

  @return
  None.

  @dependencies
  None.
*/
void dog_hb_report(dog_report_type handle);

/**
  Dynamically de-registers tasks for watchdog services for CMI compliance.

  @param[in] handle  Dog handle that the watchdog uses to identify the task.

  @return
  None.

  @dependencies
  None.
*/
void dog_hb_deregister(dog_report_type handle);

/** @} */ /* end_addtogroup dog_api */

#endif  /* DOG_HB_REX_H */
