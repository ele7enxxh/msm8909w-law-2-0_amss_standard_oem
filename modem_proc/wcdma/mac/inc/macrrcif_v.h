#ifndef MACRRCIF_V_H
#define MACRRCIF_V_H

/*========================================================================================

   MAC - RRC INTERFACE H E A D E R    F I L E

DESCRIPTION
  This file contains the declaration of the interface functions and
  data types used by MAC and RRC

  Copyright (c) 2001,2002 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2003-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/macrrcif_v.h#1 $  $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------------------------
06/02/11    kc      Added support for logical channel to transport channel mapping info
02/28/11    ssg     Added fix to indicate RRC about MAC-hs state.
01/17/11    vp      Moved the Wcdma only declarations from macrrcif.h to _v.h
12/02/10    geg     Created file

========================================================================================*/

/*=======================================================================================

                     INCLUDE FILES

========================================================================================*/

#include "wcdma_variation.h"
#include "macrrcif.h"

typedef enum
{
  RRC_NO_CODEC = 0,
  RRC_NB_AMR_CODEC,
  RRC_WB_AMR_CODEC
}rrc_event_ul_amr_codec_type;

typedef enum
 {

  MAC_AMR_MODE_0475,    /* 0 -- 4.75 kbit /s */ 

  MAC_AMR_MODE_0515,    /*1 --  5.15 kbit /s */

  MAC_AMR_MODE_0590,    /* 2 --  5.90 kbit /s */

  MAC_AMR_MODE_0670,    /* 3 --  6.70 kbit /s */

  MAC_AMR_MODE_0740,    /* 4 --  7.40 kbit /s */

  MAC_AMR_MODE_0795,    /* 5 --  7.95 kbit /s */

  MAC_AMR_MODE_1020,    /* 6 --  10.2 kbit /s */

  MAC_AMR_MODE_1220,    /* 7 --  12.2 kbit /s */

  MAC_AMR_MODE_0660,    /* 8 --  AMR-WB 6.60 kbps */

  MAC_AMR_MODE_0885,    /* 9 --   AMR-WB 8.85 kbps */

  MAC_AMR_MODE_1265,    /* 10 --  AMR-WB 12.65 kbps */

  MAC_AMR_MODE_1425,    /* 11 --  AMR-WB 14.25 kbps */

  MAC_AMR_MODE_1585,    /* 12 --  AMR-WB 15.85 kbps */

  MAC_AMR_MODE_1825,    /* 13 --  AMR-WB 18.25 kbps */

  MAC_AMR_MODE_1985,    /* 14 --  AMR-WB 19.85 kbps */

  MAC_AMR_MODE_2305,    /* 15 --  AMR-WB 23.05 kbps */

  MAC_AMR_MODE_2385,    /* 16 --  AMR-WB 23.85 kbps */

  MAC_AMR_MODE_UNDEF =0xFF /* 0xFF --  undefined */

}rrc_event_ul_amr_rate_type;

/*Event structure EVENT_WCDMA_UL_AMR_RATE:*/
typedef PACKED struct
{
  uint8 codec_type;
  uint8 num_rates;
  rrc_event_ul_amr_rate_type amr_rate[9];
}wcdma_ul_amr_rate_event_type;

/*-------------------------------------------------------------------
FUNCTION mac_get_additional_traffic_vol_msmt_results

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
boolean
mac_get_additional_traffic_vol_msmt_results(uint16 meas_id, mac_ul_traffic_meas_ind_type *);

/*-------------------------------------------------------------------
FUNCTION mac_rrc_get_hfn_info

DESCRIPTION
 Function interface which will provide RRC with the information of CS and PS HFN
 numbers during the time of RRC connection release
DEPENDENCIES
  None

RETURN VALUE
  Boolean ( TRUE indicates MAC was able to populate the cs_hfn and ps_hfn values.
  FALSE indicates the data was not populated.

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
boolean
mac_rrc_get_hfn_info(mac_hfn_info_type *hfn_info);

/*-------------------------------------------------------------------
FUNCTION rrc_get_start_value_to_transmit()

DESCRIPTION
Call back defined in rrcsmc.c to give MAC the HFN value.

DEPENDENCIES
  None

RETURN VALUE
Returns the HFN value.

SIDE EFFECTS
  None
--------------------------------------------------------------------*/

uint32
rrc_get_start_value_to_transmit(void);

/*===========================================================================
FUNCTION   rrc_get_gcf_flag_status

DESCRIPTION
  This function returns the status of gcf flag. TRUE if GCF NV is set, false
  otherwise

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/

boolean rrc_get_gcf_flag_status(void);

/*===========================================================================
FUNCTION   rrcrb_query_mac_rab_status

DESCRIPTION
This function will be called by MAC at configuration time to determine
if it is a CS only or PS only call. Based on this MAC will determine how
the AMR rate changes need to be applied based on the power.

DEPENDENCIES
  None.

RETURN VALUE
Function returns if the call is of type CS only, PS only or CS+PS call.

SIDE EFFECTS
  None.
===========================================================================*/
mac_rab_status_e_type rrc_query_mac_rab_status
(
  void
);

/*-------------------------------------------------------------------
FUNCTION mac_rrc_get_cipher_pending_status

DESCRIPTION
 This function will be called by RRC in the case when it receives
 a SMC message

DEPENDENCIES
  None

RETURN VALUE
  Boolean ( TRUE indicates MAC has not reached the ciphering
  activation time for the current config under use. FALSE indicates
  that there is no pending configuration in MAC and ciphering is
  ongoing

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
boolean
mac_rrc_get_cipher_pending_status(void);


#ifdef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION

/*-------------------------------------------------------------------
FUNCTION mac_ul_allocate_mem_ZI_opt

DESCRIPTION
 This function will be called by RRC at init time so that MAC can go for 
 dynamic memory allocation for the required varaibles.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
--------------------------------------------------------------------*/
void  mac_ul_allocate_mem_ZI_opt(void);

#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */

/*-------------------------------------------------------------------
FUNCTION l2_free_mem_ZI_opt

DESCRIPTION
 This function will be called by RRC in the call back , so that L2 (MAC) can de-allocate/free
 the dynamic memory allocated data strucutre varaibles.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
--------------------------------------------------------------------*/

void  l2_free_mem_ZI_opt(void);

/*-------------------------------------------------------------------
FUNCTION mac_is_hs_cfg_active

DESCRIPTION
 This function will be called by RRC to know whether MAC-hs is
 in ACTIVE state or not.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
--------------------------------------------------------------------*/
boolean mac_is_hs_cfg_active(void);

/*===========================================================================
FUNCTION   l2_get_trch_type_for_given_rb_id

DESCRIPTION
  This function returns transport channel type corresponding to
  given RB and RLC id.

DEPENDENCIES
  None.

RETURN VALUE
  Transport Channel type.

SIDE EFFECTS
  None.
===========================================================================*/

log_wcdma_trch_type  l2_get_trch_type_for_given_rb_id
(
  /* Radio Bearer Identifier (0-31) */
  rb_id_type  rb_id,

  /*0-> Downlink 1-> Uplink*/
  boolean  direction, 
  
 /* RLC logical channel buffer identifier (0-20) */
  rlc_lc_id_type  rlc_id
);

#endif /* MACRRCIF_V_H */

