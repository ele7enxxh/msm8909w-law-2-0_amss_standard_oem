#ifndef IRATMAN_I_H
#define IRATMAN_I_H

/*!
  @file   iratman_i.h

  @brief  IRAT List Manager internal header file

  @details
  Defines IRAT List Manager related types and interfaces used internally.

*/

/*===========================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/iratman/inc/iratman_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/16   vko/pk  Changes for FR-33944 (D2L redirection support)
01/12/15   cjb     Added Tbackoff capping algorithm
11/19/14   pk      Changes to reduce allotted time for 1x and D2L measurements
08/05/14   cjb     Avoid accessing de-allocated memory and code clean-up
08/01/14   cjb     New API to return LTE freqs that are ready for measurement
06/09/14   pk      Changes for FR-17879 (Extended EARFCN and band range)
05/28/14   cjb     Round-robin approach of returning INACTIVE nodes to avoid
                   returning the same node for multi-sim cases where home_rat
                   might not be able to measure all nodes in one DRx cycle.
02/05/14   bph     Evaluate reselection algorithms independently of reporting
                   client.
01/24/14   bph     F3 removal/level audit
01/14/14   bph     Set Tbackoff/Treselect defaults same as legacy
12/16/13   bph     Improve logging/clean up
12/10/13   bph     Remove dependancy on L1 for cell info
12/06/13   bph     FR3839 - dynamic tbackoff
05/16/13   bph     LTE Deprioritization + fix freq count/get next freq logic
05/10/13   bph     Log Tbackoff expiry when it occurs
05/09/13   bph     Hook in QTF
03/25/13   bph     Store RAT's node (more or less) preferrence
03/13/13   bph     Fix a null pointer dereference
01/09/13   cjb     Added logging capability.
04/16/12   cjb     Adding support for LTE comparison algorithms(RSRP & RSRQ).
04/06/12   cjb     Using MSG_SSID_IRATMAN for QXDM logging.
02/13/12   cjb     Increased the DEBUG_MSG level to aid in debugging.
10/28/11   cjb     First working version.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>    /* Definition of basic types and macros */
#include <customer.h>

/* IRATMan exported header */
#include "iratman.h"

/* IRATMan LTE Availability header */
#include "iratman_la.h"

/* Other */
#include "err.h"
#include "lte_as.h"
#include "lte_cphy_irat_meas_msg.h"
#include "msg.h"
#include "timetick.h"

/*===========================================================================

                    INTERNAL DEFINES

===========================================================================*/

/*-------------------------------------------------------------------------
      Defined Constants & Macros
-------------------------------------------------------------------------*/
/* Worst case time required for one LTE freq meas to complete [in ms] */
#define IRATMAN_LTE_MIN_MEAS_DURATION_MS          50

/* Buffer time for ABORT processing [in ms] */
#define IRATMAN_LTE_MEAS_ABORT_BUFFER_MS          50

/* Per C.S0097-0 v2.0 spec, the range for RxLevMinEUTRACommon is 0 - 96 */
#define IRATMAN_LTE_RSRP_MIN_EUTRA_HIGH           96

/* Per C.S0097-0 v2.0 spec: -44 - RxLevMinEUTRA in dBm is equal to the minimum
  reference signal received power (RSRP) level of an EUTRA cell */
#define IRATMAN_LTE_RSRP_MIN_EUTRA_CEILING        -44

/* Adjust pe_max range from [0 dBm, +63 dBm] to [-30 dBm, +33 dBm]
  (required because of different configuration on Network side.) */
#define IRATMAN_LTE_RSRP_PE_MAX_OFFSET            -30

/* Per C.S0097-0 v2.0 spec, the range for RxLevMinEUTRAOffsetCommon is 1 - 8 */
#define IRATMAN_LTE_RSRP_MIN_EUTRA_OFFSET_LOW     1
#define IRATMAN_LTE_RSRP_MIN_EUTRA_OFFSET_HIGH    8

/* Per 3GPP2 C.S0087-A v1.0 spec, the range for QualMinEUTRACommon is 0 - 31 */
#define IRATMAN_LTE_RSRQ_MIN_EUTRA_HIGH           31

/* Per 3GPP2 C.S0087-A v1.0 spec: (-3 – QualMinEUTRACommon) in dB is equal to
   the minimum reference signal received quality (RSRQ) level of a EUTRA cell */
#define IRATMAN_LTE_RSRQ_MIN_EUTRA_CEILING        -3

/* Per 3GPP2 C.S0087-A v1.0 spec: range for QualMinEUTRAOffset is 1 - 8 */
#define IRATMAN_LTE_RSRQ_MIN_EUTRA_OFFSET_LOW     1
#define IRATMAN_LTE_RSRQ_MIN_EUTRA_OFFSET_HIGH    8

/* Post Treselect expiry, the RATs will be notified about pending measurements
   via the registered callbacks. But, this does not guarantee that RATs will
   come back immediately asking for measurements. Hence, an additional buffer
   time is provided, where in the Treselect expiry is still considered valid.
   If the RATs come back after the buffer time, we will clear previous meas
   results and restart the measurements.

   NOTE: The buffer time has to be >= (5.12 s) to allow sleep. */
#define IRATMAN_LTE_TRESELECT_EXTENSION_MS        10000

/* Additional time to allow a sleep(5.12 s) to complete for a RAT[1x / HDR] */
#define IRATMAN_LTE_TIME_BUFFER_MS                6000

/* Default Treselect duration in ms */
#define IRATMAN_LTE_DEFAULT_TRESELECT_DUR_MS      5120  /*  5.12 sec */

/* Default Tbackoff duration in ms */
#define IRATMAN_LTE_DEFAULT_TBACKOFF_DUR_MS       10240 /* 10.24 sec */

/* Tbackoff timer cap value = 2^6 = 64 seconds */
#define IRATMAN_LTE_TBACKOFF_CAP                  6

/*===========================================================================

                    INTERNAL ENUM'S, MACROS, TYPEDEFS

===========================================================================*/

/*! Enable DEBUG functionality */
#define IRATMAN_DEBUG_ENABLED

/*! Internal MSG macros */
#define IRATMAN_DEBUG_MSG( fmt ) MSG( MSG_SSID_IRATMAN, MSG_LEGACY_LOW, fmt )

#define IRATMAN_INFO_MSG( fmt )  MSG( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH, fmt )

#define IRATMAN_ERROR_MSG( fmt ) MSG( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR, fmt )

/*! Macro to do error fatal when assert condition fails */
#define IRATMAN_ASSERT( xx_exp )                              \
  if( !( xx_exp ) )                                           \
  {                                                           \
    ERR_FATAL( "Assertion ( " #xx_exp " ) failed", 0, 0, 0 ); \
  }

/*! Maximum number of cells that can be measured in a single meas req */
#define IRATMAN_LTE_MAX_MEAS_CELLS       LTE_CPHY_IMEAS_MAX_MEAS_CELLS

/*! @brief type used to indicate which RATs a node is known to */
typedef uint16 iratman_rat_mask_type;

/*! @brief Macro to translate iratman_rat_e_type to array index */
#define RAT_MASK( RAT )     ( 1 << ( ( (uint8)(RAT) ) - 1 ) )

/*! @brief List mask */
#define LIST_MASK( list )   ( 1 << ( (uint8) (list) ) )

#define RETURN_VOID

#define IRATMAN_RAT_VALID( RAT, retval )                                     \
  if( ( RAT <= RAT_INT ) || ( RAT >= RAT_MAX ) )                             \
  {                                                                          \
    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,                               \
           "Invalid RAT passed %d", RAT);                                    \
    return retval;                                                           \
  }

#define IRATMAN_LTE_LIST_VALID( LIST, retval )                               \
  if( ( LIST != IRATMAN_LTE_MP_LIST ) && ( LIST != IRATMAN_LTE_LP_LIST ) )   \
  {                                                                          \
    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,                               \
           "Invalid LTE list passed %d", LIST );                             \
    return retval;                                                           \
  }

#define IRATMAN_LTE_ALGO_VALID( ALGO, retval )                                \
  if( ( ALGO != IRATMAN_LTE_ALGO_RSRP ) && ( ALGO != IRATMAN_LTE_ALGO_RSRQ )  \
        && ( ALGO != IRATMAN_LTE_ALGO_RSRP_RSRQ ))                                 \
  {                                                                           \
    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,                                \
           "Invalid LTE algoritm passed %d", ALGO );                          \
    return retval;                                                            \
  }

#define IRATMAN_NULL_PTR_CHECK( ptr, retval )                                \
  if (ptr == NULL)                                                           \
  {                                                                          \
    MSG( MSG_SSID_IRATMAN, MSG_LEGACY_FATAL, "Unexpected Null Ptr" );        \
    return retval;                                                           \
  }

#define IRATMAN_ERR_FATAL_IF_NULL( ptr )                                     \
  if (ptr == NULL)                                                           \
  {                                                                          \
    ERR_FATAL( "Fatal: Null Ptr", 0, 0, 0 );                                 \
  }

/* Conversion macro for time in "ms" to "sclks" */
#define TIMETICK_MS_FROM_SCLK(sclk) ((sclk)*1000/TIMETICK_NOMINAL_FREQ_HZ)

/* Both time values are unsigned, find which time occurs first taking into
  account a rollover with a range much larger than the distance between
  time_a and time_b. Returns TRUE if time_a occurs before time_b. */
#define TIME_OCCURS_FIRST( time_a, time_b ) \
  ( ( ( time_a ) - ( time_b ) ) > ( ( time_b ) - ( time_a ) ) )


/*===========================================================================

                    INTERNAL STRUCTURES

===========================================================================*/

/*! @brief RAT identifier */
typedef enum
{
  IRATMAN_LTE_INACTIVE                     = 0x0,
  IRATMAN_LTE_TBACKOFF_ACTIVE              = 0x1,
  IRATMAN_LTE_TBACKOFF_BUFFER_ACTIVE       = 0x2,
  IRATMAN_LTE_TBACKOFF_EXPIRED             = 0x3,
  IRATMAN_LTE_TRESELECT_ACTIVE             = 0x4,
  IRATMAN_LTE_TRESELECT_EXPIRED            = 0x5,
  IRATMAN_LTE_RESELECT                     = 0x6
} iratman_lte_meas_state;

/* lte_update_overall_meas_result() makes assumption about enum order
   do not change order w/o aligning w/ lte_update_overall_meas_result */
typedef enum iratman_lte_meas_result_type
{
  IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE,
  IRATMAN_LTE_MEAS_RESULT_ABOVE_CLOSE,
  IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X
} iratman_lte_meas_result_type;

/*! ----------------------------------- */
/*! LTE: Measurement Criteria Structure */
/*! ----------------------------------- */
typedef struct
{
  /*! State of LTE measurement */
  iratman_lte_meas_state           state;

  /*! Evolved Absolute Radio Frequency Channel Number (LTE freq number) */
  lte_earfcn_t                     earfcn;

  /*! Priority of the EARFCN of the neighboring EUTRA network */
  int8                             earfcn_priority[RAT_MAX];

  /*! Measuring bandwidth */
  lte_bandwidth_e                  bandwidth[RAT_MAX];

  /*! Threshold of serving(home_rat) frequency in -0.5 dB unit */
  uint8                            thresh_serving[RAT_MAX];

  /*! max Reselection timer */
  uint8                            max_treselect[RAT_MAX];

  /*! Reselection timer duration (in ms) */
  uint32                           treselect_dur_ms[RAT_MAX];

  /*! Treselect timer expiration time in units of sclks [absolute time] */
  timetick_type                    treselect_exp_sclks;

  /*! min Backoff timer */
  uint8                            min_tbackoff[RAT_MAX];

  /*! max Backoff timer */
  uint8                            max_tbackoff[RAT_MAX];

  /*! actual Backoff timer duration (in ms) */
  uint32                           tbackoff_dur_ms[RAT_MAX];

  /*! Tbackoff timer expiration time in units of sclks [absolute time] */
  timetick_type                    tbackoff_exp_sclks;

  /*! Last time the node was measured in units of sclks [absolute time] */
  timetick_type                    last_used_sclks;

  /* Whether treselect and tbackoff need to be overwritten */
  boolean                          override_treselect_tbackoff;

} iratman_mc_lte;


/*! ----------------------------------- */
/*! LTE: Comparison Criteria Structure  */
/*! ----------------------------------- */
/*!
    @brief    Struct type of array storing LTE cell IDs
              which pass reselection criteria
*/
typedef struct
{
  /*! Array containing LTE cells which pass the reselection criteria */
  struct
  {
    /*! Cell ID which passes the reselection criteria */
    uint16    cell_id;

    /*! Q_measured as provided by LTE in LTE measurement confirmation message in
         dBm units, range is -144dBm to -44dBm */
    int16     s_eutra_val;

  } cell_arr[ IRATMAN_LTE_MAX_MEAS_CELLS ];

  /*! Number of cells in the array */
  uint8                                   cnt;

} lte_reselect_cell_arr_struct_type;

/*!
    @brief    Comparison criteria used for LTE measurements
*/
typedef struct
{
  /*! Measurement comparison algorithm to be used for evaluation */
  iratman_lte_algo_e_type             meas_comp_algo[RAT_MAX];

  /*! ---------------------------- RSRP ---------------------------- */
  /*! Minimum received RSRP level from the EUTRA cell */
  uint8                               rx_lev_min_eutra[RAT_MAX];

  /*! Offset to Minimum reference power level to select EUTRA */
  uint8                               rx_lev_min_eutra_offset[RAT_MAX];

  /*! Maximum TX power in dBm */
  uint8                               pe_max[RAT_MAX];

  /*! Minimum required signal quality threshold used for selecting an EARFCN */
  uint8                               thresh_x[RAT_MAX];

  /*! ---------------------------- RSRQ ---------------------------- */
  /*! Minimum received RSRQ level from the EUTRA cell */
  uint8                               rsrq_min_eutra[RAT_MAX];

  /*! Offset to minimum RSRQ level to select EUTRA */
  uint8                               rsrq_min_eutra_offset[RAT_MAX];

  /*! Minimum required signal quality threshold used for selecting an EARFCN */
  uint8                               rsrq_thresh_x[RAT_MAX];

  /*! --------------------------- Result --------------------------- */
  /*! Measurement result:
        Mobile maximum TX power as reported by LTE in dBm */
  int16                               lte_max_tx_power;

  /*! Measurement result:
        Copy of LTE Meas Confirmation parameters (rcvd from LTE) */
  lte_cphy_irat_meas_meas_cnf_s       lte_meas_cnf_params;

  /*! Save candidate LTE cell IDs in this array to pick the final candidate */
  lte_reselect_cell_arr_struct_type   first_meas_cells;
  lte_reselect_cell_arr_struct_type   second_meas_cells;
  iratman_lte_reselect_type           reselect_cell;

} iratman_cc_lte;

/*! ----------------------------------- */
/*! LTE: Node data Structure            */
/*! ----------------------------------- */
/*!
    @brief    LTE RAT node structure
*/
typedef struct
{
  iratman_mc_lte              mc;     //!< Measurement criteria
  iratman_cc_lte              cc;     //!< Comparison criteria
} iratman_lte_node_type;


/*! ----------------------------------- */
/*!  Node related generic structures    */
/*! ----------------------------------- */
/*!
    @brief    RAT node header structure
*/
typedef struct
{
  iratman_rat_e_type  rat;        /*! RAT this node represents */
  uint8               rats_known; /*! RATs to which this node is known to */
  boolean             in_use;     /*! Measurement in progress flag */
  uint8               list[RAT_MAX]; /*! list preference per RAT */
} iratman_rat_node_hdr_type;

/*!
    @brief    Measurement RAT Node data
    @details  This defines the node data structures used by iratman.
              It contains a union of data from all RAT types that support meas.
*/
typedef union
{
  iratman_lte_node_type   lte;   //!< LTE node data
} iratman_rat_node_data;

/*!
    @brief    Measurement RAT Node structure
    @details  This defines the node structure used by various lists in iratman.
*/
typedef struct iratman_rat_node_type
{
  iratman_rat_node_hdr_type     hdr;    //!< Header
  iratman_rat_node_data         data;   //!< Data
  struct iratman_rat_node_type* next;   //!< Pointer to next node
} iratman_rat_node_type;


/*! @brief    RAT node header structure */
typedef struct
{
  iratman_rat_list_e_type   name; //!< List name
  iratman_rat_e_type        rat;  //!< RAT this list belongs to
} iratman_rat_list_hdr_type;


/*! @brief    List type */
typedef struct
{
  iratman_rat_list_hdr_type   hdr;        //!< Header
  iratman_rat_node_type*      head_ptr;   //!< Head node
  uint8                       cnt;        //!< Number of nodes in the list
} iratman_rat_list_type;


/*-------------------------------------------------------------------------
      LTE related generic typedefs, structs, union
-------------------------------------------------------------------------*/
/*  Contains all control information on pilot measurements that are not
    specific to any one LTE frequency, including pilot measurement state.
    Some of the information are shared between all LTE frequencies.
    Some are used as part of reselection criteria. */
typedef struct
{
  /* Used to arm HW timer for Treselect */
  timer_type              treselect_timer;

  /* Used to arm HW timer for Tbackoff */
  timer_type              tbackoff_timer;

  /* Array of nodes to be used for LTE frequency measurement */
  iratman_rat_node_type   node_pool[ IRATMAN_LTE_FREQ_ARR_SIZE ];

  /* Array of LTE list pointers [More preferred, Less preferred, Free pool] */
  iratman_rat_list_type   all_freq_lists[ IRATMAN_LTE_NUM_LIST ];

  /* Lte Availability structure*/
  iratman_la_type         la;
} lte_ctrl_info_struct_type;


/*-------------------------------------------------------------------------
      IRATMan related generic typedefs, structs, union
-------------------------------------------------------------------------*/
/* Common data for each measured RAT */
typedef struct
{
  /* Flag to remember initialization state */
  boolean                   initialized;

  /* Client Mask - used to keep track of active clients */
  uint8                     client_mask;

  /* The IRATMan global lock for critical sections */
  rex_crit_sect_type        iratman_crit_sect;

  /* Wakeup callback for various RATs */
  wakeup_cb_type            wakeup_cb[ RAT_MAX ];

  /* Get Cell Info callback for various RATs */
  get_cell_info_cb_type     get_cell_info_cb[ RAT_MAX ];

  /* True if dynamic backoff is enabled, otherwise false */
  boolean                   is_dynamic_backoff_enabled;

  /* RAT lists */
  lte_ctrl_info_struct_type lte; //!< LTE

} iratman_ctrl_info_type;


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION    IRATMAN_SEC_RAN_NEXT

DESCRIPTION   This function generates secure random numbers.

DEPENDENCIES  None.

RETURN VALUE  32 bit random number

SIDE EFFECTS  None.
===========================================================================*/
extern uint32 iratman_sec_ran_next
(
   uint32 default_ran_num  /* Default value to be returned if HW block fails */
);


/*===========================================================================
FUNCTION    IRATMAN_LTE_GET_CELL_INFO_CB

DESCRIPTION   This function gets a clients get cell info call back function.

DEPENDENCIES  None.

RETURN VALUE  Get Cell Info function pointer.

SIDE EFFECTS  None.
===========================================================================*/
extern get_cell_info_cb_type iratman_lte_get_cell_info_cb
(
   iratman_rat_e_type home_rat
);
#endif /* IRATMAN_I_H */

