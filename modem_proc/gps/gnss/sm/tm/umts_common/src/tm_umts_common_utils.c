/*=============================================================================

                        Transport Manager API Header File

GENERAL DESCRIPTION
  This file contains contains the structure definitions and function prototypes for 
  the Transport Manager software interface.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_common/src/tm_umts_common_utils.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  08/26/15   gk      90% conf reporting changes
  08/05/15   rh      Moved carrier-specific NV 1920 (pos mode support) to TM core 
  06/09/15   mj      Add support for W+W feature
  02/14/11   gk      Velocity rework
  09/11/14   mj     Modify tm_umts_common_utils_rr_session_status_notify to handle asid
  10/29/13   mj     Update WCDMA/TDSCDMA API calls for segment loading
  09/30/13   rk      Removed reference to deprecated NV item 3649 & use RRC API to get
                     the selected RRC protocol release major version.
  01/03/11   gk      Eph throttling rework
  07/13/09   gk      server address URL is not processed correctly
  08/22/08   jlp     Added support of emergency call simulation for simulating E911 A-GPS
  03/14/07    lt     Added gs.h as GERAN is to remove it from rr_lsm_if.h.
  07/17/07    lt     Moved NV item reading of refTime and refLoc from TmCore to here.
  06/20/07    lt     Added handling of NV Item aagps_gps_lock_control.
  05/25/07    lt     Added more utility functions.
  05/01/07    lt     Initial version.

=============================================================================*/
#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */
#ifdef FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "comdef.h"    /* Definition for basic types and macros */
#include "msg.h"
#include "math.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "tm_prtl_iface.h"
#include "sm_nv.h"
#include "tm_cm_iface.h"
#include "rr_lsm_if.h"
#include "gs.h"
#include "rrccmd.h"
#include "gnss_wwan_iface.h"

#define TM_UMTS_COMMON_VARIABLES_DEFINED
#include "tm_umts_common_utils.h"
#undef TM_UMTS_COMMON_VARIABLES_DEFINED  

#ifdef FEATURE_CGPS_UMTS_UP_SUPL
#include "tm_umts_up_supl_api.h"
#include "tm_umts_up_supl.h"
#endif
#ifdef FEATURE_CGPS_UMTS_CP
#include "ms.h"
#endif

/* Table for 2 ^ x, where x is from 0 to 7 inclusive */
static const uint8 u_pow_2[ 8 ] =
{
  1, 2, 4, 8, 16, 32, 64, 128
};


/* used by the ni_client_set_ni_param function */
pdsm_pd_option_s_type   z_ni_client_pdsm_pd_option;
pdsm_pd_qos_type        z_ni_client_pdsm_pd_qos;


/*
 ******************************************************************************
 *
 * cell_info_unchanged_for_refloc
 *
 * Function description:
 *  This function checks to see if the phone is in the same cell as the one
 *  from which the last reference position was given.
 *
 * Parameters:
 *   None.
 *
 * Return value: 
 *   TRUE: yes it is in the same cell.
 *   FALSE: no it is not in the same cell.
 *
 ******************************************************************************
*/

static boolean cell_info_unchanged_for_refloc_ck(void)
{
  
  boolean ret_val = FALSE;

  if ( z_last_refloc_info.z_cell_info_refloc.u_CachedCellIDValid &&
       ( z_last_refloc_info.z_cell_info_refloc.q_NumCellChangesCachedCellID == 1) &&
       ( z_last_refloc_info.z_cell_info_refloc.e_ActiveRAT == 
         z_cell_info_cached.e_ActiveRAT 
       ) &&
       z_cell_info_cached.u_CachedCellIDValid &&
       ( z_cell_info_cached.q_NumCellChangesCachedCellID == 1 ) 
     )
  {
    #if defined(FEATURE_CGPS_UMTS_CELLDB) || defined (FEATURE_FSM_V2)
    if ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA )
    {
      if ( ( z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id ==
             z_last_refloc_info.z_cell_info_refloc.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id ) &&
           ( z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.num_mnc_digits ==
             z_last_refloc_info.z_cell_info_refloc.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.num_mnc_digits
           ) &&

          ( memcmp ( &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mcc[0],
                     &z_last_refloc_info.z_cell_info_refloc.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mcc[0],
                     RRC_MAX_MCC_DIGITS
                   ) == 0 
          ) &&
          ( memcmp ( &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc[0],
                     &z_last_refloc_info.z_cell_info_refloc.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc[0],
                     z_last_refloc_info.z_cell_info_refloc.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.num_mnc_digits
                   ) == 0
          )
         )
      {
        ret_val = TRUE;
      } /* cell id match */
    } /* if WCDMA */

    else if ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_GSM )
    {
      if ( memcmp( &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity,
                   &z_last_refloc_info.z_cell_info_refloc.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity,
                   3
                 ) == 0
         )
      {
        if ( ( z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.cell_identity == 
               z_last_refloc_info.z_cell_info_refloc.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.cell_identity 
             ) &&
             ( z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.location_area_code ==
               z_last_refloc_info.z_cell_info_refloc.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.location_area_code
             )
           )
        {
          ret_val = TRUE;
        }
      }
    } /* if GSM */         
    #endif /* defined(FEATURE_CGPS_UMTS_CELLDB) || defined (FEATURE_FSM_V2) */
  } /* if both refloc_cell_id and current_cell_id valid */

  return ret_val;

}

/*
 ******************************************************************************
 *
 * tm_umts_common_utils_read_carrier_specific_nv
 *
 * Function description:
 *  This function reads UMTS carrier-specific NVs
 *
 * Parameters:
 *   None.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/
void tm_umts_common_utils_read_carrier_specific_nv (void)
{
#ifdef FEATURE_CGPS_UMTS_UP_SUPL
  uint8 nv_cgps_umts_pde_server_addr_url[C_MAX_URL_SIZE_BYTE];
  char port_string[C_MAX_URL_SIZE_BYTE];
  uint16 i,j;

  /* NV #04703 UMTS PDE Server Address URL */
  memset( &nv_cgps_umts_pde_server_addr_url[0], 0, C_MAX_URL_SIZE_BYTE );
  (void)sm_nv_read( NV_CGPS_UMTS_PDE_SERVER_ADDR_URL_I, (void *)&z_tm_umts_nv_read_buffer);

  i=0;
  while ( ( z_tm_umts_nv_read_buffer.cgps_umts_pde_server_addr_url[i] != 0 ) &&  
          ( i < ( C_MAX_URL_SIZE_BYTE -1 ) )
        )
  {
    nv_cgps_umts_pde_server_addr_url[i] = z_tm_umts_nv_read_buffer.cgps_umts_pde_server_addr_url[i];
    i++;
  }   
  /* retrieve SLP address from the NV item of aagps_default_url */
  if ( ((nv_cgps_umts_pde_server_addr_url[0]=='h') || (nv_cgps_umts_pde_server_addr_url[0]=='H')) &&
       ((nv_cgps_umts_pde_server_addr_url[1]=='t') || (nv_cgps_umts_pde_server_addr_url[1]=='T')) &&
       ((nv_cgps_umts_pde_server_addr_url[2]=='t') || (nv_cgps_umts_pde_server_addr_url[2]=='T')) &&
       ((nv_cgps_umts_pde_server_addr_url[3]=='p') || (nv_cgps_umts_pde_server_addr_url[3]=='P')) &&
        (nv_cgps_umts_pde_server_addr_url[4]==':') && (nv_cgps_umts_pde_server_addr_url[5]=='/')  && (nv_cgps_umts_pde_server_addr_url[6]=='/') )
  {
    j = 7;  /* Start extracting hslp_address after the URL prefix. */
  }
  else  
  {
    j = 0;  /* No http[colon][slash][slash] in nv_cgps_umts_pde_server_addr_url. */
  }

  i = j;
  while ( (nv_cgps_umts_pde_server_addr_url[i] != ':') && (nv_cgps_umts_pde_server_addr_url[i] != '\0') && ( i < ( C_MAX_URL_SIZE_BYTE - 1 ) ) )
  {
    z_umts_nv_item_ram_copy.slp_address[i-j] = nv_cgps_umts_pde_server_addr_url[i] ;
    i++;
  }
  z_umts_nv_item_ram_copy.slp_address[i-j] = '\0' ;  /* null terminated FQDN */


  i++;
  if ( i >= 128 ) /* logically, it couldn't be greater than 128 */
  {
    /* no port number in nv_item aagps_defaul_url, use OMA default */ 
    z_umts_nv_item_ram_copy.slp_port_number = 7275; 
  }
  else
  {
    j = i;

    while ( (nv_cgps_umts_pde_server_addr_url[i] != '\0') && ( i < ( C_MAX_URL_SIZE_BYTE - 1 ) ) )
    {
      if ( i >= j )
      {
        port_string[i-j] = nv_cgps_umts_pde_server_addr_url[i];
      }
      i++;
    }

    port_string[i-j] = '\0';  /* null terminated string */

    z_umts_nv_item_ram_copy.slp_port_number = (uint16) atoi(port_string); /* no range check if port_string > "65535" */
    if ( z_umts_nv_item_ram_copy.slp_port_number == 0 )
    {
      /* no valid port number, use OMA default */
      z_umts_nv_item_ram_copy.slp_port_number = 7275;
    }
  }

  /* Fetch the HSLP IP Address */
  (void)sm_nv_read( NV_CGPS_UMTS_PDE_SERVER_ADDR_IPV4_I, (void *)&z_tm_umts_nv_read_buffer );
  z_umts_nv_item_ram_copy.slp_ip_addr = z_tm_umts_nv_read_buffer.cgps_umts_pde_server_addr_ipv4;

  /* Fetch the HSLP IPv6 Address */
  (void)sm_nv_read( NV_CGPS_UMTS_PDE_SERVER_ADDR_IPV6_I, (void *)&z_tm_umts_nv_read_buffer );
  memscpy((void*)&(z_umts_nv_item_ram_copy.slp_ipv6_addr[0]), sizeof(z_umts_nv_item_ram_copy.slp_ipv6_addr),
          (void *)&(z_tm_umts_nv_read_buffer.cgps_umts_pde_server_addr_ipv6[0]), 
          sizeof(z_tm_umts_nv_read_buffer.cgps_umts_pde_server_addr_ipv6));

  /* NV #3758 AGPS Transport Security ID  */
  (void)sm_nv_read( NV_AAGPS_USE_TRANSPORT_SECURITY_I, (void *)&z_tm_umts_nv_read_buffer );
  z_umts_nv_item_ram_copy.aagps_use_transport_security = z_tm_umts_nv_read_buffer.aagps_use_transport_security;

  /* NV #6792  SUPL Version: Bytes from MSB to LSB: rsvd, major, minor, serviceIndicator  */
  (void)sm_nv_read( NV_GNSS_SUPL_VERSION_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.gnss_supl_version = z_tm_umts_nv_read_buffer.gnss_supl_version;

#endif /* FEATURE_CGPS_UMTS_UP_SUPL */

#ifdef FEATURE_CGPS_UMTS_CP
  /* NV #1920  AAGPS Positioning Modes Supported, moved to TM core */
#endif /* umts-cp */

}


/*
 ******************************************************************************
 *
 * tm_utils_common_utils_read_nv
 *
 * Function description:
 *  This function reads NV items that are common to UMTS into a structure.
 *
 * Parameters:
 *   None.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/

void tm_umts_common_utils_read_nv (void)
{
  (void)sm_nv_read( NV_AAGPS_DEFAULT_REF_TIME_UNC_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms = z_tm_umts_nv_read_buffer.aagps_default_ref_time_unc;

  (void)sm_nv_read(NV_AAGPS_DEFAULT_REF_POSITION_UNC_I,(void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.aagps_default_ref_position_unc = (uint32)z_tm_umts_nv_read_buffer.aagps_default_ref_position_unc;

  (void)sm_nv_read(NV_AAGPS_DEVELOPMENT_TEST_CONTROL5_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.aagps_development_test_control5 = (uint32)z_tm_umts_nv_read_buffer.aagps_development_test_control5;

  (void)sm_nv_read( NV_AAGPS_DEFAULT_QOS_TIME_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.aagps_default_qos_time = z_tm_umts_nv_read_buffer.aagps_default_qos_time;

  (void)sm_nv_read( NV_AAGPS_DEFAULT_QOS_UNC_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.aagps_default_qos_unc = z_tm_umts_nv_read_buffer.aagps_default_qos_unc;


  /**** UMTS CP related *****/
#ifdef FEATURE_CGPS_UMTS_CP
  /* gps_lock is defined under FEATURE_CGPS_UMTS_CP; so moved it appropriately from
     previous section */
  (void)sm_nv_read(NV_GPS1_LOCK_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.gps_lock = (pdsm_gps_lock_e_type)z_tm_umts_nv_read_buffer.gps1_lock;

  (void)sm_nv_read(NV_AAGPS_DEFAULT_SMLC_COMM_TIMEOUT_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.default_smlc_comm_timeout_sec = z_tm_umts_nv_read_buffer.aagps_default_smlc_comm_timeout;

  (void)sm_nv_read(NV_AAGPS_DEFAULT_ALLOW_RRC_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.default_allow_rrc = z_tm_umts_nv_read_buffer.aagps_default_allow_rrc;

  (void)sm_nv_read(NV_AAGPS_DEFAULT_MTLR_GUARD_TIMER_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.default_mtlr_guard_timer_sec = z_tm_umts_nv_read_buffer.aagps_default_mtlr_guard_timer;

  (void)sm_nv_read(NV_AAGPS_MT_LRSUPPORT_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.mt_lr_support = z_tm_umts_nv_read_buffer.aagps_mt_lrsupport;

  (void)sm_nv_read(NV_AAGPS_3G_MO_LRSUPPORT_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.mo_lr_support = z_tm_umts_nv_read_buffer.aagps_3g_mo_lrsupport;

  (void)sm_nv_read(NV_AAGPS_2G_MO_LRSUPPORT_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.mo_lr_2g_support = z_tm_umts_nv_read_buffer.aagps_2g_mo_lrsupport;

  /* NV_WCDMA_RRC_VERSION_I - NV 3649 deprecated. Use RRC API rrc_return_wcdma_rrc_version() instead. */
  /* Todo : Clean-up z_tm_umts_nv_read_buffer.wcdma_rrc_version variable */

#endif /* umts-cp */

  /* read carrier-specific UMTS CP/UP NVs */
  tm_umts_common_utils_read_carrier_specific_nv();

  /****** UMTS UP ( SUPL ) related ****/
#ifdef FEATURE_CGPS_UMTS_UP_SUPL
  (void)sm_nv_read( NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER1_VALUE_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.aagps_ut1_value = z_tm_umts_nv_read_buffer.aagps_default_presupl_ue_timer1_value;

  (void)sm_nv_read( NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER2_VALUE_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.aagps_ut2_value = z_tm_umts_nv_read_buffer.aagps_default_presupl_ue_timer2_value;

  (void)sm_nv_read( NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER3_VALUE_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.aagps_ut3_value = z_tm_umts_nv_read_buffer.aagps_default_presupl_ue_timer3_value;

  (void)sm_nv_read( NV_AAGPS_EMERGENCY_SERVICES_SPPRT_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.aagps_emergency_services_spprt = z_tm_umts_nv_read_buffer.aagps_emergency_services_spprt;

  /* RRLP/RRC Version support info. - OBSOLETE. Instead use GNSS_NV_EFS_SM_TM_ASSISTED_GLO_PROTOCOL_SELECT. */
  (void)sm_nv_read( NV_GNSS_RRLP8_RRC8_SUPPORTED_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.gnss_rrlp8_rrc8_supported = 
    z_tm_umts_nv_read_buffer.gnss_rrlp8_rrc8_supported;
    
  /* Glonass support */
  (void)sm_nv_read( NV_GNSS_GLO_CONTROL_I, (void *)&z_tm_umts_nv_read_buffer);
  z_umts_nv_item_ram_copy.gnss_glo_control = z_tm_umts_nv_read_buffer.gnss_glo_control; 
  
#endif   /* umts-up-supl */

}


/*
 ******************************************************************************
 *
 * tm_umts_common_utils_refresh_carrier_specific_nv
 *
 * Function description:
 *  This function refresh carrier-specific NVs after SIM hot swap
 *  sm_nv_read() will always read from HW. sm_nv_read_ex() may read from cache
 *  if available.
 * 
 * Parameters:
 *   None.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/
void tm_umts_common_utils_refresh_carrier_specific_nv (void)
{
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* read carrier-specific UMTS CP/UP NVs */
  tm_umts_common_utils_read_carrier_specific_nv();
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
}

/*
 ******************************************************************************
 *
 * tm_umts_common_utils_meter_to_GADK
 *
 * Function description:
 * This is obtained from the GAD spec V3.2.0
 *
 * The uncertainty r, expressed in meters, is mapped to a number K, 
 * with the following formula:
 *     r = C [ (1 + x)^K - 1 ]
 *     with:
 *          1- Horizontal: C = 10 and x = 0.1
 *          2- Vertical:   C = 45 and x = 0.025
 *
 * The K value must be in the range 0 <= K <= 127.  If the uncertainty
 * does not exactly match a K mapping value, then we bump up to the
 * next higher K value.  This ensures that the reported UNC is not 
 * smaller than the actual UNC.  We keep K >= 1, as K=0 means 0 meters
 * of uncertainty, which is not reasonable for computed positions from 
 * measurements.
 *
 * This code/algorithm has been verified separately with a separate program.
 * The program took all values of K from 0 to 127.  The corresponding range
 * value from the mapping table was used as an input to test the code. 
 * For each value of K, the range value, the range value plus a small number, 
 * and the range value minus a small number were all input into the function
 * and the output K value was compared to what was desired.
 * 
 * 
 * Parameters:
 *
 *   f_Unc - Uncertainty value 
 *   u_Is_Horiz - If True, compute horizontal uncertainty. Else, compute vertical.
 *
 * Return value: 
 *
 *   K value, which has a range 0 <= K <= 127;
 *
 ******************************************************************************
*/

uint8 tm_umts_common_utils_meter_to_GADK( boolean u_Is_Horiz, uint32 q_hor_acc_meters )
{
  return cgps_utils_meter_to_GADK(u_Is_Horiz, q_hor_acc_meters);
} 


/*
 ******************************************************************************
 *
 * tm_umts_common_utils_GADK_to_meter
 *
 * Function description:
 * This is obtained from the GAD spec V3.2.0
 *
 * The uncertainty r, expressed in meters, is mapped to a number K, 
 * with the following formula:
 *
 *     r = C [ (1 + x)^K - 1 ]
 *     with:
 *          1- Horizontal: C = 10 and x = 0.1
 *          2- Vertical:   C = 45 and x = 0.025
 *
 * The K value must be in the range 0 <= K <= 127.  
 *
 * This function simply returns the r value given a K value.
 * 
 * 
 * Parameters:
 *
 *   u_UncK - Coded unc value
 *   u_Horiz - If True, Return Horizontal Unc. Otherwise return Vertical.
 *
 * Return value: 
 *
 *   Unc associated with the input uncertainty K value
 *
 ******************************************************************************
*/

float tm_umts_common_utils_GADK_to_meter( uint8 u_Horiz, uint8 u_UncK )
{
  return cgps_utils_GADK_to_meter(u_Horiz, u_UncK);
}

/*
 ******************************************************************************
 *
 * tm_umts_common_utils_microsec_to_GADK
 *
 * Function description:
 * This is obtained from the GAD spec V3.2.0
 *
 * The uncertainty r, expressed in microseconds, is mapped to a number K, 
 * with the following formula:
 *
 *    r = C [ (1 + x)^K - 1 ]    with C = 0.0022 and x = 0.18  
 *
 * The K value must be in the range 0 <= K <= 127.  If the uncertainty
 * does not exactly match a K mapping value, then we bump up to the
 * next higher K value.  This ensures that the reported UNC is not 
 * smaller than the actual UNC.  We keep K >= 1, as K=0 means 0 microseconds
 * of uncertainty, which is not reasonable for computed measurements.
 *
 * This code/algorithm has been verified separately with a separate program.
 * The program took all values of K from 0 to 127.  The corresponding range
 * value from the mapping table was used as an input to test the code. 
 * For each value of K, the range value, the range value plus a small number, 
 * and the range value minus a small number were all input into the function
 * and the output K value was compared to what was desired.
 * 
 * 
 * Parameters:
 *
 *   q_time_unc_microseconds - Time uncertainity in microseconds 
 *
 * Return value: 
 *
 *   K value, which has a range 0 <= K <= 127;
 *
 ******************************************************************************
*/

uint8 tm_umts_common_utils_microsec_to_GADK( float q_time_unc_microseconds )
{
  return cgps_utils_microsec_to_GADK(q_time_unc_microseconds);
}

/*===========================================================================

FUNCTION
  tm_umts_common_utils_NBytesGet_bigEndian

DESCRIPTION
  NBytesGet will construct a uint32 value from up to 4 bytes from an input
  U8 array.  The bytes are read from MSB byte to LSB byte order: big endian

PARAMETERS
  p_B - Pointer to a data array pointer. This allows the input pointer
        to be updated.
  q_N - Number of bytes to read in from U8 array.

RETURN VALUES
  q_Word - U32 constructed from input array; data array pointer is
           updated to point to first byte of next field.

===========================================================================*/

uint32 tm_umts_common_utils_NBytesGet_bigEndian (const uint8**  p_B, uint8 u_NumBytes )
{
  uint32  q_Word = 0;
  uint8   u_ByteCount;

  if (u_NumBytes > 4)
  {
    u_NumBytes = 4;
  }

  /* Build U32 */
  for (u_ByteCount=u_NumBytes; u_ByteCount; u_ByteCount--)
  {
    q_Word <<= 8;
    q_Word |= (uint32) *((*p_B)++);
  }

  return (q_Word);
}


/*
 ******************************************************************************
 * tm_umts_common_utils_S32Convert
 *
 * Function description:
 *
 *  This function takes an unsigned 32-bit value and converts it to a 32-bit
 *  sign extended value.
 *
 * Parameters:
 *
 *  q_Word - U32 to be converted
 *  u_MsbBitPos - Position of Most Significant Bit of input
 *
 * Return value:
 *
 *  l_Word - Signed 32-bit value
 *
 ******************************************************************************
*/

int32 tm_umts_common_utils_S32Convert (uint32 q_Word, uint8 u_MsbBitPos)
{
  int32 l_Word;

  l_Word = (int32) q_Word;
  l_Word <<= (32 - u_MsbBitPos);
  l_Word >>= (32 - u_MsbBitPos);

  return (l_Word);
}

/*
 ******************************************************************************
 * me_GpsMsecWeekLimit
 *
 * Function description:
 *
 *  tm_umts_common_utils_GpsMsecWeekLimit is a helper function used to perform the 
 *  fairly common check to see if a msec is indeed with the allowable
 *  range of 0 thru WEEK_MSECS-1. Only values within a single week
 *  are entertained. (ie- Multiple week adjustments are not performed
 *
 *  Assigning a NULL pointer to p_GpsWeek will disable the week 
 *  adjustment logic ... ie - The p_GpsMsec will only be affected.
 *
 * Parameters: 
 *
 *  p_GpsMsecs - Pointer to the msec value under test
 *  p_GpsWeek - Pointer to week number which may be adjusted
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void tm_umts_common_utils_GpsMsecWeekLimit( int32 *p_GpsMsecs, uint16 *p_GpsWeek )
{
  if( *p_GpsMsecs < 0 )
  {
    (*p_GpsMsecs) += WEEK_MSECS;

    if( (p_GpsWeek != NULL) && (*p_GpsWeek != C_GPS_WEEK_UNKNOWN) )
    {
      (*p_GpsWeek) --;
  }
  }
  else if( *p_GpsMsecs >= WEEK_MSECS )
  {
    (*p_GpsMsecs) -= WEEK_MSECS;

    if( (p_GpsWeek != NULL) && (*p_GpsWeek != C_GPS_WEEK_UNKNOWN) )
    {
      (*p_GpsWeek) ++;
  }
}
}



/******************************************************************************
 * tm_umts_common_utils_full_gps_ms()
 *
 * Function description:
 *
 *  Simple helper function to convert weeks/msecs into the number
 *  of msecs since GPS clock was started. 
 *
 * Parameters:
 *  w_WeekNum - Number of weeks since GPS clock started
 *  q_Ms - Number of msecs into the current week
 *
 * Return value:
 *
 *  Time is msecs since the GPS clock was started
 *
 *
 ******************************************************************************
*/

uint64 tm_umts_common_utils_full_gps_ms( uint16 w_WeekNum, uint32 q_Ms )
{
  return (uint64)w_WeekNum * WEEK_MSECS + q_Ms;
}

/*
 ******************************************************************************
 *
 * tm_umts_common_utils_convert_to_default_conv
 *
 * Function description:
 *  This function converts the position report to the network to a default 90%
 *  If the default is not set to 90% a 68% will be used.
 *
 * Parameters:
 *
 *  lr_data_ptr: IN: pointer to a structure that holds TmCore's LR report
 *  u_AlongUncK :OUT: Uncertainity of the output for AlongUnc
 *  u_PerpUncK : OUT: Uncertainity of the output for AlongUnc
 *
 * Return value: TRUE/FALSE: FALSE if null pointers are sent, TRUE otherwise  
 *
 ******************************************************************************
*/
static boolean tm_umts_common_utils_convert_to_default_conv(
   const sm_GnssFixRptStructType const *pz_lr_data,
   uint8 *u_AlongUncK,
   uint8 *u_PerpUncK)
{
  FLT f_confFactor = 2.1460F;

  uint8 orig_AlongUncK;
  uint8 orig_PerpUncK;

  if ( NULL == pz_lr_data ||
       NULL == u_AlongUncK ||
       NULL == u_PerpUncK )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Input to conversion function");
    return FALSE;
  }
  if ( TM_CP_DEFAULT_CONFIDENCE != 90 )
  {
    /* If default confidence is not set to 90, use 68%*/
    f_confFactor = 1.5096F;
  }
  *u_AlongUncK = tm_umts_common_utils_meter_to_GADK(TRUE, (uint32)(pz_lr_data->z_NavPos.f_ErrorEllipse[1] * f_confFactor));
  *u_PerpUncK  = tm_umts_common_utils_meter_to_GADK(TRUE, (uint32)(pz_lr_data->z_NavPos.f_ErrorEllipse[2] * f_confFactor));

  /* mainly for debugging */
  orig_AlongUncK = tm_umts_common_utils_meter_to_GADK(TRUE, (uint32)(pz_lr_data->z_NavPos.f_ErrorEllipse[1] * 1.5096));
  orig_PerpUncK  = tm_umts_common_utils_meter_to_GADK(TRUE, (uint32)(pz_lr_data->z_NavPos.f_ErrorEllipse[2] * 1.5096));
  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "LR oring Along Unc = %u, Orig Perp Unc = %u, New Along Unc = %u, New Perp Unc = %u",
        orig_AlongUncK, orig_PerpUncK, *u_AlongUncK, *u_PerpUncK);
  return TRUE;


}
/*
 ******************************************************************************
 *
 * tm_umts_common_utils_construct_ellip_alt_unc_ellip_from_lr
 *
 * Function description:
 *  This function constructs the external geographical location information 
 *  data type as defined in GSM 09.02, for the shape of ellipsoid with altitude
 *  and uncertainty ellipsoid using position information from TmCore's LR data.
 *
 * Parameters:
 *
 *  p_zEllipAltUncEllip: Pointer to the structure that stores bytes that are  
 *  GAD shape ready. This is where this function stores its result.
 *
 *  lr_data_ptr: pointer to a structure that holds TmCore's LR report
 * 
 *  u_unc_confidence: confidence level of the source.
 *
 *  v_network_init : If session is NI or SI
 * 
 * Return value: TRU/FALSE: FALSE if TmCore provided position cannot be
 *                          represented using GAD Shape 9: ellipsoid with
 *                          altitude and uncertainty ellipsoid
 *
 ******************************************************************************
*/

boolean tm_umts_common_utils_construct_ellip_alt_unc_ellip_from_lr
(
  gad_ellip_alt_unc_ellip_type* p_zEllipAltUncEllip,
  sm_GnssFixRptStructType*      lr_data_ptr,
  uint8                         u_unc_confidence,
  boolean                       v_network_init
)
{
  uint16 w_Word;
  uint32 q_LongWord;
  int32 l_LongWord;
  float f_float;
  double d_Double;
  boolean v_ConvSuccess = FALSE;
  uint8  u_AngleN, u_NewAngleN, u_AlongUncK, u_PerpUncK;

//  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "LR pos report - w_PositionFlags: %x",
//        lr_data_ptr->z_NavPos.w_PositionFlags);

  if ( NULL == p_zEllipAltUncEllip )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Input to conversion function");
    return FALSE; 
  }
  p_zEllipAltUncEllip->u_ShapeCode = C_POS_SHAPE_ELLIP_ALT_UNC_ELLIP << 4; /* Octet1: shape */


  /******** latitude ****************/

  p_zEllipAltUncEllip->u_Lat0 = 0;

  d_Double = lr_data_ptr->z_NavPos.d_PosLla[0] * C_LAT_RAD_TO_GAD;

  if ( lr_data_ptr->z_NavPos.d_PosLla[0] < 0 ) /* if latitude in the southern hemesphere */
  {
    p_zEllipAltUncEllip->u_Lat0 = 0x80; /* sign bit high for southern
                                           hemesphere */

    /* then code the rest with the absolute value of the latitude */
    d_Double = -d_Double;
  }

  q_LongWord = (uint32)tm_util_nearest_long( d_Double );

  p_zEllipAltUncEllip->u_Lat2 = (uint8)q_LongWord;
  p_zEllipAltUncEllip->u_Lat1 = (uint8)(q_LongWord >> 8);
  p_zEllipAltUncEllip->u_Lat0 |= (uint8)(q_LongWord >> 16);

  /********** longitude ************/

  d_Double = lr_data_ptr->z_NavPos.d_PosLla[1] * C_LON_RAD_TO_GAD;

  l_LongWord = tm_util_nearest_long( d_Double );
  
  p_zEllipAltUncEllip->u_Long2 = (uint8)l_LongWord;
  p_zEllipAltUncEllip->u_Long1 = (uint8)(l_LongWord >> 8);
  p_zEllipAltUncEllip->u_Long0 = (uint8)(l_LongWord >> 16);


  /********* altitude ***************/
  
  p_zEllipAltUncEllip->u_Alt0 = 0;
  if ( lr_data_ptr->z_NavPos.d_PosLla[2] < 0 )
  {
    p_zEllipAltUncEllip->u_Alt0 = 0x80;
  }
  w_Word = (uint16) tm_util_nearest_long(fabs(lr_data_ptr->z_NavPos.d_PosLla[2]));
  p_zEllipAltUncEllip->u_Alt1 = (uint8)w_Word;
  p_zEllipAltUncEllip->u_Alt0 |= (uint8)(w_Word >> 8);

  /********* uncertainty  *************/

  /* Truncate, then divide by 2 without rounding */
  u_AngleN = (uint8) (((uint32) lr_data_ptr->z_NavPos.f_ErrorEllipse[0]) >> 1);

  /* Along and perpendicular uncs are currently obtained for 1 sigma (from PE), 
  *  i.e., 39% confidence level for ellip., multiply by a constant to obtain 2D 68% confidence.
  *  See \\adobe\Development\Saber\Documentation\Memos\ChiDist21 DCM.xls
  * for how this scaling is achieved.
  */

  if ( u_unc_confidence == 39 )
  {
    if ( v_network_init )
    {
      /* for NI sessions, use a 90% confidence */
      v_ConvSuccess = tm_umts_common_utils_convert_to_default_conv( lr_data_ptr, &u_AlongUncK, &u_PerpUncK);
    }
    if (FALSE == v_ConvSuccess )
    {

    u_AlongUncK = tm_umts_common_utils_meter_to_GADK(TRUE, (uint32) (lr_data_ptr->z_NavPos.f_ErrorEllipse[1] * 1.5096));
    u_PerpUncK  = tm_umts_common_utils_meter_to_GADK(TRUE, (uint32) (lr_data_ptr->z_NavPos.f_ErrorEllipse[2] * 1.5096));
      p_zEllipAltUncEllip->u_Confidence = 68; /* conversion done above */
    }
    else 
    {
      p_zEllipAltUncEllip->u_Confidence = TM_CP_DEFAULT_CONFIDENCE; /* conversion done above */
    }
  }
  else
  {
    /* this function currently does not have conversion for other confidence level */
    u_AlongUncK = tm_umts_common_utils_meter_to_GADK(TRUE, (uint32) lr_data_ptr->z_NavPos.f_ErrorEllipse[1]);
    u_PerpUncK  = tm_umts_common_utils_meter_to_GADK(TRUE, (uint32) lr_data_ptr->z_NavPos.f_ErrorEllipse[2]);
    p_zEllipAltUncEllip->u_Confidence = u_unc_confidence; 
  }

  if ( u_AlongUncK > u_PerpUncK ) 
  {
    p_zEllipAltUncEllip->u_OrientMajor = u_AngleN;
    p_zEllipAltUncEllip->u_UncMajor = u_AlongUncK;
    p_zEllipAltUncEllip->u_UncMinor = u_PerpUncK;
  }
  else
  {
    /* Add 90 deg to angle; 90/2 since value is scaled by 2.
     * The max incoming angle is 90 degree, so adding 90 to
     * this yields 180, or a N value of 90.  The N range
     * is 0 to 89.  So if we have N = 90 (180deg), it is the 
     * same as N = 0 (0deg). */
    u_NewAngleN = u_AngleN + 45;
    if( u_NewAngleN >= 90 )
    {
      u_NewAngleN -= 90;
    }
      
    p_zEllipAltUncEllip->u_OrientMajor = u_NewAngleN;
    
    /* Major is now along the perpendicular axis */
    p_zEllipAltUncEllip->u_UncMajor = u_PerpUncK;
    p_zEllipAltUncEllip->u_UncMinor = u_AlongUncK;
  }

  /* RRLP's limit is 990.5m, so we need to cap it here */
  f_float = lr_data_ptr->z_NavPos.f_PuncVertMeters;
  if ( f_float > (FLT)990.5) f_float = (FLT)990.5;

  p_zEllipAltUncEllip->u_UncAlt =
    tm_umts_common_utils_meter_to_GADK ( FALSE, (uint32) f_float );

  /* Along and perpendicular uncs are currently obtained for 1 sigma, 
  * then multiplied by a constant to obtain 2D 68% confidence.
  *  See \\adobe\Development\Saber\Documentation\Memos\ChiDist21 DCM.xls
  * for how this scaling is achieved.
  */

  return TRUE;
}

/*
 ******************************************************************************
 *
 * tm_umts_common_utils_cm_emergency_call_status
 *
 * Function description:
 *
 * Parameters:
 *   None
 *
 * Return value: TRUE/FALSE: in E911 / not in E911 
 *
 ******************************************************************************
*/

boolean tm_umts_common_utils_cm_emergency_call_status(void)
{
  tm_cm_phone_state_info_s_type phone_state_info;
  boolean ret = FALSE;

  tm_cm_iface_get_phone_state(&phone_state_info);

  if ( ( phone_state_info.e911_state == TM_CM_E911_STATE_ORIG ) ||
       ( phone_state_info.e911_state == TM_CM_E911_STATE_CB ) 
     )  
  {
    ret = TRUE;
  }
   
  return ret;
}

/*
 ******************************************************************************
 *
 * tm_umts_common_utils_
 *
 * Function description: check to see what kind of effect E911 call should have
 *   on an NILR position request, if there is an E911 going on.
 *
 * Parameters:
 *   None
 *
 * Return value: pos_session_req_acceptance_based_on_e911 
 *                 null ( not in E911 ), 
                   accept only high priority, 
                   accept all,
                   or reject all. 
 *
 ******************************************************************************
*/

emergency_handling_e_type tm_umts_common_utils_emergency_effect ( )
{
  emergency_handling_e_type  e_e911_effect = C_EMERGENCY_HANDLING_NULL;

  if ( tm_umts_common_utils_cm_emergency_call_status() ||
       ( z_umts_nv_item_ram_copy.aagps_development_test_control5 &
         NV_AAGPS_TEST5_EMERGENCY_CALL_SIM ) )
  {

    if ( z_umts_nv_item_ram_copy.aagps_emergency_services_spprt == AAGPS_NV_EMERGENCY_SUPPORT_REJECT_ALL )
    {
      e_e911_effect = C_EMERGENCY_HANDLING_REJ_ALL; 
    }
    else
    {
      if ( z_umts_nv_item_ram_copy.aagps_emergency_services_spprt == AAGPS_NV_EMERGENCY_SUPPORT_HIGH_PRIORITY )
      {
        e_e911_effect = C_EMERGENCY_HANDLING_ACCEPT_ONLY_HIGH_PRIORITY;
      }
      else if ( z_umts_nv_item_ram_copy.aagps_emergency_services_spprt == AAGPS_NV_EMERGENCY_SUPPORT_LOW_PRIORITY )
      {
        e_e911_effect = C_EMERGENCY_HANDLING_ACCEPT_ALL;
      }
    }
  }

  return e_e911_effect;

}


/*
 ******************************************************************************
 *
 * tm_umts_common_utils_build_eph_week_from_ref_time
 *
 * Function description: when referenct time is given in the same message where
 *   ephemeris is given, eph. week number is constructed here.
 *  
 * Parameters:
 *
 *   w_toe: eph. toe of this SV;
 *   q_ref_msec: msec. part of the reference time;
 *   w_ref_week: week number part of the reference time;
 *
 * Return value: 
 *  
 *   w_eph_week: eph. week of this SV. 
 *
 ******************************************************************************
*/

uint16 tm_umts_common_utils_build_eph_week_from_ref_time ( uint16 w_toe, uint32 q_ref_msec, uint16 w_ref_week )
{
  uint32 q_ToeMs; /* full msec. of TOE */
  int32  l_MsDifference; 
  uint16 w_eph_week;

  /* handles the special condition of the last 2 hours of a week: 
     Gps time in one week while Toe is referenced to the next week */
  q_ToeMs = (uint32)w_toe * (uint32)(16 * 1000); /* unit 16 sec */
  l_MsDifference = (int32)q_ref_msec - (int32)q_ToeMs;

  if (l_MsDifference > (int32)(WEEK_MSECS/2.0) )
  {
    /* handles the special condition of the last 2 hours of a week */ 
    w_eph_week = w_ref_week + 1;
  }
  else if (l_MsDifference < ( 0 - (int32)(WEEK_MSECS/2.0) ) )
  {
    /* should never get here, as server never gives eph. 
       whose toe is in the past */
    w_eph_week = w_ref_week - 1;
  }
  else
  {
    w_eph_week = w_ref_week;  
  } 

  return w_eph_week;

}


/********************************************************************
 *
 * tm_umts_common_utils_gsm0931_gps_assist_data_ie_build
 *
 * Function description:
 *  Forms a GSM 09.31 compliant GPS Assistance Data IE
 *
 * Parameters:
 *
 *
 * p_gps_assist_wishlist_gsm_0931: pointer to the AssistanceData structure for 
 * this function to fill
 * 
 * Return:
 *   None
 *
 *********************************************************************
 */
void tm_umts_common_utils_gsm0931_gps_assist_data_ie_build 
(
  sm_InternalAssistDataStatusStructType* p_assist_data_wishlist,
  GpsAssisData_Gsm0931*                  p_gps_assist_wishlist_gsm_0931
)
{
  uint8   u_nsat = 0, u_i;
  uint16  w_EphWeek_10 = 0, w_iode;

  /* at least octet 3 and 4 included */
  p_gps_assist_wishlist_gsm_0931->u_Octet2_LengthIndicator = 2;
  p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH = 0;
  p_gps_assist_wishlist_gsm_0931->u_Octet4_Whishlist_IP = 0;

  /* check whether to request something else since a request is to be made anyway: */

  /* if any of refTime, eph, or alm is requested, get RTI too */
  if (  p_assist_data_wishlist->q_WishListMask &  
        ( C_GPS_ASSIS_REFTIME | C_GPS_ASSIS_NAVMODL | C_GPS_ASSIS_ALM )
     )
  {
    p_assist_data_wishlist->q_WishListMask |= C_GPS_ASSIS_RTI;
  }

  if ( p_assist_data_wishlist->q_WishListMask & C_GPS_ASSIS_REFTIME )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_REFTIME;
  }
  if ( p_assist_data_wishlist->q_WishListMask & C_GPS_ASSIS_REFLOC )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_REFLOC;
  }
  if ( p_assist_data_wishlist->q_WishListMask & C_GPS_ASSIS_ACQ )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_ACQ;
  }
  if ( p_assist_data_wishlist->q_WishListMask & C_GPS_ASSIS_NAVMODL )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_NAVMODL;
  }
  if ( p_assist_data_wishlist->q_WishListMask & C_GPS_ASSIS_ALM )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_ALM;
  }
  if ( p_assist_data_wishlist->q_WishListMask & C_GPS_ASSIS_IONO )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_IONO;
  }
  if ( p_assist_data_wishlist->q_WishListMask & C_GPS_ASSIS_RTI )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet4_Whishlist_IP |= ( C_G0931_GPS_ASSIST_RTI >> 8 );
  }
  if ( p_assist_data_wishlist->q_WishListMask & C_GPS_ASSIS_UTC )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_UTC;
  }
      

  /* CONDITIONAL navigation info */
  if (p_assist_data_wishlist->q_WishListMask & C_GPS_ASSIS_NAVMODL )
  {

    /* at least octet 5,6,7,8 included */
    p_gps_assist_wishlist_gsm_0931->u_Octet2_LengthIndicator +=4; 

    MSG_MED( "Target eph. inc. %u, nSvs: %u", 
             p_assist_data_wishlist->u_EphInfoIncl, 
             p_assist_data_wishlist->u_NumSvs,
             0 );

    if ( p_assist_data_wishlist->u_EphInfoIncl && 
         ( p_assist_data_wishlist->u_NumSvs > 0 )
       )
    {
      w_EphWeek_10 = p_assist_data_wishlist->w_GpsWeek % 1024;

      p_gps_assist_wishlist_gsm_0931->u_Octet5_GpsWeek_MSB2 =  
        (w_EphWeek_10 >> 2) & 0x00C0;

      p_gps_assist_wishlist_gsm_0931->u_Octet6_GpsWeek_LSB8 =
        w_EphWeek_10 & 0x00FF;

      /* toe in hours: w_toe unit is 16 sec . (1hour = 3600 sec, 3600/16 = 225 ) */
      p_gps_assist_wishlist_gsm_0931->u_Octet7_GpsToe = 
        p_assist_data_wishlist->w_Toe / C_NUM_16SEC_IN_ONE_HOUR;

      p_gps_assist_wishlist_gsm_0931->u_Octet8_NSAT_ToeLimit =
        ( p_assist_data_wishlist->w_ToeLimit <= 10 ) ? 
        p_assist_data_wishlist->w_ToeLimit : 10;  

      u_nsat = ( p_assist_data_wishlist->u_NumSvs <= 15 ) ? 
               p_assist_data_wishlist->u_NumSvs : 15;
       
      for ( u_i=0; u_i< u_nsat; u_i++ )
      {
        w_iode = (uint16)p_assist_data_wishlist->z_SvEphIode[u_i].w_Iode;

        p_gps_assist_wishlist_gsm_0931->w_SvId_Iode[u_i] = 
          ( p_assist_data_wishlist->z_SvEphIode[u_i].w_Sv & 0x3F ) | 
          ( w_iode << 8 );
      }      
    }
  
    p_gps_assist_wishlist_gsm_0931->u_Octet2_LengthIndicator +=  2 * u_nsat; 
    
    /* if no SV is to be included, set Oct. 5 - 8 all 0 */
    if (u_nsat == 0 )
    {
      p_gps_assist_wishlist_gsm_0931->u_Octet5_GpsWeek_MSB2 = 0; 
      p_gps_assist_wishlist_gsm_0931->u_Octet6_GpsWeek_LSB8 = 0; 
      p_gps_assist_wishlist_gsm_0931->u_Octet7_GpsToe = 0; 
      p_gps_assist_wishlist_gsm_0931->u_Octet8_NSAT_ToeLimit = 0;
    }  
      
    /* now fill in NSAT */
    p_gps_assist_wishlist_gsm_0931->u_Octet8_NSAT_ToeLimit |= u_nsat << 4; 
      
  } /* if-wishlist-contains-navmodel-req */
} /* tm_umts_common_utils_gsm0931_gps_assist_data_ie_build */


/********************************************************************
 *
 * tm_umts_common_utils_gsm0931_ganss_assist_data_ie_build
 *
 * Function description:
 *  Forms a GSM 09.31 compliant GANSS Assistance Data IE
 *
 * Parameters:
 *
 *
 * p_gps_assist_wishlist_gsm_0931: pointer to the AssistanceData structure for 
 * this function to fill
 * 
 * Return:
 *   None
 *
 *********************************************************************
 */
void tm_umts_common_utils_gsm0931_ganss_assist_data_ie_build 
(
  sm_InternalAssistDataStatusStructType* p_assist_data_wishlist,
  GpsAssisData_ganss_Gsm0931 *p_gps_assist_wishlist_gsm_0931
)
{
  boolean  b_wishlist_eph  = FALSE;
  boolean  b_eph_info_incl = FALSE;
  uint8    u_NumSvs        = 0;
  uint8    u_nsat = 0, u_i;
  uint16   w_EphWeek_12 = 0, w_iode;
  uint32   q_WishListMask = 0;

  /* at least octet 3 and 4 included */
  p_gps_assist_wishlist_gsm_0931->u_Octet2_LengthIndicator = 2;
  p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH = 0;
  p_gps_assist_wishlist_gsm_0931->u_Octet4_Whishlist_IP = 0;

  /* check whether to request something else since a request is to be made anyway: */

  /* if any of GPS refTime or GLONASS eph, or alm is requested, get RTI too */
  q_WishListMask = p_assist_data_wishlist->z_GloAssistData.q_WishListMask;
  q_WishListMask |= p_assist_data_wishlist->q_WishListMask & C_GPS_ASSIS_REFTIME;
  if (q_WishListMask  & ( C_GPS_ASSIS_REFTIME | C_GPS_ASSIS_NAVMODL | C_GPS_ASSIS_ALM ))
  {
    p_assist_data_wishlist->z_GloAssistData.q_WishListMask |= C_GPS_ASSIS_RTI;
  }

  /* --------------------------------------------------------*/
  /* Check GLONASS wishlist and update the Assitance Data IE */
  /* --------------------------------------------------------*/
  if ( p_assist_data_wishlist->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_REFTIME )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_REFTIME;
  }
  if ( p_assist_data_wishlist->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_REFLOC )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_REFLOC;
  }
  if ( p_assist_data_wishlist->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_ACQ )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_ACQ;
  }
  if ( p_assist_data_wishlist->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_NAVMODL )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_NAVMODL;
  }
  if ( p_assist_data_wishlist->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_ALM )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_ALM;
  }
  if ( p_assist_data_wishlist->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_IONO )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_IONO;
  }
  if ( p_assist_data_wishlist->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_RTI )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet4_Whishlist_IP |= C_G0931_GPS_ASSIST_RTI;
  }
  if ( p_assist_data_wishlist->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_UTC )
  {
    p_gps_assist_wishlist_gsm_0931->u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_UTC;        
  }

  /* Construct 
     b_wishlist_eph
     b_eph_info_incl , u_NumSvs (for targeted eph) 
  */
  if (p_assist_data_wishlist->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_NAVMODL )
  {
    b_wishlist_eph = TRUE;
  }
  if (p_assist_data_wishlist->z_GloAssistData.u_EphInfoIncl == TRUE)
  {
    b_eph_info_incl = TRUE;
  }
  u_NumSvs =  p_assist_data_wishlist->z_GloAssistData.u_NumSvs;

  MSG_MED( "Ephemeris Wishlist GLONASS:%u",
           p_assist_data_wishlist->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_NAVMODL, 0, 0);

  /* CONDITIONAL navigation info */
  if (b_wishlist_eph)
  {
    /* at least octet 5,6,7,8 included */
    p_gps_assist_wishlist_gsm_0931->u_Octet2_LengthIndicator +=4; 

    MSG_MED( "GLONASS Target eph. inc.:%u, nSvs.:%u",  
             p_assist_data_wishlist->z_GloAssistData.u_EphInfoIncl,
             p_assist_data_wishlist->z_GloAssistData.u_NumSvs, 0);
           
    if ( b_eph_info_incl && ( u_NumSvs > 0 ))
    {
       /* SM API from MGP for glo_AlmStructType member w_Na: Number of days in a 4-year cycle; 
          SUPL spec.: If GANSS ID indicates GLONASS this field is expressed in days as defined
          in [3GPP 49.031], which states "this field contains a 11 bit binary representation of
          the calendar number of day within the four-year interval starting from 1st of January
          in a leap year"
       */
       w_EphWeek_12 = p_assist_data_wishlist->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_Na;  
       p_gps_assist_wishlist_gsm_0931->u_Octet5_GanssWeek_MSB4 =  
          (w_EphWeek_12 >> 4) & 0x00F0;
       p_gps_assist_wishlist_gsm_0931->u_Octet6_GanssWeek_LSB8 =
          w_EphWeek_12 & 0x00FF;

      /* SUPL: If GANSS ID is set to ‘GLONASS’, this field contains a binary representation of the time
         of ephemeris in units of 15 minutes of the newest ephemeris set contained in handset memory
         (binary range 0 to 95 representing time values between 0 and 1425 minutes).
       
         MGP API for w_toe in GLONASS eph. time matches the above definition.
       */
       
      u_nsat = ( u_NumSvs <= 15 ) ? u_NumSvs : 15;
      p_gps_assist_wishlist_gsm_0931->u_Octet7_GanssToe = 
        ( p_assist_data_wishlist->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_Tb <= 95) ?
         p_assist_data_wishlist->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_Tb : 95;

      /* If GANSS ID is set to ‘GLONASS’, this field contains a binary representation of the GANSS ephemeris
         age tolerance of the MS to the network in units of 30 minutes (binary range 0 to 15 representing time
         values of  0 to 450 minutes). This is double sided.
       
         MGP API: Ephemeris Age tolerance, double sided: 
         1 = 30 min, 2 = 45 min, 3 = 60 minutes, 0 = reserved; others: invalid.
       */

      if ( p_assist_data_wishlist->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_ToeLimit == 0 ) 
      {
        p_gps_assist_wishlist_gsm_0931->u_Octet8_NSAT_ToeLimit = 0;
      }
      else if ( p_assist_data_wishlist->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_ToeLimit == 1 ) 
      {
        p_gps_assist_wishlist_gsm_0931->u_Octet8_NSAT_ToeLimit = 1;
      }
      else if ( p_assist_data_wishlist->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_ToeLimit == 2 ) 
      {
        /* SUPL does not have 45 min representation, collapse to 30 min. */
        p_gps_assist_wishlist_gsm_0931->u_Octet8_NSAT_ToeLimit = 1;
      }
      else if ( p_assist_data_wishlist->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_ToeLimit == 3 ) 
      {
        p_gps_assist_wishlist_gsm_0931->u_Octet8_NSAT_ToeLimit = 2;
      }
      else
      {
        p_gps_assist_wishlist_gsm_0931->u_Octet8_NSAT_ToeLimit = 0;
      }


      u_nsat = ( u_NumSvs <= 15 ) ? u_NumSvs : 15;
      for ( u_i=0; u_i< u_nsat; u_i++ )
      {
        /* ayt_eph - For Targeted Eph Implementation - 
                     MGP reports 8 bit IOD. RRLP8 spec is 16 bit IOD 
                     Andreas Wachter is checking spec on this. 
        */
        w_iode = (uint16)p_assist_data_wishlist->z_GloAssistData.z_SvEphIode[u_i].w_Iode; 
        p_gps_assist_wishlist_gsm_0931->w_SvId_Iode[u_i] = 
            ( p_assist_data_wishlist->z_GloAssistData.z_SvEphIode[u_i].w_Sv & 0x3F ) | 
            ( w_iode << 8 );
      } /* for */
    }
  
    p_gps_assist_wishlist_gsm_0931->u_Octet2_LengthIndicator +=  2 * u_nsat; 
  
    /* if no SV is to be included, set Oct. 5 - 8 all 0 */
    if (u_nsat == 0 )
    {
      p_gps_assist_wishlist_gsm_0931->u_Octet5_GanssWeek_MSB4 = 0; 
      p_gps_assist_wishlist_gsm_0931->u_Octet6_GanssWeek_LSB8 = 0; 
      p_gps_assist_wishlist_gsm_0931->u_Octet7_GanssToe = 0; 
      p_gps_assist_wishlist_gsm_0931->u_Octet8_NSAT_ToeLimit = 0;
    }  
      
    /* now fill in NSAT */
    p_gps_assist_wishlist_gsm_0931->u_Octet8_NSAT_ToeLimit |= u_nsat << 4; 
  }/* CONDITIONAL navigation info */

} /* tm_umts_common_utils_gsm0931_ganss_assist_data_ie_build */


/*
 ******************************************************************************
 *
 * tm_umts_common_utils_psudoRMS_formater
 *
 * Function description:
 *  This function converts the psedorange RMS ( in meters ) to an index-based
 *  representation defined by the RRLP protocol. The index-based representation
 *  uses a 6-bit index, in which the low 3 bits is the mantissa (x) and the
 *  high 3 bits is the exponent (y). And index of i states that the pseudo
 *  range RMS error (P) is within the range defined by Z (i-1) <= P < Z (i),
 *  where Z (i ) = 0.5 * (1 + x / 8 ) * 2 ** y.
 *
 * Parameters:
 *
 *  f_pr_rms_meters: psedorange RMS error in meters.
 *
 *
 * Return value:
 *
 *  The index (0..63) as defined by the RRLP protocol.
 *
 ******************************************************************************
*/

uint8 tm_umts_common_utils_psudoRMS_formater ( float f_pr_rms_meters )
{
  float f_ValueGroup[64];
  uint8 u_i, u_Mantissa_R3, u_Expo_R3;
  uint16 w_Word;
  uint8 u_ret = 0;

  #if 0
    FILE *p_rms_table = NULL;
  #endif

  for (u_i=0; u_i<64; u_i++)
  {

    u_Mantissa_R3 = u_i & 0x07; /* Bit 0-2 of index: mantissa */
    u_Expo_R3 = (u_i >> 3) & 0x07; /* Bit 3-5 of index: exponent */

    w_Word = (uint16)u_Mantissa_R3; /* creating a floating point number with
                      using U16, with its high byte
                    representing whole number and its low
                    byte reprenting fractional number */

    w_Word <<= 5; /* left-shift 8 for the high byte to represent whole
               number, and then right-shift 3 for dividing by 8,
                     up to here: x/8  */

    w_Word += 0x0100;  /* 1 + x / 8 */
    w_Word <<= u_Expo_R3; /* ( 1 + x/8 ) * 2 ** y  */
    w_Word >>= 1; /* 0.5 * ( 1 + x/8 ) * 2 ** y */


    /* LSB of the fractional part: 2 ** (-8 ) = 0.00390625 */
    f_ValueGroup[u_i] = (float)(w_Word >> 8) +
                        (float)(w_Word & 0xFF) * (float)0.00390625 ;
  }

  #if 0 /* diag only */
  {
    p_rms_table = fopen("DiagData/RmsTable.txt", "a");
    if (p_rms_table != NULL)
    {
      fprintf(p_rms_table, "Pseudorange RMS Error Table \n");
      for (u_i=0; u_i<64; u_i++)
      {
        fprintf(p_rms_table, "%f\n", f_ValueGroup[u_i]);
      }
    }
  }
  #endif


  if ( f_pr_rms_meters < f_ValueGroup[0] )
  {
    u_ret = 0;
  }
  else if ( f_pr_rms_meters >= f_ValueGroup[62] )
  {
    u_ret = 63;
  }
  else
  {
    for (u_i=1; u_i < 63; u_i++)
    {
      if ( (f_pr_rms_meters < f_ValueGroup[u_i]) &&
         (f_pr_rms_meters >= f_ValueGroup[u_i - 1])
         )
      {
        u_ret = u_i;
        break;
      }
    }
  }

  #if 0  /* diag only */
  if (p_rms_table != NULL)
  {
    fprintf(p_rms_table,
            "RMS in meters = %f, index = %d\n", f_pr_rms_meters, u_ret);
    fflush( p_rms_table );
    fclose( p_rms_table );
  }
  #endif

  return u_ret;
}


/*===========================================================================

FUNCTION tm_umts_common_utils_gad_to_pdsm_pd_info 

DESCRIPTION
  Convert position information in BSSMP-LE GAD shapes to that of pdsm_pd_info

Parameters:

  p_bssmap_le_location_estimate: pointer to the GAD shape.

  pos_type: 
  PDSM_POSITION_INVALID = 0; 
  PDSM_POSITION_OLD = 1; 
  PDSM_POSITION_NEW =2

  gps_source:  
  PDSM_PD_DLOAD_SOURCE_MS = 0; 
  PDSM_PD_DLOAD_SOURCE_PDE = 1; 
  PDSM_PD_DLOAD_SOURCE_STANDALONE = 2;

  q_time_stamp_sec : full GPS second of the position 

  pd_info_ptr  : pointer to the pdapi position structure this function is to populate.
  pd_event_ptr : pointer to the event variable this function is to populate.

===========================================================================*/

boolean tm_umts_common_utils_gad_to_pdsm_pd_info 
( uint32                              q_client_id,
  uint8                               *p_bssmap_le_location_estimate, 
  pdsm_pd_gps_position_e_type         pos_type,
  pdsm_pd_position_mode_e_type        pos_mode,
  pdsm_pd_positioning_source_type     pos_source,
  uint32                              q_time_stamp_sec,
  pdsm_pd_info_s_type                 *pd_info_ptr,
  pdsm_pd_event_type                  *pd_event_ptr
)
{

  pdsm_pd_data_type*    pos_data_ptr;

  uint32 q_shape_code=0, q_long_word=0;
  int32  l_long_word = 0;
  uint8  u_octet1_low_nibble = 0;
  float f_major_unc_meters, f_minor_unc_meters, f_unc_meters;

  pos_data_ptr       = &pd_info_ptr->pd_info.pd_data;

  q_shape_code = *p_bssmap_le_location_estimate++;
  u_octet1_low_nibble = (uint8)q_shape_code;
  u_octet1_low_nibble &= 0x0F;
  q_shape_code >>= 4;

  /* Populate the Client ID before reporting */
  pd_info_ptr->client_id = q_client_id;

  pos_data_ptr->opt_field_mask = PDSM_PD_EXT_STATUS_VALID;
  pos_data_ptr->loc_uncertainty_conf = 0; /* 0: no information */
  pos_data_ptr->fix_type = 0; /* no altitude until given */

  *pd_event_ptr = PDSM_PD_EVENT_POSITION;

  pos_data_ptr->time_stamp  =  q_time_stamp_sec;

  /* timestamp for ext. pos. */
  if ( q_time_stamp_sec !=0 )
  {
    pos_data_ptr->ext_opt_field_mask |= PDSM_PD_GPS_TIME_VALID;
    pos_data_ptr->ext_pos_data.timestamp_gps_week = 
      (uint16)(q_time_stamp_sec / SEC_PER_WEEK); 

    pos_data_ptr->ext_pos_data.timestamp_tow_ms =
      ( q_time_stamp_sec - (pos_data_ptr->ext_pos_data.timestamp_gps_week * SEC_PER_WEEK) ) * 1000;

    pos_data_ptr->opt_field_mask |= PDSM_PD_MSEC_VALID;
    pos_data_ptr->time_stamp_msec = pos_data_ptr->ext_pos_data.timestamp_tow_ms % 1000;
  }
  else
  {
    pos_data_ptr->ext_pos_data.timestamp_gps_week = C_GPS_WEEK_UNKNOWN;
    pos_data_ptr->ext_pos_data.timestamp_tow_ms = 0;
  }

  pos_data_ptr->ext_opt_field_mask |= PDSM_PD_LAT_LONG_VALID;
  if ( q_shape_code != C_POS_SHAPE_POLYGON ) /* for non-Polygon, lat/lon. are the same */
  {
    /*************************** latitude *******************************/
    q_long_word = (uint32)*p_bssmap_le_location_estimate++;
    q_long_word <<= 16;
    q_long_word |= ( (uint32)*p_bssmap_le_location_estimate++ ) << 8;
    q_long_word |= (uint32)*p_bssmap_le_location_estimate++;

    /* PDAPI lat. LSB is ( 180 / 2^25 ) degrees, and GAD shape lat. LSB
       is (90 / 2^23 ), so, GadShapeLat * (90/2^23 ) / (180 / 2^25 ) = 
       GadShapeLat * 2 = q_long_word * 2
    */

    /* Absolute value of ext_pos_data.lat = (abs(lat rad) * C_2_32_OVER_PI);
       q_long_word * LAT_IS801_SCALING ==> abs(lat degrees)
       LAT_IS801_SCALING = 90/2^23 
       abs( ext_pos_data.lat ) = 
         q_long_word * 90 / 2^23 * 2^32 / PI * DEG_TO_RADIANS =
         q_long_word * 90 * 2^9 * DEG_TO_RADIANS / PI =
         q_long_word * 90 * 2^9 * PI / 180 / PI =
         q_long_word * 2^8 
    */

    if ( q_long_word & 0x00800000 )
    {
      q_long_word &= 0x007FFFFF;
      pos_data_ptr->lat  = (int32)( 0 - (int32)( q_long_word * 2) ); 
      pos_data_ptr->ext_pos_data.lat = (int32)( 0 - (int32)(q_long_word * 256));

    }
    else
    {
      pos_data_ptr->lat  = (int32)(q_long_word * 2 ); 
      pos_data_ptr->ext_pos_data.lat = (int32)(q_long_word * 256);
    }

    /************************ longitude ***********************************/
    q_long_word = (uint32)*p_bssmap_le_location_estimate++ << 16;
    q_long_word |= (uint32)*p_bssmap_le_location_estimate++ << 8;
    q_long_word |= (uint32)*p_bssmap_le_location_estimate++;
    if (q_long_word & 0x00800000)
    {
      q_long_word |=  0xFF800000; /* sign extend */
    }

    /* PDAPI longitude has LSB of (180/2^25) degrees, and GAD shape lat. LSB 
       is (360/2^24) degrees, so GadShapeLong * (360/2^24) / (180/2^25 ) = 
       GadShapeLong * 4     
    */

    /* ext_pos_data.lon = lon_rad * C_2_31_OVER_PI
       q_long_word * LON_IS801_SCALING ==> longitude degrees
       LON_IS801_SCALING = 180.0/(DBL)(1L<<23) 
       ext_pos_data.lon = 
         q_long_word * 180 / 2^23 * 2^31 / PI * DEG_TO_RADIANS =
         q_long_word * 180 * 2^8 * DEG_TO_RADIANS / PI =
         q_long_word * 180 * 2^8 * PI / 180 / PI =
         q_long_word * 2^8 
    */
    pos_data_ptr->lon = (int32)q_long_word * 4; 
    pos_data_ptr->ext_pos_data.lon = (int32)(q_long_word) * 256;

  } /* close-if-NOT-polygon */

  else /* if-polygon */
  {

    uint8 u_i;
    double d_avg_lat = 0.0, d_avg_long = 0.0;
    double d_double = 0.0;

    uint8 u_lat_long_0, u_lat_long_1, u_lat_long_2;

    pos_data_ptr->loc_uncertainty_conf = 0; /* 0: no information */
    pos_data_ptr->fix_type = 0; /* no altitude  */

    /* for older versions of applications that don't read unc_valid bit
       of opt_field_mask, make sure at least they get max. unc. */ 
    pos_data_ptr->loc_uncertainty_a = 0xFF;
    pos_data_ptr->loc_uncertainty_p = 0xFF;
    pos_data_ptr->loc_uncertainty_ang = 0;

    if ( u_octet1_low_nibble == 0 )
    {
      MSG_HIGH("MOLR-RR-Polygon 0 pnt", 0, 0, 0);
      return FALSE;
    }
    else if ( ( u_octet1_low_nibble == 1 ) || ( u_octet1_low_nibble == 2 ) )
    {
      MSG_MED("MOLR-RR-Polygon pnt %u", u_octet1_low_nibble, 0, 0);
    }
    else if ( u_octet1_low_nibble > 3 )
    {
      /* QQQ: polygon with N>3, use MOLR-ReturnResult with AdditionalPosition */
      MSG_HIGH("MOLR-RRpolygon>3 - forcing 3", 0, 0, 0);
      u_octet1_low_nibble = 3;
    }   


    for (u_i=0; u_i<u_octet1_low_nibble; u_i++)
    {
      u_lat_long_0 = *p_bssmap_le_location_estimate++;
      u_lat_long_1 = *p_bssmap_le_location_estimate++;
      u_lat_long_2 = *p_bssmap_le_location_estimate++;
      q_long_word = ( (uint32)u_lat_long_0 & 0x7F ) << 16;
      q_long_word |= ( (uint32)u_lat_long_1 ) << 8;
      q_long_word |= (uint32)u_lat_long_2;
      d_double = (double)q_long_word;

      if (u_lat_long_0 & 0x80 )
          d_double = -d_double;
      d_avg_lat += d_double;

      u_lat_long_0 = *p_bssmap_le_location_estimate++;
      u_lat_long_1 = *p_bssmap_le_location_estimate++;
      u_lat_long_2 = *p_bssmap_le_location_estimate++;
      q_long_word = ( (uint32)u_lat_long_0 ) << 16;
      q_long_word |= ( (uint32)u_lat_long_1 ) << 8;
      q_long_word |= (uint32)u_lat_long_2;
      if (q_long_word & 0x00800000)
      {
        q_long_word |=  0xFF800000; /* sign extend */
      }
      l_long_word = (int32)q_long_word;

      d_double = (double)l_long_word;
      d_avg_long  +=  d_double;


    } /* end of for-loop for N-point polygon */

    /* take avg. for lat. and long. */
    d_avg_lat /= (double)u_octet1_low_nibble;
    d_avg_long /= (double)u_octet1_low_nibble;

    /* PDAPI lat. LSB is ( 180 / 2^25 ) degrees, and GAD shape lat. LSB
       is (90 / 2^23 ), so, GadShapeLat * (90/2^23 ) / (180 / 2^25 ) = 
       GadShapeLat * 2 = q_long_word * 2
    */

    /* Absolute value of ext_pos_data.lat = (abs(lat rad) * C_2_32_OVER_PI);
       q_long_word * LAT_IS801_SCALING ==> abs(lat degrees)
       LAT_IS801_SCALING = 90/2^23 
       abs( ext_pos_data.lat ) = 
         q_long_word * 90 / 2^23 * 2^32 / PI * DEG_TO_RADIANS =
         q_long_word * 90 * 2^9 * DEG_TO_RADIANS / PI =
         q_long_word * 90 * 2^9 * PI / 180 / PI =
         q_long_word * 2^8 
    */

    /* PDAPI longitude has LSB of (180/2^25) degrees, and GAD shape lat. LSB 
       is (360/2^24) degrees, so GadShapeLong * (360/2^24) / (180/2^25 ) = 
       GadShapeLong * 4     
     */

    /* ext_pos_data.lon = lon_rad * C_2_31_OVER_PI
       q_long_word * LON_IS801_SCALING ==> longitude degrees
       LON_IS801_SCALING = 180.0/(DBL)(1L<<23) 
       ext_pos_data.lon = 
         q_long_word * 180 / 2^23 * 2^31 / PI * DEG_TO_RADIANS =
         q_long_word * 180 * 2^8 * DEG_TO_RADIANS / PI =
         q_long_word * 180 * 2^8 * PI / 180 / PI =
         q_long_word * 2^8 
    */
    pos_data_ptr->lat  = ( (int32)d_avg_lat) * 2; 
    pos_data_ptr->lon  = ( (int32)d_avg_long ) * 4; 
    pos_data_ptr->ext_pos_data.lat = (int32)((int32)d_avg_lat * 256);
    pos_data_ptr->ext_pos_data.lon = (int32)((int32)d_avg_long * 256);

  } /* close-if-polygon */

  /************** shape specific handling of other (non lat-long) items ******/

  if ( q_shape_code  == C_POS_SHAPE_ELLIP )
  {
    pos_data_ptr->loc_uncertainty_conf = 0; /* 0: no information */
    pos_data_ptr->fix_type = 0; /* no altitude  */

    /* for older versions of applications that don't read unc_valid bit
       of opt_field_mask, make sure at least they get max. unc. */ 
    pos_data_ptr->loc_uncertainty_a = 0xFF;
    pos_data_ptr->loc_uncertainty_p = 0xFF;
    pos_data_ptr->loc_uncertainty_ang = 0;

  }

  else if ( q_shape_code  == C_POS_SHAPE_ELLIP_UNC_CIR )
  {
    uint8  u_UncK, u_IS801; 

    pos_data_ptr->opt_field_mask |= PDSM_PD_UNCERTAINTY_VALID;
    pos_data_ptr->loc_uncertainty_conf = 0; /* 0: no information */
    pos_data_ptr->fix_type = 0; /* no altitude  */

    u_UncK = *p_bssmap_le_location_estimate++; 
    f_unc_meters = tm_umts_common_utils_GADK_to_meter ( TRUE, u_UncK );
    u_IS801 = (uint8)tm_util_select_pos_unc ( f_unc_meters );

    pos_data_ptr->loc_uncertainty_a = u_IS801;
    pos_data_ptr->loc_uncertainty_p = u_IS801;
    pos_data_ptr->loc_uncertainty_ang = 0;

    /* ext_pos_data.alongAxisUnc = (uint16)f_unc_meters */
    pos_data_ptr->ext_pos_data.alongAxisUnc = 
      (uint16)f_unc_meters;
    pos_data_ptr->ext_pos_data.perpAxisUnc = 
      (uint16)f_unc_meters;


  } /* close-if-C_POS_SHAPE_ELLIP_UNC_CIR */

  else if ( q_shape_code == C_POS_SHAPE_ELLIP_UNC_ELLIP )
  {
    uint8  u_MajorUncK, u_MinorUncK, u_UncAngle;
    uint8  u_IS801Major, u_IS801Minor, u_IS801Angle;

    pos_data_ptr->opt_field_mask |= PDSM_PD_UNCERTAINTY_VALID;
    pos_data_ptr->fix_type = 0; /* no altitude  */
  
    /* we get major axis unc, minor axis unc, and angle from North
     * of major axis (0 to 178 degrees). */
    u_MajorUncK = *p_bssmap_le_location_estimate++; /* Major axis unc */
    u_MinorUncK = *p_bssmap_le_location_estimate++; /* Minor axis unc */
    
    /* Angle from North of Major axis: 
     *    2N < angle < 2(N+1) 
     * where N = 0..89
     */
    u_UncAngle  = *p_bssmap_le_location_estimate++ << 1 ; 
    
    f_major_unc_meters =  tm_umts_common_utils_GADK_to_meter( TRUE, u_MajorUncK );
    f_minor_unc_meters =  tm_umts_common_utils_GADK_to_meter( TRUE, u_MinorUncK );
    u_IS801Major = (uint8)tm_util_select_pos_unc( f_major_unc_meters );
    u_IS801Minor = (uint8)tm_util_select_pos_unc( f_minor_unc_meters );
    
    if( u_UncAngle > (2*89) )
    {
      u_UncAngle = 2*89;
    }
      
    if( u_UncAngle >= 90 ) 
    {
      /* Major is perp axis, and minor is along axis.
       * Subtract 90 from angle */
      u_UncAngle -= 90; 
       
      pos_data_ptr->loc_uncertainty_a = u_IS801Minor;
      pos_data_ptr->loc_uncertainty_p = u_IS801Major;

      /* ext_pos_data.alongAxisUnc = (uint16)ext_pos_data_ptr->ext_loc_uncrtnty_a */
      pos_data_ptr->ext_pos_data.alongAxisUnc = 
        (uint16)f_minor_unc_meters;
      pos_data_ptr->ext_pos_data.perpAxisUnc = 
        (uint16)f_major_unc_meters;

    }
    else 
    {
      /* Major is along axis, and minor is perp axis. */
      pos_data_ptr->loc_uncertainty_a = u_IS801Major;
      pos_data_ptr->loc_uncertainty_p = u_IS801Minor;

      /* ext_pos_data.alongAxisUnc = (uint16)ext_pos_data_ptr->ext_loc_uncrtnty_a */
      pos_data_ptr->ext_pos_data.alongAxisUnc = 
        (uint16)f_major_unc_meters;
      pos_data_ptr->ext_pos_data.perpAxisUnc = 
        (uint16)f_minor_unc_meters;

    }

    /* The angle is defined as follows in IS801.
     * LOC_UNCRTNTY_ANG – Angle of axis with respect to True North for 
     * position uncertainty. The mobile station shall set this field to 
     * the angle of axis for position uncertainty, in units of 5.625 
     * degrees, in the range from 0 to 84.375 degrees, where 0 degrees 
     * is True North and the angle increases toward the East.  
     *
     * The coded value of the range is hence from 0 to 15.  We'll 
     * interpret this as:
     *
     *    N * 5.625 <= angle < (N+1) * 5.625
     *
     * where N is the coded value, since this would be the most logical way 
     * of dividing up the coded space EVENLY.  This is therefore just a 
     * simple truncation.
     */
    u_IS801Angle = (uint8)( (float)u_UncAngle / (float)5.625 );
    
    /* With the limit checking above, the u_UncAngle is guaranteed to
     * be LESS than 90, so the u_IS801Angle is in the correct range 
     * of 0..15.  No further limit checking needed. */
  
    pos_data_ptr->loc_uncertainty_ang = u_IS801Angle;

    /* confidence */
    pos_data_ptr->loc_uncertainty_conf = *p_bssmap_le_location_estimate;

  } /* close if-C_POS_SHAPE_ELLIP_UNC_ELLIP */

  else if ( q_shape_code == C_POS_SHAPE_ELLIP_ALT )
  {
    uint32 q_long_word1;

    pos_data_ptr->opt_field_mask |= PDSM_PD_ALTITUDE_VALID;
    pos_data_ptr->loc_uncertainty_conf = 0; /* 0: no information */
    pos_data_ptr->fix_type = TRUE; /* with altitude  */
    *pd_event_ptr |= PDSM_PD_EVENT_HEIGHT;

    /* for older versions of applications that don't read unc_valid bit
       of opt_field_mask, make sure at least they get max. unc. */ 
    pos_data_ptr->loc_uncertainty_a = 0xFF;
    pos_data_ptr->loc_uncertainty_p = 0xFF;
    pos_data_ptr->loc_uncertainty_ang = 0;

    q_long_word1 = (uint32)*p_bssmap_le_location_estimate++ << 8;
    q_long_word1 |= (uint32)*p_bssmap_le_location_estimate++;

    if ( q_long_word1 & 0x00008000 )
    {
      pos_data_ptr->altitude = -(int16)(q_long_word1 & 0x00007FFF); /* signed, in meter */
    }
    else
    {
      pos_data_ptr->altitude = (int16)(q_long_word1 & 0x00007FFF); /* signed, in meter */
    }

  } /* close if-C_POS_SHAPE_ELLIP_ALT */

  else if ( q_shape_code == C_POS_SHAPE_ELLIP_ALT_UNC_ELLIP )
  {
    uint32 q_long_word1;
    uint8  u_MajorUncK, u_MinorUncK, u_UncAngle;
    uint8  u_IS801Major, u_IS801Minor, u_IS801Angle;

    pos_data_ptr->opt_field_mask |= PDSM_PD_ALTITUDE_VALID | 
                                    PDSM_PD_UNCERTAINTY_VALID;

    pos_data_ptr->fix_type = TRUE; /* with altitude  */

    *pd_event_ptr |= PDSM_PD_EVENT_HEIGHT;

    q_long_word1 = (uint32)*p_bssmap_le_location_estimate++ << 8;
    q_long_word1 |= (uint32)*p_bssmap_le_location_estimate++;

    if ( q_long_word1 & 0x00008000 )
    {
      pos_data_ptr->altitude = -(int16)(q_long_word1 & 0x00007FFF); /* signed, in meter */
    }
    else
    {
      pos_data_ptr->altitude = (int16)(q_long_word1 & 0x00007FFF); /* signed, in meter */
    }

    /* we get major axis unc, minor axis unc, and angle from North
     * of major axis (0 to 178 degrees). */
    u_MajorUncK = *p_bssmap_le_location_estimate++; /* Major axis unc */
    u_MinorUncK = *p_bssmap_le_location_estimate++; /* Minor axis unc */
    
    /* Angle from North of Major axis: 
     *    2N < angle < 2(N+1) 
     * where N = 0..89
     */
    u_UncAngle  = *p_bssmap_le_location_estimate++ << 1 ; 

    f_major_unc_meters =  tm_umts_common_utils_GADK_to_meter( TRUE, u_MajorUncK );
    f_minor_unc_meters =  tm_umts_common_utils_GADK_to_meter( TRUE, u_MinorUncK );
    u_IS801Major = (uint8)tm_util_select_pos_unc( f_major_unc_meters );
    u_IS801Minor = (uint8)tm_util_select_pos_unc( f_minor_unc_meters );
    
    if( u_UncAngle > (2*89) )
    {
      u_UncAngle = 2*89;
    }
      
    if( u_UncAngle >= 90 ) 
    {
      /* Major is perp axis, and minor is along axis.
       * Subtract 90 from angle */
      u_UncAngle -= 90; 
       
      pos_data_ptr->loc_uncertainty_a = u_IS801Minor;
      pos_data_ptr->loc_uncertainty_p = u_IS801Major;

      /* ext_pos_data.alongAxisUnc = (uint16)ext_pos_data_ptr->ext_loc_uncrtnty_a */
      pos_data_ptr->ext_pos_data.alongAxisUnc = 
        (uint16)f_minor_unc_meters;
      pos_data_ptr->ext_pos_data.perpAxisUnc = 
        (uint16)f_major_unc_meters;

    }
    else 
    {
      /* Major is along axis, and minor is perp axis. */
      pos_data_ptr->loc_uncertainty_a = u_IS801Major;
      pos_data_ptr->loc_uncertainty_p = u_IS801Minor;

      /* ext_pos_data.alongAxisUnc = (uint16)ext_pos_data_ptr->ext_loc_uncrtnty_a */
      pos_data_ptr->ext_pos_data.alongAxisUnc = 
        (uint16)f_major_unc_meters;
      pos_data_ptr->ext_pos_data.perpAxisUnc = 
        (uint16)f_minor_unc_meters;

    }

    /* The angle is defined as follows in IS801.
     * LOC_UNCRTNTY_ANG – Angle of axis with respect to True North for 
     * position uncertainty. The mobile station shall set this field to 
     * the angle of axis for position uncertainty, in units of 5.625 
     * degrees, in the range from 0 to 84.375 degrees, where 0 degrees 
     * is True North and the angle increases toward the East.  
     *
     * The coded value of the range is hence from 0 to 15.  We'll 
     * interpret this as:
     *
     *    N * 5.625 <= angle < (N+1) * 5.625
     *
     * where N is the coded value, since this would be the most logical way 
     * of dividing up the coded space EVENLY.  This is therefore just a 
     * simple truncation.
     */
    u_IS801Angle = (uint8)( (FLT)u_UncAngle / (FLT)5.625 );
    
    /* With the limit checking above, the u_UncAngle is guaranteed to
     * be LESS than 90, so the u_IS801Angle is in the correct range 
     * of 0..15.  No further limit checking needed. */
  
    pos_data_ptr->loc_uncertainty_ang = u_IS801Angle;

    /* vertical uncertainty */
    pos_data_ptr->loc_uncertainty_v = (uint8)tm_util_select_pos_unc( tm_umts_common_utils_GADK_to_meter( FALSE, *p_bssmap_le_location_estimate++ ) );

    /* confidence */
    pos_data_ptr->loc_uncertainty_conf = *p_bssmap_le_location_estimate;



  } /* close if-C_POS_SHAPE_ELLIP_ALT */

  else if ( q_shape_code == C_POS_SHAPE_ELLIP_ARC )
  {

      uint16 w_inner_radius_indx;
      uint8 u_unc_radius_indx;
      //uint16 w_offset_angle, w_included_angle;
      float f_inner_radius_meter, f_unc_radius_meter;

      pos_data_ptr->opt_field_mask = PDSM_PD_UNCERTAINTY_VALID;
      pos_data_ptr->fix_type = 0; /* no altitude  */

      w_inner_radius_indx = (uint16)*p_bssmap_le_location_estimate++;
      w_inner_radius_indx <<= 8;
      w_inner_radius_indx |= (uint16)*p_bssmap_le_location_estimate++; 
      f_inner_radius_meter = (float)w_inner_radius_indx * (FLT)5.0;

      u_unc_radius_indx = *p_bssmap_le_location_estimate++;
      u_unc_radius_indx &= 0x7F;
      f_unc_radius_meter = (float)u_unc_radius_indx * (FLT)5.0;

      //w_offset_angle = *p_bssmap_le_location_estimate++;
      //w_offset_angle *= 2;

      //w_included_angle = *p_bssmap_le_location_estimate++;
      //w_included_angle *= 2;

      /* propagate unc. and re-draw a circle */
      f_unc_meters = f_inner_radius_meter + 0.5 * f_unc_radius_meter;
      pos_data_ptr->loc_uncertainty_a = 
        (uint8)tm_util_select_pos_unc(f_unc_meters);

      pos_data_ptr->loc_uncertainty_p = pos_data_ptr->loc_uncertainty_a;

      /* ext_pos_data.alongAxisUnc = (uint16)ext_pos_data_ptr->ext_loc_uncrtnty_a */
      pos_data_ptr->ext_pos_data.alongAxisUnc = 
        (uint16)f_unc_meters;
      pos_data_ptr->ext_pos_data.perpAxisUnc = 
        (uint16)f_unc_meters;

      pos_data_ptr->loc_uncertainty_ang = 0;

      pos_data_ptr->loc_uncertainty_conf = *p_bssmap_le_location_estimate++;


  } /* close if-C_POS_SHAPE_ELLIP_ARC */

  if ( pos_data_ptr->opt_field_mask & PDSM_PD_UNCERTAINTY_VALID)
  {
    pos_data_ptr->ext_pos_data.loc_unc_horizontal = (uint32)sqrt((pos_data_ptr->ext_pos_data.alongAxisUnc *
                                                                  pos_data_ptr->ext_pos_data.alongAxisUnc) +
                                                                 (pos_data_ptr->ext_pos_data.perpAxisUnc *
                                                                  pos_data_ptr->ext_pos_data.perpAxisUnc) );
  }
  pos_data_ptr->positioning_source = pos_source;
  pos_data_ptr->position_type = pos_type;
  pos_data_ptr->position_mode = pos_mode;

  pos_data_ptr->meas_data = NULL;
  pos_data_ptr->pm_data = NULL;

  pos_data_ptr->altitude += 500; /* BREW required 500m offset */

  return TRUE;

} 


/*===========================================================================

FUNCTION tm_umts_common_utils_gad_to_ref_loc

DESCRIPTION
  Convert position information in BSSMP-LE GAD shapes to that of 
  gps_RefLocStructType used by MGP

Parameters:

  p_bssmap_le_location_estimate: pointer to the GAD shape.

  q_time_stamp_sec : full GPS second of the position 
  p_ref_loc        : Structure to be filled in by this routine

===========================================================================*/

static uint32  get_big_endian_24( const uint8 **pp_bytes )
{
  uint32        q_temp;
  const uint8   *p_bytes = *pp_bytes;

  q_temp = (((uint32) *p_bytes) << 24);
  p_bytes++;
  q_temp |= (((uint32) *p_bytes) << 16);
  p_bytes++;
  q_temp |= (((uint32) *p_bytes) << 8);
  p_bytes++;

  *pp_bytes = p_bytes;

  return q_temp;
}

static uint16  get_big_endian_16( const uint8 **pp_bytes )
{
  uint16        w_temp;
  const uint8   *p_bytes = *pp_bytes;

  w_temp = (((uint32) *p_bytes) << 8);
  p_bytes++;
  w_temp |= (((uint32) *p_bytes) << 0);
  p_bytes++;

  *pp_bytes = p_bytes;

  return w_temp;
}

boolean tm_umts_common_utils_gad_to_ref_loc
( 
  const uint8           *p_bssmap_le_location_estimate, 
  uint32                q_time_stamp_sec,
  gps_RefLocStructType  *p_ref_loc
)
{
  uint8   u_shape_code;
  uint8   u_octet1_low_nibble;
  uint8   u_MajorUncK, u_MinorUncK, u_UncAngle, u_AltUncK;
  uint8   u_UncK;
  uint32  q_GpsWeek;
  uint32  q_GpsSec;
  uint32  q_temp;
  uint16  w_temp;
  DBL     d_temp;
  FLT     f_temp;

  /*
   *  Initialize the return structure to 0.  This sets all validity
   *  flags to FALSE and all integer and FLT/DBL numbers to 0.
   */
  memset(p_ref_loc, 0, sizeof( *p_ref_loc ) );

  /*
   *  The first byte of the GAD shape indicates the type of GAD shape
   */
  u_shape_code = *p_bssmap_le_location_estimate++;
  
  u_octet1_low_nibble = u_shape_code & 0x0F;
  u_shape_code >>= 4;

  /*
   *  Populate GPS Time
   */
  q_GpsWeek = (q_time_stamp_sec / SEC_PER_WEEK);
  q_GpsSec = q_time_stamp_sec - (q_GpsWeek * SEC_PER_WEEK);

  p_ref_loc->w_GpsWeek = (uint16) q_GpsWeek;
  p_ref_loc->q_GpsMsec = q_GpsSec * 1000;

  /*
   *  Initialize uncertainty to MAX values "just in case"
   */
  p_ref_loc->f_LocUncrtntySemiMajor = MAX_UNCERTAINTY;
  p_ref_loc->f_LocUncrtntySemiMinor = MAX_UNCERTAINTY;
  p_ref_loc->f_AltitudeUnc          = MAX_UNCERTAINTY;
 
  /*
   *  LATITUDE and LONGITUDE
   */
  if ( u_shape_code != C_POS_SHAPE_POLYGON )
  {
    /*
     *  GAD shape latitude LSB is 90/2^23 degrees.  Convert into
     *  a double precision float in radians.
     */
    q_temp = get_big_endian_24( &p_bssmap_le_location_estimate );

    if( q_temp & 0x80000000 )
    {
      d_temp = (DBL) ((int32) (q_temp & 0x7FFFFFFF) * -1);
    }
    else
    {
      d_temp = (DBL) q_temp;
    }

    p_ref_loc->d_Latitude = d_temp * LATITUDE_SCALING * DEG_TO_RADIANS;

    /*
     *  GAD shape longitude LSB is 180/2^23 degrees.  Convert into
     *  a double precision float in radians.
     */
    q_temp = get_big_endian_24( &p_bssmap_le_location_estimate );

    d_temp = (DBL) ((int32) q_temp);

    p_ref_loc->d_Longitude = d_temp * LONGITUDE_SCALING * DEG_TO_RADIANS;
  }
  else
  {
    uint8 u_i;
    DBL   d_avg_lat = 0.0;
    DBL   d_avg_long = 0.0;

    if ( u_octet1_low_nibble == 0 )
    {
      MSG_HIGH("MOLR-RR-Polygon 0 pnt", 0, 0, 0);
      return FALSE;
    }
    else if ( ( u_octet1_low_nibble == 1 ) || ( u_octet1_low_nibble == 2 ) )
    {
      MSG_MED("MOLR-RR-Polygon pnt %u", u_octet1_low_nibble, 0, 0);
    }
    else if ( u_octet1_low_nibble > 3 )
    {
      /* QQQ: polygon N>3, use MOLR-ReturnResult with AdditionalPosition */
      MSG_HIGH("MOLR-RRpolygon>3 - forcing 3", 0, 0, 0);
      u_octet1_low_nibble = 3;
    }   

    for (u_i=0; u_i<u_octet1_low_nibble; u_i++)
    {
      q_temp = get_big_endian_24( &p_bssmap_le_location_estimate );

      if( q_temp & 0x80000000 )
      {
        d_temp = (DBL) (((int32) (q_temp & 0x7FFFFFFF)) * -1);
      }
      else
      {
        d_temp = (DBL) q_temp;
      }
      
      d_avg_lat += d_temp;

      q_temp = get_big_endian_24( &p_bssmap_le_location_estimate );

      d_temp = (DBL) ((int32) q_temp);

      d_avg_long  +=  d_temp;
    } /* end of for-loop for N-point polygon */

    /*
     *  Calculate the average Latitude and Longitude.
     */   
    d_avg_lat /= (DBL) (u_octet1_low_nibble);
    d_avg_long /= (DBL) (u_octet1_low_nibble);

    /*
     *  Scale latitude from units of 90/2^31 degrees into Radians
     *  Scale longitude from units of 180/2^31 degrees into Radians
     */ 
    d_avg_lat *= LATITUDE_SCALING * DEG_TO_RADIANS;
    d_avg_long *= LONGITUDE_SCALING * DEG_TO_RADIANS;

    p_ref_loc->d_Latitude = d_avg_lat;
    p_ref_loc->d_Longitude = d_avg_long;
  }

  /*
   *  Shape specific handling of other (Non lat/lon) items
   *  such as position uncertainty and altitude.
   */
  if ( u_shape_code == C_POS_SHAPE_ELLIP_UNC_CIR )
  {
    u_UncK = *p_bssmap_le_location_estimate++; 

    f_temp = (FLT) tm_umts_common_utils_GADK_to_meter( TRUE, u_UncK );
    
    p_ref_loc->f_LocUncrtntyAng       = 0.0;
    p_ref_loc->f_LocUncrtntySemiMajor = f_temp;
    p_ref_loc->f_LocUncrtntySemiMinor = f_temp;
  }
  else if ( u_shape_code == C_POS_SHAPE_ELLIP_UNC_ELLIP )
  {
    /*
     *  We get major axis unc., minor axis unc., and angle from North
     *  of major axis (0 to 178 degrees).
     */
    u_MajorUncK = *p_bssmap_le_location_estimate++;
    u_MinorUncK = *p_bssmap_le_location_estimate++;
    
    /*
     *  Angle from North of Major axis: 
     *    2N < angle < 2(N+1) 
     *  where N = 0..89
     */
    u_UncAngle  = *p_bssmap_le_location_estimate++ << 1 ; 
    
    if( u_UncAngle > (2*89) )
    {
      u_UncAngle = 2*89;
    }
    
    p_ref_loc->f_LocUncrtntyAng = (FLT) u_UncAngle;
    
    p_ref_loc->f_LocUncrtntySemiMajor =
          (FLT) tm_umts_common_utils_GADK_to_meter( TRUE, u_MajorUncK );
    
    p_ref_loc->f_LocUncrtntySemiMinor =
          (FLT) tm_umts_common_utils_GADK_to_meter( TRUE, u_MinorUncK );
  }
  else if ( u_shape_code == C_POS_SHAPE_ELLIP_ALT )
  {
    w_temp = get_big_endian_16( &p_bssmap_le_location_estimate );

    p_ref_loc->u_AltitudeIsValid = TRUE;
    
    if ( w_temp & 0x8000 )
    {
      p_ref_loc->f_Altitude = -(FLT)(w_temp & 0x7FFF);
    }
    else
    {
      p_ref_loc->f_Altitude = (FLT)(w_temp & 0x7FFF);
    }
  }
  else if ( u_shape_code == C_POS_SHAPE_ELLIP_ALT_UNC_ELLIP )
  {
    w_temp = get_big_endian_16( &p_bssmap_le_location_estimate );

    p_ref_loc->u_AltitudeIsValid = TRUE;

    if ( w_temp & 0x8000 )
    {
      p_ref_loc->f_Altitude = -(FLT)(w_temp & 0x7FFF);
    }
    else
    {
      p_ref_loc->f_Altitude = (FLT)(w_temp & 0x7FFF);
    }

    u_MajorUncK = *p_bssmap_le_location_estimate++;
    u_MinorUncK = *p_bssmap_le_location_estimate++;
    
    /* Angle from North of Major axis: 
     *    2N < angle < 2(N+1) 
     * where N = 0..89
     */
    u_UncAngle  = *p_bssmap_le_location_estimate++ << 1 ; 
    
    if( u_UncAngle > (2*89) )
    {
      u_UncAngle = 2*89;
    }
    
    p_ref_loc->f_LocUncrtntyAng = (FLT) u_UncAngle;
    
    p_ref_loc->f_LocUncrtntySemiMajor =
           (FLT) tm_umts_common_utils_GADK_to_meter( TRUE, u_MajorUncK );

    p_ref_loc->f_LocUncrtntySemiMinor =
           (FLT) tm_umts_common_utils_GADK_to_meter( TRUE, u_MinorUncK );
 
    u_AltUncK = *p_bssmap_le_location_estimate++;

    p_ref_loc->f_AltitudeUnc =
           (FLT) tm_umts_common_utils_GADK_to_meter( FALSE, u_AltUncK );
  }
  else if ( u_shape_code == C_POS_SHAPE_ELLIP_ARC )
  {
    uint16  w_inner_radius_indx;
    uint8   u_unc_radius_indx;
    uint16  w_offset_angle, w_included_angle;
    float   f_inner_radius_meter, f_unc_radius_meter;

    w_inner_radius_indx = (uint16)*p_bssmap_le_location_estimate++;
    w_inner_radius_indx <<= 8;
    w_inner_radius_indx |= (uint16)*p_bssmap_le_location_estimate++; 
    f_inner_radius_meter = (float)w_inner_radius_indx * (FLT)5.0;

    u_unc_radius_indx = *p_bssmap_le_location_estimate++;
    u_unc_radius_indx &= 0x7F;
    f_unc_radius_meter = (float)u_unc_radius_indx * (FLT)5.0;

    w_offset_angle = *p_bssmap_le_location_estimate++;
    w_offset_angle *= 2;

    w_included_angle = *p_bssmap_le_location_estimate++;
    w_included_angle *= 2;
      
    p_ref_loc->f_LocUncrtntyAng = 
                    (FLT) (w_offset_angle) + (((FLT) w_included_angle) / 2.0);
    if( p_ref_loc->f_LocUncrtntyAng >= 180.0 )
      p_ref_loc->f_LocUncrtntyAng -= 360.0;

    p_ref_loc->f_LocUncrtntyAng *= DEG_TO_RADIANS;
    
    p_ref_loc->f_LocUncrtntySemiMajor =
           (FLT) f_inner_radius_meter + 0.5 * f_unc_radius_meter;

    p_ref_loc->f_LocUncrtntySemiMinor = p_ref_loc->f_LocUncrtntySemiMajor;
  }

  return TRUE;
} 

#ifdef FEATURE_CGPS_DUAL_WCDMA
/********************************************************************
 *
 * tm_umts_common_utils_mm_session_status_notify
 *
 * Function description:
 *  This function notifies the MM layer when of the beginning / end of
 *  a positioning session. MM uses this information to decide whether
 *  or not to tell LSM of network events which may require the termination
 *  of an ongoing session
 *
 * Parameters:
 *
 *  started - If true, indicates a session has started, otherwise indicates
 *          that a session has ended.
 *  e_as_id - The cached sub id for this session to be used when calling RRC API
 *		"rrcscr_get_signalling_connection_status_sub"	
 *
 *
 * Return:
 *   None
 *
 *********************************************************************
 */
void tm_umts_common_utils_mm_session_status_notify( uint8 started, sys_modem_as_id_e_type e_as_id )
{
  #ifdef FEATURE_CGPS_UMTS_CP

  lcs_active_ind_T lcs_active_ind;
  static uint8 current_started_value = FALSE;

   /* 
   * Only send a message if UE isn't in PS domain. Because if UE is in PS 
   * domain MM will be in idle state which would cause MM to send us an abort 
   * indication if it received a message from TM.
   */
  #ifdef FEATURE_CGPS_WCDMA
  if (gnss_wwan_iface_rrcscr_get_signalling_connection_status_sub (RRC_PS_DOMAIN_CN_ID, e_as_id))
  {
    MSG_MED("Notify MM of LCS start while in CS", 0, 0, 0);
    // return;
  }
  #endif /* FEATURE_CGPS_WCDMA */

  /* Only send a message to MM if the current state has changed */
  if ( started != current_started_value )
  {
    current_started_value = started;

    /* Format inter-task message to MM layer */
    lcs_active_ind.message_header.message_set = MS_MM_LSM;
    lcs_active_ind.message_header.message_id = LCS_ACTIVE_IND;
    lcs_active_ind.lcs_in_progress = started;

    PUT_IMH_LEN
    (
      sizeof(lcs_active_ind_T) - sizeof(IMH_T), 
      &lcs_active_ind.message_header 
    ) ;

    (void) gs_send_message( GS_QUEUE_MM, (void *)&lcs_active_ind, TRUE );
  }

  #endif /*  FEATURE_CGPS_UMTS_CP */

}
#else
/********************************************************************
 *
 * tm_umts_common_utils_mm_session_status_notify
 *
 * Function description:
 *  This function notifies the MM layer when of the beginning / end of
 *  a positioning session. MM uses this information to decide whether
 *  or not to tell LSM of network events which may require the termination
 *  of an ongoing session
 *
 * Parameters:
 *
 *  started - If true, indicates a session has started, otherwise indicates
 *          that a session has ended.
 *
 *
 * Return:
 *   None
 *
 *********************************************************************
 */
void tm_umts_common_utils_mm_session_status_notify( uint8 started )
{
  #ifdef FEATURE_CGPS_UMTS_CP

  lcs_active_ind_T lcs_active_ind;
  static uint8 current_started_value = FALSE;

   /* 
   * Only send a message if UE isn't in PS domain. Because if UE is in PS 
   * domain MM will be in idle state which would cause MM to send us an abort 
   * indication if it received a message from TM.
   */
  #ifdef FEATURE_CGPS_WCDMA
  if (gnss_wwan_iface_rrcscr_get_signalling_connection_status (RRC_PS_DOMAIN_CN_ID))
  {
    MSG_MED("Notify MM of LCS start while in CS", 0, 0, 0);
    // return;
  }
  #endif /* FEATURE_CGPS_WCDMA */

  /* Only send a message to MM if the current state has changed */
  if ( started != current_started_value )
  {
    current_started_value = started;

    /* Format inter-task message to MM layer */
    lcs_active_ind.message_header.message_set = MS_MM_LSM;
    lcs_active_ind.message_header.message_id = LCS_ACTIVE_IND;
    lcs_active_ind.lcs_in_progress = started;

    PUT_IMH_LEN
    (
      sizeof(lcs_active_ind_T) - sizeof(IMH_T), 
      &lcs_active_ind.message_header 
    ) ;

    (void) gs_send_message( GS_QUEUE_MM, (void *)&lcs_active_ind, TRUE );
  }

  #endif /*  FEATURE_CGPS_UMTS_CP */

}
#endif

/********************************************************************
 *
 * tm_umts_common_utils_rr_session_status_notify
 *
 * Function description:
 *  This function notifies the RR layer when of the beginning / end of
 *  a positioning session. RR uses this information to decide whether
 *  or not to tell TM of network events which may require the termination
 *  of an ongoing session
 *
 * Parameters:
 *
 *  started - If true, indicates a session has started, otherwise indicates
 *          that a session has ended.
 *
 * as_id - Indicates the sub related for this rr session
 *
 * Return:
 *   None
 *
 *********************************************************************
 */
void tm_umts_common_utils_rr_session_status_notify( uint8 started, sys_modem_as_id_e_type  as_id )
{
  #ifdef FEATURE_CGPS_UMTS_CP_GSM

  rr_cgps_event_req_T rr_tm_event_req;

  /* Format inter-task message to RR layer */
  rr_tm_event_req.message_header.message_set = MS_CGPS_RR;
  rr_tm_event_req.message_header.message_id = RR_CGPS_EVENT_REQ;
  rr_tm_event_req.send_rr_event = started;
  rr_tm_event_req.as_id = as_id;

  PUT_IMH_LEN
  (
    sizeof(rr_cgps_event_req_T) - sizeof(IMH_T), 
    &rr_tm_event_req.message_header 
  ) ;

  (void) gs_send_message( GS_QUEUE_RR, (void *)&rr_tm_event_req, TRUE );

  #endif   /* FEATURE_CGPS_UMTS_CP_GSM */

}

/********************************************************************
*
* tm_umts_common_send_pdapi_comm_event
*
* Function description:
*   Send pdapi comm events through Tmcore
*
* Parameters: 
*
*   comm_event: part of pdsm_pd_event_type that are relevant to
*               comm.
*
*   tm_active_session_handle: tm session handle 
*
* Return:
*  None
*  
*********************************************************************
*/

void tm_umts_common_send_pdapi_comm_event 
(
  pdsm_pd_comm_protocol_e_type comm_protocol,
   pd_event_comm_state_e_type      comm_event, 
   tm_sess_handle_type             tm_active_session_handle,
   pdsm_client_id_type             client_id
)
{
  pdsm_pd_info_s_type       z_pd_info_buffer;
  
  memset(&z_pd_info_buffer, 0, sizeof( pdsm_pd_info_s_type ) );

  z_pd_info_buffer.pd_info.comm_data.protocol_type = comm_protocol;
  z_pd_info_buffer.client_id = client_id;

  (void) tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                     tm_active_session_handle,
                     comm_event, 
                     &z_pd_info_buffer,
                     NULL);
}

/*===========================================================================
FUNCTION tm_umts_common_ni_client_start_session

DESCRIPTION
  This function is called to start SUPL NI session.

DEPENDENCIES 
  None.
  
Parameters: 
  e_op_mode: const struct tm_sess_operation_mode_e_type indicating TM Session Operation Mode
  u_accuracy_threshold: const uint8 Accuracy Threshold
  gps_session_timeout: const byte indicating gps_session_timeout
  e_mt_initiator: const struct pdsm_mt_initiator_e_type indicating initiator of get_pos

RETURN VALUE
  TRUE/FALSE
  
SIDE EFFECTS
 None

===========================================================================*/
boolean tm_umts_common_ni_client_start_session
(  const tm_sess_operation_mode_e_type    e_op_mode, 
   const uint8                            u_accuracy_threshold,
   const byte                             gps_session_timeout,
   const dword                            num_fixes,
   const dword                            time_between_fixes,
   const pdsm_mt_initiator_e_type         e_mt_initiator,
   boolean                                u_internal_session
)
{
  boolean                              ret_val = TRUE;
  tm_pdapi_get_pos_param_s_type        z_pdapi_get_pos_param;
  pdsm_pd_session_operation_e_type     e_pdsm_pd_session_operation;

  if ( e_op_mode == TM_SESS_OPERATION_MODE_MSB )
  {
    e_pdsm_pd_session_operation = PDSM_SESSION_OPERATION_MSBASED;
  }
  else if ( e_op_mode == TM_SESS_OPERATION_MODE_STANDALONE )
  {
    e_pdsm_pd_session_operation = PDSM_SESSION_OPERATION_STANDALONE_ONLY;
  }  
  else
  {
    e_pdsm_pd_session_operation = PDSM_SESSION_OPERATION_MSASSISTED;
  }    

  z_pdapi_get_pos_param.client_type = PDSM_CLIENT_TYPE_NI; 
  z_pdapi_get_pos_param.client_id = pdsm_ni_client_id_get(); 
  z_pdapi_get_pos_param.mt_initiator = e_mt_initiator;

  z_ni_client_pdsm_pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL; /* don't care */
  z_ni_client_pdsm_pd_option.fix_rate.num_fixes = num_fixes;
  z_ni_client_pdsm_pd_option.fix_rate.time_between_fixes_ms = time_between_fixes * 1000;
  z_ni_client_pdsm_pd_option.session = PDSM_PD_SESS_TYPE_NEW;
  z_ni_client_pdsm_pd_option.operation_mode = e_pdsm_pd_session_operation;
  z_pdapi_get_pos_param.option_ptr = &z_ni_client_pdsm_pd_option;

  z_ni_client_pdsm_pd_qos.gps_session_timeout = gps_session_timeout;
  z_ni_client_pdsm_pd_qos.accuracy_threshold =  u_accuracy_threshold;
  z_pdapi_get_pos_param.qos_ptr = &z_ni_client_pdsm_pd_qos;

  z_pdapi_get_pos_param.internal_session = u_internal_session;

  if( tm_core_handle_get_pos(&z_pdapi_get_pos_param) )
  {      
    MSG_MED("NI client triggers get_pos successfully", 0, 0, 0);
    ret_val = TRUE;
  }
  else
  {
    MSG_MED("NI client fails to trigger get_pos", 0, 0, 0);
    ret_val = FALSE;
  }
      
  return ret_val;
}

/*===========================================================================
FUNCTION tm_umts_common_assist_wishlist_build

DESCRIPTION
  This function modifies the PE and TM-core provided wishlist to suit 
  UMTS protocol modules' design.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/

void tm_umts_common_assist_wishlist_build
( sm_InternalAssistDataStatusStructType *p_assist_data_status )
{

  /* if PE knows no time, or time unc. is huge, request everything */
  if ( (p_assist_data_status->w_GpsWeek == C_GPS_WEEK_UNKNOWN) ||
       !p_assist_data_status->z_TimeUnc.u_Valid
     )
  {
    MSG_MED("Time unknown - GPS wishlist=%lu, GpsWeek=%u, TimeUncValid=%u", 
            p_assist_data_status->q_WishListMask, 
            p_assist_data_status->w_GpsWeek, 
            p_assist_data_status->z_TimeUnc.u_Valid);
  }

  else /* when time is known to PE, check ref.Loc and refTime requests */
  {

    /* reference position: if there is no ref. pos. in PE, WishListMask will indicate
       the need for ref. pos.; if there is ref. pos. in PE, need to check its unc. here
       to determine if a request for ref. pos. is to be sent to the protocol submodule. */

    if ( !( p_assist_data_status->q_WishListMask & C_GPS_ASSIS_REFLOC ) )
    {
      if ( p_assist_data_status->z_PosUnc.f_PosUnc > (float)( z_umts_nv_item_ram_copy.aagps_default_ref_position_unc * 3 ) )
      {
        p_assist_data_status->q_WishListMask |= C_GPS_ASSIS_REFLOC;

        /* check if the last reference position was given in the same cell with
         cell info. the same as the current cell */
        if ( cell_info_unchanged_for_refloc_ck() )
        {
          p_assist_data_status->q_WishListMask &= ~C_GPS_ASSIS_REFLOC;

          /* no need to req. ref. loc., due to identical cell */
          if ( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
          {
            (void) snprintf( tm_f3_msg_buffer, C_TM_F3_MSG_BUF_SIZE, 
                             "no refL req: same cell; pUnc=%8.6f",
                             p_assist_data_status->z_PosUnc.f_PosUnc );
            MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", tm_f3_msg_buffer);
          }
        }
        else
        {
          if ( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
          {
            (void) snprintf( tm_f3_msg_buffer, C_TM_F3_MSG_BUF_SIZE, "refL needed pUnc.: %8.6f %8.6f",
                           p_assist_data_status->z_PosUnc.f_PosUnc,
                           (float)( z_umts_nv_item_ram_copy.aagps_default_ref_position_unc * 3 ));
            MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", tm_f3_msg_buffer);
          }  
        } 
      } 
    } 
    else /* PE reports no position at all */
    {

      /* check if the last reference position was given in the same cell with
       cell info. the same as the current cell */
      if ( cell_info_unchanged_for_refloc_ck() )
      {
        /* no need to req. ref. loc., due to identical cell */
        MSG_HIGH("ref loc req. suppressed due to identical cell", 0, 0, 0);

        if ( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
        {
          (void) snprintf( tm_f3_msg_buffer, C_TM_F3_MSG_BUF_SIZE, "Suppressed refLoc req due to same cell: would be needed pUnc=%8.6f",
                         p_assist_data_status->z_PosUnc.f_PosUnc );
          MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", tm_f3_msg_buffer);
        }  
      }
      else
      {
        p_assist_data_status->q_WishListMask |= C_GPS_ASSIS_REFLOC;

        if ( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
        {
          (void) snprintf( tm_f3_msg_buffer, C_TM_F3_MSG_BUF_SIZE, "refLoc needed due to PE flag.: pUnc=%8.6f",
                         p_assist_data_status->z_PosUnc.f_PosUnc );
          MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", tm_f3_msg_buffer);
        } 
      }
    }
      
 
    /* reference time: if PE demands reference time or, if the unc. is at lease 300ms greater
      than what the network-provided assistance ref. time has, and at least is
      10 sec., in line with PE, request should be sent to the protocol 
      submodule */
    if ( p_assist_data_status->q_WishListMask & C_GPS_ASSIS_REFTIME )
    {
     MSG_MED("refTime demanded by PE", 0, 0, 0);
    }
    else if ( ( ( p_assist_data_status->z_TimeUnc.f_TimeUnc > 
                 ( (float)(z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms * 50 ) + 300.f)
               ) && 
               ( p_assist_data_status->z_TimeUnc.f_TimeUnc > 10000 )
             ) ||
             ( p_assist_data_status->z_TimeUnc.u_Valid == FALSE )
           )
    {
     p_assist_data_status->q_WishListMask |= C_GPS_ASSIS_REFTIME;
     MSG_MED("refTime needed", 0, 0, 0);
    }
  } /* else if (PE does have time) */

}


/*===========================================================================
FUNCTION tm_umts_common_glo_assist_wishlist_build

DESCRIPTION
  This function modifies the PE and TM-core provided Glonass wishlist to suit 
  UMTS protocol modules' design.

DEPENDENCIES 
  None.

RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/

void tm_umts_common_glo_assist_wishlist_build
( sm_InternalAssistDataStatusStructType *p_assist_data_status )
{

  /* if PE knows no time, or time unc. is huge, request everything we need
     from GLO assistance */
  if ( (p_assist_data_status->w_GpsWeek == C_GPS_WEEK_UNKNOWN) ||
       !p_assist_data_status->z_TimeUnc.u_Valid
     )
  {
    MSG_MED("Time unknown - GLO wishlist=%lu, GpsWeek=%u, TimeUncValid=%u", 
            p_assist_data_status->q_WishListMask, 
            p_assist_data_status->w_GpsWeek, 
            p_assist_data_status->z_TimeUnc.u_Valid);
  }
  else
  {

    MSG_MED("PE provided GLO wishlist: 0x%lX -- will remove T/L/U if present", 
            p_assist_data_status->z_GloAssistData.q_WishListMask, 0, 0);
    /* the follow items are handled by the GPS assistance wishlist, will not duplicate */
    p_assist_data_status->z_GloAssistData.q_WishListMask &= 
      ~(C_GNSS_ASSIST_REFTIME | C_GNSS_ASSIST_REFLOC | C_GNSS_ASSIST_UTC);

  }

}


/*===========================================================================
FUNCTION tm_umts_common_next_int_log2_max_128

DESCRIPTION
  This function computes the smallest integer that is greater than log2(x) 
  for x up to 128.

DEPENDENCIES 
  None.

RETURN VALUE
  The smallest integer that is greater than log2(x). 
  For x = 128, returns 7.
    
SIDE EFFECTS
  None

===========================================================================*/

uint8 tm_umts_common_next_int_log2_max_128 ( uint8 u_num )
{
  uint8 u_i;

  if ( u_num > 128 )
  {
    u_num = 128;
  }

  u_i = 0;

  while ( u_pow_2[ u_i ] < u_num )
  {
    u_i++;

    if ( u_i >= 8 )
    {
      u_i = 7;
      break;
    }
  };
  
  return u_i;    
  
}


/*===========================================================================
FUNCTION tm_umts_common_srv_interact

DESCRIPTION
  This function is used to alert TM_UMTS the starting of a specific protocol
  session and decided if it needs to cancel any ongoing session. 

DEPENDENCIES 
  None.

RETURN VALUE
  TRUE: this session is granted; FALSE: reject this session request.
    
SIDE EFFECTS
  None

===========================================================================*/

boolean tm_umts_common_srv_interact( umts_session_type_e_type e_session_type )
{
  boolean ret_val = TRUE;
  tm_sess_req_param_u_type  req_param;
  
  req_param.stop_param.stop_type = TM_STOP_TYPE_SESSION; 
  req_param.stop_param.stop_reason = TM_STOP_REASON_USER_TERMINATED; 

  if ( e_session_type == C_UMTS_SESSION_WCDMA_CP_NI ||
       e_session_type == C_UMTS_SESSION_GSM_CP_NI )
  {
    #ifdef FEATURE_CGPS_UMTS_UP_SUPL
    /* if there is an on-going SUPL session, this will close it */
    if ( !tm_umts_up_supl_api_close_session(TM_STOP_REASON_USER_TERMINATED) )
    {
      /* if there has been no SUPL session that would have been already
         closed, ask TmCore to close any ongoing session */
      (void) tm_sess_req ( TM_PRTL_TYPE_UMTS_UP,
                    TM_SESS_HANDLE_FORCE_STOP,
                    TM_SESS_REQ_STOP, 
                    &req_param
                  ); 
    }
    #else
    (void) tm_sess_req ( TM_PRTL_TYPE_UMTS_UP,
                  TM_SESS_HANDLE_FORCE_STOP,
                  TM_SESS_REQ_STOP, 
                  &req_param
                ); 
    #endif
  } 
  
  return ret_val; 
}     

/*===========================================================================
FUNCTION tm_umts_common_utils_2s_comp_to_sign_mag

DESCRIPTION
  This function converts 2's complement to sign-magnitude.

DEPENDENCIES 
  None.

RETURN VALUE

  l_2s_comp: 32-bit 2's complement to be converted.
  u_sign_bit: a hex. one-hot indicator of sign bit in the sign magnitude.
    
SIDE EFFECTS
  None

===========================================================================*/

uint32 tm_umts_common_utils_2s_comp_to_sign_mag
(int32 l_2s_comp, uint32 q_sign_bit )
{
  uint32 q_sign_mag; 

  if ( l_2s_comp >= 0 )
  {
    q_sign_mag = (uint32)l_2s_comp;
  }
  else
  {
    q_sign_mag = (~l_2s_comp + 1) | q_sign_bit;
  }

  return q_sign_mag;

}

/*===========================================================================
FUNCTION tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal

DESCRIPTION
  This function converts 2's complement number to negative value (only if sign bit isn't zero).
  Then convert the negative value to a decimal number without adding negative sign in the front.
 
  Generally the input is from network/server & the final value is fed to MGP, where the negative sign
  is added to the front based on the sign bit.
 
  E.g.
  Input : -63 in the bit string form '100 0001b' (i.e. 65) with sign bit as '100 0000b'
  This function : Converts to negative value '1111 1111 1111 1111 1111 1111 1100 0001b'
                  Then 2's complement to get the decimal '111 1111' & ensure the sign bit is set.
 
DEPENDENCIES 
  None.
 
PARAMETERS
  uq2sComp: 32-bit 2's complement number to be converted.
  qSignBit: Sign bit indicator in the 2's complement value.
 
RETURN VALUE
  Return the decimal number when sign bit is TRUE (1), else return
  the same value.
    
SIDE EFFECTS
  None

===========================================================================*/

uint32 tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal( uint32 uq_2sComp, uint32 q_SignBit )
{
  uint32 uqSignMag = uq_2sComp;
  int32 qSignNum = 0;
  uint32 uBitStringLen = 0; /* Denotes bit string length (0..31) */

  /* Given number expressed as negative integer ? */
  if ( uq_2sComp & q_SignBit )
  {
    /* Find the sign bit position to know the bit string length */
    while ( (q_SignBit & (1L << uBitStringLen)) == 0 )
    {
      ++uBitStringLen;
    }

    /* Increment by 1 to denote bit pos from 1 to 32 */
    ++uBitStringLen;

    /* Convert to signed magnitude */
    qSignNum = ((int32)uq_2sComp) << ((sizeof(uq_2sComp) * 8) - uBitStringLen) >> ((sizeof(uq_2sComp) * 8) - uBitStringLen);
    /* 2's complement */
    uqSignMag = (~qSignNum + 1) | q_SignBit;
  }

  return uqSignMag;
}

/*
 ******************************************************************************
 *
 * tm_umts_common_map_glo_eph_svid_to_freq_num_from_alm 
 *
 * Function description:
 *  Associates SV Id & frequencey number for Glonass eph. data & Glo Almanac model respectively.
 *  
 * Parameters:
 *   gloAlmnA [IN] : Glonass Satellite/Slot ID in the range of 1..24
 *   gloAlmHA [IN] : Glonass channel frequency number. Only the 5-bit LSB (rightmost) is needed.
 *   p_ProtGloEphBuf [OUT] : pointer to rrc glonass eph. buffer
 * 
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/

void tm_umts_common_map_glo_eph_svid_to_freq_num_from_alm( uint8 const gloAlmnA, uint8 const gloAlmHA,
                                                           prot_glo_eph_buffer_s_type p_ProtGloEphBuf[N_GLO_SV] )
{
  /* gloAlmnA (1..24) - sv slot id */
  if ( (gloAlmnA < 1) || (gloAlmnA > N_GLO_SV) )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRC GLO Alm SvId invalid: %3u", gloAlmnA);
    return;
  }

  /* gloAlmnA is SV_ID :  gloAlmnA : INTEGER (1..24) */
  p_ProtGloEphBuf[gloAlmnA - 1].u_freq_num_valid = TRUE;

  /* gloAlmHA BIT STRING (SIZE (5)). Do a straight copy to the 5-bit LSB */
  p_ProtGloEphBuf[gloAlmnA - 1].z_eph_glo_data.u_FreqNum = gloAlmHA;

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "RRC GLO Alm. SV slot id %u to FreqNum %d mapped", gloAlmnA, gloAlmHA);
}


/*
 ******************************************************************************
 *
 * tm_umts_common_glonass_tod_mod_hour 
 *
 * Function description:
 *  Compute the Glonass Time of Day (TOD) modulo 1 hr from the Glonas TOD & after sanity check.
 *  
 * Parameters:
 *   u_GloNumFourYear [IN] : Glonass Satellite/Slot ID in the range of 1..24
 *   w_GloNumDaysInFourYear [IN] : Glonass channel frequency number. Only the 5-bit LSB (rightmost) is needed.
 *   q_GloTimeMs [OUT] : pointer to rrc glonass eph. buffer
 * 
 * Return value: 
 *   Glonass Time of Day (modulo 1 hr) in 0..3599999 milliseconds (rounded down to the nearest millisecond unit).
 *
 ******************************************************************************
*/
uint32 tm_umts_common_glonass_tod_mod_hour( uint8 const u_GloNumFourYear, uint16 const w_GloNumDaysInFourYear,
                                            uint32 const q_GloTimeMs )
{
  if ((u_GloNumFourYear == 255)||
      (w_GloNumDaysInFourYear == 65535)||
      (q_GloTimeMs > 86400000)
      )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "GlonassTOD invalid: Num4Year=%u, NumDaysIn4Year=%u, ToD_ms=%lu",
          u_GloNumFourYear, w_GloNumDaysInFourYear, q_GloTimeMs);
    return 0xFFFFFFFF;
  }
  else
  {
   return (q_GloTimeMs % HOUR_MSECS); 
  }
}

/*
 ******************************************************************************
 *
 * tm_umts_common_convert_velocity
 *
 * Function description:
 *  Converts the velocity values to a common format used to populate ASN.1
 *  
 * Parameters:
 *  Input: *f_vel_east - East Vel
 *         *f_vel_north - North Vel
 *         *f_vel_vertical - Vertical Vel
 *  Output:*w_hor_veloicty - Horizontal Velocity
 *          *s_bearing - Bearing value
 *          *b_vertical_velocity - Vertical Velocity
 *           *u_ver_direct - Vertical Velocity Direction
 *           *u_ver_velocity_bit_string,
 *
 * Return value: TRUE: if conversion is succesful , FALSE otherwise
 *
 ******************************************************************************
*/
void tm_umts_common_convert_velocity(float  *f_heading_ang,
                                     uint16 *w_hor_velocity,
                                     int16  *s_bearing,
                                     int8   *b_vertical_velocity,
                                     uint8  *u_ver_direct,
                                     uint8  *u_ver_velocity_bit_string,
                                     const float  *f_vel_east,
                                     const float  *f_vel_north,
                                     const float  *f_vel_vertical)
{
  float  f_temp                    = 0;

  if ( (f_heading_ang == NULL) ||
      (w_hor_velocity == NULL) ||
      (s_bearing == NULL) ||
      (b_vertical_velocity == NULL) ||
      (u_ver_direct == NULL) ||
      (u_ver_velocity_bit_string == NULL) ||
      (f_vel_east == NULL) ||
      (f_vel_north == NULL ) ||
      (f_vel_vertical == NULL) )
  {
    MSG_ERROR("TMCOMMON: Invalid Parameters", 0, 0, 0);
    return ;
  }

  /* horizontal velocity has unit of 1 km/hour */
  /* 1st get meters/sec */
  /* convert to km/hour */
  f_temp = sqrt(((*f_vel_east) * (*f_vel_east)) + ((*f_vel_north) * (*f_vel_north)));
  /* convert to km/hour */
  f_temp *= 3.6;

  *w_hor_velocity = (uint16)f_temp;

  /* heading: north = 0, clock-wise */
  if (*f_vel_north == 0)
  {
    if (*f_vel_east > 0.0) *f_heading_ang = 90.0f;
    else if  (*f_vel_east == 0.0f) *f_heading_ang = 0.0f;  /* not moving, but force it to 0 heading angle */
    else *f_heading_ang = 270.0f;
  }
  else if (*f_vel_east == 0)
  {
    if (*f_vel_north >= 0) *f_heading_ang = 0.0f;  /* incl. going north or not moving */
    else *f_heading_ang = 180.0f;
  }
  else
  {
    *f_heading_ang = (float)(atan2(*f_vel_east, *f_vel_north) * RAD2DEG);
  }
  if (*f_heading_ang < 0.0f)
  {
    *f_heading_ang += 360.0f;
  }
  *s_bearing = (int16)(*f_heading_ang);

  /* however, if we are reporting horizontal velocity 0, it makes
  no sense to have a non-zero heading degree */
  if (*w_hor_velocity == 0)
  {
    *s_bearing = 0;
  }

  /* convert to km/hour from m/s first. spec allows only uint8 */
  f_temp  = *f_vel_vertical * 3.6;

  if (f_temp > 255.0f)
  {
    *u_ver_velocity_bit_string = 255;
  }
  else
  {
    *u_ver_velocity_bit_string = (uint8)(fabs(f_temp));
  }
}
#endif /* ifdef FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS */
