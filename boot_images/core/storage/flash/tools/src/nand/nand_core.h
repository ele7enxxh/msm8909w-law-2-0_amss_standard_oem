#ifndef NAND_CORE_H
#define NAND_CORE_H

/*=======================================================================
 * FILE:        nand_core.h
 *
 * SERVICES:    Flash Programming Plug-In for Lauterbach FLASH_PROG/ICD TRACE32
 *
 * DESCRIPTION
 *   This file has the main functions used for programming the flash
 *
 * Copyright(c) 2008-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_core.h#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ 
 *   $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 07/14/14     sb      Move page buffers to Code RAM to support non DDR based tools
 * 06/21/13     sb      Clean up as part of removing user partition table from MIBIB 
 * 02/15/13     sv      Clean up
 * 09/11/12     eo      Remove miboot.h
 * 08/02/11     sv      Add DSP3 support
 * 07/20/11     eo      Added SBL2/RPM partition support for 9x15
 * 04/11/11     jz      Define FLASH_TOTAL_NUM_PART_ENTRIES for factory image partitions
 * 03/18/11     jz      Updated flash_prog_init_parti()
 * 09/13/10     sb      Define MAX_PAGE_SIZE if not define
 * 09/01/10     nr      Defer first block erase in tools till stream write
 * 06/10/10     sv      Change update image names, Fix compilation issue
 * 04/12/10     mh      Revert to the latest
 * 04/12/10     mh      Revert to #10 & add dual nand ionterleave mode support
 * 02/03/10     op      Add OSBL, DSP, AMSS Image update for 8650A
 * 01/07/10     eo      Add support for 4K page OneNAND
 * 12/07/09     wt      Added support for ROFS partition Dump 
 * 12/04/09     sv      Enable support to program APPS image in XSR parti
 * 12/03/09     wt      Fix to Remove the warnings  
 * 09/25/09     mh      Increase FLASH_NAND_SPARE_NUM_BYTES support for dual NAND
 * 10/12/09     mm      Fix Bad Block Mode setting
 * 10/08/09     op      Make DM buffer 32byte alignment to avoid any misaligned
 * 10/02/09     mm      Added support for programming images in BML Format
 * 07/08/09     mm      Support for programming extended partition table
 * 02/25/09     eo      Add OneNAND QFIT support
 * 02/09/09     jz      Added support for SB2.0
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_prog.h"
#include "nand_tools.h"
#include "flash_miparti.h"

#ifndef BUILD_FOR_X86
#include "flash.h"
#define ALIGN_VAL 32
#else
#include "nand_sim.h"
#define ALIGN_VAL 4
#endif

/* These defines is just use for compile the Jnand tools, so we don't have to the #ifdef in the tools code */
/*  
 * Define the dummy partition names for those partion names undefined in mibib.h
 */
#ifndef MIBIB_OEMSBL1_PARTI_NAME
 #define MIBIB_OEMSBL1_PARTI_NAME "0:OEMSBL1"
#endif
#ifndef MIBIB_OEMSBL2_PARTI_NAME
 #define MIBIB_OEMSBL2_PARTI_NAME "0:OEMSBL2"
#endif
#ifndef MIBIB_OEMSBL_PARTI_NAME
 #define MIBIB_OEMSBL_PARTI_NAME "0:OEMSBL"
#endif
#ifndef MIBIB_QCSBL_PARTI_NAME
 #define MIBIB_QCSBL_PARTI_NAME "0:QCSBL"
#endif

/* Partition names used by Secure Boot architecture 2.0 */
#ifndef MIBIB_DBL_PARTI_NAME
  #define MIBIB_DBL_PARTI_NAME "0:DBL"
#endif
#ifndef MIBIB_FSBL_PARTI_NAME
  #define MIBIB_FSBL_PARTI_NAME "0:FSBL"
#endif
#ifndef MIBIB_OSBL_PARTI_NAME
  #define MIBIB_OSBL_PARTI_NAME "0:OSBL"
#endif
#define MI_FSBL_MAGIC1 0x6FC123DF
#define MI_FSBL_MAGIC2 0x60FDEFC7
#define MI_OSBL_MAGIC1 0x6CBA1CFD
#define MI_OSBL_MAGIC2 0x68D2CBE9

/* Partition names for Chrome OS */
#ifndef MIBIB_APPSBL_REC_PARTI_NAME
  #define MIBIB_APPSBL_REC_PARTI_NAME "0:APPSBL_REC"
#endif
#ifndef MIBIB_OSBL_UPDATE_PARTI_NAME
  #define MIBIB_OSBL_UPDATE_PARTI_NAME "0:OSBL_B"
  #define MI_OSBL_CHROME_MAGIC1 0x7DCB2D1E
  #define MI_OSBL_CHROME_MAGIC2 0x79E3DCFA
#endif
#ifndef MIBIB_AMSS_UPDATE_PARTI_NAME
  #define MIBIB_AMSS_UPDATE_PARTI_NAME "0:AMSS_B"
#endif
#ifndef MIBIB_DSP_UPDATE_PARTI_NAME
  #define MIBIB_DSP_UPDATE_PARTI_NAME "0:DSP_B"
#endif

#ifndef MIBIB_ADSP_Q5_PARTI_NAME
  #define MIBIB_ADSP_Q5_PARTI_NAME "0:QDSP_Q5"
#endif

#ifndef MIBIB_SBL2_PARTI_NAME
  #define MIBIB_SBL2_PARTI_NAME  "0:SBL2"
  #define MI_SBL2_MAGIC1         0x6012780C
  #define MI_SBL2_MAGIC2         0x6C93B127
#endif

#ifndef MIBIB_RPM_PARTI_NAME
  #define MIBIB_RPM_PARTI_NAME  "0:RPM"
#endif

#ifndef MIBIB_DSP3_PARTI_NAME
  #define MIBIB_DSP3_PARTI_NAME  "0:DSP3"
#endif

#ifndef NAND_TOOLS_SBL1_PARTI_NAME
  #define NAND_TOOLS_SBL1_PARTI_NAME  "0:SBL1"
#endif

#ifndef NAND_TOOLS_DBL_PARTI_NAME
  #define NAND_TOOLS_DBL_PARTI_NAME  "0:DBL"
#endif

/* Defines of OneNand 2KB Page Spare ECC field locations info. 
 * Used in nand_core.c for initializing the num_cw (number of 
 * codewords), ecc_offset and ecc_len fields in image partition 
 * table with proper ECC layout. 
 */
#define FLASH_ONENAND_2K_SPARE_NUM_CW    4
#define FLASH_ONENAND_2K_SPARE_ECCM_BOFF 8
#define FLASH_ONENAND_2K_SPARE_ECCM_BLEN 3
#define FLASH_ONENAND_2K_SPARE_ECCS_BOFF 11
#define FLASH_ONENAND_2K_SPARE_ECCS_BLEN 2

/* Defines of OneNand 2KB Page Spare ECC field locations info. 
 * Used in nand_core.c for initializing the num_cw (number of 
 * codewords), ecc_offset and ecc_len fields in image partition 
 * table with proper ECC layout. 
 */
#define FLASH_ONENAND_4K_SPARE_NUM_CW    8
#define FLASH_ONENAND_4K_SPARE_ECCM_BOFF 6
#define FLASH_ONENAND_4K_SPARE_ECCM_BLEN 10

/* Defines related to flash buffers and sizes */

/* Always define this to be so that the same set of tools will work for
 * Large Block and regular NAND.
 */

#ifndef MAX_PAGE_SIZE
  #ifdef NAND_PAGE_SIZE
    #define MAX_PAGE_SIZE NAND_PAGE_SIZE
  #else
    #error MAX_PAGE_SIZE or NAND_PAGE_SIZE must be defined!!
  #endif
#endif

#define MAX_SPARE_SIZE  (MAX_PAGE_SIZE >> 5)
#define TOTAL_PAGE_SIZE (MAX_PAGE_SIZE + MAX_SPARE_SIZE)


#define FLASH_NAND_SPARE_NUM_BYTES 256

#define NAND_FILL_CHAR (0xFF)

/* Value that indicates that we have not received the security mode */
#define SEC_MODE_NOT_RCVD (-1)

/* Total number of partitions supported (including both modem and apps side) */
#define FLASH_TOTAL_NUM_PART_ENTRIES 32

/* Programming mode types */
typedef enum
{
  MODE_NONE =        0x00,   /* No Programming mode yet            */
  MODE_QCSBLHD =     0x01,   /* Device Boot Loader                 */
  MODE_QCSBL =       0x02,   /* Device Boot Loader                 */
  MODE_OEMSBL =      0x03,   /* OEM Secondary Boot Loader          */
  MODE_DBL =         0x04,   /* Device Boot Loader                 */
  MODE_FSBL =        0x05,   /* Failsafe Secondary Boot Loader     */
  MODE_OSBL =        0x06,   /* OS Secondary Boot Loader           */
  MODE_AMSS =        0x07,   /* AMSS Image                         */
  MODE_APPSBL =      0x08,   /* APPS boot loader                   */
  MODE_APPS =        0x09,   /* APPS Image                         */
  MODE_CEFS =        0x0A,   /* Compact EFS image                  */
  MODE_FOTAUI =      0x0B,   /* FOTA UI Image                      */
  MODE_CEFS_APPS =   0x0C,   /* Compact EFS image for APPS proc.   */
  MODE_WM_APPS =     0x0D,   /* Flash.bin for APPS proc. for WM    */
  MODE_CUSTOM_IMG =  0x0E,   /* Image for user defined partition   */
  MODE_DSP1 =        0x0F,   /* DSP1 Image                         */
  MODE_DSP2 =        0x10,   /* DSP2 Image                         */
  MODE_RAW_IMG =     0x11,   /* Raw image                          */   
  MODE_FACTORY_IMG = 0x12,   /* Factory image                      */
  MODE_ROFS1       = 0x13,   /* ROFS1   image                      */
  MODE_ROFS2       = 0x14,   /* ROFS2   image                      */
  MODE_ROFS3       = 0x15,   /* ROFS3   image                      */
  MODE_OSBL_UPDATE=  0x16,   /* OSBL UPDATE Mode                   */
  MODE_AMSS_UPDATE=  0x17,   /* AMSS Update Mode                   */
  MODE_DSP_UPDATE=   0x18,   /* DSP update mode                    */
  MODE_SBL =         0x1B,   /* Secondary Boot Loader              */
  MODE_RPM =         0x1C,   /* RPM Image                          */
  MODE_DSP3 =        0x1D,   /* DSP3 Image                        */
  MODE_UNKNOWN =     0xFF    /* Unsupported mode                   */
} mode_type;

/* Definition of structure to hold device structure, current state variables
 * and page buffers for each partition that is in use.  Since we will have
 * multiple partitions open at the same time, each partition must have its
 * set of these data structures
 */
struct parti_ctl;
typedef struct parti_ctl *parti_ctl_t;

/* Factory Image partition table */
typedef struct factory_img_parti_tbl
{
  /* Logical address of start of partition in flash blocks. */
  uint32 start_addr;

  /* Last valid flash block in the current partition */
  uint32 end_addr;

  /* Number of blocks of data to program from the factory image input file */
  uint32 actual_blks;

  /* Partition name */
  char name[FLASH_FACTORY_IMAGE_PARTI_NAME];

} factory_img_parti_tbl_t;


/* Parser states */
typedef enum parsing_state
{
  FLASH_FACTORY_PARSER_STATE_HDR_PARTIAL,  /* Parsing the partial header  */
  FLASH_FACTORY_PARSER_STATE_HDR_COMPLETE, /* Parsing the complete header */
  FLASH_FACTORY_PARSER_STATE_PARTITION,    /* Parsing the partition table */
  FLASH_FACTORY_PARSER_STATE_DONE          /* Program binary image data */
}parsing_state_t;

/* Factory Image Programming variables */
typedef struct factory_img_info
{
  uint32 parti_tbl_offset; /* Partition Image offset in the FACTORY Image     */
  uint32 bin_img_offset;   /* Offset where the binary image data starts       */
                           /* Partition table from the factory image          */
  factory_img_parti_tbl_t parti_tbl[FLASH_TOTAL_NUM_PART_ENTRIES];
  parsing_state_t parsing_state; /* State information of the image parser     */

  byte *buf; /* Buffer for incomplete data received           */
  unsigned int buf_bytes;    /* Number of bytes buffered                      */

  uint32 bb_count[FLASH_TOTAL_NUM_PART_ENTRIES]; /* Bad block count per partition */
  uint32 curr_parti;                      /* Image partition being programmed */

}factory_img_info_t;

struct  parti_ctl
{
  flash_handle_t flash_handle;
  struct flash_info flash_info;
  uint32 current_block;
  uint32 total_block_count;
  uint32 avail_pages_in_block;
  uint32 current_page;
  uint32 start_addr_loaded;
  uint32 last_code_addr;
  uint32 page_size;
  /* Buffer to store data to write in.  Generally only used on blocks
   * which span successive invocations. Make sure it's 4-bytes aligned.
   */
  unsigned char *partial_page_buffer;
  enum page_write_opcode write_ops;
  /* Variables added for flash.bin programming */
  byte   wm_version;           /* Save the WM version (6.1, 7 etc) */
  uint32 record_header_parsed; /* Flag to check if the record header is parsed*/
  uint32 curr_record_addr;     /* Current record address                      */
  uint32 prev_record_addr;     /* Previous record address                     */
  uint32 record_len;           /* Record Length*/
  uint32 spare_bytes_len;      /* Length of Spare bytes to be read            */

  /* Buffer to store spare data to write in */
  unsigned char partial_spare_buffer[FLASH_NAND_SPARE_NUM_BYTES];
  unsigned int partial_spare_index;
  uint32 write_spare;          /* Flag indicate need to read spare byte       */
                            /* Store Bad block info on a partition basis      */
  uint32 bad_block_count[FLASH_MAX_PARTITION_FLASH_BIN];
  uint32 flash_bin_partitions; /* Number of partition in flash.bin            */
  int record_header_parsed_partially;  /* Flag to show partial read of header */
  int len_record_header_parsed;  /* Length of header parsed                   */
                                 /* Temp buffer to store the header           */
  uint32 flash_bin_record_header_size;
  uint32 flash_bin_record_data_size;
  uint32 flash_bin_page_spare_size;

  byte temp_record_addr[FLASH_WM_FLASH_BIN_RECORD_ADDR_SIZE];

  unsigned int partial_page_index;
  factory_img_info_t factory_img;        /* Factory image programming info    */
  flash_param_t dev_params;
  
  /* Variables used for OneNAND factory image programming */
  unsigned int ecc_num_cw;
  unsigned int ecc_offset;
  unsigned int ecc_len;
  uint32 partition_initial_blk_prepared;
};

extern struct parti_ctl mibib;
extern int partition_rcvd;
extern int oldest_mibib_block;
extern int newest_mibib_block;
extern struct parti_ctl image;
extern mode_type current_mode;
extern struct boot_block_ctl boot_layout;
extern int flash_hw_present;
extern unsigned char oldest_usr_parti_buffer[MAX_PAGE_SIZE];
extern unsigned char oldest_parti_buffer[MAX_PAGE_SIZE];
extern enum page_write_opcode write_type;
extern enum page_read_opcode read_type;

/* Mode for bad block management 
 * Skip for Qualcomm bad block management Algorithm 
 * Replace for Unistore II Bad Block Algorithm used by Unistore II
 */
extern flash_prog_bad_block_mode_t current_bad_block_mode;

uint32 flash_prog_set_security_mode (void);
uint32 flash_prog_init_parti (uint32 size);
uint32 flash_prog_deinit_tools(void);
int    flash_prog_get_flash_info (void);
uint32 flash_prog_program_img (parti_ctl_t pctl);
uint32 flash_prog_program_factory_img(parti_ctl_t pctl);
uint32 flash_prog_program_flash_bin(parti_ctl_t pctl);
int flash_prog_write_magic_number_page (parti_ctl_t pctl, uint32 *magic_buf,
  int count);
int flash_prog_erase_partition(parti_ctl_t pctl);
int flash_prog_find_starting_erase_block (flash_partable_t ram_parti_ptr);
uint32 flash_prog_erase_chip (void);
uint32 flash_prog_erase_some_of_chip (uint32 start_block);
int flash_prog_erase_bad_block_table(void);
uint32 flash_prog_erase_MIBIB_partition(flash_partable_t parti_ptr);
int flash_prog_write_partial_page_data (parti_ctl_t pctl);
int flash_prog_write_current_page (parti_ctl_t pctl, 
  unsigned char *src_main, unsigned char *src_spare );
int flash_prog_init_for_partition_image (parti_ctl_t pctl, char *pname);
flash_prog_status_t flash_prog_set_img_type(flash_prog_img_type_t img_type,
  void *hdr_data, uint32 hdr_len);

int flash_prog_find_nth_good_block(flash_handle_t flash_handle,
                                          int which_block);

int flash_prog_convert_usr_parti_tbl_to_sys (flash_handle_t flash_handle,
                                             flash_usr_partable_t parti1,
                                             flash_partable_t parti2);

int flash_prog_partition_tables_are_equal (flash_partable_t parti1,
                                      flash_partable_t parti2);

uint32 flash_prog_init_parti_dump(void);
uint32 flash_prog_dump_parti(void);
uint32 flash_prog_print_mibib(void);
uint32 flash_prog_dump_mibib_parti_block(void);
uint32 flash_prog_print_layout(void);

/* The following functions are implemented in config folders */
int    flash_prog_image_init(void);
uint32 flash_prog_image_finalize (void);
int flash_prog_prepare_this_block (parti_ctl_t pctl);
int flash_prog_write_mibib(void);
int flash_prog_setup_mibib_not_present (void);
uint32 flash_prog_set_block_offsets_in_mibib(int security_mode);
int flash_prog_setup_mibib_present (flash_usr_partable_t usr_parti_ptr,
  flash_partable_t sys_parti_ptr);
int flash_prog_get_bbtb_block_from_mibib(void);

#ifdef FLASH_PROG_EXT_PARTI
  int flash_prog_bml_write_current_page (parti_ctl_t pctl, 
    unsigned char *src_main, unsigned char *src_spare);
  int flash_prog_bml_write_partial_page_data (parti_ctl_t pctl);
int flash_prog_extended_parti(void);
  uint32 flash_prog_bml_image_init(uint32 image_id);
  uint32 flash_prog_init_parti_xsr_userdata_dump(void);
  uint32 flash_prog_dump_xsr_userdata_parti(void);
  uint32 flash_prog_init_parti_xsr_rofs_dump(void);
  uint32 flash_prog_dump_xsr_rofs_parti(void);
  uint32 flash_display_rofs_partitions(void);
  flash_prog_bad_block_mode_t flash_prog_get_bad_blk_mode(mode_type img_mode);
#else
  /* Stubs */
  static inline int flash_prog_bml_write_current_page (parti_ctl_t pctl, 
    unsigned char *src_main, unsigned char *src_spare) 
  { return FLASH_PROG_FAIL; }
  static inline int flash_prog_bml_write_partial_page_data (parti_ctl_t pctl)
  { return FLASH_PROG_FAIL; }
  static inline int flash_prog_extended_parti(void) 
  { return FLASH_PROG_SUCCESS; }
  static inline uint32 flash_prog_bml_image_init(uint32 image_id) 
  { return FLASH_PROG_FAIL; }
  static inline uint32 flash_prog_init_parti_xsr_userdata_dump(void)
  { return FLASH_PROG_FAIL; }
  static inline uint32 flash_prog_dump_xsr_userdata_parti(void)
  { return FLASH_PROG_FAIL; }
   static inline uint32 flash_prog_init_parti_xsr_rofs_dump(void)
  { return FLASH_PROG_FAIL; }
  static inline uint32 flash_prog_dump_xsr_rofs_parti(void)
  { return FLASH_PROG_FAIL; }
  static inline uint32 flash_display_rofs_partitions(void)
  { return FLASH_PROG_FAIL; }
  static flash_prog_bad_block_mode_t flash_prog_get_bad_blk_mode (
    mode_type img_mode)
  { return FLASH_PROG_BAD_BLOCK_SKIP; }
  
#endif

#endif /* NAND_CORE_H */
