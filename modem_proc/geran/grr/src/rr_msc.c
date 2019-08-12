/*!
 * \file rr_msc.c 
 *  
 * This module contains functionality to manage advertised multi-slot class. 
 *  
 *              Copyright (c) 2014 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_msc.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "rr_msc.h"
#include "rr_defs.h"
#include "rr_nv.h"
#include "rr_if.h"
#include "rr_mm_send.h"
#include "rr_multi_sim.h"
#include "rr_thermal.h"
#include "rr_gprs_defs.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define MAX_NUM_OF_TX_SLOTS 4
#define MAX_NUM_OF_RX_SLOTS 5

#define MAX_NUM_OF_DTM_TX_SLOTS 3
#define MAX_NUM_OF_DTM_RX_SLOTS 4

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

// Typedef for a MSC calculation function, which takes 'tx_slots' and 'rx_slots' and returns 'msc'
typedef uint8 (*rr_calc_msc_fn_t)(uint8, uint8);

typedef struct
{
  uint8  tx_slots;
  uint8  rx_slots;
} tx_rx_slots_t;

typedef struct
{
  uint8          msc_val;
  tx_rx_slots_t  tx_rx_slots;
} rr_msc_t;

typedef struct
{
  rr_msc_t  gprs_multislot_class;
  rr_msc_t  egprs_multislot_class;
#ifdef FEATURE_GSM_DTM
  rr_msc_t  dtm_multislot_class;
#endif
} rr_msc_data_t;

/**
 * The module data is comprised of: valid - set when the NV data becomes available 
 * orig_msc_data - Original values (unmodified, copy of NV values) 
 * mod_msc_data  - Modified values per client 
 * new_msc_data  - New values, determining after combining all the modified values (i.e. lowest common 
 *                 denominator)
 */
typedef struct
{
  boolean        valid;
  rr_msc_data_t  orig_msc_data;
  rr_msc_data_t  mod_msc_data[RR_MSC_CLIENT_MAX_COUNT];
  rr_msc_data_t  new_msc_data;
  boolean        msc_changed;
} rr_msc_module_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

// Lookup table of TX vs RX slots => MSC
// Note: Table starts with 1 TX 1 RX (not 0 TX 0 RX)
const uint8 tx_rx_slots_to_msc_lookup[MAX_NUM_OF_RX_SLOTS][MAX_NUM_OF_TX_SLOTS] =
{
   {1,  5,  7, 12},    //    tx --> (1-4)
   {2,  5,  7, 12},    // rx
   {4,  6,  7, 12},    // |
   {8, 10, 11, 12},    // |
   {30, 31, 32, 33}     // v (1-5)
};

// Lookup table of TX vs RX slots => DTM MSC
// Note: Table starts with 2 TX 2 RX (not 0 TX 0 RX, or 1 TX 1 RX)
// Note: MSC9 is skipped with a jump from MSC11 to MSC5
const uint8 tx_rx_slots_to_dtm_msc_lookup[MAX_NUM_OF_DTM_RX_SLOTS-1][MAX_NUM_OF_DTM_TX_SLOTS-1] =
{
   {5,  5},    //    tx > (2-3)
   {5,  5},    // rx
   {5, 11}     // v (2-4)
};

// Value to be used when using MSC as an index
const uint8 msc_to_index_lookup[] = {1,2,4,5,6,7,8,9,10,11,12,30,31,32,33};

// Look-up of TX+RX slots per MSC
const tx_rx_slots_t msc_index_to_tx_rx_lookup[] =
{
  {1,1},  // MSC=1  => 1 TX, 1 RX
  {1,2},  // MSC=2  => 1 TX, 2 RX
  {1,3},  // MSC=4  => 1 TX, 3 RX
  {2,2},  // MSC=5  => 2 TX, 2 RX
  {2,3},  // MSC=6  => 2 TX, 3 RX
  {3,3},  // MSC=7  => 3 TX, 3 RX
  {1,4},  // MSC=8  => 1 TX, 4 RX
  {2,3},  // MSC=9  => 2 TX, 3 RX
  {2,4},  // MSC=10 => 2 TX, 4 RX
  {3,4},  // MSC=11 => 3 TX, 4 RX
  {4,4},  // MSC=12 => 4 TX, 4 RX
  {1,5},  // MSC=30 => 1 TX, 5 RX
  {2,5},  // MSC=31 => 2 TX, 5 RX
  {3,5},  // MSC=32 => 3 TX, 5 RX
  {4,5}   // MSC=33 => 4 TX, 5 RX
};

// Module data per instance
static rr_msc_module_data_t rr_msc_module_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 * 
 * \param as_id (in)
 * 
 * \return rr_msc_module_data_t* 
 */
static rr_msc_module_data_t *rr_msc_get_data_ptr(const sys_modem_as_id_e_type as_id)
{
#ifdef FEATURE_TRIPLE_SIM
  if (as_id == SYS_MODEM_AS_ID_3)
  {
    return &rr_msc_module_data[GERAN_DATA_SPACE_INDEX_3];
  }
#endif // FEATURE_TRIPLE_SIM

#ifdef FEATURE_DUAL_SIM
  if (as_id == SYS_MODEM_AS_ID_2)
  {
    return &rr_msc_module_data[GERAN_DATA_SPACE_INDEX_2];
  }
#endif // FEATURE_DUAL_SIM

  return &rr_msc_module_data[GERAN_DATA_SPACE_INDEX_1];
}

/*!
 * \brief Calculate the MSC value from the given number of TX slots & RX slots.
 * 
 * \param tx_slots (in)
 * \param rx_slots (in)
 * 
 * \return uint8 - MSC value
 */
static uint8 calc_msc_from_tx_rx_slots(uint8 tx_slots, uint8 rx_slots)
{
  uint8 msc = 0;

  if ((tx_slots > 0) && (rx_slots > 0))
  {
    tx_slots = MIN(tx_slots, MAX_NUM_OF_TX_SLOTS);
    rx_slots = MIN(rx_slots, MAX_NUM_OF_RX_SLOTS);

    msc = tx_rx_slots_to_msc_lookup[rx_slots-1][tx_slots-1];
  }
  
  MSG_GERAN_HIGH_3("[MSC} tx_slots=%d rx_slots=%d => msc=%d", (int)tx_slots, (int)rx_slots, (int)msc);

  return msc;
}

/*!
 * \brief Calculate the DTM MSC value from the given number of TX slots & RX slots.
 * 
 * \param tx_slots (in)
 * \param rx_slots (in)
 * 
 * \return uint8 - DTM MSC value
 */
static uint8 calc_dtm_msc_from_tx_rx_slots(uint8 tx_slots, uint8 rx_slots)
{
  uint8 msc = 0;

  // Note: Minimum of 2 slots required for DTM (due to requiring 1 slot for CS)
  if ((tx_slots >= 2) && (rx_slots >= 2))
  {
    tx_slots = MIN(tx_slots, MAX_NUM_OF_DTM_TX_SLOTS);
    rx_slots = MIN(rx_slots, MAX_NUM_OF_DTM_RX_SLOTS);

    msc = tx_rx_slots_to_dtm_msc_lookup[rx_slots-2][tx_slots-2];
  }
  
  MSG_GERAN_HIGH_3("[MSC} tx_slots=%d rx_slots=%d => dtm_msc=%d", (int)tx_slots, (int)rx_slots, (int)msc);

  return msc;
}

/*!
 * \brief Given a MSC value, returns the index needed to index into msc_index_to_tx_rx_lookup.
 * 
 * \param msc (in)
 * \param index_ptr (out)
 * 
 * \return boolean - TRUE if MSC value found, FALSE otherwise
 */
static boolean msc_to_index(uint8 msc, uint8 *index_ptr)
{
  uint8 index;

  for (index = 0; index < ARR_SIZE(msc_to_index_lookup); index++)
  {
    if (msc_to_index_lookup[index] == msc)
    {
      // Set output value
      *index_ptr = index;

      // Exit now
      return TRUE;
    }
  }

  return FALSE;
}

/*!
 * \brief Given a MSC value, returns the number of TX slots & RX slots.
 * 
 * \param msc (in)
 * \param tx_slots_ptr (out) 
 * \param rx_slots_ptr (out)
 */
static void calc_tx_rx_slots_from_msc(uint8 msc, uint8 *tx_slots_ptr, uint8 *rx_slots_ptr)
{
  RR_NULL_CHECK_RETURN_VOID(tx_slots_ptr);
  RR_NULL_CHECK_RETURN_VOID(rx_slots_ptr);

  if (0 == msc)
  {
    *tx_slots_ptr = 0;
    *rx_slots_ptr = 0;
  }
  else
  {
    uint8 msc_index;

    if (msc_to_index(msc, &msc_index))
    {
      if (msc_index < ARR_SIZE(msc_index_to_tx_rx_lookup))
      {
        *tx_slots_ptr = msc_index_to_tx_rx_lookup[msc_index].tx_slots;
        *rx_slots_ptr = msc_index_to_tx_rx_lookup[msc_index].rx_slots;
      }
    }
  }

  MSG_GERAN_HIGH_3(
    "[MSC} msc=%d => tx_slots=%d rx_slots=%d",
    (int)msc,
    (int)*tx_slots_ptr,
    (int)*rx_slots_ptr
  );

  return;
}

/*!
 * \brief Calculates a new MSC value after setting a specific number of TX slots.
 * 
 * \param msc_ptr (in/out)
 * \param tx_slots (in)
 * \param rx_slots (in)
 * \param calc_msc_fn (in)
 * 
 * \return boolean - TRUE if the MSC value has changed, FALSE otherwise
 */
static boolean set_tx_rx_slots(
  rr_msc_t *msc_ptr,
  uint8 tx_slots,
  uint8 rx_slots,
  rr_calc_msc_fn_t calc_msc_fn
)
{
  uint8 msc;

  RR_NULL_CHECK_RETURN_PARAM(msc_ptr, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(calc_msc_fn, FALSE);

  msc_ptr->tx_rx_slots.tx_slots = tx_slots;
  msc_ptr->tx_rx_slots.rx_slots = rx_slots;

  msc = (*calc_msc_fn)(
    msc_ptr->tx_rx_slots.tx_slots,   // tx_slots
    msc_ptr->tx_rx_slots.rx_slots    // rx_slots
  );

  if (msc_ptr->msc_val != msc)
  {
    msc_ptr->msc_val = msc;

    // MSC value changed
    return TRUE;
  }

  // MSC value not changed
  return FALSE;
}

/*!
 * \brief Calculates a new MSC value after reducing the number of TX slots.
 * 
 * \param msc_ptr (in/out)
 * \param min_tx_slots (in)
 * \param calc_msc_fn (in)
 * 
 * \return boolean - TRUE if the MSC value has changed, FALSE otherwise
 */
static boolean reduce_tx_slots(rr_msc_t *msc_ptr, uint8 min_tx_slots, rr_calc_msc_fn_t calc_msc_fn)
{
  RR_NULL_CHECK_RETURN_PARAM(msc_ptr, FALSE);

  // Check if TX slots can be reduced by at least one
  if (msc_ptr->tx_rx_slots.tx_slots > min_tx_slots)
  {
    uint8 msc;

    --msc_ptr->tx_rx_slots.tx_slots;

    // Reduce TX slots by one more if possible
    if (msc_ptr->tx_rx_slots.tx_slots > min_tx_slots)
    {
      --msc_ptr->tx_rx_slots.tx_slots;
    }

    msc = (*calc_msc_fn)(
      msc_ptr->tx_rx_slots.tx_slots,   // tx_slots
      msc_ptr->tx_rx_slots.rx_slots    // rx_slots
    );

    if (msc_ptr->msc_val != msc)
    {
      msc_ptr->msc_val = msc;

      // MSC value changed
      return TRUE;
    }
  }

  // MSC value not changed
  return FALSE;
}

/*!
 * \brief Compares two sets of TX/RX slot data and determines the lowest number of TX & RX slots.
 * 
 * \param tx_rx_slots_1 (in)
 * \param tx_rx_slots_2 (in)
 * \param result_tx_rx_slots_ptr (out)
 */
static void determine_lowest_num_tx_rx_slots(
  tx_rx_slots_t   tx_rx_slots_1,
  tx_rx_slots_t   tx_rx_slots_2,
  tx_rx_slots_t  *result_tx_rx_slots_ptr
)
{
  result_tx_rx_slots_ptr->tx_slots = MIN(tx_rx_slots_1.tx_slots, tx_rx_slots_2.tx_slots);
  result_tx_rx_slots_ptr->rx_slots = MIN(tx_rx_slots_1.rx_slots, tx_rx_slots_2.rx_slots);

  return;
}

/*!
 * \brief Determines the new MSC values based on the modified values across clients.
 * 
 * \param as_id (in)
 */
static void determine_new_msc(sys_modem_as_id_e_type as_id)
{
  uint8 i;

#ifdef FEATURE_DUAL_SIM
  gas_id_t gas_id = RR_AS_ID_TO_GAS_ID;   // gas_id used for diag
#endif /* FEATURE_DUAL_SIM */

  // Obtain a pointer to the module data
  rr_msc_module_data_t *module_data_ptr = rr_msc_get_data_ptr(as_id);
  RR_NULL_CHECK_FATAL(module_data_ptr);

  // Start with the assumption that the first set of modified data has the lowest number of timeslots
  module_data_ptr->new_msc_data = module_data_ptr->mod_msc_data[0];

  for (i = 1; i < RR_MSC_CLIENT_MAX_COUNT; i++)
  {
    determine_lowest_num_tx_rx_slots(
      module_data_ptr->new_msc_data.gprs_multislot_class.tx_rx_slots,
      module_data_ptr->mod_msc_data[i].gprs_multislot_class.tx_rx_slots,
      &module_data_ptr->new_msc_data.gprs_multislot_class.tx_rx_slots
    );

    determine_lowest_num_tx_rx_slots(
      module_data_ptr->new_msc_data.egprs_multislot_class.tx_rx_slots,
      module_data_ptr->mod_msc_data[i].egprs_multislot_class.tx_rx_slots,
      &module_data_ptr->new_msc_data.egprs_multislot_class.tx_rx_slots
    );

#ifdef FEATURE_GSM_DTM
    determine_lowest_num_tx_rx_slots(
      module_data_ptr->new_msc_data.dtm_multislot_class.tx_rx_slots,
      module_data_ptr->mod_msc_data[i].dtm_multislot_class.tx_rx_slots,
      &module_data_ptr->new_msc_data.dtm_multislot_class.tx_rx_slots
    );
#endif
  }

  MSG_GERAN_HIGH_0_G("[MSC] Determine new values");

  module_data_ptr->new_msc_data.gprs_multislot_class.msc_val = calc_msc_from_tx_rx_slots(
    module_data_ptr->new_msc_data.gprs_multislot_class.tx_rx_slots.tx_slots,
    module_data_ptr->new_msc_data.gprs_multislot_class.tx_rx_slots.rx_slots
  );

  module_data_ptr->new_msc_data.egprs_multislot_class.msc_val = calc_msc_from_tx_rx_slots(
    module_data_ptr->new_msc_data.egprs_multislot_class.tx_rx_slots.tx_slots,
    module_data_ptr->new_msc_data.egprs_multislot_class.tx_rx_slots.rx_slots
  );

#ifdef FEATURE_GSM_DTM
  module_data_ptr->new_msc_data.dtm_multislot_class.msc_val = calc_dtm_msc_from_tx_rx_slots(
    module_data_ptr->new_msc_data.dtm_multislot_class.tx_rx_slots.tx_slots,
    module_data_ptr->new_msc_data.dtm_multislot_class.tx_rx_slots.rx_slots
  );
#endif

  return;
}

/*!
 * \brief Compares the given MSC data with that last updated to MM.
 * 
 * \param msc_data_ptr (in)
 * \param as_id (in)
 * 
 * \return boolean - TRUE if the MSC data is different to that last updated to MM, FALSE otherwise.
 */
boolean compare_msc_to_gmm_msc(rr_msc_data_t *msc_data_ptr, sys_modem_as_id_e_type as_id)
{
  uint8 gprs_multislot_class;
  uint8 egprs_multislot_class;
#ifdef FEATURE_GSM_DTM
  uint8 dtm_multislot_class;
#endif
  boolean gmm_updated;

  gas_id_t gas_id = RR_AS_ID_TO_GAS_ID;

  // Assume no MSC value has changed
  boolean msc_changed = FALSE;

  // Obtain a pointer to the module data
  rr_msc_module_data_t *module_data_ptr = rr_msc_get_data_ptr(as_id);

  RR_NULL_CHECK_RETURN_PARAM(msc_data_ptr, FALSE);

  gmm_updated = rr_mm_get_multislot_class(
    &egprs_multislot_class,
#ifdef FEATURE_GSM_DTM
    &dtm_multislot_class,
#endif
    &gprs_multislot_class,
    gas_id
  );

  // If GMM has not been updated before, then it will be using the original (NV) values
  if (FALSE == gmm_updated)
  {
    gprs_multislot_class = module_data_ptr->orig_msc_data.gprs_multislot_class.msc_val;
    egprs_multislot_class = module_data_ptr->orig_msc_data.egprs_multislot_class.msc_val;
#ifdef FEATURE_GSM_DTM
    dtm_multislot_class = module_data_ptr->orig_msc_data.dtm_multislot_class.msc_val;
#endif
  }

  if (gprs_multislot_class != msc_data_ptr->gprs_multislot_class.msc_val)
  {
    MSG_GERAN_HIGH_2_G(
      "[MSC] gprs_multislot_class=%d (was %d)",
      (int)msc_data_ptr->gprs_multislot_class.msc_val,
      (int)gprs_multislot_class
    );

    msc_changed = TRUE;
  }

  if (egprs_multislot_class != msc_data_ptr->egprs_multislot_class.msc_val)
  {
    MSG_GERAN_HIGH_2_G(
      "[MSC] egprs_multislot_class=%d (was %d)",
      (int)msc_data_ptr->egprs_multislot_class.msc_val,
      (int)egprs_multislot_class
    );

    msc_changed = TRUE;
  }

#ifdef FEATURE_GSM_DTM
  if (dtm_multislot_class != msc_data_ptr->dtm_multislot_class.msc_val)
  {
    MSG_GERAN_HIGH_2_G(
      "[MSC] dtm_multislot_class=%d (was %d)",
      (int)msc_data_ptr->dtm_multislot_class.msc_val,
      (int)dtm_multislot_class
    );

    msc_changed = TRUE;
  }
#endif

  return msc_changed;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Used to set a specific number of TX slots.
 * 
 * \param msc_client (in)
 * \param tx_slots (in)
 * \param rx_slots (in)
 * \param as_id (in)
 * 
 * \return boolean - TRUE if the MSC has changed, FALSE otherwise
 */
boolean rr_msc_set_tx_rx_slots(
  rr_msc_client_e_t msc_client,
  uint8 tx_slots,
  uint8 rx_slots,
  sys_modem_as_id_e_type as_id
)
{
  rr_msc_data_t *msc_data_ptr;

#ifdef FEATURE_DUAL_SIM
  gas_id_t gas_id = RR_AS_ID_TO_GAS_ID;   // gas_id used for diag
#endif /* FEATURE_DUAL_SIM */
  boolean msc_changed = FALSE;

  // Obtain a pointer to the module data
  rr_msc_module_data_t *module_data_ptr = rr_msc_get_data_ptr(as_id);

  if ((module_data_ptr != NULL) &&
      (module_data_ptr->valid))
  {
    uint8 new_tx_slots;
    uint8 new_rx_slots;

    msc_data_ptr = &module_data_ptr->mod_msc_data[msc_client];

    MSG_GERAN_HIGH_1_G("[MSC] Set TX slots (%d)", (int)tx_slots);

    // GPRS_MULTISLOT_CLASS
    if (msc_data_ptr->gprs_multislot_class.msc_val != 0)
    {
      // A value of 0 means revert to the original value
      if (0 == tx_slots)
      {
        MSG_GERAN_HIGH_0_G("[MSC] Revert TX slots");

        new_tx_slots = module_data_ptr->orig_msc_data.gprs_multislot_class.tx_rx_slots.tx_slots;
      }
      else
      {
        new_tx_slots = MIN(tx_slots, MAX_NUM_OF_TX_SLOTS);
      }

      // A value of 0 means revert to the original value
      if (0 == rx_slots)
      {
        MSG_GERAN_HIGH_0_G("[MSC] Revert RX slots");

        new_rx_slots = module_data_ptr->orig_msc_data.gprs_multislot_class.tx_rx_slots.rx_slots;
      }
      else
      {
        new_rx_slots = MIN(rx_slots, MAX_NUM_OF_RX_SLOTS);
      }

      msc_changed |= set_tx_rx_slots(
        &msc_data_ptr->gprs_multislot_class,      // msc_ptr
        new_tx_slots,                             // tx_slots
        new_rx_slots,                             // rx_slots
        calc_msc_from_tx_rx_slots                 // calc_msc_fn
      );
    }

    // EGPRS_MULTISLOT_CLASS
    if (msc_data_ptr->egprs_multislot_class.msc_val != 0)
    {
      // A value of 0 means revert to the original value
      if (0 == tx_slots)
      {
        MSG_GERAN_HIGH_0_G("[MSC] Revert TX slots");

        new_tx_slots = module_data_ptr->orig_msc_data.egprs_multislot_class.tx_rx_slots.tx_slots;
      }
      else
      {
        new_tx_slots = MIN(tx_slots, MAX_NUM_OF_TX_SLOTS);
      }

      // A value of 0 means revert to the original value
      if (0 == rx_slots)
      {
        MSG_GERAN_HIGH_0_G("[MSC] Revert RX slots");

        new_rx_slots = module_data_ptr->orig_msc_data.egprs_multislot_class.tx_rx_slots.rx_slots;
      }
      else
      {
        new_rx_slots = MIN(rx_slots, MAX_NUM_OF_RX_SLOTS);
      }

      msc_changed |= set_tx_rx_slots(
        &msc_data_ptr->egprs_multislot_class,     // msc_ptr
        new_tx_slots,                             // tx_slots
        new_rx_slots,                             // rx_slots
        calc_msc_from_tx_rx_slots                 // calc_msc_fn
      );
    }

#ifdef FEATURE_GSM_DTM
    // DTM_MULTISLOT_CLASS
    if (msc_data_ptr->dtm_multislot_class.msc_val != 0)
    {
      // A value of 0 means revert to the original value
      if (0 == tx_slots)
      {
        MSG_GERAN_HIGH_0_G("[MSC] Revert TX slots");

        new_tx_slots = module_data_ptr->orig_msc_data.dtm_multislot_class.tx_rx_slots.tx_slots;
      }
      else
      {
        // Note: The number of slots is incremented for DTM for the CS slot
        new_tx_slots = MIN(tx_slots+1, MAX_NUM_OF_DTM_TX_SLOTS);
      }

      // A value of 0 means revert to the original value
      if (0 == rx_slots)
      {
        MSG_GERAN_HIGH_0_G("[MSC] Revert RX slots");

        new_rx_slots = module_data_ptr->orig_msc_data.dtm_multislot_class.tx_rx_slots.rx_slots;
      }
      else
      {
        // Note: The number of slots is incremented for DTM for the CS slot
        new_rx_slots = MIN(rx_slots+1, MAX_NUM_OF_DTM_RX_SLOTS);
      }

      msc_changed |= set_tx_rx_slots(
        &msc_data_ptr->dtm_multislot_class,       // msc_ptr
        new_tx_slots,                             // tx_slots
        new_rx_slots,                             // rx_slots
        calc_dtm_msc_from_tx_rx_slots             // calc_msc_fn
      );
    }
#endif

    if (msc_changed)
    {
      // Re-calculate new MSC values
      determine_new_msc(as_id);
    }

    if (compare_msc_to_gmm_msc(&module_data_ptr->new_msc_data, as_id))
    {
      // Flag that an update is required
      module_data_ptr->msc_changed = TRUE;

      // Exit indicating update required
      return TRUE;
    }

    // Flag that an update is not required
    module_data_ptr->msc_changed = FALSE;
  }

  // Exit indicating update not required
  return FALSE;
}

/*!
 * \brief Used to reduce the number of TX slots.
 * 
 * \param msc_client (in)
 * \param as_id (in)
 * 
 * \return boolean - TRUE if the MSC has changed, FALSE otherwise
 */
boolean rr_msc_reduce_tx_slots(rr_msc_client_e_t msc_client, sys_modem_as_id_e_type as_id)
{
  rr_msc_data_t *msc_data_ptr;

#ifdef FEATURE_DUAL_SIM
  gas_id_t gas_id = RR_AS_ID_TO_GAS_ID;   // gas_id used for diag
#endif /* FEATURE_DUAL_SIM */
  boolean msc_changed = FALSE;

  // Obtain a pointer to the module data
  rr_msc_module_data_t *module_data_ptr = rr_msc_get_data_ptr(as_id);

  if ((module_data_ptr != NULL) &&
      (module_data_ptr->valid))
  {
    msc_data_ptr = &module_data_ptr->mod_msc_data[msc_client];

    MSG_GERAN_HIGH_0_G("[MSC] Reduce TX slots");

    // GPRS_MULTISLOT_CLASS
    if (msc_data_ptr->gprs_multislot_class.msc_val != 0)
    {
      msc_changed |= reduce_tx_slots(
        &msc_data_ptr->gprs_multislot_class,   // msc_ptr
        1,                                     // min_tx_slots
        calc_msc_from_tx_rx_slots              // calc_msc_fn
      );
    }

    // EGPRS_MULTISLOT_CLASS
    if (msc_data_ptr->egprs_multislot_class.msc_val != 0)
    {
      msc_changed |= reduce_tx_slots(
        &msc_data_ptr->egprs_multislot_class,   // msc_ptr
        1,                                      // min_tx_slots
        calc_msc_from_tx_rx_slots               // calc_msc_fn
      );
    }

#ifdef FEATURE_GSM_DTM
    // DTM_MULTISLOT_CLASS
    if (msc_data_ptr->dtm_multislot_class.msc_val != 0)
    {
      msc_changed |= reduce_tx_slots(
        &msc_data_ptr->dtm_multislot_class,   // msc_ptr
        2,                                    // min_tx_slots
        calc_dtm_msc_from_tx_rx_slots         // calc_msc_fn
      );
    }
#endif

    if (msc_changed)
    {
      // Re-calculate new MSC values
      determine_new_msc(as_id);
    }

    if (compare_msc_to_gmm_msc(&module_data_ptr->new_msc_data, as_id))
    {
      // Flag that an update is required
      module_data_ptr->msc_changed = TRUE;

      // Exit indicating update required
      return TRUE;
    }

    // Flag that an update is not required
    module_data_ptr->msc_changed = FALSE;
  }

  // Exit indicating update not required
  return FALSE;
}

/*!
 * \brief Used to revert the MSC data to the original NV values.
 * 
 * \param msc_client (in)
 * \param as_id (in)
 * 
 * \return boolean - TRUE if the MSC has changed, FALSE otherwise
 */
boolean rr_msc_revert_to_orig(rr_msc_client_e_t msc_client, sys_modem_as_id_e_type as_id)
{
#ifdef FEATURE_DUAL_SIM
  gas_id_t gas_id = RR_AS_ID_TO_GAS_ID;   // gas_id used for diag
#endif /* FEATURE_DUAL_SIM */

  // Obtain a pointer to the module data
  rr_msc_module_data_t *module_data_ptr = rr_msc_get_data_ptr(as_id);

  if ((module_data_ptr != NULL) &&
      (module_data_ptr->valid))
  {
    if (msc_client < RR_MSC_CLIENT_MAX_COUNT)
    {
      MSG_GERAN_HIGH_0_G("[MSC] Revert to original values");

      // Copy all of 'original' into 'modified' for the specified client
      module_data_ptr->mod_msc_data[msc_client] = module_data_ptr->orig_msc_data;

      // Re-calculate new MSC values
      determine_new_msc(as_id);

      if (compare_msc_to_gmm_msc(&module_data_ptr->new_msc_data, as_id))
      {
        // Flag that an update is required
        module_data_ptr->msc_changed = TRUE;

        // Exit indicating update required
        return TRUE;
      }
    }

    // Flag that an update is not required
    module_data_ptr->msc_changed = FALSE;
  }

  // Exit indicating update not required
  return FALSE;
}

/*!
 * \brief Called when the NV data becomes available.
 * 
 * \param as_id (in)
 */
void rr_msc_nv_ready(const sys_modem_as_id_e_type as_id)
{
  rr_msc_data_t *msc_data_ptr;

  // Obtain a pointer to the module data
  rr_msc_module_data_t *module_data_ptr = rr_msc_get_data_ptr(as_id);

  msc_data_ptr = &module_data_ptr->orig_msc_data;

  msc_data_ptr->gprs_multislot_class.msc_val = rr_get_nv_multislot_class(as_id);

  calc_tx_rx_slots_from_msc(
    msc_data_ptr->gprs_multislot_class.msc_val,
    &msc_data_ptr->gprs_multislot_class.tx_rx_slots.tx_slots,
    &msc_data_ptr->gprs_multislot_class.tx_rx_slots.rx_slots
  );

  msc_data_ptr->egprs_multislot_class.msc_val = rr_get_nv_egprs_multislot_class(as_id);

  calc_tx_rx_slots_from_msc(
    msc_data_ptr->egprs_multislot_class.msc_val,
    &msc_data_ptr->egprs_multislot_class.tx_rx_slots.tx_slots,
    &msc_data_ptr->egprs_multislot_class.tx_rx_slots.rx_slots
  );

#ifdef FEATURE_GSM_DTM
  msc_data_ptr->dtm_multislot_class.msc_val = rr_get_nv_dtm_multislot_class(as_id);

  calc_tx_rx_slots_from_msc(
    msc_data_ptr->dtm_multislot_class.msc_val,
    &msc_data_ptr->dtm_multislot_class.tx_rx_slots.tx_slots,
    &msc_data_ptr->dtm_multislot_class.tx_rx_slots.rx_slots
  );
#endif

  // Copy all of 'original' into 'modified' and 'new' for all clients
  {
    uint8 i;

    for (i = 0; i < RR_MSC_CLIENT_MAX_COUNT; i++)
    {
      module_data_ptr->mod_msc_data[i] = *msc_data_ptr;
    }

    module_data_ptr->new_msc_data = *msc_data_ptr;
  }

  // Data is valid for this instance (subscription)
  module_data_ptr->valid = TRUE;

  return;
}

/*!
 * \brief Returns the MSC values.
 * 
 * \param egprs_multislot_class_ptr (out)
 * \param dtm_multislot_class_ptr (out)
 * \param gprs_multislot_class_ptr (out)
 * \param as_id 
 */
void rr_msc_get_multislot_class(
  uint8 *egprs_multislot_class_ptr,
#ifdef FEATURE_GSM_DTM
  uint8 *dtm_multislot_class_ptr,
#endif
  uint8 *gprs_multislot_class_ptr,
  sys_modem_as_id_e_type as_id
)
{
  // Obtain a pointer to the module data
  rr_msc_module_data_t *module_data_ptr = rr_msc_get_data_ptr(as_id);

  if (gprs_multislot_class_ptr != NULL)
  {
    *gprs_multislot_class_ptr = module_data_ptr->new_msc_data.gprs_multislot_class.msc_val;
  }

  if (egprs_multislot_class_ptr != NULL)
  {
    *egprs_multislot_class_ptr = module_data_ptr->new_msc_data.egprs_multislot_class.msc_val;
  }

  if (dtm_multislot_class_ptr != NULL)
  {
    *dtm_multislot_class_ptr = module_data_ptr->new_msc_data.dtm_multislot_class.msc_val;
  }

  return;
}

/*!
 * \brief Converts DTM multislot subclass value to a DTM multislot class value.
 * 
 * \param dtm_multislot_subclass (in)
 * 
 * \return uint8 - DTM multislot class value
 */
uint8 rr_msc_conv_dtm_multislot_subclass_to_class(uint8 dtm_multislot_subclass)
{
  // See 3GPP 24.008 Table 10.5.146 MS Radio Access Capability IE

  switch (dtm_multislot_subclass)
  {
    case 1:  return  5;   // 01 => MSC= 5
    case 2:  return  9;   // 10 => MSC= 9
    case 3:  return 11;   // 11 => MSC=11
    default: ;
  }

  // DTM not configured
  return 0;
}

/*!
 * \brief Converts DTM multislot class value to a DTM multislot subclass value.
 * 
 * \param dtm_multislot_class (in)
 * 
 * \return uint8 - DTM multislot subclass value
 */
uint8 rr_msc_conv_dtm_multislot_class_to_subclass(uint8 dtm_multislot_class)
{
  // See 3GPP 24.008 Table 10.5.146 MS Radio Access Capability IE

  switch (dtm_multislot_class)
  {
    case  5:  return 1;    // MSC= 5 => 01
    case  9:  return 2;    // MSC= 9 => 10
    case 11:  return 3;    // MSC=11 => 11
    default: ;
  }

  // DTM not configured
  return 0;
}

#ifdef FEATURE_DUAL_SIM
/*!
 * \brief Process function for MPH_COEX_MSC_CHANGE_IND
 * 
 * \param msg_ptr (in)
 */
void rr_msc_process_l1_coex_msc_change_ind(mph_coex_msc_change_ind_T *msg_ptr, const gas_id_t gas_id)
{
  boolean msc_changed;

  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  // A value of 0 means restore the original NV MSC values
  if ((0 == msg_ptr->ul_slot_restriction) &&
      (0 == msg_ptr->dl_slot_restriction))
  {
    msc_changed = rr_msc_revert_to_orig(
      RR_MSC_CLIENT_L1_COEX,   // msc_client
      as_id                    // as_id
    );
  }
  else
  {
    msc_changed = rr_msc_set_tx_rx_slots(
      RR_MSC_CLIENT_L1_COEX,          // mcs_client
      msg_ptr->ul_slot_restriction,   // num_tx_slots
      msg_ptr->dl_slot_restriction,   // num_rx_slots
      as_id                           // as_id
    );
  }

  if (msc_changed)
  {
    MSG_GERAN_HIGH_0_G("[MSC] Update required");

    rr_msc_send_rr_imsg_msc_change_ind(as_id);
  }
  else
  {
    MSG_GERAN_HIGH_0_G("[MSC] Update not required");
  }

  return;
}
#endif // FEATURE_DUAL_SIM

/*!
 * \brief Updates MM with new multi-slot class parameters (if required). 
 *        Note: Also calls into the RR-THERMAL module which will update the service capability (if required). 
 * 
 * \param can_send_service_ind 
 * \param gas_id 
 */
void rr_msc_update(boolean can_send_service_ind, const gas_id_t gas_id)
{
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  // Obtain a pointer to the module data
  rr_msc_module_data_t *module_data_ptr = rr_msc_get_data_ptr(as_id);

  // Check if a MSC update is required
  if (module_data_ptr->msc_changed)
  {
    if (rr_thermal_ps_allowed(gas_id))
    {
      uint8 gprs_multislot_class;
      uint8 egprs_multislot_class;
#ifdef FEATURE_GSM_DTM
    uint8 dtm_multislot_class;
#endif

    uint8 temp_egprs_multislot_class = 0;
    uint8 temp_dtm_multislot_class = 0;

      rr_msc_get_multislot_class(
        &egprs_multislot_class,
#ifdef FEATURE_GSM_DTM
      &dtm_multislot_class,
#endif
        &gprs_multislot_class,
        as_id
      );

      temp_egprs_multislot_class = egprs_multislot_class,
#ifdef FEATURE_GSM_DTM
    temp_dtm_multislot_class = dtm_multislot_class;
#endif

    MSG_GERAN_HIGH_3_G(
      "Updating multislot class: GPRS=%d EGPRS=%d DTM=%d",
      (int)gprs_multislot_class,
      (int)temp_egprs_multislot_class,
      (int)temp_dtm_multislot_class
    );

      rr_public_store_update_multislot_class(
        egprs_multislot_class,
#ifdef FEATURE_GSM_DTM
      dtm_multislot_class,
#endif
        gprs_multislot_class,
        gas_id
      );

      rr_send_gmm_gprs_multislot_class_ind(
        egprs_multislot_class,
#ifdef FEATURE_GSM_DTM
      dtm_multislot_class,
#endif
        gprs_multislot_class,
        gas_id
      );

      module_data_ptr->msc_changed = FALSE;
    }
    else
    {
      MSG_GERAN_HIGH_0_G("Multislot class update not done - PS blocked");
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Multislot class unchanged");
  }

  // Check if any update to service capability is required
  rr_thermal_update(can_send_service_ind, gas_id);

  return;
}

/*!
 * \brief Sends RR_IMSG_MSC_CHANGE_IND.
 * 
 * \param as_id (in) Note: Not GAS-ID
 */
void rr_msc_send_rr_imsg_msc_change_ind(sys_modem_as_id_e_type as_id)
{
  rr_imsg_msc_change_ind_t  msg;

  gas_id_t gas_id = RR_AS_ID_TO_GAS_ID;

  msg.header.imh.message_id  = RR_IMSG_MSC_CHANGE_IND;
  msg.header.imh.message_set = MS_RR_RR_EXT;

  PUT_IMH_LEN((sizeof(rr_imsg_msc_change_ind_t) - sizeof(IMH_T)), &(msg.header.imh));

  msg.header.rr_message_set = RR_IMSG;
  msg.header.gas_id         = gas_id;

  if (GS_SUCCESS != gs_send_message(GS_QUEUE_RR, (void *)&msg, FALSE))
  {
    MSG_GERAN_ERROR_0_G("Unable to send RR_IMSG_MSC_CHANGE_IND");
  }

  return;
}

/*!
 * \brief Initialisation function called once at RR task start.
 */
void rr_msc_task_start_init(void)
{
  // Zero data structure across all subs
  memset(&rr_msc_module_data, 0, sizeof(rr_msc_module_data));

  return;
}

/* EOF */

