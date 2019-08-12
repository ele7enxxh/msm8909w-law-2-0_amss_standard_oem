#ifndef MCALTDS_TFW_EVT_H
#define MCALTDS_TFW_EVT_H
/*============================================================================*/
/** @file
 * This module has definition and declaration related to MCAL TDS FW EVENTS 
 * processing functions
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 - 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/


/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "tdscdma_variation.h"
/*+weiz change code of share memory and event of sw and fw*/
/* FW interrupt events to L1 SW; the naming convention maps to the FW definition */
typedef enum
{
  TDS_TFW_INT_PICH_DEC_DONE,
  TDS_TFW_INT_FPACH_DEC_DONE,
  TDS_TFW_INT_NONHS_DEC_DONE,
  TDS_TFW_INT_HS_DEC_DONE,
  TDS_TFW_INT_HS_SCCH_DEC_DONE,
  TDS_TFW_INT_EAGCH_DEC_DONE,
  TDS_TFW_INT_SCHLD_EHICH_DEC_DONE,
  TDS_TFW_INT_NON_SCHLD_EHICH_DEC_DONE,
  TDS_TFW_INT_UL_PWR_INFO_UPDATE,
  TDS_TFW_INT_SW_CMD_ERROR_FLAG,
  TDS_TFW_INT_NUM_EVENTS
} mcaltds_tfw_int_event_type;

/* callback function pointer */
typedef void MCALTDS_TFW_INT_EVENT_CB_FUNC_TYPE(void);

/*===========================================================================
FUNCTION mcaltds_tfw_int_register_event_done_cb

DESCRIPTION
  This function for tds l1 to register interrupt events handler call back functions.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mcaltds_tfw_int_register_event_done_cb(
  /* event type */
  mcaltds_tfw_int_event_type event_type,
  /* callback function pointer */
  MCALTDS_TFW_INT_EVENT_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION mcaltds_fw_deregister_event_done_cb

DESCRIPTION
  This function is used for application to deregister a call back function for 
  a TFW interrupt event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mcaltds_tfw_int_deregister_event_done_cb(
  /* event type */
  mcaltds_tfw_int_event_type event_type
);

/*===========================================================================
FUNCTION MCALTDS_TFW_EVENT_ISR_HANDLER

DESCRIPTION
  FW to L1 ISR handler

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcaltds_tfw_event_isr_handler(uint32 irq_num);

/*===========================================================================
FUNCTION mcaltds_tfw_event_init

DESCRIPTION
  This function initializes the TDS FWSW events and shared memory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mcaltds_tfw_event_init( void );
/*-weiz change code of share memory and event of sw and fw*/

#endif /* MCALTDS_TFW_EVT_H */
