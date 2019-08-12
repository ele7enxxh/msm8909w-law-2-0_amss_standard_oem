/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC L1 to MAC SIGNAL DEFINITION
***
***
*** DESCRIPTION
***
***  Signal definition for use with access function GPRS_MAC_l1_ACC_FUNC()
***
***  Defines the ENUM for all the signals used from L1 to MAC  .
***
***  Defines the Union that holds all the signal struct definitions
***  used from L1 to MAC
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacl1sig_g.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 09/11/10   sjw     Updated for Dual SIM support
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 08/25/06   og      EDTM addition of the mac_ph_cs_connect_ind message and
                       typedef.
*** 06/20/03   pjr         Added rrbp_fn to l1_mac_qual_measurements_t
*** 06/26/01   tlx     Created

*****************************************************************************/

#ifndef INC_GMACL1SIG_G_H
#define INC_GMACL1SIG_G_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "queue.h" // Added due to MAC/L1 merge WS.

#include "sys_type.h"

#include "csn.h"

#include "geran_multi_sim.h"

/*****************************************************************************
***
***     Private Type Definitions
***
*****************************************************************************/
typedef struct
{
  boolean   i_lev_ind;
  uint8     i_lev_tn;

} i_level_t;

typedef struct
{
  i_level_t  i_lev_tn0;
  i_level_t  i_lev_tn1;
  i_level_t  i_lev_tn2;
  i_level_t  i_lev_tn3;
  i_level_t  i_lev_tn4;
  i_level_t  i_lev_tn5;
  i_level_t  i_lev_tn6;
  i_level_t  i_lev_tn7;

} i_level_meas_t;

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*************************************************************************
*   "L1_MAC_QUAL_MEASURMENTS " L1 to MAC message structure
*************************************************************************/




/* Used during egprs UL tbf to indicate the absence
 * of the sign var parameter in the quality report.
 */
#define GMAC_INVALID_SIGN_VAR  0xFF


typedef struct
{
  boolean   mean_bep_ind;
  uint8     mean_bep;
} mean_bep_T;

typedef struct
{
  boolean   mean_cv_ind;
  uint8     mean_cv;
} mean_cv_bep_T;

typedef struct
{
  boolean         egprs_bep_qm_present;
  mean_bep_T      gmsk_mean_bep;
  mean_cv_bep_T   gmsk_cv_bep;
  mean_bep_T      eightpsk_mean_bep;
  mean_cv_bep_T   eightpsk_cv_bep;
} bep_qual_meas_T;


typedef struct
{
  boolean     egprs_ts_qm_present;
  mean_bep_T  mean_bep_tn0;
  mean_bep_T  mean_bep_tn1;
  mean_bep_T  mean_bep_tn2;
  mean_bep_T  mean_bep_tn3;
  mean_bep_T  mean_bep_tn4;
  mean_bep_T  mean_bep_tn5;
  mean_bep_T  mean_bep_tn6;
  mean_bep_T  mean_bep_tn7;
} ts_qual_meas_T;
#ifdef FEATURE_GPRS_PS_HANDOVER

typedef struct
{
  psho_status_t     psho_status;
  gas_id_t      gas_id;
}l1_mac_psho_cnf_t;

#endif /*FEATURE_GPRS_PS_HANDOVER*/
typedef struct
{
  uint8           c_value;
  uint8           rxqual;
  uint8           sign_var;
  i_level_meas_t  i_lev_meas;
  uint32          rrbp_fn;

  enum
  {
    GPRS_REPORT,
    EGPRS_REPORT
  } qual_meas_type;

  bep_qual_meas_T   egprs_bep_qual_meas;
  ts_qual_meas_T    egprs_bep_ts_qual_meas;
  gas_id_t          gas_id;
} l1_mac_qual_measurements_t;

/*************************************************************************
*   "MAC_PH_RANDOM_ACCESS_CONF " L1 to MAC message structure
*************************************************************************/
typedef struct
{
  boolean     tx_pcr;
  uint16      prach_fn;
  uint8       prach_ind;     /* can have a value of 0 or 1   */
                             /* and is used to pair up       */
                             /* MAC_PH_RANDOM_ACCESS_REQ and */
                             /* MAC_PH_RANDOM_ACCESS_CONF    */
  boolean    bad_cell_timeout;
  gas_id_t   gas_id;
} mac_ph_random_access_cnf_t;



/*************************************************************************
*   "L1_MAC_RRBP_FAILED_IND " L1 to MAC message structure
*************************************************************************/

typedef struct
{
  boolean rrbp_contained_chan_des;
  gas_id_t gas_id;
} l1_mac_rrbp_failed_ind_t;


#ifdef  FEATURE_GSM_DTM
#ifdef  FEATURE_GSM_EDTM

/*************************************************************************
*   "MAC_PH_CS_CONNECT_IND " L1 to MAC message structure
*************************************************************************/

typedef struct
{
  uint8  cs_timeslot;
  gas_id_t gas_id;
} mac_ph_cs_connect_ind_t;

#endif
#endif

/*************************************************************************
*   "L1_MAC_USF_DETECTED " L1 to MAC message structure
*************************************************************************/
  typedef struct
  {
    gas_id_t gas_id;
  } l1_mac_usf_detected_t;

/*************************************************************************
*   "L1_MAC_1ST_DATA_BLOCK_SENT " L1 to MAC message structure
*************************************************************************/
  typedef struct
  {
    gas_id_t gas_id;
  } l1_mac_1st_data_block_sent_t;

/*************************************************************************
*   "L1_MAC_51_SB_FAILURE " L1 to MAC message structure
*************************************************************************/
  typedef struct
  {
    gas_id_t gas_id;
  } l1_mac_51_sb_failure_t;

/*************************************************************************
*   "L1_MAC_SINGLE_BLOCK_SENT " L1 to MAC message structure
*************************************************************************/
  typedef struct
  {
    gas_id_t gas_id;
  } l1_mac_sb_sent_t;

/************************************************************************
* Union of all L1 to MAC message structure
*
*   note:- L1_MAC_1ST_DATA_BLOCK_SENT  carry no signal data
*          L1_MAC_USF_DETECTED
*          L1_MAC_51_SB_FAILURE
*          L1_MAC_SINGLE_BLOCK_SENT
*
*************************************************************************/

typedef union
{
  /* application dependent signals */
  l1_mac_tbf_freq_ass_cnf_t   tbf_freq_ass_cnf;
  l1_mac_qual_measurements_t  qual_measurements;
  l1_mac_tbf_rel_confrim_t    tbf_released;
  l1_mac_rrbp_failed_ind_t    rrbp_failed_ind;

#ifdef FEATURE_GPRS_PS_HANDOVER
  l1_mac_psho_cnf_t           psho_cnf;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

  /* GPRS specification primitives */
  mac_ph_connect_ind_t        connect_ind;
  mac_ph_data_ind_t           data_ind;
  mac_ph_random_access_cnf_t  random_access_cnf;

#ifdef  FEATURE_GSM_DTM
#ifdef  FEATURE_GSM_EDTM
  mac_ph_cs_connect_ind_t     cs_connect_ind;
#endif
#endif

  l1_mac_usf_detected_t        usf_detected;
  l1_mac_1st_data_block_sent_t first_data_block_sent;
  l1_mac_51_sb_failure_t       sb_51_failure;
  l1_mac_sb_sent_t             sb_sent;
} l1_mac_msg_u;


typedef struct
{
  q_link_type       link;
  byte              task_id;
  l1_mac_sig_id_t   sig_id;
  l1_mac_msg_u      msg;
} l1_mac_sig_t;


/* Size query function */
extern void new_gmac_trans_calc_e_pdan_size(
    egprs_chn_qual_rpt_t *egprs_qual_rpt,
    uint16               *size_with_qaul,
    uint16               *size_without_qual,
    boolean              *chan_req_present,
    gas_id_t              gas_id
);


#endif /* INC_GMACL1SIG_H */
/*** EOF: don't remove! ***/
