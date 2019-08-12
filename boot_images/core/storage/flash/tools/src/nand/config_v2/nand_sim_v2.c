/*=======================================================================
 * FILE:        nand_sim_v2.c
 *
 * SERVICES:    Supports factory image generation for SB2.0 configuration.
 *
 * GENERAL DESCRIPTION
 *   This file provides wrapper for calling main_c from a console application
 *   to generate contents of MIBIB partition without actual flash device.
 *
 * Copyright(c) 2010 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/config_v2/nand_sim_v2.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 08/03/10     bn      Klocwork warning fixes
 * 10/08/09     jz      Fixed the read mode in binary file open
 * 02/27/09     jz      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "nand_core.h"
#include "flash_prog.h"
#include "nand_tools.h"
#include "nand_param.h"
#include "nand_core_v2.h"
#include "nand_wrapper.h"
#include "nand_sim.h"


/* Allocate one extra block beyond what is required for essential MIBIB blocks
 * to represent rest of the device. This is required for the the init routines
 * that prep the main image area by erasing the first good block in it. */
unsigned char ram_blocks[NAND_RAM_DEV_SIZE][MAX_BLOCK_SIZE][TOTAL_PAGE_SIZE];

static char flash_name[MAX_NAME_SIZE];
static char log_fname[MAX_NAME_SIZE], yes_flag[MAX_FLAG_SIZE];
static char usr_parti_fname[MAX_NAME_SIZE], mibib_fname[MAX_NAME_SIZE];
static char dbl_fname[MAX_NAME_SIZE], bin_path[SIZE_ONE_K];
static char usr_bin_path[SIZE_ONE_K], mibib_bin_path[SIZE_ONE_K];
static char log_txt_path[SIZE_ONE_K], dbl_bin_path[SIZE_ONE_K];


/*===========================================================================

  This function is a wrapper for calling main_c from a console application to
  generate contents of MIBIB partition without actual flash device.

===========================================================================*/
int main(int argc, char *argv[])
{
  FILE *usr_parti_file, *mibib_file, *dbl_file, *log_file, *cfg_file;
  int count, i;
  char ch, log_page_size, log_block_size, log_device_size;
  char * buf_ptr = (char *) prog_param.data;

  unsigned int page_size, block_size, dev_size, jnand_extended_error;
  unsigned int page, blk, dump_start_block, dump_end_block;

  int bin_path_known = FALSE;
  int usr_parti_fname_known = FALSE;
  int mibib_fname_known = FALSE;
  int device_name_known = FALSE;
  int page_size_known = FALSE;
  int block_size_known = FALSE;
  int device_size_known = FALSE;
  int dump_log_file = FALSE;
  int log_fname_known = FALSE;
  int dump_mibib_partition = FALSE;
  int dbl_fname_known = FALSE;
  int dump_mibib_block = FALSE;
  int use_config_file = FALSE;

  struct flash_block_vector blk_vector;

  if(argc < 3)
  {
    printf("\n\nConfiguration data not available!! ");
    printf("Please provide config data on command line or in config file\n\n");
    printf("\n\nUSAGE\n\n");
    printf("%s -c config_file.cfg\n", argv[0]);
    printf ("\nOR\n\n");
    printf("%s ", argv[0]);
    printf("-fp flash_page_size ");
    printf("-fb flash_block_size ");
    printf("-fd flash_device_size ");
    printf("-d device_bootloader_fname ");
    printf("-u usr_partition_fname ");
    printf("-m mibib_partition_fname ");
    printf("-p binary_folder_path \n");

    printf ("\n\nOPTIONAL SWITCHES\n\n");
    printf("-f \"flash_device_name\" ");

    printf ("\n\nPARAMETER DESCRIPTION\n\n");
    printf("flash_page_size            Page size of flash device in bytes\n");
    printf("flash_block_size           Block size of flash device in pages\n");
    printf("flash_device_size          Flash device size in blocks\n");
    printf("device_bootloader_fname   File containing DBL binary\n");
    printf("usr_partition_fname        FIle containing usr partition table\n");
    printf("mibib_partition_fname      File to dump MIBIB partition\n");
    printf("binary_folder_path         Path to folder caontaining binaries\n");
    printf("\n\nOPTIONAL SWITCHES\n\n");
    printf("-s  Used to specify MSM security mode when default security\n");
    printf("    security mode is not used. By default security mode is set\n");
    printf("    to \"TRUSTED\". This switch requires '-r' switch as well.\n\n");
    printf("Example:  mjnand.exe -fp 512 -fb 32 -fd 64 [-f \"TC58512FT\" \\\n");
    printf("                     -u partition.mbn -m mibib.mbn \\\n");
    printf("                     -p .\\..\\..\\build\\ms\\bin\\KYRQ \n");

    exit(EXIT_FAILURE);
  }
  else if (argc == 3)
  {
    if ((argv[1][0] == '-') && (argv[1][1] == 'c'))
    {
      use_config_file = TRUE;
      if ((cfg_file=fopen(argv[2], "r")) == NULL)
      {
        printf("\nCould not open config file: %s\n", argv[2]);
        exit(EXIT_FAILURE);
      }
      printf("Using config file %s to read input parameters\n\n", argv[2]);
    }
    else
    {
      if ((argv[1][1] != 'f') && (argv[1][1] != 'u') &&
          (argv[1][1] != 's') && (argv[1][1] != 'p') &&
          (argv[1][1] != 'm') && (argv[1][1] != 'd'))
      {
        printf("\nUnknown command line switch: %s\n", argv[1]);
      }
      else if ('f' == argv[1][1])
      {
        if ((argv[1][2] != 'p') && (argv[1][2] != 'b') &&
            (argv[1][2] != 'd'))
        {
          printf("\nUnknown command line switch: %s\n", argv[1]);
        }
      }
      else
      {
        printf("\nInsufficient configuration data!\n");
      }
      exit(EXIT_FAILURE);
    }

    /* Start parsing the config file to read input parameters. If necessary
     * info is not available bail out indicating error. */
    i = fscanf(cfg_file, "FLASH_NAME=%s\n", flash_name);

    if (1 == i)
    {
      device_name_known = TRUE;
      printf("Using flash name %s to find geometry data\n", flash_name);
    }
    else
    {
      i = fscanf(cfg_file, "PAGE_SIZE=%d\n", &page_size);
      printf("i = %d\n", i);
      if ((i != 0) && (page_size != 0))
      {
        page_size_known = TRUE;
      }

      i = fscanf(cfg_file, "BLOCK_SIZE=%d\n", &block_size);
      printf("i = %d\n", i);
      if ((i != 0) && (block_size != 0))
      {
        block_size_known = TRUE;
      }

      i = fscanf(cfg_file, "NUM_BLOCKS=%d\n", &dev_size);
      printf("i = %d\n", i);
      if ((i != 0) && (block_size != 0))
      {
        device_size_known = TRUE;
      }
      //printf("Using nand page size of %d bytes\n", page_size);
      //printf("Using nand block size of %d pages\n", block_size);
      //printf("Using nand device size of %d blocks\n", dev_size);
    }

    i = fscanf(cfg_file, "BIN_PATH=%s\n", bin_path);
    printf("BIN_PATH i = %d\n", i);
    if (1 == i)
    {
      bin_path_known = TRUE;
    }

    i = fscanf(cfg_file, "USR_PARTITION_BINARY=%s\n", usr_parti_fname);
    printf("USR_PARTI i = %d\n", i);
    if (1 == i)
    {
      usr_parti_fname_known = TRUE;
    }

    i = fscanf(cfg_file, "MIBIB_BINARY=%s\n", mibib_fname);
    printf("MIBIB_BINARY i = %d\n", i);
    if (1 == i)
    {
      mibib_fname_known = TRUE;
      dump_mibib_partition = TRUE;
    }

    i = fscanf(cfg_file, "DBL_BINARY=%s\n", dbl_fname);
    printf("DBL_BINARY i = %d\n", i);
    if (1 == i)
    {
      dbl_fname_known = TRUE;
    }

    i = fscanf(cfg_file, "CREATE_LOG=%s\n", yes_flag);
    printf("CREATE_LOG i = %d\n", i);

    if( (1 == i) &&
        (yes_flag[0] == 'Y') &&
        (yes_flag[1] == 'E') &&
        (yes_flag[2] == 'S') )
    {
      i = fscanf(cfg_file, "LOG_FILE=%s\n", log_fname);
      printf("LOG_FILE i = %d\n", i);
      if (1 == i)
      {
        log_fname_known = TRUE;
      }
      dump_log_file = TRUE;
    }
    else
    {
      printf("\nNo Logs will be created\n\n");
    }

    fclose (cfg_file);
  } // end - parsing of the config file.
  else
  {
    /* Process the given command line parameters. We will need to have the
     * flash device geometry information, path to the bin directory and the
     * file names for binaries that comprise MIBIB blcoks.
     */
    bin_path_known = FALSE;

    for (i = 1; i < argc; i = i + 2)
    {

      if (argv[i][0] != '-')
      {
        printf("\n\nOptions not specified correctly\n");
        return 1;
      }

      switch (argv[i][1])
      {
        case 'p':
          //printf("Using binaries in folder: %s\n", argv[i+1]);
          flash_sim_strcpy(bin_path, argv[i+1]);
          bin_path_known = TRUE;
          break;
        case 'u':
          //printf("Using usr parti binary: %s\n", argv[i+1]);
          flash_sim_strcpy(usr_parti_fname, argv[i+1]);
          usr_parti_fname_known = TRUE;
          break;
        case 'd':
          //printf("Using device boot loader binary: %s\n", argv[i+1]);
          flash_sim_strcpy(dbl_fname, argv[i+1]);
          dbl_fname_known = TRUE;
          break;
        case 'm':
          //printf("Using mibib binary: %s\n", argv[i+1]);
          flash_sim_strcpy(mibib_fname, argv[i+1]);
          mibib_fname_known = TRUE;
          dump_mibib_partition = TRUE;
          break;
        case 'f':
          {
            int length;
            length = strlen(argv[i]);

            if (2 == length)
            {
              printf("Using Flash part: %s\n", argv[i+1]);
              flash_sim_strcpy(flash_name, argv[i+1]);
              device_name_known = TRUE;
              break;
            }
            else if (3 == length)
            {
              switch (argv[i][2])
              {
                case 'p':
                  page_size = atoi(argv[i+1]);
                  page_size_known = TRUE;
                  break;
                case 'b':
                  block_size = atoi(argv[i+1]);
                  block_size_known = TRUE;
                  break;
                case 'd':
                  dev_size = atoi(argv[i+1]);
                  device_size_known = TRUE;
                  break;
                default:
                  printf("Unknown switch -f%c encountered\n", argv[i][2]);
                  exit(EXIT_FAILURE);
                  break;
              }
            } // end - if length equals 3
            else
            {
              printf("Unknown switch %s encountered\n", argv[i]);
              exit(EXIT_FAILURE);
            }
          }
          break;
        default:
          printf("Unknown switch -%c encountered\n", argv[i][1]);
          exit(EXIT_FAILURE);
          break;
      }
    }
  }
  /* All command line switches have been parsed. Check to see if
   * there is sufficent information available to proceed. Make
   * some choices and post appropriate messages on the
   * console. */
  if (TRUE == use_config_file)
  {
    printf("\n\nUsing config file for input parameters\n\n");
  }
  else
  {
    printf("\n\nUsing command line input parameters\n\n");
  }

  if (TRUE == device_name_known)
  {
    printf("Using flash name %s to find geometry data\n", flash_name);
  }
  else if ((TRUE == page_size_known) &&
           (TRUE == block_size_known) &&
           (TRUE == device_size_known))
  {
    printf("Using flash device geomery:\n");
    printf("  Page size of %d bytes\n", page_size);
    printf("  Block size of %d pages\n", block_size);
    printf("  Device size of %d blocks\n\n", dev_size);
  }
  else
  {
    printf("Insufficient device geometry information\n");
    exit(EXIT_FAILURE);
  }

  if (TRUE == bin_path_known)
  {
    printf("Using binaries in folder:      %s\n", bin_path);
  }
  else
  {
    printf("Path to the binaries not provided. Cannot Proceed!\n");
    exit(EXIT_FAILURE);
  }

  if (TRUE == usr_parti_fname_known)
  {
    flash_sim_makepath(usr_bin_path, bin_path, usr_parti_fname);
    printf("Using usr parti binary:        %s\n", usr_bin_path);
  }
  else
  {
    printf("User partition table name not provided. Cannot Proceed!\n");
    exit(EXIT_FAILURE);
  }

  if (TRUE == dbl_fname_known)
  {
    flash_sim_makepath(dbl_bin_path, bin_path, dbl_fname);
    printf("Using dbl binary:              %s\n", dbl_bin_path);
  }
  else
  {
    printf("DBL file name not provided. Cannot Proceed!\n");
    exit(EXIT_FAILURE);
  }


  if (TRUE == mibib_fname_known)
  {
    flash_sim_makepath(mibib_bin_path, bin_path, mibib_fname);

    if (TRUE == dump_mibib_partition)
    {
      printf("\nDumping MIBIB partition in binary: %s\n", mibib_bin_path);
    }
    else
    {
      int len;

      dump_mibib_block = TRUE;

      /* Force the filename extention to be 'dat' if only MIBIB blocks are
       * being dumped. */
      len = strlen(mibib_bin_path);

      if (len > 3)
      {
        nand_memcpy(&mibib_bin_path[len - 3], "dat", 3);
        printf("Dumping MIBIB block in binary: %s\n", mibib_bin_path);
      }
      else
      {
        printf("Error! Unsupported filename. Path = %s\n", mibib_bin_path);
        exit(EXIT_FAILURE);
      }
    }
  }
  else
  {
    printf("File name for MIBIB data dump not provided. Cannot Proceed!\n");
    exit(EXIT_FAILURE);
  }

  if (TRUE == dump_log_file)
  {
    if (TRUE == log_fname_known)
    {
      flash_sim_makepath(log_txt_path, bin_path, log_fname);
      printf("Generating logs in: %s\n", log_txt_path);
    }
    else
    {
      printf("File name log messages not provided\n");
      printf("Log will not be generated\n");
    }
  }

  /* Since this code is compiled on host build environment check the derived
   * types to make sure that they are defined properly. This is just a safety
   * check.
   */
  if (sizeof(uint32) != 4)
  {
    printf("\nType uint32 is defined to be %d bytes\n", sizeof(uint32));
    exit(EXIT_FAILURE);
  }

  /* Sufficient input information is available to proceed ahead. Start by
   * opening required files. */

  if ((usr_parti_file=fopen(usr_bin_path, "rb")) == NULL)
  {
    printf("\nCould not open User parti binary: %s\n", usr_bin_path);
    exit(EXIT_FAILURE);
  }

  if (TRUE == dump_mibib_partition)
  {
    if ((dbl_file=fopen(dbl_bin_path, "rb")) == NULL)
    {
      printf("\nCould not open DBL binary: %s\n", dbl_bin_path);
      exit(EXIT_FAILURE);
    }
  }

  if ((mibib_file=fopen(mibib_bin_path, "wb")) == NULL)
  {
    printf("\nCould not open MIBIB data binary: %s\n", mibib_bin_path);
    exit(EXIT_FAILURE);
  }

  if (TRUE == dump_log_file)
  {
    if ((log_file=fopen(log_txt_path, "w")) == NULL)
    {
      printf("\nCould not open log file: %s\n", log_txt_path);
      exit(EXIT_FAILURE);
    }
  }

  if (device_name_known != TRUE)
  {
    /* Device geometry paramaters were provided as command line paramaters.
     * Populate the device descriptor with dummy functions and pctl struucture
     * with appropriate data. */

    mibib.flash_info.partition.page_size_bytes = page_size;
    mibib.flash_info.partition.total_page_size_bytes = page_size + (page_size >> 5);
    mibib.flash_info.partition.pages_per_block = block_size;
    mibib.flash_info.partition.block_count = dev_size;

    if (mibib.flash_info.partition.total_page_size_bytes > TOTAL_PAGE_SIZE)
    {
      printf("Tool configured to handle max total page size of %d bytes!!",
             (int) TOTAL_PAGE_SIZE);
      exit(EXIT_FAILURE);
    }

    if (mibib.flash_info.partition.pages_per_block > MAX_BLOCK_SIZE)
    {
      printf("Tool configured to handle max block size of %d pages!!",
             (int) MAX_BLOCK_SIZE);
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    mibib.flash_info.partition.page_size_bytes = 0;
    mibib.flash_info.partition.total_page_size_bytes = 0;
    mibib.flash_info.partition.pages_per_block = 0;
    mibib.flash_info.partition.block_count = 0;
  }

  // mibib.nand = &ram_device;

  flash_hw_present = FALSE;

  /* Initiate the sequence used by JTAG prorammer when programming the build.
   * If this sequence is not identical to the one used by programmer, the
   * results could be quite different */

  /* Set security mode for the programmer */
  prog_param.status = FLASH_PROG_SECURITY_MODE;
  prog_param.reserved = MI_BOOT_SEC_MODE_NON_TRUSTED;

  main_c();

  /* return error code back to TRACE32, 0 = success, 0x100 = fail */
  if (prog_param.status == FLASH_PROG_FAIL)
  {
    printf("\nSecurity mode cannot be set\n");
    jnand_extended_error = (unsigned int) prog_param.reserved;
    printf("Extended error code 0x%x\n", jnand_extended_error);
    exit(EXIT_FAILURE);
  }

  /* Read partition table data from the file in to a buffer where the
   * conversion routine can manipulate it.
   */
  count = 0;

  while (!feof(usr_parti_file))
  {
    ch = getc(usr_parti_file);
    if (ferror(usr_parti_file))
    {
       printf("\nFailure reading data from file %s\n", usr_parti_fname);
    }
    if(!feof(usr_parti_file))
    {
       *buf_ptr = ch;
       ++buf_ptr;
       count++;
       if (count >= FLASH_PROG_BUFFER_SIZE)
       {
         printf("\nBuffer overflow while reading file %s\n", usr_parti_fname);
       }
    }
    else
    {
       printf("\n%d bytes of file %s read successfully\n", count,
                                                           usr_parti_fname);
       fclose(usr_parti_file);
       break;
    }
  }

  /* Send partition table over to the flash programmer */
  prog_param.status = FLASH_PROG_USR_PARTI_TBL;
  prog_param.size =  count;
  prog_param.width = FALSE;
  prog_param.reserved = FALSE;  /* No override */

  main_c();

  /* return error code back to TRACE32, 0 = success, 0x100 = fail */
  if (prog_param.status == FLASH_PROG_FAIL)
  {
    printf("\nUser Partition Table initialization failed\n");
    jnand_extended_error = (unsigned int) prog_param.reserved;
    printf("Extended error code 0x%x\n", jnand_extended_error);
    exit(EXIT_FAILURE);
  }

  if (TRUE == dump_mibib_partition)
  {
    char * wr_ptr;

    /* Read data from DBL binary and write it into emulated flash device */

    /* Read partition table data from the file in to a buffer from
     * where flash driver routine can write it to RAM flash device.
     */
    count = 0;
    page = 0;
    blk = boot_layout.dbl_block;
    buf_ptr = (char *) prog_param.data;
    wr_ptr = buf_ptr;

    blk_vector.start_block = blk;
    blk_vector.block_count = 1;
    if (DAL_SUCCESS != flash_erase_blocks(mibib.flash_handle, &blk_vector, 1))
    {
      printf("Cannot erase block %d on flash device.\n", blk);
      exit(EXIT_FAILURE);
    }

    while (!feof(dbl_file))
    {
      ch = getc(dbl_file);
      if (ferror(dbl_file))
      {
        printf("\nFailure reading DBL binary\n");
      }
      if(!feof(dbl_file))
      {
         *buf_ptr = ch;
         ++buf_ptr;
         count++;
         if (page >= ((blk + 1) * mibib.flash_info.partition.pages_per_block))
         {
           printf("PBL data cannot be accomodated in a block\n");
           exit(EXIT_FAILURE);
         }
         if (0 == (count % mibib.flash_info.partition.page_size_bytes))
         {
           //printf("\n\npbl write, page: %d count: %d\n", page, count);
           //printf("\n\npbl write, b_p: 0x%x w_p: 0x%x\n", buf_ptr, wr_ptr);
           if (FLASH_DEVICE_DONE != flash_prog_write_page(mibib.flash_handle,
             &mibib.flash_info,page,FLASH_WRITE_MAIN,wr_ptr))
           {
             printf("Failed writing page %d in block %d\n", page, blk);
             exit(EXIT_FAILURE);
           }
           page++;
           buf_ptr = wr_ptr;
         }
      }
      else
      {
        unsigned int bytes_left;

        /* There could be a partial page left to be written. If there is one,
         * write it to the flash. To keep the result deterministic initilalize
         * remaining bytes in the buffer with '0xFF'. */
        bytes_left = count % mibib.flash_info.partition.page_size_bytes;

        if (bytes_left != 0)
        {
          memset(&wr_ptr[bytes_left], 0xFF, (mibib.flash_info.partition.page_size_bytes - bytes_left));

          //printf("\n\npbl write, page: %d count: %d\n", page, count);
          if (FLASH_DEVICE_DONE != flash_prog_write_page(mibib.flash_handle,
            &mibib.flash_info,page,FLASH_WRITE_MAIN,wr_ptr))
          {
            printf("Failed writing page %d in block %d\n", page, blk);
            exit(EXIT_FAILURE);
          }
        }

        printf("\n%d bytes of file %s read successfully\n", count, dbl_fname);
        fclose(dbl_file);
        break;
      }
    } //while
  }

  /* Initialize bad block table block by erasing it */
  {
    blk = boot_layout.bbtb_block;
    /* erase the block */
    blk_vector.start_block = blk;
    if (DAL_SUCCESS != flash_erase_blocks(mibib.flash_handle, &blk_vector, 1))
    {
      printf("Cannot erase block %d on flash device.\n", blk);
      exit(EXIT_FAILURE);
    }

    printf("\nBBT erase complete\n");
  }

  dump_start_block = boot_layout.dbl_block;
  dump_end_block = MI_BLOCK_LAST;

  page = mibib.flash_info.partition.pages_per_block;
  count = mibib.flash_info.partition.total_page_size_bytes;

  /* Write the decoding key in spare area of first page of the dump file to
   * help decoding of the file.
   */
  buf_ptr = &(ram_blocks[dump_start_block][0][mibib.page_size]);

  i = mibib.flash_info.partition.page_size_bytes;
  log_page_size = 1;

  while(i > 2)
  {
    log_page_size++;
    i = i >> 1;
  }

  i = mibib.flash_info.partition.pages_per_block;
  log_block_size = 1;

  while(i > 2)
  {
    log_block_size++;
    i = i >> 1;
  }

  i = mibib.flash_info.partition.block_count;
  log_device_size = 1;

  while(i > 2)
  {
    log_device_size++;
    i = i >> 1;
  }

  /* First location in the spare area is already used by written/erased flag.
   * Start with the next available location */
  buf_ptr[1] = (char) NAND_MIBIB_MBN_DECODE_KEY_BEGIN;
  buf_ptr[2] = (char) NAND_MIBIB_MBN_DECODE_KEY_VERSION;
  buf_ptr[3] = log_page_size;
  buf_ptr[4] = log_block_size;
  buf_ptr[5] = log_device_size;
  buf_ptr[6] = dump_end_block - dump_start_block;
  buf_ptr[7] = (char) NAND_MIBIB_MBN_DECODE_KEY_END;

  printf("\nGenerating a file with MIBIB partition dump: \n");

  for (blk = dump_start_block; blk < dump_end_block; blk++)
  {
    //printf("dumping block %d\n", blk);
    for (i = 0; i < page; i++)
    {
      if(fwrite(&ram_blocks[blk][i][0], sizeof(char), count, mibib_file) !=
         count)
      {
        printf("\nFailure dumping MIBIB page %d to file.\n", i);
      }
    }
    //printf("Total Bytes written: %d for block %d\n", (i * count), blk);
  }

  printf("\n%d bytes from %d blocks dumped\n", (i * count * blk), blk);

  exit(EXIT_SUCCESS);
}
