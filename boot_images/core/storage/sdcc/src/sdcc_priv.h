#ifndef __SDCC_PRIV_H
#define __SDCC_PRIV_H

/**********************************************************************
 * sdcc_priv.h
 *
 * This file provides SDCC internal definitions.
 *
 * Copyright (c) 2007-15 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_priv.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------
2015-01-28   sb      Fix Klocwork errors
2014-04-23   pa      Modified max sdcc slots supported to 2 on bear family
2013-09-03   bn      Add SDHCi, DDR Support
2013-08-12   bn      Code clean up
2013-05-14   bb      Increasing eMMC/SD Timeout values
2011-08-08   pa      Clock regime DAL migration
2011-09-22   bb      Increase the read timeout for multi threaded environment
2011-09-19   rh      Record the partition setting complete value
2011-09-09   rh      Add ability to report physical partition size
2011-06-28   pa      Added struct member to indicate divided DMA transfer
2011-06-16   pa      Changes to support release of thread local errno
2010-10-05   vj      Added force erase functionality
2010-10-04   vin     Added sruct member to indicate if ISR is currently attached
2010-09-22   rh      Prevent deadlock caused by SDIO interrupt
2010-09-07   yg      Adding changes to support BAM-SYSTEM transfer mode
2010-08-12   rh      Increase the write timeout value for MMC cards
2010-07-21   rh      Add DATA_END interrupt support
2010-07-20   vj      Moved legacy timetick implementation
2010-04-22   rh      Added define for clock ramp up delay
2010-03-02   yg      Added SPS support, removed DMOV support (moved to
                     into sdcc_dep_os)
2010-02-19   yg      Added #define HAL_DRIVENO and changed location of
                     sdcc_dmov_command_s_type
2010-01-06   sp      Added SDCC_CARD_STATUS under extraction tag
2009-10-30   sc      Added feature to set active boot partition
2009-10-23   vj      Added enum SDCC_INIT_MODE to SDCC_CARD_MODE
2009-10-16   sc      Added power-on write protection feature
2009-10-12   sc      Added write_prot_support
2009-09-14   vj      Added DM support at boot & DAL DM support
2009-08-21   sp      Added struct sdcc_io_max_speed_type for SDIO maxclk
2009-09-01   rh      Remove the include of bio.h
2009-08-07   sc      Added SDCC_MEM_ACCESS_CMD_ARG marco
2009-07-16   rcc     Allow customers to override the default "power up" time
2009-07-14   vin     Added macro for DM length check 
2009-06-15   rh      Adding common interface extraction tag
2009-06-18   vj      Reverting the 32KB DM size limitation
2009-05-20   vj      Changed the DM alignment and Temp buffer padding
2009-05-26   vin     Removed redundant SDCC_INIT_MODE
2009-05-15   rh      Fix WM7 compile issue
2009-05-07   vin     Moved data timeout constants from HAL.
2009-05-07   vin     Cleanup - removed unused defines, moved SDIO specific
                     defines to sdcc_sdio_util.h
2009-04-15   vin     Support for HS SDIO
2009-03-09   vj      Boot compilation of the driver
2009-03-30   vin     Updated sdcc_io_type
2009-04-13   rh      Cleanup of unused defines
2009-04-10   rcc     Specify a minimum transfer size to use DM
2009-03-20   sc      Added handle support
2009-03-11   sc      Added macro for shifting RCA
2009-03-10   rh      Fix WM7 compile issue
2009-02-09   vj      OS Abstraction Layer
2009-02-17   vin     Added powerup & supply ramp up time for spec compliance
2009-01-23   rh      Merge WM7 specific code
2009-01-09   rh      Cleanup of cust file dependencies
2008-12-05   sr      HAL Adaptation for WM6.
2008-11-21   vj      Changed clk_freq in sdcc_mem_type to enum type 
                     sdcc_card_freq_type
2008-11-14   sc      Updated reliable_write_sector_count
2008-10-30   sc      Cleaned up file sdcc_api.h
2008-10-23   vj      Added macro for read fifo timeout
2008-10-23   vin     Add state variable to monitor card presence
2008-10-17   vin     HAL implementation
2008-10-13   vin     Added SDCC_SDIO_RESET
2008-09-22   sc      Added reliable write support
2008-08-13   rh      Add timeout when clearing status register
2008-06-25   rcc     Removed WINCE #ifdef around sleep.h to accommodate 8K
2008-06-23   sc      Added dmov_enable and dmov_disable
2008-05-21   sc      Ported raw command API
2008-05-16   sc      Updated the temp data buffer codes
2008-05-15   sc      Changed IS_ALIGNED macro to SDCC_ADDR_ALIGNED_FOR_DM
2008-04-28   vin     Re-assigning SDCC signals to one signal per SD slot
==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#ifdef DPRINTF
#undef DPRINTF
#define DPRINTF(x)
#endif

#define TPRINTF(level, x)

#include SDCC_OSAL_H
#include "sdcc_api.h"
#include "sdcc_debug.h"
#if defined (FEATURE_SDCC_FOR_BOOT)
#else /* FEATURE_SDCC_FOR_BOOT */
#include "timetick.h"
#include "clk.h"
#include "clkregim.h"
#include "sleep.h"
#endif /* FEATURE_SDCC_FOR_BOOT */
#include "HALsdcc.h"
#include "sdcc_bsp.h"
#include "HALsdhci.h"

/*SDCC_EXTRACT_PRIORITY1*/
/*SDCC_EXTRACT_COMMON_BEGIN*/
/*************************************************************/
/*                     DEFINES                               */
/*************************************************************/
#define SDCC_CMD8_RETRIES           3

/* Number of times to attempt to clear the status register                 */
/* before timeout occurs. This number is obtained though an educated       */
/* guess process.  The number of trys depending on the speed difference    */
/* between the MBUS and PBUS speed.  The worse case will be when the       */
/* MBUS is slow and PBUS is fast.  Assume a MBUS speed of 150kHz and       */
/* PBUS speed of 100MHz, it will take 666 PBUS cycle for the reset         */
/* to sync up.  A while loop used to reset the status register is about    */
/* 10 instruction in length, so it would normall take 70 retries to clear  */
/* the status register.  Since we normally would not want the timeout      */
/* to occur very often, it is reasonable to multiply this number by a      */
/* large constant, say 10.                                                 */
#define SDCC_CLR_STATUS_RETRIES     (1000)

/* Per section 6.4.1 in SD2.0 System Specification, */
/* time out value for initialization process is 1 second. */
#define SDCC_SD_DETECT_MAX_BUSY_MS           1000
/* time interval (in ms) between the ACMD41 retry */
/* Refer to section 3.6 of SD Specifications Part A2 SD Host Controller */
/* Standard Specifrication Version 2.00 */
#define SDCC_SD_DETECT_BUSY_RETRY_INTERVAL   50

/* Time (in ms) to wait until the card can start accepting commands */
/* Refer to section 6.4 of SD Specifications Part 1 Physical Layer */
/* Specification Version 2.00 */
#define SDCC_SUPPLY_RAMP_UP_TIME_MS   1

/* Time (in ms) to wait after changing the clock frequency before */
/* clock is stable.  This value is currently an conservative guess. */
#define  SDCC_CLK_RAMP_TIME_MS        1

/* Check if driveno is good */
#define SDCC_DRIVENO_IS_VALID(x) ((x) < SDCC_MAX_NUM_CONTROLLER)

/* translate the physical partition number */
#define SDCC_XLATE_PARTITION_NUM(n) \
   ((SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE == (n)) ? 0 : (n))

/* SDCC_SD_DETECT_MAX_RETRIES is derived from the the maximum time out value */
/* of the initialization process.  The time after the end of the first */
/* ACMD41 to card ready is 1 second. */
/* That is 1000 milli-second = 20 x 50 milli-seond where 50 milli-second is */
/* wait time between each ACMD41 retry. */
/* Refer to section 6.4.1 in SD2.0 System Specification */
#define SDCC_SD_DETECT_MAX_RETRIES \
   (SDCC_SD_DETECT_MAX_BUSY_MS / SDCC_SD_DETECT_BUSY_RETRY_INTERVAL)

#define SDCC_STATUS_POLL_MAX        0x80000
#define SDCC_PROG_DONE_MAX          0x80000
#define SDCC_DEFAULT_BLK_LENGTH_SIZE 512

#define SDCC_CARD_ERASE_FAIL        0xFDFF8000   /* erase failure mask */
#define SDCC_NEGOTIATE_OCR          0x00FF8000   /* voltage 2.7-3.6 */

#define SDCC_CARD_STATUS_BMSK       0x0000000F
#define SDCC_CARD_STATUS_SHFT       9

#define SDCC_CARD_STATUS_BLKLEN_ERR 0x20000000

/* number of bits to be shifted for RCA in CMD13 */
/* Refer to section 4.7.4 in SD2.0 Spec. */
#define SDCC_SEND_STATUS_RCA_SHFT   16
#define SDCC_ARG_RCA(rca) ((uint32)(rca) << SDCC_SEND_STATUS_RCA_SHFT)

/* The following macro is used for the memory access commands such as */
/* CMD17/18, CMD24/25, CMD28.  It examines the card type and returns the */
/* command argument according to the two notes below. */
/* For media <= 2GB card size, the data address is in 32bit byte address. */
/* For media > 2GB, the data address is in a 32bit sector (512Byte) address. */
/* Refer to section 7.10.4 in JEDEC Standard No. 84-A44 or */
/* section 4.3.14 in SD2.0 System Specification */
/* c represents the card type. */
/* a represents the data address. */
/* bl represents the block length. */
#define SDCC_MEM_ACCESS_CMD_ARG(c, a, bl) \
   (((c) == SDCC_CARD_SDHC || (c) == SDCC_CARD_MMCHC) ? (a) : ((a) * (bl)))

/* The following definitions are used to examine if a write violation */
/* occurs.  That is if there is an attempt to program a write protected */
/* block. */
/* Refer to section 7.13 in JEDEC Standard No. 84-A44 */
#define SDCC_WP_VIOLATION_SHFT    26
#define SDCC_WP_VIOLATION_BITMASK 0x00000001
#define SDCC_WP_VIOLATION_ERROR   1

/* The following macro translates the value of WP_VIOLATION bit specified in */
/* section 7.13 in JEDEC Standard No. 84-A44. */
#define SDCC_XLATE_WP_VIOLATION(x) \
   (((x) >> SDCC_WP_VIOLATION_SHFT) & SDCC_WP_VIOLATION_BITMASK)

#define SDCC_DATA_READ              0x0UL
#define SDCC_DATA_WRITE             0x1UL

/* There is a very small chance that between 'DM setup
   and start read from SD card', the CPU can get busy
   doing other high priority things, the HW timer started initially can
   timeout, and subsequently the transfer will not proceed. This is observed
   in few customer setups. We’ve never seen this situation before in L4
   targets, but this was noticed once in Windows Phone, CR 255489 . */
#define SDCC_READ_TIMEOUT_MS        5000
#define SDCC_WRITE_TIMEOUT_MS       400
/* MMC card can have a much longer timeout */
#define SDCC_MMC_WRITE_TIMEOUT_MS   2000

#define SDCC_CMD_ARGU_NULL          0x0

/* MMC partition isuse mask bit-wise flag */
#define SDCC_BMSK_USER_PARTITION    0x01
#define SDCC_BMSK_BOOT1_PARTITION   0x02
#define SDCC_BMSK_BOOT2_PARTITION   0x04
#define SDCC_BMSK_RPMB_PARTITION    0x08
#define SDCC_BMSK_GP1_PARTITION     0x10
#define SDCC_BMSK_GP2_PARTITION     0x20
#define SDCC_BMSK_GP3_PARTITION     0x40
#define SDCC_BMSK_GP4_PARTITION     0x80
#define SDCC_XLATE_PARTITION_ID_TO_MASK(x)     (0x01 << x)

/* The following definitions are used for checking the card status. */
/* They are used in function sdcc_poll_card_status. */
/* Refer to section 7.12 in JEDEC Standard No. 84-A43 */
#define SDCC_MEM_CURRENT_STATE_SHFT         9
#define SDCC_MEM_CURRENT_STATE_BITMASK      0x0000000F
#define SDCC_MEM_READY_FOR_DATA_SHFT        8
#define SDCC_MEM_READY_FOR_DATA_BITMASK     0x00000001
#define SDCC_MEM_READY_FOR_DATA             1
#define SDCC_POLL_CARD_MAX                  0xF000

/* The following two macros translate the value of the CURRENT_STATE bits */
/* and READY_FOR_DATA bit specified in section 7.12 in JEDEC Standard */
/* No. 84-A43 */
#define SDCC_XLATE_CARD_CURRENT_STATE(x) \
   (((x) >> SDCC_MEM_CURRENT_STATE_SHFT) & SDCC_MEM_CURRENT_STATE_BITMASK)
#define SDCC_XLATE_READY_FOR_DATA(x) \
   (((x) >> SDCC_MEM_READY_FOR_DATA_SHFT) & SDCC_MEM_READY_FOR_DATA_BITMASK)

/*************************************************************/
/*             SDHC SPECIFIC DEFINES                         */
/*************************************************************/
/* CSD Version 2.0 for High Capacity SD memory card */
#define SDCC_CSD_TAAC        0x0E /* data read access-time is 1ms */
#define SDCC_CSD_NSAC        0x00 /* data read access-time in CLK cycles */
#define SDCC_CSD_READ_BL_LEN 0x9 /* max. read data block length is 512 bytes */

/* SDCC_TEMP_DATA_BUFF_LENGTH is defined for targets that have data mover */
/* restrictions such as 32 or 128 alignment and 1-to-1 mapping. */
/* This buffer length is for the statically allocated temporary data buffer, */
/* sdcc_temp_data_buff_array, when the memory address of the SDCC client's */
/* data buffer does not comply with the data mover requirement.  64KB plus */
/* 128 padding is chosen for the optimal data length. */
#define SDCC_TEMP_DATA_BUFF_LENGTH (64 * 1024 + 128)

#define SDCC_DEVICE_RDY             0x80000000   /* card is ready */
#define SDCC_MEM_BLK_MODE           0x1
#define SDCC_MEM_BYTE_MODE          0x0

#define SDCC_POLL_CARD_MAX          0xF000

/*************************************************************/
/*             CLKREGIM SPECIFIC DEFINES                         */
/*************************************************************/
#define SDCC_DEFAULT_CLK_KHZ           50000

/*************************************************************/
/*             ENUMERATIONS                                  */
/*************************************************************/
/* Standard commands supported by the Multimedia Card */
enum SD_CMD
{
   SD_CMD0_GO_IDLE_STATE = 0,
   SD_CMD1_SEND_OP_COND,
   SD_CMD2_ALL_SEND_CID,
   SD_CMD3_SET_RELATIVE_ADDR,
   SD_CMD4_SET_DSR,
   SD_CMD5_IO_SEND_OP_COND,
   SD_CMD6_SWITCH_FUNC,                         /* SD CMD6 adtc cmd type */
   SD_CMD6_SWITCH = SD_CMD6_SWITCH_FUNC,        /* MMC CMD6 ac cmd type */
   SD_CMD7_SELECT_CARD,
   SD_CMD8_SEND_IF_COND,                        /* SD CMD8 bcr cmd type */
   SD_CMD8_SEND_EXT_CSD = SD_CMD8_SEND_IF_COND, /* MMC CMD8 adtc cmd type */
   SD_CMD9_SEND_CSD,
   SD_CMD10_SEND_CID,
   SD_CMD11_READ_DAT_UNTIL_STOP,
   SD_CMD12_STOP_TRANSMISSION,
   SD_CMD13_SEND_STATUS,
   SD_CMD14,
   SD_CMD15_GO_INACTIVE_STATE,
   SD_CMD16_SET_BLOCKLEN,
   SD_CMD17_READ_BLOCK,
   SD_CMD18_READ_MULTIPLE_BLOCK,
   SD_CMD19,
   SD_CMD20_WRITE_DAT_UNTIL_STOP,
   SD_CMD21,
   SD_CMD22,
   SD_CMD23_SET_BLOCK_COUNT, /* eMMC command index */
   SD_CMD24_WRITE_BLOCK,
   SD_CMD25_WRITE_MULTIPLE_BLOCK,
   SD_CMD26,
   SD_CMD27_PROGRAM_CSD,
   SD_CMD28_SET_WRITE_PROT,
   SD_CMD29_CLR_WRITE_PROT,
   SD_CMD30_SEND_WRITE_PROT,
   SD_CMD31,
   SD_CMD32_TAG_SECTOR_START,
   SD_CMD33_TAG_SECTOR_END,
   SD_CMD34_READ_SEC_CMD = 34,     /* SD Mc-EX specific READ_SEC_CMD */
   SD_CMD34_UNTAG_SECTOR = 34,     /* MMC command index */
   SD_CMD35_WRITE_SEC_CMD = 35,         /* SD Mc-EX specific WRITE_SEC_CMD */
   SD_CMD35_TAG_ERASE_GROUP_START = 35, /* MMC command index with ac cmd type */
   SD_CMD36_SEND_PSI = 36,            /* SD Mc-EX specific SEND_PSI */
   SD_CMD36_TAG_ERASE_GROUP_END = 36, /* MMC command index with ac cmd type */
   SD_CMD37_CONTROL_TRM = 37,       /* SD Mc-EX specific CONTROL_TRM */
   SD_CMD37_UNTAG_ERASE_GROUP = 37, /* MMC command index */
   SD_CMD38_ERASE,
   SD_CMD39_FAST_IO,
   SD_CMD40_GO_IRQ_STATE,
   SD_CMD41,
   SD_CMD42_LOCK_UNLOCK,
   SD_CMD43_SELECT_PARTITION,         /* eSD SELECT_PARTITION */
   SD_CMD50_DIRECT_SECURE_READ = 50,  /* SD Mc-EX specific DIRECT_SECURE_READ */
   SD_CMD52_IO_RW_DIRECT = 52,
   SD_CMD53_IO_RW_EXTENDED,
   SD_CMD55_APP_CMD = 55,
   SD_CMD56_GEN_CMD,
   SD_CMD57_DIRECT_SECURE_WRITE = 57 /* SD Mc-EX specific DIRECT_SECURE_WRITE */
};

/* Application-specific commands supported by all SD cards */
enum SD_ACMD
{
   SD_ACMD6_SET_BUS_WIDTH=6,
   SD_ACMD13_SD_STATUS=13,
   SD_ACMD18_SECURE_READ_MULTI_BLOCK=18,
   SD_ACMD22_SEND_NUM_WR_BLOCKS=22,
   SD_ACMD23_SET_WR_BLK_ERASE_COUNT=23,
   SD_ACMD25_SECURE_WRITE_MULTI_BLOCK=25,
   SD_ACMD26_SECURE_WRITE_MKB=26,
   SD_ACMD38_SECURE_ERASE=38,
   SD_ACMD41_SD_APP_OP_COND=41,
   SD_ACMD42_SET_CLR_CARD_DETECT=42,
   SD_ACMD43_GET_MKB=43,
   SD_ACMD44_GET_MID=44,
   SD_ACMD45_SET_CER_RN1=45,
   SD_ACMD46_GET_CER_RN2=46,
   SD_ACMD47_SET_CER_RES2=47,
   SD_ACMD48_GET_CER_RES1=48,
   SD_ACMD49_CHANGE_SECURE_AREA=49,
   SD_ACMD51_SEND_SCR=51
};

/* On-card states */
typedef enum sdcc_card_state_t
{
   SDCC_CARD_IDLE = 0,
   SDCC_CARD_READY,
   SDCC_CARD_IDENT,
   SDCC_CARD_STBY,
   SDCC_CARD_TRAN,
   SDCC_CARD_DATA,
   SDCC_CARD_RCV,
   SDCC_CARD_PRG,
   SDCC_CARD_DIS,
   SDCC_CARD_IGNORE         /*dummy entry*/
} SDCC_CARD_STATUS;

/* SDCC response types */
typedef enum sdcc_card_resp_t
{
   SDCC_RESP_NONE   = 0x00,
   SDCC_RESP_SHORT  = 0x01,
   SDCC_RESP_LONG   = 0x04
} SDCC_CARD_RESP;

/* SDCC mode types */
typedef enum sdcc_card_mode_t
{
   SDCC_IDENTIFICATION_MODE = 0,
   SDCC_DATA_TRANSFER_MODE,
   SDCC_READ_TRANSFER_MODE,
   SDCC_WRITE_TRANSFER_MODE,
   SDCC_HS_TRANSFER_MODE,
   SDCC_INIT_MODE,
   SDCC_DATA_TRANSFER_DDR_MODE
} SDCC_CARD_MODE;
/*SDCC_EXTRACT_COMMON_END*/

/* On-card states */
typedef enum sdcc_host_state_t
{
   SDCC_HOST_IDLE = 0,
   SDCC_HOST_IDENT,
   SDCC_HOST_TRAN
} SDCC_HOST_STATE;

/* SDCC frequency types */
typedef enum
{
   SDCC_SD_DEFAULT_FREQUENCY = 0, /* Frequency <= 25MHz for SD */
   SDCC_SD_HIGH_FREQUENCY,        /* Frequency > 25MHz and <= 50MHz for SD */
   SDCC_MMC_DEFAULT_FREQUENCY,    /* Frequency <= 20MHz for MMC */
   SDCC_MMC_HIGH_FREQUENCY_26MHZ, /* MMC 4.X cards that support upto 26MHz */
   SDCC_MMC_HIGH_FREQUENCY_52MHZ  /* MMC 4.x cards that support upto 52MHz */
} sdcc_card_freq_type;

/******************************************************************/
/*                            TYPES                               */
/******************************************************************/

typedef uint32            sdcc_sleep_handle_type;

/* Flags used by the DMA data structure */
#define SDCC_DIR_IN               (1 << 0)
#define SDCC_DIR_OUT              (1 << 1)

typedef struct sdcc_dma_t
{
   void *hdma;            /* DMA internal data structure */
   struct sdcc_slot_data *pslot;
   int (*setup_dma) (struct sdcc_slot_data *pslot, uint32 paddr, 
                       uint32 len, uint32 flag);
   int (*done_dma) (struct sdcc_slot_data *pslot, uint32 flag);
} sdcc_dma_type;

typedef struct sdcc_csd_t
{
   uint8  csd_structure;
   uint8  spec_vers;
   uint8  taac;
   uint8  nsac;
   uint8  tran_speed;
   uint16 ccc;
   uint8  read_bl_len;
   uint8  read_bl_partial;
   uint8  write_blk_misalign;
   uint8  read_blk_misalign;
   uint8  dsr_imp;
   uint32 c_size;
   uint8  vdd_r_curr_min;
   uint8  vdd_r_curr_max;
   uint8  vdd_w_curr_min;
   uint8  vdd_w_curr_max;
   uint8  c_size_mult;
   union {
      struct { /* MMC system specification version 3.1 */
         uint8  erase_grp_size;
         uint8  erase_grp_mult;
      } v31;
      struct { /* MMC system specification version 2.2 */
         uint8  sector_size;
         uint8  erase_grp_size;
      } v22;
   } erase;
   uint8  wp_grp_size;
   uint8  wp_grp_enable;
   uint8  default_ecc;
   uint8  r2w_factor;
   uint8  write_bl_len;
   uint8  write_bl_partial;
   uint8  file_format_grp;
   uint8  copy;
   uint8  perm_write_protect;
   uint8  tmp_write_protect;
   uint8  file_format;
   uint8  crc;
} sdcc_csd_type;

typedef struct sdcc_cmd_t {
   uint32              cmd;
   uint32              cmd_arg;
   SDCC_CARD_RESP      resp_type;
   uint32              resp[4];
   uint32              prog_scan;
   uint32              status;
   uint32              flags;
} sdcc_cmd_type;

typedef struct {
   /* memory device information such as card type, card size (in sectors) */
   /* block length, speed class and card identification information */
   sdcc_mem_info_type  mem_info;
   uint32              block_mode;
   /* For eMMC only: indicates the device type -- removable card or */
   /* embedded BGA */
   uint8               mmc_device_type;
   uint8               spec_vers;   /* For MMC only: MMC System Spec version */
   boolean             write_prot_support; /* indicates if write protection is */
                                           /* supported on the embedded device */
   sdcc_card_freq_type clk_freq;    /* freq card supported */
   uint8               speed_class; /* card's speed class */
                                    /* e.g. SD has speed class 0, 2, 4 and 6 */
   boolean             partition_support; /* indicate if partition is supported */
   uint32              partition_boot_size;      /* boot partition size in kilobytes */
   uint32              partition_gp_size[4];     /* GPP  partition size in sectors */
   uint8               boot_partition_enable_val; /* indicate which partition */
                                                  /* is bootable */
   uint32              partition_inuse_flag;     /* Bit OR mask to indicate partition use */
   uint8               partition_conf_done;      /* Partition setting done value in extCSD */
} sdcc_mem_type;

/* Name the sdcc_dev structure sdcc_card_info due to similarly named 
 * structure sdcc_device */
typedef struct sdcc_card_info {
   uint32                 driveno;            /* controller ID    */
   uint32                 active_partition_num; /* current partition region in use */
   SDCC_CARD_TYPE         card_type;          /* card type        */
   SDCC_HOST_STATE        host_state;         /* controller state */
   uint16                 rca;
   boolean                prog_scan;

   SDCC_STATUS            errno;              /* only for debugging */
   uint32                 status;             /* cntr status register */

   sdcc_mem_type          mem;                /* info pertain to memory device */
   uint32                 clk_freq_in_khz;    /* current SDCC clock freq in KHz */
   uint32                 cache_align_sz;     /* cache alignment size */
   boolean                sdhci_mode;           /* SDHCi Mode enabled */
   boolean                sdhci_ddr_support;  /* SDHCi DDR support */
   boolean                sdhci_adma_enable;  /* SDHCi ADMA Enable */
   boolean                enable_dma;
   struct sdcc_slot_data  *pslot;
} sdcc_dev_type;

typedef struct sdcc_slot_data
{
   uint32                 driveno;          /* SDCC controller ID    */
   boolean                drive_in_use;     /* current drive in use  */
   sdcc_sleep_handle_type sleep_handle;     /* sleep handle for this controller */
   sdcc_dev_type          dev;              /* devices attached to the drive */
   boolean                slot_state_changed; /* Set if card is inserted or removed */
   sdcc_dma_type          dma_data;         /* Data structure for DMA transfer */
   struct HAL_sdhci_Capabilities capabilities;
} sdcc_slot_type;

typedef struct _sdcc_mini_boot_device_info
{
   uint32                           driveno;   /**< drive number */
   SDCC_CARD_TYPE                   card_type; /**< card type SD or MMC */
   uint16                           rca;       /**< relative card address */
} sdcc_mini_boot_device_info_type;

/* bit-wise flags indicating transfer type */
#define SDCC_TRANSFER_FLAG_IS_CMD_ONLY        0x0 
#define SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER  0x1
#define SDCC_TRANSFER_FLAG_IS_READ_TRANSFER   0x2
#define SDCC_TRANSFER_FLAG_IS_APP_COMMAND     0x4

#define SDCC_IS_READ_TRANSFER(flags)                             \
   (flags & SDCC_TRANSFER_FLAG_IS_READ_TRANSFER) 
#define SDCC_IS_WRITE_TRANSFER(flags)                            \
   (flags & SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER)
#define SDCC_DATA_DIRECTION(flags)                               \
   (SDCC_IS_READ_TRANSFER(flags) ? SDCC_DATA_READ : SDCC_DATA_WRITE)
#define SDCC_IS_APP_COMMAND(flags)                               \
   (flags & SDCC_TRANSFER_FLAG_IS_APP_COMMAND)

typedef enum sdcc_mmc_buswidth_t
{
   SDCC_MMC_BUSWIDTH_1BIT     = 0,
   SDCC_MMC_BUSWIDTH_4BIT     = 1,
   SDCC_MMC_BUSWIDTH_8BIT     = 2,
   SDCC_MMC_BUSWIDTH_DDR_4BIT = 5,
   SDCC_MMC_BUSWIDTH_DDR_8BIT = 6  
} SDCC_MMC_BUSWIDTH;

#endif /* ifdef __SDCC_PRIV_H */

