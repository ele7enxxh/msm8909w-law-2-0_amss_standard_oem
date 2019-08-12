/*===========================================================================

                        P D P   R E G I S T R Y   L I B R A R Y 

                          
GENERAL DESCRIPTION
  This file contains data and function definitions for PDP registry.
  PDP registry provides an API library to maintain PDP profiles in 
  Embedded File System (EFS). This allows the profiles to be accessed
  across power cycles. ATCOP / UI and Data services use the API library
  to store and retrieve the PDP profile data
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds_umts_pdp_registry_init() should be called at task / system startup. 
  ds_umts_pdp_registry_init checks for the presence of the profiles in 
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

  h) On L4 based targets, the PDP registry APIs use critical section to
  serialize access to the PDP registry and the efs file system. So after the
  first call, subsequent calls to the PDP registry will be blocked untill
  first call completes. The PDP Registry uses the rex critical sections to
  prevent multiple processes to access the registry or the efs file system.

  i) the PDP registry APIs call the internal PDP registry functions to 
  perform the requested tasks. All the external APIs should access the
  registry through the internal functions and should not directly manipulate
  the registry cache or EFS.
  
Copyright (c) 2003-2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/dsprofile/src/dsumtspdpreg.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/11   ttv     Changes to add src_stat_desc as profile parameter.
02/28/11   hs      Added changes to support APN class in profile
02/16/11   ttv     Added DSDS changes for consolidated profile family.
01/17/11   ss      Added support for Iface linger.
11/22/10   ttv     Added support for DSDS.
07/14/10   asn     Address initialization of DS Profile and UMTS Profile Reg
05/17/10   hs      Added APN disable and PDN inactivity feature support.
10/12/09   vk      PDP profile changes for IPv4v6 support
09/30/09   asn     Consolidation of Families
09/29/09   ars     Fixed KW errors
09/23/09   vd      Added support for LTE Qos.
09/22/09   vrk     Merged LTE changes.
05/19/09   vk      Defeaturization of PDP registry for CMI
11/15/07   asn     Added support for new feature MT-PDP terminating in TE
05/15/07   aw      Added support for IMS IM CN Flag.
08/16/06   aw      Fixed errors with FEATURE_PROFILE_OTAP_ENHANCEMENTS on.
08/16/06   rsl     Added missing enter_critic_section in reset_gprs_qos_min API.
07/17/06   aw      Changed cscf to pcscf.
06/13/06   aw      Added support for cscf addresses for IMS.
01/20/06   aw      Replaced the task locks with rex critical sections and 
                   added a critical section during profile cache initialization.
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
#include "ds3gmgr.h"

#include <string.h>
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "dsumtspdpreg.h"
#include "dsati.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_LTE)
#include "dsumtspdpregint.h"
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_LTE) */

#if !defined(FEATURE_MODEM_RCINIT) && defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE)
#error code not present
#endif /* defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE) */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/



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
  Macros to Initialize, Enter and Leave the critical section associated with
  the profile. The critical section is used to provide access to the profile.
  The critical section controls the access to PDP Registry cache as well as 
  EFS.
---------------------------------------------------------------------------*/
#if !defined(FEATURE_MODEM_RCINIT) && defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE)
#error code not present
#else

  #define DS_UMTS_PDP_INIT_CRIT_SECTION() {                              \
    memset(&ds_umts_pdp_registry_critical_section,                       \
           0,                                                            \
           sizeof(rex_crit_sect_type));                                  \
    rex_init_crit_sect(&ds_umts_pdp_registry_critical_section);   }

  #define DS_UMTS_PDP_ENTER_CRIT_SECTION() {                              \
    rex_enter_crit_sect(&ds_umts_pdp_registry_critical_section);   }      

  #define DS_UMTS_PDP_LEAVE_CRIT_SECTION() {                              \
    rex_leave_crit_sect(&ds_umts_pdp_registry_critical_section); }        

#endif /* TEST_FRAMEWORK && FEATURE_QUBE */

  /*---------------------------------------------------------------------
    Critical Section to provide access to PDP registry cache and EFS.
  ---------------------------------------------------------------------*/
#if !defined(FEATURE_MODEM_RCINIT) && defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE)
  #error code not present
#else
  static rex_crit_sect_type        ds_umts_pdp_registry_critical_section;
#endif

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_LTE)
#define DS_UMTS_PDP_INTERNAL_API_NAME(func) func ## _internal

  /* PDP registry exists only if one of the above features are defined */
  /* In this case, do the following for each PDP reg API .. */
  /* Step 1: Enter the critical section for the profile cache */
  /* Step 2: Call the internal PDP Registry function for the api being called */
  /* Step 3: Leave the PDP Registry Critical Section */
#define DS_UMTS_PDP_API_INTERNAL(func, ...)             \
            DS_UMTS_PDP_ENTER_CRIT_SECTION();           \
            status = DS_UMTS_PDP_INTERNAL_API_NAME(func) (__VA_ARGS__); \
            DS_UMTS_PDP_LEAVE_CRIT_SECTION();

#define DS_UMTS_PDP_API_INTERNAL_NOARG(func)            \
            DS_UMTS_PDP_ENTER_CRIT_SECTION();           \
            status = DS_UMTS_PDP_INTERNAL_API_NAME(func) (); \
            DS_UMTS_PDP_LEAVE_CRIT_SECTION();
#else
  /* PDP registry does not exist in this case */
  /* Fail all PDP registry APIs */
#define DS_UMTS_PDP_API_INTERNAL(func, ...) \
            status = DS_UMTS_PDP_FAIL;

#define DS_UMTS_PDP_API_INTERNAL_NOARG(func) \
            status = DS_UMTS_PDP_FAIL;

#endif
/*===========================================================================
                    Forward declarations/prototypes
===========================================================================*/

/*===========================================================================
FUNCTION DS_UMTS_PDP_PROFILE_REGISTRY_POWER_UP_INIT

DESCRIPTION
  It initializes the PDP registry critical section. 
   
PARAMETERS 
  none

DEPENDENCIES 
  Should be called only ONCE during startup.

RETURN VALUE 

SIDE EFFECTS 
  
===========================================================================*/
void ds_umts_pdp_registry_power_up_init(void)
{
  /* Initialize the PDP Registry critical section */
  DS_UMTS_PDP_INIT_CRIT_SECTION();
}

/*===========================================================================
FUNCTION DS_UMTS_PDP_PROFILE_REGISTRY_INIT

DESCRIPTION
  This function initializes the PDP profile registry. It initializes the PDP 
  registry critical section. It calls the internal PDP Registry init function.
   
PARAMETERS 
  none

DEPENDENCIES 
  Should be called only ONCE during startup.

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS  - initialization was successfull
  DS_UMTS_PDP_FAIL     - initialization failed

SIDE EFFECTS 
  Creates the profiles in EFS if the profiles are not present at startup
===========================================================================*/
ds_umts_pdp_profile_status_etype ds_umts_pdp_registry_init(void)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_ENTER_CRIT_SECTION();

  DS_UMTS_PDP_API_INTERNAL_NOARG(ds_umts_pdp_registry_init);

  DS_UMTS_PDP_LEAVE_CRIT_SECTION();

  return status;
 
}


/*===========================================================================
FUNCTION DS_UMTS_GET_DEFAULT_DATA_SUBSCRIPTION

DESCRIPTION
  This function calls ds3gsubsmgr to get the default data subscription
PARAMETERS


DEPENDENCIES 
  None

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_subs_e_type  ds_umts_get_default_data_subscription
(
   void
)
{
  ds_umts_pdp_subs_e_type data_subs = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
  /*- - - - - - - - - - - -  - - - - - - - - - -  - - - - - - - - - - - - - */

  data_subs = dsumts_subs_mgr_get_subs_id(ds3g_get_ps_subs_id());

  return data_subs;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ALL_DATA_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve all the
  parameters associated with a profile number. 
  
  NOTE:
  No check is done to see if a valid context is defined. The caller has to 
  check if the context and associated parameter set is defined and use it
  accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id. 
  profile_data_ptr    : pointer to store the profile information

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_all_data_per_subs
(
  uint16                   profile_number, 
  ds_umts_pdp_subs_e_type  subs_id,
  ds_umts_pdp_profile_type *profile_data_ptr
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_all_data, \
    profile_number, \
    subs_id, \
    profile_data_ptr \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ALL_DATA

DESCRIPTION
  This function calls the registry internal function to retrieve all the
  parameters associated with a profile number. 
  
  NOTE:
  No check is done to see if a valid context is defined. The caller has to 
  check if the context and associated parameter set is defined and use it
  accordingly.

PARAMETERS
  profile_number  : profile number
  profile_data_ptr    : pointer to store the profile information

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_all_data
(
  uint16                   profile_number, 
  ds_umts_pdp_profile_type *profile_data_ptr
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_all_data, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    profile_data_ptr \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NAME_UTF16

DESCRIPTION
  This function calls the internal registry function to retrieve the name of
  the specified profile number that is stored in UTF16 format. 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_name_utf16
(
  uint16                profile_number,
  uint16                *profile_name_buf,
  byte                  profile_name_buf_len
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_name_utf16, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    profile_name_buf, \
    profile_name_buf_len \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NAME_UTF16_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the name of
  the specified profile number that is stored in UTF16 format. 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_name_utf16_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                *profile_name_buf,
  byte                  profile_name_buf_len
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_name_utf16, \
    profile_number, \
    subs_id, \
    profile_name_buf, \
    profile_name_buf_len \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NAME

DESCRIPTION
  This function call the internal registry function to retrieve the name of
  the specified profile number . 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_name
(
  uint16                profile_number,
  byte                  *profile_name_buf,
  byte                  profile_name_buf_len
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_name, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    profile_name_buf, \
    profile_name_buf_len \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NAME_PER_SUBS

DESCRIPTION
  This function call the internal registry function to retrieve the name of
  the specified profile number . 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_name_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                  *profile_name_buf,
  byte                  profile_name_buf_len
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_name, \
    profile_number, \
    subs_id, \
    profile_name_buf, \
    profile_name_buf_len \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_OTAP_NAPID

DESCRIPTION
  This function calls the registry internal function to retrieve the NAPID
  of the specified profile number . 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number          : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_otap_napid
(
  uint16                profile_number,
  byte                  *otap_napid_buf,
  byte                  otap_napid_buf_len
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_otap_napid, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    otap_napid_buf, \
    otap_napid_buf_len \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_OTAP_NAPID_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the NAPID
  of the specified profile number . 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number          : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_otap_napid_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                  *otap_napid_buf,
  byte                  otap_napid_buf_len
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_otap_napid, \
    profile_number, \
    subs_id, \
    otap_napid_buf, \
    otap_napid_buf_len \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IS_READ_ONLY_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the
  "read_only_flag" specified for a profile. 
  The "read_only_flag" indicates if the profile is read-only (TRUE) or 
  not (FALSE)
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  read_only_flag  : pointer to store the read_only_flag

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_is_read_only_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *read_only_flag
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_is_read_only, \
    profile_number, \
    subs_id, \
    read_only_flag \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IS_READ_ONLY

DESCRIPTION
  This function calls the registry internal function to retrieve the
  "read_only_flag" specified for a profile. 
  The "read_only_flag" indicates if the profile is read-only (TRUE) or 
  not (FALSE)
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  read_only_flag  : pointer to store the read_only_flag

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_is_read_only
(
  uint16                profile_number,
  boolean               *read_only_flag
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_is_read_only, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    read_only_flag \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IS_OTAP_ENABLED_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the
  "otap_enabled_flag" specified for a profile. 
  The "otap_enabled_flag" indicates if the context is otap_enabled (TRUE) or 
  not (FALSE)
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number     : profile number
  subs_id         :  Subscription id.
  otap_enabled_flag  : pointer to store the otap_enabled_flag

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_is_otap_enabled_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *otap_enabled_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_is_otap_enabled, \
    profile_number, \
    subs_id, \
    otap_enabled_flag \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IS_OTAP_ENABLED

DESCRIPTION
  This function calls the internal registry function to retrieve the
  "otap_enabled_flag" specified for a profile. 
  The "otap_enabled_flag" indicates if the context is otap_enabled (TRUE) or 
  not (FALSE)
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number     : profile number
  otap_enabled_flag  : pointer to store the otap_enabled_flag

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_is_otap_enabled
(
  uint16                profile_number,
  boolean               *otap_enabled_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_is_otap_enabled, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    otap_enabled_flag \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the context
  information for a profile number. 
  This information is retrieved only if the context definition is valid
  (valid_flg for context definition is TRUE)

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_context_info_per_subs
(
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_context_type  *pdp_context_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_context_info, \
    profile_number, \
    subs_id, \
    pdp_context_data \
  );

  return status;
}




/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO

DESCRIPTION
  This function calls the registry internal function to retrieve the context
  information for a profile number. 
  This information is retrieved only if the context definition is valid
  (valid_flg for context definition is TRUE)

PARAMETERS
  profile_number  : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_context_info
(
  uint16                    profile_number,
  ds_umts_pdp_context_type  *pdp_context_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_context_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdp_context_data \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_IS_VALID_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the
  "valid_flg" specified in the context information for a profile number.
  The "valid_flg" indicates if the context has been defined (TRUE)
  or not (FALSE)

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  valid_flg      : pointer to store the valid_flg

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_context_info_is_valid_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *valid_flg
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_context_info_is_valid, \
    profile_number, \
    subs_id, \
    valid_flg \
  );
  
  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_IS_VALID

DESCRIPTION
  This function calls the registry internal function to retrieve the
  "valid_flg" specified in the context information for a profile number.
  The "valid_flg" indicates if the context has been defined (TRUE)
  or not (FALSE)

PARAMETERS
  profile_number  : profile number
  valid_flg      : pointer to store the valid_flg

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_context_info_is_valid
(
  uint16                profile_number,
  boolean               *valid_flg
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_context_info_is_valid, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    valid_flg \
  );
  
  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_PDP_ADDRESS_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the
  "pdp address" specified in the context information for a profile number.
  This parameter is retrieved only if a valid context is defined for the
  profile specified.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_context_info_pdp_address_per_subs 
(  
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_addr_type     *pdp_address 
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_context_info_pdp_address, \
    profile_number, \
    subs_id, \
    pdp_address \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_PDP_ADDRESS

DESCRIPTION
  This function calls the registry internal function to retrieve the
  "pdp address" specified in the context information for a profile number.
  This parameter is retrieved only if a valid context is defined for the
  profile specified.

PARAMETERS
  profile_number  : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_context_info_pdp_address 
(  
  uint16                    profile_number,
  ds_umts_pdp_addr_type     *pdp_address 
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_context_info_pdp_address, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdp_address \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_APN_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the "apn" 
  specified in the context information for a profile number. This parameter
  is retrieved only if a valid context is defined for the profile specified.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_context_info_apn_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                  *apn_buf,
  byte                  apn_buf_len
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_context_info_apn, \
    profile_number, \
    subs_id, \
    apn_buf, \
    apn_buf_len \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_APN

DESCRIPTION
  This function calls the registry internal function to retrieve the "apn" 
  specified in the context information for a profile number. This parameter
  is retrieved only if a valid context is defined for the profile specified.

PARAMETERS
  profile_number  : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_context_info_apn
(
  uint16                profile_number,
  byte                  *apn_buf,
  byte                  apn_buf_len
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_context_info_apn, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    apn_buf, \
    apn_buf_len \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_IPV4_ADDR_ALLOC_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the "ipv4
  addr alloc" mechanism specified in the context information for a profile 
  number. This parameter is retrieved only if a valid context is defined for 
  the profile specified.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_context_info_ipv4_addr_alloc_per_subs
(
  uint16                              profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_ipv4_addr_alloc_e_type *ipv4_addr_alloc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_ENTER_CRIT_SECTION();

  /*-----------------------------------------------------------------------
    Get PDP profile context info IPv4 addr alloc
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_context_info_ipv4_addr_alloc, \
    profile_number, \
    subs_id, \
    ipv4_addr_alloc \
  );

  DS_UMTS_PDP_LEAVE_CRIT_SECTION();
  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_IPV4_ADDR_ALLOC

DESCRIPTION
  This function calls the registry internal function to retrieve the "ipv4
  addr alloc" mechanism specified in the context information for a profile 
  number. This parameter is retrieved only if a valid context is defined for 
  the profile specified.

PARAMETERS
  profile_number  : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_context_info_ipv4_addr_alloc
(
  uint16                              profile_number,
  ds_umts_pdp_ipv4_addr_alloc_e_type *ipv4_addr_alloc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_ENTER_CRIT_SECTION();

  /*-----------------------------------------------------------------------
    Get PDP profile context info IPv4 addr alloc
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_context_info_ipv4_addr_alloc, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    ipv4_addr_alloc \
  );

  DS_UMTS_PDP_LEAVE_CRIT_SECTION();
  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_PER_SUBS

DESCRIPTION
  This function retrieves the authentication information for a profile number.

  The auth info is returned ir-respective of the state of the context
  definition for the profile.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  pdp_auth_data   : pointer to store the authentication information

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_auth_info_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_auth_type *pdp_auth_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_auth_info, \
    profile_number, \
    subs_id, \
    pdp_auth_data \
  );

  return status;  
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO

DESCRIPTION
  This function retrieves the authentication information for a profile number.

  The auth info is returned ir-respective of the state of the context
  definition for the profile.

PARAMETERS
  profile_number  : profile number
  pdp_auth_data   : pointer to store the authentication information

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_auth_info
(
  uint16                profile_number,
  ds_umts_pdp_auth_type *pdp_auth_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_auth_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdp_auth_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_TYPE_PER_SUBS

DESCRIPTION
  This function retrieves the "authentication type" specified in the
  authentication information for a profile number . This parameter is 
  retrieved ir-respective of the state of the context definition of
  the profile

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  auth_type       : pointer to store the authentication type

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_auth_info_type_per_subs
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_auth_enum_type *auth_type
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_auth_info_type, \
    profile_number, \
    subs_id, \
    auth_type \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_TYPE

DESCRIPTION
  This function retrieves the "authentication type" specified in the
  authentication information for a profile number . This parameter is 
  retrieved ir-respective of the state of the context definition of
  the profile

PARAMETERS
  profile_number  : profile number
  auth_type       : pointer to store the authentication type

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_auth_info_type
(
  uint16                     profile_number,
  ds_umts_pdp_auth_enum_type *auth_type
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_auth_info_type, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    auth_type \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_USERNAME_PER_SUBS

DESCRIPTION
  This function retrieves the "username" specified in the authentication 
  information for a profile number . This parameter is retrieved 
  ir-respective of the state of the context definition of the profile

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_auth_info_username_per_subs
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                       *username_buf,
  byte                       username_buf_len
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_auth_info_username, \
    profile_number, \
    subs_id, \
    username_buf, \
    username_buf_len \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_USERNAME

DESCRIPTION
  This function retrieves the "username" specified in the authentication 
  information for a profile number . This parameter is retrieved 
  ir-respective of the state of the context definition of the profile

PARAMETERS
  profile_number  : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_auth_info_username
(
  uint16                     profile_number,
  byte                       *username_buf,
  byte                       username_buf_len
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_auth_info_username, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    username_buf, \
    username_buf_len \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_PASSWORD_PER_SUBS

DESCRIPTION
  This function retrieves the password specified in the authentication 
  information for a profile number . This parameter is retrieved 
  ir-respective of the state of the context definition of the profile

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_auth_info_password_per_subs
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                       *password_buf,
  byte                       password_buf_len
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_auth_info_password, \
    profile_number, \
    subs_id, \
    password_buf, \
    password_buf_len \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_PASSWORD

DESCRIPTION
  This function retrieves the password specified in the authentication 
  information for a profile number . This parameter is retrieved 
  ir-respective of the state of the context definition of the profile

PARAMETERS
  profile_number  : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_auth_info_password
(
  uint16                     profile_number,
  byte                       *password_buf,
  byte                       password_buf_len
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_auth_info_password, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    password_buf, \
    password_buf_len \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_UMTS_QOS_REQ_INFO_PER_SUBS

DESCRIPTION
  This function retrieves the UMTS qos parameters (requested) specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  pdp_umts_qos_data   : pointer to store the QOS parameters

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_umts_qos_req_info_per_subs
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_umts_qos_params_type *pdp_umts_qos_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_umts_qos_req_info, \
    profile_number, \
    subs_id, \
    pdp_umts_qos_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_UMTS_QOS_REQ_INFO

DESCRIPTION
  This function retrieves the UMTS qos parameters (requested) specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  pdp_umts_qos_data   : pointer to store the QOS parameters

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_umts_qos_req_info
(
  uint16                       profile_number,
  ds_umts_umts_qos_params_type *pdp_umts_qos_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_umts_qos_req_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdp_umts_qos_data \
  );

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_UMTS_QOS_MIN_INFO_PER_SUBS

DESCRIPTION
  This function retrieves the UMTS qos parameters (minimum) specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  pdp_umts_qos_data   : pointer to store the QOS parameters

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_umts_qos_min_info_per_subs
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_umts_qos_params_type *pdp_umts_qos_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_umts_qos_min_info, \
    profile_number, \
    subs_id, \
    pdp_umts_qos_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_UMTS_QOS_MIN_INFO

DESCRIPTION
  This function retrieves the UMTS qos parameters (minimum) specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  pdp_umts_qos_data   : pointer to store the QOS parameters

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_umts_qos_min_info
(
  uint16                       profile_number,
  ds_umts_umts_qos_params_type *pdp_umts_qos_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_umts_qos_min_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdp_umts_qos_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_SRC_STAT_DESC_REQ_INFO_PER_SUBS

DESCRIPTION
  This function retrieves the src_stat_desc value specified for a
  profile number .

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_src_stat_desc_req_info_per_subs
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32                       *src_stat_desc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_src_stat_desc_req_info, \
    profile_number, \
    subs_id, \
    src_stat_desc \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_SRC_STAT_DESC_REQ_INFO

DESCRIPTION
  This function retrieves the src_stat_desc value specified for a
  profile number .

PARAMETERS
  profile_number  : profile number
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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_src_stat_desc_req_info
(
  uint16                       profile_number,
  uint32                       *src_stat_desc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_src_stat_desc_req_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    src_stat_desc \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_SRC_STAT_DESC_MIN_INFO_PER_SUBS

DESCRIPTION
  This function retrieves the src_stat_desc value specified for a
  profile number .

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_src_stat_desc_min_info_per_subs
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32                       *src_stat_desc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_src_stat_desc_min_info, \
    profile_number, \
    subs_id, \
    src_stat_desc \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_SRC_STAT_DESC_MIN_INFO

DESCRIPTION
  This function retrieves the src_stat_desc value specified for a
  profile number .

PARAMETERS
  profile_number  : profile number
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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_src_stat_desc_min_info
(
  uint16                       profile_number,
  uint32                       *src_stat_desc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_src_stat_desc_min_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    src_stat_desc \
  );

  return status;  
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_GPRS_QOS_REQ_INFO_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the GPRS
  requested" qos parameters specified for a profile number .
  No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  pdp_gprs_qos_req_data   : pointer to store the QOS parameters

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_gprs_qos_req_info_per_subs
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_gprs_qos_params_type *pdp_gprs_qos_req_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_gprs_qos_req_info, \
    profile_number, \
    subs_id, \
    pdp_gprs_qos_req_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_GPRS_QOS_REQ_INFO

DESCRIPTION
  This function calls the internal registry function to retrieve the GPRS
  requested" qos parameters specified for a profile number .
  No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  pdp_gprs_qos_req_data   : pointer to store the QOS parameters

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_gprs_qos_req_info
(
  uint16                       profile_number,
  ds_umts_gprs_qos_params_type *pdp_gprs_qos_req_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_gprs_qos_req_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdp_gprs_qos_req_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_GPRS_QOS_MIN_INFO_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the GPRS
  "minimum" qos parameters specified for a profile number . No check is done
  to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  pdp_gprs_qos_min_data   : pointer to store the QOS parameters

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_gprs_qos_min_info_per_subs
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_gprs_qos_params_type *pdp_gprs_qos_min_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_gprs_qos_min_info, \
    profile_number, \
    subs_id, \
    pdp_gprs_qos_min_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_GPRS_QOS_MIN_INFO

DESCRIPTION
  This function calls the internal registry function to retrieve the GPRS
  "minimum" qos parameters specified for a profile number . No check is done
  to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  pdp_gprs_qos_min_data   : pointer to store the QOS parameters

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_gprs_qos_min_info
(
  uint16                       profile_number,
  ds_umts_gprs_qos_params_type *pdp_gprs_qos_min_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_gprs_qos_min_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdp_gprs_qos_min_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_3GPP_GET_PDP_PROFILE_LTE_QOS_INFO_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the LTE
  qos parameters specified for a profile number . No check is done
  to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number     : profile number
  subs_id         :  Subscription id.
  pdp_lte_qos_data   : pointer to store the QOS parameters

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
ds_umts_pdp_profile_status_etype  ds_3gpp_get_pdp_profile_lte_qos_info_per_subs
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_3gpp_lte_qos_params_type *pdp_lte_qos_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_ENTER_CRIT_SECTION();

 /*-------------------------------------------------------------------
    calls the internal registry function to get the GPRS QOS min info.
  -------------------------------------------------------------------*/
  status = ds_3gpp_get_pdp_profile_lte_qos_info_internal(
                                       profile_number,
                                       subs_id, 
                                       pdp_lte_qos_data);

  DS_UMTS_PDP_LEAVE_CRIT_SECTION();
  return status;  
}



/*===========================================================================
FUNCTION DS_3GPP_GET_PDP_PROFILE_LTE_QOS_INFO

DESCRIPTION
  This function calls the internal registry function to retrieve the LTE
  qos parameters specified for a profile number . No check is done
  to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number     : profile number
  pdp_lte_qos_data   : pointer to store the QOS parameters

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
ds_umts_pdp_profile_status_etype  ds_3gpp_get_pdp_profile_lte_qos_info
(
  uint16                       profile_number,
  ds_3gpp_lte_qos_params_type *pdp_lte_qos_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_ENTER_CRIT_SECTION();

 /*-------------------------------------------------------------------
    calls the internal registry function to get the GPRS QOS min info.
  -------------------------------------------------------------------*/
  status = ds_3gpp_get_pdp_profile_lte_qos_info_internal(
                                       profile_number,
                                       ds_umts_get_default_data_subscription(), 
                                       pdp_lte_qos_data);

  DS_UMTS_PDP_LEAVE_CRIT_SECTION();
  return status;  
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the DNS
  addresses specified for a profile number . No check is done to see if a
  valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined ( non zero dns address values ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  pdp_dns_addr    : pointer to store the dns addresses

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_dns_address_info_per_subs
(
  uint16                         profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_dns_addresses_type *pdp_dns_addr
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_dns_address_info, \
    profile_number, \
    subs_id, \
    pdp_dns_addr \
  );

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO

DESCRIPTION
  This function calls the internal registry function to retrieve the DNS
  addresses specified for a profile number . No check is done to see if a
  valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined ( non zero dns address values ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  pdp_dns_addr    : pointer to store the dns addresses

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_dns_address_info
(
  uint16                         profile_number,
  ds_umts_pdp_dns_addresses_type *pdp_dns_addr
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_dns_address_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdp_dns_addr \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_PRIMARY_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the primary
  DNS address specified for a profile number .
  No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (non zero dns address ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  primary_dns     : pointer to store the dns address

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_dns_address_info_primary_per_subs
(
  uint16                  profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_addr_type   *primary_dns
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_dns_address_info_primary, \
    profile_number, \
    subs_id, \
    primary_dns \
  );

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_PRIMARY

DESCRIPTION
  This function calls the internal registry function to retrieve the primary
  DNS address specified for a profile number .
  No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (non zero dns address ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  primary_dns     : pointer to store the dns address

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_dns_address_info_primary
(
  uint16                  profile_number,
  ds_umts_pdp_addr_type   *primary_dns
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_dns_address_info_primary, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    primary_dns \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_SECONDARY_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the
  secondary DNS address specified for a profile number . No check is done to
  see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (non zero dns address ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  secondary_dns   : pointer to store the dns address

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_dns_address_info_secondary_per_subs
(
  uint16                   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_addr_type   *secondary_dns
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_dns_address_info_secondary, \
    profile_number, \
    subs_id, \
    secondary_dns \
  );

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_SECONDARY

DESCRIPTION
  This function calls the internal registry function to retrieve the
  secondary DNS address specified for a profile number . No check is done to
  see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (non zero dns address ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  secondary_dns   : pointer to store the dns address

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_dns_address_info_secondary
(
  uint16                   profile_number,
  ds_umts_pdp_addr_type   *secondary_dns
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_dns_address_info_secondary, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    secondary_dns \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_TFT_INFO_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the Traffic
  Flow Template (TFT) parameters specified for a profile number and filter
  identifer. No check is done to see if a valid context is defined.  The
  caller has to check if the context and associated parameter set is defined
  (valid_flg of TFT is set) and use it accordingly.

PARAMETERS
  profile_number : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_tft_info_per_subs
(
  uint16                  profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                    filter_id,
  ds_umts_tft_params_type *pdp_tft_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_tft_info, \
    profile_number, \
    subs_id, \
    filter_id, \
    pdp_tft_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_TFT_INFO

DESCRIPTION
  This function calls the internal registry function to retrieve the Traffic
  Flow Template (TFT) parameters specified for a profile number and filter
  identifer. No check is done to see if a valid context is defined.  The
  caller has to check if the context and associated parameter set is defined
  (valid_flg of TFT is set) and use it accordingly.

PARAMETERS
  profile_number : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_tft_info
(
  uint16                  profile_number,
  byte                    filter_id,
  ds_umts_tft_params_type *pdp_tft_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_tft_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    filter_id, \
    pdp_tft_data \
  );

  return status;  
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_TFT_ALL_INFO_PER_SUBS

DESCRIPTION

  This function calls the internal registry function to retrieve ALL the
  Traffic Flow Template (TFT) parameters specified for a profile number. No
  check is done to see if a valid context is defined.  The caller is
  responsible for allocating sufficent storage for the returned structure.
  The caller has to check if the context and associated parameter set is
  defined (valid_flg of TFT is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  pdp_tft_data   : pointer to store the TFT parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  Array of size DS_UMTS_MAX_TFT_PARAM_SETS is returned to caller.
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_tft_all_info_per_subs
(
  uint16                   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_tft_params_type  *pdp_tft_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_tft_all_info, \
    profile_number, \
    subs_id, \
    pdp_tft_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_TFT_ALL_INFO

DESCRIPTION

  This function calls the internal registry function to retrieve ALL the
  Traffic Flow Template (TFT) parameters specified for a profile number. No
  check is done to see if a valid context is defined.  The caller is
  responsible for allocating sufficent storage for the returned structure.
  The caller has to check if the context and associated parameter set is
  defined (valid_flg of TFT is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  pdp_tft_data   : pointer to store the TFT parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  Array of size DS_UMTS_MAX_TFT_PARAM_SETS is returned to caller.
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_tft_all_info
(
  uint16                   profile_number,
  ds_umts_tft_params_type  *pdp_tft_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_tft_all_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdp_tft_data \
  );

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_LINGER_PARAMS_INFO_PER_SUBS

DESCRIPTION
  This function retrieves the linger parameters from the profile.

PARAMETERS
  profile_number     : profile number
  subs_id         :  Subscription id.
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
   ds_umts_get_pdp_profile_linger_params_info_per_subs
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_linger_params_type *linger_params_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_linger_params_info, \
    profile_number, \
    subs_id, \
    linger_params_data \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_LINGER_PARAMS_INFO

DESCRIPTION
  This function retrieves the linger parameters from the profile.

PARAMETERS
  profile_number     : profile number
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
   ds_umts_get_pdp_profile_linger_params_info
(
  uint16                     profile_number,
  ds_umts_linger_params_type *linger_params_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_linger_params_info, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    linger_params_data \
  );

  return status;  
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_SOCK_CALLS

DESCRIPTION
  This function calls the internal registry function to retrieve the profile
  number specififed for sockets data calls ( sockets calls ).  The cached
  value is return if available; otherwise, the read is done from the EFS
  file.

PARAMETERS
  profile_number  : pointer to store profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  The cached sockets call number is updated if necessary.
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_num_for_sock_calls
( 
  byte *profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_num_for_embedded_calls, \
    ds_umts_get_default_data_subscription(), \
    profile_number \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_SOCK_CALLS_PER_SUBS
===========================================================================*/
/**
  Calls the internal registry function to retrieve the profile
  number specified for sockets data calls (sockets calls). The cached
  value is returned if available; otherwise, the read is performed from 
  the EFS file.

  @param profile_number [out] Pointer to the stored profile number.
  @param subs_id [in] Subscription ID for the profile

  @return 
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in accessing the file.

  @dependencies 
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized.
*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_num_for_sock_calls_per_subs
( 
  ds_umts_pdp_subs_e_type subs_id,
  byte     *profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_num_for_embedded_calls, \
    subs_id, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_DEFAULT_PROFILE_NUM_FOR_RMNET_CALLS

DESCRIPTION
  This function calls the internal registry function to retrieve the profile
  number specififed for RmNet data calls.  The cached value is return if 
  available; otherwise, the read is done from the EFS file.

PARAMETERS
  profile_number  : pointer to store profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  The cached sockets call number is updated if necessary.
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_num_for_rmnet_calls
( 
  byte *profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_num_for_tethered_calls, \
    ds_umts_get_default_data_subscription(), \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_RMNET_CALLS_PER_SUBS
===========================================================================*/
/**
  Calls the internal registry function to retrieve the profile
  number specified for RmNet data calls. The cached value is returned if 
  available; otherwise, the read is performed from the EFS file.

  @param profile_number [out] Pointer to the stored profile number.
  @param subs_id [in] Subscription ID for the profile

  @return 
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in accessing the file.
	
  @dependencies 
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized.
*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_num_for_rmnet_calls_per_subs
( 
  ds_umts_pdp_subs_e_type subs_id,
  byte     *profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_num_for_tethered_calls, \
    subs_id, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_DEFAULT_PROFILE_NUM_FOR_ATCOP_CALLS

DESCRIPTION
  This function calls the internal registry function to retrieve the profile
  number specififed for ATCoP calls. The cached value is return if available
  otherwise, the read is done from the EFS file.

PARAMETERS
  profile_number  : pointer to store profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  The cached sockets call number is updated if necessary.
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_num_for_atcop_calls
( 
  byte *profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_num_for_tethered_calls, \
    ds_umts_get_default_data_subscription(), \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_ATCOP_CALLS_PER_SUBS
===========================================================================*/
/**
  Calls the internal registry function to retrieve the profile
  number specified for ATCOP data calls. The cached value is returned if 
  available; otherwise, the read is performed from the EFS file.
 
  @param subs_id [in] Subscription ID for the profile
  @param profile_number [out] Pointer to the stored profile number.

  @return 
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in accessing the file.

  @dependencies 
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized.
*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_num_for_atcop_calls_per_subs
( 
  ds_umts_pdp_subs_e_type subs_id,
  byte     *profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_num_for_tethered_calls, \
    subs_id, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_EMBEDDED_CALLS

DESCRIPTION
  This function calls the internal registry function to retrieve the profile
  number specififed for embedded data calls.  The cached value is return if 
  available; otherwise, the read is done from the EFS file.

PARAMETERS
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

ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_num_for_embedded_calls
( 
  byte     *profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_num_for_embedded_calls, \
    ds_umts_get_default_data_subscription(), \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_EMBEDDED_CALLS_PER_SUBS
===========================================================================*/
/**
  Calls the internal registry function to retrieve the profile number 
  specified for embedded data calls. The cached value is returned if 
  available; otherwise, the read is performed from the EFS file.
 
  @param subs_id [in] Subscription ID for the profile
  @param profile_number [out] Pointer to the stored profile number.

  @return 
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in accessing the file.

  @dependencies 
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized.
*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_num_for_embedded_calls_per_subs
( 
  ds_umts_pdp_subs_e_type subs_id,
  byte     *profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_num_for_embedded_calls, \
    subs_id, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_DEFAULT_PROFILE_NUM_FOR_TETHERED_CALLS

DESCRIPTION
  This function calls the internal registry function to retrieve the profile
  number specififed for tethered data calls.  The cached value is return if 
  available; otherwise, the read is done from the EFS file.

PARAMETERS
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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_num_for_tethered_calls
( 
  byte     *profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_num_for_tethered_calls, \
    ds_umts_get_default_data_subscription(), \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_FLAG_PER_SUB

DESCRIPTION
  This function calls the internal registry function to retrieve the
  "request_pcscf_address" flag specified for a profile. 
  The "request_pcscf_address" indicates if the pcscf address should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  request_pcscf_address_flag   : pointer to store the request_pcscf_address_flag


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
  ds_umts_get_pdp_profile_request_pcscf_address_flag_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *request_pcscf_address_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_request_pcscf_address_flag, \
    profile_number, \
    subs_id, \
    request_pcscf_address_flag \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_FLAG

DESCRIPTION
  This function calls the internal registry function to retrieve the
  "request_pcscf_address" flag specified for a profile. 
  The "request_pcscf_address" indicates if the pcscf address should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_request_pcscf_address_flag
(
  uint16                profile_number,
  boolean               *request_pcscf_address_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_request_pcscf_address_flag, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    request_pcscf_address_flag \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_USING_DHCP_FLAG_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the
  "request_pcscf_address_using_dhcp_flag" flag specified for a profile. 
  The "request_pcscf_address_using_dhcp" indicates if the pcscf address should
  be requested for this profile using DHCP or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_request_pcscf_address_using_dhcp_flag_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *request_pcscf_address_using_dhcp_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_request_pcscf_address_using_dhcp_flag, \
    profile_number, \
    subs_id, \
    request_pcscf_address_using_dhcp_flag \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_USING_DHCP_FLAG

DESCRIPTION
  This function calls the internal registry function to retrieve the
  "request_pcscf_address_using_dhcp_flag" flag specified for a profile. 
  The "request_pcscf_address_using_dhcp" indicates if the pcscf address should
  be requested for this profile using DHCP or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_request_pcscf_address_using_dhcp_flag
(
  uint16                profile_number,
  boolean               *request_pcscf_address_using_dhcp_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_request_pcscf_address_using_dhcp_flag, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    request_pcscf_address_using_dhcp_flag \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IM_CN_FLAG_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the
  "im_cn" flag specified for a profile. 
  The "im_cn" indicates if the im_cn should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  im_cn_flag   : pointer to store the im_cn_flag

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
  ds_umts_get_pdp_profile_im_cn_flag_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *im_cn_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_im_cn_flag, \
    profile_number, \
    subs_id, \
    im_cn_flag \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IM_CN_FLAG

DESCRIPTION
  This function calls the internal registry function to retrieve the
  "im_cn" flag specified for a profile. 
  The "im_cn" indicates if the im_cn should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  im_cn_flag   : pointer to store the im_cn_flag

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
  ds_umts_get_pdp_profile_im_cn_flag
(
  uint16                profile_number,
  boolean               *im_cn_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_im_cn_flag, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    im_cn_flag \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_DISABLE_FLAG_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_diable_flag" mechanism specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_apn_disable_flag_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean  *apn_disable_flag
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Get PDP profile context info IPv4 addr alloc
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_apn_disable_flag, \
    profile_number, \
    subs_id, \
    apn_disable_flag \
  );

  return status;
} /* ds_umts_get_pdp_profile_apn_disable_flag */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_DISABLE_FLAG

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_diable_flag" mechanism specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_apn_disable_flag
(
  uint16   profile_number,
  boolean  *apn_disable_flag
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Get PDP profile context info IPv4 addr alloc
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_apn_disable_flag, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    apn_disable_flag \
  );

  return status;
} /* ds_umts_get_pdp_profile_apn_disable_flag */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_INACTIVITY_TIMER_VAL_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "inactivity_timer_val" value specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number       : profile number
  subs_id         :  Subscription id.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_inactivity_timer_val_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32   *inactivity_timer_val
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Get PDP profile context info IPv4 addr alloc
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_inactivity_timer_val, \
    profile_number, \
    subs_id, \
    inactivity_timer_val \
  );

  return status;
} /* ds_umts_get_pdp_profile_inactivity_timer_val */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_INACTIVITY_TIMER_VAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "inactivity_timer_val" value specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number       : profile number
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_inactivity_timer_val
(
  uint16   profile_number,
  uint32   *inactivity_timer_val
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Get PDP profile context info IPv4 addr alloc
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_inactivity_timer_val, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    inactivity_timer_val \
  );

  return status;
} /* ds_umts_get_pdp_profile_inactivity_timer_val */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_CLASS_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_class" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id         :  Subscription id.
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
ds_umts_get_pdp_profile_apn_class_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint8    *apn_class
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the APN_CLASS info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_apn_class, \
    profile_number, \
    subs_id, \
    apn_class \
  );

  return status;
} /* ds_umts_get_pdp_profile_apn_class */



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_CLASS

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_class" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
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
ds_umts_get_pdp_profile_apn_class
(
  uint16   profile_number,
  uint8    *apn_class
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the APN_CLASS info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_apn_class, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    apn_class \
  );

  return status;
} /* ds_umts_get_pdp_profile_apn_class */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_BEARER_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_bearer" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id         :  Subscription id.
  apn_bearer        : pointer to the uint8 (to hold APN class)

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
ds_umts_get_pdp_profile_apn_bearer_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint8    *apn_bearer
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the APN_CLASS info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_apn_bearer, \
    profile_number, \
    subs_id, \
    apn_bearer \
  );

  return status;
} /* ds_umts_get_pdp_profile_apn_class */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_BEARER

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_bearer" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  apn_bearer        : pointer to the uint8 (to hold APN class)

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
ds_umts_get_pdp_profile_apn_bearer
(
  uint16   profile_number,
  uint8    *apn_bearer
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the APN_CLASS info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_apn_bearer, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    apn_bearer \
  );

  return status;
} /* ds_umts_get_pdp_profile_apn_class */




/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_EMERGENCY_CALLS_ARE_SUPPORTED_FLAG_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to retrieve the
  "emergency_calls_are_supported" flag specified for a profile. 
  This flag indicates if IMS emergency calls can be made using this profile.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id         :  Subscription id.
  emergency_calls_are_supported   : pointer to return the value in the field.

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
  ds_umts_get_pdp_profile_emergency_calls_are_supported_flag_per_subs
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *emergency_calls_are_supported_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
#ifdef FEATURE_EMERGENCY_PDN
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_emergency_calls_are_supported_flag, \
    profile_number, \
    subs_id, \
    emergency_calls_are_supported_flag \
  );
#else
  status = DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /* FEATURE_EMERGENCY_PDN */
  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_EMERGENCY_CALLS_ARE_SUPPORTED_FLAG

DESCRIPTION
  This function calls the internal registry function to retrieve the
  "emergency_calls_are_supported" flag specified for a profile. 
  This flag indicates if IMS emergency calls can be made using this profile.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  emergency_calls_are_supported   : pointer to return the value in the field.

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
  ds_umts_get_pdp_profile_emergency_calls_are_supported_flag
(
  uint16                profile_number,
  boolean               *emergency_calls_are_supported_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
#ifdef FEATURE_EMERGENCY_PDN
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_emergency_calls_are_supported_flag, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    emergency_calls_are_supported_flag \
  );
#else
  status = DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /* FEATURE_EMERGENCY_PDN */
  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MCC_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "MCC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id         :  Subscription id.
  mcc              : pointer to the uint16 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_mcc_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16    *mcc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mcc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_mcc, \
    profile_number, \
    subs_id, \
    mcc \
  );

  return status;
} /* ds_umts_get_pdp_profile_mcc */



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MCC

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "MCC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  mcc              : pointer to the uint16 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_mcc
(
  uint16   profile_number,
  uint16    *mcc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mcc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_mcc, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    mcc \
  );

  return status;
} /* ds_umts_get_pdp_profile_mcc */



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MNC_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "MNC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id         :  Subscription id.
  mcc              : pointer to the uint16 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_mnc_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_3gpp_mnc_type    *mnc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_mnc, \
    profile_number, \
    subs_id, \
    mnc \
  );

  return status;
} /* ds_umts_get_pdp_profile_mnc */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MNC

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "MNC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  mcc              : pointer to the uint16 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_mnc
(
  uint16   profile_number,
  ds_umts_3gpp_mnc_type    *mnc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_mnc, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    mnc \
  );

  return status;
} /* ds_umts_get_pdp_profile_mnc */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_OPERATOR_RESERVED_PCO_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the Operator 
  reserved "PCO" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id         :  Subscription id.
  pco              : pointer to the uint16 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_operator_reserved_pco_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16    *pco
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the operator reserved pcp info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_operator_reserved_pco, \
    profile_number, \
    subs_id, \
    pco \
  );

  return status;
} /* ds_umts_get_pdp_profile_operator_reserved_pco */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_OPERATOR_RESERVED_PCO

DESCRIPTION
  This function calls the registry internal function to retrieve the Operator 
  reserved "PCO" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  pco              : pointer to the uint16 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_operator_reserved_pco
(
  uint16   profile_number,
  uint16    *pco
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the operator reserved pcp info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_operator_reserved_pco, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pco \
  );

  return status;
} /* ds_umts_get_pdp_profile_operator_reserved_pco */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MAX_PDN_CONN_PER_BLK_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  Max PDN Conn Per Blk for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id         :  Subscription id.
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
ds_umts_get_pdp_profile_max_pdn_conn_per_blk_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16    *max_pdn_conn_per_blk
)
{
#ifdef FEATURE_DATA_LTE
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_max_pdn_conn_per_blk, \
    profile_number, \
    subs_id, \
    max_pdn_conn_per_blk \
  );

  ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
#else
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /*FEATURE_DATA_LTE*/
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MAX_PDN_CONN_PER_BLK

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  Max PDN Conn Per Blk for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
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
ds_umts_get_pdp_profile_max_pdn_conn_per_blk
(
  uint16   profile_number,
  uint16    *max_pdn_conn_per_blk
)
{
#ifdef FEATURE_DATA_LTE
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_max_pdn_conn_per_blk, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    max_pdn_conn_per_blk \
  );

  ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
#else
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /*FEATURE_DATA_LTE*/
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_PDN_REQ_WAIT_TIME_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  PDN Req Wait Time for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id         :  Subscription id.
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
ds_umts_get_pdp_profile_pdn_req_wait_time_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16    *pdn_req_wait_time
)
{
#ifdef FEATURE_DATA_LTE
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_pdn_req_wait_time, \
    profile_number, \
    subs_id, \
    pdn_req_wait_time \
  );

  ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
#else
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /*FEATURE_DATA_LTE*/
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_PDN_REQ_WAIT_TIME

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  PDN Req Wait Time for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
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
ds_umts_get_pdp_profile_pdn_req_wait_time
(
  uint16   profile_number,
  uint16    *pdn_req_wait_time
)
{
#ifdef FEATURE_DATA_LTE
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_pdn_req_wait_time, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdn_req_wait_time \
  );

  ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
#else
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /*FEATURE_DATA_LTE*/
}

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MAX_PDN_CONN_TIME

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  Max PDN Conn Time for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
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
ds_umts_get_pdp_profile_max_pdn_conn_time
(
  uint16   profile_number,
  uint16    *max_pdn_conn_time
)
{
#ifdef FEATURE_DATA_LTE
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_max_pdn_conn_time, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    max_pdn_conn_time \
  );

    ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
#else
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /*FEATURE_DATA_LTE*/
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MAX_PDN_CONN_TIME_PER_SUBS

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  Max PDN Conn Time for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id         :  Subscription id.
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
ds_umts_get_pdp_profile_max_pdn_conn_time_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16    *max_pdn_conn_time
)
{
#ifdef FEATURE_DATA_LTE
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_max_pdn_conn_time, \
    profile_number, \
    subs_id, \
    max_pdn_conn_time \
  );

    ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
#else
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /*FEATURE_DATA_LTE*/
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_USER_APP_DATA_PER_SUBS
===========================================================================*/
/** 
  
  This function calls the registry internal function to get the 
  "user_app_data" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

  @profile_number            : profile number
  subs_id         :  Subscription id.
  @user_app_data             : uint32 value being passed in for the
                               user_app_data

  @return
 
  @dependencies
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_user_app_data_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32    *user_app_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_user_app_data, \
    profile_number, \
    subs_id, \
    user_app_data \
  );

  ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_USER_APP_DATA
===========================================================================*/
/** 
  
  This function calls the registry internal function to get the 
  "user_app_data" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

  @profile_number            : profile number
  @user_app_data             : uint32 value being passed in for the
                               user_app_data

  @return
 
  @dependencies
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_user_app_data
(
  uint16   profile_number,
  uint32    *user_app_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_user_app_data, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    user_app_data \
  );

  ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ROAMING_DISALLOWED_FLAG_PER_SUBS
===========================================================================*/

/**
  Calls the registry internal function to retrieve the 
  roaming_disallowed mechanism specified in the context 
  information for a profile number.This parameter is retrieved 
  only if a valid context is defined for the profile specified. 

  @param profile_number          [in]  Profile number.
  subs_id         :  Subscription id.
  @param roaming_disallowed      [out] Pointer to the 
                                       roaming_disallowed
                                       flag
   
  @return  
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_INVALID_PROFILE_NUM -- Invalid profile number. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in accessing the file. \n
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED -- Valid context is not defined for the 
                                     profile.

  @dependencies
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized. \n
  A valid context must have been defined for the profile number.
*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_roaming_disallowed_flag_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint8   *roaming_disallowed
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Get PDP profile context info IPv4 addr alloc
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_roaming_disallowed_flag, \
    profile_number, \
    subs_id, \
    roaming_disallowed \
  );

  return status;
} /* ds_umts_get_pdp_profile_roaming_disallowed_flag */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ROAMING_DISALLOWED_FLAG
===========================================================================*/

/**
  Calls the registry internal function to retrieve the 
  roaming_disallowed mechanism specified in the context 
  information for a profile number.This parameter is retrieved 
  only if a valid context is defined for the profile specified. 

  @param profile_number          [in]  Profile number.

  @param roaming_disallowed      [out] Pointer to the 
                                       roaming_disallowed
                                       flag
   
  @return  
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_INVALID_PROFILE_NUM -- Invalid profile number. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in accessing the file. \n
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED -- Valid context is not defined for the 
                                     profile.

  @dependencies
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized. \n
  A valid context must have been defined for the profile number.
*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_roaming_disallowed_flag
(
  uint16   profile_number,
  uint8   *roaming_disallowed
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Get PDP profile context info IPv4 addr alloc
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_roaming_disallowed_flag, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    roaming_disallowed \
  );

  return status;
} /* ds_umts_get_pdp_profile_roaming_disallowed_flag */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_PDN_DISCON_WAIT_TIME
===========================================================================*/

/**
  Calls the registry internal function to retrieve the 
  pdn_discon_wait_time mechanism specified in the context 
  information for a profile number.This parameter is retrieved 
  only if a valid context is defined for the profile specified. 

  @param profile_number          [in]  Profile number.
  @param pdn_discon_wait_time    [out] Pointer to the 
                                       pdn_discon_wait_time
                                       value
   
  @return  
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_INVALID_PROFILE_NUM -- Invalid profile number. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in accessing the file. \n
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED -- Valid context is not defined for the 
                                     profile.

  @dependencies
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized. \n
  A valid context must have been defined for the profile number.
*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_pdn_discon_wait_time
(
  uint16   profile_number,
  uint8   *pdn_discon_wait_time
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Get PDP profile context info IPv4 addr alloc
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_pdn_discon_wait_time, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    pdn_discon_wait_time \
  );

  return status;
} /* ds_umts_get_pdp_profile_pdn_discon_wait_time */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_PDN_DISCON_WAIT_TIME_PER_SUBS
===========================================================================*/

/**
  Calls the registry internal function to retrieve the 
  pdn_discon_wait_time mechanism specified in the context 
  information for a profile number.This parameter is retrieved 
  only if a valid context is defined for the profile specified. 

  @param profile_number          [in]  Profile number.
  subs_id         :  Subscription id.
  @param pdn_discon_wait_time    [out] Pointer to the 
                                       pdn_discon_wait_time
                                       value
   
  @return  
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_INVALID_PROFILE_NUM -- Invalid profile number. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in accessing the file. \n
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED -- Valid context is not defined for the 
                                     profile.

  @dependencies
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized. \n
  A valid context must have been defined for the profile number.
*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_pdn_discon_wait_time_per_subs
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint8   *pdn_discon_wait_time
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Get PDP profile context info IPv4 addr alloc
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_pdn_discon_wait_time, \
    profile_number, \
    subs_id, \
    pdn_discon_wait_time \
  );

  return status;
} /* ds_umts_get_pdp_profile_pdn_discon_wait_time */

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_USING_DHCP_FLAG

DESCRIPTION
  This function calls the internal registry function to set the
  "request_pcscf_address_using_dhcp_flag" flag specified for a profile. 
  The "request_pcscf_address_using_dhcp_flag" indicates if the pcscf address
  should be requested for this profile using DHCP or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_set_pdp_profile_request_pcscf_address_using_dhcp_flag
(
  uint16                profile_number,
  boolean               request_pcscf_address_using_dhcp_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_request_pcscf_address_using_dhcp_flag, \
    profile_number, \
    request_pcscf_address_using_dhcp_flag \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_REQUEST_DNS_ADDRESS_USING_DHCP_FLAG

DESCRIPTION
  This function calls the internal registry function to set the
  "request_dns_address_using_dhcp_flag" flag specified for a profile. 
  The "request_dns_address_using_dhcp_flag" indicates if the dns address
  should be requested for this profile using DHCP or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  request_dns_address_using_dhcp_flag   : pointer to store the 
                                  request_dns_address_using_dhcp_flag


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
  ds_umts_set_pdp_profile_request_dns_address_using_dhcp_flag
(
  uint16                profile_number,
  boolean               request_dns_address_using_dhcp_flag
)
{
  return DS_UMTS_PDP_FAIL;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_IM_CN_FLAG

DESCRIPTION
  This function calls the internal registry function to set the
  "im_cn" flag specified for a profile. 
  The "im_cn" indicates if the "im_cn" should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  im_cn_flag   : pointer to store the im_cn_flag


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
  ds_umts_set_pdp_profile_im_cn_flag
(
  uint16                profile_number,
  boolean               im_cn_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_im_cn_flag, \
    profile_number, \
    im_cn_flag \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_APN_DISABLE_FLAG

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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_apn_disable_flag
(
  uint16   profile_number,
  boolean  apn_disable_flag
)
{  
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_apn_disable_flag, \
    profile_number, \
    apn_disable_flag \
  );

  return status;
} /* ds_umts_set_pdp_profile_apn_disable_flag */

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_INACTIVITY_TIMER_VAL

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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_inactivity_timer_val
(
  uint16   profile_number,
  uint32   inactivity_timer_val
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_inactivity_timer_val, \
    profile_number, \
    inactivity_timer_val \
  );

  return status;
} /* ds_umts_set_pdp_profile_inactivity_timer_val */

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_APN_CLASS

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
ds_umts_set_pdp_profile_apn_class
(
  uint16   profile_number,
  uint8    apn_class
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to set the APN_CLASS info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_apn_class, \
    profile_number, \
    apn_class \
  );

  return status;
} /* ds_umts_set_pdp_profile_apn_class */


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_APN_BEARER

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
ds_umts_set_pdp_profile_apn_bearer
(
  uint16   profile_number,
  uint8    apn_bearer
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to set the APN_CLASS info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_apn_bearer, \
    profile_number, \
    apn_bearer \
  );

  return status;
} /* ds_umts_set_pdp_profile_apn_bearer */

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_MCC

DESCRIPTION
  This function calls the registry internal function to set the 
  "mcc" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  mcc              : uint16 value being passed in for the MCC

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
ds_umts_set_pdp_profile_mcc
(
  uint16   profile_number,
  uint16   mcc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to set the APN_CLASS info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_mcc, \
    profile_number, \
    mcc \
  );

  return status;
} /* ds_umts_set_pdp_profile_mcc */

/*===========================================================================
  FUNCTION DS_UMTS_SET_PDP_PROFILE_MNC

DESCRIPTION
  This function calls the registry internal function to set the 
  "mnc" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  mnc              : uint16 value being passed in for the MNC

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
ds_umts_set_pdp_profile_mnc
(
  uint16   profile_number,
  ds_umts_3gpp_mnc_type   mnc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to set the APN_CLASS info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_mnc, \
    profile_number, \
    mnc \
  );

  return status;
} /* ds_umts_set_pdp_profile_mnc */


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_OPERATOR_RESERVED_PCO 

DESCRIPTION
  This function calls the registry internal function to set the 
  operator reserved "PCO" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  pco        : uint16 value being passed in for the MNC

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
ds_umts_set_pdp_profile_operator_reserved_pco
(
  uint16   profile_number,
  uint16   pco
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to set the APN_CLASS info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_operator_reserved_pco, \
    profile_number, \
    pco \
  );

  return status;
} /* ds_umts_set_pdp_profile_operator_reserved_pco */

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_MAX_PDN_CONN_PER_BLK

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
ds_umts_set_pdp_profile_max_pdn_conn_per_blk
(
  uint16   profile_number,
  uint16   max_pdn_conn_per_blk
)
{
#ifdef FEATURE_DATA_LTE
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_max_pdn_conn_per_blk, \
    profile_number, \
    max_pdn_conn_per_blk \
  );

  ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
#else
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /*FEATURE_DATA_LTE*/
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_PDN_REQ_WAIT_TIME

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
ds_umts_set_pdp_profile_pdn_req_wait_time
(
  uint16   profile_number,
  uint16   pdn_req_wait_time
)
{
#ifdef FEATURE_DATA_LTE
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_pdn_req_wait_time, \
    profile_number, \
    pdn_req_wait_time \
  );
  ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
#else
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /*FEATURE_DATA_LTE*/
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
ds_umts_set_pdp_profile_max_pdn_conn_time
(
  uint16   profile_number,
  uint16   max_pdn_conn_time
)
{
#ifdef FEATURE_DATA_LTE
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the internal API to get the mnc info
  ------------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_max_pdn_conn_time, \
    profile_number, \
    max_pdn_conn_time \
  );
  ASSERT(status == DS_UMTS_PDP_SUCCESS);
  return status;
#else
  return DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /*FEATURE_DATA_LTE*/
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NAME_UTF16

DESCRIPTION
  This function call the internal registry function to update the name of a
  profile with the value passed.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_name_utf16
(
  uint16                    profile_number,
  const uint16              *profile_name_buf,
  byte                      profile_name_len
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_name_utf16, \
    profile_number, \
    profile_name_buf, \
    profile_name_len \
  );

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NAME

DESCRIPTION
  This function calls the internal registry function to update the name of a
  profile with the value passed.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_name
(
  uint16                    profile_number,
  const byte                *profile_name_buf,
  byte                      profile_name_len
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_name, \
    profile_number, \
    profile_name_buf, \
    profile_name_len \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_OTAP_NAPID

DESCRIPTION
  This function call the internal registry function to update the napid of
  a profile with the value passed.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_otap_napid
(
  uint16                    profile_number,
  byte                      *otap_napid_buf,
  byte                      otap_napid_len
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_otap_napid, \
    profile_number, \
    otap_napid_buf, \
    otap_napid_len \
  );

  return status;  
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_READ_ONLY_FLAG

DESCRIPTION
  This function calls the internal registry function to update the
  read_only_flag of a profile with the value passed.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_read_only_flag
(
  uint16                    profile_number,
  boolean                   read_only_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_read_only_flag, \
    profile_number, \
    read_only_flag \
  );

  return status;  
}



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_OTAP_ENABLED_FLAG

DESCRIPTION
  This function call the internal registry function to update the
  otap_enabled_flag of a profile with the value passed.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_otap_enabled_flag
(
  uint16                    profile_number,
  boolean                   otap_enabled_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_otap_enabled_flag, \
    profile_number, \
    otap_enabled_flag \
  );

  return status;  
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO

DESCRIPTION
  This function calls the internal registry function to update the context
  definition  parameters with the values passed.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_context_info
(
  uint16                    profile_number,
  const ds_umts_pdp_context_type  *pdp_context_data
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_context_info, \
    profile_number, \
    pdp_context_data \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_EX

DESCRIPTION
  This function calls the internal registry function to update the context
  definition  parameters with the values passed. This is special API called
  only from ATCOP which will always try to create persistent profile.
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
  DS_UMTS_PDP_ERR_OUT_OF_PERSISTENT_PROFILES : All persistent profile slots are taken.
                                              Can't create any more persistent profile.


SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_context_info_ex
(
  uint16                    profile_number,
  const ds_umts_pdp_context_type  *pdp_context_data
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_context_info_ex, \
    profile_number, \
    pdp_context_data \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_APN

DESCRIPTION
  This function call the internal registry function to update the APN for
  the profile specified with the value passed. The APN  can be updated ONLY
  for a valid context as it is part of the context definition.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_context_info_apn
(
  uint16                    profile_number,
  const byte                *apn_buf,
  byte                      apn_buf_len
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_context_info_apn, \
    profile_number, \
    apn_buf, \
    apn_buf_len \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_ADDRESS

DESCRIPTION
  This function call the internal registry function to update the pdp
  address for the profile specified with the value passed. The address can
  be updated ONLY for a valid context as it is part of the context definition
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_context_info_address
(
  uint16                    profile_number,
  ds_umts_pdp_addr_type     pdp_address
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_context_info_address, \
    profile_number, \
    pdp_address \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_IPV4_ADDR_ALLOC

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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_context_info_ipv4_addr_alloc
(
  uint16                              profile_number,
  ds_umts_pdp_ipv4_addr_alloc_e_type  ipv4_addr_alloc
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_ENTER_CRIT_SECTION();

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_context_info_ipv4_addr_alloc, \
    profile_number, \
    ipv4_addr_alloc \
  );

  DS_UMTS_PDP_LEAVE_CRIT_SECTION();

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_TO_DEFAULT

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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_context_info_to_default
(
  uint16                  profile_number,
  ds_ip_version_enum_type ip_vsn
)
{
  ds_umts_pdp_profile_status_etype     status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_context_info_to_default, \
    profile_number, \
    ip_vsn \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_CONTEXT_INFO

DESCRIPTION
  This function calls the internal registry function to resets the context
  information to be undefined for the profile number specified. It does not
  over-write the values of other parameter sets (qos, auth etc).
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
ds_umts_pdp_profile_status_etype  ds_umts_reset_pdp_profile_context_info
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype   status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_reset_pdp_profile_context_info, \
    profile_number \
  );

  return status;

}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO

DESCRIPTION
  This function calls the internal registry function to update the
  authentication information with the values passed.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_auth_info
(
  uint16                 profile_number,
  const ds_umts_pdp_auth_type  *pdp_auth_data
)
{
  ds_umts_pdp_profile_status_etype   status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_auth_info, \
    profile_number, \
    pdp_auth_data \
  );

  return status;

}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_TYPE

DESCRIPTION
  This function calls the internal registry function to update the
  authentication type field of authentication information with the value
  passed.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_auth_info_type
(
  uint16                     profile_number,
  ds_umts_pdp_auth_enum_type auth_type
)
{
  ds_umts_pdp_profile_status_etype   status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_auth_info_type, \
    profile_number, \
    auth_type \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_USERNAME

DESCRIPTION
  This function calls the internal registry function to update the username
  field of authentication information with the value passed.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_auth_info_username
(
  uint16                     profile_number,
  const byte                 *usernme_buf,
  byte                       username_buf_len
)
{
  ds_umts_pdp_profile_status_etype   status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_auth_info_username, \
    profile_number, \
    usernme_buf, \
    username_buf_len \
  );

  return status;

}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_PASSWORD

DESCRIPTION
  This function calls the internal registry function to update the password
  field of authentication information with the value passed.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_auth_info_password
(
  uint16  profile_number,
  const byte    *password_buf,
  byte    password_buf_len
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_auth_info_password, \
    profile_number, \
    password_buf, \
    password_buf_len \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_TO_DEFAULT

DESCRIPTION
  This function calls the internal registry function to reset the
  authentication information for the profile specified. Authentication is
  set to "none" 
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_auth_info_to_default
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_auth_info_to_default, \
    profile_number \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_UMTS_QOS_REQ_INFO

DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "requested" qos parameters with the values passed. 
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_umts_qos_req_info
(
  uint16                       profile_number,
  const ds_umts_umts_qos_params_type *pdp_umts_qos_data
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_umts_qos_req_info, \
    profile_number, \
    pdp_umts_qos_data \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_UMTS_QOS_REQ_INFO
DESCRIPTION
  This function call the internal registry function to update the UMTS
  "requested" qos parameters for the profile indicated. The parameters are
  updated to be subscribed values and the "valid_flg" is set to FALSE
  indicating that the values are not valid.
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
ds_umts_pdp_profile_status_etype  ds_umts_reset_pdp_profile_umts_qos_req_info
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_reset_pdp_profile_umts_qos_req_info, \
    profile_number \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_UMTS_QOS_MIN_INFO

DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "minimum" qos parameters with the values passed. 
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_umts_qos_min_info
(
  uint16                       profile_number,
  const ds_umts_umts_qos_params_type *pdp_umts_qos_data
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_umts_qos_min_info, \
    profile_number, \
    pdp_umts_qos_data \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_UMTS_QOS_MIN_INFO
DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "minimum" qos parameters for the profile indicated. The parameters are
  updated to be subscribed values and the "valid_flg" is set to FALSE
  indicating that the values are not valid.
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
ds_umts_pdp_profile_status_etype  ds_umts_reset_pdp_profile_umts_qos_min_info
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_reset_pdp_profile_umts_qos_min_info, \
    profile_number \
  );

  return status;

}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_SRC_STAT_DESC_INFO

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

ds_umts_pdp_profile_status_etype ds_umts_set_pdp_profile_src_stat_desc_req_info
(
  uint16                       profile_number,
  const uint32                 src_stat_desc
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_src_stat_desc_req_info, \
    profile_number, \
    src_stat_desc \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_SRC_STAT_DESC_REQ_INFO

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
ds_umts_pdp_profile_status_etype  ds_umts_reset_pdp_profile_src_stat_desc_req_info
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_reset_pdp_profile_src_stat_desc_req_info, \
    profile_number \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_SRC_STAT_DESC_MIN_INFO

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
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_src_stat_desc_min_info
(
  uint16                       profile_number,
  const uint32                 src_stat_desc
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_src_stat_desc_min_info, \
    profile_number, \
    src_stat_desc \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_UMTS_QOS_MIN_INFO

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
  sets the umts qos (minimum) information to be undefined and all parameters
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_reset_pdp_profile_src_stat_desc_min_info
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_reset_pdp_profile_src_stat_desc_min_info, \
    profile_number \
  );

  return status;

}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_GPRS_QOS_REQ_INFO

DESCRIPTION
  This function calls the internal registry function to update the GPRS
  "requested" qos parameters with the values passed. 
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_gprs_qos_req_info
(
  uint16                       profile_number,
  const ds_umts_gprs_qos_params_type *pdp_gprs_qos_req_data
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_gprs_qos_req_info, \
    profile_number, \
    pdp_gprs_qos_req_data \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_GPRS_QOS_REQ_INFO
DESCRIPTION
  This function calls the internal registry function to update the GPRS
  "requested" qos parameters for the profile indicated. The parameters are
  updated to be subscribed values and the "valid_flg" is set to FALSE
  indicating that the values are not valid.
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
ds_umts_pdp_profile_status_etype  ds_umts_reset_pdp_profile_gprs_qos_req_info
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_reset_pdp_profile_gprs_qos_req_info, \
    profile_number \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_GPRS_QOS_MIN_INFO

DESCRIPTION
  This function calls the internal registry function to update the GPRS
  "minimum" qos parameters with the values passed. 
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_gprs_qos_min_info
( 
  uint16                       profile_number,
  const ds_umts_gprs_qos_params_type *pdp_gprs_qos_min_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_gprs_qos_min_info, \
    profile_number, \
    pdp_gprs_qos_min_data \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_GPRS_QOS_MIN_INFO

DESCRIPTION
  This function calls the internal registry function to update the GPRS 
  "minimum" qos required parameters for the profile indicated. The
  parameters are updated to be subscribed values and the "valid_flg" is set
  to FALSE indicating that the values are not valid.
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
ds_umts_pdp_profile_status_etype  ds_umts_reset_pdp_profile_gprs_qos_min_info
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_reset_pdp_profile_gprs_qos_min_info, \
    profile_number \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_3GPP_SET_PDP_PROFILE_LTE_QOS_INFO

DESCRIPTION
  This function calls the internal registry function to update the LTE
  qos parameters with the values passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_lte_data    : the values to be updated with 

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
ds_umts_pdp_profile_status_etype  ds_3gpp_set_pdp_profile_lte_qos_info
( 
  uint16                             profile_number,
  const ds_3gpp_lte_qos_params_type *pdp_lte_data
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_ENTER_CRIT_SECTION();

  /*-------------------------------------------------------------------
    Call the local routine to update the QOS params.It performs a 
    check on the init flag and the profile number
  -------------------------------------------------------------------*/
  status = ds_3gpp_set_pdp_profile_lte_qos_info_internal(
                                         profile_number,
                                         pdp_lte_data);

  DS_UMTS_PDP_LEAVE_CRIT_SECTION();

  return status;
}

/*===========================================================================
FUNCTION DS_3GPP_RESET_PDP_PROFILE_LTE_QOS_INFO

DESCRIPTION
  This function calls the internal registry function to update the LTE 
  qos required parameters for the profile indicated. The parameters are 
  updated to be subscribed values and the "valid_flg" is set
  to FALSE indicating that the values are not valid.
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
ds_umts_pdp_profile_status_etype  ds_3gpp_reset_pdp_profile_lte_qos_info
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_ENTER_CRIT_SECTION();

 /*-------------------------------------------------------------------------
    Calls the internal registry function to constuct the LTE QOS info : 
    All fields set to subscribed & valid_flg set to FALSE
    writing 0x0 to the entire structure sets the above
  -------------------------------------------------------------------------*/
  status = ds_3gpp_reset_pdp_profile_lte_qos_info_internal(profile_number);

  DS_UMTS_PDP_LEAVE_CRIT_SECTION();
  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_TFT_INFO

DESCRIPTION
  This function calls the internal registry function to update the Traffic
  Flow Template (TFT) parameters with the values passed for the given filter
  identifier. 
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_tft_info
(
  uint16                      profile_number,
  byte                        filter_id,
  const  ds_umts_tft_params_type     *pdp_tft_data
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_tft_info, \
    profile_number, \
    filter_id, \
    pdp_tft_data \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_TFT_ALL_INFO

DESCRIPTION
  This function calls the internal registry function to update the Traffic
  Flow Template (TFT) parameters with the values passed. The data pointer is
  assumed to be the address of an array of size DS_UMTS_MAX_TFT_PARAM_SETS.
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_tft_all_info
(
  uint16                      profile_number,
  const ds_umts_tft_params_type     *pdp_tft_data
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_tft_all_info, \
    profile_number, \
    pdp_tft_data \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_TFT_ALL_INFO

DESCRIPTION
  This function calls the internal registry function to update the Traffice
  Flow Template (TFT) required parameters for the profile indicated. The
  parameters are updated to zero values and the "valid_flg" is set to FALSE
  indicating that the values are not valid.

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
ds_umts_pdp_profile_status_etype  ds_umts_reset_pdp_profile_tft_all_info
(
  uint16                   profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_reset_pdp_profile_tft_all_info, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_LINGER_PARAMS_INFO

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
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  
   ds_umts_set_pdp_profile_linger_params_info
(
  uint16                           profile_number,
  const ds_umts_linger_params_type *linger_params_data
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_linger_params_info, \
    profile_number, \
    linger_params_data \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_INFO

DESCRIPTION
  This function calls the internal registry function to update the dns
  addresses for the profile specified with the values passed
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_dns_address_info
(
  uint16                         profile_number,
  const ds_umts_pdp_dns_addresses_type *pdp_dns_addr
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_dns_address_info, \
    profile_number, \
    pdp_dns_addr \
  );

  return status;
}



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_PRIMARY

DESCRIPTION
  This function calls the internal registry function to set the primary dns
  address for a pdp profile 
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_dns_address_primary
(
  uint16                    profile_number,
  ds_umts_pdp_addr_type     primary_dns_addr
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_dns_address_primary, \
    profile_number, \
    primary_dns_addr \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_SECONDARY

DESCRIPTION
  This function calls the internal registry function to set the secondary
  dns address for a pdp profile. The dns parameters are updated with the
  values passed even if a valid context defintion is not present. 
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_dns_address_secondary
(
  uint16                    profile_number,
  ds_umts_pdp_addr_type     secondary_dns_addr
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_dns_address_secondary, \
    profile_number, \
    secondary_dns_addr \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_INFO_TO_DEFAULT

DESCRIPTION
  This function calls the internal registry function to set the dns addresses
  of a pdp profile to default values primary, secondary DNS addresses are set
  to 0.0.0.0
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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_dns_address_info_to_default
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_dns_address_info_to_default, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_ALL_DATA

DESCRIPTION
  This function is used to calls the internal registry function to set all
  the parameters associated with a profile. The profile_data_ptr is passed
  in. The profile_data_ptr should atleast have a valid context for this
  function to update the profile parameters.
  
  NOTE : The name of the profile is overwritten with the value passed in 
  the profile

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

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_all_data
(
  uint16                   profile_number, 
  ds_umts_pdp_profile_type *profile_data_ptr 
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_all_data, \
    profile_number, \
    profile_data_ptr \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_ALL_DATA_TO_DEFAULT

DESCRIPTION
  This function calls the internal registry function to set the profile
  information to default values for a given profile number. It defines a
  valid context for the profile (pdp-ip)or pdp-ipv6  as per ip_vsn and
  sets all other parameters to the default values
  context : no compression, ip address :0.0.0.0, APN : Null
  Auth    : no auth
  QOS     : no qos params defined ( umts and gprs)
  dns     : primary and secondary set to 0.0.0.0
  TFT     : no tft

  NOTE : The name of the profile is left as-is.

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
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_all_data_to_default
(
  uint16                  profile_number,
  ds_ip_version_enum_type ip_vsn
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_all_data_to_default, \
    profile_number, \
    ip_vsn \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_ALL_DATA_TO_DEFAULT_EX

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
  ds_umts_set_pdp_profile_all_data_to_default_ex
(
  uint16                  profile_number,
  ds_ip_version_enum_type ip_vsn,
  boolean                 transience_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_all_data_to_default_ex, \
    profile_number, \
    ip_vsn, \
    transience_flag \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_TO_UNDEFINED

DESCRIPTION
  This function calls the internal registry function to reset the profile
  specified to undefined values. 
  An undefined profile has the foll : 
  context - not defined
  auth information : none
  QOS params ( umts and gprs ) : not defined
  dns addresses : all addresses are set to 0.0.0.0
  TFT information : none

  NOTE : The name of the profile is left as-is.

PARAMETERS 
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
ds_umts_pdp_profile_status_etype  ds_umts_reset_pdp_profile_to_undefined
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_reset_pdp_profile_to_undefined, \
    ds_umts_get_default_data_subscription(), \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_TO_UNDEFINED_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to reset the profile
  specified to undefined values. 
  An undefined profile has the foll : 
  context - not defined
  auth information : none
  QOS params ( umts and gprs ) : not defined
  dns addresses : all addresses are set to 0.0.0.0
  TFT information : none

  NOTE : The name of the profile is left as-is.

PARAMETERS 
  subs_id         :  Subscription id.
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
ds_umts_pdp_profile_status_etype  ds_umts_reset_pdp_profile_to_undefined_per_subs
(
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_reset_pdp_profile_to_undefined, \
    subs_id, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_SOCK_CALLS

DESCRIPTION
  This function calls the internal registry function to set the profile
  number specified to be used for sockets calls. It writes the value
  specified to the file the socks_call_prof_num file.

PARAMETERS 
  subs_id         :  Subscription id. 
  profile_number  : profile number 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_num_for_sock_calls
(
  uint16 profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_num_for_embedded_calls, \
    ds_umts_get_default_data_subscription(), \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_SOCK_CALLS_PER_SUBS
===========================================================================*/
/** 
  Calls the internal registry function to set the profile
  number specified to be used for sockets calls.\ This function writes 
  the value specified to the socks_call_prof_num file.
 
  @param subs_id [in] Subscription ID for the profile
  @param profile_number [in] Profile number.

  @return 
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_INVALID_PROFILE_NUM -- Invalid profile number. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in updating the file.

  @dependencies 
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized.
*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_num_for_sock_calls_per_subs
(
  ds_umts_pdp_subs_e_type subs_id,
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_num_for_embedded_calls, \
    subs_id, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_RMNET_CALLS

DESCRIPTION
  This function calls the internal registry function to set the profile
  number specified to be used for RmNet data calls. It writes the value
  specified to the file the socks_call_prof_num file.

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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_num_for_rmnet_calls
(
  uint16 profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_num_for_tethered_calls, \
    ds_umts_get_default_data_subscription(), \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_RMNET_CALLS_PER_SUBS 
===========================================================================*/ 
/**
  Calls the internal registry function to set the profile
  number specified to be used for RmNet calls.\ This function writes 
  the value specified to the rmnet_call_prof_num file. 
   
  @param subs_id [in] Subscription ID for the profile
  @param profile_number [in] Profile number.

  @return 
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_INVALID_PROFILE_NUM -- Invalid profile number. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in updating the file.

  @dependencies 
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized.
*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_num_for_rmnet_calls_per_subs
(
  ds_umts_pdp_subs_e_type subs_id,
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_num_for_tethered_calls, \
    subs_id, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_SOCK_CALLS

DESCRIPTION
  This function calls the internal registry function to set the profile
  number specified to be used for embedded calls. It writes the value
  specified to the file the embedded_call_prof_num file.

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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_num_for_embedded_calls
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_num_for_embedded_calls, \
    ds_umts_get_default_data_subscription(), \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_EMBEDDED_CALLS_PER_SUBS 
===========================================================================*/ 
/**
  Calls the internal registry function to set the profile number specified
  to be used for embedded calls.\ This function writes the value specified
  to the "embedded_call_prof_num" file. 
   
  @param subs_id [in] Subscription ID for the profile
  @param profile_number [in] Profile number.

  @return 
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_INVALID_PROFILE_NUM -- Invalid profile number. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in updating the file.

  @dependencies 
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized.
*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_num_for_embedded_calls_per_subs
(
  ds_umts_pdp_subs_e_type subs_id,
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_num_for_embedded_calls, \
    subs_id, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_TETHERED_CALLS

DESCRIPTION
  This function calls the internal registry function to set the profile
  number specified to be used for tethered data calls. It writes the value
  specified to the file the tethered_call_prof_num file.

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
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_num_for_tethered_calls
(
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_num_for_tethered_calls, \
    ds_umts_get_default_data_subscription(), \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_TETHERED_CALLS_PER_SUBS 
===========================================================================*/ 
/**
  Calls the internal registry function to set the profile number specified
  to be used for tethered calls.\ This function writes the value specified
  to the "tethered_call_prof_num" file.
   
  @param subs_id [in] Subscription ID for the profile 
  @param profile_number [in] Profile number.

  @return 
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_INVALID_PROFILE_NUM -- Invalid profile number. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in updating the file.

  @dependencies 
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized.
*/
ds_umts_pdp_profile_status_etype  ds_umts_set_pdp_profile_num_for_tethered_calls_per_subs
(
  ds_umts_pdp_subs_e_type subs_id,
  uint16                  profile_number
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_num_for_tethered_calls, \
    subs_id, \
    profile_number \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ACCESS_CTRL_FLAG

DESCRIPTION
  This function calls the registry internal function to retrieve the
  "access_ctrl_flag" specified for a PDP context. 

  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  read_only_flag  : pointer to store the read_only_flag

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
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_access_ctrl_flag
(
  uint16                          profile_number,
  dsat_pdp_access_control_e_type *access_ctrl_flag
)
{
  ds_umts_pdp_profile_status_etype status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_access_ctrl_flag, \
    profile_number, \
    access_ctrl_flag \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_DEFAULT_PROFILE_NUMBER_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to set the default profile
  number for a Subscription. For dual standby there can be two active 
  subscriptions and the default profile number should be specified for both 
  subscriptions.

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
  DS_UMTS_PDP_FAIL                 : Failed to finish operation

SIDE EFFECTS
  none
===========================================================================*/
/*ARGSUSED*/
ds_umts_pdp_profile_status_etype  ds_umts_set_default_profile_number_per_subs
(
  ds_umts_profile_family_e_type        family,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                               profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  /*------------------------------------------------------------------------
    Set the profile number specified to be used for embedded/tethered calls
  -------------------------------------------------------------------------*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_default_profile_number, \
    family, \
    subs_id, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_GET_DEFAULT_PROFILE_NUMBER_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to get the default profile
  number for a given Subscription. 
 
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
  DS_UMTS_PDP_FAIL                 : Failed to finish operation

SIDE EFFECTS
  none
===========================================================================*/
/*ARGSUSED*/
ds_umts_pdp_profile_status_etype  ds_umts_get_default_profile_number_per_subs
(
  ds_umts_profile_family_e_type        family,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                              *profile_number
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;

  /*-------------------------------------------------------------------
    Get the profile number specified to be used for embedded/
    tethered calls.
  -------------------------------------------------------------------*/
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_default_profile_number, \
    family, \
    subs_id, \
    profile_number \
  );

  return status;
}

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_EMERGENCY_CALLS_ARE_SUPPORTED_FLAG

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
  ds_umts_set_pdp_profile_emergency_calls_are_supported_flag
(
  uint16                profile_number,
  boolean               emergency_calls_are_supported
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_EMERGENCY_PDN
  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_set_pdp_profile_emergency_calls_are_supported_flag, \
    profile_number, \
    emergency_calls_are_supported \
  );
#else
  status = DS_UMTS_PDP_FEATURE_NOT_SUPPORTED;
#endif /* FEATURE_EMERGENCY_PDN*/  
  return status;
}

#if 0
/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IPV6_DELEGATION_FLAG_PER_SUBS

DESCRIPTION
  This function calls the internal registry function to set the ipv6
  delegation for a pdp profile. The dns parameters are updated with the
  values passed even if a valid context defintion is not present. 
  It also updates the cache table with the secondary dns address value passed.

PARAMETERS 
  profile_number  : profile number
  subs             : Denotes one of two active subscriptions
  ipv6_delegation_flag: dns address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_ipv6_prefix_delegation_per_subs
(
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean                   *ipv6_delegation_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_ipv6_prefix_delegation, \
    profile_number, \
    subs_id, \
    ipv6_delegation_flag \
  );

  return status;
}


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IPV6_DELEGATION_FLAG

DESCRIPTION
  This function calls the internal registry function to set the ipv6
  delegation for a pdp profile. The dns parameters are updated with the
  values passed even if a valid context defintion is not present. 
  It also updates the cache table with the secondary dns address value passed.

PARAMETERS 
  profile_number  : profile number
  ipv6_delegation_flag: dns address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_ipv6_prefix_delegation
(
  uint16                    profile_number,
  boolean                   *ipv6_delegation_flag
)
{
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UMTS_PDP_API_INTERNAL \
  ( \
    ds_umts_get_pdp_profile_ipv6_prefix_delegation, \
    profile_number, \
    ds_umts_get_default_data_subscription(), \
    ipv6_delegation_flag \
  );

  return status;
}

#endif
