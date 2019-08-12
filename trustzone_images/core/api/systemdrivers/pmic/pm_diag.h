/*! \file 
 *  \n
 *  \brief  pm_diag.h ---- PMIC-DIAG
 *  \details  This file contains variable and function declarations to support 
 *   the PMIC diag interface.
 *
 *
 *  \n &copy; Copyright (c) 2009 - 2011  by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/20/10   wra     Added diag call that will allow targets with multiple
                   PMICs to be specifically written to/read from based on
                   their PMIC device index.
08/04/09   jtn     Fix Lint critical and high warnings.
08/01/09   jtn     Added declaration for pm_app_set_mode_diag_i()
06/16/09   jtn     New file
===========================================================================*/

#ifndef PM_DIAG_H
#define PM_DIAG_H
/*===========================================================================
===========================================================================*/
void pm_app_diag_lib_diag_i(PACKED void* req_pkt,PACKED void* data);
void pm_app_diag_lib_diag_ext_i(PACKED void* req_pkt,PACKED void* data);
void pm_app_diag_lib_diag_dev_i(PACKED void* req_pkt,PACKED void* data);
void pm_app_set_mode_diag_i(PACKED void* req_pkt, PACKED void* data);
#endif

