/*==================================================================
 *
 * FILE:        hostdl_gen.c
 *
 * SERVICES:    A I F 2 H E X   Postprocessor tool
 *
 * DESCRIPTION:
 *   This version of the tool is for hostdl being built with ADS1.01 or
 *   later tools.
 *
 *   This program must be compiled with Visual C++ 6.0 or some other non-ARM
 *   compiler suite.  It generates hostdl_gen.exe which is checked into the
 *   archive so this hopefully will not need to be compiled again.  It is
 *   NEVER built with the ARM tools.
 *
 *   This program post-processes the output of the AIF2HEX tool supplied by
 *   ARM Ltd.  It is responsible for concatenating the different ROM region
 *   output files, and then appends the start address given as a command-line
 *   paramter parameter.  This start address is used as the jump point for
 *   downloaded applications to be executed out of RAM.  This program is
 *   specific to the Intel MCS-86 file format.
 *
 *        Copyright © 1998, 2008-2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *            QUALCOMM Proprietary/GTDR
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl_gen.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 10/29/08     mm      Clean up
 * 09/23/98     hcg     Created.
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <direct.h>


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define FALSE 0
#define TRUE  1
#define ADDR 3
#define LEN  4
#define MAX_FILES 20
#define BUFSIZE 80
#define MAX_PATH_LENGTH 100

/*
 * Define this if you want the original behavior of writing two files.
 * Writing the file that is named based on the directory name will
 * fail on Windows 9x, NT and 2000 if the directory name is longer
 * than 8.3 DOS naming convention.  Also, the second file, the one
 * made from the directory name is never used for anything at all,
 * so there is no need to ever write it.
 */
#undef  WRITE_TWO_FILES

/* generic type definitions as in comdef.h */
typedef char byte;
typedef unsigned int word;
typedef unsigned long dword;

/* globals for checking command line usage */
int t_flag = FALSE;
int s_flag = FALSE;
int f_flag = FALSE;

/*===========================================================================

  Convert a string of ASCII hex to a dword.

===========================================================================*/
dword hex_to_long
(
  char *s_ptr   /* string of ASCII hexadecimal digits */
)
{
  dword   i;
  char   *p, c;

  /* Loop over the input bytes until the NULL byte is reached.
  */
  for (i = 0, p = s_ptr; (c = *p) != '\0'; p++) {
    i <<= 4;                                    /* shift left 1 hex digit */
    if (c <= '9') {
      i += (byte) (c - '0');                    /* 0..9 */
    } else {
    if (c == 'x' || c == 'X')
      continue;
      i += (byte) ((c & ~0x20) - 'A' + 10);     /* mask lower/upper, A..F */
    }
  }

  return(i);    /* Return the result */

} /* end of hex_to_long */

/*===========================================================================

  Parses command line for command line arguments and sets up corresponding
  internal flags.

  Formats:
      -target ................. target name
      -start  ................. start address
      -<n> <rom hex name> ..... number of rom area, rom name

      e.g.

      gen -t panther.hex -s 0x0 -f ROM.hex ROM_2.hex ... ROM_n.hex

===========================================================================*/
void parse_command_line(int argc, char *argv[], char **t_name,
            long *s_addr, int *n_files,
            char **r_name)
{
  int i, j;
  int file_no;

  i=1;
  file_no=0;
  while (i<argc)
    {
      if (argv[i][0] == '-')
  {
    /* target name */
    if ( (argv[i][1] == 't') || (argv[i][1] == 'T') )
      {
        *t_name = (char *) strdup(argv[i+1]);
        t_flag = TRUE;
      }

    /* start address */
    if ( (argv[i][1] == 's') || (argv[i][1] == 'S') )
      {
        *s_addr = hex_to_long(argv[i+1]);
        s_flag = TRUE;
      }

    /* ROM area names */
    if ( (argv[i][1] == 'f') || (argv[i][1] == 'F') )
      {
        j=i+1;
        while (argv[j][0] != '-')
    {
      f_flag = TRUE;
      r_name[file_no] = (char *) strdup(argv[j]);
      *n_files = ++file_no;
      j++;
      if (j==argc)
        break;
    }
      }
  }
      i++;
    }
}

/*===========================================================================

  Generate a checksum for an array of binary data.

===========================================================================*/
static byte gen_hex_checksum
(
  byte *buf_ptr,
  word addr,
  word len
)
{
  word i;
  byte checksum;

  /* The checksum is the sum of the records databytes
  */
  checksum = (byte) len;
  checksum += (byte)(addr & 255L);
  checksum += ((byte)(addr >>  8) & 0x00ff);

  /* Sum in the array of data bytes in the data section of the record
  */
  for (i = 0; i < len; i++)
    checksum += buf_ptr[i];

  checksum = (byte) (256 - checksum) & 0xFF;   /* The sum causes a roll to 0 */

  return( checksum );

} /* end of gen_hex_checksum */

/*===========================================================================

  Prints informative message when invalid command line options are passed to
  the program.

===========================================================================*/

void usage()
{
  printf("Hex file generation tool:\n");
  printf("Usage: gen -t <target file> -s <start hex address> -f <rom1> <rom2> ....\n");
  exit(0);
}

/*===========================================================================

  Entry point into program.  Manages command line parsing, file concatenation,
  as well as appending start record to end of the concatenated file.

===========================================================================*/
void main (int argc, char *argv[])
{
  dword start_address;
  byte chk;

  int  file_no;
  int  num_files=0;

  char buf[BUFSIZE];
  char *start_pattern;
  char *end_pattern;
  char *separator = {"\\"};
  char *targetname;
  char *romname[MAX_FILES];

  FILE *hexfile;
  FILE *romfile[MAX_FILES];


  /* read in the target name, start address, and the ROM area names from command line */
  parse_command_line(argc, argv, &targetname,
         &start_address, &num_files,
         romname);

  /* not all the flags are set - print helpful message */
  if (!t_flag || !s_flag || !f_flag)
    usage();

  /* the PST requires a start address */
  chk = gen_hex_checksum( (byte *) &start_address, ADDR, LEN);

  /* now open the files and begin concatenating */

  /* target file */
  if ( (hexfile = fopen(targetname, "w")) == NULL)
    {
      fprintf(stderr, "Can't open %s for writing\n", targetname);
      perror("Program Failed.");
    }

  /* rom area files generated by AIF2HEX tool */
  for (file_no=0; file_no<num_files; file_no++)
    {
    if ( (romfile[file_no] = fopen(romname[file_no], "r")) == NULL)
      {
      fprintf(stderr, "Can't open %s for reading\n", romname[file_no]);
      perror("Program Failed.");
    }
  }

  /*
   Intel-86 Hex format:

   :02000002xxxxSS
      Extended address record, xxxx = upper segment base address,
      SS is a checksum.

   :ccAAAA00dd..ddSS
      Data record, cc = byte count, aaaa = load address,
      dd..dd = data, SS is a checksum.

   :04000003xxxxyyyyss
      Start address record, xxxx = CS, yyyy = IP, SS is a checksum.

   :00000001FF
      End-of-file, FF is checksum.
  */

  start_pattern = (char *) strdup(":02000002");
  end_pattern = (char *) strdup(":00000001FF\n");

  /* read from each file - Don't include the final record - append it manually */
  for (file_no=0; file_no<num_files; file_no++)
    {
      while (fgets(buf, BUFSIZE, romfile[file_no]) != NULL)
        {
      /* found an END record - Don't include it */
        if (!strcmp(buf, end_pattern))
          continue;

      /* print record */
        fprintf(hexfile, "%s", buf);
      }
  }

  /* close all the files */
  for (file_no=0; file_no<num_files; file_no++)
    fclose(romfile[file_no]);

  /* now remove them */
  for (file_no=0; file_no<num_files; file_no++)
    remove(romname[file_no]);

  /* now append start address record */
  fprintf(hexfile, ":04000003%08lX%02X\n",
    start_address, chk & 0xFF);

  /* and finally, the END record */
  fprintf(hexfile, "%s", end_pattern);

  fclose(hexfile);

  exit(0);
}
