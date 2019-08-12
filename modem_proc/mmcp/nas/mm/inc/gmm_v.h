#ifndef GMM_V_H
#define GMM_V_H
/*===========================================================================

              GPRS MOBILITY MANAGEMENT (GMM) SUB-SYSTEM HEADER FILE

DESCRIPTION
  This file data and function declarations necessary for the GMM sub-system
  of the UMTS/GSM MM task.

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmm.h_v   1.9   06 Jun 2002 10:13:56   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/gmm_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/01   jca     Created file.
04/12/01   jca     Merged with MM baseline code.
05/11/01   jca     Moved L3 message definitions to gmm_msg.h
05/23/01   jca     Added gmm_service_state_type.
08/03/01   jca     Modified gmm_handle_plmn_found() function prototype.
08/15/01   ab      Rename mmi_reg.h to reg_cm.h.
02/20/02   sbk     CSN encoder function prototypes added
06/05/02   ks      Added support for Dual mode, consolidated the
                   PLMN informatin and GMM state information.
08/01/03   jca     Added new substate GMM_NULL_SUBSTATE.
08/27/04   ab      CR39294. Updated the NPDU list primitives.
02/28/04   ks      Externalized function set_access_tech_info() so that
                   MAC uses this function directly for supporting EGPRS.
12/18/09   RI      Renamed gmm_load_sim_data() to gmm_load_psloci_data()
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "gmm.h"
#include "mm_v.h"
#include "mmtask_int.h"
#include "ghdi_exp_int.h"



/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* --------------------------------------
** GMM States (see 3GPP TS 24.008 4.1.3 )
** -------------------------------------- */
typedef enum
{
  GMM_NULL,
  GMM_DEREGISTERED,
  GMM_REGISTERED_INITIATED,
  GMM_REGISTERED,
  GMM_DEREGISTERED_INITIATED,
  GMM_ROUTING_AREA_UPDATING_INITIATED,
  GMM_SERVICE_REQUEST_INITIATED
} gmm_state_type;

/* -------------------------------------------------------
** Substates of GMM-DEREGISTERED and GMM_REGISTERED states
** (3GPP TS 24.008 4.1.3.1.2 and 4.1.3.1.3)
** ------------------------------------------------------- */
typedef enum
{
  GMM_NORMAL_SERVICE,
  GMM_LIMITED_SERVICE,
  GMM_ATTACH_NEEDED,
  GMM_ATTEMPTING_TO_ATTACH,
  GMM_NO_IMSI,
  GMM_NO_CELL_AVAILABLE,
  GMM_PLMN_SEARCH,
  GMM_SUSPENDED,
  GMM_UPDATE_NEEDED,
  GMM_ATTEMPTING_TO_UPDATE,
  GMM_ATTEMPTING_TO_UPDATE_MM,
  GMM_IMSI_DETACH_INITIATED,
  GMM_NULL_SUBSTATE
} gmm_substate_type;

/* -----------------------------------------
** GPRS Update Status values (31.102 4.2.23)
** ----------------------------------------- */
#define GMM_GU1_UPDATED                  0x00
#define GMM_GU2_NOT_UPDATED              0x01
#define GMM_GU3_PLMN_NOT_ALLOWED         0x02
#define GMM_GU3_ROUTING_AREA_NOT_ALLOWED 0x03

typedef byte gmm_update_status_type;

/* -------------
** P-TMSI format
** ------------- */
typedef struct
{
  byte digit[PTMSI_SIZE];
} gmm_ptmsi_type;

/* -----------------------
** P-TMSI signature format
** ----------------------- */
typedef struct
{
  byte value[PTMSI_SIGNATURE_SIZE];
} gmm_ptmsi_signature_type;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern gmm_state_type           gmm_state_sim[MAX_NAS_STACKS];

#define  gmm_state gmm_state_sim[mm_as_id]

#else
extern gmm_state_type           gmm_state;
#endif /* FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/

/* ---------------------------------------------------------------------
** Message types for GMM peer-to-peer messages.  These are as defined in
** TS 24.008 Table 10.4
** --------------------------------------------------------------------- */
#define GMM_ATTACH_REQUEST                         0x01
#define GMM_ATTACH_ACCEPT                          0x02
#define GMM_ATTACH_COMPLETE                        0x03
#define GMM_ATTACH_REJECT                          0x04
#define GMM_DETACH_REQUEST                         0x05
#define GMM_DETACH_ACCEPT                          0x06

#define GMM_ROUTING_AREA_UPDATE_REQUEST            0x08
#define GMM_ROUTING_AREA_UPDATE_ACCEPT             0x09
#define GMM_ROUTING_AREA_UPDATE_COMPLETE           0x0A
#define GMM_ROUTING_AREA_UPDATE_REJECT             0x0B

#define GMM_SERVICE_REQUEST                        0x0C
#define GMM_SERVICE_ACCEPT                         0x0D
#define GMM_SERVICE_REJECT                         0x0E

#define GMM_PTMSI_REALLOCATION_COMMAND             0x10
#define GMM_PTMSI_REALLOCATION_COMPLETE            0x11
#define GMM_AUTHENTICATION_AND_CIPHERING_REQUEST   0x12
#define GMM_AUTHENTICATION_AND_CIPHERING_RESPONSE  0x13
#define GMM_AUTHENTICATION_AND_CIPHERING_REJECT    0x14
#define GMM_AUTHENTICATION_AND_CIPHERING_FAILURE   0x1C
#define GMM_IDENTITY_REQUEST                       0x15
#define GMM_IDENTITY_RESPONSE                      0x16
#define GMM_STATUS                                 0x20
#define GMM_INFORMATION                            0x21





/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern void gmm_initialize( void );

extern void gmm_handle_message( mm_cmd_type *message );

extern void gmm_load_psloci_data( void );

extern void gmm_handle_plmn_found( serving_plmn_info_T serving_plmn,
                                   boolean location_update_required );


extern uint8 gmm_set_multislot_class
(
  uint8          ms_class
);

extern boolean set_access_tech_info_per_nas_stacks
(
  csn_encoder_T  *encoder ,
  byte           access_technology ,
  byte           revision_level_ind,
  byte           rf_power_class,
  hw_classmark_T *hw_classmark,
  boolean        *A5_already_included,
  boolean        *gprs_msclass_already_included,
  word           max_rac_length, /*0 means no length specified*/
  sys_modem_as_id_e_type sub_id,
  sys_modem_as_id_e_type as_id
);


#ifdef FEATURE_LTE
extern void gmm_perform_local_detach(void);
#endif
#endif /* GMM_H */
