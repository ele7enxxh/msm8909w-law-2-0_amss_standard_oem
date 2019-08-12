#ifndef RR_BAND_AVOID_IF_H
#define RR_BAND_AVOID_IF_H
/*============================================================================
  @file rr_band_avoid_if.c

  @brief Interface for band avoidance feature

                Copyright (c) 2014 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_band_avoid_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_BAND_AVOIDANCE

#include "comdef.h"
#include "rr_message_header.h"
#include "rr_l1.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/
typedef struct
{
  rr_message_header_t      header;
  frequency_list_T         freq;
} rr_band_avoid_ded_freq_upd_t;

typedef enum
{
  RR_BAND_AVOID_IMSG_DED_FREQ_UPD_IND,
  RR_BAND_AVOID_IMSG_MEAS_RPT_SENT_IND,
  RR_BAND_AVOID_IMSG_COUNT
} rr_band_avoid_imsg_e;

typedef union
{
  rr_message_header_t           header;
  rr_band_avoid_ded_freq_upd_t  ded_freq_upd_ind;
} rr_band_avoid_imsg_u;

typedef union
{
  rr_band_avoid_imsg_u imsg;
} rr_band_avoid_msg_u;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

void rr_band_avoid_send_cxm_blist_rsp(uint16 blist_id, const gas_id_t gas_id);


void rr_band_avoid_send_cxm_freq_ind(frequency_list_T *freq, const gas_id_t gas_id);


void rr_band_avoid_ded_freq_upd_send_imsg(frequency_list_T *freq_upd, const gas_id_t gas_id);


void rr_band_avoid_meas_rpt_sent_send_imsg(const gas_id_t gas_id);

const char * rr_band_avoid_imsg_name(rr_band_avoid_imsg_e omsg);
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
#endif /*RR_BAND_AVOID_IF_H*/
