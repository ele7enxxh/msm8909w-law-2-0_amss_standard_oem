/******************************************************************************
  @file    ps_sys_conf_parser.c
  @brief   

  DESCRIPTION
  This file defines functions, enums and structs used to parse the configuration
  items from EFS
  
  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2011-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ps_sys_conf_parser.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $ 

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/14   svj     Created module
===========================================================================*/

/*---------------------------------------------------------------------------
                           INCLUDE FILES
---------------------------------------------------------------------------*/

#include "ps_system_heap.h"
#include "dserrno.h"
#include "ds_Utils_DebugMsg.h"
#include <stringl/stringl.h>
#include "ds_Utils_DebugMsg.h"
#include "ps_utils.h"
#include "ps_sys_conf.h"
#include "ps_sys_conf_parser.h"


/**
  @brief  This function extracts the configuration name string 
          from the given index 
   
  @param[in] from   Start of the string
  @param[in] to     End of the string 
  @param[out] param_name     Extracted parameter string
   
  @see  PS_SYS_CONF_EFS_APN_INFO 
   
  @return  0     Success
  @return  -1    On any failure.     
*/
int ps_sys_conf_parser_read_param_name_index
(    
   char   *from, 
   char   *to, 
   int    *param_name_index
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( (NULL == from) || (NULL == to) || (from == to) )
  {
    LOG_MSG_ERROR_0 ("ps_sys_conf_read_param_name(): "
                     "Input parameters are NULL");
    return -1;
  }
  
  *param_name_index = *from - '0';
  return 0;
}/* ps_sys_conf_parser_read_param_name */

/**
  @brief  This function extracts the configuration name string 
          from the given index 
   
  @param[in] from   Start of the string
  @param[in] to     End of the string 
  @param[out] param_value     Extracted parameter val string
   
  @see  PS_SYS_CONF_EFS_APN_INFO 
   
  @return  0     Success
  @return  -1    On any failure.     
*/
int ps_sys_conf_parser_read_param_value
( 
  char        *from, 
  char        *to, 
  char        **param_value
)
{
  char *conf_val = NULL;
  int   num_bytes = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( (NULL == from) || (NULL == to) )
  {
    LOG_MSG_ERROR_0 ("ps_sys_conf_read_param_name(): "
                      "Input parameters are NULL");
    return -1;
  }

  num_bytes = (to - from) + 1;
  PS_SYSTEM_HEAP_MEM_ALLOC( *param_value, num_bytes + 3, char*);
  if (NULL == *param_value)
  {
     LOG_MSG_ERROR_0 ("ps_sys_conf_read_param_value(): "
                      "No mem to cpy param val !");

     return -1;
  }

  conf_val = *param_value; 
  strlcpy((char*)conf_val,
          (char*)from,
          num_bytes);

  conf_val[num_bytes] = '\0';
  return 0;

}/* ps_sys_conf_parser_read_param_value */

/**
  @brief  This function parses the given EFS config file, 
          extracts the parameters and updates the corresponding
          configuration cache
   
  @param[in] file_name   File path string
  @param[in] subs_id     Subscription identifier
   
  @return  0     Success
  @return  -1    On any failure.     
*/
int ps_sys_conf_parse_cfg_items
(
  char*            file_name, 
  uint32           subs_id
)
{
  ps_efs_token_type efs_handle;
  ps_efs_token_parse_status_enum_type ret_val = PS_EFS_TOKEN_PARSE_SUCCESS;
  boolean is_param_name = FALSE;
  int     param_name_index = 0;
  char   *param_val = NULL;
  char   *from, *to;
  int     err_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ps_efs_file_init_ext(file_name, &efs_handle, subs_id) == -1)
  {
    LOG_MSG_ERROR_0("ps_sys_conf_parse_cfg_items(): "
                    "Error opening data config file");
    return -1;
  }
  /*-----------------------------------------------------------------            
    Parser expects the input file to be in the format:
    config1:value1;config1:value2; and so on.For e.g 0:0;2:1,0,"att"; 
  -----------------------------------------------------------------*/
  efs_handle.seperator = ':';
  is_param_name = TRUE;
  /*-------------------------------------------------------------------------
    Parse the record tokens from the EFS file and assign them to the 
    appropriate field. The tokens in a record may be separated either
    by '\n' or by ';'
  -------------------------------------------------------------------------*/  
  while (PS_EFS_TOKEN_PARSE_EOF 
          != (ret_val = ps_efs_tokenizer(&efs_handle, &from, &to )))
  {
     /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and 
      to point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines 
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (PS_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      efs_handle.seperator = ':';
      is_param_name = TRUE;
      continue;
    }     
    else if(PS_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      if(is_param_name)
      {
         //Extract param name index
        if(ps_sys_conf_parser_read_param_name_index(from, to,
                                                    &param_name_index) != 0)
        {
          err_val = -1;
          break;
        }
        efs_handle.seperator = ';'; 
        is_param_name = FALSE;
        
      }
      else
      { 
        //Extract param value
        if(ps_sys_conf_parser_read_param_value(from, to, &param_val) != 0)
        {
          err_val = -1;
          break;
        }

        efs_handle.seperator = ':';
        is_param_name = TRUE;
        if(ps_sys_conf_update_cache_from_efs(param_name_index, param_val,
                                             subs_id) != 0)
        {
          LOG_MSG_ERROR_0("Update from efs failed");
          PS_SYSTEM_HEAP_MEM_FREE(param_val);
          err_val = -1;
          break;
        }
        //Mem is allocated in the in the above funct for every iteration
        PS_SYSTEM_HEAP_MEM_FREE(param_val);
      }
    }
    else
    {
      LOG_MSG_ERROR_1("Parse token error, ret_val %d", ret_val);
      err_val = -1;
    }
  }/* end while */
  
  ps_efs_file_close(&efs_handle);
  return err_val;
}/* ps_sys_conf_parse_cfg_items */

void ps_sys_conf_parser_update_cfg_cache
(
  void
)
{
   int           file_handle;
   uint32        subs_id;
   char         file_path[PS_SYS_CONF_PARSER_MAX_NAME_STRING];
   mcfg_fs_sub_id_e_type  mcfg_subs_id = MCFG_FS_SUBID_NONE;
   mcfg_fs_e_type file_type = MCFG_FS_TYPE_EFS;
   int ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (subs_id = PS_SYS_PRIMARY_SUBS; subs_id < PS_SYS_SUBS_MAX; subs_id++)
  {
    strlcpy(file_path, PS_SYS_CONF_PARSER_DATA_SERVICES_SETTINGS,
              PS_SYS_CONF_PARSER_MAX_NAME_STRING);
    
    mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(subs_id);

    /* Validate if the file is present */
    file_handle = mcfg_fopen (file_path, MCFG_FS_O_RDONLY | MCFG_FS_O_TRUNC, 
                              MCFG_FS_ALLPERMS, 
                              file_type,
                              mcfg_subs_id);

    if ( 0 > file_handle )
    {
      LOG_MSG_INFO1_1("File does not exist or cannot open, err %d !", mcfg_fs_errno( file_type )); 
      ret_val = -1;
    }
    else
    {
      mcfg_fclose(file_handle,
                  file_type);
      //parse the values from efs temp file
      if(ps_sys_conf_parse_cfg_items(file_path, subs_id) !=  0)
      {
        ret_val = -1;
      }
    }

    if (ret_val != 0) 
    {
      // If EFS read fails, initialise cache with default values
      ps_sys_conf_set_default_values(subs_id);
      ret_val = 0;
    }
  }/* end for */
}/* ps_sys_conf_parser_update_cfg_cache*/

void ps_sys_conf_parser_update_efs_from_cache
(
  uint32             subs_id
)
{
  char         file_path[PS_SYS_CONF_PARSER_MAX_NAME_STRING];
  int          file_handle = 0;
  char         *file_buffer = NULL;
  int           total_bytes = 0;
  fs_ssize_t    efs_ret = 0;
  mcfg_fs_sub_id_e_type  mcfg_subs_id = MCFG_FS_SUBID_NONE;
  mcfg_fs_e_type file_type = MCFG_FS_TYPE_EFS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 strlcpy(file_path, PS_SYS_CONF_PARSER_DATA_SERVICES_SETTINGS,
            PS_SYS_CONF_PARSER_MAX_NAME_STRING);
 
  mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(subs_id);

  /* Validate if the file is present */
  file_handle = mcfg_fopen (file_path, 
                            MCFG_FS_O_RDONLY | MCFG_FS_O_TRUNC,
                            MCFG_FS_ALLPERMS, 
                            file_type,
                            mcfg_subs_id);

  if ( 0 > file_handle )
  {
    LOG_MSG_INFO1_1("File does not exist or cannot open, err %d !", mcfg_fs_errno( file_type ));
  }
  else
  {
    /* File already exists; Truncate and write new contents */
    mcfg_fs_truncate(file_path, 0, file_type, mcfg_subs_id);
    mcfg_fclose(file_handle, file_type);
  }

  file_handle = mcfg_fopen (file_path,  
                            MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_WRONLY, 
                            MCFG_FS_DEFFILEMODE,
                            file_type,
                            mcfg_subs_id);
  if ( 0 > file_handle )
  {
    LOG_MSG_INFO1_1("Cannot create new file err %d !", mcfg_fs_errno( file_type ));
    return;
  }   

  /* Allocate buffer to write contents */
  PS_SYSTEM_HEAP_MEM_ALLOC(file_buffer, PS_SYS_CONF_PARSER_MAX_FILE_BUFFER_SIZE,
                           char*);
  if (file_buffer == NULL)
  {
    LOG_MSG_ERROR_0 ("ps_sys_conf_update_efs_from_cache(): "
                     "No memory for file buffer ");
    return;
  }  

  if( -1 == ps_sys_conf_write_conf_items_buffer(file_buffer, &total_bytes,
                                                subs_id))
  {
    LOG_MSG_ERROR_0 ("ps_sys_conf_update_efs_from_cache(): "
                     "Writing conf items failed");
    return;
  }  

  efs_ret = mcfg_fwrite(file_handle, file_buffer, total_bytes, file_type);
  if((-1 == efs_ret) || (efs_ret != total_bytes))
  {
    LOG_MSG_ERROR_1 ("EFS Write failed. Efs_ret:%d", efs_ret);
    return;
  }

  PS_SYSTEM_HEAP_MEM_FREE(file_buffer);
  mcfg_fclose(file_handle, file_type);
}/* ps_sys_conf_parser_update_efs_from_cache */

