#ifndef RR_MESSAGE_SET_H
#define RR_MESSAGE_SET_H

/*============================================================================
  @file rr_message_set.h

  This header file defines the set of modules that may send and receive
  messages on the RR signalling interface (MS_RR_RR).

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_message_set.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  /* rr_control */
  RR_IMSG = 0x00,
  /* rr_control is the top-level state machine so has no output as output is
  generally directed from a state-machine to it's parent. */
  /* RR_OMSG, */

  /* grr_control */
  RR_GRR_IMSG = 0x01,
  RR_GRR_OMSG = 0x02,

  /* rr_gprs_cell_reselection */
  RR_RESEL_IMSG = 0x03,
  RR_RESEL_OMSG = 0x04,

  /* rr_pscan */
  RR_PSCAN_IMSG = 0x05,
  RR_PSCAN_OMSG = 0x06,

  /* rr_pscan */
  RR_DECODE_BCCH_IMSG = 0x07,
  RR_DECODE_BCCH_OMSG = 0x08,

  /* rr_inter_cell_handover_control */
  RR_INTER_HO_IMSG = 0x09,
  RR_INTER_HO_OMSG = 0x0a,

  /* rr_cell_acq_control */
  RR_CELL_ACQ_IMSG = 0x0b,
  RR_CELL_ACQ_OMSG = 0x0c,

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  /* rr_resel_g2w_control */
  RR_RESEL_G2W_IMSG = 0x0d,
  RR_RESEL_G2W_OMSG = 0x0e,

  /* rr_x2g_resel_control */
  RR_X2G_RESEL_IMSG = 0x0f,
  RR_X2G_RESEL_OMSG = 0x10,
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

  /* rr_select_bcch_control */
  RR_SELECT_BCCH_IMSG = 0x11,
  RR_SELECT_BCCH_OMSG = 0x12,

#ifdef FEATURE_FEMTO_GSM_NL
  /* rr_femto_control */
  RR_FEMTO_IMSG = 0x13,
  RR_FEMTO_OMSG = 0x14,
#endif /* FEATURE_FEMTO_GSM_NL */

  /* rr_connection_establishment_control */
  RR_CE_IMSG = 0x15,
  RR_CE_OMSG = 0x16,

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS_G2T_REDIRECTION)
  RR_G2X_REDIR_IMSG = 0x17,
  RR_G2X_REDIR_OMSG = 0x18,
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE_TO_GSM_CGI
  RR_X2G_CGI_IMSG = 0x19,
  RR_X2G_CGI_OMSG = 0x1a,
#endif /* FEATURE_LTE_TO_GSM_CGI */

  RR_L1_IDLE_IMSG = 0x1b,
  RR_L1_IDLE_OMSG = 0x1c,

  RR_PS_IMSG = 0x1d,
  RR_PS_OMSG = 0x1e,

  RR_DTM_IMSG = 0x1f,
  RR_DTM_OMSG = 0x20,

  RR_SGLTE_BAND_COEX_IMSG = 0x21,
  RR_SGLTE_BAND_COEX_OMSG = 0x22,

  RR_PLMN_LIST_IMSG = 0x23,
  RR_PLMN_LIST_OMSG = 0x24,

  RR_SGLTE_X2G_IMSG = 0x25,
  RR_SGLTE_X2G_OMSG = 0x26,

  RR_SGLTE_G2X_RESEL_CONN_IMSG = 0x27,
  RR_SGLTE_G2X_RESEL_CONN_OMSG = 0x28,

#ifdef FEATURE_GSM_BAND_AVOIDANCE
  RR_BAND_AVOID_IMSG = 0x29,
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

  RR_MESSAGE_SET_COUNT
} rr_message_set_e;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#endif /* RR_MESSAGE_SET_H */

/* EOF */

