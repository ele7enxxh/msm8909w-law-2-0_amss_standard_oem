#ifndef WMCVSDRV_H
#define WMCVSDRV_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         WCDMA MCVS Driver interface

GENERAL DESCRIPTION

  This file contains the source code for the WCDMA Modem Power Abstraction
  layer implementation.


Copyright (c) 2000-2015 by Qualcomm Technologies, Inc. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wmcvsdrv.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when           who      what, where, why
--------       ----     ---------------------------------------------------------- 
24/02/16       kr       Update srch cycle count to Max, it will help in cases of dense NW, where UTIL is >85%
09/29/15       bj       Increase the split acq step1+NASTT step lock duration from  80 to 85 ms 
                        and boost the clock level to NOM for NASTT search in ACQ state to avoid 1x page miss.
09/11/15       stk      FR 30139: SVS+ and NOM+ frequency corner support in MCVS
04/15/15       sks      Bump Q6 to NOM if in a voice call with multiple PDPs on HS and CM also active. 
02/06/15       sks      QUery Triage db during HSPA request and clear the CME db at QICE relinquish.
02/04/15       sks      Add changes to bump up clk to 576 Mhz if EUL 2ms + CM is active. Change CM client to MCVS. 
11/26/14       sks      Check for a modified QSET index in case of DC.
11/11/14       sks      Temp fix for JO to take care of HW turbo speed capped at 345.6 MHz
11/7/14        sks      Redesign of MCVS algo.
10/1/14        stk      Fixed the NV bug for clock bumpup and added support for THOR clk bump up via NV
09/16/14       stk      JO SW-FW interface change to support Q6 clock speeds 
08/08/14       sks      Initial Version. MCPM/MCVS code cleanup and JO MCVS 

 ===========================================================================*/

#include "rex.h"
#include "mcpm_api.h"

#ifdef FEATURE_WCDMA_MCVS

/* ******************************************** */
/*                ENUM DECLARATIONS             */
/* ******************************************** */

/* MCVS clients */
typedef enum
{
  WCDMA_WMCPM_MCVSDRV_VPE_PULLIN_UPDATE,      /* 0 */
  WCDMA_WMCPM_MCVSDRV_VPE_UPDATE,             /* 1 */  
  WCDMA_WMCPM_MCVSDRV_SRCHCLK_UPDATE,         /* 2 */
  WCDMA_WMCPM_MCVSDRV_VPE_UPDATE_CME,         /* 3 */
  WCDMA_WMCPM_MCVSDRV_CLIENT_CM,              /* 4 */
  WCDMA_WMCPM_MCVSDRV_Q6B_NASTT_UPDATE        /* 5 */
}wmcvsdrv_client_type;

/* ******************************************** */
/*               COMMON GLOBAL VARIABLES        */
/* ******************************************** */

#define MAX_CARRIERS 2

/* offline clock speed enums no longer needed */
typedef struct 
{
   /*# SHO fingers */
   uint8 num_aset_fingers[MAX_CARRIERS];
      /*# SHO cells */
   uint8 num_sho_cells[MAX_CARRIERS];
   /*number of neighbors*/
   uint8 num_neighbor_cells[MAX_CARRIERS];
   /*Total number of fingers*/
   uint8 tot_num_of_fingers[MAX_CARRIERS];
   /* total number of cells will be used for PULL in*/
   uint8 total_num_cells;
   /*TxD is present or not*/	  
   boolean status_txd[MAX_CARRIERS];
   /*PCCPCH  info in DCH */
   boolean pccpch_enabled ;
}wmcpmdrv_mcvs_vpe_update_struct_type;

extern wmcpmdrv_mcvs_vpe_update_struct_type wmcpmdrv_mcvs_vpe_update_params;
extern uint8 wmcpmdrv_mcvs_cme_mask;
extern uint32 wmcvs_debug_bumpup_vpe_offline;
extern mcpm_mcvsrequest_parms_type wmcpmdrv_wmcvs_request_params;
extern uint16 wmcpm_mcvs_int_srch_period;
extern uint32 wmcvsdrv_int_client_bmask;

#define WMCVS_PERCENT_UTIL_HIGH   85
#define WMCVS_PERCENT_UTIL_LOW    80
#define WMCVS_SRCH_PERIOD360      36
#define WMCVS_SRCH_PERIOD180      18

#define WMCVSDRV_SET_INT_CLIENT_BMSK(client) \
do{ \
    wmcvsdrv_int_client_bmask |= ((0x1) << client); \
}while(0)

#define WMCVSDRV_RESET_INT_CLIENT_BMSK(client) \
do{ \
    wmcvsdrv_int_client_bmask &= ~((0x1) << client); \
}while(0)

#define WMCVSDRV_CLIENT_MASK_VPE_UPDATE_CME             (0x1 << (int32)WCDMA_WMCPM_MCVSDRV_VPE_UPDATE_CME)
#define WMCVSDRV_CLIENT_MASK_CM                         (0x1 << (int32)WCDMA_WMCPM_MCVSDRV_CLIENT_CM)

#define IS_VPE_CME_ACTIVE()                             (wmcvsdrv_int_client_bmask & WMCVSDRV_CLIENT_MASK_VPE_UPDATE_CME)
#define IS_CM_ACTIVE()                                  (wmcvsdrv_int_client_bmask & WMCVSDRV_CLIENT_MASK_CM)

/* bits 1 and 2 are set by CME module to indicate >= 2 QSET cells on carriers 0 and 1 respectively.*/

#define WMCVSDRV_FING_STEP 3
#define WMCVDRV_CELL_STEP 2
#define WMCVS_MAX_SRCHR_MODES 2

typedef enum
{
   MCVS_STANDALONE,
   MCVS_THROUGH_MCPM
}wmcvsdrv_q6_mcvs_req_type;

typedef enum
{
  WMCVS_Q6CLK_NULL      = 0, 
  WMCVS_Q6CLK_SVS       = 384000,  /* SVS  */
  WMCVS_Q6CLK_SVS_PLUS  = 480000,  /* SVS+ */
  WMCVS_Q6CLK_NOM       = 576000,  /* NOM  */
  WMCVS_Q6CLK_NOM_PLUS  = 615000,  /* NOM+ */
  WMCVS_Q6CLK_TURBO     = 691200   /* TURBO*/  
} wmcpmdrv_mcvs_q6clk_speedType;


typedef enum
{
   W_Q6_INVALID = MCPM_CLK_NULL,   
   W_Q6_CLK_19_2M = MCPM_CLK_19_2M,   
   W_Q6_115M = MCPM_CLK_Q6_115M,
   W_Q6_144M = MCPM_CLK_Q6_144M, /* low */
   W_Q6_230M = MCPM_CLK_Q6_230M,
   W_Q6_288M = MCPM_CLK_Q6_288M, /* low */
   /* following 2 levels are a temp fix for JO. HW turbo is currently capped at 345.6 MHz.
      also SVS levels are mostly going to change from 384 to 370 on later versions */
   W_Q6_345M = MCPM_CLK_Q6_345M,
   W_Q6_370M = MCPM_CLK_Q6_370M,
   W_Q6_384M = MCPM_CLK_Q6_384M, /* SVS  */
   W_Q6_480M = MCPM_CLK_Q6_480M, /* SVS+ */
   W_Q6_576M = MCPM_CLK_Q6_576M, /* NOM  */
   W_Q6_615M = MCPM_CLK_Q6_615M, /* NOM+ */
   W_Q6_691M = MCPM_CLK_Q6_691M  /* TURBO*/  
} wmcpmdrv_mcpm_offline_clk_speed_type;


extern uint8 wmcvsdrv_srch_cycle_count;

extern wmcpmdrv_mcvs_q6clk_speedType wmcpmdrv_mcvs_int_q6clk_speed;

/*===========================================================================
FUNCTION     wmcvsdrv_compute_q6_change

DESCRIPTION
Checks the switching conditions for NOM/TURBO in SC/DC HS for pure MCVS req./MCVS piggy backed on MCPM.
SC-HS : 
   NO QICE : SAME TABLE AS NON-HS
   EQ ONLY: SAME TABLE AS NON-HS
   ELSE: NOMINAL
 
DC-HS : 
   NO QICE : SAME TABLE AS NON-HS
   QSET = 1 AND NO TD : NOMINAL
   ELSE :TURBO
 

 
PARAMETERS
none

RETURN VALUE

SIDE EFFECTS
none 
 
===========================================================================*/
void wmcvsdrv_compute_q6_change(wmcvsdrv_q6_mcvs_req_type req_type);

/*===========================================================================
FUNCTION     wmcvsdrv_get_qset_txd_status

DESCRIPTION
  This function gets TxD status and QSET size.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wmcvsdrv_get_qset_txd_status(void);

/*===========================================================================
FUNCTION     wmcvsdrv_ret_txd_count

DESCRIPTION
returns the TxD count - 1/2.

PARAMETERS
none

RETURN VALUE
uint8

SIDE EFFECTS
none 
 
===========================================================================*/

boolean wmcvsdrv_ret_txd_status(void);

/*===========================================================================
                          COMMON LOCAL/EXTERN Function Declaration
============================================================================*/
/*===========================================================================
FUNCTION     wmcpmdrv_mcvs_config_wrapper

DESCRIPTION
  Wrapper function to send MCVS_Config_modem with W globals set and reset. 
  This W global variable will be checked in the offline_clk_change_cb() when offline clk changes. 
  
PARAMETERS
  None.

RETURN VALUE

SIDE EFFECTS
  None. No SRCH abort should happen through MCVS, since searcher module takes care of it. 
===========================================================================*/
LOCAL void wmcpmdrv_mcvs_config_modem_wrapper(void);

/*===========================================================================
FUNCTION     wmcvsdrv_clear_mvcs_db()

DESCRIPTION
  memset WMCVS request_parms and all globals to zero.
  This will be called during W stack init and de-init
 
PARAMETERS
  None.
 
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wmcvsdrv_clear_wmvcs_db(void);

/*===========================================================================
FUNCTION     WMCPMDRV_MCVS_UPDATE_SRCH_PERIODICITY

DESCRIPTION
function to return the new/updated srch period.
 
PARAMETERS
  None.

RETURN VALUE
updated srch period

SIDE EFFECTS
  None.
===========================================================================*/
uint8 wmcpmdrv_mcvs_update_srch_periodicity(void);

/*===========================================================================
FUNCTION     WMCPMDRV_MCVS_UPDATE_SRCH_CLK_TO_SRCH

DESCRIPTION
function to return the offline clk freq in Mhz.
 
PARAMETERS
  None.

RETURN VALUE
offline clk freq in Mhz

SIDE EFFECTS
  None.
===========================================================================*/
uint8 wmcpmdrv_mcvs_update_srch_clk_to_srch(void);

/*===========================================================================
FUNCTION     wmcpmdrv_mcvs_get_triage_param_update

DESCRIPTION
  Wrapper function for retrieving TxD Status from WL1

PARAMETERS
  None.

RETURN VALUE
  TxD_OFF/TxD_ON/TxD_DC

SIDE EFFECTS
  None.
===========================================================================*/
void wmcpmdrv_mcvs_get_triage_param_update(wmcpmdrv_mcvs_vpe_update_struct_type* vpe_params );

/*===========================================================================
FUNCTION     wmcpmdrv_mcvs_get_Pullin_cell_cnt

DESCRIPTION
  Gets the cell cnt from demod during pull in procedure.
 
PARAMETERS
 global VPE struct Address

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wmcpmdrv_mcvs_get_pullin_cell_cnt(wmcpmdrv_mcvs_vpe_update_struct_type *vpe_params);

/*===========================================================================
FUNCTION     wmcpmdrv_q6clk_update_in_mcvs_param

DESCRIPTION
function to send MCVS request when not already running at desired Q6 frequency
pure MCVS req

PARAMETERS
Q6 frequency wanted

RETURN VALUE
None

SIDE EFFECTS
Blocking call for x usec
===========================================================================*/
void wmcpmdrv_q6clk_update_in_mcvs_param(wmcpmdrv_mcvs_q6clk_speedType q6_speed,mcvs_request_type req_type);
/*===========================================================================
FUNCTION     wmcpmdrv_q6clk_update_in_mcpm_param

DESCRIPTION
function to send MCVS request when not already running at desired Q6 frequency
MCVS embedded in mcpm req.

PARAMETERS
Q6 frequency wanted

RETURN VALUE
None

SIDE EFFECTS
Blocking call for x usec
===========================================================================*/
void wmcpmdrv_q6clk_update_in_mcpm_param(wmcpmdrv_mcvs_q6clk_speedType q6_speed,mcvs_request_type req_type);

/*===========================================================================
FUNCTION     WMCVSDRV_REQUEST_CLIENT_POWER_CONFIG

DESCRIPTION
  Wrapper function for W MCPM clients to request for power resources.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wmcvsdrv_request_client_power_config(wmcvsdrv_client_type client);

/*===========================================================================
FUNCTION     WMCVSDRV_RELINQUISH_CLIENT_POWER_CONFIG

DESCRIPTION
  Wrapper function for W MCPM clients to relinquish for power resources.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wmcvsdrv_relinquish_client_power_config(wmcvsdrv_client_type client);

/*===========================================================================
FUNCTION     WMCPMDRV_GET_WCDMA_Q6_CLK_SPEED

DESCRIPTION
Called from inside the clock change callback to find out the new speed.

PARAMETERS
none

RETURN VALUE
none

SIDE EFFECTS
none 
 
===========================================================================*/
void wmcpmdrv_get_wcdma_q6_clk_speed(void);

uint8 wmcvsdrv_modulo(uint8 x,uint8 y);
/*===========================================================================
FUNCTION     WMCVSDRV_CLEAR_MCVS_CME_INFO

DESCRIPTION
  Clears the CME client from the client bitmask and clears the CME database.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wmcvsdrv_clear_mcvs_cme_info(void);

/*===========================================================================
FUNCTION     WMCVSDRV_UPDATE_MPDP_STATUS

DESCRIPTION
  Updates the mpdp status with the RRC input received from the CPHY_SETUP req

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wmcvsdrv_update_mpdp_status(boolean mrab_status);

/*===========================================================================
FUNCTION     wmcvsdrv_convert_sys_array_type_to_khz

DESCRIPTION
This converts the Q6 level passed to the Q6 MCVS type.

PARAMETERS
none

RETURN VALUE
wmcpmdrv_mcvs_q6clk_speedType (Q6 speed in KHz)
 
SIDE EFFECTS
none 
 
===========================================================================*/
extern wmcpmdrv_mcvs_q6clk_speedType wmcvsdrv_convert_sys_array_type_to_khz(wmcpmdrv_mcpm_offline_clk_speed_type q6);

#endif /*FEATURE_WCDMA_MCVS*/

#endif /* ifndef WMCVSDRV_H */
