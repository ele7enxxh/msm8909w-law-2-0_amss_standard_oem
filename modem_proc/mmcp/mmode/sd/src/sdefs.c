/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

                       SD EFS  I N T E R F A C E

GENERAL DESCRIPTION
  This file makes up the EFS-Interface component of the SD.

  The SD EFS-Interface component encapsulates all the functionality that is
  associated with reading and writing from/to EFS SD related items, such as
  MRU table.

EXTERNALIZED FUNCTIONS
  sdefs_write
  sdefs_read

REGIONAL FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  sdefs_init() must be call to initialize this component before any other
  function declared in this component is called.


Copyright (c) 2009 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdefs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/13   dj      New EFS Item added for 1XCSFB_ECBM feature.
10/09/12   sr      Support  to derive MCC of an EvDo system from eqprl.
08/17/12   gm      Mainline EFS feature - Remove FEATURE_EFS
07/17/12   jh      Added LTE Available File BSR support
05/11/12   jh      Fix SDEFS regular files/item files read handling
03/23/12   gm      DIME integration fix: TMC deprecation and others.
16/02/12   vk      Replace usage of AEE library function with corresponding
                   CoreBSP library functions
05/17/11   rn      WCDMA channel locking feature
04/22/11   cl      Fix how SD handles EFS_WRITE function
03/10/11   gm      Add QXDM NV Browser support for ECBM timer.
02/16/11   gm      Extend acquisition while in ECBM & sys loss
02/10/11   am      Adding QTF support.
02/10/11   xs      Fix JCDMA NV reading issue
02/07/11   xs      Dynamic FEATURE_JCDMA control in EFS
07/19/10   aj      Update sdefs_write() to truncate existing filesize
                   to 0 before writing new values.
07/14/10   mh      Fixed compile warnings
06/28/10   ay      Added EOOS
06/18/10   ay      Added EFS based MRU
06/18/10   aj      add validation for mapping function
05/18/10   rn      Added rat_pri_list_info in get_net_gw
05/12/10   aj      Add support to save equivalent PRL in EFS
03/22/10   aj      updated default TOT order
02/17/10   rn      Get UE_cap from EFS
02/10/09   rn      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "sd.h"       /* External interface to sd.c */
#include "sd_v.h"
#include "sddbg.h"    /* SD debug macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "sdefs.h"    /* SD EFS interface */
#include "mcfg_fs.h"    /* Interface to MCFG efs wrappers */

#include <stringl/stringl.h>

#include "sdss.h"
#include "sdssscr.h"

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* Structure for EFS table
*/
typedef struct sdefs_tbl_s
{
   sdefs_id_e_type            efs_id;       /* EFS id */
   const char                *file_name;    /* EFS file name */
} sdefs_tbl_s_type;


/*
** EFS table that maps EFS Id to filename
*/
static const sdefs_tbl_s_type sdefs_tbl[] =
{
   {SDEFS_MRU_NAM_1,           SDEFS_MMODE_MRU_NAM_1_F_NAME} ,
   {SDEFS_MRU_NAM_2,           SDEFS_MMODE_MRU_NAM_2_F_NAME} ,
   {SDEFS_MMSS_TIMER,          SDEFS_MMSS_TIMER_F_NAME     } ,
   {SDEFS_BST_TBL,             SDEFS_BST_TBL_F_NAME        } ,
   {SDEFS_MMSS_EQPRL,          SDEFS_MMSS_EQPRL_F_NAME     } ,
   {SDEFS_MMSS_IGNORE_UPLMN,   SDEFS_MMSS_IGNORE_UPLMN_F_NAME },
   {SDEFS_FEATURE_JCDMA,       SDEFS_FEATURE_FLAG_CFG_F_NAME },
   {SDEFS_SDSSSCR_TIMERS,      SDEFS_SDSSSCR_TIMERS_F_NAME } ,
   {SDEFS_LTE_AVAIL_FILE,      SDEFS_LTE_AVAIL_FILE_F_NAME } ,
   {SDEFS_LTE_NOT_AVAIL_FILE,  SDEFS_LTE_NOT_AVAIL_FILE_F_NAME } ,
   {SDEFS_WCDMA_FREQ_LOCK,     SDEFS_WCDMA_FREQ_LOCK_F_NAME} ,
   {SDEFS_LBSR_MCC_LIST,       SDEFS_LBSR_MCC_LIST_F_NAME  } ,
   {SDEFS_FULL_SRV_REQ_EMERG,  SDEFS_FULL_SRV_REQ_EMERG_F_NAME },
   {SDEFS_QMSS_ENABLED,        SDEFS_QMSS_ENABLED_F_NAME   },
   {SDEFS_OPERATOR_NAME,       SDEFS_OPERATOR_NAME_F_NAME  },
   {SDEFS_1XCSFB_ECBM,         SDEFS_1XCSFB_ECBM_F_NAME    },
   {SDEFS_CUSTOM_EMERG_INFO,   SDEFS_CUSTOM_EMERG_INFO_F_NAME},
   {SDEFS_LOCK_SID_NID,        SDEFS_LOCK_SID_NID_F_NAME   },
   {SDEFS_SCAN_SCOPE_RULE,     SDEFS_SCAN_SCOPE_RULE_F_NAME  },
   {SDEFS_MANUAL_SEARCH_IN_WRLF,  SDEFS_MANUAL_SEARCH_IN_WRLF_F_NAME},
   {SDEFS_CSG_AUTO_MODE,       SDEFS_CSG_AUTO_MODE_NAME },
   {SDEFS_EHRPD_OPT_REDIAL,    SDEFS_EHRPD_OPT_REDIAL_F_NAME },
   {SDEFS_C2K_RESEL_SPLMN,  SDEFS_C2K_RESEL_SPLMN_F_NAME},
   {SDEFS_BUFFER_INT_SRV_LOST,    SDEFS_BUFFER_INT_SRV_LOST_F_NAME},
   {SDEFS_MANUAL_SRCH_BST_TBL, SDEFS_MANUAL_SRCH_BST_TBL_F_NAME},
   {SDEFS_1XCSFB_CALL_END_OPT, SDEFS_1XCSFB_CALL_END_OPT_F_NAME },
   {SDEFS_RECENTLY_ACQUIRED_LIST, SDEFS_RECENTLY_ACQUIRED_LIST_F_NAME},
   {SDEFS_EMERGENCY_RAT_ACQ_ORDER, SDEFS_EMERGENCY_RAT_ACQ_ORDER_F_NAME},
   {SDEFS_EXTEND_LBSR_1x_ACTIVE,   SDEFS_EXTEND_LBSR_1x_ACTIVE_F_NAME},
   {SDEFS_ROAM_IND_UPDATE_IN_L_TO_1X_HO,  SDEFS_ROAM_IND_UPDATE_IN_L_TO_1X_HO_F_NAME},
   {SDEFS_1XSIB8_SCAN_OPT,     SDEFS_1XSIB8_SCAN_OPT_F_NAME},
   {SDEFS_EMERG_CALL_AFTER_LU_IN_LPM, SDEFS_EMERG_CALL_AFTER_LU_IN_LPM_F_NAME},
   {SDEFS_EMERG_CALL_1X_SCAN_FIRST, SDEFS_EMERG_CALL_1X_SCAN_FIRST_F_NAME},
   {SDEFS_CUSTOM_VOWIFI_CONFIG, SDEFS_CUSTOM_VOWIFI_CONFIG_F_NAME},
   {SDEFS_INT_MAX,             SDEFS_NO_FILE               } ,
   {SDEFS_MAX,                 SDEFS_NO_FILE               }
};

#ifdef FEATURE_EOOS
    #error code not present
#endif
/* WCDMA channel locking status */
static boolean is_wcdma_freq_lock_efs_read_done[MAX_SIMS] = {FALSE};

/*===========================================================================

FUNCTION sdefs_map_efs_id_to_filename

DESCRIPTION
  Map EFS id to EFS filename


DEPENDENCIES
  None

RETURN VALUE
  Return TRUE if mapping was successful, else FALSE

SIDE EFFECTS
  None.

===========================================================================*/

static boolean                  sdefs_map_efs_id_to_filename(

        sdefs_id_e_type          efs_id,
            /* passing the efs Id to map.
            */

        char                     *file_name
            /* Pointer to file name.
            */

)
{
  int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT(file_name != NULL);
  SD_ASSERT_ENUM_IS_INRANGE(efs_id,SDEFS_MAX);
  SD_ASSERT( efs_id != SDEFS_NONE &&
             efs_id != SDEFS_MAX  &&
             efs_id != SDEFS_INT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(i = 0; i < ARR_SIZE(sdefs_tbl); i++)
  {
     if(sdefs_tbl[i].efs_id == efs_id)
     {
        (void)strlcpy(file_name,
                           sdefs_tbl[i].file_name,
                           SDEFS_FILENAME_MAX_LEN);
        return TRUE;

     }

     if(sdefs_tbl[i].efs_id == SDEFS_MAX)
     {
       break ;
     }
  }

  SD_MSG_HIGH_0("No matching EFS id found");

  /* efs id not found */
  return FALSE;

}

#define EFS_DIRECTORY_SEPARATOR '/'

/*===========================================================================

FUNCTION sdefs_create_path

DESCRIPTION
  This function creates directory path in EFS.

DEPENDENCIES
  None

RETURN VALUE
  -1 if error, anything else is success.

SIDE EFFECTS
  None.

===========================================================================*/

static int sdefs_create_path(const char *dir_path)
{
  int result = -1;
  int path_i = 0;
  int dir_i = 0;
  char dir_name[SDEFS_FILENAME_MAX_LEN];

  memset(dir_name, 0, SDEFS_FILENAME_MAX_LEN);

  if(dir_path[path_i] != EFS_DIRECTORY_SEPARATOR)
  {
    SD_MSG_HIGH_0("Error, provide valid absolute path");
    return result;
  }

  dir_name[dir_i++] = dir_path[path_i++];

  while(dir_path[path_i] != '\0')
  {
    if(dir_path[path_i] == EFS_DIRECTORY_SEPARATOR)
    {
      result = mcfg_mkdir(dir_name,0777,MCFG_FS_TYPE_EFS);
    }
    dir_name[dir_i++] = dir_path[path_i++];
  }
  result = mcfg_mkdir(dir_name,0777,MCFG_FS_TYPE_EFS);

  return result;
}

/*===========================================================================

FUNCTION sdefs_map_sub_id_to_mcfg_id

DESCRIPTION
  Map modem asubs_id to mcfg id

DEPENDENCIES
  None

RETURN VALUE
  mcfg id

SIDE EFFECTS
  None

===========================================================================*/
static mcfg_fs_sub_id_e_type sdefs_map_as_id_to_mcfg_id(
  sys_modem_as_id_e_type    asubs_id
)
{
  mcfg_fs_sub_id_e_type  mcfg_subs;

  switch (asubs_id)
  {
    case SYS_MODEM_AS_ID_3:
      mcfg_subs = MCFG_FS_SUBID_2;
      break;

    case SYS_MODEM_AS_ID_2:
      mcfg_subs = MCFG_FS_SUBID_1;
      break;

    case SYS_MODEM_AS_ID_NONE:
      mcfg_subs = MCFG_FS_SUBID_NONE;
      break;

    case SYS_MODEM_AS_ID_1:
    default:
      mcfg_subs = MCFG_FS_SUBID_0;
      break;
  }

  return mcfg_subs;
}

/*===========================================================================

FUNCTION sdefs_get

DESCRIPTION
Get EFS item denoted by efs_id and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

==========================================================================*/
boolean                  sdefs_get(

        sdefs_id_e_type          efs_id,
            /* passing the efs Id to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    size
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
)
{
  char          filename[SDEFS_FILENAME_MAX_LEN];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( data_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if(!sdefs_map_efs_id_to_filename(efs_id, filename))
   {
      SD_ERR_0("EFS file map failed");
      return FALSE;
   }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sdefs_get2( filename, data_ptr, size );
}

/*===========================================================================

FUNCTION sdefs_get2

DESCRIPTION
Get EFS item denoted by filename and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful and size read matches the size passed in,
  FALSE otherwise.

SIDE EFFECTS
  None.

==========================================================================*/
boolean                  sdefs_get2(

        const char*              filename,
            /* passing the filename to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    size
            /* The  maximum number of bytes to be copied into the buffer
            ** pointed by data_ptr.
  */
)
{
  return sdefs_get_per_subs(filename, data_ptr, size, SYS_MODEM_AS_ID_1);
}

/*===========================================================================

FUNCTION sdefs_get_per_subs

DESCRIPTION
  Get EFS item denoted by filename from given subscription and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful and size read matches the size passed in,
  FALSE otherwise.

SIDE EFFECTS
  None.

==========================================================================*/
boolean                  sdefs_get_per_subs(

        const char*              filename,
            /* passing the filename to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    size,
            /* The  maximum number of bytes to be copied into the buffer
            ** pointed by data_ptr.
            */
        sys_modem_as_id_e_type  asubs_id
            /* subscription to read the EFS file from */
)
{
  mcfg_fs_status_e_type status;

  if (NULL == filename)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get data from file.
  */
  status = mcfg_fs_read (filename, 
                         data_ptr, 
                         size, 
                         MCFG_FS_TYPE_EFS, 
                         sdefs_map_as_id_to_mcfg_id(asubs_id) );

  if(status != MCFG_FS_STATUS_OK)
  {
    SD_MSG_HIGH_2("mcfg fs read error status %d, errno %d", status, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

}

/*===========================================================================

FUNCTION sdefs_nv_refresh

DESCRIPTION
  Initialize SD EFS items.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void sdefs_nv_refresh()
{
  int32 efs_size = sizeof(jcdma_me_capability);
  if(!sdefs_read(SDEFS_FEATURE_JCDMA, (byte*)sd_jcdma_ptr(), &efs_size))
  {
    sd_jcdma_ptr()->service = JCDMA_SERVICE_DISABLED;
  }

  SD_MSG_HIGH_1("NV JCDMA is %d.",sd_jcdma_ptr()->service);
}
/*===========================================================================

FUNCTION sdefs_init

DESCRIPTION
  Initialize SD EFS items.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void sdefs_init()
{
  int result = -1;
  uint8 i;

  #ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE
  int efs_id;
  char file_name[SDEFS_FILENAME_MAX_LEN];
  struct fs_stat file_stat;
  int fd;
  #endif

  sdefs_nv_refresh();

  /* Reset the flag with every Init */
  for(i=0; i<MAX_SIMS; i++)
  {
    is_wcdma_freq_lock_efs_read_done[i] = FALSE;
  }

  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  result = sdefs_create_path("/sd");
  if(result == -1)
  {
      SD_MSG_HIGH_1("Error to create /sd %d",mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return;
  }
  result = sdefs_create_path("/nv/item_files/modem/mmode/sd");
  if(result == -1)
  {
      SD_MSG_HIGH_1("Error to create sdssscr_timers error %d",mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return;
  }

  return;
}

/*===========================================================================

FUNCTION sdefs_fs_read_per_subs

DESCRIPTION
Read EFS item denoted by efs_id and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

==========================================================================*/
boolean                  sdefs_fs_read_per_subs(

        sdefs_id_e_type          efs_id,
            /* passing the efs Id to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    *size_ptr,
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
        sys_modem_as_id_e_type   sub_id
            /* A different EFS file is read based on the sub_id 
            */
)
{
  char                  file[SDEFS_FILENAME_MAX_LEN];
  struct fs_stat          st;
  mcfg_fs_status_e_type   mcfg_status;
  mcfg_fs_sub_id_e_type   mcfg_subs = sdefs_map_as_id_to_mcfg_id(sub_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( data_ptr != NULL );
  SD_ASSERT( size_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!sdefs_map_efs_id_to_filename(efs_id, file))
  {
     return FALSE;
  }

 
   memset(&st, 0, sizeof(st));
 
   /* Get the size of the file.
   */
   mcfg_status = mcfg_fs_stat( file,
							   &st,
							   MCFG_FS_TYPE_EFS,
							   mcfg_subs );
 
   if (mcfg_status != MCFG_FS_STATUS_OK)
  {
	SD_MSG_HIGH_3("mcfg fs stat error sub_id %d status %d errno %d", sub_id, mcfg_status, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    *size_ptr = 0;
    return FALSE;
  }
 
  *size_ptr = st.st_size;
 
   /* Get data from file.
            */

  mcfg_status = mcfg_fs_read (file, 
                         data_ptr, 
                         *size_ptr, 
                         MCFG_FS_TYPE_EFS, 
                         mcfg_subs);

  SD_MSG_HIGH_3("reading file: %s using mcfg_fs_read, sub_id %d, status %d",
                  file, sub_id, mcfg_status);

  if(mcfg_status != MCFG_FS_STATUS_OK)
  {
    SD_MSG_HIGH_3("mcfg fs read error sub_id %d status %d errno %d", sub_id, mcfg_status, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    *size_ptr = 0;
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION sdefs_read

DESCRIPTION
Read EFS item denoted by efs_id and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

==========================================================================*/
boolean                  sdefs_read(

        sdefs_id_e_type          efs_id,
            /* passing the efs Id to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    *size_ptr
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
)
{
  int                         fd;
  char          file[SDEFS_FILENAME_MAX_LEN];
  int32                       file_size = 0;
  int32                       size_requested = 0;
  int32                       byte_read = 0;
  struct fs_stat              file_stat;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( data_ptr != NULL );

  /* cannot read this EFS due to null size_ptr */
  if(size_ptr == NULL)
  {
    return FALSE;
  }

  size_requested = *size_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if(!sdefs_map_efs_id_to_filename(efs_id, file))
   {
      SD_ERR_0("EFS file map failed");
      return FALSE;
   }

  /* If the file is NV ITEM Files, redirect to sdefs_read_item_files() */
  if(strncmp(SDEFS_ITEM_FILES_PREFIX,file,strlen(SDEFS_ITEM_FILES_PREFIX)) == FALSE)
  {
   return sdefs_fs_read_per_subs(efs_id, data_ptr,size_ptr,SYS_MODEM_AS_ID_1);
  }

  /* get the efs actual file size */
  if( mcfg_fs_stat(file, &file_stat, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0)
      == MCFG_FS_STATUS_OK )
  {
    file_size = (int32)file_stat.st_size;

    if (file_size <= 0)
    {
      SD_MSG_MED_1(" File size error: size %d", file_size);
      return FALSE;
    }

    SD_MSG_MED_2("file_size:%d, size_requested:%d", file_size, size_requested);
  }
  else
  {
    SD_ERR_2(" EFS_STAT ERROR: file-%d, errno:%d", efs_id, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /* Open the specified resource file.
  ** If open fails, return error code
  */
  SD_MSG_HIGH_2( "Open file=%s buffer size %d",
               file, *size_ptr);

  fd = mcfg_fopen( file, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0);

  if( fd < 0 )
  {
    SD_MSG_HIGH_2( "open efs_err=%d errno %d", fd, mcfg_fs_errno(MCFG_FS_TYPE_EFS));

    return FALSE;
  }

  /* The max size for one efs_read() operation dependes on the file system.
  ** To support a big size file read operation, we need to run efs_read until
  ** we read the file upto the size requested or it reaches at the end of the file.
  */
  while((byte_read < size_requested) &&
        (file_size != byte_read))
  {
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read data from file.
  */
    *size_ptr = mcfg_fread( fd,
                          (byte*)data_ptr+byte_read,
                            (size_requested-byte_read),
                            MCFG_FS_TYPE_EFS);

  if( *size_ptr < 0 )
  {
    SD_MSG_HIGH_2( "efs read error %d errno %d", *size_ptr, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    *size_ptr = 0;
    (void) mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
    return FALSE;
  }

    byte_read += *size_ptr;

  /* Return the number of bytes actually read.
  */
  
    SD_MSG_HIGH_2("size_req=%d, size_read=%d", size_requested, *size_ptr);
  }

  *size_ptr = byte_read;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Close the file */
  (void) mcfg_fclose(fd, MCFG_FS_TYPE_EFS);


  return TRUE;
}



/*===========================================================================

FUNCTION sdefs_write

DESCRIPTION
Write contents of data_ptr in EFS item denoted by efs_id

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if write is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean                  sdefs_write(

        sdefs_id_e_type          efs_id,
            /* passing the efs Id to write.
            */
        const byte               *data_ptr,
            /* Pointer to a data buffer to write to EFS.
            */
        int32                    size
            /* Size of the data_ptr buffer.
            */
)
{
  char        file_name[SDEFS_FILENAME_MAX_LEN];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( data_ptr != NULL );
  SD_ASSERT( size != 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if(!sdefs_map_efs_id_to_filename(efs_id, file_name))
   {
      SD_ERR_0("EFS file map failed");
      return FALSE;
   }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sdefs_write2(file_name, data_ptr, size);

}

/*===========================================================================

FUNCTION sdefs_write2

DESCRIPTION
Write contents of data_ptr in EFS item denoted by filename

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if write is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean                  sdefs_write2(

        const char                *filename,
            /* passing the filename to read. 
            */ 
        const byte                *data_ptr,
            /* Pointer to a data buffer to write to
            */
        int32                     size
            /* Size of the data_ptr buffer. 
            */
)
{
  int   fd;
  int32	size_committed;
  int32	byte_written = 0;
  byte*	next_block = (byte*)data_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( data_ptr != NULL );
  SD_ASSERT( size != 0 );

  if(data_ptr == NULL)
  {
    return FALSE;
  }
  if(filename == NULL)
  {
    return FALSE;
  }
  if(size == 0)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Open the specified resource file.
  ** If open fails, return error code
  */
  fd = mcfg_fopen( filename,
                   MCFG_FS_O_WRONLY | MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC,
                   MCFG_FS_ALLPERMS,
                   MCFG_FS_TYPE_EFS,
                   MCFG_FS_SUBID_0);

  if ( fd < 0 )
  {
    SD_MSG_HIGH_2( "open efs_err=%d errno %d", fd, mcfg_fs_errno(MCFG_FS_TYPE_EFS));

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write data to file.
  */

  while(byte_written < size)
  {
    //commit next block
    size_committed = mcfg_fwrite(fd, next_block, size - byte_written, MCFG_FS_TYPE_EFS);

    if ( size_committed <= 0 )
    {
      SD_MSG_HIGH_2( "write efs_err=%d errno %d", size_committed , mcfg_fs_errno(MCFG_FS_TYPE_EFS));

      /* Close the file */
      (void) mcfg_fclose(fd, MCFG_FS_TYPE_EFS);

      return FALSE;
    }

    //Update number of byte written
    byte_written += size_committed;

    //update offset pointer
    next_block += size_committed;

  } //while loop

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the number of bytes actually written.
  */
  SD_MSG_HIGH_3("Open file=%s, size_req=%d, size_wrote=%d",
                filename, size, byte_written);

  /* Close the file */
  (void) mcfg_fclose(fd, MCFG_FS_TYPE_EFS);

  return TRUE;
}

/*===========================================================================

FUNCTION sdefs_fs_write

DESCRIPTION
  Write contents of data_ptr to EFS item denoted by efs_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean                  sdefs_fs_write_per_subs(

        sdefs_id_e_type          efs_id,
            /* passing the efs Id to write.
            */
        const byte                  *data_ptr,
            /* Pointer to a data buffer to write to EFS.
            */
        int32                       size,
            /* Size of the data_ptr buffer.
            */
        sys_modem_as_id_e_type      sub_id
            /* A different EFS file is read based on the sub_id
            */
)
{
  mcfg_fs_status_e_type       status;
  char file_name[SDEFS_FILENAME_MAX_LEN];
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT(data_ptr != NULL);
  SD_ASSERT(size     != 0);

  /* Map efs_id to filename
  */
  if(!sdefs_map_efs_id_to_filename(efs_id, file_name))
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write the specified resource file
  */
  SD_MSG_HIGH_2("mcfg_fs_write file = %s, sub_id %d", file_name, sub_id);

  status = mcfg_fs_write(file_name,
                         (void*)data_ptr,
                         size,
                         MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                         MCFG_FS_ALLPERMS,
                         MCFG_FS_TYPE_EFS,
                         sdefs_map_as_id_to_mcfg_id(sub_id));

  if(status != MCFG_FS_STATUS_OK)
  {
    SD_MSG_HIGH_3("mcfg_fs_write error, sub_id %d, status=%d, errno %d", sub_id, status,
                   mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  return TRUE;
}

#ifdef FEATURE_EOOS
#error code not present
#endif

/*===========================================================================

FUNCTION sd_jcdma_ptr

DESCRIPTION
  Return the pointer to feature


DEPENDENCIES
  None

RETURN VALUE
  pointer of the jcdma feature set

SIDE EFFECTS
  None.

===========================================================================*/
jcdma_me_capability* sd_jcdma_ptr(void )
{
  static jcdma_me_capability sd_feature_jcdma;

  return &sd_feature_jcdma;
}

/*===========================================================================

FUNCTION sd_is_jcdma_enable

DESCRIPTION
  If a specified feature is enabled return TRUE, otherwise return FALSE


DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE
SIDE EFFECTS
  None.

===========================================================================*/
boolean sd_is_jcdma_enable(void)
{
  jcdma_me_capability *feature_jcdma = sd_jcdma_ptr();

  switch(feature_jcdma->service)
  {
  case JCDMA_SERVICE_ENABLED:
    return TRUE;
  case JCDMA_SERVICE_DISABLED:
  default:
    return FALSE;
  }
}

/*===========================================================================

FUNCTION sdefs_wcdma_freq_lock_ptr

DESCRIPTION
  Return the pointer to WCDMA Freq Lock status


DEPENDENCIES
  None

RETURN VALUE
  pointer of the WCDMA Freq Lock status

SIDE EFFECTS
  None.

===========================================================================*/
uint16* sdefs_wcdma_freq_lock_ptr(sys_modem_as_id_e_type asubs_id )
{
  static uint16 sd_wcdma_freq_lock[MAX_SIMS] = {0};
  int32 int_size = sizeof(uint16);

  if(!(asubs_id >= SYS_MODEM_AS_ID_1 && asubs_id < MAX_SIMS
    && asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }
  if(!is_wcdma_freq_lock_efs_read_done[asubs_id])
  {
    if(!sdefs_fs_read_per_subs(
      SDEFS_WCDMA_FREQ_LOCK, (byte*)&(sd_wcdma_freq_lock[asubs_id]), &int_size, asubs_id))
    {
      sd_wcdma_freq_lock[asubs_id] = 0;
    }
    else
    {
      is_wcdma_freq_lock_efs_read_done[asubs_id] = TRUE;
    }
    
    SD_MSG_HIGH_2("WCDMA Freq Lock status[%d] %d", asubs_id, sd_wcdma_freq_lock[asubs_id]);
  }

  return &sd_wcdma_freq_lock[asubs_id];
}
/*===========================================================================

FUNCTION sdefs_csg_auto_mode_bsr_disable

DESCRIPTION
  Returns true if bsr is disabled in NV


DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean sdefs_csg_auto_mode_bsr_disable(void)
{
  sdss_efs_get_global_mode_csg csg_auto_mode;

  if(!sdefs_get(SDEFS_CSG_AUTO_MODE, (byte*)&csg_auto_mode, sizeof(sdss_efs_get_global_mode_csg)))
  {
    SD_MSG_HIGH_0("CSG Auto mode EFS read failed!");
    csg_auto_mode.disable_bsr_global_csg = 0;
    csg_auto_mode.csg_global_mode_support = 0;
    return FALSE;
  }
  else
  {
     SD_MSG_HIGH_2("CSG Auto mode csg support %d bsr %d",
     csg_auto_mode.csg_global_mode_support,
     csg_auto_mode.disable_bsr_global_csg);

     /*If disable bsr is set and csg global mode support is not none disable bsr */
     if((csg_auto_mode.disable_bsr_global_csg) &&
        (csg_auto_mode.csg_global_mode_support))
    {
      return TRUE;
    }
  }
  return FALSE;
}





