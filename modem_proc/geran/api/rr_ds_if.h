#ifndef RR_DS_IF_H
#define RR_DS_IF_H
/*============================================================================
 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_ds_if.h#1 $

 @file rr_ds_if.h

 Inferace file for DS

 Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/
#ifndef COMDEF_H
  #include "comdef.h"
#endif
#include "customer.h"

#include "sys.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/*
  @brief Returns the num of octets that can be transferred via GTTP (at the time the function is called)

  @return uint16
*/
extern uint16 rr_dtm_get_max_LAPDm_octets(void);
extern uint16 rr_dtm_get_max_LAPDm_octets_per_subs(sys_modem_as_id_e_type as_id);

#endif /* #ifndef RR_DS_IF_H */

/* EOF */

