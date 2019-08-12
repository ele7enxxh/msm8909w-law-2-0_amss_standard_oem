/*! \file 
 *  \n
 *  \brief  ftm_pmic_diag_dispatch.h
 *  \details  This header file contains function prototypes for diag dispatchers
 *			  to register PMIC FTM interfaces
 *
 *
 *  \n &copy; Copyright (c) 2012  by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/application/ftm/src/ftm_pmic_diag_dispatch.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
===========================================================================*/

void ftm_pmic_diag_init(void);

PACKED void *  ftm_pmic_dispatch ( PACKED void * request , uint16 length );
