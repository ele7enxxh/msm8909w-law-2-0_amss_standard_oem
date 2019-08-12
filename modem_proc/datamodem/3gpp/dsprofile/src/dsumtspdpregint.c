/*===========================================================================

                        P D P   R E G I S T R Y   L I B R A R Y 

                          
GENERAL DESCRIPTION
  This file contains data and function definitions for PDP registry.
  PDP registry provides a library to maintain PDP profiles in 
  Embedded File System (EFS). This allows the profiles to be accessed
  across power cycles. ATCOP / UI and Data services use the API library
  to store and retrieve the PDP profile data
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds_umts_pdp_registry_init_internal() should be called at task / system startup. 
  ds_umts_pdp_registry_init_internal checks for the presence of the profiles in 
  EFS and creates them if they are not found and initializes them.

  *********************  I M P O R T A N T ***************************
  a) PDP profiles are PACKED structures and are stored as such in binary
  files. The offset of the sub structures & parameters within ( auth,
  qos info etc) use the offset location of from the start of the file. 
  Care should be take to ensure this when changes are made to the 
  profile content structure definitions.

  b) The EFS file access requires fully qualified file names to be specified
  e.g., to access "profile1" file under "pdp_profiles" directory, the 
  fully qualified file name "pdp_profiles/profile1" has to be specified.

  c) MACROS are used to perform checks on input/output parameters and also 
  repetetive FILE operations. The macros return from the calling function
  if an error occurs. 

  d) Profile format revisions and identifying changes :
  If there are any changes to the profile formats, the s/w would have a 
  different size/format than the profiles which are already stored in EFS.
  Later API releases added a version field to the profile schema to
  identify unique versions along with a configuration file.  Earlier releases
  use the length of the file contents to see if the profile stored in EFS
  is of a different version than the s/w.  The presence of the configuration
  file determines which mode is used to detect version upgrades.
  
  e) No validation is performed on the parameter sets for their 
  validity or combination with respect to other fields. It is the 
  caller's responsibility to ensure this. However, the "valid_flg"
  field is checked to see if the parameter set is defined.

  f) The PDP registry API uses syncrhonous EFS mechanism to retrieve
  write data from/to the file system. The calling task has to be 
  blocked for this to work correctly and is done by the EFS API
  functions. DO NOT DISABLE CONTEXT SWITCHING by USING TASKLOCK()/TASKFREE()
  when calling the pdp registry API's to ensure the proper functioning
  of the API's. 

  g) The PDP registry API's do not enforce the dependency checks between
  the context definition and other parameter sets for a profile. Parameters
  other than the context definition (auth,qos,dns) are in effect an extension
  of the context definition. It is upto the caller & users of the API's to
  check for the dependency between the different parameter subsets and use 
  then accordingly.

  h) For L4, The PDP registry APIs use critical section to serialize access
 to the PDP registry and the efs file system. So after the first call,
 subsequent calls to the PDP registry will be blocked untill first call
 completes. The PDP Registry uses the rex critical sections to prevent
 multiple processes to access the registry or the efs file system.
  
Copyright (c) 2006-2016 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/dsprofile/src/dsumtspdpregint.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/20/13   nd      Skip creation of default emb/tethered profile num file if
                   not present in EFS.
07/27/12   ss      Fix to make embedded/tethered profile_num_file as an item 
                   file and use appropriate efs apis to read/write. 
06/27/12   kv      Support for mode T in Apn bearer field.
05/09/12   nd      Memory reduction effort in 3gpp.
09/27/11   nd      Fixed compiler warnings.
09/13/11   ttv     Changes to add src_stat_desc as profile parameter.
07/27/11   ttv     Added changes required for source statistics descriptor
                   addition.
05/31/11   rr      Added support for transient profiles
05/10/11   ttv     Fixed compilation warning.
04/20/11   ttv     Added changes for setting the default profile per family 
                   using set_default_profile_num_per_subs incase of dualsim.
04/13/11   ttv     Added changes to set pdp_type as ip based on 
                   v6_enabled_flag.
02/28/11   hs      Added changes to support APN class in profile
02/24/11   ttv     Fixed compilation warnings.
02/16/11   ttv     Added DSDS changes for consolidated profile family.
01/17/11   ss      Added support for Iface linger.
11/22/10   ttv     Added support for DSDS.
10/01/10   sa      Set default type to IPV4V6 only if IPV6 is enabled.
08/31/10   sa      Added support for default pdp type as IPv4v6.
02/07/10   ss      Reset string type PDP profile parameters before updating new 
                   value.
06/28/10   ss      Fixed compiler warnings.
06/01/10   ss      Compiler warning fix.
05/24/10   asn     Remove call to efs_sync
05/17/10   hs      Added APN disable and PDN inactivity feature support.
4/28/10    ars     Removed for loop in profile registry init that sets all 
                   profiles as valid upon bootup.
04/04/10   sa      Fixed reset issue for ds_umts_reset_pdp_profile_tft
                   _all_info_internal().
11/03/09   vk      Removed restriction that default sockets profile cannot be 
                   IPv6
11/02/09   asn     Fix bug in Profile number checking
10/24/09   asn     Removed FEATURE_DATA_RM_NET
10/19/09   ars     Replaced STD_SIZEOF as it was not defined for Linux builds
10/12/09   vk      PDP profile changes for IPv4v6 support
09/30/09   asn     Consolidation of Families
09/23/09   vd      Added support for LTE Qos.
09/22/09   vrk     Merged LTE changes.
05/19/09   vk      Defeaturization of PDP registry for CMI
12/11/08   vk      Fixed bug where socks call profile number was not being 
                   written correctly to EFS.
04/04/08   vd      Lint Fixes.
02/20/08   vrk     Fixed a bug for IMS flags by passing pointer to efs_write 
11/15/07   asn     Added support for new feature MT-PDP terminating in TE
06/15/07   aw      Added support for PCSCF Address discovery using DHCP.
05/01/07   aw      Added support for IM CN Flag.
02/15/07   aw      Removed the banned string APIs.
11/06/06   vk      Added function ds_umts_pdp_itoa() to remove dependency on 
                   ATCOP.
08/16/06   aw      Fixed the errors with FEATURE_PROFILE_OTAP_ENHANCEMENTS
                   turned on.
07/26/06   rsl     Changes relating to using EFS2 as supposed to EFS1.
07/17/06   rr      Added a check to make sure that Default profile type cannot 
                   be set as IPV6.
07/17/06   aw      Changed cscf to pcscf.
07/14/06   aw      Fixed a bug in PDP Registry function where PDP Registry
                   is not inidialized if new profile registry is loaded.
06/13/06   aw      Added support for IMS cscf address flag.
01/20/06   aw      Moved all functions from dsumtspdpreg file to this internal
                   file. Now these are called by wrappers in dsumtspdpreg
                   wrapper functions instead of by applications directly.
12/02/05   rr      Added RMNET Profile Family
11/18/05   rr      Featurized context_valid variable in ds_umts_pdp_registry
                   _init to fix lint error when FEATURE_PROFILE_OTAP_
                   ENHANCEMENTS is defined.
11/18/05   rc      Set all the undefined Socket Profiles to Default Value if
                   FEATURE_PROFILE_OTAP_ENHANCEMENTS is not defined.
09/20/05   rc      Fixed bug where profile file was not closed before 
                   returning from a function in case of error.
07/20/05   rc      Fixed bug in ds_umts_set_pdp_read_only_flag() and
                   ds_umts_set_pdp_otap_enabled_flag() where address-of the
                   variable was not passed in memcpy(). 
07/18/05   rr      Always check the validity of file size in  
                   ds_umts_pdp_validate_file_content
05/11/05   rr      Fixed Lint errors/warnings.
05/03/05   rr      Set ds_umts_pdp_registry_init_flag to FALSE if 
                   initialization fails. 
04/20/05   rr      Fixed a typo in calls to ds_umts_set_pdp_profile_info
                   where parameters were swapped.
04/13/05   rr      Fixed string copies to ensure that they are NULL 
                   terminated. 
03/23/05   rr      Replaced arguments of memcpy with correct pointers
03/09/05   rc      Added support for read_only cache. All get*() operations
                   are done from the profile family cache table.
                   Added check to see if read_only_flag for the profile is
                   set before performing set*() operations.
02/10/05   rc      Changed type (to UTF16 format) and length of profile name.
                   Added get/set routines for read_only_flag, otap_enabled,
                   and otap_napid fields of the profile.
                   All changes were made under FEATURE_PROFILE_OTAP_ENHANCEMENTS.
                   Moved version field to be the first field of the structure.
12/06/04   rr      PDP registry initialization will set all the undefined 
                   Sockets profiles to default value.
11/24/04   rc      Added code to set default atcop profile. 
11/22/04   rc      Added support for separation of ATCOP and sockets profiles
06/04/04   rc      Fixed Lint errors/warnings.
03/12/04   vsk     set sockets call profile number to 1 if value at startup
                   is invalid.
                   code cleanup to remove tab chars.
01/27/04   vsk     mainline M-PDP support
08/05/03   ar      Add configuration metadata file.
07/23/03   rc      Included files msg.h and err.h.
07/11/03   ar      Unconditionally truncate EFS file during reset operation. 
06/26/03   ar      Added support for secondary context and traffic flow
                   template paramaters.
05/23/03   rc      Removed Compiler Warnings.
02/14/03   vsk     Removed the dependency between context definition and
                   other parameter sets for a profile.
02/10/03   vsk     Added support to store/retrieve minimum UTMS QOS (AT+CGEQMIN)
                   parameters
02/05/03   vsk     Updated the file header section and added comments about
                   synchronous access & importance of not using TASKLOCKS
01/24/03   vsk     fixed lint warnings/errors
01/17/03   vsk     fixed a return error in the function
                   ds_umts_get_pdp_profile_context_info_pdp_address
01/16/03   vsk     creation of file 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_LTE)
#include <string.h>
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "mcfg_fs.h"
#include "dsumtspdpregint.h"
#include "ds_system_heap.h"

#include <stringl/stringl.h>
#include "dstaski.h"
#include "nv_items.h"
#include "ds_3gppi_utils.h"
#ifdef FEATURE_GOBI_MUXD_PDP_PROFILE_DIR
#include "dsatctab.h"
#endif /* FEATURE_GOBI_MUXD_PDP_PROFILE_DIR */

#include "modem_mem.h"

#ifdef FEATURE_DUAL_SIM
#include "dsumts_subs_mgr.h"
#include "ds3gmgr.h"
#endif /* FEATURE_DUAL_SIM */
#include "rex.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*------------------------------------------------------------------
    Macro for acquiring a Tasklock for cache access
-------------------------------------------------------------*/
  #define DS_UMTS_PDP_TASKLOCK_NON_L4() {}

/*------------------------------------------------------------------
  Macro for freeing a tasklock after cache access
------------------------------------------------------------------*/
  #define DS_UMTS_PDP_TASKFREE_NON_L4() {}

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*---------------------------------------------------------------------------
 Macros to perform simple checks on input paramaters in the access functions
 Since the checks are performed in each API, these macros are helpfull
 
NOTE : THE MACROS RETURN FROM THE CALLING FUNCTION IF AN ERROR OCCURS
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Macro to check if we are initialized
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_CHECK_INIT_FLAG() {\
  if( !ds_umts_pdp_registry_init_flag )\
    {\
      DS_3GPP_MSG0_ERROR("PDP registry not initialized");\
      return DS_UMTS_PDP_NOT_INITIALIZED;\
    }\
  }

 /*---------------------------------------------------------------------------
   Macro for transience_flag reading.
 ---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_PROFILE_IS_PERSISTENT( profile_ptr ) ( \
    (((profile_ptr)->transience_flag) == FALSE) \
 )

/*---------------------------------------------------------------------------
  Macro to check if the TFT filter identifier is valid
---------------------------------------------------------------------------*/
#define  DS_UMTS_PDP_CHECK_FILTER_ID(filter_id) {\
  if( (filter_id == 0) || \
      (filter_id > DS_UMTS_MAX_TFT_PARAM_SETS) )\
    {\
      DS_3GPP_MSG1_ERROR( "Invalid filter identifier:%d ",filter_id);\
      return DS_UMTS_PDP_INVALID_FILTER_ID;\
    }\
  }

/*---------------------------------------------------------------------------
  Macro to check if output buffer length is valid 
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_CHECK_OUT_BUFF_LEN(val,max) {\
  if( max > val ) \
    {\
      DS_3GPP_MSG2_ERROR("out buffer parm short :V:%d M:%d",val,max);\
      return DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER;\
    }\
  }

/*---------------------------------------------------------------------------
  Macro to check if input buffer length is valid
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_CHECK_IN_BUFF_LEN(val,max) {\
  if( val > max ) \
    {\
      DS_3GPP_MSG2_ERROR("Input buff > Max allowed V:%d M :%d",val,max);\
      return DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID;\
    }\
  }



/*---------------------------------------------------------------------------
  Macro to check if the profile's read_only_flag is set.
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_CHECK_READ_ONLY_FLAG(profile_num,subs_id,read_only_flag) {\
  ds_umts_pdp_profile_status_etype ro_status;\
  ro_status = ds_umts_get_pdp_profile_is_read_only_per_subs(profile_number,\
                                                  subs_id, \
                                                &read_only_flag);\
  if(ro_status != DS_UMTS_PDP_SUCCESS )\
  {\
    return ro_status;\
  }\
  if(read_only_flag )\
  {\
    DS_3GPP_MSG1_HIGH("Read-only Profile:%d.Unable to set.",\
                      profile_number);\
    return DS_UMTS_PDP_READ_ONLY_FLAG_SET;\
  }\
}

/*-----------------------------------------------------------------------------
  Macro to get the subscription ID.
  If subscription is none, set to default (subscription 1)
-----------------------------------------------------------------------------*/
#define DS_UMTS_PDP_GET_SUBS_ID(data_subs) {\
  data_subs = dsumts_subs_mgr_get_subs_id(ds3g_get_ps_subs_id());\
  if (data_subs == DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE) \
  {\
    data_subs = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_1;\
  }\
  DS_3GPP_MSG1_HIGH("Data Subscription is : %d", data_subs);\
}


/*---------------------------------------------------------------------------
  file name and directory name definitions for accessing EFS
  PDP profiles are stored as files in the Embedded File System (EFS)
  under the "pdp_profiles" directory
  profiles directory name          : pdp_profiles
  ATCOP profiles directory name    : atcop_profiles
  Sockets profiles directory name  : sockets_profiles
  profile file names      : profileXY  where XY represents the profile number
  for.e.g, profile1,profile2,profile3,.....profile10,profile11....
  sockets call file name  : socks_call_prof_file_num 
  configuration file name : config 
  
  API uses profile numbers as input parameters. The profile nummbers are 
  mapped to the profile file names e.g., profile number1 is stored in
  the file "profile1" and profile number '12' is stored in "profile12"
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Directory name for PDP profiles Config File in EFS 
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_PROFILE_DIR_NAME "pdp_profiles"

/*---------------------------------------------------------------------------
  Length of the profile directory name "pdp_profiles"
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_PROFILE_DIR_NAME_LEN  12

/*---------------------------------------------------------------------------
  Max length of the Consolidated profile directory name. This
  is the maximum path length for the directory name. If
  a profile is added whose directory name is longet than this, then this
  needs to be updated.

  "pdp_profiles"  = 12
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_PROFILE_MAX_DIR_NAME_LEN 50

#ifdef FEATURE_GOBI_MUXD_PDP_PROFILE_DIR

#define UNDP_PDP_PROFILE_CONSL_DIR_NAME_START "pdp_profiles"
#define UNDP_PDP_PROFILE_CONSL_DIR_NAME_INDEX 12
#define UNDP_CONSL_PROFILE_EN_MASK  0xFF00

#define DS_UMTS_PDP_PROFILE_CONSL_DIR_NAME undp_get_muxd_pdp_profile_dirname()
#define DS_UMTS_PDP_PROFILE_CONSL_DIR_NAME_LEN 14

#endif /* FEATURE_GOBI_MUXD_PDP_PROFILE_DIR */

/*---------------------------------------------------------------------------
  File name for PDP profiles in EFS : XY will be replaced with the profile
  number during access
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_PROFILE_FILE_NAME "profileXY"

/*---------------------------------------------------------------------------
  offset in "profileXY" where the profile number starts( index runs from 0 
  to N-1
  X - index0, Y - index1
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_PROFILE_FILE_NAME_NUM_INDEX0 7

/*---------------------------------------------------------------------------
  Length of the profile file name "profilexy" where xy will be replaced by 
  1,2, 10,11 ... 
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_PROFILE_FILE_NAME_LEN  9

/*---------------------------------------------------------------------------
  File name which stores the profile number used for embedded call
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_PROFILE_NUM_FOR_EMBEDDED_FILE_NAME "embedded_call_prof_num"

/*---------------------------------------------------------------------------
  Length of the embedded call profile number file name "embedded_call_prof_num" 
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_EMBEDDED_CALL_PROF_NUM_FILE_NAME_LEN  22

/*---------------------------------------------------------------------------
  File name which stores default Profile number used for tethered call
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_PROFILE_NUM_FOR_TETHERED_FILE_NAME "tethered_call_prof_num"

/*---------------------------------------------------------------------------
  Length of the tethered Profile number file name "tethered_call_prof_num" 
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_TETHERED_CALL_PROF_NUM_FILE_NAME_LEN  22

#define DS_UMTS_PDP_PROFILE_SUBSCRIPTION_LEN 15

/*---------------------------------------------------------------------------
  Macro to allocate memory for local pdp profile.Used for temporary operations
  in PDPREG internal functions.
  ---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE(PTR) {\
  PTR = (ds_umts_pdp_profile_type *)ds_system_heap_mem_alloc(\
                                            sizeof(ds_umts_pdp_profile_type));\
  if(NULL == PTR)\
  {\
    MSG_ERROR("Memory allocation failure for client:%d, size:%d",\
               MODEM_MEM_CLIENT_DATA,\
               sizeof(ds_umts_pdp_profile_type),0);\
    return DS_UMTS_PDP_FAIL;\
  }\
  memset((void*)PTR, 0x0, sizeof(ds_umts_pdp_profile_type));\
}

/*---------------------------------------------------------------------------
  Macro to free local pdp profile memory.Used for temporary operations
  in PDPREG internal functions.
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(PTR) {\
  ds_system_heap_mem_free(PTR);\
}

/*---------------------------------------------------------------------------
  Size of configuration file content
---------------------------------------------------------------------------*/
#define DS_UMTS_PDP_CONFIG_FILE_SIZE  ( sizeof (byte) + sizeof (byte) + \
                                        sizeof(boolean) +\
                                       ( sizeof (ds_umts_pdp_profile_type) - \
                                        DS_UMTS_PDP_PROFILE_CONTEXT_OFFSET))


/*---------------------------------------------------------------------------
  EFS directory name to hold "pdp_profiles/" : TWO additional spaces are req
  one for '/' and the other for the ASCIZ character
---------------------------------------------------------------------------*/
static char ds_umts_pdp_profile_dir_name[DS_UMTS_PDP_PROFILE_DIR_NAME_LEN + \
       1 + 1];


/*---------------------------------------------------------------------------
  EFS file name for the profiles "profileXY". Allocate an additional 
  character for the ASCIZ terminator
---------------------------------------------------------------------------*/
static char ds_umts_pdp_profile_file_name[DS_UMTS_PDP_PROFILE_FILE_NAME_LEN+1];

/*---------------------------------------------------------------------------
  Fully qualified EFS file name for the storing profile fullly qualified name
  "profileXY" for all profile types

  The fully qualified file name consists of the high level directory 
  pdp_profiles. Under the base directory there are directories for
  consolidated profile family. 
  Under each directory, there are profile files, one for each profile names
  profileXY where XY is the profile number.
  
  e.g., "pdp_profiles/profile3"

  Note: This length is dependent on DS_UMTS_PDP_PROFILE_MAX_DIR_NAME_LEN. If
  a longer length is required, the dir name length as defined by above 
  variable needs to be increased accordingly.
---------------------------------------------------------------------------*/
char ds_umts_pdp_fully_qual_profile_file_name[\
              DS_UMTS_PDP_PROFILE_MAX_DIR_NAME_LEN + \
              1 + \
              DS_UMTS_PDP_PROFILE_FILE_NAME_LEN + \
              1 ];

/*---------------------------------------------------------------------------
  Fully qualified EFS file name for the embedded call profile number file
  i.e., "pdp_profiles/embedded_call_prof_num"
---------------------------------------------------------------------------*/
static char ds_umts_pdp_fully_qual_embedded_call_file_name[\
              DS_UMTS_PDP_PROFILE_DIR_NAME_LEN + \
              1 + \
              DS_UMTS_PDP_EMBEDDED_CALL_PROF_NUM_FILE_NAME_LEN + \
              DS_UMTS_PDP_PROFILE_SUBSCRIPTION_LEN + \
              1 ];

/*---------------------------------------------------------------------------
  Fully qualified EFS file name for the tethered call profile number file
  i.e., "pdp_profiles/tethered_call_prof_num"
---------------------------------------------------------------------------*/
static char ds_umts_pdp_fully_qual_tethered_call_file_name[\
              DS_UMTS_PDP_PROFILE_DIR_NAME_LEN + \
              1 + \
              DS_UMTS_PDP_TETHERED_CALL_PROF_NUM_FILE_NAME_LEN + \
              DS_UMTS_PDP_PROFILE_SUBSCRIPTION_LEN + \
              1 ];

/*---------------------------------------------------------------------------
  Flag which stores the status of PDP registry initialization
---------------------------------------------------------------------------*/
static boolean ds_umts_pdp_registry_init_flag = FALSE;

/*---------------------------------------------------------------------------
  Scratchpad variable : Default PDP Profile storage
---------------------------------------------------------------------------*/
static ds_umts_pdp_profile_type   ds_umts_pdp_default_profile;


/*---------------------------------------------------------------------------
  Scratchpad storage to access profile data
---------------------------------------------------------------------------*/
static ds_umts_pdp_profile_type*  ds_umts_pdp_local_profile_ptr = NULL;

/*-------------------------------------------------------------------------- 
  Static variables to know if an embedded profile/tethered default profile
  has been read or not
  --------------------------------------------------------------------------*/
boolean tethered_call_profile_number_read[DS_SUBSCRIPTION_MAX];
boolean embedded_call_profile_number_read[DS_SUBSCRIPTION_MAX];

/*---------------------------------------------------------------------------
  Cache to store profile data Consolidated profiles.
  Profile families are represnted by first subscript and profiles by second
  subscript.

  Note: the profile numbers start from array subscript 1 and not 0. The
  profile 0 does not exist and ds_umts_pdp_profile_cache[][0] is unused.

  The first subscript in the array points to the profile family.
  Family CONSL -> array location 0

  Profiles start at array location 1. 
  profile 1 -> array location 1
  profile 2 -> array location 2
  ..................
  profile DS_UMTS_MAX_PDP_PROFILE_NUM -> array location DS_UMTS_MAX_PDP_PROFILE_NUM

  So ds_umts_pdp_profile_cache[0][2] will refer to profile number 2 in
  Consolidated profile family.
  So ds_umts_pdp_profile_cache[0][0] cannot be used as profile number 0 is
  not defined.
---------------------------------------------------------------------------*/

ds_umts_pdp_profile_type
 *ds_umts_pdp_profile_cache_ptr[DS_SUBSCRIPTION_MAX]
    [DS_UMTS_MAX_PDP_PROFILE_NUM+1];


#ifdef FEATURE_GOBI_MUXD_PDP_PROFILE_DIR
static byte  undp_muxd_consl_pdp_profile_dir_name_len = 0;
static char  undp_muxd_consl_pdp_profile_file_dir_name[DS_UMTS_PDP_PROFILE_CONSL_DIR_NAME_LEN+1];
#endif 

/*---------------------------------------------------------------------------
 Number of persistent profiles. NV vonfigurable value.
---------------------------------------------------------------------------*/
//static uint8 ds_3gpp_num_persistent_profiles;
/*===========================================================================
                    Forward declarations/prototypes
===========================================================================*/

static byte * ds_umts_pdp_itoa
(
  uint32 v,         /*  Value to convert to ASCII     */
  byte *s,          /*  Pointer to string for result  */
  uint16 r          /*  Conversion radix              */
);


/*---------------------------------------------------------------------------
  Cached value of profile number for embedded calls.  This value is set
  during library initialization and anytime the embedded profile number
  is updated.  Reads for the embedded profile number will return this
  value.
---------------------------------------------------------------------------*/

byte  cached_embedded_call_profile_number[DS_SUBSCRIPTION_MAX];

/*---------------------------------------------------------------------------
  Cached value of profile number for tethered calls.  This value is set
  during library initialization and anytime the tethered profile number
  is updated.  Reads for the tethered profile number will return this
  value.
---------------------------------------------------------------------------*/

byte  cached_tethered_call_profile_number[DS_SUBSCRIPTION_MAX];


/*===========================================================================
                   INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================

FUNCTION DS_UMTS_PDP_ITOA

DESCRIPTION
  This function converts an integer to ASCII, using a specified
  radix. The resulting string is null terminated.

DEPENDENCIES
  The function does not work correctly if r < 2 or r > 36.
  (r is the radix argument.)

  The length of the returned string depends on the value and the
  radix.  The worst case is the value 0xFFFFFFFF and a radix of 2,
  which returns 33 bytes (32 one's and the null terminator.)
  The array pointed to by 's' must be large enough for the returned
  string.

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS
  None
===========================================================================*/
static byte * ds_umts_pdp_itoa
(
  uint32 v,         /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  uint16 r          /*  Conversion radix              */
)
{
  byte buf[33], c;
  int n;

  n = sizeof(buf) - 1;

  buf[n] = '\0';


  do
  {
    c = (byte) (v % r);

    if (n <= 0)
    {
      DS_3GPP_MSG0_HIGH ("OVERFLOW");
      break;
    }

    buf[--n] = (byte) ( (c > 9) ? c + 'A' - 10 : c + '0');

  } while ( (v /= r) > 0);

  while ( (*s++ = buf[n++]) != 0)
      ;

  return (s-1);
} /*  ds_umts_pdp_itoa */
  
/*===========================================================================
FUNCTION DS_UMTS_PDP_CHECK_PROFILE_NUMBER

DESCRIPTION
This function checks the validity of profile number. It checks if the profile
number and profile family are in valid range and profile family is defined.
PARAMETERS
  profile_number - profile number passed. This profile number is a two byte
                   number. First byte is the profile family and the second
                   byte is the actual profile number.

DEPENDENCIES
  None

RETURN VALUE
  ds_umts_pdp_profile_status_etype

SIDE EFFECTS
  None
===========================================================================*/
static ds_umts_pdp_profile_status_etype ds_umts_pdp_check_profile_number
(
  uint16 profile_number,
  ds_umts_pdp_subs_e_type subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id) == -1)
  {
    DS_3GPP_MSG1_ERROR("Profile_number:%d not present in cache",
                        profile_number);
    return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  }

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     DS_3GPP_MSG0_ERROR("Subscription ID is invalid");
     return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  }

  return DS_UMTS_PDP_SUCCESS;
}   


/*===========================================================================
FUNCTION DS_UMTS_PDP_CHECK_PROFILE_NUMBER

DESCRIPTION
This function checks the validity of profile number. It checks if the profile
number and profile family are in valid range and profile family is defined.
PARAMETERS
  profile_number - profile number passed. This profile number is a two byte
                   number. First byte is the profile family and the second
                   byte is the actual profile number.

DEPENDENCIES
  None

RETURN VALUE
  ds_umts_pdp_profile_status_etype

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype ds_umts_pdp_check_subscription_id
(
  ds_umts_pdp_subs_e_type              subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( (subs_id < DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_1) || 
      (subs_id > DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_MAX) ) 
  {
    DS_3GPP_MSG0_ERROR("Active subscription ID is invalid");
    return DS_UMTS_PDP_INVALID_SUBS_ID;
  }

  return DS_UMTS_PDP_SUCCESS;
} 

#ifdef FEATURE_PROFILE_OTAP_ENHANCEMENTS
/*===========================================================================
FUNCTION DS_UMTS_PDP_STRING_TO_WIDE_STRING

DESCRIPTION
This function converts a single-byte string into a wide string 

PARAMETERS
  in_str  -  Pointer to null terminated string comprised of single-byte 
             characters 
  out_str -  Pointer to destination buffer to receive the wide string  
  nbytes  -  Size (in bytes) of out_str buffer. 
             If this is 0, this function does not do any conversion. 
             It returns out_str as is without any changes

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds_umts_string_to_wide_string
(
  const byte *in_str, 
  PACKED uint16 *out_str, 
  int nbytes
)
{
  int   i;
  int   nFilled;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (nbytes >= sizeof(uint16))
  {
    /*---------------------------------------------------------------------- 
      Get to the end of the input string.
    ----------------------------------------------------------------------*/
    for (i = 0; in_str[i]; i++);

    /*---------------------------------------------------------------------- 
      We are expanding out, so make certain we dont over-index...
    ----------------------------------------------------------------------*/
    nFilled = i * sizeof(uint16);

    if (nFilled >= nbytes) {
      i = (nbytes / sizeof(uint16)) - 1;
      out_str[i--] = 0;
      nFilled = nbytes;
    }
    /*---------------------------------------------------------------------- 
      Now, do a byte to uint16 conversion.
    ----------------------------------------------------------------------*/
    while (i >= 0) {
      out_str[i] = (uint16)((unsigned char)in_str[i]);
      i--;
    }
  }
}
#endif

/*===========================================================================
FUNCTION DS_UMTS_PDP_SET_QUALIFIED_PROFILE_FILE_NAME

DESCRIPTION
  This function sets the PDP qualified file name.
  
PARAMETERS
  uint16 profile_number
  
DEPENDENCIES 
  None

RETURN VALUE 
  boolean

SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_umts_pdp_set_qualified_profile_file_name
(
  uint16 profile_number
)
{
  byte                            profile_num_lower_byte;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     profile number passed in is a combination of profile family and actual
     profile number. The higher byte is profile family and lower byte is
     profile number. So get the actual profile number to append inside the 
     file name by getting the lower byte from the profile number 
  -------------------------------------------------------------------------*/
  profile_num_lower_byte = (byte)profile_number;

                                                                      
  /*lint -e534 ignoring return code from function*/                       
  strlcpy(ds_umts_pdp_fully_qual_profile_file_name,                  
              ds_umts_pdp_profile_dir_name,
              sizeof(ds_umts_pdp_fully_qual_profile_file_name) );             
  ds_umts_pdp_itoa(profile_num_lower_byte,                                           
    ((byte*)&ds_umts_pdp_profile_file_name[DS_UMTS_PDP_PROFILE_FILE_NAME_NUM_INDEX0]),
    10);                                                                  
  strlcat(ds_umts_pdp_fully_qual_profile_file_name,
              ds_umts_pdp_profile_file_name,
              sizeof(ds_umts_pdp_fully_qual_profile_file_name));
  /*lint +e534*/                                                          
  return TRUE;
}

/*===========================================================================
FUNCTION DS_UMTS_UPDATE_EMBEDDED_CALL_PROF_NUM

DESCRIPTION
  This function updates the profile number in the "embedded_call_prof_num" file
  with the value passed.  The profile number is cached to support reads when
  EFS read operations are disabled.

PARAMETERS
 subs_id : Subscription ID 
 profile_number : profile number to be written to the file
  
DEPENDENCIES 
  None

RETURN VALUE 
  TRUE       : profile number updated successfully
  FALSE      : Error in updating profile number

SIDE EFFECTS 
  None
===========================================================================*/

boolean ds_umts_update_embedded_call_prof_num
(
  ds_umts_pdp_subs_e_type subs_id,
  uint16  profile_number
)
{
  mcfg_fs_status_e_type     result;
  byte                      profile_num; /* Holds lower 8 bits of profile number*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   profile_num = (byte) profile_number;

  /*-------------------------------------------------------------------------
    Init the fully qualified profile file name to pdp_profiles directory
  -------------------------------------------------------------------------*/
  /*lint -e534 ignoring return code from function*/
  if(strlcpy( ds_umts_pdp_fully_qual_embedded_call_file_name,
                  ds_umts_pdp_profile_dir_name,
                  sizeof(ds_umts_pdp_fully_qual_embedded_call_file_name) ) >=
     (int) sizeof(ds_umts_pdp_fully_qual_embedded_call_file_name) )
  {
    return FALSE;
  }

  strlcat( ds_umts_pdp_fully_qual_embedded_call_file_name,
               DS_UMTS_PDP_PROFILE_NUM_FOR_EMBEDDED_FILE_NAME,
               sizeof(ds_umts_pdp_fully_qual_embedded_call_file_name) );

  /*-------------------------------------------------------------------------
    Open the file for write 
  -------------------------------------------------------------------------*/
  result = mcfg_fs_write(ds_umts_pdp_fully_qual_embedded_call_file_name,
                   (void *)(&profile_num),
                   sizeof(byte),
                         MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_RDWR,
                         MCFG_FS_DEFFILEMODE,
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type)subs_id);
  if ( result != MCFG_FS_STATUS_OK )
  {
    DS_3GPP_MSG2_ERROR("File write failed result=%d errno:%d",
                       result, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Set the cached embedded call profile_number for later reads
  -------------------------------------------------------------------------*/
  cached_embedded_call_profile_number[subs_id] = (byte)profile_number;
  embedded_call_profile_number_read[subs_id] = TRUE;

  return TRUE;  
}


/*===========================================================================
FUNCTION DS_UMTS_UPDATE_DEFAULT_TETHERED_PROF_NUM_PER_SUBS

DESCRIPTION
  This function updates the profile number in the "tethered_call_prof_num" file
  with the value passed.  The profile number is cached to support reads when
  EFS read operations are disabled.

PARAMETERS
  subs_id : Subscription ID 
  profile_number : profile number to be written to the file
  
DEPENDENCIES 
  None

RETURN VALUE 
  TRUE       : profile number updated successfully
  FALSE      : Error in updating profile number

SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_umts_update_default_tethered_prof_num
(
  ds_umts_pdp_subs_e_type   subs_id,
  uint16  profile_number
)
{
  mcfg_fs_status_e_type     result;
  byte                      profile_num; /* Holds lower 8 bits of profile number*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   profile_num = (byte) profile_number;

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return FALSE;
  }

  /*-------------------------------------------------------------------------
    Init the fully qualified profile file name to pdp_profiles directory
  -------------------------------------------------------------------------*/
  /*lint -e534 ignoring return code from function*/
  if( strlcpy( ds_umts_pdp_fully_qual_tethered_call_file_name,
                   ds_umts_pdp_profile_dir_name,
                   sizeof(ds_umts_pdp_fully_qual_tethered_call_file_name) ) >=
      (int) sizeof(ds_umts_pdp_fully_qual_tethered_call_file_name) )
  {
    return FALSE;
  }

  strlcat( ds_umts_pdp_fully_qual_tethered_call_file_name,
               DS_UMTS_PDP_PROFILE_NUM_FOR_TETHERED_FILE_NAME,
               sizeof(ds_umts_pdp_fully_qual_tethered_call_file_name) );

  /*-------------------------------------------------------------------------
    Open the file for write 
  -------------------------------------------------------------------------*/
  result = mcfg_fs_write(ds_umts_pdp_fully_qual_tethered_call_file_name,
                   (void *)(&profile_num),
                   sizeof(byte),
                         MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_RDWR,
                         MCFG_FS_DEFFILEMODE,
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type)subs_id);
  if ( result != MCFG_FS_STATUS_OK)
  {
    DS_3GPP_MSG2_ERROR("File write failed result=%d errno:%d",
                       result, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Set the cached tethered call profile_number for later reads
  -------------------------------------------------------------------------*/
  cached_tethered_call_profile_number[subs_id] = (byte)profile_number;
  tethered_call_profile_number_read[subs_id] = TRUE;
  
  return TRUE;  
}


/*===========================================================================
FUNCTION DS_UMTS_PDP_INIT_CACHE_DEFAULT_PROFILE_INFO

DESCRIPTION
  This function updates the cache with default profile information.
  
PARAMETERS
  proifle_num : Profile Numner
  subs_id     : Subscripiton ID

DEPENDENCIES 
  ds_umts_pdp_profile_cache_ptr should be set
  
RETURN VALUE 
  TRUE               : the operation completed successfully
  FALSE              : Fails to initialize cache.
SIDE EFFECTS 
  None
===========================================================================*/
static boolean ds_umts_pdp_init_cache_default_profile_info
(
  uint16 profile_num,
  ds_umts_pdp_subs_e_type subs_id
)
{
  ds_umts_pdp_profile_type *profile_data_ptr;
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Init the fully qualified profile file name to 
    "pdp_profiles" directory
  -----------------------------------------------------------------------*/

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_num, subs_id);

  if (cache_index <= 0)
  {
	return FALSE;
  }

  ds_umts_pdp_default_profile.context.pdp_context_number = (byte)profile_num;
  ds_umts_pdp_default_profile.profile_number = profile_num;

#ifdef FEATURE_PROFILE_OTAP_ENHANCEMENTS
  ds_umts_string_to_wide_string(
       (const byte *)ds_umts_pdp_profile_file_name,
       (uint16 *)ds_umts_pdp_default_profile.profile_name,
       sizeof(ds_umts_pdp_default_profile.profile_name));
#else
  /*lint -e534 ignoring return code from function*/
  if(strlcpy((char *)ds_umts_pdp_default_profile.profile_name,
         ds_umts_pdp_profile_file_name,
         sizeof(ds_umts_pdp_default_profile.profile_name)) >=
         (int) sizeof(ds_umts_pdp_default_profile.profile_name) )
  {
    DS_3GPP_MSG0_ERROR("Copying profile name(strlcpy) failed");
    return FALSE;
  }
  /*lint +e534*/
#endif

  /*--------------------------------------------------------------------------
    memcpy default profile data in cache.
  -------------------------------------------------------------------------*/
  profile_data_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);
  DS_UMTS_PDP_TASKLOCK_NON_L4();
  memscpy(profile_data_ptr, sizeof(ds_umts_pdp_profile_type), 
          &ds_umts_pdp_default_profile,
          sizeof(ds_umts_pdp_profile_type)); 
  DS_UMTS_PDP_TASKFREE_NON_L4();
  return TRUE;
}
/*===========================================================================
FUNCTION DS_UMTS_PDP_DELETE_FILE

DESCRIPTION
  This function deletes an efs file 
  
PARAMETERS
 None
  
DEPENDENCIES 
  None

RETURN VALUE 
  TRUE       : file deleted successfully
  FALSE      : Error in file deletion / check

SIDE EFFECTS 
  None
===========================================================================*/
static boolean ds_umts_pdp_delete_file
(
  char *file_path
)
{
  int                   result;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  result = mcfg_fs_delete(file_path, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE);

  if ( result == -1)
  {
    DS_3GPP_MSG2_ERROR("efs_unlink failed for fd=%d errno=%d.",
                       result, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  return TRUE;
} /* ds_umts_pdp_delete_file */

/*===========================================================================
FUNCTION DS_UMTS_UPDATE_PDP_PROFILE_TO_UNDEFINED

DESCRIPTION
  This function undefines the context and profile parameters for a specific
  profile. The context "valid_flg" is set to FALSE and all parameters 
  are set to subscribed (0x0) values and the "valid_flg" in the related sets 
  are set to FALSE as well. 

PARAMETERS
  proifle_num : Profile Numner
  subs_id     : Subscripiton ID
  
DEPENDENCIES 
  None

RETURN VALUE 
  TRUE       : profile updated successfully
  FALSE      : Error in updating profile

SIDE EFFECTS 
  None
===========================================================================*/
static boolean ds_umts_update_pdp_profile_to_undefined
(
  uint16  profile_number,
  ds_umts_pdp_subs_e_type subs_id
)
{
  ds_umts_pdp_profile_type         *profile_data_ptr;
  int8 cache_index = -1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);
  if (cache_index <= 0)
  {
	return FALSE;
  }
    
  profile_data_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index );
  /*--------------------------------------------------------------------------
    check valid flag and transient flag. If it is valid and 
    not transient then delete the file
  --------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------
    Delete from EFS only when file is present which means is valid and persistent.
  -----------------------------------------------------------------------*/
  if ( (profile_data_ptr->context.valid_flg == TRUE) && 
       (profile_data_ptr->transience_flag == FALSE) )
  {
    DS_3GPP_MSG1_HIGH("Profile %d file is present in EFS delete it",
                      profile_number);
    /*-----------------------------------------------------------------------
      Delete the file from EFS.
    -----------------------------------------------------------------------*/
    ds_umts_pdp_delete_file((char*)ds_umts_pdp_fully_qual_profile_file_name);
    }

    /*--------------------------------------------------------------------------
    memcpy default profile data in cache, Update the file data with default
    values.
    -------------------------------------------------------------------------*/
  ds_umts_pdp_init_cache_default_profile_info(profile_number, subs_id);
  return TRUE;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ALL_INFO_FROM_CACHE
DESCRIPTION
  This function retrieves the profile information from the cache for a
  specific profile number
  
PARAMETERS
  profile_number  : profile number
  subs_id: Subscription id
  profile_data_ptr    : pointer to store the profile information

DEPENDENCIES 

RETURN VALUE 
  TRUE             : the operation completed successfully
  FALSE            : The operation did not complete successfully.
SIDE EFFECTS 
  None
===========================================================================*/
static boolean ds_umts_get_pdp_profile_all_info_from_cache
(
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_profile_type  *profile_data_ptr
)
{
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return FALSE;
  }
  /*-----------------------------------------------------------------------
    Get the profile information from the profile cache table.
  -----------------------------------------------------------------------*/
  DS_UMTS_PDP_TASKLOCK_NON_L4();
  memscpy( (void*) profile_data_ptr,
  	    sizeof(ds_umts_pdp_profile_type),
            (void *)DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index),
            sizeof(ds_umts_pdp_profile_type) );
  DS_UMTS_PDP_TASKFREE_NON_L4();

  return TRUE;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_FROM_CACHE

DESCRIPTION
  This function retrieves the context information from the cache for a
  specific profile number
  
PARAMETERS
  profile_number  : profile number
  pdp_context_data: pointer to store the context information

DEPENDENCIES 

RETURN VALUE 
  TRUE             : the operation completed successfully
  FALSE            : error code returned by file system functions
SIDE EFFECTS 
  None
===========================================================================*/
static boolean ds_umts_get_pdp_profile_context_info_from_cache
(
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_context_type  *pdp_context_data 
)
{
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return FALSE;
  }
  /*-----------------------------------------------------------------------
    Get the profile context information from the cache table.
  -----------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();
  memscpy( (void*) pdp_context_data,
  	    sizeof(ds_umts_pdp_context_type),
            (void*)&(DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->context),
             sizeof(ds_umts_pdp_context_type) );
  DS_UMTS_PDP_TASKFREE_NON_L4();
  return TRUE;
}

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION DS_UMTS_PDP_PROFILE_REGISTRY_INIT_INTERNAL

DESCRIPTION
  This function initializes the PDP profile registry. It checks the EFS to 
  see if the required profile files are present. If the files are not present,
  it creates the files and initializes them.
  The embedded call profile number is read to initialize the cached value.
  The default embedded call profile is reset after initialization if profile
  data was undefined.
  The default atcop and rmnet call profile is reset after initialization if profile
  data was undefined.
  
PARAMETERS 
  None

DEPENDENCIES 
  Should be called only ONCE during startup.

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS  - initialization was successfull
  DS_UMTS_PDP_FAIL     - initialization failed

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype ds_umts_pdp_registry_init_internal(void)
{
  static boolean ds_3gpp_reg_init_called_once = FALSE;
  EFSDIR *dirp = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds_3gpp_reg_init_called_once) 
  {
    ds_3gpp_reg_init_called_once = TRUE;
  }
  else
  {
    /*-------------------------------------------------------------------------
      PDP reg has already been inited. Just return Success
    -------------------------------------------------------------------------*/
    return DS_UMTS_PDP_SUCCESS;
  }

  memset(tethered_call_profile_number_read,0,sizeof(boolean)*DS_SUBSCRIPTION_MAX);
  memset(embedded_call_profile_number_read,0,sizeof(boolean)*DS_SUBSCRIPTION_MAX);
  memset(cached_embedded_call_profile_number,0,sizeof(byte)*DS_SUBSCRIPTION_MAX);
  memset(cached_tethered_call_profile_number,0,sizeof(byte)*DS_SUBSCRIPTION_MAX);

  /* read number of persistent profile from NV */
  /*-------------------------------------------------------------------------- 
    Commenting out some portions for the new rearchitecture, MH would not need to
    write to EFS
    --------------------------------------------------------------------------*/

  /* Verify the ds_3gpp_num_persistent_profiles value */
  /*-------------------------------------------------------------------------
    Set the default values for a profile. All fields are set to 0x0.
    The schema version is set based on header constant.
    APN and profile name being ASCIZ strings are initialized to null strings
    Other parameters have the value '0' as subscribed
    Flags have the value 0x0 indicating that the set is invalid
  -------------------------------------------------------------------------*/
  memset((void*)&ds_umts_pdp_default_profile,
         0x0,sizeof(ds_umts_pdp_profile_type) );
  ds_umts_pdp_default_profile.version = DS_UMTS_PROFILE_VERSION;
  ds_umts_pdp_default_profile.apn_bearer = 
                        DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_ALL;

  ds_umts_pdp_default_profile.max_pdn_conn_per_blk = 
    DS_UMTS_PDP_DEF_MAX_PDN_CONN_PER_BLK;
  ds_umts_pdp_default_profile.pdn_req_wait_time = 
    DS_UMTS_PDP_DEF_PDN_REQ_WAIT_TIME;
  ds_umts_pdp_default_profile.max_pdn_conn_time = 
    DS_UMTS_PDP_DEF_MAX_PDN_CONN_TIME;

  /*-------------------------------------------------------------------------
    By default all profiles now are created transient.
  -------------------------------------------------------------------------*/
  ds_umts_pdp_default_profile.transience_flag = TRUE;



  /*-------------------------------------------------------------------------
    Check the file system to see if the profiles are present
    - If not create and initialize the files
    - Initialize the Directory Name to the qualified name "pdp_profiles/"
  -------------------------------------------------------------------------*/
  /*lint -e534 ignoring return code from function*/
  if(strlcpy(ds_umts_pdp_profile_dir_name,DS_UMTS_PDP_PROFILE_DIR_NAME,
                 sizeof(ds_umts_pdp_profile_dir_name) ) >=
     (int) sizeof(ds_umts_pdp_profile_dir_name) )
  {
    return DS_UMTS_PDP_FAIL;
  }

  ds_umts_pdp_profile_dir_name [DS_UMTS_PDP_PROFILE_DIR_NAME_LEN] = DS_FS_DIRECTORY_CHAR;
  ds_umts_pdp_profile_dir_name [DS_UMTS_PDP_PROFILE_DIR_NAME_LEN +1] = '\0';

  /*lint +e534*/
    /*-----------------------------------------------------------------------
      Check the file system to see if the profiles are present
      - If not create and initialize the files
      - Initialize the Directory Name to the qualified name 
        "pdp_profiles"
    -----------------------------------------------------------------------*/
    /*lint -e534 ignoring return code from function*/

#ifdef FEATURE_GOBI_MUXD_PDP_PROFILE_DIR
  if( undp_muxd_consl_pdp_profile_dir_name_len == UNDP_PDP_PROFILE_CONSL_DIR_NAME_INDEX) 
  {
      ds_umts_pdp_profile_consl_dir_name [UNDP_PDP_PROFILE_CONSL_DIR_NAME_INDEX] = DS_FS_DIRECTORY_CHAR;
      ds_umts_pdp_profile_consl_dir_name [UNDP_PDP_PROFILE_CONSL_DIR_NAME_INDEX +1] = '\0';
  }
#endif /* FEATURE_GOBI_MUXD_PDP_PROFILE_DIR */
    /*lint +e534*/

  dirp = mcfg_fs_opendir(ds_umts_pdp_profile_dir_name, MCFG_FS_TYPE_EFS);
  if ((dirp == NULL) || (EFSDIR *)-1 == dirp) 
  {
    DS_3GPP_MSG0_LOW("pdp_profiles directory not present, creating it");
    if (-1 == mcfg_mkdir(ds_umts_pdp_profile_dir_name, 0777, MCFG_FS_TYPE_EFS))
    {
      DS_3GPP_MSG0_ERROR("Could not create pdp_profiles directory");
    }
  }
  
  ds_umts_pdp_registry_init_flag = TRUE;

  DS_3GPP_MSG0_HIGH("PDP profiles initialized ");
  
  return DS_UMTS_PDP_SUCCESS;
}   

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ALL_DATA_INTERNAL

DESCRIPTION
  This function retrieves all the parameters associated with a profile number. 
  
  NOTE:
  No check is done to see if a valid context is defined. The caller has to 
  check if the context and associated parameter set is defined and use it
  accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  profile_data_ptr    : pointer to store the profile information

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_all_data_internal
(
  uint16                   profile_number, 
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_profile_type *profile_data_ptr
)
{
  boolean  status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();

  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
  {
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 
  }

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
     DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return DS_UMTS_PDP_INVALID_SUBS_ID;
  }

  /*-------------------------------------------------------------------------
    Read the profile from the cache
  -------------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_info_from_cache(
            profile_number,
            subs_id, 
            profile_data_ptr);

  if( status != TRUE )
  {
    DS_3GPP_MSG1_ERROR("PDP profile access error:%d",status);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
  
  /*-------------------------------------------------------------------------
    Pass the profile as is to the caller. Caller has to check
    if the context is defined explicitly (check the valid_flg) and 
    also for the validity for other parameter sets
  -------------------------------------------------------------------------*/
  return DS_UMTS_PDP_SUCCESS;
}
/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NAME_UTF16_INTERNAL

DESCRIPTION
  This function retrieves the name of the specified profile number that is
  stored in UTF16 format. 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  profile_name_buf    : pointer to store the name string (UTF16 string)
  profile_name_buf_len: length of the profile_name_buf passed (UTF16 words)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of profile_name_buf is shorter
                                      than required

SIDE EFFECTS 
  None
===========================================================================*/
/*ARGSUSED*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_name_utf16_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                *profile_name_buf,
  byte                  profile_name_buf_len
)
{
#ifndef FEATURE_PROFILE_OTAP_ENHANCEMENTS
  DS_3GPP_MSG0_ERROR("Format not supported");
  return DS_UMTS_PDP_FORMAT_NOT_SUPPORTED;
#else
  boolean  status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
     DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return DS_UMTS_PDP_INVALID_SUBS_ID;
  }

  /*-------------------------------------------------------------------------
    Check if the buffer passed in is atleast the length required
  -------------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_OUT_BUFF_LEN(profile_name_buf_len,
        DS_UMTS_MAX_PROFILE_NAME_LEN);
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

  /*-------------------------------------------------------------------------
    Read the profile from the cache.
  -------------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_info_from_cache(
                                            profile_number,
                                            subs_id, 
                                            ds_umts_pdp_local_profile_ptr);
  if( status != TRUE )
  {
    DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);
    DS_3GPP_MSG3_ERROR("PDP profile access error:%d",status,0,0);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
  
  /*-------------------------------------------------------------------------
    Profile name is not dependent on having a valid profile. Just
    return the string currently in the profile
  -------------------------------------------------------------------------*/
  memscpy( (void*)profile_name_buf,
           sizeof(uint16)*profile_name_buf_len,
           (void*) ds_umts_pdp_local_profile_ptr->profile_name,
           sizeof(uint16)*profile_name_buf_len );
  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return DS_UMTS_PDP_SUCCESS;
#endif
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NAME_INTERNAL

DESCRIPTION
  This function retrieves the name of the specified profile number . 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  profile_name_buf    : pointer to store the name string
  profile_name_buf_len: length of the profile_name_buf passed

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of profile_name_buf is shorter
                                      than required

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_name_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                  *profile_name_buf,
  byte                  profile_name_buf_len
)
{
#ifdef FEATURE_PROFILE_OTAP_ENHANCEMENTS
  DS_3GPP_MSG3_ERROR("Format not supported", 0, 0, 0);
  return DS_UMTS_PDP_FORMAT_NOT_SUPPORTED;
#else
  boolean  status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
     DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return DS_UMTS_PDP_INVALID_SUBS_ID;
  }

  /*-------------------------------------------------------------------
    Check if the buffer passed in is atleast the length required
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_OUT_BUFF_LEN(profile_name_buf_len,
        DS_UMTS_MAX_PROFILE_NAME_LEN);
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

  /*-------------------------------------------------------------------
    Read the profile from the cache
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_info_from_cache(
                                            profile_number,
                                            subs_id, 
                                            ds_umts_pdp_local_profile_ptr);
  if( status != TRUE )
  {
    DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);
    DS_3GPP_MSG1_ERROR("PDP profile access error:%d",status);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
  
  /*-------------------------------------------------------------------
    Profile name is not dependent on having a valid profile. Just
    return the string currently in the profile
  -------------------------------------------------------------------*/
  /*lint -e534 ignoring return code from function*/
  strlcpy( (char*)profile_name_buf,
          (char*)ds_umts_pdp_local_profile_ptr->profile_name,
          profile_name_buf_len);
  /*lint +e534*/

  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return DS_UMTS_PDP_SUCCESS;
#endif /* FEATURE_PROFILE_OTAP_ENHANCEMENTS */
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_FLAG_INTERNAL

DESCRIPTION
  This function retrieves the "request_pcscf_address" flag specified for a 
  profile. 
  The "request_pcscf_address" indicates if the pcscf address should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  request_pcscf_address_flag : pointer to store request_pcscf_address_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_request_pcscf_address_flag_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *request_pcscf_address_flag
)
{
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the request_pcscf_address_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  

  *request_pcscf_address_flag = 
      (DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index))->request_pcscf_address_flag;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION 
  DS_UMTS_GET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_USING_DHCP_FLAG_INTERNAL

DESCRIPTION
  This function retrieves the "request_pcscf_address_using_dhcp_flag" flag
  specified for a profile. 
  The "request_pcscf_address_using_dhcp_flag" indicates if the pcscf address
  should be requested for this profile using DHCP or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  request_pcscf_address_using_dhcp_flag : pointer to store
                                        request_pcscf_address_using_dhcp_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_request_pcscf_address_using_dhcp_flag_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *request_pcscf_address_using_dhcp_flag
)
{
   int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 
  
  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 
  
   /*-------------------------------------------------------------------
    Read the otap_enabled_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();
  *request_pcscf_address_using_dhcp_flag = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->
      request_pcscf_address_using_dhcp_flag;

  DS_UMTS_PDP_TASKFREE_NON_L4();
  
  return DS_UMTS_PDP_SUCCESS;  
}

/*===========================================================================
FUNCTION 
  DS_UMTS_GET_PDP_PROFILE_REQUEST_DNS_ADDRESS_USING_DHCP_FLAG_INTERNAL

DESCRIPTION
  This function retrieves the "request_dns_address_using_dhcp_flag" flag
  specified for a profile. 
  The "request_dns_address_using_dhcp_flag" indicates if the dns address
  should be requested for this profile using DHCP or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  request_dns_address_using_dhcp_flag : pointer to store
                                        request_dns_address_using_dhcp_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_request_dns_address_using_dhcp_flag_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *request_dns_address_using_dhcp_flag
)
{
   int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 
  
  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 
  
   /*-------------------------------------------------------------------
    Read the otap_enabled_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();
  *request_dns_address_using_dhcp_flag = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->
      request_dns_address_using_dhcp_flag;

  DS_UMTS_PDP_TASKFREE_NON_L4();
  
  return DS_UMTS_PDP_SUCCESS;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IM_CN_FLAG_INTERNAL

DESCRIPTION
  This function retrieves the "im_cn" flag specified for a 
  profile.
  The "im_cn" indicates if the dedicated context should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  im_cn_flag : pointer to store im_cn_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_im_cn_flag_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *im_cn_flag
)
{
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the im_cn_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *im_cn_flag = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->
      im_cn_flag;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IPV6_PREFIX_DELEGATION_INTERNAL

DESCRIPTION
  This function retrieves the "ipv6_prefix_delegation" flag specified for a 
  profile.
  The "im_cn" indicates if the dedicated context should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  ipv6_prefix_delegation : pointer to store ipv6_prefix_delegation


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_ipv6_prefix_delegation_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *ipv6_delegation_flag
)
{
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the im_cn_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *ipv6_delegation_flag = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->
      ipv6_prefix_delegation_flag;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
}
/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_DISABLE_FLAG_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_diable_flag" mechanism specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  apn_disable_flag : pointer to the boolean flag

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_apn_disable_flag_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean  *apn_disable_flag
)
{
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the im_cn_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *apn_disable_flag = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->apn_disable_flag;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
} /* ds_umts_get_pdp_profile_context_info_apn_disable_flag_internal */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_INACTIVITY_TIMER_VAL_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "inactivity_timer_val" value specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number       : profile number
  subs_id :Subscription id. 
  inactivity_timer_val : pointer to the inactivity_timer_val

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_inactivity_timer_val_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32   *inactivity_timer_val
)
{
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the im_cn_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *inactivity_timer_val = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->
      inactivity_timer_val;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
} /* ds_umts_get_pdp_profile_inactivity_timer_val_internal */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_CLASS_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_class" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  apn_class        : pointer to the uint8 (to hold APN class)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_apn_class_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint8    *apn_class
)
{
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the APN class info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *apn_class = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->apn_class;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
} /* ds_umts_get_pdp_profile_apn_class_internal */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_BEARER_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_bearer" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  apn_bearer       : pointer to the uint8 (to hold APN class)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_apn_bearer_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint8    *apn_bearer
)
{
   int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the APN class info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *apn_bearer = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->apn_bearer;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
} /* ds_umts_get_pdp_profile_apn_bearer_internal */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MCC_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "MCC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  mcc              : pointer to the uint16 (to hold MCC)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_mcc_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16   *mcc
)
{
   int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the mcc info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *mcc = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->mcc;
  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
} /* ds_umts_get_pdp_profile_mcc_internal */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MNC_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "MNC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  mnc              : pointer to the struct (to hold MNC)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_mnc_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_3gpp_mnc_type    *mnc
)
{
   int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the mnc class info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *mnc = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->mnc;
  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
} /* ds_umts_get_pdp_profile_mnc_internal */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_OPERATOR_RESERVED_PCO_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "MNC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  mnc              : pointer to the uint16 (to hold operator reserved PCO)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_operator_reserved_pco_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16   *pco
)
{
   int8 cache_index = -1;
    /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the operator_reserved_pco class info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *pco = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->operator_reserved_pco;
  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
} /* ds_umts_get_pdp_profile_operator_reserved_pco_internal */

#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MAX_PDN_CONN_PER_BLK_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  Max PDN Conn Per Blk for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  max_pdn_conn_per_blk: pointer to the uint16 (to hold max_pdn_conn_per_blk)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_max_pdn_conn_per_blk_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16   *max_pdn_conn_per_blk
)
{
   int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the mnc class info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *max_pdn_conn_per_blk = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->
     max_pdn_conn_per_blk;
  DS_UMTS_PDP_TASKFREE_NON_L4();

  DS_3GPP_MSG1_MED("max_pdn_conn_per_blk read is %d", *max_pdn_conn_per_blk);

  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_PDN_REQ_WAIT_TIME_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  PDN Req Wait Time for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  pdn_req_wait_time: pointer to the uint16 (to hold pdn_req_wait_time)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
ds_umts_get_pdp_profile_pdn_req_wait_time_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16    *pdn_req_wait_time
)
{
   int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the mnc class info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *pdn_req_wait_time = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->pdn_req_wait_time;
  DS_UMTS_PDP_TASKFREE_NON_L4();

  DS_3GPP_MSG1_MED("pdn_req_wait_time read is %d", *pdn_req_wait_time);
  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MAX_PDN_CONN_TIME_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  Max PDN Conn Time for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  max_pdn_conn_time: pointer to the uint16 (to hold max_pdn_conn_time)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_max_pdn_conn_time_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16   *max_pdn_conn_time
)
{
   int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the mnc class info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *max_pdn_conn_time = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->max_pdn_conn_time;
  DS_UMTS_PDP_TASKFREE_NON_L4();

  DS_3GPP_MSG1_MED("max_pdn_conn_time read is %d", *max_pdn_conn_time);
  return DS_UMTS_PDP_SUCCESS;
} /* ds_umts_get_pdp_profile_max_pdn_conn_time_internal */

#endif /*FEATURE_DATA_LTE*/

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_USER_APP_DATA_INTERNAL
===========================================================================*/
/** 
  
  This function gets the value of parameter
  "user_app_data" for a specified profile number. 

  @profile_number            : profile number
  subs_id :Subscription id. 
  @user_app_data             : uint32 value being passed in for the
                               user_app_data

  DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none

*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_user_app_data_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32    *user_app_data
)
{
 int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *user_app_data = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->user_app_data;
  DS_UMTS_PDP_TASKFREE_NON_L4();

  DS_3GPP_MSG1_MED("user_app_data read is %d", *user_app_data);
  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ROAMING_DISALLOWED_FLAG_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "roaming_disallowed_flag" mechanism specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number          : profile number
  subs_id :Subscription id. 
  roaming_disallowed      : pointer to the boolean flag

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_roaming_disallowed_flag_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean  *roaming_disallowed
)
{
  int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
    return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the im_cn_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *roaming_disallowed = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->roaming_disallowed;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_PDN_DISCON_WAIT_TIME_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "pdn_discon_wait_time" mechanism specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number          : profile number
  subs_id :Subscription id. 
  pdn_discon_wait_time    : pointer to the pdn_discon_wait_time

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_pdn_discon_wait_time_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint8    *pdn_discon_wait_time
)
{
  int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
    return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the im_cn_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *pdn_discon_wait_time = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->pdn_discon_wait_time;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_TRANSIENCE_FLAG

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "transience_flag" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number         : profile number
  subs_id :Subscription id. 
  transience_flag        : pointer to boolean (to hold transience_flag)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_transience_flag
(
  uint16     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean    *transience_flag
)
{

   int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the transience flag from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *transience_flag = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->transience_flag;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
  
} /* ds_umts_get_pdp_profile_transience_flag */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_OTAP_NAPID_INTERNAL

DESCRIPTION
  This function retrieves the NAPID of the specified profile number . 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number          : profile number
  subs_id                 :Subscription id. 
  otap_napid_buf          : pointer to store the napid string (UTF8 string)
  otap_napid_buf_len      : length of the otap_napid_buf passed (UTF8 bytes)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of otap_napid_buf is shorter
                                      than required

SIDE EFFECTS 
  None
===========================================================================*/
/*ARGSUSED*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_otap_napid_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                  *otap_napid_buf,
  byte                  otap_napid_buf_len
)
{
#ifndef FEATURE_PROFILE_OTAP_ENHANCEMENTS
  DS_3GPP_MSG0_ERROR("PDP profile API not supported");
  return DS_UMTS_PDP_API_NOT_SUPPORTED;
#else
  boolean  status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM;

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
     DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return DS_UMTS_PDP_INVALID_SUBS_ID;
  }

  /*-------------------------------------------------------------------
    Check if the buffer passed in is atleast the length required
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_OUT_BUFF_LEN(otap_napid_buf_len,
        DS_UMTS_MAX_OTAP_NAPID_LEN);
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

  /*-------------------------------------------------------------------
    Read the profile from the cache
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_info_from_cache(
                                            profile_number,
                                            subs_id, 
                                            ds_umts_pdp_local_profile_ptr);
  if( status != TRUE )
  {
    DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);
    DS_3GPP_MSG3_ERROR("PDP profile access error:%d",status,0,0);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
  
  /*-------------------------------------------------------------------
   Return the napid currently in the profile.
  -------------------------------------------------------------------*/
  memscpy( (void*)otap_napid_buf,
           otap_napid_buf_len,
           (void*)ds_umts_pdp_local_profile_ptr->otap_napid,
           otap_napid_buf_len );
  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return DS_UMTS_PDP_SUCCESS;
#endif
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CLAT_ENABLED_INTERNAL

DESCRIPTION
  This function retrieves the "clat_enabled" flag specified for a 
  profile.
  The "im_cn" indicates if the dedicated context should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  clat_enabled : pointer to store clat_enabled


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_clat_enabled
(
  uint16                  profile_number,
  ds_umts_pdp_subs_e_type subs_id,
  boolean                 *clat_enabled
)
{
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
    return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  }

  /*-------------------------------------------------------------------
    Read the im_cn_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();
  *clat_enabled= DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->clat_enabled;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IS_READ_ONLY_INTERNAL

DESCRIPTION
  This function retrieves the "read_only_flag" specified for a profile. 
  The "read_only_flag" indicates if the profile is read-only (TRUE) or 
  not (FALSE)
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  read_only_flag  : pointer to store the read_only_flag

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_is_read_only_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *read_only_flag
)
{
   int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the read_only_flag info from the cache table.
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_TASKLOCK_NON_L4();
  *read_only_flag = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->read_only_flag;
  DS_UMTS_PDP_TASKFREE_NON_L4();
  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IS_OTAP_ENABLED_INTERNAL

DESCRIPTION
  This function retrieves the "otap_enabled_flag" specified for a profile. 
  The "otap_enabled_flag" indicates if the context is otap_enabled (TRUE) or 
  not (FALSE)
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number     : profile number
  subs_id :Subscription id. 
  otap_enabled_flag  : pointer to store the otap_enabled_flag

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
 ds_umts_get_pdp_profile_is_otap_enabled_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *otap_enabled_flag
)
{

   int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the otap_enabled_flag info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *otap_enabled_flag = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->otap_enabled_flag;
  DS_UMTS_PDP_TASKFREE_NON_L4();
  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_INTERNAL

DESCRIPTION
  This function retrieves the context information for a profile number. 
  This information is retrieved only if the context definition is valid
  (valid_flg for context definition is TRUE)

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_context_data: pointer to store the context information

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_context_info_internal
(
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_context_type  *pdp_context_data
)
{
  boolean status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  /*-------------------------------------------------------------------
    Read the context info from the cache. 
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_context_info_from_cache(
                                                profile_number, subs_id, 
                                                pdp_context_data );
  if( status != TRUE )
  {
    DS_3GPP_MSG1_LOW("PDP profile access error %d",status);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }
  
  if( !pdp_context_data->valid_flg )
  {
    DS_3GPP_MSG1_LOW("Context not defined %d", profile_number);
    return DS_UMTS_PDP_CONTEXT_NOT_DEFINED;
  }

  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_IS_VALID_INTERNAL

DESCRIPTION
  This function retrieves the "valid_flg" specified in the context 
  information for a profile number. The "valid_flg" indicates if the 
  context has been defined (TRUE) or not (FALSE)

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  valid_flg      : pointer to store the valid_flg

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_context_info_is_valid_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *valid_flg
)
{
  int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the valid_flg info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();
  *valid_flg = 
        DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->context.valid_flg;
  DS_UMTS_PDP_TASKFREE_NON_L4();
  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_PDP_ADDRESS_INTERNAL

DESCRIPTION
  This function retrieves the "pdp address" specified in the context 
  information for a profile number. This parameter is retrieved only 
  if a valid context is defined for the profile specified.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_address     : pointer to store the pdp address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_context_info_pdp_address_internal
(  
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_addr_type     *pdp_address 
)
{
  ds_umts_pdp_profile_status_etype status;
  ds_umts_pdp_context_type    pdp_context_data;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    read the context info from the cache. 
    ds_umts_pdp_get_profile_context_info_from_cache
    performs err check on profile number & init flag & returns
    a context only if it is valid
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_context_info_per_subs(
            profile_number,
            subs_id, 
            &pdp_context_data);

  if( status != DS_UMTS_PDP_SUCCESS )
  {
    DS_3GPP_MSG1_ERROR("PDP profile access error:%d",status);
  }
  else
  {
    /*-------------------------------------------------------------------
      Valid context , retrieve the address
    -------------------------------------------------------------------*/
    *pdp_address = pdp_context_data.pdp_addr;
  }

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_APN_INTERNAL

DESCRIPTION
  This function retrieves the "apn" specified in the context information 
  for a profile number. This parameter is retrieved only if a valid 
  context is defined for the profile specified.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  apn_buf         : pointer to store the apn string
  apn_buf_len     : length of the buffer pointed by apn_buf

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of apn_buf is shorter than 
                                      required

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_context_info_apn_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                  *apn_buf,
  byte                  apn_buf_len
)
{
  ds_umts_pdp_profile_status_etype status;
  ds_umts_pdp_context_type    pdp_context_data;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Check if we have been initialized & the in/out paramaters are correct
    ds_umts_pdp_get_profile_context_info_from_cache
    performs err check on profile number & init flag & returns
    a context only if it is valid .so just check for the output buffer len
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_OUT_BUFF_LEN(apn_buf_len,DS_UMTS_MAX_APN_STRING_LEN);

  /*-------------------------------------------------------------------
    read the context info from the file. An error could also indicate 
    that the context has not been defined
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_context_info_per_subs(
                                           profile_number,
                                           subs_id,
                                           &pdp_context_data);

  if( status != DS_UMTS_PDP_SUCCESS )
  {
    DS_3GPP_MSG1_ERROR("PDP profile access error:%d",status);
  }
  else
  {
    /*-------------------------------------------------------------------
      Valid context , retrieve the apn
    -------------------------------------------------------------------*/
    /*lint -e534 ignoring return code from function*/
    strlcpy((char*)apn_buf,(char*)pdp_context_data.apn, apn_buf_len);
    /*lint +e534*/
  }

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_IPV4_ADDR_ALLOC_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the "ipv4
  addr alloc" mechanism specified in the context information for a profile 
  number. This parameter is retrieved only if a valid context is defined for 
  the profile specified.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  ipv4_addr_alloc : pointer to store the ipv4 addr alloc

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_context_info_ipv4_addr_alloc_internal
(
  uint16                              profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_ipv4_addr_alloc_e_type *ipv4_addr_alloc
)
{
  ds_umts_pdp_profile_status_etype status;
  ds_umts_pdp_context_type    pdp_context_data;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    read the context info from the cache. 
    ds_umts_pdp_get_profile_context_info_from_cache
    performs err check on profile number & init flag & returns
    a context only if it is valid
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_context_info_per_subs(
            profile_number,
            subs_id, 
            &pdp_context_data);

  if( status != DS_UMTS_PDP_SUCCESS )
  {
    DS_3GPP_MSG1_ERROR("PDP profile access error:%d",status);
  }
  else
  {
    /*-------------------------------------------------------------------
      Valid context , retrieve the address
    -------------------------------------------------------------------*/
    *ipv4_addr_alloc = pdp_context_data.ipv4_addr_alloc;
  }

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_INTERNAL

DESCRIPTION
  This function retrieves the authentication information for a profile number.

  The auth info is returned ir-respective of the state of the context definition
  for the profile.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_auth_data   : pointer to store the authentication information

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_auth_info_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_auth_type *pdp_auth_data
)
{
  ds_umts_pdp_profile_status_etype status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

  /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    The profile is returned ONLY if a valid context is defined.
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the auth info 
    -------------------------------------------------------------------*/
    memscpy( (void*) pdp_auth_data,
             sizeof(ds_umts_pdp_auth_type),
             (void*) &(ds_umts_pdp_local_profile_ptr->auth),
             sizeof(ds_umts_pdp_auth_type) );
  }

  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_TYPE_INTERNAL

DESCRIPTION
  This function retrieves the "authentication type" specified in the
  authentication information for a profile number . This parameter is 
  retrieved ir-respective of the state of the context definition of
  the profile

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  auth_type       : pointer to store the authentication type

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_auth_info_type_internal
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_auth_enum_type *auth_type
)
{
  ds_umts_pdp_profile_status_etype status;
  ds_umts_pdp_auth_type  pdp_auth_data;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Get the auth info completely and then just return the auth type
    ds_umts_get_pdp_profile_auth_info performs checks on init flag and
    profile number. 
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_auth_info(
                                        profile_number,
                                        &pdp_auth_data);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the auth type 
    -------------------------------------------------------------------*/
    *auth_type = pdp_auth_data.auth_type;
  }
  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_USERNAME_INTERNAL

DESCRIPTION
  This function retrieves the "username" specified in the authentication 
  information for a profile number . This parameter is retrieved 
  ir-respective of the state of the context definition of the profile

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  username_buf    : pointer to store the username string
  username_buf_len: length of the username_buf passed

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of username_buf is shorter than 
                                      required

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_auth_info_username_internal
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                       *username_buf,
  byte                       username_buf_len
)
{
  ds_umts_pdp_profile_status_etype status;
  ds_umts_pdp_auth_type  pdp_auth_data;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /*-------------------------------------------------------------------
    Get the auth info completely and then just return the username
    ds_umts_get_pdp_profile_auth_info performs checks on init flag and
    profile number. 
  
    Check the length of the output buffer here.
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_OUT_BUFF_LEN(username_buf_len,DS_UMTS_MAX_QCPDP_STRING_LEN);

  status = ds_umts_get_pdp_profile_auth_info(
                                        profile_number,
                                        &pdp_auth_data);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the username which is a ASCIZ string
    -------------------------------------------------------------------*/
    /*lint -e534 ignoring return code from function*/
    strlcpy((char*)username_buf,(char*)pdp_auth_data.username,
                username_buf_len);
    /*lint +e534*/
  }
  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_PASSWORD_INTERNAL

DESCRIPTION
  This function retrieves the password specified in the authentication 
  information for a profile number . This parameter is retrieved 
  ir-respective of the state of the context definition of the profile

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  password_buf    : pointer to store the password string
  password_buf_len: length of the password_buf passed

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of password_buf is shorter than 
                                      required

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_auth_info_password_internal
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                       *password_buf,
  byte                       password_buf_len
)
{
  ds_umts_pdp_profile_status_etype status;
  ds_umts_pdp_auth_type  pdp_auth_data;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /*-------------------------------------------------------------------
    Get the auth info completely and then just return the password
    ds_umts_get_pdp_profile_auth_info performs checks on init flag and
    profile number. 
  
    Check the length of the output buffer here.
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_OUT_BUFF_LEN(password_buf_len,DS_UMTS_MAX_QCPDP_STRING_LEN);

  status = ds_umts_get_pdp_profile_auth_info(
                                        profile_number,
                                        &pdp_auth_data);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the password which is a ASCIZ string
    -------------------------------------------------------------------*/
    /*lint -e534 ignoring return code from function*/
    strlcpy((char*) password_buf,(char *) pdp_auth_data.password,
                password_buf_len);
    /*lint +e534*/
  }
  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_UMTS_QOS_REQ_INFO_INTERNAL

DESCRIPTION
  This function retrieves the UMTS qos parameters (requested) specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_umts_qos_data   : pointer to store the QOS parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_umts_qos_req_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_umts_qos_params_type *pdp_umts_qos_data
)
{
  ds_umts_pdp_profile_status_etype status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );
 
 /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    Note: the UMTS QOS parameters might be invalid & not defined
    The caller has to handle this scenario by checking the "valid_flg"
    of the UMTS parameters returned
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the UMTS QOS info.
    -------------------------------------------------------------------*/
    memscpy( (void*) pdp_umts_qos_data,
             sizeof(ds_umts_umts_qos_params_type),
             (void *) (&(ds_umts_pdp_local_profile_ptr->qos_request_umts)),
             sizeof(ds_umts_umts_qos_params_type) );
  }

  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_UMTS_QOS_MIN_INFO_INTERNAL

DESCRIPTION
  This function retrieves the UMTS qos parameters (minimum) specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_umts_qos_data   : pointer to store the QOS parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_umts_qos_min_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_umts_qos_params_type *pdp_umts_qos_data
)
{
  ds_umts_pdp_profile_status_etype status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

  /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    Note: the UMTS QOS parameters might be invalid & not defined
    The caller has to handle this scenario by checking the "valid_flg"
    of the UMTS parameters returned
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the UMTS QOS info.
    -------------------------------------------------------------------*/
    memscpy( (void*) pdp_umts_qos_data,
             sizeof(ds_umts_umts_qos_params_type),
             (void *) (&(ds_umts_pdp_local_profile_ptr->qos_minimum_umts)),
             sizeof(ds_umts_umts_qos_params_type) );
  }

  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_SRC_STAT_DESC_REQ_INFO_INTERNAL

DESCRIPTION
  This function retrieves the src_stat_desc value specified for a
  profile number .

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  src_stat_desc   : pointer to store the src_stat_desc

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_src_stat_desc_req_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32                       *src_stat_desc
)
{
  ds_umts_pdp_profile_status_etype status;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );
 
 /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    Note: the UMTS QOS parameters might be invalid & not defined
    The caller has to handle this scenario by checking the "valid_flg"
    of the UMTS parameters returned
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the Req Source Statistiocs Descriptor info.
    -------------------------------------------------------------------*/
    memscpy( (void*) src_stat_desc,
             sizeof(uint32),
             (void *) (&(ds_umts_pdp_local_profile_ptr->src_stat_desc_req)),
             sizeof(uint32) );
  }

  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_SRC_STAT_DESC_MIN_INFO_INTERNAL

DESCRIPTION
  This function retrieves the src_stat_desc value specified for a
  profile number .

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  src_stat_desc   : pointer to store the src_stat_desc

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_src_stat_desc_min_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32                       *src_stat_desc
)
{
  ds_umts_pdp_profile_status_etype status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );
 
 /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    Note: the UMTS QOS parameters might be invalid & not defined
    The caller has to handle this scenario by checking the "valid_flg"
    of the UMTS parameters returned
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the Min Source Statistics Descriptor info
    -------------------------------------------------------------------*/
    memscpy( (void*) src_stat_desc,
             sizeof(uint32),
             (void *) (&(ds_umts_pdp_local_profile_ptr->src_stat_desc_min)),
             sizeof(uint32) );
  }

  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_GPRS_QOS_REQ_INFO_INTERNAL

DESCRIPTION
  This function retrieves the GPRS "requested" qos parameters specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_gprs_qos_req_data   : pointer to store the QOS parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_gprs_qos_req_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_gprs_qos_params_type *pdp_gprs_qos_req_data
)
{
  ds_umts_pdp_profile_status_etype status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

 /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    Note: the GPRS QOS parameters might be invalid & not defined
    The caller has to handle this scenario by checking the "valid_flg"
    of the GPRS parameters returned
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the GPRS requested QOS info.
    -------------------------------------------------------------------*/
    memscpy( (void *) pdp_gprs_qos_req_data,
             sizeof(ds_umts_gprs_qos_params_type),
            (void *) &(ds_umts_pdp_local_profile_ptr->qos_request_gprs),
            sizeof(ds_umts_gprs_qos_params_type) );
  }
  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_GPRS_QOS_MIN_INFO_INTERNAL

DESCRIPTION
  This function retrieves the GPRS "minimum" qos parameters specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_gprs_qos_min_data   : pointer to store the QOS parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_gprs_qos_min_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_gprs_qos_params_type *pdp_gprs_qos_min_data
)
{
  ds_umts_pdp_profile_status_etype status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

  /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    Note: the GPRS QOS parameters might be invalid & not defined
    The caller has to handle this scenario by checking the "valid_flg"
    of the GPRS parameters returned
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the GPRS requested QOS info.
    -------------------------------------------------------------------*/
    memscpy( (void*) pdp_gprs_qos_min_data,
             sizeof(ds_umts_gprs_qos_params_type),
            (void*) &(ds_umts_pdp_local_profile_ptr->qos_minimum_gprs),
            sizeof(ds_umts_gprs_qos_params_type) );
  }
  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}

/*===========================================================================
FUNCTION DS_3GPP_GET_PDP_PROFILE_LTE_QOS_INFO_INTERNAL

DESCRIPTION
  This function retrieves the LTE qos parameters specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_lte_qos_data   : pointer to store the QOS parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_3gpp_get_pdp_profile_lte_qos_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_3gpp_lte_qos_params_type *pdp_lte_qos_data
)
{
  ds_umts_pdp_profile_status_etype status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

  /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    Note: the GPRS QOS parameters might be invalid & not defined
    The caller has to handle this scenario by checking the "valid_flg"
    of the GPRS parameters returned
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the LTE requested QOS info.
    -------------------------------------------------------------------*/
    memscpy( (void*) pdp_lte_qos_data,
             sizeof(ds_3gpp_lte_qos_params_type),
            (void*) &(ds_umts_pdp_local_profile_ptr->qos_request_lte),
            sizeof(ds_3gpp_lte_qos_params_type) );
  }
  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_INTERNAL

DESCRIPTION
  This function retrieves the DNS addresses specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined ( non zero dns address values ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_dns_addr    : pointer to store the dns addresses

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_dns_address_info_internal
(
  uint16                         profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_dns_addresses_type *pdp_dns_addr
)
{
  ds_umts_pdp_profile_status_etype status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );
 
  /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the DNS addresses
    -------------------------------------------------------------------*/
    memscpy( (void*) pdp_dns_addr,
             sizeof(ds_umts_pdp_dns_addresses_type),
            (void*) &(ds_umts_pdp_local_profile_ptr->dns_addr),
            sizeof(ds_umts_pdp_dns_addresses_type) );
  }
  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_PRIMARY_INTERNAL

DESCRIPTION
  This function retrieves the primary DNS address specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (non zero dns address ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  primary_dns     : pointer to store the dns address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_dns_address_info_primary_internal
(
  uint16                  profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_addr_type   *primary_dns
)
{
  ds_umts_pdp_profile_status_etype status;
  ds_umts_pdp_dns_addresses_type pdp_dns_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Get the DNS infor by using using ds_umts_get_pdp_profile_dns_address_info
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_dns_address_info_per_subs(
                                       profile_number,
                                       subs_id,
                                       &pdp_dns_info);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the DNS addresses
    -------------------------------------------------------------------*/
    *primary_dns = pdp_dns_info.primary_dns_addr;
  }

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_SECONDARY_INTERNAL

DESCRIPTION
  This function retrieves the secondary DNS address specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (non zero dns address ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  secondary_dns   : pointer to store the dns address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_dns_address_info_secondary_internal
(
  uint16                   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_addr_type   *secondary_dns
)
{
  ds_umts_pdp_profile_status_etype status;
  ds_umts_pdp_dns_addresses_type pdp_dns_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /*-------------------------------------------------------------------
    Get the DNS infor by using using ds_umts_get_pdp_profile_dns_address_info
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_dns_address_info_per_subs(
                                       profile_number,
                                       subs_id,
                                       &pdp_dns_info);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the DNS addresses
    -------------------------------------------------------------------*/
    *secondary_dns = pdp_dns_info.secondary_dns_addr;
  }

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_TFT_INFO_INTERNAL

DESCRIPTION
  This function retrieves the Traffic Flow Template (TFT) parameters
  specified for a profile number and filter identifer. No check is
  done to see if a valid context is defined.  The caller has to check
  if the context and associated parameter set is defined (valid_flg of
  TFT is set) and use it accordingly.

PARAMETERS
  profile_number : profile number
  subs_id :Subscription id. 
  filter_id      : filter identifier 
  pdp_tft_data   : pointer to store the TFT parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_INVALID_FILTER_ID    : Invalid filter identifier
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_tft_info_internal
(
  uint16                  profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                    filter_id,
  ds_umts_tft_params_type *pdp_tft_data
)
{
  ds_umts_pdp_profile_status_etype status;
  byte index = filter_id - 1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if the input parameters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_FILTER_ID(filter_id);
  
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

 /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    The profile is returned ONLY if a valid context is defined.
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
      /*-------------------------------------------------------------------
        Get the TFT info 
      -------------------------------------------------------------------*/
      memscpy( (void*) pdp_tft_data,
               sizeof(ds_umts_tft_params_type),
              (void*) &ds_umts_pdp_local_profile_ptr->tft[index],
              sizeof(ds_umts_tft_params_type) );
  }

  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_TFT_ALL_INFO_INTERNAL

DESCRIPTION

  This function retrieves ALL the Traffic Flow Template (TFT)
  parameters specified for a profile number. No check is done to see
  if a valid context is defined.  The caller is responsible for
  allocating sufficent storage for the returned structure.  The caller
  has to check if the context and associated parameter set is defined
  (valid_flg of TFT is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_tft_data   : pointer to store the TFT parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_tft_all_info_internal
(
  uint16                   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_tft_params_type  *pdp_tft_data
)
{
  ds_umts_pdp_profile_status_etype status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

  /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    The profile is returned ONLY if a valid context is defined.
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                       profile_number,
                       subs_id,
                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the TFT info 
    -------------------------------------------------------------------*/
    memscpy( (void*) pdp_tft_data,
             (DS_UMTS_MAX_TFT_PARAM_SETS * sizeof(ds_umts_tft_params_type)),
            (void*) ds_umts_pdp_local_profile_ptr->tft,
            (DS_UMTS_MAX_TFT_PARAM_SETS * sizeof(ds_umts_tft_params_type))
          );
  }
  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_LINGER_PARAMS_INFO_INTERNAL

DESCRIPTION
  This function retrieves the linger parameters from the profile.

PARAMETERS
  profile_number     : profile number
  subs_id :Subscription id. 
  linger_params_data : pointer to store the linger parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
   ds_umts_get_pdp_profile_linger_params_info_internal
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_linger_params_type *linger_params_data
)
{
  ds_umts_pdp_profile_status_etype status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_UMTS_PDP_ALLOC_LOCAL_PDP_PROFILE( ds_umts_pdp_local_profile_ptr );

  /*-------------------------------------------------------------------
    Get the complete profile using ds_umts_get_pdp_profile_all_data
    It performs the err check on profile number & init flag
    The profile is returned ONLY if a valid context is defined.
  -------------------------------------------------------------------*/
  status = ds_umts_get_pdp_profile_all_data_internal(
                                       profile_number,
                                       subs_id,
                                       ds_umts_pdp_local_profile_ptr);

  if( status == DS_UMTS_PDP_SUCCESS )
  {
    /*-------------------------------------------------------------------
      Get the Linger params info 
    -------------------------------------------------------------------*/
    memscpy( (void*) linger_params_data,
            sizeof(ds_umts_linger_params_type),
            (void*) ( &(ds_umts_pdp_local_profile_ptr->iface_linger_params )),
            sizeof(ds_umts_linger_params_type) );
  }
  DS_UMTS_PDP_FREE_LOCAL_PDP_PROFILE(ds_umts_pdp_local_profile_ptr);

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_EMBEDDED_CALLS_INTERNAL

DESCRIPTION
  This function retrieves the profile number specififed for embedded
  data calls ( embedded calls ).  The cached value is return if available;
  otherwise, the read is done from the EFS file.

PARAMETERS
  subs_id : Subscription ID 
  profile_number  : pointer to store profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
   ds_umts_get_pdp_profile_num_for_embedded_calls_internal
( 
  ds_umts_pdp_subs_e_type          subs_id,
  byte *profile_number
)
{
  mcfg_fs_status_e_type efs_result = 0;

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return DS_UMTS_PDP_INVALID_SUBS_ID;
  }

  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();

  /*-------------------------------------------------------------------
    Check if cached embedded call profile number is available
  -------------------------------------------------------------------*/
  if(( 0 != cached_embedded_call_profile_number[subs_id]) && (embedded_call_profile_number_read[subs_id]))
  {
    *profile_number = cached_embedded_call_profile_number[subs_id];
  }
  else
  {
    /*-------------------------------------------------------------------
      Init the fully qualified embedded call file name 
    -------------------------------------------------------------------*/
    /*lint -e534 ignoring return code from function*/
    strlcpy( ds_umts_pdp_fully_qual_embedded_call_file_name,
                 ds_umts_pdp_profile_dir_name,
                 sizeof(ds_umts_pdp_fully_qual_embedded_call_file_name) );
    strlcat( ds_umts_pdp_fully_qual_embedded_call_file_name,
                 DS_UMTS_PDP_PROFILE_NUM_FOR_EMBEDDED_FILE_NAME,
                 sizeof(ds_umts_pdp_fully_qual_embedded_call_file_name) );

    /*lint +e534*/
    /*-------------------------------------------------------------------
      read the contents of the file : only one field 
      a one byte context number is stored.
      -------------------------------------------------------------------*/
    efs_result = mcfg_fs_read( ds_umts_pdp_fully_qual_embedded_call_file_name,
                              (void *)profile_number,
                               sizeof(byte),
                               MCFG_FS_TYPE_EFS,
                               (mcfg_fs_sub_id_e_type)subs_id );
    if ( efs_result != MCFG_FS_STATUS_OK)
    {
      DS_3GPP_MSG2_ERROR("file read failed result:%d errno:%d ",
                         efs_result, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return DS_UMTS_PDP_ACCESS_ERROR;
    }

    /*-------------------------------------------------------------------
      Set the cached embedded call profile_number for later reads
      -------------------------------------------------------------------*/
    cached_embedded_call_profile_number[subs_id] = *profile_number;
    embedded_call_profile_number_read[subs_id] = TRUE;
    
  }

  return DS_UMTS_PDP_SUCCESS;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_TETHERED_CALLS_INTERNAL

DESCRIPTION
  This function retrieves the profile number specififed for embedded
  data calls ( tethered calls ).  The cached value is return if available;
  otherwise, the read is done from the EFS file.

PARAMETERS
  subs_id : Subscription ID
  profile_number  : pointer to store profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_num_for_tethered_calls_internal
( 
  ds_umts_pdp_subs_e_type subs_id,
  byte *profile_number
)
{
  mcfg_fs_status_e_type efs_result = 0;

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return DS_UMTS_PDP_INVALID_SUBS_ID;
  }


  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();


  /*-------------------------------------------------------------------
    Check if cached tethered call profile number is available
  -------------------------------------------------------------------*/
  if(( 0 != cached_tethered_call_profile_number[subs_id]) && (tethered_call_profile_number_read[subs_id]))
  {
    *profile_number = cached_tethered_call_profile_number[subs_id];
  }
  else
  {
    /*-------------------------------------------------------------------
      Init the fully qualified tethered call file name 
    -------------------------------------------------------------------*/
    /*lint -e534 ignoring return code from function*/
    strlcpy( ds_umts_pdp_fully_qual_tethered_call_file_name,
                 ds_umts_pdp_profile_dir_name,
                 sizeof(ds_umts_pdp_fully_qual_tethered_call_file_name) );
    strlcat( ds_umts_pdp_fully_qual_tethered_call_file_name,
                 DS_UMTS_PDP_PROFILE_NUM_FOR_TETHERED_FILE_NAME,
                 sizeof(ds_umts_pdp_fully_qual_tethered_call_file_name) );

    /*lint +e534*/
    /*-------------------------------------------------------------------
      read the contents of the file : only one field 
      a one byte context number is stored.
      -------------------------------------------------------------------*/
    efs_result = mcfg_fs_read( ds_umts_pdp_fully_qual_tethered_call_file_name,
                      (void *)profile_number,
                               sizeof(byte),
                               MCFG_FS_TYPE_EFS,
                               (mcfg_fs_sub_id_e_type)subs_id );
    if ( efs_result != MCFG_FS_STATUS_OK )
    {
      DS_3GPP_MSG2_ERROR("efs_open failed result:%d errno",
                         efs_result, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return DS_UMTS_PDP_ACCESS_ERROR;
    }
  
    /*-------------------------------------------------------------------
      Set the cached tethered call profile_number for later reads
      -------------------------------------------------------------------*/
    cached_tethered_call_profile_number[subs_id] = *profile_number;
    tethered_call_profile_number_read[subs_id] = TRUE;
  }

  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_EMERGENCY_CALLS_ARE_SUPPORTED_INTERNAL

DESCRIPTION
  This function retrieves the "emergency_calls_are_supported" flag specified for a
  profile. 
  This flag indicates if IMS emergency can be made uising this profile.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  emergency_calls_are_supported   : pointer to store the emergency_calls_are_supported flag.


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_emergency_calls_are_supported_flag_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *emergency_calls_are_supported
)
{
#ifdef FEATURE_EMERGENCY_PDN
  int8 cache_index = -1;
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the emergency_calls_are_supported info from the cache table.
  -------------------------------------------------------------------*/

  DS_UMTS_PDP_TASKLOCK_NON_L4();  
  *emergency_calls_are_supported = 
      DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index )->
      emergency_calls_are_supported;

  DS_UMTS_PDP_TASKFREE_NON_L4();

  return DS_UMTS_PDP_SUCCESS;
#endif /* FEATURE_EMERGENCY_PDN */
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NAME_UTF16_INTERNAL

DESCRIPTION
  This function updates the name of a profile with the value passed.
  NOTE: The name of the profile can be updated at any time ir-respecitive
  of the valididity of the context associated with the profile
  It also updates the profile name in the cache table.

PARAMETERS
  profile_number  : profile number
  profile_name_buf : pointer to the name string (UTF16 string)
  profile_name_len : length of the name string (UTF16 words)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : length of profile name is too long

SIDE EFFECTS 
  None
===========================================================================*/
/*ARGSUSED*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_name_utf16_internal
(
  uint16                    profile_number,
  const uint16              *profile_name_buf,
  byte                      profile_name_len
)
{
 return DS_UMTS_PDP_FAIL;  

}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NAME_INTERNAL

DESCRIPTION
  This function updates the name of a profile with the value passed.
  NOTE: The name of the profile can be updated at any time ir-respecitive
  of the valididity of the context associated with the profile
  It also updates the profile name in the cache table.

PARAMETERS
  profile_number  : profile number
  profile_name_buf : pointer to the name string
  profile_name_len : length of the name string

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : length of profile name is too long

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_name_internal
(
  uint16                    profile_number,
  const byte                *profile_name_buf,
  byte                      profile_name_len
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_OTAP_NAPID_INTERNAL

DESCRIPTION
  This function updates the napid of a profile with the value passed.
  No check is done to see if a valid context is defined.
  It also updates the cache table with the new value.

PARAMETERS
  profile_number : profile number
  otap_napid_buf : pointer to the napid string (UTF8 string)
  otap_napid_len : length of the napid string  (variable length UTF8 bytes)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : length of otap napid is too long

SIDE EFFECTS 
  None
===========================================================================*/
/*ARGSUSED*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_otap_napid_internal
(
  uint16                    profile_number,
  byte                      *otap_napid_buf,
  byte                      otap_napid_len
)
{
  return DS_UMTS_PDP_FAIL; 
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_READ_ONLY_FLAG_INTERNAL

DESCRIPTION
  This function updates the read_only_flag of a profile with the value passed.
  No check is done to see if a valid context is defined.
  It also updates the cache table with the new value.
  
PARAMETERS
  profile_number : profile number
  read_only_flag : profile is read-only or not

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_read_only_flag_internal
(
  uint16                    profile_number,
  boolean                   read_only_flag
)
{
  return DS_UMTS_PDP_FAIL;  
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_OTAP_ENABLED_FLAG_INTERNAL

DESCRIPTION
  This function updates the otap_enabled_flag of a profile with the value passed.
  No check is done to see if a valid context is defined.
  It also updates the cache table with the new otap_enabled_flag value.
  
PARAMETERS
  profile_number : profile number
  otap_enabled_flag : profile is otap_enabled or not

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_otap_enabled_flag_internal
(
  uint16                    profile_number,
  boolean                   otap_enabled_flag
)
{
  return DS_UMTS_PDP_FAIL;  
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_FLAG_INTERNAL

DESCRIPTION
  This function sets the "request_pcscf_address_flag" flag specified for a 
  profile. 
  The "request_pcscf_address_flag" indicates if the pcscf address should be
  requested for this profile using PCO or not.

PARAMETERS
  profile_number  : profile number
  request_pcscf_address_flag   : pointer to store the request_pcscf_address_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_set_pdp_profile_request_pcscf_address_flag_internal
(
  uint16                profile_number,
  boolean               request_pcscf_address_flag
)
{
  return DS_UMTS_PDP_FAIL;  
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_USING_DHCP_FLAG_INTERNAL

DESCRIPTION
  This function sets the "request_pcscf_address_using_dhcp_flag" flag
  specified for a profile. 
  The "request_pcscf_address_using_dhcp_flag" indicates if the pcscf address
  should be requested for this profile using DHCP or not.

PARAMETERS
  profile_number  : profile number
  request_pcscf_address_using_dhcp_flag   : pointer to store the 
                  request_pcscf_address_using_dhcp_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_set_pdp_profile_request_pcscf_address_using_dhcp_flag_internal
(
  uint16                profile_number,
  boolean               request_pcscf_address_using_dhcp_flag
)
{
  return DS_UMTS_PDP_FAIL;  
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_IM_CN_FLAG_INTERNAL

DESCRIPTION
  This function sets the "im_cn" flag specified for a 
  profile. 
  The "im_cn" indicates if the im_cn flag should be
  requested for this profile or not.

PARAMETERS
  profile_number  : profile number
  im_cn_flag   : pointer to store the im_cn_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been
  initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_set_pdp_profile_im_cn_flag_internal
(
  uint16                profile_number,
  boolean               im_cn_flag
)
{
  return DS_UMTS_PDP_FAIL;  
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_APN_DISABLE_FLAG_INTERNAL

DESCRIPTION
  This function call the internal registry function to update the apn diable
  flag setting for the profile specified with the value passed. The update 
  can be done ONLY for a valid context as it is part of the context 
  definition. It also updates the cache table with the new value.

PARAMETERS
  profile_number   : profile number
  apn_disable_flag : boolean value to set in the registry

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_apn_disable_flag_internal
(
  uint16   profile_number,
  boolean  apn_disable_flag
)
{
  return DS_UMTS_PDP_FAIL;
} /* ds_umts_set_pdp_profile_apn_disable_flag_internal */

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_INACTIVITY_TIMER_VAL_INTERNAL

DESCRIPTION
  This function call the internal registry function to update the inactivity
  timer setting for the profile specified with the value passed. The update 
  can be done ONLY for a valid context as it is part of the context 
  definition. It also updates the cache table with the new value.

PARAMETERS
  profile_number       : profile number
  inactivity_timer_val : boolean value to set in the registry

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_inactivity_timer_val_internal
(
  uint16   profile_number,
  uint32   inactivity_timer_val
)
{
  return DS_UMTS_PDP_FAIL;
} /* ds_umts_set_pdp_profile_inactivity_timer_val_internal */

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_APN_CLASS_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the 
  "apn_class" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  apn_class        : uint8 value being passed in for the APN class

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_apn_class_internal
(
  uint16   profile_number,
  uint8    apn_class
)
{
  return DS_UMTS_PDP_FAIL;
} /* ds_umts_set_pdp_profile_apn_class_internal */


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_APN_BEARER_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the 
  "apn_bearer" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  apn_bearer        : uint8 value being passed in for the APN class

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_apn_bearer_internal
(
  uint16   profile_number,
  uint8    apn_bearer
)
{
return DS_UMTS_PDP_FAIL;
} /* ds_umts_set_pdp_profile_apn_class_internal */

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_MCC_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the 
  "MCC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  mcc              : MCC of the carrier

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  

SIDE EFFECTS 
  none
===========================================================================*/

ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_mcc_internal
(
  uint16   profile_number,
  uint16   mcc
)
{
  return DS_UMTS_PDP_FAIL;

}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_MNC_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the 
  "MNC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  mnc              : MNC for the carrier 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  

SIDE EFFECTS 
  none
===========================================================================*/

ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_mnc_internal
(
  uint16   profile_number,
  ds_umts_3gpp_mnc_type     mnc
)
{
    return DS_UMTS_PDP_FAIL;

}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_OPERATOR_RESERVED_PCO_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the operator reserved 
  "PCO" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  pco              : operator range

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  

SIDE EFFECTS 
  none
===========================================================================*/

ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_operator_reserved_pco_internal
(
  uint16   profile_number,
  uint16   pco
)
{
    return DS_UMTS_PDP_FAIL;


}
/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_TRANSIENCE_FLAGS_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the 
  "transience_flag" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.
 
PARAMETERS
  profile_number   : profile number
  transience_flag  : boolean value being passed in for the APN class

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_transience_flag_internal
(
  uint16   profile_number,
  boolean  transience_flag
)
{
  return DS_UMTS_PDP_FAIL;
} /* ds_umts_set_pdp_profile_apn_class_internal */
/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_INTERNAL

DESCRIPTION
  This function updates the context definition  parameters with the
  values passed.
  It also updates the cache table with the new context information.

PARAMETERS
  profile_number  : profile number
  pdp_context_data  : pointer to the context definition 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : the valid_flg for the context is not 
                                    set to TRUE

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_context_info_internal
(
  uint16                    profile_number,
  const ds_umts_pdp_context_type  *pdp_context_data
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_EX_INTERNAL

DESCRIPTION
  This function updates the context definition  parameters with the values
  passed. This is special API called only from ATCOP which will always try to
  create persistent profile. It also updates the cache table with the new
  context information.

PARAMETERS
  profile_number  : profile number
  pdp_context_data  : pointer to the context definition 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : the valid_flg for the context is not 
                                    set to TRUE
  DS_UMTS_PDP_ERR_OUT_OF_PERSISTENT_PROFILES : All persistent profile slots are taken.
                                               Can't create any more persistent profile.

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_context_info_ex_internal
(
  uint16                    profile_number,
  const ds_umts_pdp_context_type  *pdp_context_data
)
{
  return DS_UMTS_PDP_FAIL;

}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_APN_INTERNAL

DESCRIPTION
  This function updates the APN for the profile specified with the
  value passed. The APN  can be updated ONLY for a valid context as it
  is part of the context definition
  It also updates the cache table with the new APN string.

PARAMETERS
  profile_number  : profile number
  apn_buf         : pointer to the APN string
  apn_buf_len     : length of the APN string

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number already

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : length of APN string is too long

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_context_info_apn_internal
(
  uint16                    profile_number,
  const byte                *apn_buf,
  byte                      apn_buf_len
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_ADDRESS_INTERNAL

DESCRIPTION
  This function updates the pdp address for the profile specified with the
  value passed. The address can be updated ONLY for a valid context as it 
  is part of the context definition
  It also updates the cache table with the new pdp address value.

PARAMETERS
  profile_number  : profile number
  pdp_address     : ip address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number already

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_context_info_address_internal
(
  uint16                    profile_number,
  ds_umts_pdp_addr_type     pdp_address
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_IPV4_ADDR_ALLOC_INTERNAL

DESCRIPTION
  This function call the internal registry function to update the ipv4 addr
  alloc setting for the profile specified with the value passed. The update 
  can be done ONLY for a valid context as it is part of the context 
  definition. It also updates the cache table with the new value.

PARAMETERS
  profile_number  : profile number
  ipv4_addr_alloc : ipv4 address allocation (mechanism)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number already

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_context_info_ipv4_addr_alloc_internal
(
  uint16                              profile_number,
  ds_umts_pdp_ipv4_addr_alloc_e_type  ipv4_addr_alloc
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_TO_DEFAULT_INTERNAL

DESCRIPTION
  This function sets the context information to be defined for the 
  profile number specified and the following values are set for the context
  pdp type : IP
  IP address :0.0.0.0
  APN        : Null string
  data & header compression : OFF

  This function can be called by UI to define a context & hence a profile
  to default and then allow the user to override other fields of the 
  context
  If valid flag is not set (profile being created), transience_flag is set
  to FALSE (default value)
   
PARAMETERS
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_context_info_to_default_internal
(
  uint16                  profile_number,
  ds_ip_version_enum_type ip_vsn
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_CONTEXT_INFO_INTERNAL

DESCRIPTION
  This function resets the context information to be undefined for the 
  profile number specified. It does not over-write the values of
  other parameter sets (qos, auth etc).
  It also resets the context information to be undefined in the cache table.

PARAMETERS
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
 The context definition is reset, but all other parameter sets are left
 as is. Auth, qos, dns address values are not changed.
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_context_info_internal
(
  uint16                  profile_number
)
{
  return DS_UMTS_PDP_FAIL;

}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_INTERNAL

DESCRIPTION
  This function updates the authentication information with the values passed.
  The auth parameters are updated with the values passed even if a valid 
  context defintion is not present. 
  It also updates the cache table with the passed authentication information.

PARAMETERS
  profile_number  : profile number
  pdp_auth_data   : pointer to authentication information struct

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_auth_info_internal
(
  uint16                 profile_number,
  const ds_umts_pdp_auth_type  *pdp_auth_data
)
{
  return DS_UMTS_PDP_FAIL;

}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_TYPE_INTERNAL

DESCRIPTION
  This function updates the authentication type field of authentication 
  information with the value passed.
  The auth parameters are updated with the values passed even if a valid 
  context defintion is not present. 
  It also updates the cache table with the authentication type value.

PARAMETERS
  profile_number  : profile number
  auth_type       : Authentication type

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_auth_info_type_internal
(
  uint16                     profile_number,
  ds_umts_pdp_auth_enum_type auth_type
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_USERNAME_INTERNAL

DESCRIPTION
  This function updates the username field of authentication information 
  with the value passed.
  The auth parameters are updated with the values passed even if a valid 
  context defintion is not present. 
  It also updates the cache table with the value passed.

PARAMETERS
  profile_number  : profile number
  usename_buf     : pointer to the username string
  username_buf_len: length of the username string

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : The length of username string is too long

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_auth_info_username_internal
(
  uint16                     profile_number,
  const byte                       *usernme_buf,
  byte                       username_buf_len
)
{
  return DS_UMTS_PDP_FAIL;

}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_PASSWORD_INTERNAL

DESCRIPTION
  This function updates the password field of authentication information 
  with the value passed.
  The auth parameters are updated with the values passed even if a valid 
  context defintion is not present. 
  It also updates the cache table with the value passed.

PARAMETERS
  profile_number  : profile number
  password_buf    : pointer to the password string
  password_buf_len: length of the password string

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : The length of password string is too long

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_auth_info_password_internal
(
  uint16  profile_number,
  const byte    *password_buf,
  byte    password_buf_len
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_TO_DEFAULT_INTERNAL

DESCRIPTION
  This function resets the authentication information for the profile 
  specified. Authentication is set to "None" 
  The auth parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_auth_info_to_default_internal
(
  uint16                  profile_number
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_UMTS_QOS_REQ_INFO_INTERNAL

DESCRIPTION
  This function updates the UMTS "requested" qos parameters with the values
  passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_umts_qos_data : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_umts_qos_req_info_internal
(
  uint16                       profile_number,
  const ds_umts_umts_qos_params_type *pdp_umts_qos_data
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_UMTS_QOS_REQ_INFO_INTERNAL

DESCRIPTION
  This function updates the UMTS "requested" qos parameters for the 
  profile indicated. The parameters are updated to be subscribed values
  and the "valid_flg" is set to FALSE indicating that the values are not
  valid.
  The qos parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the umts qos (requested) information to be undefined and all parameters
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_umts_qos_req_info_internal
(
  uint16                  profile_number
)
{
  return DS_UMTS_PDP_FAIL;

}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_UMTS_QOS_MIN_INFO_INTERNAL

DESCRIPTION
  This function updates the UMTS "minimum" qos parameters with the values
  passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_umts_qos_data : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_umts_qos_min_info_internal
(
  uint16                       profile_number,
  const ds_umts_umts_qos_params_type *pdp_umts_qos_data
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_UMTS_QOS_MIN_INFO_INTERNAL
DESCRIPTION
  This function updates the UMTS "minimum" qos parameters for the 
  profile indicated. The parameters are updated to be subscribed values
  and the "valid_flg" is set to FALSE indicating that the values are not
  valid.
  The qos parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the umts qos (minimum) information to be undefined and all parameters
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_umts_qos_min_info_internal
(
  uint16                  profile_number
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_SRC_STAT_DESC_REQ_INFO_INTERNAL

DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "requested" src_stat_desc info with the values passed. 

PARAMETERS
  profile_number  : profile number
  src_stat_desc   : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_src_stat_desc_req_info_internal
(
  uint16                       profile_number,
  const uint32                 src_stat_desc
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_SRC_STAT_DESC_REQ_INFO_INTERNAL

DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "requested" src_stat_desc info with the values passed. 

PARAMETERS
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  sets the umts qos (requested) information to be undefined and all parameters
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_src_stat_desc_req_info_internal
(
  uint16                  profile_number
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_SRC_STAT_DESC_MIN_INFO_INTERNAL

DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "requested" src_stat_desc info with the values passed. 

PARAMETERS
  profile_number  : profile number
  src_stat_desc   : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none

===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_src_stat_desc_min_info_internal
(
  uint16                       profile_number,
  const uint32                 src_stat_desc
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_SRC_STAT_DESC_MIN_INFO_INTERNAL

DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "requested" src_stat_desc info with the values passed. 

PARAMETERS
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  sets the umts qos (requested) information to be undefined and all parameters
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_src_stat_desc_min_info_internal
(
  uint16                  profile_number
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_GPRS_QOS_REQ_INFO_INTERNAL

DESCRIPTION
  This function updates the GPRS "requested" qos parameters with the values
  passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_gprs_qos_req_data : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_gprs_qos_req_info_internal
(
  uint16                       profile_number,
  const ds_umts_gprs_qos_params_type *pdp_gprs_qos_req_data
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_GPRS_QOS_REQ_INFO_INTERNAL
DESCRIPTION
  This function updates the GPRS "requested" qos parameters for the 
  profile indicated. The parameters are updated to be subscribed values
  and the "valid_flg" is set to FALSE indicating that the values are not
  valid.
  The qos parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the gprs requested qos information to be undefined and all parameters 
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_gprs_qos_req_info_internal
(
  uint16                  profile_number
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_GPRS_QOS_MIN_INFO_INTERNAL

DESCRIPTION
  This function updates the GPRS "minimum" qos parameters with the values
  passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_gprs_qos_min_data : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_gprs_qos_min_info_internal
( 
  uint16                       profile_number,
  const ds_umts_gprs_qos_params_type *pdp_gprs_qos_min_data
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_GPRS_QOS_MIN_INFO_INTERNAL

DESCRIPTION
  This function updates the GPRS "minimum" qos required parameters for the 
  profile indicated. The parameters are updated to be subscribed values
  and the "valid_flg" is set to FALSE indicating that the values are not
  valid.
  The qos parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the gprs minimum qos information to be undefined and all parameters 
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_gprs_qos_min_info_internal
(
  uint16                  profile_number
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_3GPP_SET_PDP_PROFILE_LTE_QOS_INFO_INTERNAL

DESCRIPTION
  This function updates the LTE qos parameters with the values passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_lte_qos_data : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_3gpp_set_pdp_profile_lte_qos_info_internal
( 
  uint16                       profile_number,
  const ds_3gpp_lte_qos_params_type *pdp_lte_qos_data
)
{
  return DS_UMTS_PDP_FAIL;
}


/*===========================================================================
FUNCTION DS_3GPP_RESET_PDP_PROFILE_LTE_QOS_INFO_INTERNAL

DESCRIPTION
  This function updates the LTE qos required parameters for the 
  profile indicated. The parameters are updated to be subscribed values
  and the "valid_flg" is set to FALSE indicating that the values are not
  valid.
  The qos parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the lte qos information to be undefined and all parameters 
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_3gpp_reset_pdp_profile_lte_qos_info_internal
(
  uint16                  profile_number
)
{
#ifdef FEATURE_PROFILE_OTAP_ENHANCEMENTS
  boolean               read_only_flag;
#endif
  return DS_UMTS_PDP_FAIL;
}



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_TFT_INFO_INTERNAL

DESCRIPTION
  This function updates the Traffic Flow Template (TFT) parameters with
  the values passed for the given filter identifier. 
  The TFT parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  filter_id       : filter identifer
  pdp_tft_data    : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_INVALID_FILTER_ID    : Invalid fulter identifier
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_tft_info_internal
(
  uint16                      profile_number,
  byte                        filter_id,
  const ds_umts_tft_params_type     *pdp_tft_data
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_TFT_ALL_INFO_INTERNAL

DESCRIPTION
  This function updates the Traffic Flow Template (TFT) parameters
  with the values passed. The data pointer is assumed to be the
  address of an array of size DS_UMTS_MAX_TFT_PARAM_SETS.
  The TFT parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_tft_data    : the values to be updated with;
                    array[DS_UMTS_MAX_TFT_PARAM_SETS] 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_tft_all_info_internal
(
  uint16                      profile_number,
  const ds_umts_tft_params_type     *pdp_tft_data
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_TFT_ALL_INFO_INTERNAL

DESCRIPTION
  This function updates the Traffice Flow Template (TFT) required
  parameters for the profile indicated. The parameters are updated to
  zero values and the "valid_flg" is set to FALSE indicating that the
  values are not valid.

  The TFT parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the TFT information to be undefined and all parameters 
  are set to zero values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_tft_all_info_internal
(
  uint16                   profile_number
)
{
  return DS_UMTS_PDP_FAIL;
}



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_LINGER_PARAMS_INFO_INTERNAL

DESCRIPTION
  This function updates the linger parameters in the profile

PARAMETERS
  profile_number     : profile number
  linger_params_data : pointer to the linger parameter data

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  
  ds_umts_set_pdp_profile_linger_params_info_internal
(
  uint16                           profile_number,
  const ds_umts_linger_params_type *linger_params_data
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_INFO_INTERNAL

DESCRIPTION
  This function updates the dns addresses for the profile specified with 
  the values passed
  The dns parameters are updated with the values passed even if a valid 
  context defintion is not present. 
  It also updates the cache table with the dns addresses passed.

PARAMETERS 
  profile_number  : profile number
  pds_dns_addr    : pointer to the dns addresses

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_dns_address_info_internal
(
  uint16                         profile_number,
  const ds_umts_pdp_dns_addresses_type *pdp_dns_addr
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_PRIMARY_INTERNAL

DESCRIPTION
  This function sets the primary dns address for a pdp profile
  The dns parameters are updated with the values passed even if a valid 
  context defintion is not present. 
  It also updates the cache table with the primary dns address value passed.

PARAMETERS 
  profile_number  : profile number
  primary_dns_addr: dns address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_dns_address_primary_internal
(
  uint16                    profile_number,
  ds_umts_pdp_addr_type     primary_dns_addr
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_SECONDARY_INTERNAL

DESCRIPTION
  This function sets the secondary dns address for a pdp profile
  The dns parameters are updated with the values passed even if a valid 
  context defintion is not present. 
  It also updates the cache table with the secondary dns address value passed.

PARAMETERS 
  profile_number  : profile number
  secondary_dns_addr: dns address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_dns_address_secondary_internal
(
  uint16                    profile_number,
  ds_umts_pdp_addr_type     secondary_dns_addr
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_INFO_TO_DEFAULT_INTERNAL

DESCRIPTION
  This function sets the dns addresses of a pdp profile to default values
  primary, secondary DNS addresses are set to 0.0.0.0
  The dns parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_dns_address_info_to_default_internal
(
  uint16                  profile_number
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_ALL_DATA_INTERNAL

DESCRIPTION
  This function is used to set all the parameters associated with a profile
  The profile_data_ptr is passed in. The profile_data_ptr should atleast have a
  valid context for this function to update the profile parameters 
  
  NOTE : The name of the profile is overwritten with the value passed in 
  the profile
         The profile is not modified if read_only_flag is set
         The transience_flag cannot be modified.
 
PARAMETERS 
  profile_number  : profile number
  profile_data_ptr    : pointer to the profile data parameters
 
 
DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : The parameter did not contain a valid context
  DS_UMTS_PDP_FAIL                 : Everything else...
SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_all_data_internal
(
  uint16                   profile_number, 
  ds_umts_pdp_profile_type *profile_data_ptr 
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_ALL_DATA_TO_DEFAULT_INTERNAL

DESCRIPTION
  This function sets the profile information to default values for a given
  profile number. It defines a valid context for the profile (pdp-ip)or 
  pdp-ipv6  as per ip_vsn and sets all other parameters to the default values
  context : no compression, ip address :0.0.0.0, APN : Null
  Auth    : no auth
  QOS     : no qos params defined ( umts and gprs)
  dns     : primary and secondary set to 0.0.0.0
  TFT     : no tft

  NOTE : The name of the profile is left as-is.
         If the profile was already created (valid flag true),
         the transience_flag is left as-is
 
PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  All the profile parameters are overwritten.
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_all_data_to_default_internal
(
  uint16                  profile_number,
  ds_ip_version_enum_type ip_vsn
)
{
 return DS_UMTS_PDP_FAIL;
}   

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_ALL_DATA_TO_DEFAULT_EX_INTERNAL

DESCRIPTION 
  This function is similar to DS_UMTS_SET_PDP_PROFILE_ALL_DATA_TO_DEFAULT_INTERNAL.
  It takes an extra parameter, transience_flag - allowing DS_PROFILE_CREATE_EX to
  create a profile in a single EFS update.
 
  This function sets the profile information to default values for a given
  profile number. It defines a valid context for the profile (pdp-ip)or 
  pdp-ipv6  as per ip_vsn and sets all other parameters to the default values
  context : no compression, ip address :0.0.0.0, APN : Null
  Auth    : no auth
  QOS     : no qos params defined ( umts and gprs)
  dns     : primary and secondary set to 0.0.0.0
  TFT     : no tft

  NOTE : The name of the profile is left as-is.
         If the profile was already created (valid flag true),
         the transience_flag is left as-is, otherwise, transience_flag
         is set.
 
 
PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  All the profile parameters are overwritten.
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_all_data_to_default_ex_internal
(
  uint16                  profile_number,
  ds_ip_version_enum_type ip_vsn,
  boolean                 transience_flag
)
{
  return DS_UMTS_PDP_FAIL;
}   

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_TO_UNDEFINED_INTERNAL

DESCRIPTION
  This function resets the profile specified to undefined values. 
  An undefined profile has the foll : 
  context - not defined
  auth information : None
  QOS params ( umts and gprs ) : not defined
  dns addresses : all addresses are set to 0.0.0.0
  TFT information : None

  NOTE : The name of the profile is left as-is.

PARAMETERS 
  subs_id :Subscription id. 
  profile_number  : profile number to be set to undefined 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  All the profile parameters are overwritten
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_to_undefined_internal
(
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                  profile_number
)
{
  boolean               read_only_flag;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  DS_UMTS_PDP_CHECK_READ_ONLY_FLAG(profile_number,subs_id,read_only_flag);

  /*-------------------------------------------------------------------------
    Call the local function to reset the profile : undefined values are
    context : not defined
    auth    : None
    umts qos : not defined
    GPRS qos(req &min) :not defined
    dns addresses : primary & secondary - 0.0.0.0
    TFT :not defined
  -------------------------------------------------------------------------*/
  if(! ds_umts_update_pdp_profile_to_undefined(profile_number, subs_id) )
  {
    DS_3GPP_MSG1_HIGH("error in resetting context/profile:%d", profile_number);
    return DS_UMTS_PDP_ACCESS_ERROR;
  }

  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_EMBEDDED_CALLS_INTERNAL

DESCRIPTION
  This function sets the profile number specified to be used for embedded 
  calls. It writes the value specified to the file the embedded_call_prof_num
  file

PARAMETERS 
  profile_number  : profile number 
  subs_id: subscription ID

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
   ds_umts_set_pdp_profile_num_for_embedded_calls_internal
(
  uint16 profile_number,
  ds_umts_pdp_subs_e_type subs_id
)
{
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
       DS_UMTS_PDP_INVALID_PROFILE_NUM )
  {
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 
  }

  /*---------------------------------------------------------------------
    Call the local routine to update the file with the new profile number
  ---------------------------------------------------------------------*/ 
  if( ! ds_umts_update_embedded_call_prof_num(subs_id, profile_number ) )
  {
    return  DS_UMTS_PDP_ACCESS_ERROR;
  }

  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_TETHERED_CALLS_INTERNAL

DESCRIPTION
  This function sets the profile number specified to be used for tethered 
  calls. It writes the value specified to the file the tethered_call_prof_num file

PARAMETERS 
  subs_id: subscription ID 
  profile_number  : profile number 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
   ds_umts_set_pdp_profile_num_for_tethered_calls_internal
(
  ds_umts_pdp_subs_e_type subs_id,
  uint16 profile_number
)
{
  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if( ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
      DS_UMTS_PDP_INVALID_PROFILE_NUM )
  {
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 
  }

  /*---------------------------------------------------------------------
    Call the local routine to update the file with the new profile number
  ---------------------------------------------------------------------*/ 
  if( ! ds_umts_update_default_tethered_prof_num(subs_id, profile_number ) )
  {
    return  DS_UMTS_PDP_ACCESS_ERROR;
  }

  return DS_UMTS_PDP_SUCCESS;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ACCESS_CTRL_FLAG_INTERNAL

DESCRIPTION
  This function retrieves the "access_ctrl_flag" specified for a profile. 
 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  access_ctrl_flag  : pointer to store the access_ctrl_flag

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
/*ARGSUSED*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_access_ctrl_flag_internal
(
  uint16                          profile_number,
  dsat_pdp_access_control_e_type  *access_ctrl_flag
)
{
#ifndef FEATURE_DATA_TE_MT_PDP
  DS_3GPP_MSG0_ERROR("PDP registry API not supported");
  return DS_UMTS_PDP_API_NOT_SUPPORTED;
#else
  int8 cache_index = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
     DS_UMTS_PDP_INVALID_PROFILE_NUM )
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 

  cache_index = ds_3gpp_profile_cache_get_cache_index(profile_number, subs_id);

  if (cache_index <= 0)
  {
	return DS_UMTS_PDP_INVALID_PROFILE_NUM;
  } 

  /*-------------------------------------------------------------------
    Read the access_ctrl_flag from the cache table.
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_TASKLOCK_NON_L4();
  *access_ctrl_flag = 
    DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index)->context.access_ctrl_flag;
  DS_UMTS_PDP_TASKFREE_NON_L4();
  return DS_UMTS_PDP_SUCCESS;
#endif
}

#ifdef FEATURE_GOBI_MUXD_PDP_PROFILE_DIR

/*===========================================================================
FUNCTION UNDP_GET_MUXD_PDP_PROFILE_DIRNAME

DESCRIPTION
  Return Consolidated Profile Directory Name

PARAMETERS 
  None

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  Consolidated Profile Directory Name

SIDE EFFECTS 
  None
===========================================================================*/

static char* undp_get_muxd_pdp_profile_dirname(void)
{
  nv_stat_enum_type  nv_status;
  uint8              gobi_consl_profle_dir_index;

  /* Get the index number from NV if not done so already */
  if ( !undp_muxd_consl_pdp_profile_dir_name_len )
  {
    nv_status = ds3gcfgmgr_read_legacy_nv(NV_IRDA_PNP_VERSION_I, &ds_nv_item);
    gobi_consl_profle_dir_index = (uint8)((ds_nv_item.irda_pnp_version & UNDP_CONSL_PROFILE_EN_MASK) >> 8);

    /* Consolidated AtCOP & RmNET profiles */
    strlcpy( undp_muxd_consl_pdp_profile_file_dir_name,                
                 UNDP_PDP_PROFILE_CONSL_DIR_NAME_START,
                 sizeof(undp_muxd_consl_pdp_profile_file_dir_name) );     

    if((nv_status == NV_DONE_S) && (gobi_consl_profle_dir_index !=0 ))
    {
        ds_umts_pdp_itoa( gobi_consl_profle_dir_index, 
                      ((byte*)&undp_muxd_consl_pdp_profile_file_dir_name[UNDP_PDP_PROFILE_CONSL_DIR_NAME_INDEX]), 
                      10 );
        undp_muxd_consl_pdp_profile_dir_name_len = DS_UMTS_PDP_PROFILE_CONSL_DIR_NAME_LEN; 
    }
    else
    {
        /* NV Item Inactive or set to 0 */
        undp_muxd_consl_pdp_profile_dir_name_len = UNDP_PDP_PROFILE_CONSL_DIR_NAME_INDEX; 
    }
  }

  return undp_muxd_consl_pdp_profile_file_dir_name;
}
#endif /* FEATURE_GOBI_MUXD_PDP_PROFILE_DIR */

/*===========================================================================
FUNCTION DS_UMTS_SET_DEFAULT_PROFILE_NUMBER_PER_SUBS_INTERNAL

DESCRIPTION
  This function sets the profile number for tethered or embedded family for 
  the specified subscription. The value is written to either 
  embedded_call_prof_nums_dsds or tethered_call_prof_nums_dsds file.

PARAMETERS 
  family           : Denotes embedded or tetherd profile
  subs             : Denotes one of two active subscriptions
  profile_number   : Profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : The operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/

ds_umts_pdp_profile_status_etype
ds_umts_set_default_profile_number_internal
(
  ds_umts_profile_family_e_type        family,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                               profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return DS_UMTS_PDP_INVALID_SUBS_ID;
  }


  /*---------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  ---------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();
  if(  ds_umts_pdp_check_profile_number(profile_number, subs_id) == 
                                     DS_UMTS_PDP_INVALID_PROFILE_NUM )
  {
    return DS_UMTS_PDP_INVALID_PROFILE_NUM; 
  }
  

  /*---------------------------------------------------------------------
    Call the local routine based on family to update the EFS file with
    the new profile number
  ---------------------------------------------------------------------*/ 
  switch( family )
  {
    case DS_UMTS_EMBEDDED_PROFILE_FAMILY:

      if(ds_umts_update_embedded_call_prof_num(subs_id,profile_number) == TRUE)
      {
         status = DS_UMTS_PDP_SUCCESS;
      }
      break;
      
    case DS_UMTS_TETHERED_PROFILE_FAMILY:
      if(ds_umts_update_default_tethered_prof_num(subs_id,profile_number))
      {
         status = DS_UMTS_PDP_SUCCESS;
      }
      break;

    default:
      status = DS_UMTS_PDP_FAIL; 
  }
  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_DEFAULT_PROFILE_NUMBER_INTERNAL

DESCRIPTION
  This function gets the profile number for the specifies subscription.
  It gets the value specified to the embedded_call_prof_nums_dsds file or 
  tethered_call_prof_nums_dsds file.

PARAMETERS 
  family           : Denotes embedded or tethered profile
  subs             : Denotes one of two active subscriptions
  profile_number   : Profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : The operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  None
===========================================================================*/

ds_umts_pdp_profile_status_etype
ds_umts_get_default_profile_number_internal
(
  ds_umts_profile_family_e_type        family,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                              *profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  *profile_number = 0;

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return DS_UMTS_PDP_INVALID_SUBS_ID;
  }


  /*---------------------------------------------------------------------
    Check if we have been initialized & the input paramaters are correct
  ---------------------------------------------------------------------*/
  DS_UMTS_PDP_CHECK_INIT_FLAG();


  /*---------------------------------------------------------------------
    Call the local routine based on family to get the default 
    profile number
  ---------------------------------------------------------------------*/ 
  switch( family )
  {
    case DS_UMTS_EMBEDDED_PROFILE_FAMILY:

      status = ds_umts_get_pdp_profile_num_for_embedded_calls_internal(
                                           subs_id,
                                           (byte *)profile_number);
      break;

    case DS_UMTS_TETHERED_PROFILE_FAMILY:
      status = ds_umts_get_pdp_profile_num_for_tethered_calls_internal(
                                           subs_id, 
                                           (byte*)profile_number);
      break;

    default:
      status = DS_UMTS_PDP_FAIL;
  }
  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_EMERGENCY_CALLS_ARE_SUPPORTED_FLAG_INTERNAL

DESCRIPTION
  This function calls the internal registry function to set the
  "emergency_calls_are_supported" flag specified for a profile. 
  This flag indicates if IMS emergency calls can be made using this profile.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  emergency_calls_are_supported   :value to store in the emergency_calls_are_supported
                                   flag.

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_set_pdp_profile_emergency_calls_are_supported_flag_internal
(
  uint16                profile_number,
  boolean               emergency_calls_are_supported
)
{
/*
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
*/
  return DS_UMTS_PDP_FAIL;
}

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_MAX_PDN_CONN_PER_BLK_INTERNAL

DESCRIPTION
  This function sets the Max PDN Conn Per Block field in the profile to
  the specified value.

PARAMETERS
  profile_number       : profile number
  max_pdn_conn_per_blk : Max PDN Connections Per Blk

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_max_pdn_conn_per_blk_internal
(
  uint16   profile_number,
  uint16   max_pdn_conn_per_blk
)
{
        return DS_UMTS_PDP_FAIL;
      }

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_PDN_REQ_WAIT_TIME_INTERNAL

DESCRIPTION
  This function sets the PDN Req Wait Time field in the profile to
  the specified value.

PARAMETERS
  profile_number       : profile number
  pdn_req_wait_time    : PDN Req Wait Time

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_pdn_req_wait_time_internal
(
  uint16   profile_number,
  uint16   pdn_req_wait_time
)
{
        return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_MAX_PDN_CONN_TIME_INTERNAL

DESCRIPTION
  This function sets the Max PDN Conn TIme field in the profile to
  the specified value.

PARAMETERS
  profile_number       : profile number
  pdn_req_wait_time    : Max PDN Conn Time

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_max_pdn_conn_time_internal
(
  uint16   profile_number,
  uint16   max_pdn_conn_time
)
{
        return DS_UMTS_PDP_FAIL;
}

#endif /*FEATURE_DATA_LTE*/


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_CACHE_INDEX

DESCRIPTION
This function loops through the cache and returns the cache index based on 
profile number. 
 
PARAMETERS 
profile_number -   profile number passed
 
DEPENDENCIES
  None

RETURN VALUE
  int8 cache_index
  Return a cache index of - 1 if no free cache index is found.
 
SIDE EFFECTS
  None
===========================================================================*/
int8 ds_3gpp_profile_cache_get_cache_index
(
	 ds_profile_db_profile_num_type  profile_number,
         ds_umts_pdp_subs_e_type subs_id
)
{
	int8 cache_index = -1;
	ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
          {
            return cache_index;
          }

	/*---------------------------------------------------------------------- 
	  Start with index 1, since cache ranges from 1-24
	  ----------------------------------------------------------------------*/
	for (cache_index = 1; cache_index <= DS_UMTS_MAX_PDP_PROFILE_NUM;
				cache_index++)
	{
		ds_umts_pdp_profile_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

		if (ds_umts_pdp_profile_ptr != NULL && ds_umts_pdp_profile_ptr->profile_number == profile_number)
		{
			break;
		}
	}

	if (cache_index > DS_UMTS_MAX_PDP_PROFILE_NUM)
	{
    DS_3GPP_MSG1_HIGH("Cache index not found for subs_id:%d", subs_id);
		cache_index = -1;
	}

	return cache_index;
}   

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_CACHE_INDEX_FOR_DDS

DESCRIPTION
This function loops through the cache and returns the cache index based on 
profile number. 
 
PARAMETERS 
profile_number -   profile number passed
 
DEPENDENCIES
  None

RETURN VALUE
  int8 cache_index
  Return a cache index of - 1 if no free cache index is found.
 
SIDE EFFECTS
  None
===========================================================================*/
int8 ds_3gpp_profile_cache_get_cache_index_for_dds
(
	 ds_profile_db_profile_num_type  profile_number
)
{
        int8 cache_index = -1;
	ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
        ds_umts_pdp_subs_e_type subs_id = ds_umts_get_default_data_subscription();
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          if(ds_umts_pdp_check_subscription_id(subs_id) == 
               DS_UMTS_PDP_INVALID_SUBS_ID)
          {
            return cache_index;
          }

	/*---------------------------------------------------------------------- 
	  Start with index 1, since cache ranges from 1-24
	  ----------------------------------------------------------------------*/

	for (cache_index = 1; cache_index <= DS_UMTS_MAX_PDP_PROFILE_NUM;
				cache_index++)
	{
		ds_umts_pdp_profile_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

		if (ds_umts_pdp_profile_ptr != NULL && ds_umts_pdp_profile_ptr->profile_number == profile_number)
		{
			break;
		}
	}

	if (cache_index > DS_UMTS_MAX_PDP_PROFILE_NUM)
	{
    DS_3GPP_MSG0_HIGH("Cache index not found");
		cache_index = -1;
	}

	return cache_index;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_VALID_PROFILE_NUMBERS

DESCRIPTION
This function gets the all valid 3gpp profile_numbers from the cache  
if the context_flag is valid 


PARAMETERS
  subs_id            - ds_umts_pdp_subs_e_type subscription id for profiles
  profile_number_arr - uint8 array to fill the valid profiles
  arr_size           - uint array size profile_number_arr

RETURN VALUE
  valid number of profile present in the cache

  @dependencies 
  caller function need to make sure the memory is already allocated as described by arr_size
===========================================================================  */

uint8  ds_3gpp_profile_cache_get_valid_profile_numbers
(
  ds_umts_pdp_subs_e_type subs_id,
  uint8  *profile_number_arr, 
  uint8  arr_size
)
{
  uint8 cache_index = 0;
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  uint8 profile_count = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
     DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return 0;
  }

  if(profile_number_arr == NULL)
  {
    DS_3GPP_MSG0_HIGH("profile_number_arr is NULL");
    return DS_UMTS_PDP_FAIL;
  }

  DS_3GPP_MSG1_LOW("arr_size is %d", arr_size);

  memset(profile_number_arr,0,arr_size * sizeof(uint8));
  /*---------------------------------------------------------------------- 
    Start with index 1, since cache ranges from 1-24
  ----------------------------------------------------------------------*/

  for (cache_index = 1; (cache_index <= DS_UMTS_MAX_PDP_PROFILE_NUM) && 
        (profile_count <= arr_size) ; cache_index++)
  {
    ds_umts_pdp_profile_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

    if (ds_umts_pdp_profile_ptr != NULL && ds_umts_pdp_profile_ptr->context.valid_flg)
    {
      profile_number_arr[profile_count] = (uint8)ds_umts_pdp_profile_ptr->profile_number;
      profile_count++;
    }
  }

  return profile_count;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_VALID_PROFILE_NUMBERS_FOR_DDS

DESCRIPTION
This function gets the all valid 3gpp profile_numbers from the cache  
if the context_flag is valid 


PARAMETERS
  profile_number_arr - uint8 array to fill the valid profiles
  arr_size           - uint array size profile_number_arr

RETURN VALUE
  valid number of profile present in the cache

  @dependencies 
  caller function need to make sure the memory is already allocated as described by arr_size
===========================================================================  */

uint8  ds_3gpp_profile_cache_get_valid_profile_numbers_for_dds
(
  uint8  *profile_number_arr, 
  uint8  arr_size
)
{
  uint8 cache_index = 0;
  ds_umts_pdp_profile_type *ds_umts_pdp_profile_ptr = NULL;
  uint8 profile_count = 0;
  ds_umts_pdp_subs_e_type subs_id = ds_umts_get_default_data_subscription();
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_umts_pdp_check_subscription_id(subs_id) == 
     DS_UMTS_PDP_INVALID_SUBS_ID)
  {
     return 0;
  }

  if(profile_number_arr == NULL)
  {
    DS_3GPP_MSG0_HIGH("profile_number_arr is NULL");
    return DS_UMTS_PDP_FAIL;
  }

  DS_3GPP_MSG1_LOW("arr_size is %d", arr_size);

  memset(profile_number_arr,0,arr_size * sizeof(uint8));
  /*---------------------------------------------------------------------- 
    Start with index 1, since cache ranges from 1-24
  ----------------------------------------------------------------------*/

  for (cache_index = 1; (cache_index <= DS_UMTS_MAX_PDP_PROFILE_NUM) && 
        (profile_count <= arr_size) ; cache_index++)
  {
    ds_umts_pdp_profile_ptr = DS_UMTS_PDP_PROFILE_CACHE(subs_id, cache_index);

    if (ds_umts_pdp_profile_ptr != NULL && ds_umts_pdp_profile_ptr->context.valid_flg)
    {
      profile_number_arr[profile_count] = (uint8)ds_umts_pdp_profile_ptr->profile_number;
      profile_count++;
    }
  }

  return profile_count;
}

#endif /*  (FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_LTE)) */
