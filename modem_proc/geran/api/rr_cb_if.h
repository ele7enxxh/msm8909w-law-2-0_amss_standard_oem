/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              rr_cb_if.h

GENERAL DESCRIPTION
  RR callback functionality.  Provides callback for multiple regstrations
  (set by RR_MAX_NUM_OF_REG_CB).  User registers a function with the
  callback handler.  This function(s) is triggered by the callback handler when
  the data item being monitored has changed.

  Currently provided:
    1) Serving cell callback - Provides the cell_id of the serving cell.
    2) Best six neighbour cells callback -  Provides cell_id's of the best
       six neighbour cells.

  Both neighbour cell and serving cell id changes are serviced by:

  Register a cell id callback call function:
    rr_register_cell_change_ind_cb()

  Deregister a cell id callback function call:
    rr_deregister_cell_change_ind_cb()

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002-2013 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_cb_if.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
19/10/10   SJW     Added support for FEATURE_DUAL_SIM
10-04-01   tjw     Add customer.h and comdef.h to all SU API files


===========================================================================*/

#ifndef RR_CB_IF_H
#define RR_CB_IF_H


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "geran_dual_sim.h"
#include "sys.h"

/*===========================================================================

                         DEFINITIONS

===========================================================================*/
#define RR_MAX_NUM_NEIGHBOUR_CELLS     6
#define RR_CELL_INFO_MAX_PLMN_ID_SIZE  3
#define RR_CELL_ID_NOT_PRESENT         0xffffffff

/*===========================================================================

                         TYPE DEFS

===========================================================================*/

typedef enum
{
  NO_CAUSE = 1
} rr_cell_info_cb_cause_t;

typedef struct
{
  uint32 cell_id; /* neighbour cell id, valid range: 0..0xffff, neighbour cell id not present: RR_CELL_ID_NOT_PRESENT */
} rr_ncell_cell_t;

typedef struct
{
  uint8           num_cells; /* indicates number of neighbour cells, range 0.. RR_MAX_NUM_NEIGHBOUR_CELLS */
  rr_ncell_cell_t cell[RR_MAX_NUM_NEIGHBOUR_CELLS];
} rr_ncell_t;

typedef struct
{
  uint8 plmn_id[RR_CELL_INFO_MAX_PLMN_ID_SIZE]; /*see 24.008 10.5.1.3 for representation*/
} rr_cell_info_plmn_id_t;

typedef struct
{
  /* Indicates the serving cell ID */
  uint32                  cell_id;
  rr_ncell_t              neighbour_cell;
  rr_cell_info_plmn_id_t  plmn;
  uint16                  lac;     /* range 0..65535 */
  rr_cell_info_cb_cause_t rr_cell_info_cause;
//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type  as_id;
//endif /* F EATURE_DUAL_SIM */
} rr_cell_info_t;

/* Callback function type for cell ID change notification */
typedef void RR_CELL_CHANGE_IND_CB_FUNC_TYPE(rr_cell_info_t cell_info);


/*===========================================================================

                         FUNCTION DEFS

===========================================================================*/

boolean rr_register_cell_change_ind_cb
(
  RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr
);

boolean rr_deregister_cell_change_ind_cb
(
  RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr
);

boolean rr_ds_register_cell_change_ind_cb
(
  const sys_modem_as_id_e_type as_id,
  RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr
);

boolean rr_ds_deregister_cell_change_ind_cb
(
  const sys_modem_as_id_e_type as_id,
  RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr
);






#endif /* RR_CB_IF_H */
