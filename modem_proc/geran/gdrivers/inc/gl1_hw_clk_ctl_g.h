#ifndef GL1_HW_CLK_CTL_G_H
#define GL1_HW_CLK_CTL_G_H
/*===========================================================================
                      GSM GPRS NPL1 FRAME LAYER HEADER FILE
DESCRIPTION
   This file defines the interfaces to the GSM/GPRS NPL1 FRAME LAYER.
   The GSM/GPRS NPL1 frame layer abstracts all hardware concepts. The frame
   layer functions typically need to be called periodically every frame.

   The implentation for the functions are distributed in several files.
   gl1_hw.c           - File handling common functionality.
   gl1_hw_gsm.c       - File handling GSM/GPRS frame layer functionality
   gl1_hw_gprs.c      - File handling GPRS functionality.
   gl1_hw_sleep.c     - File handling micro shutdown for sleep.
   gl1_hw_sleep_ctl.c - File handling sleep controller and L1 sleep.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_hw_clk_ctl_g.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
16/04/14    ws      CR 645067 - Increase Q6 clk speed when AEQ2A is enabled in EGPRS mode
22/11/13    cja     CR579775 On Dime use MCVS for settinig clocks for TCH AEq and speed-up
21/11/13    cja     CR455527 Mainline or remove features to tidy up code.
03/09/13    cja     CR538140 Add mcpm_tx_on flag for race condition in sending Tx Config
28/08/13    cja     CR535578 Ensure wait for MCPM Geran start during init
21/08/13    npt     CR531633 - Remove FEATURE_DIME_MODEM dependencies
28/03/13    pg      CR467324 Do not disable G2W clocks whilst OLS is pending
14/12/12    cja     CR426248 Wait for HW Tx Off before MCPM request for voice stop.
28/11/12    pg      CR420950: add MCPM wakeup voting for GLLC ciphering
14/11/12    npt     CR420954 - Added changes for power collapse
23/10/12    npt     CR413635 - Port sleep code for DIME modem
26/03/12    pg      Make FEATURE_MCPM and FEATURE_DYNAMIC_CLOCK_VOLTAGE_SCALING mutually exclusive
05/12/11    cja     CR323200 Run MCPM start req via dpc, wait for it to complete
17/11/11    cja     CR319954 Do not sleep unless GSM MCPM is idle
07/11/11    cja     CR311154 Add request MCPM AEq on for AMR
09/09/11    cja/ws  CR301686. Use DPC for MCPM update state
31.05/11    ws      Fixed Compile warning in gl1_hw_get_mcpm_state()
24/05/11    cja     Add get gl1_mcpm state
19/04/11    cja     Add cipher clock gating for Nikel
30/03/11    cja     Initial IRAT for Nikel
09/05/11    ky      Increase dedicated mips to 120 for only QSC6695 and dont increase MIPS For 7x30.LA.4.1
03/05/11    ky      CR 285501 - Increase MIPS for SCMM dedicated mode from 96 to 120
10/03/11    ws      CR276952 Added EBI bus request for EGPRS Data mover access
22/02/11    ip/cs   CR267278 GP clock vote handled by RF drivers
21/01/11    ws      CR 263081 - Implement Latency node of 1mSec
08/12/10    ws      CR 267264 - Merge NPA clients for CPU resource to 1 client to avoid
                    mutiple npa_cancel_request() which can take 1-2 mSecs to return
10/02/11    pg      Fix compilation error on enabling DEBUG_GL1_CLK
10/01/11    ip      CR 269097 - Increase MIPS for 7x30 dedicated mode
02/12/10    nf      9K changed the way clk delays and clk switching is handled
18/11/10    nf      9K added handling for clock speed based on efs value
16/11/10    ip      CR263589 Changed idle mips to 340 from 384 MHz
20/09/10    og      Added LTE clock regime client support.
15/09/10    ip      CR248185 Increase GSM IDLE MIPS to use 384 for Poseidon2
22/07/10    ip      CR240253 Increased the Edge MIPS
05/07/10    rc      Added support for GEA register access if GP clock is off in Q6
18/06/10    ip      CR241108 add funtionality to enable/disable SSBI clk
09/06/10    ip      CR240254 - SCMM NPA MIPS changes
07/05/10    ip      CR235204 DCVS settings added for Poseidon 2
19/04/10    cs      Increase to 270MIPS for idle/edge as DVCS switches speed now
09/04/10    ip      CR221022 Added extra FEE function call to block estimation
06/04/10    ip      CR233495 Set the CPU speeds at 122MHz until optimised
01/04/10   tjw      Add customer.h and comdef.h to all SU API files
25/03/10    cs      Fix debug clock info for poseidon
24/03/10    ip      CR231770 - Added GLLC client control for GSAC
24/03/10    ip      CR231696 - Set Default mDSP clock speed
11/03/10    cs      CR229926 NPA robustness improvements
04/03/10    ip      CR228168 Added DEVMAN code changes for mDSP speed
08/02/10    ws      Merged QDSP6 changes back to mainline
26/01/10    ip      CR224128, Added clock gating for SCMM
04/11/09    ip      Added first draft of NPA for cpu clock resource
05/08/09    ws      Removed FEATURE_GSM_MDSP_DTM it's now mainlined
30/03/09    ws      Added FEATURE_GSM_GPRS_QDSP6 and FEATURE_GSM_GPRS_POSEIDON
                    to replace FEATURE_GSM_GPRS_MUSTANG
19/12/08    ip      CR167592 Added additional LCU clock switching
01/12/08    cs      Initial version based on gl1_hw.h

===========================================================================*/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "rex.h"
#include "gl1_defs.h"
#include "gl1_defs_g.h"
#include "rfcom.h"
#include "sys_type.h"  /* dBx16_T */
#include "gl1_sys_algo.h"

#include "mcpm_api.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#ifdef DEBUG_GL1_CLK
#error code not present
#endif

/* -----------------------------------------------------------------------
**                             GSM Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                             GSM/GPRS Declarations
** ----------------------------------------------------------------------- */

/* Which type of clock to en/disable */
typedef enum
{
   GL1_NULL_CLK,
   GL1_CHIPXN_DIV2_CLK,
   GL1_CHIPXN_DIV4_CLK,
   GL1_SAMPLE_RAM_CLK,
   GL1_GSM_CLK,
   GL1_INIT_WCDMA_CLK,
   GL1_ENCRYPT_CLK,
   GL1_GSAC_CLK,
   GL1_CLK_ALL,

   GL1_EDGE_CLK,

   GL1_STMR_CLK,

   GL1_GSM_TX_CLK,

   GL1_WCDMA_CLK,

   GL1_CM_GSM_RX_CLK,
   GL1_WCDMA_CM_INIT_CLK,
   GL1_GSM_CM_INIT_CLK,

#if !(defined( FEATURE_INTERRAT_HANDOVER_GTOW ) || \
      defined( FEATURE_INTERRAT_CELL_RESELECTION_GTOW ) || \
      defined( FEATURE_INTERRAT_HANDOVER_WTOG ) || \
      defined( FEATURE_INTERRAT_CELL_RESELECTION_WTOG ) )
   GL1_WCDMA_INACT_CLK,
#endif

   GL1_GSM_ENABLE_SLEEP_CLK,
   GL1_GSM_DISABLE_SLEEP_CLK,
   GL1_WCDMA_FORCE_WAKEUP_CLK,

#if defined ( MDSP_agcTrkLoAdjShadow )
   GL1_MDSP_CLK,
#endif /* MDSP_agcTrkLoAdjShadow */

   GL1_LEAVING_DRX,
   GL1_ENTER_IDLE,
   GLLC_GSAC_CLK,

#ifdef  FEATURE_LTE_TO_GSM

   GL1_LTE_CM_INIT_CLK,

#endif

   GL1_HW_CLK_MAX

} gl1_hw_clk_type;

#ifdef DEBUG_GL1_CLK
#error code not present
#endif

/****************************************************************************
                            GSM/GPRS FUNCTION PROTOTYPES
 ***************************************************************************/

/*===========================================================================
FUNCTION gl1_hw_gllc_encrypt_enable

DESCRIPTION
  This function enables the tripler and encrypt clock for GLLC.
  Also stops GL1 from going to sleep as that would disable tripler.

RETURN
  None
===========================================================================*/
extern void gl1_hw_gllc_encrypt_enable( void );

/*===========================================================================
FUNCTION gl1_hw_gllc_encrypt_disable

DESCRIPTION
  This function disables the tripler and encrypt clock for GLLC.
  Also allows GL1 to sleep which will disable tripler.

RETURN
  None
===========================================================================*/
extern void gl1_hw_gllc_encrypt_disable( void );

/*===========================================================================
FUNCTION gl1_hw_cipher_clock_required
DESCRIPTION
  Sets a gsac clock is required or not, for turning on/off clock
===========================================================================*/
void gl1_hw_cipher_clock_required(boolean cipher_clk_on, gas_id_t gas_id);

/*===========================================================================
FUNCTION  gl1_hw_mcpm_state_update
===========================================================================*/
typedef enum
{
  GL1_MCPM_GERAN_STOP     = 0,
  GL1_MCPM_GERAN_START    = 1,
  GL1_MCPM_GERAN_IDLE     = 2,
  GL1_MCPM_GERAN_TX_ON    = 3,
  GL1_MCPM_GERAN_TX_OFF   = 4,
  GL1_MCPM_GERAN_RAND     = 5,
  GL1_MCPM_GERAN_VOICE    = 6,
  GL1_MCPM_GERAN_DATA     = 7,
  GL1_MCPM_GERAN_DTM      = 8,
  GL1_MCPM_GERAN_SLEEP    = 9,
  GL1_MCPM_GERAN_FTM      = 10,
  GL1_MCPM_GERAN_STARTING = 11
} gl1_mcpm_state_type;

void gl1_hw_mcpm_state_update (mcpm_request_type req_state, gl1_mcpm_state_type gl1_state,gas_id_t gas_id);
void gl1_hw_mcpm_state_update_dpc(mcpm_request_type req_state, gl1_mcpm_state_type gl1_state,gas_id_t gas_id);
void gl1_hw_mcpm_gts_apc( gas_id_t gas_id );



/*===========================================================================

FUNCTION  gl1_egprs_mcpm_change
DESCRIPTION
  This function requeste MCPM change at start/end of EGPRS mode so that
  clock speed is increased during AEQ-2A algorithm running in GFW

===========================================================================*/
void g11_hw_egprs_mcpm_change( boolean egprs_start, gas_id_t gas_id );
/*===========================================================================
FUNCTION  gl1_hw_mcpm_neighbor_update
===========================================================================*/
void gl1_hw_mcpm_neighbor_update (mcpm_nbr_meas_type req_nbr,gas_id_t gas_id);
/*===========================================================================
FUNCTION gl1_hw_mcpm_gprs_aeq_update
===========================================================================*/
void gl1_hw_mcpm_gprs_aeq_update (boolean aeq_on,gas_id_t gas_id);
/*===========================================================================
FUNCTION gl1_hw_mcpm_amr_aeq_update
===========================================================================*/
void gl1_hw_mcpm_amr_aeq_update (boolean aeq_on, gas_id_t gas_id);
/*===========================================================================
FUNCTION gl1_mcvs_clock_set
===========================================================================*/
boolean gl1_mcvs_clock_set(boolean SpeedReq, uint16 SpeedState, gas_id_t gas_id);
/*===========================================================================
FUNCTION g1l_hw_is_mcpm_idle
===========================================================================*/
boolean g1l_hw_is_mcpm_idle (gas_id_t gas_id);
/*===========================================================================
FUNCTION g1l_hw_is_mcpm_tx_on
===========================================================================*/
boolean g1l_hw_is_mcpm_tx_on (gas_id_t gas_id);
/*===========================================================================
FUNCTION gl1_hw_mcpm_tx_on_gas1_cb
===========================================================================*/
void gl1_hw_mcpm_tx_on_gas1_cb (void);
/*===========================================================================
FUNCTION gl1_hw_mcpm_tx_on_gas2_cb
===========================================================================*/
void gl1_hw_mcpm_tx_on_gas2_cb (void);
/*===========================================================================
FUNCTION gl1_hw_mcpm_tx_on_gas3_cb
===========================================================================*/
void gl1_hw_mcpm_tx_on_gas3_cb (void);
/*===========================================================================
FUNCTION g1l_hw_set_mcpm_tx_on
===========================================================================*/
void gl1_hw_set_mcpm_tx_on (boolean mcpm_tx_on_setting, gas_id_t gas_id);
/*===========================================================================
FUNCTION gl1_hw_mcpm_set_state
===========================================================================*/
void gl1_hw_mcpm_set_state(gl1_mcpm_state_type mcpm_state,gas_id_t gas_id);
/*===========================================================================
FUNCTION gl1_hw_mcpm_get_state
===========================================================================*/
gl1_mcpm_state_type gl1_hw_mcpm_get_state(gas_id_t gas_id);
/*===========================================================================
FUNCTION gl1_hw_mcpm_config_g2w_stop
===========================================================================*/
void gl1_hw_mcpm_config_g2w_stop ( sys_modem_as_id_e_type as_id );


#endif /* GL1_HW_CLK_CTL_H */

/* EOF */
