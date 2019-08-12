#ifndef RR_LSM_IF_H
#define RR_LSM_IF_H

/*!
 * \file rr_lsm_if.h 
 *  
 * Interface provided for location software to access GERAN data and receive event data.
 *  
 *              Copyright (c) 2002-2014 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_lsm_if.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "rr_lai.h"
#include "rr_cgps_defs.h"
#include "rr_usat_if.h"
#include "gs.h"
#include "sys_type.h"
#include "sys.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  boolean camped_on_flag;
  word    cell_identity;
  LAI_T   location_area_identification;
  ARFCN_T arfcn;
  uint8   bsic;
} geran_grr_current_bs_info_type;

// Map legacy type name
#define current_bs_info_T geran_grr_current_bs_info_type

typedef enum
{
  RR_CGPS_APDU_MSG_EVENT,
  RR_CGPS_APDU_ABORT_EVENT,
  RR_CGPS_SESSION_ABORT_EVENT
} rr_cgps_event_type;

typedef struct
{
  uint8           flags;
  uint8           id;
  uint8           length;
  const uint8 *   p_data;
} rr_cgps_apdu_data_seg_type;

typedef union
{
  rr_cgps_apdu_data_seg_type    apdu;
} rr_cgps_event_data_union_type;

typedef struct
{
  rr_cgps_event_type              event_id;
  rr_cgps_event_data_union_type   u;
  sys_modem_as_id_e_type          as_id;
} rr_cgps_event_data_type;

// Call-back function to pass event data
typedef void (*rr_cgps_event_cb_type) (const rr_cgps_event_data_type *event_data);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns various identification information.
 * 
 * \param p_BsInfo (out)
 */
extern void geran_grr_get_current_bs_info(geran_grr_current_bs_info_type *p_BsInfo);

/*!
 * \brief Returns various identification information (per subscription).
 * 
 * \param as_id (in)
 * \param p_BsInfo (out)
 */
extern void geran_grr_get_current_bs_info_per_subs(
  sys_modem_as_id_e_type as_id,
  geran_grr_current_bs_info_type *p_BsInfo
);

// Map legacy function name
#define rr_get_current_bs_info geran_grr_get_current_bs_info
#define rr_ds_get_current_bs_info geran_grr_get_current_bs_info_per_subs

/*!
 * \brief Registration function for event data call-back.
 * 
 * \param cb_fp (in)
 */
extern void geran_grr_register_cgps_event_cb(rr_cgps_event_cb_type cb_fp);

// Map legacy function name
#define rr_register_cgps_event_cb geran_grr_register_cgps_event_cb

/*!
 * \brief Access function for timing advance information.
 * 
 * \param p_TimingAdvance (out)
 */
extern void geran_grr_get_current_timing_advance_info(geran_grr_current_timing_advance_type *p_TimingAdvance);

extern void geran_grr_get_current_timing_advance_info_per_subs(
  geran_grr_current_timing_advance_type *p_TimingAdvance,
  sys_modem_as_id_e_type as_id
);

// Map legacy function name
#define rr_get_current_timing_advance_info geran_grr_get_current_timing_advance_info

#endif /* RR_LSM_IF_H */

/* EOF */

