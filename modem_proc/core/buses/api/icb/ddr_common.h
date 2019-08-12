#ifndef DDR_COMMON_H
#define DDR_COMMON_H

#define DDR_COMMON_H_MAJOR  01
#define DDR_COMMON_H_MINOR  04

/**
  @file ddr_common.h
  @brief
  Common definitions used by DDR drivers on all targets.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ddr_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ddr_mainpage.dox file or contact Tech Pubs.
=============================================================================*/
/*=============================================================================
  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
                                EDIT HISTORY

$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/icb/ddr_common.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/17/14   vg      Added this file to support ICB CFG Query in MBA
04/07/14   vg      Added ddr common file to support  64Gb x 4POR DDR in modem
05/06/11   tw      Added structure to support extended ram partition table
03/03/11   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
11/17/10   dxiang  Doxygen grouping changes for API auto-generation
10/11/10   tw      Added sdram interface 2 for newer targets that have more than
                   two interfaces
09/27/10   tw      Added parameter for hal status to save last frequency 
                   switched to
06/21/10   sl      Forced enum to 32 bits; moved private macros to HAL_DDR.h
06/16/10   sl      Doxygenated comments
05/05/10   tw      Added definition for manufacture id
04/19/10   tw      Added more definition for ram size detection constant
02/22/10   tw      Added definition for ram size detection constant
01/07/10   tw      Added new definition for ddr types
10/08/09   TW      Initial revision.
=============================================================================*/

#include "comdef.h"

/** @addtogroup ddr_apis
@{ */

/** SDRAM interfaces. */
typedef enum
{
  SDRAM_INTERFACE_0,                /**< Interface0 -- EBI0 or SMI. */
  SDRAM_INTERFACE_1,                /**< Interface1 -- EBI1 (Channel 0). */
  SDRAM_INTERFACE_2,                /**< Interface1 -- EBI1 (Channel 1). */
  SDRAM_INTERFACE_MAX = 0x7FFFFFFF  /**< Forces the enumerator to 32 bits. */
} SDRAM_INTERFACE;

/** SDRAM chip selects. */
typedef enum
{
  SDRAM_CS0 = 1,                     /**< 0b01 -- chip select 0 only. */
  SDRAM_CS1,                         /**< 0x10 -- chip select 1 only. */
  SDRAM_BOTH,                        /**< 0x11 -- Both chip selects. */
  SDRAM_CHIPSELECT_MAX = 0x7FFFFFFF  /**< Forces the enumerator to 32 bits. */
} SDRAM_CHIPSELECT;

/** SDRAM status. */
typedef enum
{
  DDR_ACTIVE,                    /**< Active mode. */
  DDR_SELF_REFRESH,              /**< Self Refresh mode. */
  DDR_STALL,                     /**< Stalled mode. */
  DDR_DPD,                       /**< Deep Power Down mode. */
  DDR_UNAVAILABLE,               /**< SDRAM is unavailable on this chip select. */
  SDARM_STATUS_MAX = 0x7FFFFFFF  /**< Forces the enumerator to 32 bits. */
} SDRAM_STATUS;

/** DDR types. */
typedef enum
{
  DDR_TYPE_LPDDR1,           /**< Low power DDR1. */
  DDR_TYPE_LPDDR2_S2,        /**< Low power DDR2 S2. */
  DDR_TYPE_LPDDR2_S4,        /**< Low power DDR2 S4. */
  DDR_TYPE_PCDDR2,           /**< Personal computer DDR2. */
  DDR_TYPE_PCDDR3,           /**< Personal computer DDR3. */
  DDR_TYPE_RESERVED,         /**< Reserved for future use. */
  DDR_TYPE_MAX = 0x7FFFFFFF  /**< Forces the enumerator to 32 bits. */
} DDR_TYPE;

/** DDR manufacturers. */
typedef enum
{
  RESERVED_0,                        /**< Reserved for future use. */
  SAMSUNG,                           /**< Samsung. */
  QIMONDA,                           /**< Qimonda. */
  ELPIDA,                            /**< Elpida Memory, Inc. */
  ETRON,                             /**< Etron Technology, Inc. */
  NANYA,                             /**< Nanya Technology Corporation. */
  HYNIX,                             /**< Hynix Semiconductor Inc. */
  MOSEL,                             /**< Mosel Vitelic Corporation. */
  WINBOND,                           /**< Winbond Electronics Corp. */
  ESMT,                              /**< Elite Semiconductor Memory Technology Inc. */
  RESERVED_1,                        /**< Reserved for future use. */
  SPANSION,                          /**< Spansion Inc. */
  SST,                               /**< Silicon Storage Technology, Inc. */
  ZMOS,                              /**< ZMOS Technology, Inc. */
  INTEL,                             /**< Intel Corporation. */
  NUMONYX = 254,                     /**< Numonyx, acquired by Micron Technology, Inc. */
  MICRON = 255,                      /**< Micron Technology, Inc. */
  DDR_MANUFACTURES_MAX = 0x7FFFFFFF  /**< Forces the enumerator to 32 bits. */
} DDR_MANUFACTURES;

/** DDR interleave type */
typedef enum
{
  SDRAM_CS0_INTERLEAVE = 1,
  SDRAM_CS1_INTERLEAVE = 2,
  SDRAM_CS0_CS1_INTERLEAVE = 3,
} ddr_interleave_type;

/** @brief DDR states of all interfaces and chip selects. */
typedef struct
{
  SDRAM_STATUS sdram0_cs0;    /**< SDRAM status of Interface0 and chip select 0. */
  SDRAM_STATUS sdram0_cs1;    /**< SDRAM status of Interface0 and chip select 1. */
  SDRAM_STATUS sdram1_cs0;    /**< SDRAM status of Interface1 and chip select 0. */
  SDRAM_STATUS sdram1_cs1;    /**< SDRAM status of Interface1 and chip select 1. */
  SDRAM_STATUS sdram2_cs0;    /**< SDRAM status of Interface1 and chip select 0. */
  SDRAM_STATUS sdram2_cs1;    /**< SDRAM status of Interface1 and chip select 1. */  
  boolean clk_freq_increase;  /**< Clock frequency is increased or decreased. */
  uint32 clk_speed;           /**< Clock speed. */
} ddr_interface_state;

/** @brief DDR sizes and start addresses of all interfaces and chip selects. */
typedef struct
{
  uint32 sdram0_cs0;       /**< DDR size of Interface0 and chip select 0. */
  uint32 sdram0_cs1;       /**< DDR size of Interface0 and chip select 1. */
  uint32 sdram1_cs0;       /**< DDR size of Interface1 and chip select 0. */
  uint32 sdram1_cs1;       /**< DDR size of Interface0 and chip select 1. */
  uint32 sdram2_cs0;       /**< DDR size of Interface1 and chip select 0. */
  uint32 sdram2_cs1;       /**< DDR size of Interface0 and chip select 1. */
  
  uint32 sdram0_cs0_addr;  /**< DDR start address of Interface0 and chip select 0. */
  uint32 sdram0_cs1_addr;  /**< DDR start address of Interface0 and chip select 1. */
  uint32 sdram1_cs0_addr;  /**< DDR start address of Interface1 and chip select 0. */
  uint32 sdram1_cs1_addr;  /**< DDR start address of Interface1 and chip select 1. */
  uint32 sdram2_cs0_addr;  /**< DDR start address of Interface1 and chip select 0. */
  uint32 sdram2_cs1_addr;  /**< DDR start address of Interface1 and chip select 1. */  
} ddr_size_info;

/** @brief ddr information that are relevent to clients outside of ddr driver */
typedef struct
{
  ddr_size_info ddr_size; /**< DDR size and address configuration */  
  uint32 interleaved_memory; /**< Return whether the ddr is interleaved or not. */
  DDR_TYPE ddr_type;  /**< Return ddr type enum. */ 
} ddr_info;

typedef struct
{
  uint32 num_partitions;   /**< DDR size of Interface0/CS0 */
  uint32 sdram_addr;       /**< DDR start address of Interface0/CS0 */
} ddr_partitions;

typedef struct
{
  ddr_partitions sdram0_cs0; /**< Partition information for Interface 0 CS0 */
  ddr_partitions sdram0_cs1; /**< Partition information for Interface 0 CS1 */
  ddr_partitions sdram1_cs0; /**< Partition information for Interface 1 CS0 */
  ddr_partitions sdram1_cs1; /**< Partition information for Interface 1 CS1 */
} ddr_size_partition;



/** @} */ /* end_addtogroup ddr_data_structures */

#endif  /* DDR_COMMON_H */
