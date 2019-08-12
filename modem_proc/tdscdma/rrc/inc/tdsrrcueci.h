#ifndef TDSRRCUECI_H
#define TDSRRCUECI_H
/*===========================================================================
                  R R C  U E  C A P A B I L I T Y  I N F O R M A T I O N

DESCRIPTION
  This header file contains function prototypes for UE Capability Information
  procedure

Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
Export of this technology or software is regulated by the U.S. Government. 
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rrc/inc/tdsrrcueci.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/11   ysh   Merged with VU_MODEM_WCDMA_RRC_OSYS.01.93.53
11/17/10   bj     Made changes to set IE "UE RadioAccessCapability"(FDD->TDD).
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00

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
#define  TDSRRC_UECI_MAX_BANDS_ALLOWED       8
#define  RRC_UECI_MAX_BANDS_ALLOWED_TDS2 16
#define  RRC_UECI_MAX_TDS_BANDS_FOR_IHO   4
#define  TDSRRC_UECI_INVALID_BAND_TYPE       0xFF

#define TDSRRCUECI_INVALID_TRANSACTION_ID 0xFF

#define QWORD_LOW( qword )  ( (dword) ((uint64) qword & (uint64) 0xFFFFFFFF))
#define QWORD_HIGH( qword ) ( (dword) ((uint64) qword >> 32) )

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef enum
{
  TDSRRCUECI_SUCCESS,                            /* Successful Status */      
  TDSRRCUECI_FAILURE_MINOR,                      /* Minor Failure Status */
  TDSRRCUECI_FAILURE_MAJOR                       /* Major Failure status */
} tdsrrcueci_status_e_type;

typedef enum
{
  TDSRRCUECI_CAP_UPDATE,                         /* Cap Update by RCE proc */      
  TDSRRCUECI_CAP_HO_INFO_TRANSFER,               /* HO INFO Trnsfr by IHO  */
  TDSRRCUECI_CAP_MAX                             /* MAX Enum Val           */
} tdsrrcueci_cap_update_cause_e_type;

typedef enum
{
  TDSRRCUECI_INT_CAP_UPDATE_2100,                          /* Cap Update by RCE proc */      
  TDSRRCUECI_INT_CAP_UPDATE_1900_MULTI_BAND,               /* HO INFO Trnsfr by IHO  */
  TDSRRCUECI_INT_CAP_MAX                                   /* MAX Enum Val           */
} tdsrrcueci_internal_cap_update_cause_e_type;

typedef struct
{
  uint8   num_supported_bands;
  boolean band_type_present[TDSRRC_UECI_MAX_BANDS_ALLOWED];
  uint32  band_type[TDSRRC_UECI_MAX_BANDS_ALLOWED];

  /* For FDD2 bands */
  uint8   num_supported_bands_fdd2;
  boolean band_type_present_fdd2[RRC_UECI_MAX_BANDS_ALLOWED_TDS2];
  uint32  band_type_fdd2[RRC_UECI_MAX_BANDS_ALLOWED_TDS2];

  /* For TDD v860Ext 1.28Mbps bands */
  uint8   num_supported_bands_tddv860;
  boolean band_type_present_tddv860[RRC_UECI_MAX_BANDS_ALLOWED_TDS2];
  uint32  band_type_tddv860[RRC_UECI_MAX_BANDS_ALLOWED_TDS2];

}tdsrrcueci_band_storage_type;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
extern unsigned short tdsrrc_v_304;

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION tdsrrcueci_reset_ueci_procedure

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
void tdsrrcueci_reset_ueci_procedure(void);

/*===========================================================================

FUNCTION tdsrrcueci_init_procedure

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

void tdsrrcueci_init_procedure
(
  void
);

/*===========================================================================

FUNCTION tdsrrcueci_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the UE Capability Enquiry procedure.
  The following events are handled by this procedure:

    TDSRRC_DOWNLINK_SDU_IND Primitive (received from RLC) containing UE 
    CAPABILITY INFORMATION CONFIRM message.

    TDSRRC_INITIATE_UE_CAPABILITY_REQ Primitive
    TDSRRC_STATE_CHANGE_IND Primitive
    TDSRRC_CELL_UPDATE_COMPLETE_CNF Primitive

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void tdsrrcueci_procedure_event_handler
(
  tdsrrc_cmd_type *cmd_ptr
);
/*===========================================================================
FUNCTION tdsrrcueci_append_ue_radio_access_capability

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsrrcueci_append_ue_radio_access_capability
(
  tdsrrc_UE_RadioAccessCapability *ue_radio_access_capability_ptr,  
  boolean irat_ho_info,
  tdsrrcueci_cap_update_cause_e_type update_cause                  /* Capability query cause*/
);

#include "tdscdma_variation.h"
#ifdef FEATURE_TDSCDMA_IRAT_HO_INFO_COMPRESSED
/*===========================================================================
FUNCTION RRCUECI_APPEND_UE_RADIO_ACCESS_CAPABILITY_COMPRESSED

DESCRIPTION
  This function fills in the Radio Access Capability compressed extensions.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrcueci_append_ue_radio_access_capability_compressed
(
  tdsrrc_RF_CapabBandListFDDComp *rrc_RFCapabilityCompressed, // R5
  tdsrrc_RF_CapabBandListFDDComp_ext *rrc_RFCapabilityFDDComp, // R6
  boolean *is_fdd_comp_ext_present
);
#endif /* FEATURE_TDSCDMA_IRAT_HO_INFO_COMPRESSED */

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
void tdsrrcueci_append_ue_radio_access_capability_later_than_r3
(
  tdsrrc_UE_RadioAccessCapability_v590ext *ue_radio_access_capability_ptr,  
                                                /* Pointer to UE Radio Access 
                                                   Capability */
   tdsrrc_UE_CapabilityContainer_IEs *ue_capability_container,
  tdsrrcueci_cap_update_cause_e_type update_cause  /* Capability query cause*/
);

/*===========================================================================
FUNCTION tdsrrcueci_append_gsm_specific_capability

DESCRIPTION
  This function appends the GSM Specific Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrcueci_append_gsm_specific_capability
(
  tdsrrc_InterRAT_UE_RadioAccessCapability *inter_rat_capability_ptr   
                                /* Pointer to Inter RAT Capabilities List */
);



/*===========================================================================
FUNCTION tdsrrcueci_verify_security_capability

DESCRIPTION
  This function checks the Security Capability.
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if check is successful. Otherwise returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsrrcueci_verify_security_capability
( 
  tdsrrc_SecurityCapability * securityCapability
);

/*===========================================================================
FUNCTION tdsrrcueci_verify_inter_rat_security_capability

DESCRIPTION
  This function checks the GSM Specific Capability.
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if check is successful. Otherwise returns FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsrrcueci_verify_inter_rat_security_capability
(
  tdsrrc_InterRAT_UE_SecurityCapList * ue_SystemSpecificSecurityCap
);

/*===========================================================================

FUNCTION tdsrrcueci_get_nv_items

DESCRIPTION
  This  function gets NV item NV_HSDPA_CAT_I and stores it in the global variable

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void tdsrrcueci_get_nv_items
(
void
);
/*===========================================================================
FUNCTION tdsrrcueci_append_ue_radio_access_capability_rel7_capability

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
 void  tdsrrcueci_append_ue_radio_access_capability_rel7_capability
 (
   tdsrrc_UE_CapabilityContainer_IEs * rel7_ue_capablity,
    tdsrrcueci_cap_update_cause_e_type update_cause
 );

#ifdef FEATURE_TDSCDMA_REL8
/*===========================================================================
FUNCTION tdsrrcueci_append_ue_radio_access_capability_rel8_capability

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
void  tdsrrcueci_append_ue_radio_access_capability_rel8_capability
(
  tdsrrc_UE_CapabilityContainer_IEs * ue_capability_container
);
#ifdef FEATURE_TDS_DC_HSDPA
#ifdef FEATURE_TDSCDMA_BAND_SPEC_DC_HSDPA
/*===========================================================================
FUNCTION tdsrrcueci_check_rfm_band_support_for_dual_carrier

DESCRIPTION
  This function check the support for DC-hSDPA
  Release 8.

DEPENDENCIES
  None

RETURN VALUE
  booelan

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsrrcueci_check_rfm_band_support_for_dual_carrier
(
void
);

#endif /* FEATURE_TDSCDMA_BAND_SPEC_DC_HSDPA */
#endif /* FEATURE_TDS_DC_HSDPA */

#endif /*FEATURE_TDSCDMA_REL8*/

#ifdef FEATURE_TD2G_NACC
/*===========================================================================

FUNCTION tdsrrcueci_get_td2g_nacc_nv_item

DESCRIPTION
  This  function reads the NVs NV_GERAN_FEATURE_PACK_1_I and NV_INTERRAT_NACC_SUPPORT_I
  and based on these NV items updates global with TDSTOG NACC enabled status

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void tdsrrcueci_get_td2g_nacc_nv_item
(
  void
);
#endif
#ifdef FEATURE_TDSCDMA_TO_LTE
/*===========================================================================

FUNCTION tdsrrcueci_send_eutra_capabilitites_req_to_lte

DESCRIPTION
  This  function sends EUTRA capabilities request to LTE

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void tdsrrcueci_send_eutra_capabilitites_req_to_lte
(
  void
);

/*===========================================================================

FUNCTION tdsrrcueci_save_eutra_capability

DESCRIPTION
  This  function saves the E-UTRA capability info

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void tdsrrcueci_save_eutra_capability
(
  lte_rrc_eutra_capabilities_rsp_s *lte_eutra_capabilities_rsp
);

/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_tdd

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean tdsrrcueci_append_ue_radio_access_capability_tdd
(
  tdsrrc_MeasurementCapabilityTDD *tdd_ptr,
  sys_band_mask_e_type lte_band_pref
);
#endif

#endif /* TDSRRCUECI_H */

