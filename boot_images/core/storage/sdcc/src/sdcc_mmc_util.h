#ifndef __SDCC_MMC_UTIL_H
#define __SDCC_MMC_UTIL_H
/**********************************************************************
 * File: sdcc_mmc_util.h
 *
 * Services: 
 *    This file provides SDCC util definitions for MMC device.
 *
 * Description:
 *    This file contains the MMC specific utility function prototypes.
 *
 * Copyright (c) 2007-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_mmc_util.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     -----------------------------------------------
2014-01-13   bn      Fixed CMD6 PowerOn Write Protection Failure.
2013-09-03   bn      Add SDHCi, DDR support
2011-09-19   rh      Record the partition setting complete value
2011-09-09   rh      Correctly reporting physical partition size
2011-08-30   rh      Added GPP partitioning support for eMMC
2011-01-03   rh      Correct frequency switch for eMMC card supporting DDR
2010-11-29   rh      Add API to enable the hardware reset function
2009-11-12   vj      Eliminate multiple enumerations of the eMMC card
2009-10-30   sc      Added feature to set active boot partition
2009-10-23   sc      Renamed boot_partition_access to partition_access
2009-10-16   sc      Added power-on write protection feature
2009-10-12   sc      Updated sdcc_ext_csd_type per eMMC v4.4
2009-08-20   sc      Removed the 1ms wait for HS_TIMING and BUS_WIDTH setting
2009-08-18   sc      Added MMC spec version definitions
2009-08-13   sc      Added device type and ext_csd_rev definitions
2009-06-15   rh      Adding common interface extraction tag
2009-05-20   sc      Consolidated physical partition info support to get device info
2009-05-13   sc      Fixed issue taking up 25% CPU usage when Hotplug polls SDCC
2009-04-27   sc      Added support to get physical partition info
2009-04-15   sc      Added sdcc_find_mmc_device
2009-03-09   sc      Added handle support
2009-02-23   sc      Added the TRAN_SPEED values
2009-02-10   vj      OS Abstraction Layer
2009-01-09   rh      Cleanup of cust file dependencies
2008-11-21   vj      Added SDCC_HIGHSPEED_MMC_26MHZ and SDCC_HIGHSPEED_MMC_52MHZ
                     for High Speed card types
2008-09-29   vin     HAL implementation
2008-09-22   sc      Added reliable write from JEDEC Standard No. 84-A43
2008-05-21   sc      Included sdcc_errno.h
03/15/08     sc      Updated SDCC_MMC_DETECT_MAX_RETRIES
12/14/07     sc      Handled MMC card of card size <= 2GB with invalid
                     SEC_COUNT value
12/11/07     sc      Added SDCC_MMC_HCS_NEGOTIATE_OCR for high capacity support
                     of CMD1 argument and defined SDCC_MMC_DETECT_MAX_RETRIES
11/26/07     sc      Replaced the T_MSM7600 flag with 
                     FEATURE_SDCC_MMC_8BIT_BUS_WIDTH_SUPPORT
06/06/07     sc      Updated variable name in bus width setting and added macro
                     for the WIDEBUS mode
02/15/07     sc      Initial version

==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#include SDCC_OSAL_H
#include "sdcc_priv.h"

/*SDCC_EXTRACT_COMMON_BEGIN*/

/*************************************************************/
/*               DEFINITIONS AND CONSTANTS                   */
/*************************************************************/
/* Per section 9.3 in MMC v4.2 Specification, the card shall complete */
/* initialization within 1 second from the first CMD1 to card ready. */
#define SDCC_MMC_DETECT_MAX_BUSY_MS           1000

/* time interval (in ms) between the CMD1 retry */
#define SDCC_MMC_DETECT_BUSY_RETRY_INTERVAL   50

/* SDCC_MMC_DETECT_MAX_RETRIES is derived from the initialization within */
/* 1 second from the first CMD1 to card ready. */
/* That is 1000 milli-second = 20 x 50 milli-seond where 50 milli-second is */
/* wait time between each CMD1 retry. */
/* Refer to section 9.3 in MMC v4.2 Specification */
#define SDCC_MMC_DETECT_MAX_RETRIES \
   (SDCC_MMC_DETECT_MAX_BUSY_MS / SDCC_MMC_DETECT_BUSY_RETRY_INTERVAL)

/* voltage 2.7-3.6 and high capacity support */
/* Refer to section 5.1 in MMC v4.2 Specification */
#define SDCC_MMC_HCS_NEGOTIATE_OCR          0x40FF8000

/* MMC 2GB card size (i.e., 2 * 1024 * 1024 * 1024) */
/* This 2GB card size constant is used to check against the MMC card size */
/* calculated from CSD register.  If this calculated MMC card size is less */
/* than or equal to 2GB, SEC_COUNT from EXT_CSD register is ignored. */
/* Otherwise, SEC_COUNT is used to calculate the MMC card size if SEC_COUNT */
/* is non-zero from the EXT_CSD register. */
#define SDCC_MMC_2GB_CARD_SIZE              0x80000000

/* the device type for MMC */
/* Refer to section 8.2 in JEDEC Standard No. 84-A44 */
#define SDCC_MMC_DEVICE_TYPE_REMOVABLE_CARD 0
#define SDCC_MMC_DEVICE_TYPE_BGA            1
#define SDCC_MMC_DEVICE_TYPE_POP            2

/* start year of manufacturing date in CID register */
/* Refer to section 8.2 in JEDEC Standard No. 84-A43 */
#define SDCC_MMC_CID_MDT_START_YEAR         1997

/* MMC system specification version supported by the card */
/* Refer to section 8.3 in JEDEC Standard No. 84-A44 */
/* spec version 3x denotes version 3.1 - 3.2 - 3.31 */
#define SDCC_MMC_SPEC_VERS_3X               3
/* spec version 4x denotes version 4.0 - 4.1 - 4.2 - 4.3 */
#define SDCC_MMC_SPEC_VERS_4X               4

/* maximum number of pre-defined block count for CMD23 SET_BLOCK_COUNT */
/* Refer to section 7.9.4 in JEDEC Standard No. 84-A43 */
#define SDCC_EMMC_MAX_PREDEFINED_NUM_BLOCKS     65535

/* reliable write request support in CMD23 SET_BLOCK_COUNT */
/* Refer to section 7.9.4 in JEDEC Standard No. 84-A43 */
#define SDCC_EMMC_RELIABLE_WRITE_REQUEST    0x80000000

/* extended CSD revision */
/* revision 1.5 was added in JEDEC Standard No. 84-A44 */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_MMC_EXT_CSD_REV_1_5            5
#define SDCC_MMC_EXT_CSD_REV_1_3            3

/* CMD6 Switch command to set field value in EXT_CSD register */
/* Refer to section 7.9.4 in JEDEC Standard No. 84-A44 */
#define SDCC_MMCPLUS_CHANGE_BUSWIDTH   0x03B70000
#define SDCC_MMCPLUS_ENABLE_HS_TIMING  0x03B90000
#define SDCC_EMMC_SET_BOOT_PARTITION_ENABLE 0x03B30000
#define SDCC_EMMC_SET_PARTITION_ACCESS 0x03B30000
#define SDCC_EMMC_ENABLE_ERASE_GROUP_DEF 0x03AF0000

/* Used by CMD6 to set the US_PWR_WP_EN and B_PWR_WP_EN bits of the  */
/* EXT_CSD BOOT_WP [173] and USER_WP [171] registers. */
/* Bit[24:25] of the CMD6 argument indicates the Access Mode. */
/* Use Set Bits Access Mode (b01) to set only the PWR_WP_EN bit. Other bits */
/* in the registers are not affected */
#define SDCC_EMMC_ENABLE_BOOT_PWR_WP   0x01AD0100
#define SDCC_EMMC_ENABLE_USER_PWR_WP   0x01AB0100

#define SDCC_EMMC_ENABLE_HW_RESET      0x03A20100
#define SDCC_EMMC_SET_GPP1_SIZE        0x038F0000
#define SDCC_EMMC_SET_GPP2_SIZE        0x03920000
#define SDCC_EMMC_SET_GPP3_SIZE        0x03950000
#define SDCC_EMMC_SET_GPP4_SIZE        0x03980000
#define SDCC_EMMC_SET_ENH_SIZE         0x038C0000
#define SDCC_EMMC_SET_ENH_START_ADDR   0x03880000
#define SDCC_EMMC_CFG_BYTE1_OFFSET     0x00010000
#define SDCC_EMMC_CFG_BYTE2_OFFSET     0x00020000
#define SDCC_EMMC_CFG_BYTE3_OFFSET     0x00030000
#define SDCC_EMMC_PARTITION_SET_DONE   0x039B0100
#define SDCC_EMMC_SET_PARTITION_ATTR   0x039C0000
#define SDCC_MMC_SET_DATA_VAL(va)      (((va) << 8) & 0x0000ff00)

/* The following definitions are the physical partitions that can be */
/* accessed. */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_PHY_PARTITION_USER_AREA    0
#define SDCC_EMMC_PHY_PARTITION_BOOT_PART_1  1
#define SDCC_EMMC_PHY_PARTITION_BOOT_PART_2  2
#define SDCC_EMMC_PHY_PARTITION_RPMB         3
#define SDCC_EMMC_PHY_PARTITION_GPP_1        4
#define SDCC_EMMC_PHY_PARTITION_GPP_2        5
#define SDCC_EMMC_PHY_PARTITION_GPP_3        6
#define SDCC_EMMC_PHY_PARTITION_GPP_4        7

/* maximum number of general purpose area partitions */
/* Refer to section 7.2.1 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_MAX_GPP_NUM   4

/* extended CSD slice number for USER_WP, BOOT_WP, PARTITION_CONFIG */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_EXT_CSD_SLICE_USER_WP           171
#define SDCC_EMMC_EXT_CSD_SLICE_BOOT_WP           173
#define SDCC_EMMC_EXT_CSD_SLICE_PARTITION_CONFIG  179

/* bit position for fields in USER_WP */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_BIT_US_PWR_WP_DIS       3
#define SDCC_EMMC_BIT_US_PERM_WP_EN       2

/* bit position for fields in BOOT_WP */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_BIT_BOOT_PWR_WP_DIS       6
#define SDCC_EMMC_BIT_BOOT_PERM_WP_EN       2
#define SDCC_EMMC_BIT_BOOT_PWR_WP_EN        0

/* bit position, number of bits and boot data for fields in PARTITION_CONFIG */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_BIT_BOOT_PARTITION_ENABLE       3
#define SDCC_EMMC_NUM_BITS_BOOT_PARTITION_ENABLE  3
#define SDCC_EMMC_BOOT_DATA_NOT_BOOT_ENABLED      0
#define SDCC_EMMC_BOOT_DATA_BOOT_PART_1_ENABLED   1
#define SDCC_EMMC_BOOT_DATA_BOOT_PART_2_ENABLED   2
#define SDCC_EMMC_BOOT_DATA_USER_AREA_ENABLED     7

/* For cards supporting version 4.0, 4.1 and 4.2 of the JEDEC Standard */
/* No. 84-A43, the TRAN_SPEED value in CSD register shall be 20MHz (0x2A). */
/* For cards supporting version 4.3, the TRAN_SPEED value in CSD register */
/* shall be 26MHz (0x32). */
/* Refer to section 8.3 in JEDEC Standard No. 84-A43 */
#define SDCC_MMC_TRAN_SPEED_20MHZ 0x2A
#define SDCC_MMC_TRAN_SPEED_26MHZ 0x32

/* MMC 4.X compliant card types known from EXT_CSD */
/* Refer to section 5.4 in MMC v4.2 Specification */
#define SDCC_HIGHSPEED_MMC_26MHZ           0x01
#define SDCC_HIGHSPEED_MMC_52MHZ           0x02
#define SDCC_HIGHSPEED_MMC_DDR_52MHZ       0x04
#define SDCC_HIGHSPEED_MMC_DDR_52MHZ_1_2V  0x08

/* The following definitions are used for checking the card status. */
/* They are used in function sdcc_mmc_set_partition_access. */
/* Refer to section 7.12 in JEDEC Standard No. 84-A43 */
#define SDCC_MMC_SWITCH_ERROR_SHFT    7
#define SDCC_MMC_SWITCH_ERROR_BITMASK 0x00000001
#define SDCC_MMC_SWITCH_ERROR         1

/* The following definitions are used for checking the address out of range */
/* error. */
/* Refer to section 7.13 in JEDEC Standard No. 84-A44 */
#define SDCC_MMC_ADDR_OUT_OF_RANGE_SHFT  31
#define SDCC_MMC_ADDR_OUT_OF_RANGE_ERROR 1

/* The following definition indicates if partition programming is completed */
/* Refer to section 7.4 in JEDEC Standard No. 84-A45 */
#define SDCC_MMC_PARTI_SET_COMPLETED_BMSK  1

/*SDCC_EXTRACT_COMMON_END*/

/*************************************************************/
/*                           MACROS                          */
/*************************************************************/
/* The following macro translates the value of SWITCH_ERROR bit specified in */
/* section 7.12 in JEDEC Standard No. 84-A43. */
#define SDCC_XLATE_SWITCH_ERROR(x) \
   (((x) >> SDCC_MMC_SWITCH_ERROR_SHFT) & SDCC_MMC_SWITCH_ERROR_BITMASK)

/* The following macro translates the value of ADDRESS_OUT_OF_RANGE bit */
/* specified in section 7.13 in JEDEC Standard No. 84-A44. */
#define SDCC_XLATE_ADDR_OUT_OF_RANGE(x) \
   ((x) >> SDCC_MMC_ADDR_OUT_OF_RANGE_SHFT)

/* The following macro calculates the partition size in kBytes into number */
/* of sectors. */
#define SDCC_CALC_PARTITION_SIZE_IN_SECTORS(s) \
   (((s) * 1024) / SDCC_DEFAULT_BLK_LENGTH_SIZE)

/* macro to validate the boot partition value */
/* refer to PARTITION_CONFIG in section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_IS_BOOT_PARTITION_VAL_VALID(v) \
   !(((v) > SDCC_EMMC_BOOT_DATA_BOOT_PART_2_ENABLED && \
   (v) < SDCC_EMMC_BOOT_DATA_USER_AREA_ENABLED) || \
   (v) > SDCC_EMMC_BOOT_DATA_USER_AREA_ENABLED)

/* macro to map the physical partition number to the value defined for */
/* BOOT_PARTITION_ENABLE in PARTITION_CONFIG register specified in */
/* section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_MAP_PHY_PART_NUM_TO_BOOT_PART_VAL(n) \
   ((n) == SDCC_EMMC_PHY_PARTITION_USER_AREA ? \
   SDCC_EMMC_BOOT_DATA_USER_AREA_ENABLED : (n))

/* macro to construct the PARTITION_CONFIG value for CMD6 SWITCH */
/* refer to section 8.4 in JEDEC Standard No. 84-A44 */
/* bpe denotes the BOOT_PARTITION_ENABLE value */
/* pa denotes the PARTITION_ACCESS value */
#define SDCC_EMMC_CONSTRUCT_PARTITION_CONFIG_VAL_FOR_SWITCH(bpe, pa) \
   ((((bpe) << SDCC_EMMC_BIT_BOOT_PARTITION_ENABLE) | (pa)) << 8)

/* macro to examine if the queried physical partition number is from the */
/* user area */
#define SDCC_EMMC_IS_FROM_USER_AREA_PART(n) \
   ((n) == SDCC_EMMC_PHY_PARTITION_USER_AREA || \
    (n) == SDCC_EMMC_PHY_PARTITION_GPP_1 || \
    (n) == SDCC_EMMC_PHY_PARTITION_GPP_2 || \
    (n) == SDCC_EMMC_PHY_PARTITION_GPP_3 || \
    (n) == SDCC_EMMC_PHY_PARTITION_GPP_4)

/* macro to examine if the queried physical partition number is from the */
/* boot area */
#define SDCC_EMMC_IS_FROM_BOOT_AREA_PART(n) \
   ((n) == SDCC_EMMC_PHY_PARTITION_BOOT_PART_1 || \
    (n) == SDCC_EMMC_PHY_PARTITION_BOOT_PART_2)

/* macro to extract the specified bit value from a 1-byte register */
/* r denotes the 1-byte register */
/* b denotes the bit position in the register */
/* n denotes number of bits */
/* For example, to extract the bit value of BOOT_PARTITION_ENABLE from */
/* register PARTITION_CONFIG where BOOT_PARTITION_ENABLE is at bit */
/* position [5:3], use the macro as */
/* SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(partition_config, 3, 3) */
/* where partition_config is the register, */
/* the 2nd parameter "3" is the start bit position of [5:3], and */
/* the 3rd parameter "3" is the number of bits in [5:3] */
#define SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(r, b, n) \
   (((r) >> (b)) & ((1 << (n)) - 1))

/******************************************************************/
/*                     TYPES DECLARATIONS                         */
/******************************************************************/
/* The Extended CSD register is according to section 8.4 in */
/* JEDEC Standard No.84-A44. */
typedef struct sdcc_ext_csd_t {
   uint8  s_cmd_set;            /* supported command sets */
   uint8  ini_timeout_ap;       /* 1st initialization time after partitioning */
   uint8  pwr_cl_ddr_52_360;    /* power class for 52MHz, DDR at 3.6V */
   uint8  pwr_cl_ddr_52_195;    /* power class for 52MHz, DDR at 1.95V */
   uint8  min_perf_ddr_w_8_52;  /* minimum write performance for 8bit at */
                                /* 52MHz in DDR mode */
   uint8  min_perf_ddr_r_8_52;  /* minimum read performance for 8bit at */
                                /* 52MHz in DDR mode */
   uint8  trim_mult;            /* TRIM multiplier */
   uint8  sec_feature_support;  /* secure feature support */
   uint8  sec_erase_mult;       /* secure erase multiplier */
   uint8  sec_trim_mult;        /* secure TRIM multiplier */
   uint8  boot_info;            /* boot information */
   uint8  boot_size_multi;      /* boot partition size */
   uint8  acc_size;             /* access size */
   uint8  hc_erase_grp_size;    /* high-capacity erase unit size */
   uint8  erase_timeout_mult;   /* high-capacity erase timeout */
   uint8  rel_wr_sec_c;         /* reliable write sector count */
   uint8  hc_wp_grp_size;       /* high-capacity write protect group size */
   uint8  s_c_vcc;              /* sleep current (VCC) */
   uint8  s_c_vccq;             /* sleep current (VCCQ) */
   uint8  s_a_timeout;          /* sleep/awake timeout */
   uint32 sec_count;            /* sector count */
   uint8  min_perf_w_8_52;      /* min write performance for 8bit @52MHz */
   uint8  min_perf_r_8_52;      /* min read performance for 8bit @52MHz */
   uint8  min_perf_w_8_26_4_52; /* min write performance for 8bit @26MHz */
                                /* or 4bit @52MHz */
   uint8  min_perf_r_8_26_4_52; /* min read performance for 8bit @26MHz */
                                /* or 4bit @52MHz */
   uint8  min_perf_w_4_26;      /* min write performance for 4bit @26MHz */
   uint8  min_perf_r_4_26;      /* min read performance for 4bit @26MHz */
   uint8  pwr_cl_26_360;        /* power class for 26MHz @3.6V */
   uint8  pwr_cl_52_360;        /* power class for 52MHz @3.6V */
   uint8  pwr_cl_26_195;        /* power class for 26MHz @1.95V */
   uint8  pwr_cl_52_195;        /* power class for 52MHz @1.95V */
   uint8  card_type;            /* card type: 1 means 26MHz, 3 means 52MHz */
   uint8  csd_structure;        /* CSD structure version */
   uint8  ext_csd_rev;          /* extended CSD revision */
   uint8  cmd_set;              /* command set */
   uint8  cmd_set_rev;          /* command set revision */
   uint8  power_class;          /* power class */
   uint8  hs_timing;            /* high speed interface timing */
   uint8  bus_width;            /* bus width mode */
   uint8  erased_mem_cont;      /* erased memory content */
   uint8  partition_config;     /* partition configuration */
   uint8  boot_bus_width;       /* boot bus width */
   uint8  erase_group_def;      /* high-density erase group definition */
   uint8  boot_wp;              /* boot area write protection register */
   uint8  user_wp;              /* user area write protection register */
   uint8  fw_config;            /* FW configuration */
   uint8  rpmb_size_mult;       /* RPMB size */
   uint8  rst_n_function;       /* H/W reset function */
   uint8  partitioning_support; /* partitioning support */
   uint32 max_enh_size_mult;    /* max enhanced area size */
   uint8  partitions_attribute; /* partitions attribute */
   uint8  partition_setting_completed; /* partitioning setting */
   uint32 gp_size_mult[SDCC_EMMC_MAX_GPP_NUM]; /* general purpose partition */
                                               /* size */
   uint32 enh_size_mult;        /* enhanced user data area size */
   uint32 enh_start_addr;       /* enhanced user data start address */
   uint8  sec_bad_blk_mgmnt;    /* bad block management mode */
} sdcc_ext_csd_type;

/*********************************************************************/
/*                           PROTOTYPES                              */
/*********************************************************************/
/******************************************************************************
* Name: sdcc_find_mmc_device
*
* Description: 
*    This function looks for the MMC device of the specified driveno.
*
* Parameters:
*    driveno [IN] : a valid drive number between 0 and the total number of card
*                   slots minus one
*
* Returns:
*    card type that indicates if device is MMC or unknown
*
******************************************************************************/
SDCC_CARD_TYPE sdcc_find_mmc_device ( struct sdcc_device *handle );


/******************************************************************************
* Name: sdcc_mmc_switch
*
* Description:
*    This function sends CMD6 SWITCH to the memory device to modify the EXT_CSD
*    registers.  It then checks if switch error occurs.
*
* Arguments:
*    sdcc_pdev  [IN] : pointer to device type
*    switch_arg [IN] : command argument for CMD6 SWITCH
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_switch
(
   sdcc_dev_type *sdcc_pdev,
   uint32 switch_arg
);

/******************************************************************************
* Name: sdcc_config_mmc_bus_width
*
* Description:
*    This function configures the MMC bus width.
*
* Arguments:
*    driveno          [IN] : drive number
*    bus_width        [IN] : MMC bus width
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean sdcc_config_mmc_bus_width( uint32               driveno,
                                   SDCC_MMC_BUSWIDTH   bus_width );

/******************************************************************************
* Name: sdcc_config_mmc_modes_segment
*
* Description:
*    This function programs the modes segment in EXT_CSD register of the
*    MMC/eMMC device.  It gets the EXT_CSD information and sends CMD6 SWITCH
*    command to configure the HS_TIMING and BUS_WIDTH.  It also calculates the
*    card size for the device that is higher than 2GB, boot partition size and
*    write protect group size.
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS sdcc_config_mmc_modes_segment( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_config_mmc_modes_segment_decode
*
* Description:
*    This function decodes the already available data such as EXT_CSD, bus width
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*    raw_ext_csd  [IN]     : pointer to raw EXT CSD
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS 
sdcc_config_mmc_modes_segment_decode
(
   sdcc_dev_type *sdcc_pdev,
   byte *raw_ext_csd
);

/******************************************************************************
* Name: sdcc_get_ext_csd
*
* Description:
*    This function gets the EXT_CSD register as a block of data from the
*    MMCPlus card.  It performs the adtc command with the read operation.
*
* Arguments:
*    sdcc_pdev              [IN] : pointer to device type
*    buff                  [OUT] : pointer to buffer for the incoming data
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_get_ext_csd( sdcc_dev_type   *sdcc_pdev,
                              byte                  *buff );

/******************************************************************************
* Name: sdcc_decode_extCSD
*
* Description:
*    This function is to parse out the Extended CSD data.
*
* Arguments:
*    data          [IN] : pointer to buffer that contains ext_csd data
*    ext_csd      [OUT] : pointer to the structure to save the ext csd data to
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean sdcc_decode_extCSD( const byte          *data,
                            sdcc_ext_csd_type   *ext_csd );

/******************************************************************************
* Name: sdcc_decode_mmc_cid
*
* Description:
*    This function is to parse out the Card Identification (CID) register.
*
* Arguments:
*    raw_cid        [IN] : pointer to raw CID info
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean sdcc_decode_mmc_cid( const uint32   *raw_cid,
                             sdcc_dev_type  *sdcc_pdev );

/******************************************************************************
* Name: sdcc_decode_mmc_mfr_date
*
* Description:
*    This function decodes the manufacturing date as mm/yyyy format.
*
* Arguments:
*    mfr_date       [IN] : raw manufacturing date
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean sdcc_decode_mmc_mfr_date( uint8          mfr_date,
                                  sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_mmc_set_partition_access
*
* Description:
*    This function sets the partition for read and write operation by
*    sending CMD6 SWITCH to the eMMC device.
*
* Arguments:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_set_partition_access( struct sdcc_device *handle );

/******************************************************************************
* Name: sdcc_mmc_get_phy_partition_info
*
* Description: 
*    This function gets the physical partition information on eMMC device.
*
* Parameters:
*    handle                     [IN] : a pointer to the SDCC device that was
*                                      returned from sdcc_handle_open()
*    partition_size_in_sectors [OUT] : physical partition size in sectors
*    phy_partition_type        [OUT] : type of physical partition to indicate
*                                      if partition is the user area, boot
*                                      partition, general purpose partition,
*                                      etc.
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_get_phy_partition_info(
   struct sdcc_device *handle,
   uint32 *partition_size_in_sectors,
   uint32 *phy_partition_type );

/******************************************************************************
* Name: sdcc_mmc_set_pwr_on_write_prot
*
* Description: 
*    This function sets the power-on period write protection to the physical
*    partition specified in parameter handle.  If the physical partition is
*    the user area, the write protection is applied to the specified parameter
*    data_address.  Parameter num_of_blocks should be aligned with the write
*    protect group size specified in write_protect_group_size_in_sectors in
*    structure sdcc_mem_info_type.  If the physical partition is the boot area,
*    the write protection is enabled to the entire boot partition and both boot
*    areas would get write protected.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    data_address  [IN] : start of the sector to get power-on write protected
*    num_of_blocks [IN] : number of blocks to be power-on write protected
*                         (512 bytes per block)
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_set_pwr_on_write_prot(
   struct sdcc_device *handle,
   uint32 data_address,
   uint32 num_of_blocks );

/******************************************************************************
* Name: sdcc_mmc_set_active_bootable_partition
*
* Description: 
*    This function sets the BOOT_PARTITION_ENABLE bits specified in parameter
*    handle.  It clears the PARTITION_ACCESS bits if the current active
*    partition is not the user area.  It then sends the SWITCH command to the
*    eMMC device to set the BOOT_PARTITION_ENABLE bits in the EXT_CSD register.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_set_active_bootable_partition(struct sdcc_device *handle);

/******************************************************************************
* Name: sdcc_mmc_is_phy_partition_bootable
*
* Description: 
*    This function examines if the partition specified in parameter handle is
*    the bootable partition. 
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    TRUE if partition is bootable.  FALSE otherwise.
*
******************************************************************************/
boolean sdcc_mmc_is_phy_partition_bootable(struct sdcc_device *handle);

/******************************************************************************
* Name: sdcc_mmc_config_gpp_enh
*
* Description:
*    This function configures the General Purpose Partition and 
*    enhanced user data area information
*    Note: This routine can only be used once for the life of each
*    emmc chip!
*
* Arguments:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open()
*    desc       [IN] : a pointer to the structure that describes the layout
*                      of the GPP and ENH partition information
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_config_gpp_enh (struct sdcc_device *handle,
                         sdcc_emmc_gpp_enh_desc_type *desc);
#endif /* __SDCC_MMC_UTIL_H */

