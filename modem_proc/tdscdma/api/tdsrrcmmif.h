#ifndef TDSRRCMMIF_H
#define TDSRRCMMIF_H

#include "rrcmmif.h"

/* Include the following two NAS header files to work with IMH_T */
#include "sys_v.h"

/*===========================================================================

                    TDSR R C    M M    I F

DESCRIPTION
  This file contains TDS only external references
  for the RRC NAS Interface.


  
  Copyright (c) 2000-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/





/*Let us discuss this file during next week's sync up meeting. 
  If we decide to reuse/override the internal fields, we may remove this file completely*/

typedef struct
{
  /* Indicates the service cell ID */
  uint32 cell_id;
  /* Indicates the selected PLMN */
  rrc_plmn_identity_type plmn_id;
  /* Indicates the LAC ID */
  uint8                     lac[RRC_MAX_GSMMAP_LAC_LENGTH];
  /* Indicates the serving PSC */
  uint16 psc;
  /*DL UARFCN*/
  uint16 uarfcn_dl;
  /*UL UARFCN*/
  uint16 uarfcn_ul;
/*temporary mainline  FEATURE_DUAL_SIM*/
  sys_modem_as_id_e_type as_id;
/*FEATURE_DUAL_SIM*/
} tdscell_info_type;
#ifdef FEATURE_CMCC_SECURE_CALL
typedef enum
{
  RRC_SECURE_VOICE_NULL,
  RRC_SECURE_VOICE_ENABLE
}rrc_secure_voice_status_e_type;

typedef struct
{
  rrc_secure_voice_status_e_type secure_voice_status;
  sys_modem_as_id_e_type as_id;
}tdsrrc_secure_call_ind_type;
#endif
/* Callback function type for cell ID change notification */
typedef 
void TDSRRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE(tdscell_info_type cell_info);


typedef struct 
{
  boolean                          dual_standby_mode;
  sys_modem_dual_standby_pref_e_type  dual_standby_mode_info;
  sys_modem_device_mode_e_type        device_mode;
}tdsrrc_ds_status_change_ind_type;

typedef struct
{
  sys_modem_as_id_e_type as_id;
}tdsrrc_subscription_id_type;

typedef struct 
{
  boolean  tuneaway_status; /* FALSE indicate disable tuneaway and TRUE indicates enable tuneway*/
} tdsrrc_ds_tuneaway_status_change_ind_type;

/*===========================================================================

FUNCTION tdsrrc_malloc_for_external_cmd

DESCRIPTION
  This function is called by the external modules like NAS in order to allocate 
  dynamic memory. This function simply calls mem_malloc and selects suitable
  heap and returs pointer to the allocated memory.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None.

===========================================================================*/
extern void * tdsrrc_malloc_for_external_cmd
(
size_t size
);

/* ==============================================================================
  Temporary make below inclusion relation
  Will make the tdsrrcmmi.h to include tdsrrcmmif.h later and sync together with other RRC files
  =============================================================================*/



/***********************************************************************/
/************************ SYS_MODE_CODEC_HO_INFO ************************/
/***********************************************************************/

/*Indicates TDSCDMA and INTER-RAT voice handover progress and the current codec being used*/
  
/* Callback function type for CODEC info and ho type notification */
typedef 
void TDSRRC_CODEC_HO_INFO_CB_FUNC_TYPE(sys_codec_ho_info eng_mode_codec_ho_info);

/*===========================================================================

FUNCTION tdsrrc_register_rrc_codec_ho_info_cb

DESCRIPTION
  This function is the API to register the function pointer for CM call back .

DEPENDENCIES


RETURN VALUE
  log_message_type

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrc_register_rrc_codec_ho_info_cb
(
  TDSRRC_CODEC_HO_INFO_CB_FUNC_TYPE *cb_ptr
);
/*===========================================================================

FUNCTION tdsrrc_deregister_rrc_codec_ho_info_cb

DESCRIPTION
  This function is the API to de-register the function pointer for CM call back.

DEPENDENCIES


RETURN VALUE
  log_message_type

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrc_deregister_rrc_codec_ho_info_cb (void);

/*===========================================================================

FUNCTION TDSRRC_REGISTER_SERVING_CELL_CHANGE_IND_CB

DESCRIPTION

This function enables upper layer to register a callback function if notification
of the serving cell id is required.  Once the function is registered, it will be 
called whenever there is a serving cell change.


DEPENDENCIES

  None.
  
RETURN VALUE
  
  BOOLEAN:  TRUE signifies that registration has succeeded.  FALSE signifies
            that registration failed.

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_register_serving_cell_change_ind_cb 
(
  TDSRRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE* cb_ptr
);

/*===========================================================================

FUNCTION tdsrrcrcr_is_proc_active_sub

DESCRIPTION
  Indicates whether RCR is active or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If UE is Multi-SIM, and if RCR is in progress on as_id given by NAS
        If UE is single SIM and RCR is in progress.

  FALSE:If UE is in Multi-SIM and as_id doesn't match the ID on which TDS is active
        Else If RCR is not in progress at TDSRRC


SIDE EFFECTS
  None
===========================================================================*/

boolean tdsrrcrcr_is_proc_active_sub(sys_modem_as_id_e_type as_id);
#endif

