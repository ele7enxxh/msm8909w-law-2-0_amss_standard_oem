#ifndef TM_LPP_COMMON_UTILS_H
#define TM_LPP_COMMON_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM LPP Common Utility Module

GENERAL DESCRIPTION
  This file contains TM's LPP Common Utility Functions

  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_common/inc/tm_lpp_common_utils.h#1 $
 
  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  08/05/15   rh      Moved carrier-specific NV 1920 (pos mode support) to TM core 
  01/14/15   rk      LPP >24 AD/Meas feature support.
  05/14/13   rk      Promoted Glonass conversion function to this LPP common module.
  02/1/12    atien   Initial version

===========================================================================*/
#include "customer.h"   /* Customer configuration file */
/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "comdef.h"    /* Definition for basic types and macros */
#include "msg.h"
#include "math.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "sm_nv.h"
#include "sm_nv_efs.h"

#include "tm_data.h"
#include "tm_lpp_asn1.h"
#include "tm_umts_up_supl.h"

typedef struct {
  uint8   mt_lr_support;                /* used by Capability: LCS value_added_lcs_supported: 0: not supported */

  /* NOTE - LPP CP enable/disable NV item is maintained in TM CORE via tm_core_get_lpp_cp_enable() */

  uint32  aagps_default_qos_time;
  uint32  aagps_default_qos_unc;
  uint32  aagps_default_ref_time_unc_50ms; 
  uint8   aagps_default_lpp_cp_timeout_secs;

} lpp_cp_nv_items_s_type;

extern lpp_cp_nv_items_s_type z_lpp_cp_nv_item_ram_copy;

/*
 ******************************************************************************
 *
 * tm_lpp_common_utils_read_nv
 *
 * Function description:
 *  This function reads NV items that are common to LPP into a structure.
 *
 * Parameters:
 *   None.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/
extern void tm_lpp_common_utils_read_nv (void);

/*
 ******************************************************************************
 *
 * tm_lpp_common_utils_refresh_carrier_specific_nv
 *
 * Function description:
 *    This function refresh carrier-specific NVs after SIM hot swap
 *
 * Parameters:
 *   None.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/
extern void tm_lpp_common_utils_refresh_carrier_specific_nv (void);

/*
 ******************************************************************************
 *
 * tm_lpp_common_glo_eph_svid_freq_num_map_aux 
 *
 * Function description:
 *  Associates sv ID with a frequencey number for Glonass eph. data using 
 *  information from auxilary assistance.                   
 *  
 * Parameters:
 *   p_lpp_glo_aux_info : pointer to the structure holding Glonass aux. data.
 *   p_lpp_glo_eph_freq : points to glonass eph. array struct.
 * 
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/

void tm_lpp_common_glo_eph_svid_freq_num_map_aux (const GNSS_AuxiliaryInformation* const p_gnss_aux_info, prot_glo_eph_buffer_s_type p_lpp_glo_eph_freq[N_GLO_SV]);

/*
 ******************************************************************************
 *
 * tm_lpp_common_glo_acqast_svid_freq_num_map_aux 
 *
 * Function description:
 *  Associates sv ID with a frequencey number for Glonass Aquisition Assistance data using 
 *  information from auxilary assistance.                   
 *  
 * Parameters:
 *   p_lpp_glo_aux_info : pointer to the structure holding Glonass aux. data.
 *   p_lpp_glo_aa_freq : pointer to LPP Glonass aquisition assistance & auxi info struct.
 * 
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/

void tm_lpp_common_glo_acqast_svid_freq_num_map_aux (const GNSS_AuxiliaryInformation* const p_gnss_aux_info,
                                                     prot_glo_acqast_buffer_s_type* p_lpp_glo_aa_freq);

/*
 ******************************************************************************
 *
 * tm_lpp_common_glo_eph_svid_freq_num_map_alm 
 *
 * Function description:
 *  Associates sv ID with a frequencey number for Glonass eph. data using 
 *  information from Almanac model for Glonass.                   
 *  
 * Parameters:
 *   p_keplerianGLONASS : pointer to the structure holding Almanac model for Glonass.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/

void tm_lpp_common_glo_eph_svid_freq_num_map_alm (const AlmanacGLONASS_AlmanacSet* const p_keplerianGLONASS, prot_glo_eph_buffer_s_type p_lpp_glo_eph_freq[N_GLO_SV]);

/*
 ******************************************************************************
 *
 * tm_lpp_common_convert_to_glonass_time
 *
 * Function description:
 *  This function processes GPS offset time (deltaT) to compute the Glonass time
 *  & set the appropiate Glonass time fields in set_assist_pos_time_stamp struct.
 *  The deltaT could be from GPS UTCModel AD and/or GLONASS TimeModel AD.
 *
 * Parameters:
 *  bDeltaT [IN]: Positive GPS time offset value
 *  p_supl_session_postime [OUT]: Pointer to position time structure.
 * 
 * Return value:
 *  On success TRUE, else FALSE is return to indicate the failure.
 *
 *
 ******************************************************************************
*/
boolean tm_lpp_common_convert_to_glonass_time(const int8 bDeltaT, gnss_time_info *p_supl_session_postime);

/*
 ******************************************************************************
 *
 * tm_lpp_util_add_otdoa_data_from_list
 *
 * FUNCTION DESCRIPTION:
 *  Remove the OTDOA/RSTD measurements from the structure that's already processed.
 *
 * PARAMETERS:
 *  pz_otdoa_data_in_list [IN/OUT]: allocate memory and add the OTDOA data to the list.
 * 
 * RETURN VALUE:
 *  On success return pointer to newly added OTDOA data, else return NULL.
 *
 *
 ******************************************************************************
*/
sm_loc_lte_otdoa_meas_info_type* tm_lpp_util_add_otdoa_data_to_list( tm_otdoa_data_store *pz_otdoa_data_store );


/*
 ******************************************************************************
 *
 * tm_lpp_util_remove_otdoa_data_from_list
 *
 * FUNCTION DESCRIPTION:
 *  Remove the OTDOA/RSTD measurements based on the last processed index.
 *
 * PARAMETERS: remove and free the data from the list.
 * 
 * RETURN VALUE:
 *  None.
 *
 *
 ******************************************************************************
*/
void tm_lpp_util_remove_otdoa_data_from_list( tm_otdoa_data_store *pz_otdoa_data_store );

/*
 ******************************************************************************
 *
 * tm_lpp_util_remove_all_otdoa_data_from_list
 *
 * FUNCTION DESCRIPTION:
 *  Remove all the OTDOA/RSTD measurements from the structure & initialize the index fields.
 *
 * PARAMETERS:
 *  pz_otdoa_data_store [IN/OUT]: Pointer to the OTDOA data store
 * 
 * RETURN VALUE:
 *  None.
 *
 *
 ******************************************************************************
*/
void tm_lpp_util_remove_all_otdoa_data_from_list(tm_otdoa_data_store *pz_otdoa_data_store); 

#ifdef __cplusplus
}
#endif

#endif /* TM_LPP_COMMON_UTILS_H */

