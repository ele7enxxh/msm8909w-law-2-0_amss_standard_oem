#ifndef TDSRRCMM_V_H
#define TDSRRCMM_V_H

#include "tdscdma_variation.h"
#include "tdsrrcmmif.h"

/*===========================================================================

                    TDSR R C    M MI 

DESCRIPTION
  This file contains TDS only internal references related 
  to the RRC NAS Interface.


  
  Copyright (c) 2010-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/



/*Below four functions will be revert and shared with TDS. 
  They will be removed after change other TDS RRC files 
  Please ignore when you do the code review
 
    tdsmm_rrc_plmn_lai_allowed
    tdsmm_rrc_registration_in_progress
    tdsmm_rrc_per_subs_plmn_lai_allowed
    tdsmm_per_subs_rrc_registration_in_progress
 
  */


/*===========================================================================

FUNCTION TDSMM_RRC_PLMN_LAI_ALLOWED : Reserve for future use. 

DESCRIPTION
  This function checks if the specified PLMN/LAI is alowed for CS or PS registration.
  This function checks if PLMN capability is matching with UE capability and
  the service domain is allowed (not barred) for registration or not.
  Moreover, in AUTOMATIC mode registration is allowed if LAI/PLMN is not in forbidden list.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if either CS or PS registration is possible/allowed in this LAI/PLMN.
  FALSE: Otherwise.


SIDE EFFECTS
  None

===========================================================================*/
boolean  tdsmm_rrc_plmn_lai_allowed(rrc_service_info_type service_info);



/*===========================================================================

FUNCTION TDSMM_RRC_REGISTRATION_IN_PROGRESS : Reserve for future use.

DESCRIPTION
  This function returns registration status
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE : registration in progress
  FALSE: registration in progress


SIDE EFFECTS
  This function shall be called only iff LAI is allowed according to tdsmm_rrc_plmn_lai_allowed

===========================================================================*/

boolean tdsmm_rrc_registration_in_progress(void);



#ifdef FEATURE_TDSCDMA_DSDS

/*===========================================================================

FUNCTION TDSMM_RRC_PER_SUBS_PLMN_LAI_ALLOWED : Reserve for future use.

DESCRIPTION
  This function checks if the specified PLMN/LAI is alowed for CS or PS registration.
  This function checks if PLMN capability is matching with UE capability and
  the service domain is allowed (not barred) for registration or not.
  Moreover, in AUTOMATIC mode registration is allowed if LAI/PLMN is not in forbidden list.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if either CS or PS registration is possible/allowed in this LAI/PLMN.
  FALSE: Otherwise.


SIDE EFFECTS
  None

===========================================================================*/
boolean  tdsmm_rrc_per_subs_plmn_lai_allowed(rrc_service_info_type service_info, sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION MM_PER_SUBS_RRC_REGISTRATION_IN_PROGRESS : Reserve for future use.

DESCRIPTION
  This function returns registration status
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE : registration in progress
  FALSE: registration in progress


SIDE EFFECTS
  This function shall be called only iff LAI is allowed according to tdsmm_rrc_plmn_lai_allowed

===========================================================================*/

boolean tdsmm_per_subs_rrc_registration_in_progress(sys_modem_as_id_e_type as_id);

#endif






#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
#define TDS_INVALID_CELL_ID 0xFFFFFFFF
#define TDS_INVALID_URA 0xFFFFFFFF

#define TDS_MAX_ASET_CELLS 6
#define TDS_MAX_MONITORED_FREQ 3
#define TDS_MAX_MONITORED_CELLS_PER_FREQ 8
#define TDS_MAX_MONITORED_GSM_CELLS 8

typedef struct
{
  /*Primary Scrambling Code*/
  uint8 cpid;

  /*RSCP*/
  int16 rscp;

  /*EC/Io*/
  int16 ecio;

} tds_cell_info_type;

typedef struct
{

  /*Serving Cell DL UARFCN*/
  uint16 uarfcn;

  /*Serving Cell Primay Scrambling Code*/
  uint8 cpid;

  /*PLMN ID*/
  rrc_plmn_identity_type plmn_id;

  /*Indicate the LAC*/
  uint8 lac[RRC_MAX_GSMMAP_LAC_LENGTH];
  
  /*Boolean indicated is RAC is present*/
  boolean is_rac_present;
  /*Indicate the RAC*/
  uint8 rac[RRC_MAX_GSMMAP_RAC_LENGTH];

  /*URA*/
  uint32 ura;

  /* Indicates the service cell ID */
  uint32 cell_id;

  /*Network Operation mode*/
  sys_network_op_mode_e_type nw_operation_mode;
} tds_serving_cell_info_type;



typedef struct
{
  /*Serving Cell Primary Scrambling Code*/
  uint8 cpid;

  /*RSCP*/
  int16 rscp;

  /*EC/Io*/
  int16 ecio;
}tds_serving_cell_power_info_type;

typedef struct
{
  /*DL UARFCN*/
  uint16 uarfcn;
  /*CPID*/
  uint8 cpid;

  /*PLMN ID*/
  rrc_plmn_identity_type plmn_id;

  /*Indicate the LAC*/
  uint8 lac[RRC_MAX_GSMMAP_LAC_LENGTH];
  
  /*Boolean indicated is RAC is present*/
  boolean is_rac_present;
  /*Indicate the RAC*/
  uint8 rac[RRC_MAX_GSMMAP_RAC_LENGTH];

  uint32 cell_id;
} tds_active_set_reference_rl_info_type;

typedef struct
{
  /*Number of RLs in active set*/
  uint8 num_rls;
  /*Primary Scrambling codes*/
  uint8 cpid[TDS_MAX_ASET_CELLS];

  uint32 cell_id[TDS_MAX_ASET_CELLS];

  /*RSCP*/
  int16 rscp[TDS_MAX_ASET_CELLS];

  /*EC/Io*/
  int16 ecio[TDS_MAX_ASET_CELLS];
} tds_active_set_info_type;

typedef struct
{
  /*UARFCN*/
  uint16 uarfcn;

  /*Number of cells in the frequency*/
  uint8 num_of_cells;

  /*Cells info*/
  tds_cell_info_type cell[TDS_MAX_MONITORED_CELLS_PER_FREQ];
} tds_monitored_freq_info_type;

typedef struct
{
  /*ARFCN*/
  uint16 arfcn;      /* Range 0-1023 */
  /*BSIC-NCC*/
  uint8 bsic_ncc;    /* Range 0-7, 0XFF is invalid */
  /*BSIC-BCC*/
  uint8 bsic_bcc;    /* Range 0-7, 0XFF is invalid */
  /*RSSI*/
  int16 rssi;
} tds_gsm_cell_info_type;

typedef struct
{
  /*Number of frequencies*/
  uint8 num_of_freqs;

  /*Freq info*/
  tds_monitored_freq_info_type freq[TDS_MAX_MONITORED_FREQ];

  /*Number of GSM cells*/
  uint8 num_gsm_cells;  /* Range 0-TDS_MAX_MONITORED_GSM_CELLS */

  /*GSM cell info*/
  tds_gsm_cell_info_type gsm_cell[TDS_MAX_MONITORED_GSM_CELLS];
} tds_monitored_set_info_type;


typedef struct
{
  tds_serving_cell_info_type wcdma_serving_cell_info;
  tds_serving_cell_power_info_type wcdma_serving_cell_power_info;
  tds_active_set_reference_rl_info_type  wcdma_active_set_reference_rl_info;
  tds_active_set_info_type wcdma_active_set_info;
  tds_monitored_set_info_type wcdma_monitored_set_info;
  sys_wcdma_rrc_state_e_type  wcdma_rrc_state;  /*sys_wcdma_rrc_state_e_type is defined in mmode/sys.h, which could be reused for TDS*/
} tds_event_info_type;


typedef struct
{
  /*Bitmask*/
  uint16 bitmask;

  tds_event_info_type wcdma_event_info;
} tds_UI_event_info_type;

/*===========================================================================

FUNCTION tdsrrc_set_modem_stats_mask 

DESCRIPTION
  This function updates Events bitmask stored at RRC

DEPENDENCIES
  None

RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/
void tdsrrc_set_modem_stats_mask (uint16 bit_mask);

/*===========================================================================

FUNCTION tdsrrc_get_modem_stats_info 

DESCRIPTION
  This function copies the TDS event fields to the stucture provided as arguments

DEPENDENCIES
  None

RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/
void tdsrrc_get_modem_stats_info (tds_UI_event_info_type *tdscdma_info);

#endif



#ifdef FEATURE_TDSCDMA_DSDS
/*===========================================================================

FUNCTION          tdsrrc_is_ps_abort_required

DESCRIPTION       This function is to be called by CM/SD to check if PS abort can be attemped on W
                         or G, for CS call on the other RAT.
                         1. In PCH/Connected Mode OOS states do not allow PS Abort
                         2. In FACH/DCH state allow PS abort.
                         3. If W is INACTIVE or PS domain in W is not active, then this API returns TRUE to 
                             allow PS abort on G.

DEPENDENCIES      None

RETURN VALUE      TRUE: To allow PS abort. and FALSE otherwise.

SIDE EFFECTS      None
===========================================================================*/
boolean tdsrrc_is_ps_abort_required(void);
#endif


#endif
