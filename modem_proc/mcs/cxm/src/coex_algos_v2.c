/*!
  @file
  coex_algos_v2.c

  @brief
  This file contains the msg handlers for CoEx algorithms (version 2 of
  QMI COEX interface)

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/coex_algos_v2.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
10/29/14   ckk     Handle sync req/resp to report any missing/new WWAN techs
09/11/14   btl     Only update antenna sharing token if protocol running
08/21/14   btl     Update process conflicts to pass LTE tech ID (only)
08/01/14   tak     Generalized process conflict
07/28/14   btl     Disable conflict if sync msg times out
06/13/14   btl     Exchange NV params support
05/05/14   ckk     Handling GSM2 & GSM3 RAT state updates
03/18/14   btl     Add LTE SINR support
02/12/14   btl     Update to include all WWAN techs
08/09/13   btl     Initial version

==========================================================================*/
/*=============================================================================

                      INCLUDE FILES

=============================================================================*/
#include <coexistence_service_v02.h>
#include "coex_qmb.h"
#include "cxm_utils.h"
#include "cxm.h"
#include "coex_algos_v2.h"
#include "coex_algos.h"
#include "cxm_antenna.h"

/*=============================================================================

                      INTERNAL VARIABLES

=============================================================================*/
/* General tech defines */
#define COEX_TECH_TYPE_MASK   0x3F
#define COEX_TECH_TYPE_SHIFT  0
#define COEX_TECH_TOKEN_MASK  0x1F
#define COEX_TECH_TOKEN_SHIFT 0
#define COEX_TECH_STATE_MASK  0x07
#define COEX_TECH_STATE_SHIFT 5
#define COEX_TECH_BW_MASK     0x3FFF
#define COEX_TECH_BW_SHIFT    0
#define COEX_TECH_DIR_MASK    0x03
#define COEX_TECH_DIR_SHIFT   14

/* LTE-specific defines */
#define COEX_LTE_OPERATING_DIM_MASK    0x01
#define COEX_LTE_OPERATING_DIM_SHIFT   15
#define COEX_LTE_FRAME_OFFSET_MASK     0x03FF
#define COEX_LTE_FRAME_OFFSET_SHIFT    0
#define COEX_LTE_FRAME_OFFSET_MAX      2000
#define COEX_LTE_TDD_UL_LINK_CFG_MASK  0x03
#define COEX_LTE_TDD_UL_LINK_CFG_SHIFT 10
#define COEX_LTE_TDD_DL_LINK_CFG_MASK  0x03
#define COEX_LTE_TDD_DL_LINK_CFG_SHIFT 8
#define COEX_LTE_TDD_CFG_MASK          0x0F
#define COEX_LTE_TDD_CFG_SHIFT         4
#define COEX_LTE_TDD_SSP_MASK          0x0F
#define COEX_LTE_TDD_SSP_SHIFT         0
#define COEX_LTE_MIN_SINR_VAL        (-10)
#define COEX_LTE_MAX_SINR_DUR          0xFF

/* NV defines */
#define COEX_NV_WWAN_OPDIM_MASK           0x03 /* operating dimension */
#define COEX_NV_WWAN_OPDIM_SHIFT          8    /* operating dimension */
#define COEX_NV_WWAN_DIR_MASK             0x03 /* direction */
#define COEX_NV_WWAN_DIR_SHIFT            6    /* direction */
#define COEX_NV_WLAN_MODE_MASK            0x03
#define COEX_NV_WLAN_MODE_SHIFT           6
#define COEX_NV_TECH_TYPE_MASK            0x3F
#define COEX_NV_TECH_TYPE_SHIFT           0
#define COEX_NV_WLAN_RX_IMD_BOOL_MASK     0x01
#define COEX_NV_WLAN_RX_IMD_BOOL_SHIFT    6
#define COEX_NV_WLAN_RX_IMD_PWR_LMT_MASK  0x3F
#define COEX_NV_WLAN_RX_IMD_PWR_LMT_SHIFT 0

/* Macros to read bit fields */
/* the value of a field */
#define COEX_BITFLD_VAL(var,field) ( \
  ((var) & (field##_MASK << field##_SHIFT)) >> field##_SHIFT )
/* take a numerical value and shift and mask it into field position */
#define COEX_BITFLD_FVAL(var,field) ( \
  ((var) & field##_MASK) << field##_SHIFT )
/* read, mask in supplied data, then write back */
#define COEX_BITFLD_SET(var,val,field) (var)=    \
  ( ((var) & ~(field##_MASK << field##_SHIFT)) | \
    (((val) & field##_MASK) << field##_SHIFT) )

#define COEX_IS_WWAN_TECH(id) \
  ( \
    id == COEX_TECH_TYPE_WWAN_LTE_V02 || \
    id == COEX_TECH_TYPE_WWAN_TDSCDMA_V02 || \
    id == COEX_TECH_TYPE_WWAN_GSM_V02 || \
    id == COEX_TECH_TYPE_WWAN_GSM2_V02 || \
    id == COEX_TECH_TYPE_WWAN_GSM3_V02 || \
    id == COEX_TECH_TYPE_WWAN_ONEX_V02 || \
    id == COEX_TECH_TYPE_WWAN_HDR_V02 || \
    id == COEX_TECH_TYPE_WWAN_WCDMA_V02 \
  )

/* timeout in ms to receive a resp to a msg */
#define COEX_QMB_MSG_TIMER_TIMEOUT 5
#define COEX_QMB_NV_TIMER_TIMEOUT  500

#define MHZ_HZ_CONVERSION 1000000

/* indicates maximum allowed tech types, referring to the QMI_COEX V02 
 * IDL enum */
#define COEX_TECH_MAX_ALLOWED_V02 12

typedef struct
{
  /* sync tokens */
  coex_tech_sync_state_v02    sync[COEX_TECH_MAX_ALLOWED_V02];

  /* for each wlan tech, track wwan value when last msg was received */
  /* alternately, send state info every time 0 is crossed until we get an
   * acknowledgement */
  boolean                     wrp_tkns[COEX_TECH_MAX_ALLOWED_V02];

  /* WWAN tech states */
  coex_wwan_lte_state_v02     lte;
  coex_wwan_tdscdma_state_v02 tdscdma;
  coex_wwan_gsm_state_v02     gsm;
  coex_wwan_onex_state_v02    onex;
  coex_wwan_hdr_state_v02     hdr;
  coex_wwan_wcdma_state_v02   wcdma;
  coex_antenna_state_v02      antenna;
  coex_wwan_gsm_state_v02     gsm2;
  coex_wwan_gsm_state_v02     gsm3;

  /* WLAN tech states */
  coex_wlan_wifi_state_v02    wifi;

  /* manage uart state */
  uint32                      awake_mask;
  uint32                      wci2_handle;
  boolean                     uart_en;

  /* NV params */
  coex_config_params_v8     * config_params;
  boolean                     nv_exch_active;

}coex_qmb_state_info_s;

/* global coex state data for v02 WCI-2 messaging */
static coex_qmb_state_info_s coex_qmb_state_info;

typedef struct
{
  uint64  lte_sinr_start_time;
  boolean lte_sinr_started;
} coex_qmb_metrics_s;

static coex_qmb_metrics_s coex_qmb_metrics = { 0, FALSE };

static timer_type coex_qmb_msg_timer;
static timer_type coex_qmb_nv_timer;

static DalDeviceHandle *coex_qmb_DAL_handle = NULL;

/* defined in coex_algos.c */
extern coex_wwan_tech_info coex_wwan_state_info[CXM_TECH_MAX];

/*=============================================================================

                      INTERNAL FUNCTION DEFINITIONS

=============================================================================*/
void coex_send_wwan_state_ind_v02( uint8 id );

/*=============================================================================

  FUNCTION:  coex_wcn_v1_tech_to_v2_id_transl

=============================================================================*/
/*!
    @brief
    translate the coex_wcn_tech_v01 type to coex_tech_type_v02

    @return
    inline coex_tech_type_v02
*/
/*===========================================================================*/
inline coex_tech_type_v02 coex_wcn_v1_tech_to_v2_id_transl( 
  coex_wcn_tech_v01 wcn_id
)
{
  coex_tech_type_v02 tech_id;
  /*-----------------------------------------------------------------------*/
  switch( wcn_id )
  {
    case COEX_WIFI_TECH_V01:
      tech_id = COEX_TECH_TYPE_WLAN_WIFI_V02;
      break;

    case COEX_BT_TECH_V01:
      tech_id = COEX_TECH_TYPE_WLAN_BT_V02;
      break;

    default:
      tech_id = COEX_TECH_TYPE_UNINIT_V02;
      break;
  }

  return tech_id;
}

/*=============================================================================

  FUNCTION:  coex_cxm_tech_to_v2_id_transl

=============================================================================*/
/*!
    @brief
    translate the cxm_tech_type to the coex_algos_v2 QMI tech id type.
    If there are no valid translations, set v02 tech_id to UNINIT

    @return
    coex_tech_type_v02 id
*/
/*===========================================================================*/
inline coex_tech_type_v02 coex_cxm_tech_to_v2_id_transl( cxm_tech_type cxm_id )
{
  coex_tech_type_v02 tech_id;
  /*-----------------------------------------------------------------------*/
  switch( cxm_id )
  {
    case CXM_TECH_LTE:
      tech_id = COEX_TECH_TYPE_WWAN_LTE_V02;
      break;

    case CXM_TECH_TDSCDMA:
      tech_id = COEX_TECH_TYPE_WWAN_TDSCDMA_V02;
      break;

    case CXM_TECH_GSM1:
      tech_id = COEX_TECH_TYPE_WWAN_GSM_V02;
      break;

    case CXM_TECH_ONEX:
      tech_id = COEX_TECH_TYPE_WWAN_ONEX_V02;
      break;

    case CXM_TECH_HDR:
      tech_id = COEX_TECH_TYPE_WWAN_HDR_V02;
      break;

    case CXM_TECH_WCDMA:
      tech_id = COEX_TECH_TYPE_WWAN_WCDMA_V02;
      break;

    case CXM_TECH_GSM2:
      tech_id = COEX_TECH_TYPE_WWAN_GSM2_V02;
      break;

    case CXM_TECH_GSM3:
      tech_id = COEX_TECH_TYPE_WWAN_GSM3_V02;
      break;

    default:
      tech_id = COEX_TECH_TYPE_UNINIT_V02;
      break;
  }

  return tech_id;
}

/*=============================================================================

  FUNCTION:  coex_manage_npa_vote_v2

=============================================================================*/
/*!
    @brief
    Decide whether QMB needs the UART on or off, and vote appropriately

    @return
    void
*/
/*===========================================================================*/
void coex_manage_npa_vote_v2( void )
{
  boolean new_req_state = FALSE;
  /*-----------------------------------------------------------------------*/
  /* if any tech is active and awake, power on UART (if not already on) */
  /* if we don't want it on, we must want it off! */
  if( coex_qmb_state_info.awake_mask != 0 ||
      coex_qmb_state_info.nv_exch_active )
  {
    new_req_state = TRUE;
  }

  if( new_req_state != coex_qmb_state_info.uart_en )
  {
    /* execute the vote */
    coex_qmb_state_info.uart_en = new_req_state;
    wci2_client_enable_uart( coex_qmb_state_info.wci2_handle, 
                             coex_qmb_state_info.uart_en );
  }

  CXM_MSG_3( MED, "coex_v2 npa - vote %d, techs_awake 0x%x, nv_exchange %d",
             coex_qmb_state_info.uart_en, 
             coex_qmb_state_info.awake_mask,
             coex_qmb_state_info.nv_exch_active );

  return;
}

/*=============================================================================

  FUNCTION:  coex_send_sync_req_v02

=============================================================================*/
/*!
    @brief
    Fill out and send the sync indication msg to WCN. Contains current
    WWAN tokens and what we think the current WLAN tokens are.

    @return
    void
*/
/*===========================================================================*/
void coex_send_sync_req_v02( void )
{
  qmi_coex_tech_sync_req_msg_v02 req;
  unsigned int                   i = 0;
  unsigned int                   j;
  /*-----------------------------------------------------------------------*/
  /* fill in WWAN/WLAN techs */
  req.tech_valid = TRUE;
  for( j = 0; j < COEX_TECH_MAX_ALLOWED_V02; j++ )
  {
    /* Determine whether to include this tech in the sync or not.
     * NOTE: this isn't a perfect solution but will catch either non-zero
     *   tokens or non-zero states. Will not include an inactive tech
     *   with a token of zero. */
    if( coex_qmb_state_info.sync[j].sync_state_token != 0 )
    {
      if( j == COEX_TECH_TYPE_SPECIAL_DIVERSITY_ANTENNA_V02 )
      {
        cxm_antenna_update_duration();
      }
      req.tech[i++] = coex_qmb_state_info.sync[j];
    }
  }
  req.tech_len = i;

  CXM_MSG_1( MED, "QMB: Sending sync req, size=%d", req.tech_len );
  coex_qmb_publish_msg( QMI_COEX_TECH_SYNC_REQ_V02, &req, sizeof(req) );

  /* set timeout - should receive sync response within time, or assume
   * no COEX situation */
  timer_set( &coex_qmb_msg_timer, COEX_QMB_MSG_TIMER_TIMEOUT, 0, T_MSEC );

  return;
}

/*=============================================================================

  FUNCTION:  coex_send_sync_update_ind_v02

=============================================================================*/
/*!
    @brief
    send a state update for a single tech

    @return
    void
*/
/*===========================================================================*/
void coex_send_sync_update_ind_v02( coex_tech_type_v02 tech_id )
{
  qmi_coex_tech_sync_update_ind_msg_v02 ind;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( tech_id > COEX_TECH_TYPE_UNINIT_V02 &&
              tech_id < COEX_TECH_MAX_ALLOWED_V02 );

  ind.tech_valid = TRUE;
  ind.tech = coex_qmb_state_info.sync[tech_id];
  coex_qmb_publish_msg( QMI_COEX_TECH_SYNC_UPDATE_IND_V02,
                        &ind, sizeof(ind) );
  return;
}

/*=============================================================================

  FUNCTION:  coex_send_wwan_lte_state_ind_v02

=============================================================================*/
/*!
    @brief
    fill out LTE state indication and send over QMB

    @return
    void
*/
/*===========================================================================*/
void coex_send_wwan_lte_state_ind_v02( void )
{
  qmi_coex_wwan_lte_state_ind_msg_v02  ind;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_3( MED, "QMB: Sending LTE state info (sync=%x data=%x tdd=%x)", 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_LTE_V02].sync_state_token,
    coex_qmb_state_info.lte.data, coex_qmb_state_info.lte.tdd_info );

  memset( &ind, 0, sizeof(qmi_coex_wwan_lte_state_ind_msg_v02) );

  /* fill out and send the LTE band info & state */
  ind.lte_band_info_valid = TRUE;
  ind.lte_band_info = coex_qmb_state_info.lte;
  ind.lte_band_info.sync_state_token = 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_LTE_V02].sync_state_token;
  coex_qmb_publish_msg( QMI_COEX_WWAN_LTE_STATE_IND_V02, &ind, sizeof(ind) );

  return;
}

/*=============================================================================

  FUNCTION:  coex_send_wwan_tdscdma_state_ind_v02

=============================================================================*/
/*!
    @brief
    fill out TDSCDMA state indication and send over QMB

    @return
    void
*/
/*===========================================================================*/
void coex_send_wwan_tdscdma_state_ind_v02( void )
{
  qmi_coex_wwan_tdscdma_state_ind_msg_v02 ind;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "QMB: Sending TDS state info (sync=%x)", 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_TDSCDMA_V02].sync_state_token);

  memset( &ind, 0, sizeof(qmi_coex_wwan_tdscdma_state_ind_msg_v02) );

  /* fill out and send the TDS band info & state */
  ind.tdscdma_band_info_valid = TRUE;
  ind.tdscdma_band_info = coex_qmb_state_info.tdscdma;
  ind.tdscdma_band_info.sync_state_token = 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_TDSCDMA_V02].sync_state_token;
  coex_qmb_publish_msg( QMI_COEX_WWAN_TDSCDMA_STATE_IND_V02,
                        &ind, sizeof(ind) );
  return;
}

/*=============================================================================

  FUNCTION:  coex_send_wwan_gsm_state_ind_v02

=============================================================================*/
/*!
    @brief
    fill out and send the state ind for GSM over QMB

    @return
    void
*/
/*===========================================================================*/
void coex_send_wwan_gsm_state_ind_v02( void )
{
  qmi_coex_wwan_gsm_state_ind_msg_v02 ind;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "QMB: Sending GSM state info (sync=%x)",
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_GSM_V02].sync_state_token);

  memset( &ind, 0, sizeof(qmi_coex_wwan_gsm_state_ind_msg_v02) );

  /* fill out and send the GSM band info & state */
  ind.gsm_band_info_valid = TRUE;
  ind.gsm_band_info = coex_qmb_state_info.gsm;
  ind.gsm_band_info.sync_state_token = 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_GSM_V02].sync_state_token;
  coex_qmb_publish_msg( QMI_COEX_WWAN_GSM_STATE_IND_V02,
                        &ind, sizeof(ind) );
  return;
}

/*=============================================================================

  FUNCTION:  coex_send_wwan_gsm2_state_ind_v02

=============================================================================*/
/*!
    @brief
    fill out and send the state ind for GSM2 over QMB

    @return
    void
*/
/*===========================================================================*/
void coex_send_wwan_gsm2_state_ind_v02( void )
{
  qmi_coex_wwan_gsm2_state_ind_msg_v02 ind;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "QMB: Sending GSM2 state info (sync=%x)",
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_GSM2_V02].sync_state_token);

  memset( &ind, 0, sizeof(qmi_coex_wwan_gsm2_state_ind_msg_v02) );

  /* fill out and send the GSM band info & state */
  ind.gsm2_band_info_valid = TRUE;
  ind.gsm2_band_info = coex_qmb_state_info.gsm2;
  ind.gsm2_band_info.sync_state_token = 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_GSM2_V02].sync_state_token;
  coex_qmb_publish_msg( QMI_COEX_WWAN_GSM2_STATE_IND_V02,
                        &ind, sizeof(ind) );
  return;
}

/*=============================================================================

  FUNCTION:  coex_send_wwan_gsm3_state_ind_v02

=============================================================================*/
/*!
    @brief
    fill out and send the state ind for GSM3 over QMB

    @return
    void
*/
/*===========================================================================*/
void coex_send_wwan_gsm3_state_ind_v02( void )
{
  qmi_coex_wwan_gsm3_state_ind_msg_v02 ind;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "QMB: Sending GSM3 state info (sync=%x)",
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_GSM3_V02].sync_state_token);

  memset( &ind, 0, sizeof(qmi_coex_wwan_gsm3_state_ind_msg_v02) );

  /* fill out and send the GSM band info & state */
  ind.gsm3_band_info_valid = TRUE;
  ind.gsm3_band_info = coex_qmb_state_info.gsm3;
  ind.gsm3_band_info.sync_state_token = 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_GSM3_V02].sync_state_token;
  coex_qmb_publish_msg( QMI_COEX_WWAN_GSM3_STATE_IND_V02,
                        &ind, sizeof(ind) );
  return;
}

/*=============================================================================

  FUNCTION:  coex_send_wwan_onex_state_ind_v02

=============================================================================*/
/*!
    @brief
    fill out and send the state ind for 1x over QMB

    @return
    void
*/
/*===========================================================================*/
void coex_send_wwan_onex_state_ind_v02( void )
{
  qmi_coex_wwan_onex_state_ind_msg_v02 ind;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "QMB: Sending 1X state info (sync=%x)",
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_ONEX_V02].sync_state_token);

  memset( &ind, 0, sizeof(qmi_coex_wwan_onex_state_ind_msg_v02) );

  /* fill out and send the 1x band info & state */
  ind.onex_band_info_valid = TRUE;
  ind.onex_band_info = coex_qmb_state_info.onex;
  ind.onex_band_info.sync_state_token = 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_ONEX_V02].sync_state_token;
  coex_qmb_publish_msg( QMI_COEX_WWAN_ONEX_STATE_IND_V02,
                        &ind, sizeof(ind) );
  return;
}

/*=============================================================================

  FUNCTION:  coex_send_wwan_hdr_state_ind_v02

=============================================================================*/
/*!
    @brief
    fill out and send the state ind for HDR over QMB

    @return
    void
*/
/*===========================================================================*/
void coex_send_wwan_hdr_state_ind_v02( void )
{
  qmi_coex_wwan_hdr_state_ind_msg_v02 ind;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "QMB: Sending HDR state info (sync=%x)",
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_HDR_V02].sync_state_token);

  memset( &ind, 0, sizeof(qmi_coex_wwan_hdr_state_ind_msg_v02) );

  /* fill out and send the HDR band info & state */
  ind.hdr_band_info_valid = TRUE;
  ind.hdr_band_info = coex_qmb_state_info.hdr;
  ind.hdr_band_info.sync_state_token = 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_HDR_V02].sync_state_token;
  coex_qmb_publish_msg( QMI_COEX_WWAN_HDR_STATE_IND_V02,
                        &ind, sizeof(ind) );
  return;
}

/*=============================================================================

  FUNCTION:  coex_send_wwan_wcdma_state_ind_v02

=============================================================================*/
/*!
    @brief
    fill out and send the state ind for WCDMA over QMB

    @return
    void
*/
/*===========================================================================*/
void coex_send_wwan_wcdma_state_ind_v02( void )
{
  qmi_coex_wwan_wcdma_state_ind_msg_v02 ind;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "QMB: Sending WCDMA state info (sync=%x)",
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_WCDMA_V02].sync_state_token);

  memset( &ind, 0, sizeof(qmi_coex_wwan_wcdma_state_ind_msg_v02) );

  /* fill out and send the W band info & state */
  ind.wcdma_band_info_valid = TRUE;
  ind.wcdma_band_info = coex_qmb_state_info.wcdma;
  ind.wcdma_band_info.sync_state_token = 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WWAN_WCDMA_V02].sync_state_token;
  coex_qmb_publish_msg( QMI_COEX_WWAN_WCDMA_STATE_IND_V02,
                        &ind, sizeof(ind) );
  return;
}


/*=============================================================================

  FUNCTION:  coex_send_diversity_antenna_state_ind_v02

=============================================================================*/
/*!
    @brief
    Inform of the state of the diversity antenna chain. Sent over QMB.

    @return
    void
*/
/*===========================================================================*/
void coex_send_diversity_antenna_state_ind_v02( void )
{
  qmi_coex_diversity_antenna_state_ind_msg_v02 ind;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_3( MED, "QMB: Sending Div antenna state (sync=%x adv_not=%u dur=%u)", 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_SPECIAL_DIVERSITY_ANTENNA_V02].sync_state_token,
    coex_qmb_state_info.antenna.adv_notice,
    coex_qmb_state_info.antenna.duration );

  /* fill out and send state info from local data structs */
  memset( &ind, 0, sizeof(qmi_coex_diversity_antenna_state_ind_msg_v02) );
  ind.state_valid = TRUE;
  ind.state.data = coex_qmb_state_info.antenna;
  ind.state.sync_state_token = 
    coex_qmb_state_info.sync[COEX_TECH_TYPE_SPECIAL_DIVERSITY_ANTENNA_V02].sync_state_token;
  coex_qmb_publish_msg( QMI_COEX_DIVERSITY_ANTENNA_STATE_IND_V02,
                        &ind, sizeof(ind) );
  return;
}

/*=============================================================================

  FUNCTION:  coex_send_wwan_state_ind_v02

=============================================================================*/
/*!
    @brief
    Switch between various supported wwan state info & send it out

    @return
    void
*/
/*===========================================================================*/
void coex_send_wwan_state_ind_v02( uint8 id )
{
  /*-----------------------------------------------------------------------*/
  /* check the coex_tech_type_v02 (first 4 bits) */
  switch( id & COEX_TECH_TYPE_MASK )
  {
    case COEX_TECH_TYPE_WWAN_LTE_V02:
      coex_send_wwan_lte_state_ind_v02();
      break;

    case COEX_TECH_TYPE_WWAN_TDSCDMA_V02:
      coex_send_wwan_tdscdma_state_ind_v02();
      break;

    case COEX_TECH_TYPE_WWAN_GSM_V02:
      coex_send_wwan_gsm_state_ind_v02();
      break;

    case COEX_TECH_TYPE_WWAN_ONEX_V02:
      coex_send_wwan_onex_state_ind_v02();
      break;

    case COEX_TECH_TYPE_WWAN_HDR_V02:
      coex_send_wwan_hdr_state_ind_v02();
      break;

    case COEX_TECH_TYPE_WWAN_WCDMA_V02:
      coex_send_wwan_wcdma_state_ind_v02();
      break;

    case COEX_TECH_TYPE_WWAN_GSM2_V02:
      coex_send_wwan_gsm2_state_ind_v02();
      break;

    case COEX_TECH_TYPE_WWAN_GSM3_V02:
      coex_send_wwan_gsm3_state_ind_v02();
      break;

    case COEX_TECH_TYPE_SPECIAL_DIVERSITY_ANTENNA_V02:
      if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WLAN_ANT_SHARE) )
      {
        cxm_antenna_update_duration();
      }
      coex_send_diversity_antenna_state_ind_v02();
      break;

    default:
      break;
  }
  return;
}

/*=============================================================================

  FUNCTION:  coex_update_common_wwan_state_info_v2

=============================================================================*/
/*!
    @brief
    Helper function - update parts of WWAN state info that are the
    same for all techs

    @return
    uint8 sync_state_token
*/
/*===========================================================================*/
uint8 coex_update_common_wwan_state_info_v2(
  coex_tech_type_v02         wwan_tech,
  uint8                      band_info_len,
  cxm_tech_link_info_s      *cxm_link_info
)
{
  int     i;
  uint16  bit_math_16 = 0;
  uint8   new_sync_state_token, tech_state, max_band_info_len;
  boolean connected = FALSE;
  coex_tech_band_type_v02 *band_info = NULL;
  /*-----------------------------------------------------------------------*/
  /* cut off sending bands after the maximum has been reached */
  if( wwan_tech == CXM_TECH_GSM1 || wwan_tech == CXM_TECH_GSM2 ||
      wwan_tech == CXM_TECH_GSM3 )
  {
    max_band_info_len = COEX_WWAN_GSM_MAX_BANDS_SUPPORTED_V02;
  }
  else
  {
    max_band_info_len = COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02;
  }
  CXM_ASSERT( band_info_len <= max_band_info_len );

  switch( wwan_tech )
  {
    case COEX_TECH_TYPE_WWAN_LTE_V02:
      band_info = &coex_qmb_state_info.lte.band_info[0];
      break;

    case COEX_TECH_TYPE_WWAN_TDSCDMA_V02:
      band_info = &coex_qmb_state_info.tdscdma.band_info[0];
      break;

    case COEX_TECH_TYPE_WWAN_GSM_V02:
      band_info = &coex_qmb_state_info.gsm.band_info[0];
      break;

    case COEX_TECH_TYPE_WWAN_ONEX_V02:
      band_info = &coex_qmb_state_info.onex.band_info[0];
      break;

    case COEX_TECH_TYPE_WWAN_HDR_V02:
      band_info = &coex_qmb_state_info.hdr.band_info[0];
      break;

    case COEX_TECH_TYPE_WWAN_WCDMA_V02:
      band_info = &coex_qmb_state_info.wcdma.band_info[0];
      break;

    case COEX_TECH_TYPE_WWAN_GSM2_V02:
      band_info = &coex_qmb_state_info.gsm2.band_info[0];
      break;

    case COEX_TECH_TYPE_WWAN_GSM3_V02:
      band_info = &coex_qmb_state_info.gsm3.band_info[0];
      break;

    default:
      break;
  }
  CXM_ASSERT( band_info != NULL );
  /* copy frequency, bandwidth, and direction info into local struct */
  for( i = 0; i < band_info_len; i++ )
  {
    bit_math_16 = 0;
    band_info[i].frequency = cxm_link_info[i].frequency / 100;
    band_info[i].bandwidth_and_direction = 
      COEX_BITFLD_FVAL((cxm_link_info[i].bandwidth / 100000), COEX_TECH_BW);

    /* convert coex interface direction type to QMI_COEX_v02 type */
    if( CXM_LNK_DRCTN_UL        == cxm_link_info[i].direction ||
        CXM_LNK_DRCTN_UL_AND_DL == cxm_link_info[i].direction )
    {
      bit_math_16 |= COEX_TECH_BAND_DIRECTION_UPLINK_V02;
    }
    if( CXM_LNK_DRCTN_DL        == cxm_link_info[i].direction ||
        CXM_LNK_DRCTN_UL_AND_DL == cxm_link_info[i].direction )
    {
      bit_math_16 |= COEX_TECH_BAND_DIRECTION_DOWNLINK_V02;
    }
    band_info[i].bandwidth_and_direction |= 
      COEX_BITFLD_FVAL(bit_math_16, COEX_TECH_DIR);
  }

  /* update token for wwan tech, since state is modified */
  new_sync_state_token = COEX_BITFLD_VAL(
    coex_qmb_state_info.sync[wwan_tech].sync_state_token,
    COEX_TECH_TOKEN) + 1;
  /* if token wrapped, wrap it and set wrap flag */
  if( new_sync_state_token > COEX_TECH_TOKEN_MASK )
  {
    new_sync_state_token = 0;
    coex_qmb_state_info.wrp_tkns[wwan_tech] = TRUE;
  }
  new_sync_state_token = COEX_BITFLD_FVAL(new_sync_state_token, 
                                          COEX_TECH_TOKEN);

  /* determine WWAN conn state (idle/connected?) */
  /* if no DL & UL freq, tech is idle; if UL present, tech is connected */
  for( i = 0; i < band_info_len; i++ )
  {
    if( COEX_BITFLD_VAL(band_info[i].bandwidth_and_direction, COEX_TECH_DIR) & 
        COEX_TECH_BAND_DIRECTION_UPLINK_V02 )
    {
      connected = TRUE;
      break;
    }
  }
  /* update to new state appropriately, first clearing old state. need
   * to maintain the awake/sleep bits. */
  tech_state = COEX_BITFLD_VAL(
    coex_qmb_state_info.sync[wwan_tech].sync_state_token,
    COEX_TECH_STATE);
  tech_state &= ~(COEX_TECH_STATE_IDLE_V02 | COEX_TECH_STATE_CONN_V02);
  if( band_info_len > 0 )
  {
    /* if any UL, state = connected. if DL and no UL, idle */
    tech_state |= ((connected) ? 
      COEX_TECH_STATE_CONN_V02 : COEX_TECH_STATE_IDLE_V02 );
  }

  new_sync_state_token |= COEX_BITFLD_FVAL(tech_state, COEX_TECH_STATE);

  /* save updated state and token info */
  coex_qmb_state_info.sync[wwan_tech].sync_state_token = new_sync_state_token;

  return coex_qmb_state_info.sync[wwan_tech].sync_state_token;
}

/*=============================================================================

  FUNCTION:  coex_qmb_msg_timer_handler

=============================================================================*/
/*!
    @brief
    Timer handler for QMB recv msg timeout, set when we expect a response
    back from the WCN within a certain time

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_msg_timer_handler( void )
{
  uint8 bt_state, wifi_state, prev_wifi_state;
  /*-----------------------------------------------------------------------*/
  /* assume other side off/asleep, turn off all coex algos. */
  CXM_MSG_0( MED, "QMB: Sync resp timeout, assuming WCN asleep" );

  bt_state = COEX_BITFLD_VAL(
      coex_qmb_state_info.sync[COEX_TECH_TYPE_WLAN_BT_V02].sync_state_token,
      COEX_TECH_STATE);
  wifi_state = COEX_BITFLD_VAL(
      coex_qmb_state_info.sync[COEX_TECH_TYPE_WLAN_WIFI_V02].sync_state_token,
      COEX_TECH_STATE);
  prev_wifi_state = wifi_state;

  bt_state   &= ~COEX_TECH_STATE_ACTIVE_V02;
  wifi_state &= ~COEX_TECH_STATE_ACTIVE_V02;

  if( prev_wifi_state != wifi_state )
  {
    /* combine new state info with token and write back */
    COEX_BITFLD_SET(
        coex_qmb_state_info.sync[COEX_TECH_TYPE_WLAN_WIFI_V02].sync_state_token,
        wifi_state, COEX_TECH_STATE);
#ifdef FEATURE_COEX_USE_NV
    /* if a conflict is ongoing, stop it.*/
    if( COEX_SYS_ENABLED(CXM_SYS_BHVR_VICTIM_TABLE) )
    {
      coex_process_conflicts(CXM_TECH_LTE, CXM_TECH_WIFI);
    }
#endif
  }

  COEX_BITFLD_SET(
      coex_qmb_state_info.sync[COEX_TECH_TYPE_WLAN_BT_V02].sync_state_token,
      bt_state, COEX_TECH_STATE);

  return;
}

/*=============================================================================

  FUNCTION:  coex_qmb_nv_timer_handler

=============================================================================*/
/*!
    @brief
    Timer handler for QMB NV exchange timeout. When timeout occurs,
    we can stop holding the UART powered on

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_nv_timer_handler( void )
{
  /*-----------------------------------------------------------------------*/
  /* turn off vote to power on the UART */
  CXM_MSG_0( HIGH, "QMB: NV exchange timeout" );
  coex_qmb_state_info.nv_exch_active = FALSE;
  coex_manage_npa_vote_v2();

  return;
}

/*=============================================================================

                      EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  coex_algos_init_v2

=============================================================================*/
/*!
    @brief
    Initialize resources for COEX QMI V2 msg flows

    @return
    void
*/
/*===========================================================================*/
void coex_algos_init_v2(
  coex_config_params_v8 * coex_params
)
{
  DALResult DAL_retval = DAL_SUCCESS;
  int i;
  /*-----------------------------------------------------------------------*/
  /* define timer for qmb msg resp timeouts */
  timer_def( &coex_qmb_msg_timer, NULL, 
             &cxm_tcb, CXM_COEX_QMB_TIMER_SIG, NULL, 0 );
  /* define timer for nv exchange timeouts */
  timer_def( &coex_qmb_nv_timer, NULL,
             &cxm_tcb, CXM_COEX_QMB_NV_TIMER_SIG, NULL, 0 );

  /* create client with wci2 to send/recv t2 over UART */
  coex_qmb_state_info.wci2_handle = coex_qmb_init();

  /* fill out sync state tech IDs for each tech, so won't have to redo it
   * every time when we send sync msgs later */
  for( i = 0; i < COEX_TECH_MAX_ALLOWED_V02; i++ )
  {
    coex_qmb_state_info.sync[i].id  = i;
    coex_qmb_state_info.wrp_tkns[i] = FALSE;
  }

  /* open handle to DAL to use for getting the QTimer timestamp 
   * for recording the metric start time */
  DAL_retval = DalTimetick_Attach("SystemTimer", &coex_qmb_DAL_handle);
  CXM_ASSERT( (DAL_SUCCESS == DAL_retval) && (NULL != coex_qmb_DAL_handle) );

  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WLAN_ANT_SHARE) )
  {
    /* if Antenna Sharing with WLAN is enabled, initialize those resources
     * and begin the algorithm */
    cxm_antenna_init( &coex_qmb_state_info.sync[COEX_TECH_TYPE_SPECIAL_DIVERSITY_ANTENNA_V02], 
                      &coex_qmb_state_info.antenna,
                      &coex_qmb_state_info.wrp_tkns[COEX_TECH_TYPE_SPECIAL_DIVERSITY_ANTENNA_V02] );
  }

  coex_qmb_state_info.config_params = coex_params;

  return;
}

/*=============================================================================

  FUNCTION:  coex_algos_deinit_v2

=============================================================================*/
/*!
    @brief
    Deinitialize resources for COEX QMI v2 msg flows

    @return
    void
*/
/*===========================================================================*/
void coex_algos_deinit_v2( void )
{
  /*-----------------------------------------------------------------------*/
  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WLAN_ANT_SHARE) )
  {
    cxm_antenna_deinit();
  }

  /* clear all timers (if any pending) */
  (void) timer_clr( &coex_qmb_msg_timer, T_MSEC );
  (void) timer_clr( &coex_qmb_nv_timer, T_MSEC );

  /* disconnect from QMB */
  coex_qmb_deinit();

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_update_req_v02

=============================================================================*/
/*!
    @brief
    respond to the request for the state info of the given tech

    @return
    void
*/
/*===========================================================================*/
void coex_handle_update_req_v02( void* msg, unsigned int msg_len )
{
  int i;
  qmi_coex_tech_state_update_req_msg_v02 * req = 
    (qmi_coex_tech_state_update_req_msg_v02*) msg;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_0( MED, "QMB: Handling update_request" );

  /* look up tech ID, and send that tech's state if applicable */
  if( req->tech_ids_valid )
  {
    for( i = 0; i < req->tech_ids_len && i < COEX_MAX_TECHS_SUPPORTED_V02; i++ )
    {
      /* check the coex_tech_type_v02 (first 4 bits) */
      switch( req->tech_ids[i].id & COEX_TECH_TYPE_MASK )
      {
        case COEX_TECH_TYPE_WWAN_LTE_V02:
          coex_send_wwan_lte_state_ind_v02();
          break;

        case COEX_TECH_TYPE_WWAN_TDSCDMA_V02:
          coex_send_wwan_tdscdma_state_ind_v02();
          break;

        case COEX_TECH_TYPE_WWAN_GSM_V02:
          coex_send_wwan_gsm_state_ind_v02();
          break;

        case COEX_TECH_TYPE_WWAN_ONEX_V02:
          coex_send_wwan_onex_state_ind_v02();
          break;

        case COEX_TECH_TYPE_WWAN_HDR_V02:
          coex_send_wwan_hdr_state_ind_v02();
          break;

        case COEX_TECH_TYPE_WWAN_WCDMA_V02:
          coex_send_wwan_wcdma_state_ind_v02();
          break;

        case COEX_TECH_TYPE_WWAN_GSM2_V02:
          coex_send_wwan_gsm2_state_ind_v02();
          break;

        case COEX_TECH_TYPE_WWAN_GSM3_V02:
          coex_send_wwan_gsm3_state_ind_v02();
          break;

        case COEX_TECH_TYPE_SPECIAL_DIVERSITY_ANTENNA_V02:
          if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WLAN_ANT_SHARE) )
          {
            cxm_antenna_update_duration();
          }
          coex_send_diversity_antenna_state_ind_v02();
          break;

        default:
          break;
      }
    } /* for */
  } /* if tech_valid */

  return;
} /* coex_handle_update_req_v02 */

/*=============================================================================

  FUNCTION:  coex_handle_wlan_wifi_state_ind_v02

=============================================================================*/
/*!
    @brief
    Update our copy of WIFI state with new WIFI state info

    @return
    void
*/
/*===========================================================================*/
void coex_handle_wlan_wifi_state_ind_v02( void* msg, unsigned int msg_len )
{
  qmi_coex_wlan_wifi_state_ind_msg_v02* req = 
    (qmi_coex_wlan_wifi_state_ind_msg_v02*)msg;
  /*-----------------------------------------------------------------------*/
  /* copy info locally. this is the latest state, so no need to 
   * check against old state. */
  if( req->wifi_state_info_valid )
  {
    CXM_MSG_3( MED, "QMB: Handle WIFI State ind (sync=%x hi_prio_len=%d conn_len=%d)",
        req->wifi_state_info.sync_state_token,
        req->wifi_state_info.high_priority_events_list_len,
        req->wifi_state_info.connections_list_len );

    coex_qmb_state_info.wifi = req->wifi_state_info;
    coex_qmb_state_info.sync[COEX_TECH_TYPE_WLAN_WIFI_V02].sync_state_token = 
        req->wifi_state_info.sync_state_token;
#ifdef FEATURE_COEX_USE_NV
    /* check for conflicts and mitigate if necessary.*/
    if( COEX_SYS_ENABLED(CXM_SYS_BHVR_VICTIM_TABLE) )
    {
      coex_process_conflicts(CXM_TECH_LTE, CXM_TECH_WIFI);
    }
#endif
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_sync_state_req_v02

=============================================================================*/
/*!
    @brief
    handle sync msg to synchronize the state between WWAN & WLAN modules

    @return
    void
*/
/*===========================================================================*/
void coex_handle_sync_state_req_v02( void* msg, unsigned int msg_len )
{
  int i, j;
  boolean tech_updt_rqd[COEX_TECH_MAX_ALLOWED_V02];
  qmi_coex_tech_sync_req_msg_v02* req = (qmi_coex_tech_sync_req_msg_v02*)msg;
  qmi_coex_tech_sync_resp_msg_v02 resp;
  uint8 id, msg_token, curr_token, state;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_0( MED, "QMB: Process Sync Req" );

  /* initialize to prevent KW P1 */
  memset( tech_updt_rqd, FALSE, sizeof( boolean ) * COEX_TECH_MAX_ALLOWED_V02);

  /* 1: copy updated state from WLAN sync_state_token
   * 2: send sync_state_resp
   * 3: check WWAN sync_state_token for token mismatches and send state_info
   *    if there is a mismatch
   * 4: send state_info for an newly added WWAN tech */
  if( req->tech_valid )
  {
    for( i = 0; i < req->tech_len && i < COEX_MAX_TECHS_SUPPORTED_V02; i++ )
    {
      id = req->tech[i].id & COEX_TECH_TYPE_MASK;
      if( !COEX_IS_WWAN_TECH( id ) )
      {
        switch (id) 
        {
          case COEX_TECH_TYPE_WLAN_BT_V02:
            /* TODO: special case for BT, since BT doesn't have
             * a state info msg. We will do coex for BT if they are awake,
             * otherwise not, so for now copy sync token directly */
            if( coex_qmb_state_info.sync[id].sync_state_token != 
                req->tech[i].sync_state_token )
            {
              CXM_MSG_1( MED, "QMB: New BT sync info=%x", req->tech[i].sync_state_token );
              coex_qmb_state_info.sync[id].sync_state_token = 
                req->tech[i].sync_state_token;
            }
            break;

          case COEX_TECH_TYPE_WLAN_WIFI_V02:
            /* TODO: if this changed, expect a state update */
            CXM_MSG_2( MED, "WIFI sync RX, old=0x%x new=0x%x", 
                       coex_qmb_state_info.sync[id].sync_state_token, 
                       req->tech[i].sync_state_token );
            state = COEX_BITFLD_VAL(req->tech[i].sync_state_token, COEX_TECH_STATE);
            if( state != COEX_BITFLD_VAL(
                            coex_qmb_state_info.sync[id].sync_state_token, 
                            COEX_TECH_STATE) )
            {
              /* if state changed (possibly from inactive to active) re-process conflicts */
              COEX_BITFLD_SET(
                coex_qmb_state_info.sync[id].sync_state_token,
                state, COEX_TECH_STATE);
              #ifdef FEATURE_COEX_USE_NV
              /* check for conflicts and mitigate if necessary.*/
              if( COEX_SYS_ENABLED(CXM_SYS_BHVR_VICTIM_TABLE) )
              {
                coex_process_conflicts(CXM_TECH_LTE, CXM_TECH_WIFI);
              }
              #endif
            }
            break;

          default:
            /* unrecognized tech */
            break;

        } /* end WLAN switch */
      } /* if not WWAN tech check */
    } /* end for loop */

    /* respond with our sync */
    i = 0;
    resp.tech_valid = TRUE;
    /* fill in WWAN/WLAN techs */
    for( j = 0; j < COEX_TECH_MAX_ALLOWED_V02; j++ )
    {
      /* Determine whether to include this tech in the sync or not.
       * NOTE: this isn't a perfect solution but will catch either non-zero
       *   tokens or non-zero states. Will not include an inactive tech
       *   with a token of zero. */
      if( coex_qmb_state_info.sync[j].sync_state_token != 0 )
      {
        if( j == COEX_TECH_TYPE_SPECIAL_DIVERSITY_ANTENNA_V02 )
        {
          cxm_antenna_update_duration();
        }
        resp.tech[i++] = coex_qmb_state_info.sync[j];
        if( COEX_IS_WWAN_TECH( j ) )
        {
          tech_updt_rqd[j] = TRUE; 
        }
      }
    }
    resp.tech_len = i;
    coex_qmb_publish_msg( QMI_COEX_TECH_SYNC_RESP_V02, &resp, sizeof(resp) );

    /* check WWAN tech_state_tokens. if their sync token for one of our 
     * WWAN techs doesn't match what they sent, send that tech's state info */
    for( i = 0; i < req->tech_len && i < COEX_MAX_TECHS_SUPPORTED_V02; i++ )
    {
      msg_token = COEX_BITFLD_VAL(req->tech[i].sync_state_token, COEX_TECH_TOKEN);
      id = req->tech[i].id & COEX_TECH_TYPE_MASK;
      if( COEX_IS_WWAN_TECH( id ) )
      {
        curr_token = COEX_BITFLD_VAL(
                       coex_qmb_state_info.sync[id].sync_state_token, 
                       COEX_TECH_TOKEN);
        if( (curr_token != msg_token) ||
            coex_qmb_state_info.wrp_tkns[id] )
        {
          /* token mismatch - send updated state */
          coex_send_wwan_state_ind_v02( id );
          coex_qmb_state_info.wrp_tkns[id] = FALSE;
        }
        tech_updt_rqd[id] = FALSE;
      } /* WWAN check */
    } /* WWAN for loop */

    /* check for any/all newly added WWAN states that they are not aware of
     * and send that state_info out */
    for( i = 0; i < COEX_TECH_MAX_ALLOWED_V02; i++ )
    {
      if( tech_updt_rqd[i] == TRUE )
      {
        id = coex_qmb_state_info.sync[i].id & COEX_TECH_TYPE_MASK;
        if( COEX_IS_WWAN_TECH( id ) )
        {
          coex_send_wwan_state_ind_v02( id );
          coex_qmb_state_info.wrp_tkns[id] = FALSE;
          tech_updt_rqd[id] = FALSE;
        } /* WWAN if check */
      }
    }
  } /* tech_valid */

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_sync_state_resp_v02

=============================================================================*/
/*!
    @brief
    handle a TECH_SYNC_RESP msg (in response to a REQ we sent earlier)

    @return
    void
*/
/*===========================================================================*/
void coex_handle_sync_state_resp_v02( void* msg, unsigned int msg_len )
{
  int i;
  boolean tech_updt_rqd[COEX_TECH_MAX_ALLOWED_V02];
  qmi_coex_tech_sync_resp_msg_v02* resp = 
    (qmi_coex_tech_sync_resp_msg_v02*) msg;
  uint8 id, msg_token, curr_token, state;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_0( MED, "QMB: Handle Sync Resp" );

  /* initialize to prevent KW P1 */
  memset( tech_updt_rqd, FALSE, sizeof( boolean ) * COEX_TECH_MAX_ALLOWED_V02);

  if( resp->tech_valid )
  {
    /* clear timeout timer */
    (void) timer_clr( &coex_qmb_msg_timer, T_MSEC );

    /* assume all wwan need updates */
    for( i = 0; i < COEX_TECH_MAX_ALLOWED_V02; i++ )
    {
      /* Determine whether to include this tech in the sync or not.
       * NOTE: this isn't a perfect solution but will catch either non-zero
       *   tokens or non-zero states. Will not include an inactive tech
       *   with a token of zero. */
      if( COEX_IS_WWAN_TECH( i ) &&
          coex_qmb_state_info.sync[i].sync_state_token != 0 )
      {
        tech_updt_rqd[i] = TRUE;
      }
    }

    for( i = 0; i < resp->tech_len && i < COEX_MAX_TECHS_SUPPORTED_V02; i++ )
    {
      msg_token = COEX_BITFLD_VAL(resp->tech[i].sync_state_token, COEX_TECH_TOKEN);
      id = resp->tech[i].id & COEX_TECH_TYPE_MASK;
      if( id == COEX_TECH_TYPE_WLAN_BT_V02 ) 
      {
        /* TODO: special case for BT, since BT doesn't have
         * a state info msg. We will do coex for BT if they are awake,
         * otherwise not, so for now copy sync token directly */
        if( coex_qmb_state_info.sync[id].sync_state_token != 
            resp->tech[i].sync_state_token )
        {
          CXM_MSG_1( MED, "QMB: New BT sync info=%x", resp->tech[i].sync_state_token );
          coex_qmb_state_info.sync[id].sync_state_token = 
            resp->tech[i].sync_state_token;
        }
      }
      else if( id == COEX_TECH_TYPE_WLAN_WIFI_V02 )
      {
        /* TODO: if this token changed, expect a state update */
        CXM_MSG_2( MED, "WIFI sync RX, old=0x%x new=0x%x", 
                   coex_qmb_state_info.sync[id].sync_state_token, 
                   resp->tech[i].sync_state_token );
        state = COEX_BITFLD_VAL(resp->tech[i].sync_state_token, COEX_TECH_STATE);
        if( state != COEX_BITFLD_VAL(
                        coex_qmb_state_info.sync[id].sync_state_token, 
                        COEX_TECH_STATE) )
        {
          /* if state changed (possibly from inactive to active) re-process conflicts */
          COEX_BITFLD_SET(
            coex_qmb_state_info.sync[id].sync_state_token,
            state, COEX_TECH_STATE);
          #ifdef FEATURE_COEX_USE_NV
          /* check for conflicts and mitigate if necessary.*/
          if( COEX_SYS_ENABLED(CXM_SYS_BHVR_VICTIM_TABLE) )
          {
            coex_process_conflicts(CXM_TECH_LTE, CXM_TECH_WIFI);
          }
          #endif
        }
      }
      /* if their sync token for one of our WWAN techs doesn't match
       * what they sent, send that tech's state info */
      else if( COEX_IS_WWAN_TECH( id ) )
      {
        curr_token = COEX_BITFLD_VAL(
                       coex_qmb_state_info.sync[id].sync_state_token, 
                       COEX_TECH_TOKEN);
        if( (curr_token != msg_token) || 
            coex_qmb_state_info.wrp_tkns[id] )
        {
          /* token mismatch - send updated state */
          coex_send_wwan_state_ind_v02( id );
          coex_qmb_state_info.wrp_tkns[id] = FALSE;
        }
        tech_updt_rqd[id] = FALSE;
      }
      else
      {
        /* either COEX_TECH_TYPE_SPECIAL_DIVERSITY_ANTENNA_V02 or
         * unrecognized tech 
         * do nothing for these... */
      }
    } /* for */

    /* check for any/all newly added WWAN states that they are not aware of
     * and send that state_info out */
    for( i = 0; i < COEX_TECH_MAX_ALLOWED_V02; i++ )
    {
      if( tech_updt_rqd[i] == TRUE )
      {
        id = coex_qmb_state_info.sync[i].id & COEX_TECH_TYPE_MASK;
        if( COEX_IS_WWAN_TECH( id ) )
        {
          coex_send_wwan_state_ind_v02( id );
          coex_qmb_state_info.wrp_tkns[id] = FALSE;
          tech_updt_rqd[id] = FALSE;
        } /* WWAN if check */
      }
    }
  } /* tech_valid */

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_sync_update_ind_v02

=============================================================================*/
/*!
    @brief
    handle QMI_COEX_TECH_SYNC_UPDATE_IND_V02 from WCN

    @return
    void
*/
/*===========================================================================*/
void coex_handle_sync_update_ind_v02( void* msg, unsigned int msg_len )
{
  qmi_coex_tech_sync_update_ind_msg_v02* ind = 
    (qmi_coex_tech_sync_update_ind_msg_v02*) msg;
  uint8 id, state;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_0( MED, "QMB: Handle Sync Ind" );

  if( ind->tech_valid )
  {
    id = ind->tech.id & COEX_TECH_TYPE_MASK;
    switch( id )
    {
      case COEX_TECH_TYPE_WLAN_BT_V02:
        /* TODO: special case for BT, since BT doesn't have
         * a state info msg. We will do coex for BT if they are awake,
         * otherwise not, so for now copy sync token directly */
        if( coex_qmb_state_info.sync[id].sync_state_token != 
            ind->tech.sync_state_token )
        {
          CXM_MSG_1( MED, "QMB: New BT sync info=%x", ind->tech.sync_state_token );
          coex_qmb_state_info.sync[id].sync_state_token = 
            ind->tech.sync_state_token;
        }
        break;

      case COEX_TECH_TYPE_WLAN_WIFI_V02:
        /* TODO: if this token changed, expect a state update */
        state = COEX_BITFLD_VAL(ind->tech.sync_state_token, COEX_TECH_STATE);
        if( state != COEX_BITFLD_VAL(
                        coex_qmb_state_info.sync[id].sync_state_token, 
                        COEX_TECH_STATE) )
        {
          /* if state changed (possibly from inactive to active) re-process conflicts */
          COEX_BITFLD_SET(
            coex_qmb_state_info.sync[id].sync_state_token,
            state, COEX_TECH_STATE);
          #ifdef FEATURE_COEX_USE_NV
          /* check for conflicts and mitigate if necessary.*/
          if( COEX_SYS_ENABLED(CXM_SYS_BHVR_VICTIM_TABLE) )
          {
            coex_process_conflicts(CXM_TECH_LTE, CXM_TECH_WIFI);
          }
          #endif
        }
        break;

      default:
        /* unrecognized tech */
        break;

    } /* switch */
  } /* tech_valid */

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_metrics_lte_sinr_start_ind_v02

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to start collecting/
    collating LTE SINR metrics

    @return
    void
*/
/*===========================================================================*/
void coex_handle_metrics_lte_sinr_start_ind_v02(
  void*        msg,
  unsigned int msg_len
)
{
  uint32                                       alpha_q8;
  DalTimetickTime64Type                        timestamp;
  qmi_coex_metrics_lte_sinr_start_ind_msg_v02* req = 
    (qmi_coex_metrics_lte_sinr_start_ind_msg_v02*)msg;
  /*-----------------------------------------------------------------------*/
  if( req->data_valid )
  {
    CXM_MSG_1( MED, "QMB: Handle LTE SINR metrics START ind (alpha=%d)",
               req->data.filter_alpha );

    /* convert alpha from (uint8)x100 to (uint32)Q8 format for LTE */
    alpha_q8 = (uint32) ((req->data.filter_alpha << 8) + 50) / 100;

    /* back-to-back starts will reset SINR, it is a valid operation */
    coex_tech_metrics_initiate_req( COEX_LTE_TECH_V01, CXM_ACTION_START, alpha_q8, 0 );

    DalTimetick_GetTimetick64( coex_qmb_DAL_handle, &timestamp );
    coex_qmb_metrics.lte_sinr_start_time = (uint64) timestamp;
    coex_qmb_metrics.lte_sinr_started = TRUE;
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_metrics_lte_sinr_read_req_v02

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to read current filter
    output for LTE SINR metrics

    @return
    void
*/
/*===========================================================================*/
void coex_handle_metrics_lte_sinr_read_req_v02(
  void*        msg,
  unsigned int msg_len
)
{
  uint32                                      deferred_req_id;
  errno_enum_type                             retval = E_SUCCESS;
  qmi_coex_metrics_lte_sinr_read_resp_msg_v02 qmb_response;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "QMB: Handle LTE SINR metrics READ req, started=%d",
             coex_qmb_metrics.lte_sinr_started );

  /* This message is blank, so no need interpret it. 
   * Just send the query to ML1 */
  if( coex_qmb_metrics.lte_sinr_started )
  {
    /* push qmb req info onto delayed response stack to recover when
     * read cnf response from LTE received */
    retval = coex_qmb_push_deferred_req( 
                &deferred_req_id,
                QMI_COEX_METRICS_LTE_SINR_READ_REQ_V02 );
    CXM_ASSERT( E_SUCCESS == retval );

    coex_tech_metrics_initiate_req( COEX_LTE_TECH_V01, CXM_ACTION_READ, 0, deferred_req_id );
  }
  else
  {
    /* SINR not started, send response right away */
    qmb_response.report_valid= TRUE;
    qmb_response.report.estatus = COEX_METRICS_E_NOT_STARTED_V02;
    qmb_response.report.collection_duration = 0x00;
    qmb_response.report.stats.sinr_system = COEX_LTE_MIN_SINR_VAL;
    qmb_response.report.stats.sinr_bt_only = COEX_LTE_MIN_SINR_VAL;
    qmb_response.report.stats.sinr_wifi_only = COEX_LTE_MIN_SINR_VAL;
    qmb_response.report.stats.sinr_bt_and_wifi = COEX_LTE_MIN_SINR_VAL;
    qmb_response.report.stats.sinr_lte_only = COEX_LTE_MIN_SINR_VAL;
    coex_qmb_publish_msg( QMI_COEX_METRICS_LTE_SINR_READ_RESP_V02,
                          &qmb_response, sizeof(qmb_response) );
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_metrics_lte_sinr_stop_ind_v02

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to stop collecting/collating
    LTE SINR metrics

    @return
    void
*/
/*===========================================================================*/
void coex_handle_metrics_lte_sinr_stop_ind_v02( 
  void*        msg, 
  unsigned int msg_len 
)
{
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "QMB: Handle LTE SINR metrics STOP ind, started=%d",
             coex_qmb_metrics.lte_sinr_started );
  /* REQ only appropriate if SINR has been started */
  if( coex_qmb_metrics.lte_sinr_started )
  {
    /* tell LTE to stop SINR */
    coex_tech_metrics_initiate_req( COEX_LTE_TECH_V01, CXM_ACTION_STOP, 0, 0 );
    coex_qmb_metrics.lte_sinr_started = FALSE;
    CXM_MSG_0( HIGH, "QMB: LTE SINR metrics stopped" );
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_conflict_params_req_v02

=============================================================================*/
/*!
    @brief
    Handle the client's query for NV coex conflict params

    @return
    void
*/
/*===========================================================================*/
void coex_handle_conflict_params_req_v02(
  void* msg,
  unsigned int msg_len )
{
  qmi_coex_conflict_params_resp_msg_v02 resp;
  qmi_coex_conflict_params_req_msg_v02 *req_ptr = (qmi_coex_conflict_params_req_msg_v02*) msg;
  uint8 remaining_conflicts, i, num_conflicts, temp_field;
  coex_conflict_type * entry;
  uint16_t wwan_tech_opdim = 0x0000;
  /*-----------------------------------------------------------------------*/
  if( req_ptr->victim_tbl_offset_valid )
  {
    num_conflicts = coex_qmb_state_info.config_params->num_conflicts;
    remaining_conflicts = num_conflicts - req_ptr->victim_tbl_offset;

    CXM_MSG_2( MED, "QMB: Handle NV req, offset=%d total=%d",
               req_ptr->victim_tbl_offset,
               num_conflicts );

    memset( &resp, 0, sizeof(qmi_coex_conflict_params_resp_msg_v02) );

    resp.report_valid = TRUE;
    resp.report.request_estatus = COEX_CONFLICT_PARAMS_REQ_E_SUCCESS_V02;
    resp.report.victim_tbl_offset = req_ptr->victim_tbl_offset;
    resp.report.victim_tbl_complete_size = num_conflicts;
    if( req_ptr->victim_tbl_offset > num_conflicts )
    {
      resp.report.request_estatus = COEX_CONFLICT_PARAMS_REQ_E_INVALID_ARG_V02;
    }
    else
    {
      for( i = 0; i < remaining_conflicts && 
                  i < COEX_NV_MSG_VICTIM_TABLE_MAX_ENTRIES_V02; i++ )
      {
        entry = &coex_qmb_state_info.config_params->victim_tbl[req_ptr->victim_tbl_offset + i];
        /*
         * - bit[9:8]  - coex_tech_operating_dimension - indicates RAT's dimension
         * - bit[7:6]  - coex_tech_band_direction - Uplink/Downlink/Both/Don't Care
         *                NOTE: a band can be specified as both UL and DL by combining 
         *                the mask values of UL and DL.
         * - bit[5:0]  - coex_tech_type - identifies the COEX technology type */
        wwan_tech_opdim = ( (entry->wwan.operating_dim & COEX_TECH_OPERATING_DIMENSION_FDD_V01) ?
                            (0x0001<<COEX_TECH_OPERATING_DIMENSION_FDD_V02) : 0x0000 );
        wwan_tech_opdim |= ( (entry->wwan.operating_dim & COEX_TECH_OPERATING_DIMENSION_TDD_V01) ?
                            (0x0001<<COEX_TECH_OPERATING_DIMENSION_TDD_V02) : 0x0000 );
        resp.report.partial_victim_table[i].wwan.tech_type = wwan_tech_opdim << COEX_NV_WWAN_OPDIM_SHIFT;
        if( entry->wwan.direction & COEX_UL_BAND_DIRECTION_V01 )
        {
          resp.report.partial_victim_table[i].wwan.tech_type |= 
            COEX_TECH_BAND_DIRECTION_UPLINK_V02 << COEX_NV_WWAN_DIR_SHIFT;
        }
        if( entry->wwan.direction & COEX_DL_BAND_DIRECTION_V01 )
        {
          resp.report.partial_victim_table[i].wwan.tech_type |= 
            COEX_TECH_BAND_DIRECTION_DOWNLINK_V02 << COEX_NV_WWAN_DIR_SHIFT;
        }
        temp_field = coex_cxm_tech_to_v2_id_transl( entry->wwan.tech );
        resp.report.partial_victim_table[i].wwan.tech_type |= COEX_BITFLD_FVAL(temp_field, COEX_NV_TECH_TYPE);
        /* Valid range is 0.0 MHz to 6553.5 MHz, convert from kHz to MHz * 10 */
        resp.report.partial_victim_table[i].wwan.frequency_range_start = entry->wwan.freq_start / 100;
        resp.report.partial_victim_table[i].wwan.frequency_range_stop = entry->wwan.freq_stop / 100;
        /* - bit[15:8] - reserved for future use
         * - bit[7:6]  - coex_wlan_tech_operating_mode - connected/high_priority
         *                NOTE: mode can be specified as both connected and high_priority
         *                by combining the mask values of each in these 2 bits.
         * - bit[5:0]  - coex_tech_type - identifies the COEX technology type */
        if( entry->wcn.mode & COEX_WLAN_CONN_MODE_V01 )
        {
          resp.report.partial_victim_table[i].wlan.tech_type |= 
            COEX_WLAN_CONN_MODE_V02 << COEX_NV_WLAN_MODE_SHIFT;
        }
        if( entry->wcn.mode & COEX_WLAN_HIGH_PRIO_MODE_V01 )
        {
          resp.report.partial_victim_table[i].wlan.tech_type |= 
            COEX_WLAN_HIGH_PRIO_MODE_V02 << COEX_NV_WLAN_MODE_SHIFT;
        }
        temp_field = coex_wcn_v1_tech_to_v2_id_transl( entry->wcn.tech );
        resp.report.partial_victim_table[i].wlan.tech_type |= 
          COEX_BITFLD_FVAL(temp_field, COEX_NV_TECH_TYPE);
        /* Valid range is 0.0 MHz to 6553.5 MHz, convert from kHz to MHz * 10 */
        resp.report.partial_victim_table[i].wlan.frequency_range_start = entry->wcn.freq_start / 100;
        resp.report.partial_victim_table[i].wlan.frequency_range_stop = entry->wcn.freq_stop / 100;
        resp.report.partial_victim_table[i].wlan_coex_policy = 
          (coex_wlan_policy_config_mask_v02) entry->wcn_policy;
        /* - bit[7]   - reserved for future use
         * - bit[6]   - boolean to enable/disable blocking the WLAN transmit
         *               when RSSI is too low.
         * - bit[5:0] - static WLAN transmit power limit in dBm
         *               range between 0dBm to ~25dBm */
        resp.report.partial_victim_table[i].wlan_policy_coex_wwan_rx_imd_protection_params = 
          COEX_BITFLD_FVAL(entry->wcn_params.imd_protection_params.block_tx, 
                           COEX_NV_WLAN_RX_IMD_BOOL);
        resp.report.partial_victim_table[i].wlan_policy_coex_wwan_rx_imd_protection_params |= 
          COEX_BITFLD_FVAL(entry->wcn_params.imd_protection_params.static_backoff_tx_pwr, 
                           COEX_NV_WLAN_RX_IMD_PWR_LMT);
      } /* for loop */
      resp.report.partial_victim_table_len = i;

      /* check if we need to keep UART on (more params to exchange?) */
      if( req_ptr->victim_tbl_offset == num_conflicts )
      {
        timer_clr( &coex_qmb_nv_timer, T_MSEC );
        coex_qmb_state_info.nv_exch_active = FALSE;
        coex_manage_npa_vote_v2();
      }
      else
      {
        timer_set( &coex_qmb_nv_timer, COEX_QMB_NV_TIMER_TIMEOUT, 0, T_MSEC );
        coex_qmb_state_info.nv_exch_active = TRUE;
      }
    } /* else */

    /* send response out QMB */
    coex_qmb_publish_msg( QMI_COEX_CONFLICT_PARAMS_RESP_V02, &resp, sizeof(resp) );

  } /* if offset valid */

  return;
}

/*=============================================================================

  FUNCTION:  coex_process_wwan_sleep_wake_v2

=============================================================================*/
/*!
    @brief
    Process the sleep/wakeup indication from the WWAN tech appropriately.
    If a COEX tech, send the appropriate SYNC message to WLAN.

    @return
    void
*/
/*===========================================================================*/
void coex_process_wwan_sleep_wake_v2( 
  cxm_coex_tech_sleep_wakeup_duration_ind_s* msg_ptr
)
{
  coex_tech_type_v02 tech_id = COEX_TECH_TYPE_UNINIT_V02;
  coex_tech_state_v02 old_state, state = 0;
  /*-----------------------------------------------------------------------*/
  /* translate tech ID to QMI tech ID. tech_id will be UNINIT if the 
   * cxm_tech_id does not have a valid translation */
  tech_id = coex_cxm_tech_to_v2_id_transl( msg_ptr->tech_id );

  /* is this tech_id valid for QMI V02? */
  if( tech_id != COEX_TECH_TYPE_UNINIT_V02 )
  {
    state = COEX_BITFLD_VAL(coex_qmb_state_info.sync[tech_id].sync_state_token, 
                            COEX_TECH_STATE);
    old_state = state;

    /* send a sync req on both sleep & wake, containing the state. Set the
     * state based on sleep/wake */
    /*   bit[7:5] - coex_tech_state - informs of tech's current state */
    if( msg_ptr->is_going_to_sleep )
    {
      state &= ~COEX_TECH_STATE_ACTIVE_V02;
      coex_qmb_state_info.awake_mask &= ~(1 << tech_id);
    }
    else
    {
      state |= COEX_TECH_STATE_ACTIVE_V02;
      coex_qmb_state_info.awake_mask |= 1 << tech_id;
    }

    if( state != old_state )
    {
      /* combine new state info with token and write back */
      COEX_BITFLD_SET(coex_qmb_state_info.sync[tech_id].sync_state_token,
                      state, COEX_TECH_STATE);

      /* send an indication if going to sleep and a request if waking up,
       * because we don't want a response if we're sleeping */
      if( msg_ptr->is_going_to_sleep )
      {
        /* send then power down uart */
        coex_send_sync_update_ind_v02( tech_id );
        coex_manage_npa_vote_v2();
      }
      else
      {
        /* power up uart then send msg */
        coex_manage_npa_vote_v2();
        coex_send_sync_req_v02();
      }
    }
  } /* if tech id supported */

  return;
}

/*=============================================================================

  FUNCTION:  coex_process_wwan_state_info_msg_v2

=============================================================================*/
/*!
    @brief
    Update the local WWAN state info for the given tech
     - copy state over to our QMI_v2 format for each tech

    @return
    void
*/
/*===========================================================================*/
void coex_process_wwan_state_info_msg_v2(
  cxm_tech_type    tech_id,
  cxm_tech_data_s *tech_data
)
{
  uint16  bit_math_16 = 0;
  uint8   band_info_len, max_band_info_len;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "QMB: creating WWAN State IND for tech %d", tech_id );

  /* cut off sending bands after the maximum has been reached */
  if( tech_id == CXM_TECH_GSM1 || tech_id == CXM_TECH_GSM2 ||
      tech_id == CXM_TECH_GSM3 )
  {
    max_band_info_len = COEX_WWAN_GSM_MAX_BANDS_SUPPORTED_V02;
  }
  else
  {
    max_band_info_len = COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02;
  }

  band_info_len = tech_data->num_link_info_sets;
  if( band_info_len > max_band_info_len )
  {
    CXM_MSG_3( ERROR, "QMB: dropped band(s), tech=%d len=%d max=%d", 
               tech_id, band_info_len, max_band_info_len );
    band_info_len = max_band_info_len;
  }

  /* update tech-specific state info. NOTE: this is also needed to convert
   * from interal CXM_TECH types to COEX_TECH_..._V2 types used in this file */
  switch( tech_id )
  {
    case CXM_TECH_LTE:
      /* update token & copy band info */
      coex_qmb_state_info.lte.band_info_len = band_info_len;
      coex_update_common_wwan_state_info_v2(
            COEX_TECH_TYPE_WWAN_LTE_V02, band_info_len,
            &tech_data->link_list[0] );

      /* bit[15]    - coex_tech_operating_dimension - indicates LTE's dimension
         bit[14:10] - reserved for future use
         bit[9:0]   - frame offset (timing advance) in micro-seconds
                      valid range is from 0 to 1023 micro-seconds */
      if( LTE_L1_FRAME_STRUCTURE_FS2 == 
          tech_data->params.lte_params.frame_structure )
      {
        /* frame_offset = (2ms - timing advance). can't fit that in this 10-bit
         * field, so including (timing advance) instead. Only applies to TDD mode. */
        bit_math_16 = (uint16)tech_data->params.lte_params.frame_offset;
        if( bit_math_16 > COEX_LTE_FRAME_OFFSET_MAX )
        {
          bit_math_16 = COEX_LTE_FRAME_OFFSET_MAX;
        }
        bit_math_16  = COEX_LTE_FRAME_OFFSET_MAX - bit_math_16;

        bit_math_16 |= COEX_BITFLD_FVAL(COEX_TECH_OPERATING_DIMENSION_TDD_V02, 
                                        COEX_LTE_OPERATING_DIM);
        coex_qmb_state_info.lte.data = bit_math_16;
        /* bit[15:12] - Reserved for future use
           bit[11:10] - Uplink coex_lte_tdd_link_config
           bit[9:8]   - Downlink coex_lte_tdd_link_config
           bit[7:4]   - coex_lte_tdd_config
           bit[3:0]   - coex_lte_tdd_subframe_config */
        bit_math_16  = COEX_BITFLD_FVAL(tech_data->params.lte_params.ssp, 
                                        COEX_LTE_TDD_SSP);
        bit_math_16 |= COEX_BITFLD_FVAL(tech_data->params.lte_params.tdd_config, 
                                        COEX_LTE_TDD_CFG);
        bit_math_16 |= COEX_BITFLD_FVAL(tech_data->params.lte_params.dl_cp, 
                                        COEX_LTE_TDD_DL_LINK_CFG);
        bit_math_16 |= COEX_BITFLD_FVAL(tech_data->params.lte_params.ul_cp, 
                                        COEX_LTE_TDD_UL_LINK_CFG);
        coex_qmb_state_info.lte.tdd_info = bit_math_16;
      }
      else
      {
        /* FDD mode - don't fill out TDD */
        coex_qmb_state_info.lte.data = 0;
        coex_qmb_state_info.lte.tdd_info = 0;
      }
      /* send updated state msg */
      coex_send_wwan_lte_state_ind_v02();
      break;

    case CXM_TECH_TDSCDMA:
      /* update token & copy band info */
      coex_qmb_state_info.tdscdma.band_info_len = band_info_len;
      coex_update_common_wwan_state_info_v2(
            COEX_TECH_TYPE_WWAN_TDSCDMA_V02, band_info_len,
            &tech_data->link_list[0] );
      /* send updated state msg */
      coex_send_wwan_tdscdma_state_ind_v02();
      break;

    case CXM_TECH_GSM1:            /*!< GSM on Subscription_1 */
      /* update token & copy band info to local qmb structs */
      coex_qmb_state_info.gsm.band_info_len = band_info_len;
      coex_update_common_wwan_state_info_v2(
          COEX_TECH_TYPE_WWAN_GSM_V02, band_info_len,
          &tech_data->link_list[0] );
      /* send updated state msg */
      coex_send_wwan_gsm_state_ind_v02();
      break;

    case CXM_TECH_GSM2:            /*!< GSM on Subscription_2 */
      /* update token & copy band info to local qmb structs */
      coex_qmb_state_info.gsm2.band_info_len = band_info_len;
      coex_update_common_wwan_state_info_v2(
          COEX_TECH_TYPE_WWAN_GSM2_V02, band_info_len,
          &tech_data->link_list[0] );
      /* send updated state msg */
      coex_send_wwan_gsm2_state_ind_v02();
      break;

    case CXM_TECH_GSM3:            /*!< GSM on Subscription_3 */
      /* update token & copy band info to local qmb structs */
      coex_qmb_state_info.gsm3.band_info_len = band_info_len;
      coex_update_common_wwan_state_info_v2(
          COEX_TECH_TYPE_WWAN_GSM2_V02, band_info_len,
          &tech_data->link_list[0] );
      /* send updated state msg */
      coex_send_wwan_gsm3_state_ind_v02();
      break;

    case CXM_TECH_ONEX:            /*!< 1X */
      /* update token & copy band info to local qmb structs */
      coex_qmb_state_info.onex.band_info_len = band_info_len;
      coex_update_common_wwan_state_info_v2(
          COEX_TECH_TYPE_WWAN_ONEX_V02, band_info_len,
          &tech_data->link_list[0] );
      /* send updated state msg */
      coex_send_wwan_onex_state_ind_v02();
      break;


    case CXM_TECH_HDR:             /*!< HDR */
      /* update token & copy band info to local qmb structs */
      coex_qmb_state_info.hdr.band_info_len = band_info_len;
      coex_update_common_wwan_state_info_v2(
          COEX_TECH_TYPE_WWAN_HDR_V02, band_info_len,
          &tech_data->link_list[0] );
      /* send updated state msg */
      coex_send_wwan_hdr_state_ind_v02();
      break;

    case CXM_TECH_WCDMA:           /*!< WCDMA */
      /* update token & copy band info to local qmb structs */
      coex_qmb_state_info.wcdma.band_info_len = band_info_len;
      coex_update_common_wwan_state_info_v2(
          COEX_TECH_TYPE_WWAN_WCDMA_V02, band_info_len,
          &tech_data->link_list[0] );
      /* send updated state msg */
      coex_send_wwan_wcdma_state_ind_v02();
      break;

    default:
      break;
      /* all other techs currently unsupported */
  }

  return;
}
#ifdef FEATURE_COEX_USE_NV
/*=============================================================================

  FUNCTION:  coex_process_conflicts_v2

=============================================================================*/
/*!
    @brief
    Searches through WWAN and WCN state data and determines if there is a
    conflict then carries out appropriate action

    @detail
    Searches through current state to determine if there is a conflict. The
    search is based on the tech passed in since it will only search for conflicts
    that may be associated with this tech.

    @return
    boolean conflict_found
*/
/*===========================================================================*/
void coex_process_conflicts_v2 (
  cxm_tech_type     wwan_tech,
  cxm_wcn_tech_type wcn_tech,
  uint64            *max_conf_prio,
  uint8             *max_conf_prio_vctbl_idx
)
{
  boolean new_conflict = FALSE;
  coex_band_type_v01 wcn_band;
  uint8 i, j, k;
  uint8 wifi_state;
  coex_wwan_operating_dimension_v01 wwan_op_dim = 0;
  uint8  run_conf_prio_id = 0;
  uint64 run_conf_prio = 0;
  /*-----------------------------------------------------------------------*/
  /* TODO: "pause" conflict instead of canceling?? */
  /* if WIFI state bits indicate WIFI is not connected (perhaps did not
   * respond to the sync message, assume no conflict since WIFI not active */
  wifi_state = COEX_BITFLD_VAL(
      coex_qmb_state_info.sync[COEX_TECH_TYPE_WLAN_WIFI_V02].sync_state_token,
      COEX_TECH_STATE);
  if( wifi_state & COEX_TECH_STATE_ACTIVE_V02 )
  {
    /* Search through state data to find conflicts related to WIFI or LTE */
    if ( wcn_tech == CXM_TECH_WIFI ) 
    {
      /* determine the operating dimension if LTE */
      if( wwan_tech == CXM_TECH_LTE )
      {
        wwan_op_dim = coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.frame_structure == 
                      LTE_L1_FRAME_STRUCTURE_FS2 ? COEX_TECH_OPERATING_DIMENSION_TDD_V01 : 
                      COEX_TECH_OPERATING_DIMENSION_FDD_V01;
      }
      for( i = 0; i < coex_wwan_state_info[wwan_tech].state_data.num_link_info_sets; i++) 
      {
        /* search through WIFI high prio nodes */
        for( j = 0; j < coex_qmb_state_info.wifi.high_priority_events_list_len && 
                    j < COEX_WLAN_WIFI_MAX_EVENTS_SUPPORTED_V02; j++ )
        {
          /* TODO: store in v01 or process in v02. don't do this conversion: we lose
           * 0.1Mhz of precision, since v02 uses MHz*10 and v01 uses MHz */
          wcn_band.freq = coex_qmb_state_info.wifi.high_priority_events_list[j].frequency / 10;
          wcn_band.bandwidth = (coex_qmb_state_info.wifi.high_priority_events_list[j].bandwidth_and_direction & 0x3FFF) / 10;

          coex_resolve_conflict(wwan_tech, CXM_TECH_WIFI,
            &coex_wwan_state_info[wwan_tech].state_data.link_list[i],
            &wcn_band,CXM_WLAN_HIGH_PRIO_TYPE, wwan_op_dim,
            &run_conf_prio, &run_conf_prio_id );
          if( run_conf_prio > *max_conf_prio )
          {
            *max_conf_prio = run_conf_prio;
            *max_conf_prio_vctbl_idx = run_conf_prio_id;
          }
          run_conf_prio = 0;
        }

        /* if WIFI high prio conflict found, go with it */
        if( *max_conf_prio > 0 )
        {
          new_conflict = TRUE;
        }

        /* search through WIFI connection nodes */ 
        for( j = 0; j < coex_qmb_state_info.wifi.connections_list_len && 
                    j < COEX_WLAN_WIFI_MAX_CONNS_SUPPORTED_V02 && !new_conflict; j++ )
        {
          for( k = 0; k < coex_qmb_state_info.wifi.connections_list[j].band_info_len &&
                      k < COEX_WLAN_WIFI_MAX_BANDS_SUPPORTED_V02; k++ )
          {
            /* TODO: store in v01 or process in v02. don't do this conversion: we lose
             * 0.1Mhz of precision, since v02 uses MHz*10 and v01 uses MHz */
            wcn_band.freq = coex_qmb_state_info.wifi.connections_list[j].band_info[k].frequency / 10;
            wcn_band.bandwidth = (coex_qmb_state_info.wifi.connections_list[j].band_info[k].bandwidth_and_direction & 0x3FFF) / 10;

            coex_resolve_conflict(wwan_tech, CXM_TECH_WIFI, 
              &coex_wwan_state_info[wwan_tech].state_data.link_list[i],
              &wcn_band,
              CXM_WLAN_CONN_TYPE, wwan_op_dim,
              &run_conf_prio, &run_conf_prio_id );
            if( run_conf_prio > *max_conf_prio )
            {
              *max_conf_prio = run_conf_prio;
              *max_conf_prio_vctbl_idx = run_conf_prio_id;
            }
            run_conf_prio = 0;
          }
        }
      } /* end WWAN tech loop */
    } /* WIFI conflict search */
  } /* end WIFI active check */

  return;
}
#endif

/*=============================================================================

  FUNCTION:  coex_handle_lte_sinr_read_cnf_v2

=============================================================================*/
/*!
    @brief
    Function to send QMB message with LTE SINR stats, read on request from
    ML1

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_lte_sinr_read_cnf_v2( 
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                             retval = E_SUCCESS;
  cxm_coex_metrics_lte_sinr_rsp_s            *msg_ptr;
  qmi_coex_metrics_lte_sinr_read_resp_msg_v02 qmb_response;
  DalTimetickTime64Type                       timestamp, delta, delta_ms;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_metrics_lte_sinr_rsp_s *) rcv_msgr_msg_ptr;

  memset( &qmb_response, 0, sizeof( qmi_coex_metrics_lte_sinr_read_resp_msg_v02 ) );

  /* initialize response packet (case when stats collection never started) */
  qmb_response.report_valid= TRUE;
  qmb_response.report.estatus = COEX_METRICS_E_NOT_STARTED_V02;
  qmb_response.report.collection_duration = 0x00;
  qmb_response.report.stats.sinr_system = COEX_LTE_MIN_SINR_VAL;
  qmb_response.report.stats.sinr_bt_only = COEX_LTE_MIN_SINR_VAL;
  qmb_response.report.stats.sinr_wifi_only = COEX_LTE_MIN_SINR_VAL;
  qmb_response.report.stats.sinr_bt_and_wifi = COEX_LTE_MIN_SINR_VAL;
  qmb_response.report.stats.sinr_lte_only = COEX_LTE_MIN_SINR_VAL;

  if(msg_ptr->status == E_SUCCESS)
  {
    /* all okay; prep and send the qmb_response */
    qmb_response.report_valid= TRUE;
    qmb_response.report.estatus = COEX_METRICS_E_SUCCESS_V02;
    qmb_response.report.stats.sinr_system = msg_ptr->filtered_sinr_db;
    qmb_response.report.stats.sinr_bt_only = msg_ptr->filtered_sinr_bt_db;
    qmb_response.report.stats.sinr_wifi_only = msg_ptr->filtered_sinr_wifi_db;
    qmb_response.report.stats.sinr_bt_and_wifi = msg_ptr->filtered_sinr_bt_wifi_db;
    qmb_response.report.stats.sinr_lte_only = msg_ptr->filtered_sinr_lte_only_db;

    /* calculate metrics collection duration:
     * Units: 10s of milli-seconds
     * Range: 0x00 -> 0 milli-seconds to 0xFE -> 254*10=2540 ms */
    DalTimetick_GetTimetick64( coex_qmb_DAL_handle, &timestamp );
    delta = timestamp - coex_qmb_metrics.lte_sinr_start_time;
    DalTimetick_CvtFromTimetick64( coex_qmb_DAL_handle, delta, T_MSEC, &delta_ms );
    delta_ms = delta_ms / 10;
    qmb_response.report.collection_duration = 
        ( delta_ms < COEX_LTE_MAX_SINR_DUR ) ? delta_ms : COEX_LTE_MAX_SINR_DUR;
  }
  else if(msg_ptr->status == E_NOT_AVAILABLE)
  {
    /* likely LTE not in system */
    qmb_response.report_valid= TRUE;
    qmb_response.report.estatus = COEX_METRICS_E_TECH_NOT_ACTIVE_V02;
    qmb_response.report.collection_duration = 0x00;
    qmb_response.report.stats.sinr_system = COEX_LTE_MIN_SINR_VAL;
    qmb_response.report.stats.sinr_bt_only = COEX_LTE_MIN_SINR_VAL;
    qmb_response.report.stats.sinr_wifi_only = COEX_LTE_MIN_SINR_VAL;
    qmb_response.report.stats.sinr_bt_and_wifi = COEX_LTE_MIN_SINR_VAL;
    qmb_response.report.stats.sinr_lte_only = COEX_LTE_MIN_SINR_VAL;
  }

  /* look up the QMB delayed qmi_response to the read req */
  retval = coex_qmb_pop_deferred_req( 
              msg_ptr->msg_id,
              QMI_COEX_METRICS_LTE_SINR_READ_REQ_V02 );
  CXM_ASSERT( E_SUCCESS == retval );

  /* send the QMB delayed qmi_response */
  coex_qmb_publish_msg( QMI_COEX_METRICS_LTE_SINR_READ_RESP_V02,
                        &qmb_response, sizeof(qmb_response) );

  return retval;
}

