/*============================================================================
  @file sns_reg_conf_la.c

  @brief Implementation of registry configuration functions for Linux Android

  <br><br>

  DEPENDENCIES: This uses file system abstraction defined in sns_fsa.h.

  Copyright (c) 2013, 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

#include "sensor1.h"
#include "sns_common.h"
#include "sns_debug_str.h"
#include "sns_fsa.h"
#include "sns_main.h"
#include "sns_memmgr.h"
#include "sns_reg.h"
#include "sns_reg_api_v02.h"
#include "sns_reg_priv.h"
#include "sns_reg_platform.h"

#include <cutils/properties.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*============================================================================
  Preprocessor Definitions and Constants
  ============================================================================*/
#define USERNAME_ROOT "root"
#define USERNAME_SYSTEM "system"

/* Constants & defines for configuration file */
#define META_KEY_LEN 10
#define META_VAL_LEN 32
/* Allow to compare upto 10 key values */
#define META_MAX_KEY 10
static const char sns_reg_default_prefix[] = "sensor_def_";
#define SNS_REG_DEFAULT_PREFIX_LEN 11
static const char sns_reg_default_postfix[] = ".conf";
#define SNS_REG_DEFAULT_POSTFIX_LEN 5

typedef struct sns_reg_prop_key_val_t
{
  char name[PROPERTY_KEY_MAX];
  char value[PROPERTY_VALUE_MAX];
} sns_reg_prop_key_val_s;

/*===========================================================================

  FUNCTION:   sns_reg_find_conf_by_name

  ===========================================================================*/
/*!
  @brief Find conf settings info in the list based on the file name.

  @param settings[i]
  @param Name of the conf file.

  @return Pointer to conf info structure corresponding to the file.
*/
/*=========================================================================*/
static sns_reg_settings_conf_info_s *
sns_reg_find_conf_by_name( sns_reg_settings_group_s *settings, char *name )
{
  int i;

  for( i = 0; i < SNS_REG_SETTINGS_NUM_FILES; i++ ) {
    if( strncmp(settings->conf[i].name, name, SNS_REG_SETTINGS_FILE_NAME_LEN ) == 0 ) {
      return &(settings->conf[i]);
    }
  }
  return NULL;
}

/*===========================================================================

  FUNCTION:   sns_reg_insert_new_conf

  ===========================================================================*/
/*!
  @brief Insert new conf settings info into the list.

  @param settings[i]
  @param Pointer to conf info structure corresponding to the file.

  @return Pointer to conf info structure corresponding to the file in the list.
*/
/*=========================================================================*/
static sns_reg_settings_conf_info_s *
sns_reg_insert_new_conf( sns_reg_settings_group_s *settings, sns_reg_settings_conf_info_s const *conf )
{
  int i;

  for( i = 0; i < SNS_REG_SETTINGS_NUM_FILES; i ++ ) {
    if( settings->conf[i].name[0] == 0 ) {
      settings->conf[i] = *conf;
      return &(settings->conf[i]);
    }
  }
  return NULL;
}

/*===========================================================================

  FUNCTION:   sns_reg_lib_filter

  ===========================================================================*/
/*!
  @brief Filter function used by scandir. Filters for registry library shared
         libraries.

         Verifies the file name starts with SENSOR_DEFAULT_LIB_PREFIX, and
         ends with ".so". Verifies that the file is owned by root or system.

  @param [i] dirent: Directory entry to determine if it matches the requirements
         for a registry shared object.

  @return
   1 - The file matches the requirements
   0 - The file does not match the requirements
*/
/*=========================================================================*/
static int sns_reg_lib_filter( const struct dirent *dirent )
{
  struct stat    stat_buf;
  struct passwd *pwent;
  static uid_t   root_uid;
  static uid_t   system_uid;
  static bool    uid_found = false;

  /* Get UIDs for root & system */
  if( false == uid_found ) {
    pwent = getpwnam( USERNAME_ROOT );
    if( pwent != NULL ) {
      root_uid = pwent->pw_uid;

#ifdef SNS_LA_SIM
      UNREFERENCED_PARAMETER( system_uid );
      uid_found = true;
#else
      pwent = getpwnam( USERNAME_SYSTEM );
      if( pwent != NULL ) {
        system_uid = pwent->pw_uid;
        uid_found = true;
      }
#endif /* !SNS_LA_SIM */
    }
  }


  /* Verify that it's a regular file, and the file name prefix matches */
  if( (dirent->d_type == DT_REG) &&
      strncasecmp(dirent->d_name, sns_reg_default_prefix, SNS_REG_DEFAULT_PREFIX_LEN ) == 0 ) {
    /* Verify it ends in ".conf" */
    if( strncmp(dirent->d_name+strlen(dirent->d_name)-5, sns_reg_default_postfix, SNS_REG_DEFAULT_POSTFIX_LEN) == 0) {
      /* Verify that it's owned by root or system */
      if( uid_found ) {
        char path[256] = SNS_REG_DEFAULT_CONF_PATH;
        strlcat( path, dirent->d_name, 256 );
        if( 0 == stat( path, &stat_buf ) ) {
#ifndef SNS_LA_SIM
          if( stat_buf.st_uid == root_uid ||
              stat_buf.st_uid == system_uid ) {
#endif /* SNS_LA_SIM */
            /* verify does not have world-write permissions */
            if( !(stat_buf.st_mode & S_IWOTH) ) {
              return 1;
            }
#ifndef SNS_LA_SIM
          }
#endif /* SNS_LA_SIM */
        }
      }
    }
  }
  return 0;
}

/*===========================================================================

  FUNCTION:   sns_reg_parse_conf_item

  ===========================================================================*/
/*!
  @brief Parses a line in a conf file, looking for item IDs.

  @param line[i]: String containing a line of the conf file
  @param item_id[o]: Discovered item ID, if any
  @param item_val[o]: Discovered item val, if any
  @param item_ver[o]: Discovered item version, if any

  @return
  true - Successfully parsed an item line
  false - Not sucessfully parsed.
*/
/*=========================================================================*/
bool sns_reg_parse_conf_item( char * line,
                              int32_t *restrict item_id,
                              uint64_t *restrict item_val,
                              uint32_t *restrict item_ver )
{
  char *start = line;
  char *end = NULL;

  errno = 0;
  *item_id = strtoll(start, &end, 0);
  if( start == end || errno != 0) {
    return false;
  }
  start = end;
  *item_val = strtoull(start, &end, 0);
  if( start == end || errno != 0) {
    return false;
  }
  start = end;
  *item_ver = strtoull(start, &end, 0);
  if( start == end || errno != 0) {
    return false;
  }
  return true;
}

/*===========================================================================

  FUNCTION:   sns_reg_match_conf_item

  ===========================================================================*/
/*!
  @brief compare sys entry with key values list

  @param sys_entry: sysfs entry string
  @param key_buf: key values list

*/
/*=========================================================================*/
static int sns_reg_match_conf_item( char *sys_entry,
                                    char (*key_buf)[META_VAL_LEN] )
{
  int key_var = 0;

  for( key_var = 0; key_var < META_MAX_KEY; key_var++ ) {
    if( key_buf[key_var][0] != 0x00 &&
      strstr( sys_entry, key_buf[key_var] ) != NULL ) {
      /* key value matched with sysfs entry*/
      return 0;
    }
  }
  /* Return if key value didn't match */
  if(key_buf[0][0] != 0x00 && META_MAX_KEY == key_var) {
    /* key value didn't match,return */
    return -1;
  }
  return -1;
}

/*===========================================================================

  FUNCTION:   sns_reg_write_conf_item

  ===========================================================================*/
/*!
  @brief Writes an item into the registry if it applies to this platform.

  @param item_id[i]: Item ID
  @param item_val[i]: Discovered item val, if any
  @param hw_conf[i]: Hardware string selected in the conf file
  @param platform_conf[i]: Platform string selected in the conf file
  @param soc_id_conf[i]: SOC ID string selected in the conf file
  @param subtype_conf[i]: Platform subtype string selected in the conf file
  @param subtype_id_conf[i]: Platform sub_id string selected in the conf file
  @param soc_rev_conf[i]: SOC Revision string selected in the conf file
  @param property_conf[i]: Property name and value selected in conf file

*/
/*=========================================================================*/
void sns_reg_write_conf_item( int32_t item_id,
                              uint64_t item_val,
                              char (*hw_conf)[META_VAL_LEN],
                              char (*platform_conf)[META_VAL_LEN],
                              char (*version_conf)[META_VAL_LEN],
                              char (*soc_id_conf)[META_VAL_LEN],
                              char (*subtype_conf)[META_VAL_LEN],
                              char (*subtype_id_conf)[META_VAL_LEN],
                              char (*soc_rev_conf)[META_VAL_LEN],
                              const sns_reg_prop_key_val_s *property_conf )

{
  int index;
  static char hw_string[PROPERTY_VALUE_MAX] = "";
  static char platform_string[20] = "";
  static char platform_subtype_string[20] = "";
  static char platform_subtype_id_string[20] = "";
  static char platform_version_string[20] = "";
  static char soc_id_string[20]="";
  static char soc_rev_string[20]="";
  static const char invalid_str[] = "Invalid file";
  char prop_value[PROPERTY_VALUE_MAX];

  /* Get the hardware & platform strings */
  if( hw_string[0] == 0 ) {
    FILE *fp;

    /* get the hw string from property */
    property_get("ro.board.platform", hw_string, "");

    fp = sns_fsa_open("/sys/devices/soc0/hw_platform", "r" );
    if( fp == NULL )
    {
      SNS_PRINTF_STRING_ERROR_1( SNS_MODULE_APPS_REG,
                                 "hw_platform fopen failed %i", errno );
      strlcpy(platform_string, invalid_str, sizeof(platform_string));
    }
    else if( fgets(platform_string, sizeof(platform_string), fp) == NULL )
    {
      SNS_PRINTF_STRING_ERROR_1( SNS_MODULE_APPS_REG,
                                 "hw_platform fgets failed %i: %i", errno );
      strlcpy(platform_string, invalid_str, sizeof(platform_string));
    }
    if( fp != NULL )
    {
      sns_fsa_close( fp );
    }

    /* check the subtype */
    fp = sns_fsa_open("/sys/devices/soc0/platform_subtype", "r" );
    if( fp == NULL )
    {
      SNS_PRINTF_STRING_HIGH_1( SNS_MODULE_APPS_REG,
                                "platform_subtype fopen failed %i", errno );
      strlcpy(platform_subtype_string, invalid_str, sizeof(platform_subtype_string));
    }
    else if( fgets(platform_subtype_string, sizeof(platform_subtype_string), fp) == NULL )
    {
      SNS_PRINTF_STRING_ERROR_1( SNS_MODULE_APPS_REG,
                                 "platform_subtype fgets failed %i: %i", errno );
      strlcpy(platform_subtype_string, invalid_str, sizeof(platform_subtype_string));
    }
    if( fp != NULL )
    {
        sns_fsa_close( fp );
    }
    /* check the subtype_id */
    fp = sns_fsa_open("/sys/devices/soc0/platform_subtype_id", "r" );
    if( fp == NULL )
    {
      SNS_PRINTF_STRING_HIGH_1( SNS_MODULE_APPS_REG,
                                "platform_subtype_id fopen failed %i", errno );
      strlcpy(platform_subtype_id_string, invalid_str, sizeof(platform_subtype_id_string));
    }
    else if( fgets(platform_subtype_id_string, sizeof(platform_subtype_id_string), fp) == NULL )
    {
      SNS_PRINTF_STRING_ERROR_1( SNS_MODULE_APPS_REG,
                                 "platform_subtype_id fgets failed %i: %i", errno );
      strlcpy(platform_subtype_id_string, invalid_str, sizeof(platform_subtype_id_string));
    }
    if( fp != NULL )
    {
        sns_fsa_close( fp );
    }
    /* check the platform version */
    fp = sns_fsa_open("/sys/devices/soc0/platform_version", "r" );
    if( fp == NULL )
    {
      SNS_PRINTF_STRING_HIGH_1( SNS_MODULE_APPS_REG,
                                "platform_version fopen failed %i", errno );
      strlcpy(platform_version_string, invalid_str, sizeof(platform_version_string));
    }
    else if( fgets(platform_version_string, sizeof(platform_version_string), fp) == NULL )
    {
      SNS_PRINTF_STRING_ERROR_1( SNS_MODULE_APPS_REG,
                                 "platform_version fgets failed %i: %i", errno );
      strlcpy(platform_version_string, invalid_str, sizeof(platform_version_string));
    }
    if( fp != NULL )
    {
      sns_fsa_close( fp );
    }

    /* Get SOC ID */
    fp = sns_fsa_open("/sys/devices/soc0/soc_id", "r" );
    if( fp == NULL )
    {
      SNS_PRINTF_STRING_HIGH_1( SNS_MODULE_APPS_REG,
                                "soc_id fopen failed %i", errno );
      strlcpy(soc_id_string, invalid_str, sizeof(soc_id_string));
    }
    else if( fgets(soc_id_string, sizeof(soc_id_string), fp) == NULL )
    {
      SNS_PRINTF_STRING_ERROR_1( SNS_MODULE_APPS_REG,
                                 "soc_id fgets failed %i: %i", errno );
      strlcpy(soc_id_string, invalid_str, sizeof(soc_id_string));
    }
    if( fp != NULL )
    {
      sns_fsa_close( fp );
    }

    /* Get SOC Revision */
    fp = sns_fsa_open("/sys/devices/soc0/revision", "r" );
    if( fp == NULL )
    {
      SNS_PRINTF_STRING_HIGH_1( SNS_MODULE_APPS_REG,
                                "soc revision fopen failed %i", errno );
      strlcpy(soc_rev_string, invalid_str, sizeof(soc_rev_string));
    }
    else if( fgets(soc_rev_string, sizeof(soc_rev_string), fp) == NULL )
    {
      SNS_PRINTF_STRING_ERROR_1( SNS_MODULE_APPS_REG,
                                 "soc revision fgets failed %i: %i", errno );
      strlcpy(soc_rev_string, invalid_str, sizeof(soc_rev_string));
    }
    if( fp != NULL )
    {
      sns_fsa_close( fp );
    }
  }

  /* If this item doesn't apply to this architecture, skip to the next line */
  if( ( hw_conf[0][0] != 0 ) &&
      sns_reg_match_conf_item( hw_string, hw_conf ) ) {
    /* Configuration file set a hardware string, and it doesn't match
       this hardware. Abort */
    return;
  }
  if( ( platform_conf[0][0] != 0 ) &&
      sns_reg_match_conf_item( platform_string, platform_conf ) ) {
      /* Configuration file set a platform string, and it doesn't match
         the platform string. Abort */
      return;
  }
  if( ( soc_id_conf[0][0] != 0 ) &&
      sns_reg_match_conf_item( soc_id_string, soc_id_conf ) ) {
      /* Configuration file set a soc id string, and it doesn't match
         this hardware. Abort */
      return;
  }
  if( ( subtype_conf[0][0] != 0 ) &&
      sns_reg_match_conf_item( platform_subtype_string, subtype_conf ) ) {
      /* Configuration file set a platform subtype string, and it doesn't match
         this hardware. Abort */
      return;
  }

  if( ( subtype_id_conf[0][0] != 0 ) &&
      sns_reg_match_conf_item( platform_subtype_id_string, subtype_id_conf ) ) {
      /* Configuration file set a platform subtype_id string, and it doesn't match
         this hardware. Abort */
      return;
  }

  if( ( version_conf[0][0] != 0 ) &&
      sns_reg_match_conf_item( platform_version_string, version_conf ) ) {
      /* Configuration file set a platform version string, and it doesn't match
         this hardware. Abort */
      return;
  }

  if( ( soc_rev_conf[0][0] != 0 ) &&
      sns_reg_match_conf_item( soc_rev_string, soc_rev_conf ) ) {
      /* Configuration file set a soc rev string, and it doesn't match
         this hardware. Abort */
      return;
  }

  if( property_conf->name[0] != 0 &&
      property_get( property_conf->name, prop_value, "" ) > 0 &&
      strncmp( property_conf->value, prop_value, PROPERTY_VALUE_MAX ) != 0 ) {
    /* Property is set in conf file, and conf file value does not
     * match the target value. Abort */
    return;
  }

  index = sns_reg_lookup_item_index( item_id );

  if( index < 0 ) {
    return;
  }
  /* Assume little-endian */
  sns_reg_write( (uint8_t*)(&item_val),
                 sns_reg_item_info[index].offset,
                 sns_reg_item_size[ sns_reg_item_info[index].type ],
                 false );
}

/*===========================================================================

  FUNCTION:   sns_reg_get_conf_key

  ===========================================================================*/
/*!
  @brief get the key values list from conf file

  @param conf_buf: key values list source buffer
  @param key_buf: key values list destination buffer

*/
/*=========================================================================*/
static void sns_reg_get_conf_key( char *conf_buf,
                                  char (*key_buf)[META_VAL_LEN] )
{
  int key_var;
  char *tmp_buf;
  char *saveptr;

  /* ignore the new line character */
  conf_buf[strlen(conf_buf) - 1] = 0x00;
  /* Once the Key detected in conf file, clean up the
     previous previous key values */
  for( key_var = 0; key_var < META_MAX_KEY; key_var++ ) {
    key_buf[key_var][0] = 0x00;
  }

  key_var = 0;
  tmp_buf = strtok_r( conf_buf, " ", &saveptr );
  while( tmp_buf != NULL ) {
    if( key_var > 0 && key_var <= META_MAX_KEY ) {
      strlcpy( key_buf[key_var-1], tmp_buf, META_KEY_LEN );
    }
    key_var++;
    /* If more than META_MAX_KEY key values are provided in conf
       ignore the rest */
    if( key_var > META_MAX_KEY ) {
      SNS_PRINTF_STRING_ERROR_0( SNS_MODULE_APPS_REG,
                                 "More than allowed number of key values" );
      break;
    }
    tmp_buf = strtok_r( NULL, " ", &saveptr );
  }
}

/*===========================================================================

  FUNCTION:   sns_reg_parse_conf_file

  ===========================================================================*/
/*!
  @brief Parses a configuration file, possibly writing data to the registry

  @param file[i]: FILE pointer to the open config file
  @param conf_info[i/o]: Information about the file

*/
/*=========================================================================*/
void sns_reg_parse_conf_file( FILE *file,
                              sns_reg_settings_conf_info_s *conf_info )
{
#define str(x)          # x
#define xstr(x)         str(x)
  const size_t sz = 1024;
  char *buf = malloc(sz);
  uint32_t file_version = 0;
  char hw_conf[META_MAX_KEY][META_VAL_LEN];
  char platform_conf[META_MAX_KEY][META_VAL_LEN];
  char soc_id_conf[META_MAX_KEY][META_VAL_LEN];
  char subtype_conf[META_MAX_KEY][META_VAL_LEN];
  char soc_rev_conf[META_MAX_KEY][META_VAL_LEN];
  char version_conf[META_MAX_KEY][META_VAL_LEN];
  char subtype_id_conf[META_MAX_KEY][META_VAL_LEN];
  int key_var;
  sns_reg_prop_key_val_s property_conf = { "", "" };

  for( key_var = 0; key_var < META_MAX_KEY; key_var++ ) {
    hw_conf[key_var][0]       = 0x00;
    platform_conf[key_var][0] = 0x00;
    soc_id_conf[key_var][0]   = 0x00;
    subtype_conf[key_var][0]  = 0x00;
    soc_rev_conf[key_var][0]  = 0x00;
    version_conf[key_var][0]  = 0x00;
    subtype_id_conf[key_var][0]  = 0x00;
  }

  if( NULL == buf ) {
    SNS_PRINTF_STRING_ERROR_0( SNS_MODULE_APPS_REG,
                               "Malloc error" );
  } else {
    while( fgets( buf, sz, file ) != NULL ) {
      int sscanf_result;

      /* Variables for comments */
      char *comment_ptr;

      /* Variables for item data */
      int32_t  item_id;
      uint64_t item_val;
      uint32_t item_ver;

      /* Variables for meta data */
      char key_meta[META_KEY_LEN];
      char val_meta[META_VAL_LEN];
      char val_meta2[META_VAL_LEN];

      /* Replace a "#" character with null, so we don't parse anything after that */
      comment_ptr = strchr( buf, '#' );
      if( comment_ptr != NULL ) {
        *comment_ptr = '\0';
      }

      /* Check for an item */
      if( true == sns_reg_parse_conf_item( buf, &item_id, &item_val, &item_ver ) ) {
        if( item_ver > conf_info->version) {
          sns_reg_write_conf_item( item_id, item_val, hw_conf,
                                   platform_conf, version_conf, soc_id_conf,
                                   subtype_conf, subtype_id_conf, soc_rev_conf, &property_conf );
        }
      } else {
        /* Check for metadata */
        val_meta[0] = '\0';
        val_meta2[0] = '\0';
        sscanf_result = sscanf( buf, ":%"xstr(META_KEY_LEN)"s %"xstr(META_VAL_LEN)"s %"xstr(META_VAL_LEN)"s",
                                key_meta, val_meta, val_meta2 );
        if( sscanf_result > 0 ) {
          if( 0 == strncmp( key_meta, "hardware", META_KEY_LEN ) ) {
            sns_reg_get_conf_key(buf, hw_conf);
          } else if( 0 == strncmp( key_meta, "platform", META_KEY_LEN ) ) {
            sns_reg_get_conf_key(buf, platform_conf);
          } else if( 0 == strncmp( key_meta, "soc_id", META_KEY_LEN ) ) {
            sns_reg_get_conf_key(buf, soc_id_conf);
          } else if( 0 == strncmp( key_meta, "soc_rev", META_KEY_LEN ) ) {
            sns_reg_get_conf_key(buf, soc_rev_conf);
          } else if( 0 == strncmp( key_meta, "subtype", META_KEY_LEN ) ) {
            sns_reg_get_conf_key(buf, subtype_conf);
          } else if( 0 == strncmp( key_meta, "plat_ver", META_KEY_LEN ) ) {
            sns_reg_get_conf_key(buf, version_conf);
          } else if (0 == strncmp( key_meta, "sub_id", META_KEY_LEN ) ) {
            sns_reg_get_conf_key(buf, subtype_id_conf);
          }else if( 0 == strncmp( key_meta, "version", META_KEY_LEN ) ) {
            file_version = (uint32_t) strtoll( val_meta, NULL, 0 );
          } else if( 0 == strncmp( key_meta, "property", META_KEY_LEN ) ) {
            strlcpy( property_conf.name, val_meta, sizeof(property_conf.name) );
            strlcpy( property_conf.value, val_meta2, sizeof(property_conf.value) );
          }
        }
      }
    }
    conf_info->version = file_version;
    free( buf );
  }
}

/*===========================================================================

  FUNCTION:   sns_reg_update_external_defaults

  ===========================================================================*/
/*!
  @brief Updates default values in the sensors registry via external library.

  @param settings[i]

  @side effects
  In addition to writing the new defaults to the registry, it may write to
  the settings to update the library names and versions.

  @return
   - true if update is successful.
   - false indicates an error has occurred.
*/
/*=========================================================================*/
bool sns_reg_update_external_defaults( sns_reg_settings_group_s *settings )
{
  bool rv = true;
  int  num_files,
       grp_idx,
       i;
  struct dirent **file_list = NULL;

  UNREFERENCED_PARAMETER( settings );

  /* Android defines alphasort incorrectly -- so do the function pointer typecast here
   * to avoid warnings */
  num_files = scandir( SNS_REG_DEFAULT_CONF_PATH, &file_list,
                       sns_reg_lib_filter,
                       (int(*)(const struct dirent**, const struct dirent**))(alphasort) );

  for( i = 0; i < num_files; i++ ) {
    /*Check upto maximum supported files*/
    if(i == SNS_REG_SETTINGS_NUM_FILES)
        break;
    char path[256] = SNS_REG_DEFAULT_CONF_PATH;
    char file_name[SNS_REG_SETTINGS_FILE_NAME_LEN+1] = "";
    sns_reg_settings_conf_info_s *conf_ptr;
    FILE *file_handle;

    strlcat( path, file_list[i]->d_name, 256 );
    strlcpy( file_name,
             file_list[i]->d_name + SNS_REG_DEFAULT_PREFIX_LEN,
             SNS_REG_SETTINGS_FILE_NAME_LEN );

    /* open file */
    file_handle = fopen( path, "r" );
    if( NULL == file_handle ) {
      continue;
    }
    conf_ptr = sns_reg_find_conf_by_name( settings, file_name );
    if( conf_ptr == NULL ) {
      sns_reg_settings_conf_info_s conf;
      strlcpy( conf.name, file_name, sizeof(conf.name) );
      conf.version = 0;
      conf_ptr = sns_reg_insert_new_conf( settings, &conf );
    }
    if( conf_ptr != NULL ) {
      sns_reg_parse_conf_file( file_handle, conf_ptr );
    }
    /* close file */
    fclose( file_handle );
  }

  grp_idx = sns_reg_lookup_group_index( SNS_REG_GROUP_SETTINGS_V02 );
  if( -1 != grp_idx ) {
    sns_reg_write( (uint8_t*)settings,
                   sns_reg_group_info[ grp_idx ].offset,
                   sizeof( *settings ),
                   true );
  } else {
    SNS_PRINTF_STRING_ERROR_0( SNS_MODULE_APPS_REG,
                               "Cannot find SNS_REG_GROUP_SETTINGS_V02 index" );
  }

  for( i = 0; i < num_files; i++ ) {
    free(file_list[i]);
  }
  free( file_list );
  return rv;
}
