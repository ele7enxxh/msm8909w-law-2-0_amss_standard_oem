#ifndef NAND_CORE_V2_H
#define NAND_CORE_V2_H

/*=======================================================================
 * FILE:        nand_core_v2.h
 *
 * SERVICES:    Flash Programming Plug-In support for SB2.0 configuration
 *
 * DESCRIPTION
 *   This file has the definitions specific for SecureBoot 2.0 architecture
 *
 *        Copyright © 2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *            QUALCOMM Proprietary/GTDR
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/config_v2/nand_core_v2.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 02/27/09     jz      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
 

/* ------------------------------------------------------------------------
** Structure definition for boot Multi-Image Block Storage
** ------------------------------------------------------------------------ */
struct boot_block_ctl;
typedef struct boot_block_ctl *boot_block_ctl_t;

struct boot_block_ctl {
  int    dbl_block;      /* Block number for DBL                */
  int    bbtb_block;     /* Block number for Bad Block Table    */
  int    mibib_block;    /* Block number for MIBIB              */
};

#endif /* NAND_CORE_V2_H */
