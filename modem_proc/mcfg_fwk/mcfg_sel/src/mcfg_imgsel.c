/*==============================================================================

            M O D E M   C O N F I G   I M A G E   S E L E C T I O N

GENERAL DESCRIPTION
  Contains the logic to automatically select a image file based on
  the carrier image mapping table.

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_sel/src/mcfg_imgsel.c#1 $
$DateTime: 2016/12/13 08:00:27 $
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
01/07/12 cy   Created module

==============================================================================*/

#include "comdef.h"

#include "fs_errno.h"
#include "fs_public.h"

#include "mcfg_common.h"
#include "mcfg_osal.h"
#include "mcfg_int.h"
#include "mcfg_utils.h"
#include "mcfg_trl.h"

#include "mcfg_imgsel.h"
#include "qmi_pdc_svc.h"

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
#define MCFG_BOOT_MAP_CSV_PATH          "/nv/item_files/mcfg/mcfg_boot_map.csv"
#define MCFG_BOOT_MAP_CSV_LINE_MAXCHAR  (200)
#define MCFG_BOOT_PARTITION_ID_SIZE_MAX (20)
#define MCFG_BOOT_PARTITION_NUMS        (3)

#ifdef Q6SW_EFS_PATH
  #define MCFG_BOOT_EFS_PATH_DSP1   Q6SW_EFS_PATH
#else
  #define MCFG_BOOT_EFS_PATH_DSP1   "/q6fw.txt"
#endif

#ifdef Q6FW_EFS_PATH
  #define MCFG_BOOT_EFS_PATH_DSP2   Q6FW_EFS_PATH
#else
  #define MCFG_BOOT_EFS_PATH_DSP2   "/q6sw.txt"
#endif

#ifdef LPASS_EFS_PATH
  #define MCFG_BOOT_EFS_PATH_LPASS  LPASS_EFS_PATH
#else
  #define MCFG_BOOT_EFS_PATH_LPASS  "/lpass.txt"
#endif

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================

                 PRIVATE FUNCTION FORWARD DECLARATIONS

==============================================================================*/

static boolean mcfg_imgsel_csv_read_one_line
(
  int       fd,
  fs_off_t *offset,
  char     *line
);

static boolean mcfg_imgsel_update_boot_efs
(
  char *path,
  char *image
);

static boolean mcfg_imgsel_update_partition_by_carrier
(
   char *carrier_name
);

/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/


/*===========================================================================

  FUNCTION mcfg_imgsel_csv_read_one_line

  DESCRIPTION
    Read one line from efs file from its offset, and update offset for
    next line

  DEPENDENCIES
    None

  PARAMETERS
    fd      [in]
    offset  [in/out]
    line    [in]

  RETURN VALUE
    TRUE if successful, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_imgsel_csv_read_one_line
(
  int       fd,
  fs_off_t *offset,
  char     *line
)
{
  boolean status = FALSE;
  fs_ssize_t i=0, j=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(offset);
  MCFG_CHECK_NULL_PTR_RET_FALSE(line);

  MCFG_MSG_LOW("mcfg_imgsel_csv_read_one_line start");
  memset(line, 0, MCFG_BOOT_MAP_CSV_LINE_MAXCHAR);

  if(efs_lseek(fd, *offset, SEEK_SET) == *offset)
  {
    for (i=0; i<MCFG_BOOT_MAP_CSV_LINE_MAXCHAR; i++)
    {
      if (efs_read(fd, line+j, 1) <= 0)
      {
        line[j] = '\0';
        *offset += i+1;
        status = (j > 0);
        MCFG_MSG_HIGH_1("mcfg_imgsel_csv_read_one_line complete at offset %ld", 
                        *offset);
        break;
      }
      else if (line[j] =='\r' || line[j] =='\n' || line[j] =='\0')
      {
        if (j == 0)
        {
          MCFG_MSG_LOW("mcfg_imgsel_csv_read_one_line skip \\r & \\n");
        }
        else
        {
          line[j] = '\0';
          *offset += i+1;
          status = TRUE;
          MCFG_MSG_HIGH_1("mcfg_imgsel_csv_read_one_line complete at offset %ld", 
                          *offset);
          break;
        }
      }
      else if (line[j] == ' ' || line[j] =='\t')
      {
        MCFG_MSG_LOW("mcfg_imgsel_csv_read_one_line skip space and tab");
      }
      else
      {
        //MCFG_MSG_LOW_2("mcfg_imgsel_csv_read_one_line %x %c", line[j], line[j]);
        j++;
      }
    }
  }

  return status;
} /* mcfg_imgsel_csv_read_one_line() */

/*===========================================================================

  FUNCTION mcfg_imgsel_switch_images_by_config

  DESCRIPTION
    Switch to new images for boot to load based on the new active config. 

  DEPENDENCIES
    None

  PARAMETERS
    config_id [in]

  RETURN VALUE
    TRUE if image switched is necessary and successful,
    FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_imgsel_switch_images_by_config
(
  mcfg_config_id_s_type *config_id
)
{
  char carrier_name[MCFG_TRL_CARRIER_NAME_MAX_LEN+1];
  mcfg_config_info_s_type config_info;
  boolean status = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_id);

  MCFG_MSG_LOW_3("Image switching picked config with ID %02X%02X%02X",
                  config_id->id[0], config_id->id[1], config_id->id[2]);

  if (!mcfg_utils_get_config_info(MCFG_TYPE_SW, config_id, &config_info))
  {
    MCFG_MSG_ERROR("Couldn't get config info, skip image switching!");
  }
  else if (!mcfg_trl_decode_tlv(&config_info, MCFG_TRL_TLV_ID_CARRIER_NAME, 
             carrier_name, MCFG_TRL_CARRIER_NAME_MAX_LEN+1))
  {
    MCFG_MSG_ERROR("Couldn't get carrier name, skip image switching!");
  }
  else if (!mcfg_imgsel_update_partition_by_carrier(carrier_name))
  {
    MCFG_MSG_LOW_3("Not able to update partitiion for carrier %c%c%c, skip!",
                     carrier_name[0], carrier_name[1], carrier_name[2]);
  }
  else
  {
    MCFG_MSG_HIGH_3("Switching to new images for carrier %c%c%c",
                    carrier_name[0], carrier_name[1], carrier_name[2]);
    status = TRUE;
  }

  return status;
} /* mcfg_imgsel_switch_images_by_config() */

/*===========================================================================

  FUNCTION mcfg_imgsel_update_boot_efs

  DESCRIPTION
    Update the boot efs file with the image/partition name.

  DEPENDENCIES
    None

  PARAMETERS
    path  [in]
    image [in]

  RETURN VALUE
    TRUE if successful, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_imgsel_update_boot_efs
(
  char *path,
  char *image
)
{
  fs_ssize_t bytes_written = 0, efs_ret, len;
  int fd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(path);
  MCFG_CHECK_NULL_PTR_RET_FALSE(image);

  fd = efs_open(path, O_CREAT | O_WRONLY); 
  if (fd < 0)
  {
    MCFG_MSG_ERROR_1("Unable to create/open BOOT EFS file (%d)",efs_errno);
    return FALSE;
  }

  len = strlen(image);
  do
  {
    efs_ret = efs_write(fd, image + bytes_written, len - bytes_written);
  } while (efs_ret > 0 && (bytes_written += efs_ret) < len);

  efs_ret = efs_close(fd); 
  return ((bytes_written == len ) && (efs_ret == 0));
} /* mcfg_imgsel_update_boot_efs() */

/*===========================================================================

  FUNCTION mcfg_imgsel_update_partition_by_carrier

  DESCRIPTION
    Update image partition by carrier name

  DEPENDENCIES
    None

  PARAMETERS
    carrier_name [in]

  RETURN VALUE
    TRUE if image partition update is necessary and successful,
    FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_imgsel_update_partition_by_carrier
(
  char *carrier_name
)
{
  boolean status = FALSE;
  boolean found = FALSE;
  char line[MCFG_BOOT_MAP_CSV_LINE_MAXCHAR];
  char name[MCFG_TRL_CARRIER_NAME_MAX_LEN+1];
  char image[MCFG_BOOT_PARTITION_NUMS][MCFG_BOOT_PARTITION_ID_SIZE_MAX];
  fs_off_t line_offset = 0;
  uint32 column_offset = 0, index;
  int fd;
  struct fs_stat stat;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(carrier_name);
  MCFG_CHECK_NULL_REFERENT_RET_FALSE(*carrier_name);

  /* Open the file in read only mode */
  fd = efs_open(MCFG_BOOT_MAP_CSV_PATH, O_RDONLY); 
  if (fd < 0)
  {
    MCFG_MSG_LOW("mcfg_imgsel_update_partition_by_carrier: MCFG carrier partition csv not present, skip");
    return FALSE;
  }

  if (efs_fstat(fd, &stat) < 0)
  {
    MCFG_MSG_ERROR("mcfg_imgsel_update_partition_by_carrier: Unable to fstat");
    efs_close(fd); 
    return FALSE;
  }

  while (!found && line_offset < stat.st_size &&
           mcfg_imgsel_csv_read_one_line(fd, &line_offset, line))
  {
    column_offset = 0;
    if (mcfg_utils_get_phrase(
          line, &column_offset, MCFG_TRL_CARRIER_NAME_MAX_LEN+1, name)
        && (strcasecmp(carrier_name, name) == 0))
    {
      found = TRUE;
      for (index = 0; index < MCFG_BOOT_PARTITION_NUMS; index++)
      {
        if (!mcfg_utils_get_phrase(
               line, &column_offset, MCFG_BOOT_PARTITION_ID_SIZE_MAX, image[index]))
        {
          MCFG_MSG_ERROR_4("mcfg_imgsel_update_partition_by_carrier: Unable to"
                           "parse image (%lu) for (%c%c%c)", 
                           index, name[0], name[1], name[2]);
          break;
        }
      }

      if (index == MCFG_BOOT_PARTITION_NUMS)
      {
        if (mcfg_imgsel_update_boot_efs(MCFG_BOOT_EFS_PATH_DSP1, image[0]) &&
              mcfg_imgsel_update_boot_efs(MCFG_BOOT_EFS_PATH_DSP2, image[1]) &&
              mcfg_imgsel_update_boot_efs(MCFG_BOOT_EFS_PATH_LPASS, image[2]))
        {
          status = TRUE;
        }
        else
        {
          MCFG_MSG_ERROR("mcfg_imgsel_update_partition_by_carrier: remove"
                         "boot efs files and fall back to default");
          efs_unlink(MCFG_BOOT_EFS_PATH_DSP1);
          efs_unlink(MCFG_BOOT_EFS_PATH_DSP2);
          efs_unlink(MCFG_BOOT_EFS_PATH_LPASS);
        }
      }
    }
  }

  efs_close(fd); 
  return status;
} /* mcfg_imgsel_update_partition_by_carrier() */


