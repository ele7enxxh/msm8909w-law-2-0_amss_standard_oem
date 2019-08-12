/*============================================================================
  @file rr_ded_config.c

  @brief This module contains acccess functions to parameters used during Dedicated.

                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_ded_config.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "gprs_mem.h"
#include "rr_ded_config.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "sys_cnst.h"
#include "rr_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  rr_dedicated_data_T *data_ptr;

} rr_ded_config_static_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static rr_ded_config_static_data_t  rr_ded_config_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the static module data.
 * 
 * \param gas_id 
 * 
 * \return rr_ded_config_static_data_t* 
 */
static rr_ded_config_static_data_t *rr_ded_config_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return(&rr_ded_config_data[as_index]);
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

/*!
 * \brief Access function for run-time dedicated data.
 * 
 * \param gas_id 
 * 
 * \return rr_dedicated_data_T* 
 */
rr_dedicated_data_T *rr_get_dedicated_data_ptr(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ded_config_static_data_t *rr_ded_config_data_ptr = rr_ded_config_get_data_ptr(gas_id);

  // If 'data_ptr' is NULL, then memory is not allocated yet
  if (rr_ded_config_data_ptr->data_ptr == NULL)
  {
    rr_ded_config_data_ptr->data_ptr = (rr_dedicated_data_T *)GPRS_MEM_CALLOC(sizeof(rr_dedicated_data_T));

    if (rr_ded_config_data_ptr->data_ptr != NULL)
    {
      // Setup default values
      rr_ded_config_data_ptr->data_ptr->curr_channel_spec.channel_mode_1 = SIGNALLING_ONLY;
      rr_ded_config_data_ptr->data_ptr->curr_channel_spec.channel_mode_2 = SIGNALLING_ONLY;
      rr_ded_config_data_ptr->data_ptr->l2_link_status = NO_LINK;
      rr_ded_config_data_ptr->data_ptr->cipher_mode_command_received = FALSE;
    }
    else
    {
      ERR_GERAN_FATAL_0_G("Unable to allocate storage for rr_ded_config_dataa");
    }
  }

  return rr_ded_config_data_ptr->data_ptr;
}

/*!
 * \brief Access function for current channel specification.
 * 
 * \param dedic_ptr 
 * \param gas_id 
 * 
 * \return channel_spec_T* 
 */
channel_spec_T * rr_get_channel_spec(rr_dedicated_data_T *dedic_ptr, const gas_id_t gas_id)
{
  channel_spec_T *chan_spec_ptr;

  /* caller can pass a NULL pointer to default to the global store */
  if (dedic_ptr == NULL)
  {
    dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  }

  if (dedic_ptr != NULL)
  {
    chan_spec_ptr = &(dedic_ptr->curr_channel_spec);
  }
  else
  {
    chan_spec_ptr = NULL;
  }

  return(chan_spec_ptr);

} /* end rr_get_channel_spec() */

/*!
 * \brief Update the dedicated config to indicate that there is no channel connected.
 * 
 * \param gas_id (in)
 */
void rr_ded_set_l1_chan_is_not_connected(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ded_config_static_data_t *rr_ded_config_data_ptr = rr_ded_config_get_data_ptr(gas_id);

  // If 'data_ptr' is NULL, then memory is not allocated yet
  if ((rr_ded_config_data_ptr != NULL) &&
      (rr_ded_config_data_ptr->data_ptr != NULL))
  {
    rr_ded_config_data_ptr->data_ptr->curr_channel_spec.number_of_ded_channels = 0;
  }
}

/*!
 * \brief Indicates if L1 has a dedicated channel connected or not.
 * 
 * \param gas_id (in)
 * 
 * \return boolean - TRUE if L1 has a channel connected, FALSE otherwise
 */
boolean rr_ded_l1_chan_is_connected(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ded_config_static_data_t *rr_ded_config_data_ptr = rr_ded_config_get_data_ptr(gas_id);

  // If 'data_ptr' is NULL, then memory is not allocated yet
  if ((rr_ded_config_data_ptr != NULL) &&
      (rr_ded_config_data_ptr->data_ptr != NULL))
  {
    if (rr_ded_config_data_ptr->data_ptr->curr_channel_spec.number_of_ded_channels > 0)
    {
      // There is a dedicated channel currently connected
      return TRUE;
    }
  }

  // No dedicated channel currently connected
  return FALSE;
}

/*!
 * \brief Invalidates (clears) the multirate parameters given.
 * 
 * \param multirate_ptr 
 */
void rr_invalidate_multirate_config(multirate_config_T *multirate_ptr)
{
  if (multirate_ptr != NULL)
  {
    memset(multirate_ptr, 0, sizeof(multirate_config_T));
  }
} /* end rr_invalidate_multirate_config() */

/*!
 * \brief Restores the previous dedicated configuration.
 * 
 * \param gas_id 
 */
void rr_restore_previous_dedicated_config(const gas_id_t gas_id)
{
  rr_dedicated_data_T *dedic_ptr;

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);

  if (dedic_ptr != NULL)
  {
    /* the "current" channel spec is restored from the "previous" store */
	dedic_ptr->curr_channel_spec = dedic_ptr->prev_channel_spec;


    /* the "current" AMR config is restored from the "previous" store */

	dedic_ptr->curr_amr_config = dedic_ptr->prev_amr_config;

  }
  else
  {
    MSG_GERAN_ERROR_0_G("Unable to restore dedicated data after reconnect");
  }
}

/*!
 * \brief Saves a copy of the current dedicated configuration.
 * 
 * \param gas_id 
 */
void rr_preserve_current_dedicated_config(const gas_id_t gas_id)
{
  rr_dedicated_data_T *dedic_ptr;

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);

  if (dedic_ptr != NULL)
  {
    /* the "current" channel spec is saved in the "previous" store */
	dedic_ptr->prev_channel_spec = dedic_ptr->curr_channel_spec;


    /* the "current" AMR config is saved in the "previous" store */
	dedic_ptr->prev_amr_config = dedic_ptr->curr_amr_config;

  }
  else
  {
    MSG_GERAN_ERROR_0_G("Unable to save dedicated data for reconnect");
  }
}

/*!
 * \brief Start using the new multirate configuration.
 * 
 * \param dedic_ptr 
 */
void rr_switch_to_next_multirate_config(rr_dedicated_data_T *dedic_ptr)
{
  if (dedic_ptr != NULL)
  {
    /* output some debug if the AMR config is about to become valid */
    if ((dedic_ptr->curr_amr_config.valid == FALSE) &&
        (dedic_ptr->next_amr_config.valid == TRUE))
    {
      MSG_GERAN_MED_0("RR has acquired a valid multirate (AMR) config");
    }

    /* now the "next" multirate config becomes the "current" one */
    dedic_ptr->curr_amr_config = dedic_ptr->next_amr_config;


    /* clear down the "next" multirate config */
    memset(&(dedic_ptr->next_amr_config), 0, sizeof(multirate_config_T));
  }
  else
  {
    MSG_GERAN_ERROR_0("Unable to switch to next multirate configuration");
  }
}

/*!
 * \brief Start using the existing multirate configuration.
 * 
 * \param dedic_ptr 
 */
void rr_use_existing_multirate_config(rr_dedicated_data_T *dedic_ptr)
{
  if (dedic_ptr != NULL)
  {
    /* the "next" multirate config is inherited from the "current" one */
    //Strcuture assignment 
    dedic_ptr->next_amr_config = dedic_ptr->curr_amr_config;

  }
}

/*!
 * \brief Updates the L2 link status.
 * 
 * \param link_status 
 * \param gas_id 
 */
void rr_set_link_status(link_status_T link_status, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ded_config_static_data_t *rr_ded_config_data_ptr = rr_ded_config_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_config_data_ptr);

  if (rr_ded_config_data_ptr->data_ptr != NULL)
  {
    rr_ded_config_data_ptr->data_ptr->l2_link_status = link_status;
  }
  else
  {
    MSG_GERAN_ERROR_1("Unable to set l2_link_status (%d)",(int)link_status);
  }
}

/*!
 * \brief Returns the current L2 link status.
 * 
 * \param gas_id 
 * 
 * \return link_status_T 
 */
link_status_T rr_get_link_status(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ded_config_static_data_t *rr_ded_config_data_ptr = rr_ded_config_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_config_data_ptr);

  if (rr_ded_config_data_ptr->data_ptr != NULL)
  {
    return rr_ded_config_data_ptr->data_ptr->l2_link_status;
  }
  else
  {
    return NO_LINK;
  }
}

/*!
 * \brief Discards the dedicated run-time data.
 * 
 * \param gas_id 
 */
void rr_ded_config_discard_data(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ded_config_static_data_t *rr_ded_config_data_ptr = rr_ded_config_get_data_ptr(gas_id);

  if ((rr_ded_config_data_ptr != NULL) &&
      (rr_ded_config_data_ptr->data_ptr != NULL))
  {
    MSG_GERAN_HIGH_0("Discarding rr_ded_config_data");

    GPRS_MEM_FREE(rr_ded_config_data_ptr->data_ptr);
    rr_ded_config_data_ptr->data_ptr = NULL;
  }
}

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
/*!
 * \brief Clears TDS-related dedicated data.
 * 
 * \param gas_id 
 */
void rr_tds_pseudo_fr_reset_dedicated_info(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ded_config_static_data_t *rr_ded_config_data_ptr = rr_ded_config_get_data_ptr(gas_id);

  if ((rr_ded_config_data_ptr != NULL) &&
      (rr_ded_config_data_ptr->data_ptr != NULL))
  {
    memset(&rr_ded_config_data_ptr->data_ptr->tds_pseudo_fr, 0, sizeof (rr_ded_config_data_ptr->data_ptr->tds_pseudo_fr));
  }
}
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

/*!
 * \brief Module initialisation function to be called at task start.
 */
void rr_ded_config_task_start_init(
  void
)
{
  // Zero the control/data structure(s)
  memset(rr_ded_config_data, 0, sizeof(rr_ded_config_data));
}

/* EOF */

