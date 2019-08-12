#ifndef TDSRRCDATA_EXT_H
#define TDSRRCDATA_EXT_H

/*===========================================================================
              R R C D A T A  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  RRC Data
  
Copyright (c) 2010-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tdsrrcdataext.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
08/15/12   hx     Put HSDPA/UPA API functions for DATA team
06/08/12   hx     Create the external file to put into the api folder 
                  After testing, we may combine tdssrrcdata.h and tdsrrcdata_v.h 
===========================================================================*/
/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

#include "comdef.h"
#include "rrcmmif.h"
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
  TDSRRC_STATUS_SUCCESS,
  TDSRRC_STATUS_FAILURE
}tdsrrc_status_e_type;


typedef struct
{
  uint16 sdu_size;
  uint16 num_sdu_per_tti;
}tdsrrc_cs_data_parms_type;


/*provide MTNet NV status*/
typedef enum
{
    TDSRRC_MTNET_NV_OFF = 0,
    TDSRRC_MTNET_NV_ON  = 1,
    TDSRRC_MTNET_NV_INVALID = -1
}tdsrrc_mtnet_nv_status_e_type;

/*provide special_settings_for_testing status*/
typedef enum
{
    TDSRRC_SPECIAL_SETTINGS_FOR_TESTING_NV_OFF = 0,
    TDSRRC_SPECIAL_SETTINGS_FOR_TESTING_NV_ON  = 1,
    TDSRRC_SPECIAL_SETTINGS_FOR_TESTING_NV_INVALID = -1
}tdsrrc_special_test_settings_status_e_type;

/*provide CMCC special test settings config via NV69731*/
typedef enum
{
    TDSRRC_SPECIAL_TEST_SETTINGS_CONFIG_ENGINEERING = 0,
    TDSRRC_SPECIAL_TEST_SETTINGS_CONFIG_CMCC_LAB = 1,
    TDSRRC_SPECIAL_TEST_SETTINGS_CONFIG_CMCC_FIELD = 2,
    TDSRRC_SPECIAL_TEST_SETTINGS_CONFIG_INVALID = -1
}tdsrrc_special_test_settings_config_e_type;

/* HSDPA Status Variable */
typedef enum
{
  TDSHSDPA_INACTIVE,
  TDSHSDPA_ACTIVE,
  TDSHSDPA_SUSPEND
} tdsrrc_hsdpa_status_enum_type;

/* HSUPA Status Variable */
typedef enum
{
  TDSHSUPA_INACTIVE,
  TDSHSUPA_ACTIVE
} tdsrrc_hsupa_status_enum_type;

/*NV versino for L2*/
typedef enum
{
  TDSRRC_REL_VERSION_4 = 0,
  TDSRRC_REL_VERSION_5 = 1,
  TDSRRC_REL_VERSION_6 = 2,
  TDSRRC_REL_VERSION_7 = 3,
  TDSRRC_REL_VERSION_8 = 4,
  TDSRRC_REL_VERSION_9 = 5
}tdsrrc_rel_version_enum;

typedef struct
{
  uint16 tdsrlc_fc_min_win_size;  //Minimum value of the win size the FC can use for TX win size.
  uint16 tdsrlc_fc_num_of_steps; //Value of number of increments to be made to reach max configured win size.
  uint8 tdsfw_dl_fc_rtbs_value[8]; //Fw RTBS values for DL FC steps
}tdsl2_flow_control_value_type;

/*For L2 NV that provided by RRC*/
typedef struct
{
  tdsrrc_rel_version_enum rrc_version;
  uint32 l2_opt_bitmask;
  tdsl2_flow_control_value_type l2_flow_control_nv;
} tdsrrc_rlc_nv_list_type;

/* Enumerated type to give results of searching for a RAB in
the variable ESTABLISHED_RABS                                        */
typedef enum 
{
  TDSRRC_RAB_NOT_FOUND,
  TDSRRC_RAB_FOUND
}tdsrrc_rab_search_e_type;

/* UL Tx power range for total UL Tx power. The total TX power is for all channels on all UL TS.
     When apply the filtering, need to convert it to linear power. 
     When report the power range, convert back to dbm. 
     Formula for filtered TX Power:
     TX  =  ( ¦Á ¡Á S )  + ( 1 ¨C ¦Á ) ¡Á TX-1  
	  - S is total TX power for current frame for all TS. 
	  - ¦Á is filtering co-efficient and always less than 1. Suggested ¦Á = 0.1.
	  | TxTotalPwr (dBm) 	 | 	Cost of modem power 	   |
	  |    < 0dBm		 |		LOW		   |
	  |    0~10dBm		 |		MEDIUM		   |
	  |    >10dBm		 |		HIGH		   |

*/
typedef enum {
  /* Tx total power < 0dBm */
  TDSRRC_UL_TX_POWER_LOW,
  
  /* Tx total power within 0dBm - 10dBm (both included) */
  TDSRRC_UL_TX_POWER_MEDIUM,
  
  /* Tx total power > 10dBm */
  TDSRRC_UL_TX_POWER_HIGH,
  
  /* Initial/Invalid value */
  TDSRRC_UL_TX_POWER_MAX
} tdsrrc_ul_tx_power_range_e_type;


/* Maximum number of Logical Channel Ids (in one direction) for a RAB.
This is used while informing a user-plane entity which logical channels
have been assigned to it's RAB. The number 3 is because AMR voice RABs
can have 3 logical channels in each direction */
#define TDSRRC_MAX_LC_ID_PER_RAB 3

/* A Structure to store the LC Ids for user-plane RABs.
When the user-plane entity needs to know which lc ids it
needs to register the watermarks it queries this module
*/
typedef struct
{
  uint32 rab_id;               /* RAB Id                                   */
  uint8 num_ul_lc_ids_for_rab; /* Number of uplink RLC ids for this RAB    */
  uint8 num_dl_lc_ids_for_rab; /* Number of downlink RLC ids for this RAB  */
  uint8 ul_lc_id[TDSRRC_MAX_LC_ID_PER_RAB]; /* Uplink LC IDs                  */
  uint8 dl_lc_id[TDSRRC_MAX_LC_ID_PER_RAB]; /* Uplink LC IDs                  */
}tdsrrc_user_plane_lc_info_type;

/* This is the structure type to get UL rate info for DS */
typedef struct {
  /* Overall UL data rate in kbps for all PS services */
  uint32 max_allowed_ul_rate;
  
  /* PX  =  ( ¦Á ¡Á S )  + ( 1 ¨C ¦Á ) ¡Á PX-1  
     - PX is the likelihood for setting Plus_flag to TRUE. 
     - S = 1 (if current Serving Grand < Power header room)
     - S = 0 (if current Serving Grand >= Power header room)
     - S = ¦Â (if there is no Serving Grand for every 200ms gap since last Grand). Suggested ¦Â = 0.2.
     - ¦Á is filtering co-efficient and always less than 1. Suggested ¦Á = 0.1
     Plus_flag is TRUE when PX > Thr. Thr is the threshold. Suggested Thr = 0.9 
  */
  boolean plus_flag;

} tdsrrc_ul_rate_info_type;


/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION RRC_RETURN_CS_DATA_CALL_PARMS

DESCRIPTION

  This function fills in the SDU size and number of SDUs per TTI for the
  associated RAB id.
  
DEPENDENCIES


RETURN VALUE
  Returns FAILURE is the RAB id is not found.  Otherwise returns SUCCESS.
 

SIDE EFFECTS

  None

===========================================================================*/
tdsrrc_status_e_type tdsrrc_return_cs_data_call_parms
(    
  uint32                  rab_id,
  tdsrrc_cs_data_parms_type *cs_data_parms_ptr
);


/*====================================================================
FUNCTION: rrc_is_tds_active

DESCRIPTION:
  This function returns true of W is active, meaning MCM tdsrrc_mode
  is  returns FALSE otherwise

DEPENDENCIES:
  None

RETURN VALUE:
  uint32

SIDE EFFECTS:
====================================================================*/
boolean rrc_is_tds_active(void);


/*====================================================================
FUNCTION: tdsrrc_get_mtnet_nv_status

DESCRIPTION:
  This function returns true of NV status for MTNet support.

DEPENDENCIES:
  None

RETURN VALUE:
  TDSRRC_MTNET_NV_OFF = 0,
  TDSRRC_MTNET_NV_ON  = 1,
  TDSRRC_MTNET_NV_INVALID = -1

SIDE EFFECTS:
====================================================================*/
tdsrrc_mtnet_nv_status_e_type tdsrrc_get_mtnet_nv_status(void);

/*====================================================================
FUNCTION: tdsrrc_get_special_settings_for_testing

DESCRIPTION:
  This function returns true of NV status for special_settings_for_testing.

DEPENDENCIES:
  None

RETURN VALUE:
    TDSRRC_SPECIAL_SETTINGS_FOR_TESTING_NV_OFF = 0,
    TDSRRC_SPECIAL_SETTINGS_FOR_TESTING_NV_ON  = 1,
    TDSRRC_SPECIAL_SETTINGS_FOR_TESTING_NV_INVALID = -1

Note: This function is obsolete after new function tdsrrc_get_special_test_settings_config below is used. This
         function is maintained only for backward compatibility and compilation purpose.

SIDE EFFECTS:
====================================================================*/

tdsrrc_special_test_settings_status_e_type tdsrrc_get_special_settings_for_testing(void);

/*====================================================================
FUNCTION: tdsrrc_get_special_test_settings_config

DESCRIPTION:
  This function returns the config value of NV69731 for CMCC special_settings_for_testing.

DEPENDENCIES:
  None

RETURN VALUE:
    TDSRRC_SPECIAL_TEST_SETTINGS_CONFIG_ENGINEERING = 0,
    TDSRRC_SPECIAL_TEST_SETTINGS_CONFIG_CMCC_LAB = 1,
    TDSRRC_SPECIAL_TEST_SETTINGS_CONFIG_CMCC_FIELD = 2,
    TDSRRC_SPECIAL_TEST_SETTINGS_CONFIG_INVALID = -1

SIDE EFFECTS:
====================================================================*/

tdsrrc_special_test_settings_config_e_type tdsrrc_get_special_test_settings_config(void);

/*====================================================================
FUNCTION: tdsrrc_get_l2_opt_nv

DESCRIPTION:
  This function returns the value of NV item TDS_RRC_NV_TDS_L2_OPT_FEATURE_LIST. It is a utility function
  for L2 to access this NV item so L2 does not need a separate NV handling utility.

DEPENDENCIES:
  None

RETURN VALUE:
  UINT32

SIDE EFFECTS:
====================================================================*/
uint32 tdsrrc_get_l2_opt_nv
(
  void
);


/*===========================================================================

FUNCTION RRC_GET_HSDPA_STATUS

DESCRIPTION

  This function returns the HSDPA Status Variable.
  Put in this header file for DATA team using.
  
DEPENDENCIES


RETURN VALUE

  HSDPA Status 

SIDE EFFECTS

  None

===========================================================================*/

tdsrrc_hsdpa_status_enum_type tdsrrc_get_hsdpa_status
(
  void
);


/*===========================================================================

FUNCTION RRC_GET_HSUPA_STATUS

DESCRIPTION

  This function returns the HSUPA Status Variable.
  Created for DATA team using.
  
DEPENDENCIES


RETURN VALUE

  HSUPA Status 

SIDE EFFECTS

  None

===========================================================================*/

tdsrrc_hsupa_status_enum_type tdsrrc_get_hsupa_status
(
  void
);



/*===========================================================================

FUNCTION  RRC_RETURN_LC_INFO_FOR_RAB

DESCRIPTION

  This function returns the logical channel ids for a given RAB.
  The calling function must initialize a structure of type
  tdsrrc_user_plane_lc_info_type and fill in the RAB id before
  passing the pointer to this function. This function will fill
  in the LC ids for uplink and downlink for that RAB to the given
  pointer.
    
DEPENDENCIES

  Currently this is only supported for CS data RABs and PS RABs.
  This function must not be used for any other type of RABs at
  present.
 
RETURN VALUE

  A a value of type rrc_rab_status_e_type. TDSRRC_RAB_NOT_FOUND is returned
  if the given RAB is not a valid RAB that has been setup by RRC. TDSRRC_RAB_FOUND
  is returned if the RAB is valid.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_rab_search_e_type tdsrrc_return_lc_info_for_rab
(
  tdsrrc_user_plane_lc_info_type *ptr
);



/*===========================================================================

FUNCTION   RRC_FIND_RAB_FOR_RB_IN_EST_RABS

DESCRIPTION

  This function validates the rb_ids in the received RBR Message.
  For user plane RB ids it checks whether the rb
  is present in the established rbs. It returns the corresponding RAB id

DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_rab_id_type rab_id

SIDE EFFECTS

  None.

===========================================================================*/
rrc_rab_id_type tdsrrc_find_rab_for_rb_in_est_rabs
(
  uint8 rb_id
);

/*===========================================================================

FUNCTION tdsrrc_update_codec_info_to_cm

DESCRIPTION

  This function is called by GHDI to update the sampling rate for CM

DEPENDENCIES


RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void tdsrrc_update_codec_info_to_cm(uint32 rate); 

/*===========================================================================

FUNCTION tdsrrc_get_rlc_nv_list

DESCRIPTION
  It is a utility function for L2 to access this NV item
  so L2 does not need a separate NV handling utility.

DEPENDENCIES:
  None

RETURN VALUE:
  tdsrrc_rlc_nv_list_type


SIDE EFFECTS

  None

===========================================================================*/
tdsrrc_rlc_nv_list_type tdsrrc_get_rlc_nv_list(void);

/*===========================================================================

FUNCTION tdsrrc_public_mem_malloc

DESCRIPTION

  This function is called by segment loading platform to use RRC memory alloc
  
DEPENDENCIES


RETURN VALUE

  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS

  None

===========================================================================*/
void *tdsrrc_public_mem_malloc(size_t size);

/*===========================================================================

FUNCTION tdsrrc_public_mem_free

DESCRIPTION

  This function is called by segment loading platform to use RRC memory free
  
DEPENDENCIES


RETURN VALUE

  The memory manager must have been previously initialized.

SIDE EFFECTS

  None

===========================================================================*/
void tdsrrc_public_mem_free(void *ptr);

/*following is to inform L1 about LBT mode*/
/*===========================================================================

FUNCTION  RRCLBT_IS_LB_TEST

DESCRIPTION

  This function returns True if RRC is in the loopback test mode, otherwise,
  returns False.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrclbt_is_lb_on( void );

/*===========================================================================
FUNCTION tdsrrc_get_ul_rate_info

DESCRIPTION

  This function is called by DS to acquire UL maximum allowed data rate for
  PS service and plus_flag.
  
DEPENDENCIES


RETURN VALUE

  UL rate info for DS

SIDE EFFECTS

  None

===========================================================================*/
void tdsrrc_get_ul_rate_info
(
  tdsrrc_ul_rate_info_type *ul_rate_info_ptr
);

/*===========================================================================
FUNCTION tdsrrc_get_ul_tx_power_range

DESCRIPTION

  This function is called by DS to get UL Tx power range
  
DEPENDENCIES


RETURN VALUE

  UL Tx power range (LOW/MEDIUM/HIGH)

SIDE EFFECTS

  None

===========================================================================*/
tdsrrc_ul_tx_power_range_e_type tdsrrc_get_ul_tx_power_range(void);

/*===========================================================================

FUNCTION tdsrrc_get_ul_data_rate_for_rab

DESCRIPTION

  This function returns the UL data rate for 
  DS. 
  
DEPENDENCIES


RETURN VALUE

  Actual Rate if RAB exists
  0 Otherwise 

SIDE EFFECTS

  None

===========================================================================*/

uint32 tdsrrc_get_ul_data_rate_for_rab
(
   uint8 rab_id,
   sys_modem_as_id_e_type subs_id  /*Assume DSDS is always on for interface*/
); 

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION tdsrrc_mcfg_refresh_cb

DESCRIPTION

  This function is called when MCFG refresh event happens. 
  
DEPENDENCIES


RETURN VALUE

TRUE - successfully saved mcfg_refresh info to tdsrrc context.
FALSE - otherwise.

SIDE EFFECTS

  None

===========================================================================*/
boolean tdsrrc_mcfg_refresh_cb(mcfg_refresh_info_s_type *p_info);
#endif

#endif

