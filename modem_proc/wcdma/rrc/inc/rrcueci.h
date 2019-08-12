#ifndef RRCUECI_H
#define RRCUECI_H
/*===========================================================================
                  R R C  U E  C A P A B I L I T Y  I N F O R M A T I O N

DESCRIPTION
  This header file contains function prototypes for UE Capability Information
  procedure

Copyright (c) 2001,2003-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcueci.h_v   1.1   14 Sep 2001 21:16:08   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcueci.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/16   sn      Changes for LTE U feature
05/09/16   ad      Made changes to log important parms from OTAs
05/09/16   sp      Made changes for Mav debug snapshot
12/03/15   vi      Made changes to avoid sending fach meas ind to WL1 in multisim mode
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
02/19/15   aa      Made changes to start T304 timer with 100ms if the timer expires for N304th timer during a legacy tuneaway
11/10/14   sp      Made changes to save procedure substates in crash debug info
11/03/14   ad      Made changes to control different DB DC band combinations through NV
09/17/14    sg      Made changes to move all RRC nv macros to wnv_rrc.h
07/23/14   sp      Made changes to remove F3s as part of FR19999
05/20/14   rmn     Made changes to save DS Tuneaway Status Change indication in connected state based on the concurrency NV
05/14/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/10/14   db      Made changes to enable/disable feature support in DSDS/DSDA/TSTS modes through NV
03/10/14   sp      Fixed compilation error for boost feature
12/24/13   db      Introduced new NV to allow CPC,HSFACH,HSRACH,DC-HSDPA in DSDS and DSDA mode
12/12/13   sr      Made Changes so that W extracts LTE band prefrences from message WCDMA_RRC_LTE_UTRA_CAPABILITIES_REQ
12/03/13   sg      Made changes to update review comments for 3C
11/15/13   sp      Added Nv support for boost feature
11/14/13   sa      DC feature support will be controlled thru UE Cat
10/30/13   as      Made changes to add feature code for FEATURE_WCDMA_3C_HSDPA
10/17/13   ad      Changes for DC HSDPA feature with 3C interface
07/30/13   ad      Added code changes to support DB-DC feature
06/22/13   sr      Added code changes to manage More than 16 EUTRA Freqs in Pre-R11 WCDMA
03/26/13   sa      Added code changes to support FEATURE_WCDMA_DC_HSUPA
11/30/12   ad      Made changes to remvoe the dependancy on  rfm_band_supports_dual_carrier
10/23/12   pm      Made changes to correct enable HSRACH NV only when MACI and HSFACH
                   NVs are also ON
09/28/12   pm     Added NV support for MAC_I
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
07/23/10   ss      Added support to fill dc-hsdpa IE after checking rf support on current camped band
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
04/07/10   gkg     ASN.1 migration to 8.a.0
03/17/10   as      Resolved Errors due to ASN.1 8.9 Migration
03/15/10   as      Updated RRC Code to support ASN.1 8.9 version
11/05/09   rm      Added prototype for rrcueci_get_wtog_nacc_nv_item
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
09/03/09   ps      Made changes for EDPCCH power Boost under 
                   FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
05/08/09   ss      Updated Copyright Information
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
27/03/08   pk      Added support for feature FEATURE_IRAT_HO_INFO_COMPRESSED.
03/06/08   da      Made changes required to migrate to 09.2007 R6 version of
                   ASN.1
10/15/07   vr      Added macros to limit the number of advertised bands in InterRAT Handover Info
                   message to RRC_UECI_MAX_FDD_BANDS_FOR_IHO.
04/20/07   da      Made changes needed to update ASN.1 version to R6 sept R6 2006
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
03/07/06   vr      Added new fields in UECI storage type to support FDD2 bands
                   Changed the prototype of rrcueci_append_ue_radio_access_capability()
                   to take in v650ext to populate FDD2 bands
01/27/06   vk      Added new function rrcueci_get_nv_items() to read HSDPA Category NV Item
                   under FEATURE HSDPA
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
12/17/04   vr      Support for WCDMA -> 1x handover feature
11/23/04   sm      Support for Dynamic Power Class feature.
10/20/04   sm      Fix to advertise all the FDD bands correctly in compressed mode measurement
                   capability while sending ue capability extension to network.
09/17/04   kc      Added support for multi-band UE-capability advertisement
10/14/03   kc      Added support to provide interfaces to append IE's required 
                   by Inter-RAT HO Trnsfr when Classmark Info is queried by RR.
05/30/03   rj      Modified interface to function
                   rrcueci_verify_inter_rat_security_capability.
04/02/03   rj      Added prototypes to rrcueci_verify_security_capability
                   and rrcueci_verify_inter_rat_security_capability.
02/04/03   ram     Checked in lint changes, updated copyright.
09/14/01   rj      Update template for function prototype
06/05/01   rj      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define  RRC_UECI_MAX_BANDS_ALLOWED       8
#define  RRC_UECI_MAX_BANDS_ALLOWED_FDD2 16
#define  RRC_UECI_MAX_FDD_BANDS_FOR_IHO   4
#define  RRC_UECI_INVALID_BAND_TYPE       0xFF

#define RRCUECI_INVALID_TRANSACTION_ID 0xFF

#ifdef FEATURE_MAC_I
#define  RRC_IS_MAC_I_NV_TURNED_ON \
  ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8) && \
  (rrc_feature_nv_item_value & RRC_MAC_I_ENABLED))

#endif

#ifdef FEATURE_WCDMA_DB_DC_HSDPA
#define  RRC_NUMBER_OF_DB_DC_BAND_COMB 16


/*structure to store the number of DB-DC band combinations supported*/
typedef struct
{
  uint16 num_db_dc_band_comb_supp;
  uint16 db_dc_band_comb_list[RRC_NUMBER_OF_DB_DC_BAND_COMB];
}rrcueci_db_dc_band_storage_type;
#endif
#define MAX_BITMASK_TO_BE_PRIORITIZED     3 
#define GET_64_BMASK(idx)                 (((uint64)1) << idx)

/* T304 is is started with 100ms if the timer expires for the N304th time */
#define RRCTMR_T304_EXPIRED_N304TH_TIME_IN_MS 100

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef enum
{
  RRCUECI_SUCCESS,                            /* Successful Status */      
  RRCUECI_FAILURE_MINOR,                      /* Minor Failure Status */
  RRCUECI_FAILURE_MAJOR                       /* Major Failure status */
} rrcueci_status_e_type;

typedef enum
{
  RRCUECI_CAP_UPDATE,                         /* Cap Update by RCE proc */      
  RRCUECI_CAP_HO_INFO_TRANSFER,               /* HO INFO Trnsfr by IHO  */
  RRCUECI_CAP_MAX                             /* MAX Enum Val           */
} rrcueci_cap_update_cause_e_type;

typedef enum
{
  RRCUECI_INT_CAP_UPDATE_2100,                          /* Cap Update by RCE proc */      
  RRCUECI_INT_CAP_UPDATE_1900_MULTI_BAND,               /* HO INFO Trnsfr by IHO  */
  RRCUECI_INT_CAP_MAX                                   /* MAX Enum Val           */
} rrcueci_internal_cap_update_cause_e_type;

typedef struct
{
  uint8   num_supported_bands;
  boolean band_type_present[RRC_UECI_MAX_BANDS_ALLOWED];
  uint32  band_type[RRC_UECI_MAX_BANDS_ALLOWED];

  /* For FDD2 bands */
  uint8   num_supported_bands_fdd2;
  boolean band_type_present_fdd2[RRC_UECI_MAX_BANDS_ALLOWED_FDD2];
  uint32  band_type_fdd2[RRC_UECI_MAX_BANDS_ALLOWED_FDD2];

}rrcueci_band_storage_type;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION rrcueci_reset_ueci_procedure

DESCRIPTION
  This resets the UECI procedure i.e. stops T304 timer, reset the value of variables
  used in this procedure to default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcueci_reset_ueci_procedure(void);

/*===========================================================================

FUNCTION rrcueci_init_procedure

DESCRIPTION
  This function initializes required parameters for UE Capability Information
  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void rrcueci_init_procedure
(
  void
);

/*===========================================================================

FUNCTION rrcueci_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the UE Capability Enquiry procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC) containing UE 
    CAPABILITY INFORMATION CONFIRM message.

    RRC_INITIATE_UE_CAPABILITY_REQ Primitive
    RRC_STATE_CHANGE_IND Primitive
    RRC_CELL_UPDATE_COMPLETE_CNF Primitive

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcueci_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcueci_append_ue_radio_access_capability
(
  rrc_UE_RadioAccessCapability *ue_radio_access_capability_ptr,  
  rrc_UE_RadioAccessCapability_v370ext *ue_radio_access_capability_ext_ptr,
  rrc_UE_RadioAccessCapability_v650ext *ue_radio_access_capability_v650_ptr,
  rrcueci_cap_update_cause_e_type update_cause                  /* Capability query cause*/
);

#include "wcdma_variation.h"
#ifdef FEATURE_IRAT_HO_INFO_COMPRESSED
#error code not present
#endif /* FEATURE_IRAT_HO_INFO_COMPRESSED */

/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_r5

DESCRIPTION
  This function appends the Radio Access Capability to the message for
  Release 5.
  
  We should optimize the code later.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcueci_append_ue_radio_access_capability_later_than_r3
(
  rrc_UE_RadioAccessCapability_v590ext *ue_radio_access_capability_ptr,  
                                                /* Pointer to UE Radio Access 
                                                   Capability */
   rrc_UE_CapabilityContainer_IEs *ue_capability_container,
  rrcueci_cap_update_cause_e_type update_cause  /* Capability query cause*/
);

/*===========================================================================
FUNCTION rrcueci_append_gsm_specific_capability

DESCRIPTION
  This function appends the GSM Specific Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcueci_append_gsm_specific_capability
(
  rrc_InterRAT_UE_RadioAccessCapability *inter_rat_capability_ptr   
                                /* Pointer to Inter RAT Capabilities List */
);

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif


/*===========================================================================
FUNCTION rrcueci_verify_security_capability

DESCRIPTION
  This function checks the Security Capability.
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if check is successful. Otherwise returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcueci_verify_security_capability
( 
  rrc_SecurityCapability * securityCapability
);

/*===========================================================================
FUNCTION rrcueci_verify_inter_rat_security_capability

DESCRIPTION
  This function checks the GSM Specific Capability.
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if check is successful. Otherwise returns FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcueci_verify_inter_rat_security_capability
(
  rrc_InterRAT_UE_SecurityCapList * ue_SystemSpecificSecurityCap
);

/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_ext

DESCRIPTION
  This function appends the Radio Access Capability to the procedure which
  sends Inter-RAT HO Transfer
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcueci_append_ue_radio_access_capability_ext
(
  rrc_UE_RadioAccessCapabBandFDD * fdd_info, /* Pointer to UE Radio Access
                                                  Capability Extension  */
  rrc_RadioFrequencyBandFDD freq_band,          /* freq band to populate */
  const rrcueci_band_storage_type *ueci_band_ptr     /* pointer to get supported FDD freq bands */
#ifdef FEATURE_LTE_TO_WCDMA
  ,rrcueci_internal_cap_update_cause_e_type update_cause  /*Remove the interRAT cell list only for HO*/
#endif
);


/*===========================================================================

FUNCTION rrcueci_get_nv_items

DESCRIPTION
  This  function gets NV item NV_HSDPA_CAT_I and stores it in the global variable

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcueci_get_nv_items
(
sys_modem_as_id_e_type wrrc_as_id
);
/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_rel7_capability

DESCRIPTION
  This function appends the Radio Access Capability to the message for
  Release 7.
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 void  rrcueci_append_ue_radio_access_capability_rel7_capability
 (
   rrc_UE_CapabilityContainer_IEs * rel7_ue_capablity,
    rrcueci_cap_update_cause_e_type update_cause
 );

/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_rel8_capability

DESCRIPTION
  This function appends the Radio Access Capability to the message for
  Release 8.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  rrcueci_append_ue_radio_access_capability_rel8_capability
(
  rrc_UE_CapabilityContainer_IEs * ue_capability_container
);

#ifdef FEATURE_MAC_I

/*====================================================================
FUNCTION: rrcueci_nv_supports_mac_i()

DESCRIPTION:
This function checks whether the NV supports MAC_I or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_mac_i
(
  void
);

#endif /* FEATURE_MAC_I */

#ifdef FEATURE_WCDMA_DC_HSDPA

/*====================================================================
FUNCTION: rrcueci_nv_supports_dc_hsdpa()

DESCRIPTION:
This function checks whether the NV supports DC-HSDPA

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_dc_hsdpa
(
  void
);
#endif /*FEATURE_WCDMA_DC_HSDPA*/

#ifdef FEATURE_WCDMA_DC_HSUPA

/*====================================================================
FUNCTION: rrcueci_nv_supports_dc_hsupa()

DESCRIPTION:
This function checks whether the NV supports DC-HSUPA

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_dc_hsupa
(
  void
);
#endif /*FEATURE_WCDMA_DC_HSUPA*/

#ifdef FEATURE_WCDMA_3C_HSDPA
/*====================================================================
FUNCTION: rrcueci_nv_supports_3c_hsdpa()

DESCRIPTION:
This function checks whether the NV supports Dc HSUPA or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_3c_hsdpa
(
  void
);

#endif /*FEATURE_WCDMA_3C_HSDPA*/
#ifdef FEATURE_WCDMA_DB_DC_HSDPA
/*====================================================================
FUNCTION: rrcueci_nv_supports_dc_hsupa()

DESCRIPTION:
This function checks whether the NV supports Dual Band Dual Carrier HSDPA or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_db_dc_hsdpa
(
  void
);
#endif

/*====================================================================
FUNCTION: rrcueci_nv_supports_cpc_dtx_on_dpcch()

DESCRIPTION:
This function checks whether the NV supports cpc_dtx_on_dpcch and slot format 4

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_cpc_dtx_on_dpcch
(
  void
);

/*===========================================================================

FUNCTION rrcueci_get_wtog_nacc_nv_item

DESCRIPTION
  This  function reads the NVs NV_GERAN_FEATURE_PACK_1_I and NV_INTERRAT_NACC_SUPPORT_I
  and based on these NV items updates global with WTOG NACC enabled status

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcueci_get_wtog_nacc_nv_item
(
  sys_modem_as_id_e_type wrrc_as_id
);
/*===========================================================================

FUNCTION  rrcueci_get_current_substate

DESCRIPTION

  This function will return the current substate
    
DEPENDENCIES

  None.
 
RETURN VALUE
rrctfcc_substates_e_type


SIDE EFFECTS

  None.

===========================================================================*/
uint8 rrcueci_get_current_substate
(
  void
);

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION rrcueci_send_eutra_capabilitites_req_to_lte

DESCRIPTION
  This  function sends EUTRA capabilities request to LTE

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcueci_send_eutra_capabilitites_req_to_lte
(
  void
);

/*===========================================================================

FUNCTION rrcueci_save_eutra_capability

DESCRIPTION
  This  function saves the E-UTRA capability info

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcueci_save_eutra_capability
(
  lte_rrc_eutra_capabilities_rsp_s *lte_eutra_capabilities_rsp
);

/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_fdd3

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcueci_append_ue_radio_access_capability_fdd3_list
(
  rrc_UE_RadioAccessCapabBandFDDList3 *fddlist3,
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type lte_band_pref
#else
  sys_band_mask_e_type lte_band_pref
#endif
);
/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_fdd4

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcueci_append_ue_radio_access_capability_fdd4_list
(
  rrc_UE_RadioAccessCapabBandFDDList4 *fddlist4
);


/*===========================================================================
FUNCTION rrc_fill_lte_band_mask

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef LTE_BAND_NUM
void rrc_fill_lte_band_mask
(
  uint8* num_lte_bands,
  sys_lte_band_mask_e_type *lte_band_pref,
  sys_lte_band_mask_e_type *lte_fdd_band_pref,
  sys_lte_band_mask_e_type *lte_tdd_band_pref
);
#else
void rrc_fill_lte_band_mask
(
  uint8* num_lte_bands,
  sys_band_mask_e_type *lte_band_pref,
  sys_band_mask_e_type *lte_fdd_band_pref,
  sys_band_mask_e_type *lte_tdd_band_pref
);
#endif

/*===========================================================================
FUNCTION rrcueci_get_static_band_mask

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
sys_band_mask_type rrcueci_get_static_band_mask(void);

/*===========================================================================
FUNCTION rrc_fill_priority_bitmask_array

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrc_fill_priority_bitmask_array(sys_band_mask_type *ueci_priority_bmask);

#endif

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
/*===========================================================================
FUNCTION rrcueci_check_fe_fach_concurrency

DESCRIPTION
  This function checks for concurrency NVs before showing FE-FACH capability 
  in connectionsetupcomplete message

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/

boolean rrcueci_check_fe_fach_concurrency();
#endif

#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
/*====================================================================
FUNCTION: rrcueci_nv_supports_edpcch_pwr_boost()
DESCRIPTION:
This function checks whether the NV supports EDPCCH_PWR_BOOST or not
DEPENDENCIES:
  None.
RETURN VALUE:
  True/False.
SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_edpcch_pwr_boost(void);
#endif /* FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST */
/*===========================================================================

FUNCTION rrcueci_return_ueci_substate

DESCRIPTION
Returns current ueci substate

DEPENDENCIES
  None.

RETURN VALUE
  rrcueci_substate_e_type.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif

uint8 rrcueci_return_ueci_substate(void);

/*===========================================================================

FUNCTION rrcueci_get_cap_container_length

DESCRIPTION
Returns current ueci substate

DEPENDENCIES
  None.

RETURN VALUE
  rrcueci_substate_e_type.

===========================================================================*/
uint16 rrcueci_get_cap_container_length(void);

#endif /* RRCUECI_H */

