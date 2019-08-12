
#ifndef SLEEP_CONFIG_H
#define SLEEP_CONFIG_H

/*=============================================================================

  FILE:         sleep_config.h
  
  OVERVIEW:     This file provides methods for reading sleep configuration
                data.
 
                Copyright (c) 2010 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/config/sleep_config.h#1 $

=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/* ==================================================================
              EXTERNAL FUNCTION DECLARATIONS
   ================================================================== */              

/**
 * sleep_config_read_bool
 *
 * @brief Reads the value associated with the parameter and LPRM 
 *        passed in, for the specified core.  This function will
 *        return the value as a boolean.
 *  
 * @param core_num:  Core for which to read the config data.
 * @param lpr_name:  LPR name to read the data for.
 * @param lprm_name: LPRM name to read the data for.
 * @param param:     Parameter to read for the specified 
 *                   LPR/LPRM pair.
 * @param result:    Output buffer to write the boolean value 
 *                   read from the config data.
 * 
 * @return FALSE if an error occurred or either the LPR/LPRM 
 *         pair or parameter were not found. TRUE if the
 *         parameter was found, and the value was returned in
 *         the buffer.
 */ 
unsigned int sleep_config_read_bool( unsigned int core_num, char *lpr_name, 
                                     char *lprm_name, char *param, 
                                     unsigned char *result );

#ifdef __cplusplus
}
#endif

#endif /* !SLEEP_CONFIG_H */

