#ifndef AOSTLM_AUDIOMM_WRAPPER_H
#define AOSTLM_AUDIOMM_WRAPPER_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    A O S T L M   S U B S Y S T E M

GENERAL DESCRIPTION
The AOSTLM AudioMM Wrappper handles AudioMM licenses which use metadata 
to indicate which modules are enabled and which are not. 


EXTERNALIZED FUNCTIONS
aostlm_audiomm_init_license
aostlm_audiomm_request_access
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/aostlm/core/inc/aostlm_audiomm_wrapper.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/2014  djc     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "aostlm_api.h"

/*=====================================================================
                              Typedefs
======================================================================*/
/* Feature Id Type */
typedef aostlm_feature_id_t aostlm_license_id_t;
/* Module Id Type */
typedef uint16 aostlm_module_id_t;

/* initialization response type */
typedef enum
{
    AOSTLM_AUDIO_SUCCESS = 0,
    AOSTLM_AUDIO_NO_LICENSE,
    AOSTLM_AUDIO_METADATA_FAIL
} aostlm_init_result_t;

/*=====================================================================
                              Functions
======================================================================*/

/**
 * @brief Initializes the wrapper for a particular license id.  MUST be
 *        called before a module requests access.
 * 
 * @details Operation requests access for the license, and if granted,
 *          fetches the appropriate metadata.  Metadata is then parsed
 *          and used to determine which modules are enabled/disabled
 * 
 * @param [in] license_id: The identifier of the license to initialize
 *         
 * @return AOSTLM_AUDIO_SUCCESS if the operation succeeded
 *         AOSTLM_AUDIO_NO_LICENSE if access was denied
 *         AOSTLM_AUDIO_METADATA_FAIL if metadata failed to parse
 *
 *
 */
aostlm_init_result_t aostlm_audiomm_init_license(const aostlm_license_id_t license_id);

/**
 * @brief allows a module to request access to operate.  
 *
 * @param [in] license_id: The identifier of the license the 
 *      module is managed by
 * 
 * @param [in] module_id:  Identifier of the module requesting 
 *        access
 *  
 * @return  AOSTLM_GRANT_COMM if the module is granted 
 *          commercial access
 *  
 *          AOSTLM_GRANT_EVAL if the module is granted
 *          evaluation license 
 *  
 *          AOSTLM_DENY if access to the module is denied
 * 
 */
aostlm_access_t aostlm_audiomm_request_access(const aostlm_license_id_t license_id,
                                              const aostlm_module_id_t  module_id);

/**
 * @brief Prints out the audiomm wrapper information.  
 *
 */
void aostlm_audiomm_dump_licenses(void);

#endif /* AOSTLM_AUDIOMM_WRAPPER_H */
