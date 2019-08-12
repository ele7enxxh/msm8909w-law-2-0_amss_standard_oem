/*****************************************************************************
***
*** TITLE
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_callback.c#1 $
***
*** DESCRIPTION
***
***  RR callback functionality.  Provides callback for multiple regstrations
***  (set by RR_MAX_NUM_OF_REG_CB).  User registers a function with the
***  callback handler.  This function(s) is triggered by the callback handler when
***  the data item being monitored has changed.
***
***  Currently provided:
***    1) Serving cell callback - Provides the cell_id of the serving cell.
***    2) Best six neighbour cells callback -  Provides cell_id's of the best
***       six neighbour cells.
***
***  Both neighbour cell and serving cell id changes are serviced by:
***
***  Register a cell id callback call:
***    rr_register_cell_change_ind_cb()
***
***  Deregister a cell id callback function call:
***    rr_deregister_cell_change_ind_cb()
***
***  RR notifies the callback handler of an update in serving cell_id:
***    rr_notify_cell_id_change()
***
***  RR notifies the callback handler of an update in neigbour cell_id(s):
***    rr_handle_ncell_callback()
***
***  Reset the current stored cell_ids:
***    rr_init_cell_change_cb_data()
***
*** Copyright (c) 2008-2014 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/

/****************************************************************************
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/11/10   SW      Updated for Dual SIM callback registration
16/06/08   RN      Created file
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h"  /* Includes FEATURE_... defines */

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
#include "rr_seg_load.h"
#include "rr_callback.h"
#include "rr_cb_if.h"
#include "rr_cb_if_g.h"
#include "msg.h"
#include "rr_multi_sim.h"

/*===========================================================================

                     LOCAL DATA

==========================================================================*/
/* Store of current serving cell and neighbour cell ids */
static rr_cell_info_t  rr_callback_cell_info[NUM_GERAN_DATA_SPACES];

typedef enum
{
  RR_CALLBACK_CELLID_TRIGGER,
  RR_CALLBACK_NCELLS_TRIGGER
} rr_callback_trigger_t;

typedef struct
{
  RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_fn_ptr;
  sys_modem_as_id_e_type           as_id;
} rr_cell_change_cb_t;

/* Array of registered callback functions for cell id */
static rr_cell_change_cb_t rr_callback_list[RR_MAX_NUM_OF_REG_CB] = {{NULL}};

/*===========================================================================

                            FUNCTIONS

==========================================================================*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_cell_info_t*
 */
static rr_cell_info_t *rr_callback_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_callback_cell_info[as_index];
}

/*===========================================================================

FUNCTION RR_INIT_CELL_CHANGE_CB_DATA

DESCRIPTION

Initializes all cell change callback data.

PARAMETERS

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  write rr_callback_cell_info

===========================================================================*/
void rr_init_cell_change_cb_data(
  const gas_id_t gas_id
)
{
  uint8 i;
  rr_cell_info_t *rr_callback_data_ptr;

  // Obtain a pointer to the module data
  rr_callback_data_ptr = rr_callback_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_callback_data_ptr);

  rr_callback_data_ptr->cell_id            = RR_CELL_ID_NOT_PRESENT;
  rr_callback_data_ptr->rr_cell_info_cause = NO_CAUSE;

  rr_callback_data_ptr->neighbour_cell.num_cells = 0;
  for (i = 0; i < RR_MAX_NUM_NEIGHBOUR_CELLS; i++)
  {
    rr_callback_data_ptr->neighbour_cell.cell[i].cell_id = RR_CELL_ID_NOT_PRESENT;
  }

  rr_callback_data_ptr->plmn.plmn_id[0] = 0;
  rr_callback_data_ptr->plmn.plmn_id[1] = 0;
  rr_callback_data_ptr->plmn.plmn_id[2] = 0;
  rr_callback_data_ptr->lac             = 0;

#ifdef FEATURE_DUAL_SIM
  rr_callback_data_ptr->as_id = SYS_MODEM_AS_ID_NONE;
#endif /* FEATURE_DUAL_SIM */

  return;
}

/*===========================================================================

FUNCTION RR_CB_REGISTER_INTERNAL

DESCRIPTION

  Private utility function to register a new callback function pointer

PARAMETERS

  None

RETURN VALUE

  Pointer to an entry in the callback list or NULL if failure occurred

SIDE EFFECTS

  None

===========================================================================*/
static rr_cell_change_cb_t *rr_cb_register_internal(RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr)
{
  if (cb_ptr != NULL)
  {
    rr_cell_change_cb_t *list_ptr;
    uint16               i;

    /* insert callback function in the next available space */
    list_ptr = rr_callback_list;
    for (i = 0; i < RR_MAX_NUM_OF_REG_CB; i++, list_ptr++)
    {
      if (list_ptr->cb_fn_ptr == NULL)
      {
        list_ptr->cb_fn_ptr = cb_ptr;
        list_ptr->as_id = SYS_MODEM_AS_ID_NONE;
        return(list_ptr);
      }
    }

    if (i == RR_MAX_NUM_OF_REG_CB)
    {
      MSG_GERAN_ERROR_1("RR callback list full - cannot register 0x%08X",(int)(cb_ptr));
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("RR cannot register NULL callback");
  }

  return(NULL);
}

/*===========================================================================

FUNCTION RR_REGISTER_CELL_CHANGE_IND_CB

DESCRIPTION

This function enables upper layers to register a callback function if notification
of the serving cell id and/or neighbour cell is required.  Once the function is registered,
it will be called whenever there is a serving/neighbour cell change.


PARAMETERS

  cb_ptr - pointer to function that requires registration

RETURN VALUE

  BOOLEAN:  TRUE signifies that registration has succeeded.  FALSE signifies
            that registration failed.

SIDE EFFECTS

  Read/write rr_cell_change_ind_cb_func_ptr_ptr

===========================================================================*/

boolean rr_register_cell_change_ind_cb(RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr)
{
  if (rr_cb_register_internal(cb_ptr) != NULL)
  {
    MSG_GERAN_HIGH_1("RR registered callback 0x%08X",(int)(cb_ptr));
    return(TRUE);
  }

  return(FALSE);
}

boolean rr_ds_register_cell_change_ind_cb(const sys_modem_as_id_e_type as_id, RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr)
{
#ifdef FEATURE_DUAL_SIM
  rr_cell_change_cb_t *entry_ptr;

  entry_ptr = rr_cb_register_internal(cb_ptr);
  if (entry_ptr != NULL)
  {
    /* assign an Active Subscription ID to this callback */
    entry_ptr->as_id = as_id;
    MSG_GERAN_HIGH_2("Registered callback 0x%08X for ASID %d",(int)(cb_ptr),(int)as_id);
    return(TRUE);
  }
#else
  (void)as_id;
  (void)cb_ptr;
  MSG_GERAN_ERROR_0_G("rr_ds_register_cell_change_ind_cb() called but FEATURE_DUAL_SIM not def");
#endif /* FEATURE_DUAL_SIM */

  return(FALSE);
}
/*===========================================================================

FUNCTION RR_DEREGISTER_CELL_CHANGE_IND_CB

DESCRIPTION

This function enables upper layer to deregister the callback function that was
registered for notification of serving/neighbour cell id.


PARAMETERS

  cb_ptr - pointer to function that requires deregistration

RETURN VALUE

  BOOLEAN:  TRUE signifies that deregistration has succeeded.  FALSE signifies
            that deregistration failed.

SIDE EFFECTS

  read/write rr_cell_change_ind_cb_func_ptr_ptr

===========================================================================*/

boolean rr_deregister_cell_change_ind_cb(RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr)
{
  uint16 i;

  for (i = 0; i < RR_MAX_NUM_OF_REG_CB; i++)
  {
    if (rr_callback_list[i].cb_fn_ptr == cb_ptr)
    {
      MSG_GERAN_HIGH_2("RR Deregistering rr_serving_cell_change_ind_cb ind: %d cb: 0x%x",i,cb_ptr);
      rr_callback_list[i].cb_fn_ptr = NULL;

      return(TRUE);
    }
  }

  if (i == RR_MAX_NUM_OF_REG_CB)
  {
    MSG_GERAN_ERROR_1("RR Deregister Cannot find cb: 0x%x in list",cb_ptr);
  }

  return(FALSE);
}

boolean rr_ds_deregister_cell_change_ind_cb(const sys_modem_as_id_e_type as_id, RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr)
{
#ifdef FEATURE_DUAL_SIM
  uint16 i;

  for (i = 0; i < RR_MAX_NUM_OF_REG_CB; i++)
  {
    if ((rr_callback_list[i].cb_fn_ptr == cb_ptr) &&
        (rr_callback_list[i].as_id     == as_id))
    {
      rr_callback_list[i].cb_fn_ptr = NULL;
      MSG_GERAN_HIGH_1("RR deregistered callback 0x%08X",(int)(cb_ptr));

      return(TRUE);
    }
  }

  if (i == RR_MAX_NUM_OF_REG_CB)
  {
    MSG_GERAN_ERROR_1("RR deregister cannot find callback: 0x%08X in list",(int)(cb_ptr));
  }

  return(FALSE);
#else
  (void)as_id;
  (void)cb_ptr;
  MSG_GERAN_ERROR_0("rr_ds_deregister_cell_change_ind_cb() called but FEATURE_DUAL_SIM not def");
  return FALSE;
#endif /* FEATURE_DUAL_SIM */
}

/*===========================================================================

FUNCTION RR_CALLBACKS_EXECUTE

DESCRIPTION

  This function is called to notify the registered callbacks

PARAMETERS

  cb_trigger: used to debug the reason the callbacks are being executed

RETURN VALUE

  VOID

SIDE EFFECTS

  None

===========================================================================*/
static void rr_callbacks_execute(
  const rr_callback_trigger_t cb_trigger,
  const gas_id_t gas_id
)
{
  RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr;
  rr_cell_change_cb_t             *list_ptr;
  uint16                           num_exec = 0;
  uint16                           i;
  rr_cell_info_t *rr_callback_data_ptr;

  // Obtain a pointer to the module data
  rr_callback_data_ptr = rr_callback_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_callback_data_ptr);

#ifdef FEATURE_DUAL_SIM
  /* refresh the ASID in the current data space's structure */
  rr_callback_data_ptr->as_id = geran_map_gas_id_to_nas_id(gas_id);
#endif /* FEATURE_DUAL_SIM */

  list_ptr = rr_callback_list;
  for (i = 0; i < RR_MAX_NUM_OF_REG_CB; i++, list_ptr++)
  {
#ifdef FEATURE_DUAL_SIM
    if (list_ptr->as_id != rr_callback_data_ptr->as_id)
    {
      /* this callback was not registered for the active subscription */
      cb_ptr = NULL;
    }
    else
#endif /* FEATURE_DUAL_SIM */
    {
      cb_ptr = list_ptr->cb_fn_ptr;
    }

    if (cb_ptr != NULL)
    {
      cb_ptr(*rr_callback_data_ptr);
      num_exec++;
    }
  }

  /* if any callbacks were executed, print some debug */
  if (num_exec != 0)
  {
    if (cb_trigger == RR_CALLBACK_CELLID_TRIGGER)
    {
      MSG_GERAN_HIGH_0_G("RR has executed registered callbacks for serving cell ID change");
    }
    else if (cb_trigger == RR_CALLBACK_NCELLS_TRIGGER)
    {
      MSG_GERAN_HIGH_0_G("RR has executed registered callbacks for neighbour cell change");
    }
  }

  return;
}

/*===========================================================================

FUNCTION RR_CONVERT_BEST_SIX_CELLS

DESCRIPTION

Convert the RR internal representation of the six best ncells (best_six_ncells_t)
into the format used in the rr callback interface (rr_ncell_t).  Cell id's
not present are set to RR_CELL_ID_NOT_PRESENT.

PARAMETERS

  best_six - best six ncells for conversion.

RETURN VALUE

  rr_ncell_t - converted ncell structure.

SIDE EFFECTS

  None.

===========================================================================*/
static rr_ncell_t rr_convert_best_six_cells(best_six_ncells_t best_six)
{
  uint8 ncell_index = 0;
  uint8 best_six_index = 0;
  rr_ncell_t ncells;

  RR_ZERO_STRUCTURE(ncells);

  /* Initialize ncell data */
  ncells.num_cells = 0;

  for (ncell_index = 0; ncell_index < RR_MAX_NUM_NEIGHBOUR_CELLS; ncell_index++)
  {
    ncells.cell[ncell_index].cell_id = RR_CELL_ID_NOT_PRESENT;
  }

  /* Convert best six cells into rr_ncell_t format */
  for (best_six_index = 0;
       (best_six_index < best_six.no_of_entries) && (best_six_index < RR_MAX_NUM_NEIGHBOUR_CELLS);
       best_six_index++)
  {
    if (best_six.ncell_entry[best_six_index].ncell_data != NULL)
    {
      if (best_six.ncell_entry[best_six_index].ncell_data->bcch_or_pmo == BCCH_FORMAT)
      {
        if (best_six.ncell_entry[best_six_index].ncell_data->ncell_data.bcch.gsm.cell_id_valid == TRUE)
        {
          ncells.cell[ncells.num_cells].cell_id = best_six.ncell_entry[best_six_index].ncell_data->ncell_data.bcch.gsm.cell_id;
          ncells.num_cells++;
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_1("GRR Ncell data is NULL in best_six[%d]",best_six_index);
    }
  }

  return(ncells);
}

/*===========================================================================

FUNCTION RR_NOTIFY_CELL_ID_CHANGE

DESCRIPTION

This function is called to notify the registered callback of the serving
cell id.

PARAMETERS

  cell_id - updated serving cell id
  best_six - best six neighbour cell_ids

RETURN VALUE

  None.

SIDE EFFECTS

  read/write rr_callback_cell_info
  Call function pointers in rr_cell_change_ind_cb_func_ptr_ptr

===========================================================================*/

void rr_notify_cell_id_change(
  word cell_id,
  best_six_ncells_t best_six,
  LAI_T lai,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_cell_info_t *rr_callback_data_ptr = rr_callback_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_callback_data_ptr);

  if (cell_id != rr_callback_data_ptr->cell_id)
  {
    /* Update the Ncell when the serving cell changes because the ncell list change
       might not have been triggered yet (avoids the case of same cell id being reported
       in both scell and ncell lists) */
    rr_callback_data_ptr->neighbour_cell = rr_convert_best_six_cells(best_six);
    rr_callback_data_ptr->cell_id = cell_id;

    rr_callback_data_ptr->plmn.plmn_id[0] = lai.plmn_id.identity[0];
    rr_callback_data_ptr->plmn.plmn_id[1] = lai.plmn_id.identity[1];
    rr_callback_data_ptr->plmn.plmn_id[2] = lai.plmn_id.identity[2];
    rr_callback_data_ptr->lac             = lai.location_area_code;

    rr_callbacks_execute(RR_CALLBACK_CELLID_TRIGGER, gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION RR_DIFF_NCELLS

DESCRIPTION

Diff two neighbour cell lists

PARAMETERS

  ncell_a - First set of ncells to diff
  ncell_b - Second set of ncells to diff

RETURN VALUE

  boolean - TRUE - ncell lists differ, FALSE - ncell lists same

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rr_diff_ncells(rr_ncell_t ncell_a, rr_ncell_t ncell_b)
{
  uint8 ncell_index;

  for (ncell_index = 0; ncell_index < RR_MAX_NUM_NEIGHBOUR_CELLS; ncell_index++)
  {
    if (ncell_a.cell[ncell_index].cell_id != ncell_b.cell[ncell_index].cell_id)
    {
      return(TRUE);
    }
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION RR_HANDLE_NCELL_CALLBACK

DESCRIPTION

This function is called by RR with the latest best six neighbour cells.
The latest best six are diff'd against the stored best six.  If they are
different the callback is triggered.

PARAMETERS

  best_six - best six neighbour cell ids

RETURN VALUE

  None.

SIDE EFFECTS

  Read rr_callback_cell_info

===========================================================================*/

void rr_handle_ncell_callback(
  best_six_ncells_t best_six,
  const gas_id_t gas_id
)
{
  rr_ncell_t converted_best_six = {0};
  rr_cell_info_t *rr_callback_data_ptr;

  // Obtain a pointer to the module data
  rr_callback_data_ptr = rr_callback_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_callback_data_ptr);

  converted_best_six = rr_convert_best_six_cells(best_six);

  if (rr_diff_ncells(converted_best_six,rr_callback_data_ptr->neighbour_cell) == TRUE)
  {
    rr_callback_data_ptr->neighbour_cell = converted_best_six;
    rr_callbacks_execute(RR_CALLBACK_NCELLS_TRIGGER, gas_id);
  }

  return;
}

#ifdef PERLUTF
#error code not present
#endif

#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

/* EOF */
