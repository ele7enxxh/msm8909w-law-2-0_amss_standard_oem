#ifndef BOOT_PBL_V1_H
#define BOOT_PBL_V1_H
/*=============================================================================
                            
                       Boot PBL Version 2 Header File

GENERAL DESCRIPTION
  This file access to routines and data exported by the PBL to other boot
  loaders.
  
Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_pbl_v2.h#1 $

**   Note !! **
boot_pbl_v1.h should be in sync with BOTH  pbl files **
boot_headers.h  And  boot_flash.h 
**
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/28/14   ck      Added boot_pbl_log_milestones
05/28/14   ck      Added PBL_TIMESTAMP_TIMER_FREQ_HZ
05/21/14   ck      Updated boot_flash_type to match PBL
05/05/14   ck      Added PBL timestamp type and entry in boot_pbl_shared_data_type
04/01/14   ck      Removed pbl_secboot_verified_info_type, pbl_secboot_ftbl_type
                   and pbl_secboot_hw_ftbl_type as Bear PBL now uses common secboot lib.
02/24/13   jz      Updated for Bear support on MDM
08/26/13   jz      Added UFS flash type
08/15/13   kedara  Update bootrom shared data struct defines.
08/06/13   aus     For secboot verified info, define a local definition of
                   pbl_secboot_verified_info
05/07/13   dh      For secboot function tables, define a local definition 
                   of pbl_secboot_ftbl_type pbl_secboot_hw_ftbl_type 
02/03/12   kedara      Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "secboot_hw.h"
#include "secboot.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
                      BOOT CLK SPEED
===========================================================================*/
/* Clock speed definition structure */
/*
 * pbl_clock_sdcc_freq_type
 *
 */
typedef enum
{
  PBL_CLOCK_SDCC_FREQ_400KHZ,
  PBL_CLOCK_SDCC_FREQ_19MHZ,
  PBL_CLOCK_SDCC_FREQ_50MHZ,
  
  PBL_CLOCK_SDCC_FREQ_TOTAL,
  PBL_CLOCK_SDCC_FREQ_FORCE32BITS = 0x7FFFFFFF
} pbl_clock_sdcc_freq_type;

/*===========================================================================

                      BOOT ERRORS

===========================================================================*/

/* ERROR_CODE_DESCRIPTION:
   Error code definitions for reporting errors in PBL */
typedef enum
{
  PBL_NO_ERR                        = 0x00,
  PBL_FLASH_DET_OR_RW_ERR           = 0x01,
  PBL_FLASH_TIMEOUT_ERR             = 0x02,
  PBL_ASSERT_ERR                    = 0x03,
  PBL_LOAD_RPM_SBL_ERR              = 0x04,
  PBL_RPM_SBL_FORMAT_ERR            = 0x05,
  PBL_FLASH_SPEC_VER_ERR            = 0x06,
  PBL_CERT_CHAIN_VERIFY_ERR         = 0x07,
  PBL_SIGNATURE_VERIFY_ERR          = 0x08,
  PBL_MISC_OR_EXCEPTION_ABORT_ERR   = 0x09,
  PBL_LOAD_DLOAD_ERR                = 0x0A,
  PBL_BOOT_SELECT_OPTION_ERR        = 0x0B,
  PBL_SDCC_SPEED_SELECT_ERR         = 0x0C,
  PBL_OUT_OF_BOUND_ERR              = 0x0D,
  PBL_NULL_PTR_ERR                  = 0x0E, /* null pointer error */
  PBL_ROLLBACK_ERR                  = 0x0F, /* Denotes older secure image is being used */
  PBL_OEM_NUM_ROOT_CERTS_ERR        = 0x10, /* Denotes the number of OEM root certs were invalid */
  PBL_OEM_ROOT_CERT_SEL_ERR         = 0x11, /* Denotes the OEM root cert selected is invalid */
  PBL_INVALID_IMAGE_TYPE_ERR        = 0x12, /* invalid image being authenticated */
  PBL_INVALID_PMIC_ERR              = 0x13, /* no PMIC, or not an expected Qualcomm PMIC connected  */
  PBL_PMIC_FLCB_ERR                 = 0x14, /* Error during fast low current boot sequence that leads
                                               to PMIC power down MSM.  */
  PBL_USB_ENUM_ERR                  = 0x15, /* USB enumeration failed  */
  PBL_SAHARA_ERR                    = 0x16,
  PBL_WDOG_RESET_DEBUG              = 0x17,
  PBL_STACK_CPY_ERR                 = 0x18,
  MAX_ERR_CODE                      = 0x7FFFFFFF /* To ensure it's 32 bits wide */
} pbl_err_code_enum_type;

/* Error log structure to store constant data describing error location */
typedef struct boot_pbl_error_log_location_type
{
  uint32                   line_num;
  char                    *filename;
  pbl_err_code_enum_type   err_code;
} boot_pbl_error_log_location_type;

/* Typedef of a function pointer that receives PBL errors */
typedef void (*boot_pbl_error_handler_type)
(
  const char* filename_ptr,         /* File this function was called from -
                                      defined by __FILE__ */
  uint32      line,                 /* Line number this function was called
                                      from - defined by  __LINE__ */
  pbl_err_code_enum_type  err_code, /* Error code to indicate which error
                                       by toggling UART_DP_TX_DATA pin */
  uint32  err_log                   /* Error log to help debug */

);

/* Typedef of a function pointer that logs non-fatal PBL errors */
typedef boolean (*boot_pbl_error_add_log_type)
( 
  const boot_pbl_error_log_location_type *log, /* struct providing error  
                                                  location                */
  uint32 d1,                                   /* data1 to be saved       */
  uint32 d2,                                   /* data2 to be saved       */
  uint32 d3                                    /* data3 to be saved       */
);


/*===========================================================================

                      BOOT FLASH

===========================================================================*/
/* Define the FLASH types. */
typedef enum
{
  NO_FLASH        = 0,
  NOR_FLASH       = 1,
  NAND_FLASH      = 2,
  ONENAND_FLASH   = 3,
  SDC_FLASH       = 4,
  MMC_FLASH       = 5,
  SPI_FLASH       = 6,
  PCIE_FLASH      = 7,
  UFS_FLASH       = 8,
  RESERVED_1_FLASH= 9,
  USB_FLASH       =10,
  SIZE_FLASH_TYPE = 0x7FFFFFFF    /* Force 4 byte alignment */
} boot_flash_type;

/*---------------------------------------------------------------------------
  Define the types of flash controllers that may be in use
---------------------------------------------------------------------------*/
typedef enum
{
  NO_CTRL,                          /* Invalid controller     */
  NOR_CTRL,                         /* NOR controller         */
  NAND_CTRL,                        /* NAND controller        */
  SFLASH_CTRL,                      /* Sync flash controller          */
  SDC_CTRL,                         /* Secure Digital Card controller */
  SPI_CTRL,                         /* SPI controller         */
  UFS_CTRL,                         /* UFS controller         */
  SIZE_FLASH_CTRL_TYPE = 0x7FFFFFFF /* Force 4 byte alignment */
} boot_flash_ctrl_type;

/*---------------------------------------------------------------------------
  Define the types of ECC Mode being used by NAND Controller
---------------------------------------------------------------------------*/
typedef enum
{
  ECC_4_BIT_MODE         = 0,
  ECC_6_BIT_MODE,
  ECC_8_BIT_MODE,
  ECC_RESERVED_BIT_MODE_0,
  ECC_NOT_ENABLED,
  ECC_BIT_MODE_ENUM_COUNT,
  EBM_SIZE_ENUM          = 0x7FFFFFFF          /* Size enum to 32 bits. */
} boot_ecc_bit_mode_enum_type;

/*---------------------------------------------------------------------------
  Define the types of Boot Option is being used.
---------------------------------------------------------------------------*/
typedef enum
{
  BOOT_DEFAULT_OPTION = 0,    /* 0 NAND->HSUSB2.0 */
  BOOT_PCIe_OPTION,           /* 1 */ 
  BOOT_USB_OPTION,            /* 2 */
  BOOT_HSIC_OPTION,           /* 3 */
  BOOT_SELECT_OPTION_COUNT,   /* 4 */
  BOOT_OPTION_SIZE_ENUM = 0x7FFFFFFF /* Size enum to 32 bits. */ 

} boot_select_option_enum_type;

/*---------------------------------------------------------------------------
  NAND Flash shared data
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                      page_size_bytes;
  uint32                      page_size_shift;
  uint32                      block_size_pages;
  uint32                      block_size_shift;
  boot_ecc_bit_mode_enum_type ecc_mode;
} boot_flash_shared_nand_dev_type;

/*---------------------------------------------------------------------------
  SDC Flash shared data
---------------------------------------------------------------------------*/
/* Card Types  */
typedef enum
{
  SDC_CARD_UNKNOWN = 0,
  SDC_CARD_SD      = 1,
  SDC_CARD_MMC     = 2,
  SDC_CARD_SDHC    = 5,
  SDC_CARD_MMCHC   = 6,  /* MMC card with higher than a density of 2GB */
  SDC_CARD_TYPE_SIZE = 0x7FFFFFFF
} boot_flash_sdc_card_type;

/* On-card states */
typedef enum
{ 
  SDC_CARD_IDLE = 0,
  SDC_CARD_READY,
  SDC_CARD_IDENT,
  SDC_CARD_STBY,
  SDC_CARD_TRAN,
  SDC_CARD_DATA,
  SDC_CARD_RCV,
  SDC_CARD_PRG,
  SDC_CARD_DIS,
  SDC_CARD_BTST,
  SDC_CARD_IGNORE = 0xFF,         /*dummy entry*/
  SDC_CARD_STATE_SIZE = 0x7FFFFFFF
} boot_flash_sdc_card_state_type;

/* Memory access modes */
typedef enum
{
  SDC_MEM_BYTE_MODE,
  SDC_MEM_BLK_MODE,
  SDC_MEM_MODE_SIZE = 0x7FFFFFFF
} boot_flash_sdc_mem_mode_type;


/* MMC bus width */
typedef enum
{
  SDC_MMC_BUSWIDTH_1BIT = 0,
  SDC_MMC_BUSWIDTH_4BIT = 1,
  SDC_MMC_BUSWIDTH_8BIT = 2,
  SDC_MMC_BUSWIDTH_COUNT,
  SDC_MMC_BUSWIDTH_SIZE = 0x7FFFFFFF
} pbl_sdc_mmc_buswidth_type;

/* SDC Boot Port */
/* !!!Warning!!! order of this port numbers should match with
   sdcc_reg[] array defined in sdcc driver files. sdcc_reg array
   defines base address for particular PORT and if below order
   doesn't match, you could see issues !!! */
typedef enum
{
  SDC_PORT1,
  SDC_PORT2,
  SDC_PORT3,
  SDC_PORT4,
  SDC_PORT_INVALID,
  SDC_PORT_SIZE = 0x7FFFFFFF
} boot_flash_sdc_port_type;

/* SDC memory information */
typedef struct
{
   uint32                        block_len;       /* Block size (aka the
                                                     sector size, in bytes) */
   uint32                        block_len_shft;  /* Block length shift bits*/
   boot_flash_sdc_mem_mode_type  block_mode;      /* transfer blocks or bytes */
   uint32                        spec_vers;       /* For MMC only: MMC System
                                                     Spec version */
   uint32                        phy_parti_size;  /* number of blocks */
} boot_flash_sdc_mem_type;

/* Main SDC shared data structure */
typedef struct
{
   boot_flash_sdc_mem_type         mem;         /* info pertaining to memory device */
   boot_flash_sdc_card_type        card_type;   /* card type  */
   boot_flash_sdc_card_state_type  card_state;  /* card state */
   uint32                          rca;         /* the card's relative address */
   pbl_clock_sdcc_freq_type        mclk;        /* current host clock speed */
   boot_flash_sdc_port_type        port;        /* Port/Slot the device was detected */
   uint32                          boot_cfg;    /* Specify boot parition */
   pbl_sdc_mmc_buswidth_type       data_width;  /* data_width ( 1/4/8 bit) */ 
   boolean                         is_gpt_parti;
   boolean                         is_primary_table;
   uint64                          starting_lba;/* starting LBA of SBL partition */
   uint64                          ending_lba;  /* Last LBA of SBL partition */
} boot_flash_shared_sdc_dev_type;

/* Bootable GSBI PORTs for SPI Boot */
/* !!!Warning!!! order of this GSBIn cores should match with oder of cores
   for which registers are defined in spi_reg[] array in spi driver files. 
   spi_reg array defines base address and registers for particular GSBIn core 
   and if below order doesn't match, it would access wrong core and you could 
   see issues !!! */
typedef enum
{
  GSBI_SPI_PORT1,
  GSBI_SPI_PORT5,
  GSBI_SPI_PORT9,
  GSBI_SPI_PORT_INVALID,
  SPI_PORT_SIZE = 0x7FFFFFFF
} boot_flash_spi_port_type;

/*---------------------------------------------------------------------------
  SPI Flash shared data
---------------------------------------------------------------------------*/
typedef struct
{
  uint32   chip_select;
  uint32   addr_len;
  boot_flash_spi_port_type        port;        /* GSBIn_SPI Port where device was detected */
} boot_flash_shared_spi_dev_type;

/*---------------------------------------------------------------------------
  Flash shared data
---------------------------------------------------------------------------*/
typedef struct
{
  boot_flash_type       type;        /* Type of flash                      */
  uint32                CS_base;     /* Base address of chip select device
                                        is connected to.                   */
  uint32                data_width;  /* Bit width of device ie: 8bit/16bit */
  boot_flash_ctrl_type  ctrl_type;   /* Controller used to interface with
                                        device                             */

  union
  {
    boot_flash_shared_nand_dev_type nand;  /* Shared info for NAND device  */
    boot_flash_shared_sdc_dev_type  sdc;   /* Shared info for SDC device   */
    boot_flash_shared_spi_dev_type  spi;   /* Shared infor for SPI device  */
  }dev_info;
}boot_flash_shared_dev_info_type;


/*---------------------------------------------------------------------------
  Magic Number for AutoDetecting Page Size Algorithm
---------------------------------------------------------------------------*/
#define AUTODETECT_PAGE_SIZE_MAGIC_NUM  0x7D0B435A

/*---------------------------------------------------------------------------
  Page Size AutoDetect defines
---------------------------------------------------------------------------*/
#define BOOT_FLASH_1K_SIZE           1024                 /* 1K Byte Page   */
#define BOOT_FLASH_AUTO_DETECT_MAX_PAGE_SIZE_SUPPORTED  \
                              (8*BOOT_FLASH_1K_SIZE)      /* 8K Bytes Page  */
#define BOOT_FLASH_AUTO_DETECT_MIN_PAGE_SIZE_SUPPORTED  \
                              (2*BOOT_FLASH_1K_SIZE)      /* 512 Bytes Page */

                              

/*===========================================================================

                      BOOT HEADERS

===========================================================================*/
/*---------------------------------------------------------------------------
  Magic number definition for identifying SDIO device
---------------------------------------------------------------------------*/
#define SDIO_CODEWORD   0x6EDA809B

/*---------------------------------------------------------------------------
  Magic number definition for identifying SPI device
---------------------------------------------------------------------------*/
#define SPI_CODEWORD    0x844BDCD1

/*---------------------------------------------------------------------------
  Magic number definition for identifying NOR device
---------------------------------------------------------------------------*/
#define NOR_CODEWORD    0x844BDCD1


/*---------------------------------------------------------------------------
  Magic numbers for notifying PBL to enter download mode.
  These numbers MUST be same across bootrom otherwise customer would have to 
  change code to use force download on different bootrom. 
  !!!!!!!!!! Never change below values !!!!!!!!!!!
---------------------------------------------------------------------------*/
#define FORCE_DLOAD_MASK                 0x0000000F

#define FORCE_DLOAD_HS_USB_MAGIC_NUM     0x00000001
#define FORCE_DLOAD_SD_MAGIC_NUM         0x00000002
#define FORCE_DLOAD_MMC_MAGIC_NUM        0x00000003


/*---------------------------------------------------------------------------
  Magic number definition for identifying start of boot configuration data.
---------------------------------------------------------------------------*/
#define MAGIC_NUM       0x73D71034     /* Used for NOR, NAND, etc. flash. */

/*---------------------------------------------------------------------------
  Magic number definition for identifying PBL shared data.
---------------------------------------------------------------------------*/
#define BOOT_PBL_SHARED_DATA_MAGIC  0x78577020

/* Structure to define ID to enter download mode in PBL. */ 
typedef struct
{
  uint32 magic_1;
  uint32 magic_2;
  uint32 magic_3;

} pbl_dload_id_type;

#ifndef _ARM_ASM_
/*---------------------------------------------------------------------------
  Defines the codewords used to specify NAND data width, page size greater
  than 512 byte (future devices), and NAND device type of NAND.  These
  codewords are located in the RPM_SBL header. There are reserved codewords for
  future expansion

  Enumerations are integers according to C standard. 
---------------------------------------------------------------------------*/
#define ONENAND_16_BIT     ((uint32)NAND_8_BIT) /* 16 bit oneNAND */
#define SDC_1_4_8_BIT      ((uint32)NAND_8_BIT) /* 1- 4- or 8-bit SDC */

typedef enum
{
  NO_NAND_DEVICE      = (int)0x00000000, /* Possibly no NAND device
                                            connected */
  NAND_8_BIT          = (int)0x844BDCD1, /* 8 bit NAND */
  NAND_8_BIT_16_BIT   = (int)0xD7344BD1, /* 8 bit and 16 bit NAND codeword */
  CW_RESERVED_0       = (int)0xBF50274A, /* Previously 16 bit oneNAND */
  CW_RESERVED_1       = (int)0x233CD127, /* Reserved */
  CW_RESERVED_2       = (int)0x78A68ABC, /* Previously 8 bit, 2K SUPERAND */
  UNPROGRAMMED_WORD   = (int)0xFFFFFFFF, /* 32 bit word not programmed */
} nand_codeword_enum_type;

typedef enum
{
  DLOAD_DRIVER_INTERFACE_INSTALL_HSUSB  = 0x1, /* 2^0 */
  DLOAD_DRIVER_INTERFACE_INSTALL_HSIC   = 0x2, /* 2^1 */
  DLOAD_DRIVER_INTERFACE_INSTALL_MAX,
  DLOAD_DRIVER_INTERFACE_INSTALL_FORCE32BITS = 0x7FFFFFFF
} dload_driver_interface_install_type;

typedef enum
{
  CLK_SPEED_TCXO   = 20,
  CLK_SPEED_64MHZ  = 64,
  CLK_SPEED_MAX   = 0x7FFFFFFF
}clk_speed_enum;

typedef struct
{
  clk_speed_enum micro_speed;
}clk_speed_type;

/*---------------------------------------------------------------------------
  Defines the SBL header type stored in external flash and read by the boot
  ROM code.

  This structure needs to be word aligned (4 bytes) because the size of
  the structure is used an argument to PBL_MEMCPY
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  codeword;            /* Codeword/magic number defining flash type
                                  information. */
  uint32  magic;               /* Magic number */
  uint32  RESERVED_0;          /* RESERVED */
  uint32  RESERVED_1;          /* RESERVED */
  uint32  RESERVED_2;          /* RESERVED */
  uint32  image_src;             /* Location of RPM_SBL in flash or e-hostdl in RAM. This is given in
                                  byte offset from beginning of flash/RAM.  */
  uint8  *image_dest_ptr;        /* Pointer to location to store RPM_SBL/e-hostdl in RAM.
                                  Also, entry point at which execution begins.
                                  */
  uint32  image_size;      /* Size of RPM_SBL image in bytes */
  uint32  code_size;       /* Size of code region in RPM_SBL image in bytes */
  uint8  *signature_ptr;         /* Pointer to images attestation signature */
  uint32  signature_size;        /* Size of the attestation signature in
                                  bytes */
  uint8  *cert_chain_ptr;  /* Pointer to the chain of attestation
                                  certificates associated with the image. */
  uint32  cert_chain_size; /* Size of the attestation chain in bytes */

  uint32  oem_root_cert_sel;  /* Root certificate to use for authentication.
                                 Only used if SECURE_BOOT1 table_sel fuse is
                                 OEM_TABLE. 1 indicates the first root
                                 certificate in the chain should be used, etc */
  uint32  oem_num_root_certs; /* Number of root certificates in image.
                                 Only used if SECURE_BOOT1 table_sel fuse is
                                 OEM_TABLE. Denotes the number of certificates
                                 OEM has provisioned                          */
          
  uint32  RESERVED_5;          /* RESERVED */
  uint32  RESERVED_6;          /* RESERVED */
  uint32  RESERVED_7;          /* RESERVED */
  uint32  RESERVED_8;          /* RESERVED */
  uint32  RESERVED_9;          /* RESERVED */
} boot_sbl_header_type;


typedef struct pbl_secboot_shared_info_type
{
  secboot_verified_info_type pbl_verified_info;    /**<  */
  secboot_ftbl_type          pbl_secboot_ftbl;     /**< Contains pointers to PBL secboot routines */
  secboot_hw_ftbl_type       pbl_secboot_hw_ftbl;  /**< Contains pointers to PBL secboot hw routines */
} pbl_secboot_shared_info_type;


typedef struct pbl_timestamp_type
{
  uint32 pbl_entry_timestamp;
  uint32 bootable_media_detect_entry_timestamp;
  uint32 bootable_media_detect_success_timestamp;
  uint32 elf_loader_entry_timestamp;
  uint32 auth_hash_seg_entry_timestamp;
  uint32 auth_hash_seg_exit_timestamp;
  uint32 elf_segs_Hash_verify_entry_timestamp;
  uint32 pbl_exit_timestamp;
} pbl_timestamp_type;


/*---------------------------------------------------------------------------
  Magic words to indicate that the data structures are valid.
---------------------------------------------------------------------------*/
#define BOOT_APPS_STATUS_MAGIC          0xA4CD7EA2


/*---------------------------------------------------------------------------
  Defines the data structure that is filled with data by APPS_PBL
  and passed to SBL image. It may be necessary for the SBL to make a deep
  copy of this structure for its own use if it gets wiped out by PBL.

  This is taken directly from pbl_apps.h
---------------------------------------------------------------------------*/
typedef struct boot_pbl_shared_data_type
{
  /* Fields below are filled in by initialization function */
  /* Shared data structure version */
  uint32                          shared_data_version;

  /* PBL version */
  uint32                           pbl_version;
  
  /* Apps processor clock speed set up in PBL */
  uint32                          proc_clk_speed;

  /* PBL Boot option configured in the fuse */
  uint32                          pbl_boot_option;

  /* Force download option if there was a force dload boot */
  uint32                          force_dload_option;
  
  /* Page table base address */
  uint32                          *pbl_page_tbl_base;

  /* Page table size */
  uint32                           pbl_page_tbl_size;

  /* PBL stack base */
  uint8                           *boot_stack_base;

  /* PBL stack size */
  uint32                           boot_stack_size;

  /* The region of memory required by PBL shared data. */
  /* PBL Shared data section base address */
  uint8                           *shared_section_base;

  /* PBL Shared data section size */
  uint32                          shared_section_size;

  /* Fields below are filled in by flash module. This is a pointer
   * that points to a structure in PBL scratch memory. SBL needs to
   *  make its own deep copy of this structure */
  boot_flash_shared_dev_info_type *flash_shared_data;

  /* Fields below are filled in by auth module */
  pbl_secboot_shared_info_type    *secboot_shared_data;
  
  /* Timestamps at main milestones in PBL */
  pbl_timestamp_type              timestamps;

} boot_pbl_shared_data_type;


/* Timestamps from PBL are given to SBL as ticks of a timer.  The timer's
   frequency is not currently passed to SBL so the value must be hardcoded
   here. */
#define PBL_TIMESTAMP_TIMER_FREQ_HZ 19200000  /* 19.2Mhz */


#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

**  Function :  boot_pbl_get_pbl_version

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the PBL version from PBL shared data.
* 
* @par Dependencies
*   None
*   
* @retval
*   uint32
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_pbl_get_pbl_version
(
  void
);

/*=========================================================================

**  Function :  boot_pbl_get_clock_speed

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the clock speed set by PBL.
* 
* @par Dependencies
*   None
*   
* @retval
*   uint32
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_pbl_get_clock_speed
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_get_flash_shared_data

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the pointer to the flash related information found
*   by PBL.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer of boot_flash_shared_dev_info_type type
* 
* @par Side Effects
*   None.
* 
*/
boot_flash_shared_dev_info_type* boot_pbl_get_flash_shared_data
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_is_auth_enabled

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns whether PBL detected that auth was enabled.
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_boolean
* 
* @par Side Effects
*   None.
* 
*/
boot_boolean boot_pbl_is_auth_enabled
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_get_flash_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the flash type detected by PBL
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_flash_type
* 
* @par Side Effects
*   None.
* 
*/
boot_flash_type boot_pbl_get_flash_type
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_interface_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Function that will initialize the target interface so that generic SBL
*   code can use it.
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_boolean
* 
* @par Side Effects
*   None
* 
*/
boot_boolean boot_pbl_interface_init
(
  boot_pbl_shared_data_type   *pbl_shared_data  /* Pointer to data coming from PBL */
);


/*===========================================================================

**  Function :  boot_pbl_log_milestones

** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will parse the PBL timestamp milestones passed to SBL
*   and insert them into the boot log.  Currently PBL's unit of measure is
*   clock ticks.  PBL does not pass the clock frequency yet so it is hard
*   wired to 19.2 Mhz.  Also PBL does not pass the names of each field so for
*   now reference a structure of our own to get the names which will have
*   logic ready for the day PBL starts passing them.
*
* @param[in]
*   boot_pbl_shared_data_type
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_pbl_log_milestones(boot_pbl_shared_data_type * pbl_shared_data);


#endif /* BOOT_PBL_V2_H */
