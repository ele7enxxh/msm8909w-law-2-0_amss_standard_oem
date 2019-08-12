#ifndef _SEC_PIL_OEM_H
#define _SEC_PIL_OEM_H

/*===========================================================================

                                sescboot_oem
                                Header File

GENERAL DESCRIPTION
  This header file contains target specific declarations and definitions

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secpil/oem/sec_pil_oem.h#1 $
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who          what, where, why
--------   --------     ----------------------------------------------------------
09/09/14   hw           Initial reversion
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "HALhwio.h"
#include "msmhwiobase.h"
#include "msmhwioreg.h"  

#define SEC_BOOT_MSS_SW_TYPE        (0x2)
#define SECBOOT_ENGG_CERT_SW_TYPE   (SEC_BOOT_MSS_SW_TYPE)

/*OEM fuse All jtag debug disable status
  1 if OEM all jtag debug is disabled, 0 otherwise*/
#define FUSE_ALL_DEBUG_DISABLE  (HWIO_INF(OEM_CONFIG1, ALL_DEBUG_DISABLE))

/*QC fuse MSS jtag debug disable status     
  1 if QC MSS jtag debug is disabled, 0 otherwise*/
#define FUSE_QC_MSS_NIDEN_DISABLE  (HWIO_INF(FEATURE_CONFIG2, QC_MSS_NIDEN_DISABLE) )

/*QC fuse MSS secure jtag debug disable status
  1 if QC MSS secure jtag debug is disabled, 0 otherwise*/
#define FUSE_QC_MSS_DBGEN_DISABLE  (HWIO_INF(FEATURE_CONFIG2, QC_MSS_DBGEN_DISABLE) )

/*OEM fuse MSS jtag debug disable status
  1 if OEM MSS jtag debug is disabled, 0 otherwise*/
#define FUSE_MSS_NIDEN_DISABLE  (HWIO_INF(OEM_CONFIG1, MSS_NIDEN_DISABLE) )

/*OEM fuse MSS secure jtag debug disable status
  1 if OEM MSS secure jtag debug is disabled, 0 otherwise*/
#define FUSE_MSS_DBGEN_DISABLE  (HWIO_INF(OEM_CONFIG1, MSS_DBGEN_DISABLE) )

/*Macro to check if any mpss jtag debug feature is enabled
  return 1 if only all mpss jtag debug are enabled (all fuses return 0),
  return 0 if any jtag debug is disabled (if anyone is 1, it considers disabled) */
#define DEBUG_ENABLED_APPS (!(FUSE_ALL_DEBUG_DISABLE|  \
                              FUSE_QC_MSS_NIDEN_DISABLE|  \
                              FUSE_QC_MSS_DBGEN_DISABLE|  \
                              FUSE_MSS_NIDEN_DISABLE|  \
                              FUSE_MSS_DBGEN_DISABLE ))
                             
#endif  /* _SEC_PIL_OEM_H */
