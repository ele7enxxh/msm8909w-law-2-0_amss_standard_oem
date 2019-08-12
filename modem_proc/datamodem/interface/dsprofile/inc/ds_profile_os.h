/******************************************************************************
  @file    ds_profile_os.h
  @brief   Operating System specific header

  DESCRIPTION
  This header defines API for OS specific logging, locking mechanisms. 

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/inc/ds_profile_os.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/

#ifndef DS_PROFILE_OS_H
#define DS_PROFILE_OS_H

#include "datamodem_variation.h"

#if defined(FEATURE_DATA_DS_PROFILE_LINUX)
  #include "ds_profile_os_linux.h"
#elif defined(FEATURE_DATA_DS_PROFILE_AMSS)
  #include "customer.h"
  #include "ds_profile_os_amss.h"
#elif defined(FEATURE_DATA_HLOS_QNX)
  #include "ds_profile_os_qnx.h"
#endif

#endif /* DS_PROFILE_OS_H */
