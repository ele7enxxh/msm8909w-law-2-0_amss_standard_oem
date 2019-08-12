#ifndef L1LSMIF_H
#define L1LSMIF_H
/*==========================================================================

             L1 LSM Interface Header File

DESCRIPTION
  This file contains global declarations and external references
  for the WCDMA L1 -to- LSM (Location Services Manager) Interface

  Copyright (c) 2003 -2007 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/l1lsmif.h#1 $
  $DateTime: 2016/12/13 08:00:12 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/13   geg     WSW decoupling (except for component of QCHAT)
04/25/11   geg     Separated l1lsmif.h into l1lsmif.h and l1lsmif_v.h
04/25/11   geg     Moved this file to /wcdma/api from /l1/inc
07/16/07   scm     Support new CGPS interface for measurement reports.
06/20/07   rc      Added delta rscp support to wl1_lsm_cell_meas_type structure.
06/12/07   kps     Fix WPLT Compilation errors.
05/29/07   kps     FEATURE_CGPS: GPS interface change support
04/25/05    mc     Added P4 DateTime and Author header.
04/18/05    ld     Added Sleep timetag related code.
05/17/04   scm     Add num_failed_reacquisitions to PRE timetag struct.
02/26/04   scm     Add new type: WL1_TIMETAG_REF_CELL_CHANGE.
12/18/03   scm     Add structs to support LSM cell measurement request.
04/28/03   djm      1. initial file
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rrcdata.h"
#include "cgps_api.h"

/****************************************
**  Data passed from LSM to WCDMA L1.  **
****************************************/
/* This structure corresponds to the CPHY_LSM_TIMETAG_REQ command sent to WL1
** by LSM when a TimeTag update is requested. */
typedef struct
{
  boolean allow_sleep_timetag;
} l1_lsm_timetag_type;

/* This structure corresponds to the CPHY_LSM_CELL_MEAS_REQ command sent to
** WL1 by LSM when it wants to know received cell strengths. */
typedef struct
{
  uint16 dummy;

} l1_lsm_cell_meas_type;

/****************************************
**  Data passed from WCDMA L1 to LSM.  **
****************************************/
typedef struct
{
  boolean  active_set;    /* TRUE if this cell belongs to the active set. */
  uint16   freq;          /* Frequency of this cell.                      */
  uint16   pri_scr_code;  /* Primary scrambling code of this cell.        */
  /* For Ec/No and RSCP, we would need filtered values.
     Use the same filtering as suggested by the network.
     In effect LSM should receive the same values as the
     network would have received.
  */

  /* Ec/No and RSCP should be scaled as follows */
  /*   
   *  The values will be returned as per the mapping specified in 25.133
   *  V3.6.0    
   *  
   *  For RSCP : Table 9-4   (eg., < -115 --> 00
   *  (in units of dBm)              -115 -->01
   *                                    ...
   *                               >= -25 -->91
   *                               
   *  For ECNO:  Table 9-9  (eg:   < -24   -->00
   *  (in units of dB)               -24   -->01
   *                                 -23.5 -->02
   *                                   ...
   *                               >= 0   --->49
   */
  
  uint8    ec_no_val;    /* Ec/No of the cell. */
  uint8    rscp_val;     /* Received Signal Code Power (RSCP) of the cell.  */

  /* delta RSCP will be included if RSCP < -116 dbm */
  /* The mapping is as follows:
     -117 --> -1
     -118 --> -2
     -119 --> -3
     -120 --> -4
     -121 --> -5
  */

  /* TRUE if delta RSCP is included */
  boolean delta_rscp_included;
  /* only valid when the above field is TRUE */
  int16 delta_rscp;
} wl1_lsm_cell_meas_type;

#define WL1_LSM_CELLS_MEAS_INFO_MAX  32

typedef struct
{
  uint8                   num_cells;
  wl1_lsm_cell_meas_type  cells[WL1_LSM_CELLS_MEAS_INFO_MAX];
  
} wl1_lsm_cell_meas_update_type;

/* The following data structures are used by WL1 to send cell measurement
** report messsages to GPS in response to the GPS->WL1 request called
** CPHY_LSM_CELL_MEAS_REQ. */
typedef enum
{
  L1_CGPS_EVENT_CELL_MEAS_UPDATE
} l1_cgps_event_e_type;

typedef struct
{
  uint8  *p_data;
  uint32  length;
} l1_cgps_msg_data_s_type;

typedef struct
{
  l1_cgps_event_e_type     e_event_type;
  l1_cgps_msg_data_s_type  z_l1_cgps_msg_data;
} l1_cgps_event_data_s_type;

typedef boolean (*l1_cgps_event_cb_type)(l1_cgps_event_data_s_type
                                         *event_data_ptr);

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION     L1_CGPS_REGISTER_EVENT_CB

DESCRIPTION
  This function is called by CGPS to install a callback which will be used
  by WL1 to provide events and/or data back to CGPS.

DEPENDENCIES
  None.

PARAMETERS
  event_cb_fp - Event callback handler to register with WL1.  (Currently the
                only supported event is L1_CGPS_EVENT_CELL_MEAS_UPDATE.)

RETURN VALUE
  boolean - TRUE on success.  FALSE on failure.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean l1_cgps_register_event_cb(l1_cgps_event_cb_type event_cb_fp);

#endif  /* #ifndef L1LSMIF_H */
