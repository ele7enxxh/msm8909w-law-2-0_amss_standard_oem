#ifndef IRATMAN_H
#define IRATMAN_H

/*!
  @file    iratman.h

  @brief   IRAT List Manager header file

  @details  Defines IRAT List Manager related types and interfaces.

*/

/*===========================================================================

  Copyright (c) 2013 - 2016 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/iratman.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/16   vko/pk  Changes for FR-33944 (D2L redirection support)
06/29/15   pk      Added iratman_is_meas_feasible() API
08/01/14   cjb     New API to return LTE freqs that are ready for measurement
05/05/14   pk      Changes for FR-17879 (Extended EARFCN and band range)
05/28/14   cjb     Added new API to return min measurement duration
01/14/14   bph     Set Tbackoff/Treselect defaults same as legacy
12/20/13   bph     Enable FR3839
12/11/13   bph     Improve logging/clean up
12/10/13   bph     Remove dependancy on L1 for cell info
12/06/13   bph     FR3839 - dynamic tbackoff
11/27/13   bph     dynamic backoff API changes
05/16/13   bph     LTE Deprioritization + fix freq count/get next freq logic
03/25/13   bph     Store RAT's node (more or less) preferrence
01/09/13   cjb     Added logging capability.
04/16/12   cjb     Adding support for LTE comparison algorithms(RSRP & RSRQ).
10/28/11   cjb     First working version.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>    /* Definition of basic types and macros */
#include <customer.h>

#include "lte_as.h"
#include "lte_cphy_irat_meas_msg.h"
#include "timetick.h"

/*===========================================================================

                    EXTERNAL DEFINES

===========================================================================*/
#define FEATURE_MCS_IRATMAN_DYNAMIC_BACKOFF

/* Default values for LTE Treselect and Tbackoff timers */
#define IRATMAN_LTE_MAX_TRESELECT_DEFAULT 0xFF
#define IRATMAN_LTE_MIN_TBACKOFF_DEFAULT  0xFF
#define IRATMAN_LTE_MAX_TBACKOFF_DEFAULT  0xFF

/* Maximum number of EUTRA Freqs in ATIM / ORAT message */
#define IRATMAN_LTE_MAX_NUM_EUTRA_FREQ            7

/* Maximum size of LTE frequencies in srch_lte_nset_update_params_type */
#define IRATMAN_LTE_FREQ_ARR_SIZE       ( 2 * IRATMAN_LTE_MAX_NUM_EUTRA_FREQ )

/*===========================================================================

                    EXTERNAL ENUM'S, MACROS, TYPEDEFS

===========================================================================*/

/*! @brief RAT identifier */
typedef enum
{
  RAT_INT   = 0x0,      //!< internal identifier
  RAT_1X    = 0x1,      //!< CDMA 2000 RAT
  RAT_HDR   = 0x2,      //!< HDR/EV-DO/eHRPD RAT
  RAT_LTE   = 0x3,      //!< LTE RAT
  RAT_MAX
} iratman_rat_e_type;

/*! @brief comparison criteria, cell should pass,
    to declare mesurement as successfull */
typedef enum
{
  IRATNMAN_ANY_RSRP_RSRQ  = 0x0,
  IRATMAN_ONLY_RSRP       = 0x1,
  IRATMAN_ONLY_RSRQ       = 0x2,
  IRATMAN_BOTH_RSRP_RSRQ  = 0x3,
  IRATNMAN_NONE_RSRP_RSRQ = 0xFF
} iratman_comp_algo_e_type;
/*===========================================================================

                    EXTERNAL STRUCTURES

===========================================================================*/

/*! ----------------------------------- */
/*! LTE: Interface enum & structures    */
/*! ----------------------------------- */
/*! @brief Valid LTE lists */
typedef enum
{
  IRATMAN_LTE_FREE_LIST,  //!< LTE: Free list[node pool]
  IRATMAN_LTE_MP_LIST,    //!< LTE: More preferred list
  IRATMAN_LTE_LP_LIST,    //!< LTE: Less preferred list
  IRATMAN_LTE_NUM_LIST    //!< LTE: Number of lists
} iratman_lte_list_e_type;


/*! @brief LTE measurement comparison alogorithms supported */
typedef enum
{
  IRATMAN_LTE_ALGO_RSRP,    //!< RSRP based comparison
  IRATMAN_LTE_ALGO_RSRQ,    //!< RSRQ based comparison
  IRATMAN_LTE_ALGO_RSRP_RSRQ, //!Both RSRP/RSRQ based comparison . 
  IRATMAN_LTE_ALGO_SIZE     //!< Number of comparison methods
} iratman_lte_algo_e_type;

/*! @brief LTE input node type */
typedef struct
{
  /*! Evolved Absolute Radio Frequency Channel Number (LTE freq number) */
  lte_earfcn_t                    earfcn;

  /*! Priority of the EARFCN of the neighboring EUTRA network */
  int8                            earfcn_priority;

  /*! LTE measuring bandwidth */
  lte_bandwidth_e                 bandwidth;

  /*! Threshold of serving(home_rat) frequency */
  uint8                           thresh_serving;

  /*! Reselection timer duration (in ms) */
  timetick_type                   treselect_dur_ms;
  /*! Max reselection timer */
  uint8                           max_treselect;

  /*! Min backoff timer */
  uint8                           min_tbackoff;

  /*! Backoff timer duration (in ms) */
  timetick_type                   tbackoff_dur_ms;
  /*! Max backoff timer */
  uint8                           max_tbackoff;

  /*! Measurement comparison algorithm to be used for evaluation */
  iratman_lte_algo_e_type         meas_comp_algo;

  /*! Minimum received RSRP level from the EUTRA cell */
  uint8                           rx_lev_min_eutra;

  /*! Offset to Minimum reference power level to select EUTRA */
  uint8                           rx_lev_min_eutra_offset;

  /*! Maximum TX power in dBm */
  uint8                           pe_max;

  /*! Minimum required signal threshold used for selecting an EARFCN */
  uint8                           thresh_x;

  /*! Minimum received RSRQ level from the EUTRA cell */
  uint8                           rsrq_min_eutra;

  /*! Offset to minimum RSRQ level to select EUTRA */
  uint8                           rsrq_min_eutra_offset;

  /*! Minimum required signal quality threshold used for selecting an EARFCN */
  uint8                           rsrq_thresh_x;

} iratman_lte_input_node_type;


/*! @brief LTE measurement node list type */
typedef struct
{
  /*! Number of LTE freqs in the list */
  uint8                           count;

  /*! Array containing LTE freqs ready for measurement */
  struct
  {
    /*! Evolved Absolute Radio Frequency Channel Number (LTE freq number) */
    lte_earfcn_t                    earfcn;

    /*! LTE measuring bandwidth */
    lte_bandwidth_e                 bandwidth;

  } freq_arr[ IRATMAN_LTE_FREQ_ARR_SIZE ];

} iratman_lte_meas_node_list_type;


/*! @brief LTE measurement node type */
typedef struct
{
  /*! Meas node data is valid */
  boolean                         valid;

  /*! Evolved Absolute Radio Frequency Channel Number (LTE freq number) */
  lte_earfcn_t                    earfcn;

  /*! LTE measuring bandwidth */
  lte_bandwidth_e                 bandwidth;

} iratman_lte_meas_node_type;


/*! @brief LTE measurement report type */
typedef struct
{
  /*! Home RAT signal strength in -0.5 dB unit */
  uint8                           home_rat_ecio;
} iratman_lte_meas_req_type;


/*! @brief LTE measurement report type */
typedef struct
{
  /*! Mobile maximum TX power as reported by LTE in dBm */
  int16                           lte_max_tx_power;

  /*! comparison criteria */
  iratman_comp_algo_e_type        comp_algo;
  
  /*! Copy of LTE Meas Confirmation parameters (rcvd from LTE) */
  lte_cphy_irat_meas_meas_cnf_s*  lte_meas_cnf_params_ptr;

} iratman_lte_meas_rpt_type;


/*! @brief LTE reselection information type */
typedef struct
{
  /*! Reselection data is valid */
  boolean               valid;

  /*! Measurement of the LTE cell passed the threshold */
  boolean               passed_meas;

  /*! Frequency of the LTE cell to be selected */
  lte_earfcn_t          earfcn;

  /*! Cell id of the LTE cell to be selected */
  uint16                cell_id;

  /*! Calculated S_EUTRA value for the reselection cell */
  int16                 s_eutra_val;
} iratman_lte_reselect_type;


/*! @brief 1x Cell Info type */
typedef struct
{
  uint16       sid;
  uint16       nid;
  uint16       base_id;
} iratman_onex_cell_info_type;

/*! @brief HDR Cell Info type */
#define IRATMAN_HDRADDR_LENGTH_IN_BYTES 16
typedef unsigned char iratman_hdr_cell_info_type[IRATMAN_HDRADDR_LENGTH_IN_BYTES];

/*! @brief Union of Cell Info type */
typedef union
{
  iratman_onex_cell_info_type onex_cell_info;
  iratman_hdr_cell_info_type  hdr_cell_info;
} iratman_cell_info_type;

/*! Wakeup callback type*/
typedef boolean (*wakeup_cb_type)( void );

/*! Get Cell Info callback type*/
typedef boolean (*get_cell_info_cb_type)( iratman_cell_info_type *cell_info );

/*! IRATMan Client Call Back Structure */
typedef struct
{
  wakeup_cb_type        wakeup_cb;
  get_cell_info_cb_type get_cell_info_cb;
} iratman_cb_struct;

/*! ----------------------------------- */
/*! Meas RAT unions                     */
/*! ----------------------------------- */
/*! Union of meas RAT lists */
typedef union
{
  /*! ENUM vlaue for ease of comparison */
  uint8                         value;

  iratman_lte_list_e_type       lte;
} iratman_rat_list_e_type;


/*! Meas RAT input node union */
typedef union
{
  iratman_lte_input_node_type   lte;
} iratman_input_node_type;


/*! Meas RAT measurement node list union */
typedef union
{
  /*! Number of valid meas nodes in the list */
  uint8                              count;

  iratman_lte_meas_node_list_type    lte;
} iratman_meas_node_list_type;


/*! Meas RAT measurement node union */
typedef union
{
  /*! Meas node data is valid */
  boolean                       valid;

  iratman_lte_meas_node_type    lte;
} iratman_meas_node_type;


/*! Meas RAT measurement request union */
typedef union
{
  iratman_lte_meas_req_type     lte;
} iratman_meas_req_type;


/*! Meas RAT results report union */
typedef union
{
  iratman_lte_meas_rpt_type     lte;
} iratman_meas_rpt_type;


/*! Meas RAT reselection params union */
typedef union
{
  /*! Reselection data is valid */
  boolean                       valid;

  iratman_lte_reselect_type     lte;
} iratman_meas_rat_reselect_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  iratman_init

===========================================================================*/
/*!
  @brief
    Initialize IRATMan module

  @details
    This function initializes the IRAT List Manager (deprecated and should be
    removed once clients are changed)

  @note
    Initialization should be done first by all RATs using IRATMan

  @return
    void

*/
/*=========================================================================*/
extern void iratman_init( void );


/*===========================================================================

  FUNCTION:  iratman_initialization

===========================================================================*/
/*!
  @brief
    Initialize IRATMan module

  @details
    This function initializes the IRAT List Manager

  @note
    Initialization should be done first by all RATs using IRATMan

  @return
    void

*/
/*=========================================================================*/
extern void iratman_initialization
(
   iratman_rat_e_type home_rat,                   //!< Home/Originating RAT
   iratman_cb_struct  cbs,                        //!< Client call backs
   boolean            is_dynamic_backoff_enabled  //!< True if dynamic backoff
                                                  //!< is enabled, otherwise false
);


/*===========================================================================

  FUNCTION:  iratman_register_wakeup_cb

===========================================================================*/
/*!
  @brief
    Register wakeup callback

  @details
    This function registers a wakeup callback for home_rat. The callback is
    used to notify the RAT about a pending measurement if the "home_rat"
    does not request a measurement within a buffer time after a node is
    ready for measurement.

  @note
    Callback should return a boolean. A return value of TRUE indicates that
    the RAT is active and has accepted to do the measurement.

  @return
    void

*/
/*=========================================================================*/
extern void iratman_register_wakeup_cb
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat, //!< RAT to be measured
  wakeup_cb_type            cb        //!< RAT node to be added
);


/*===========================================================================

  FUNCTION:  iratman_add_meas_node

===========================================================================*/
/*!
  @brief
    Add measurement node

  @details
    This function adds a "node" to the IRAT List Manager's "list" of nodes
    of type "meas_rat"

  @note
    iratman_init() should have been previously called

  @return
    boolean: TRUE if RAT node was successfully added. Else, FALSE.

*/
/*=========================================================================*/
extern boolean iratman_add_meas_node
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat, //!< RAT to be measured
  iratman_input_node_type*  node,     //!< RAT node to be added
  iratman_rat_list_e_type   list      //!< Meas RAT list to be used
);


/*===========================================================================

  FUNCTION:  iratman_meas_pending

===========================================================================*/
/*!
  @brief
    Request from "home_rat" to determine if a measurement is pending/required
    for "meas_rat"

  @details
    This function will scan through the known lists of nodes to determine if an
    IRAT measurement of RAT type "meas_rat" is needed.
    A node will be returned if the following conditions are met:
      - The node has been added by the "home_rat"
      - The node is of RAT "meas_rat"
      - At least one of the node's timers has expired or it's a new node
      - The time it takes to do a measurement on this "meas_rat" is less than
        the "available_time" value passed in
      - Other conditions based on the "req_data" and spec are satisfied

  @note
    iratman_init() should have been previously called

  @return
    uint32: Time required for max number of possible measurements within the
            "available_time" provided [in ms]

*/
/*=========================================================================*/
extern uint32 iratman_meas_pending
(
  iratman_rat_e_type        home_rat,       //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat,       //!< RAT to be measured
  uint32                    available_time, //!< Meas time available [in ms]
  iratman_meas_req_type*    req_data        //!< Data for meas request processing
);


/*===========================================================================

  FUNCTION:  iratman_get_min_meas_dur_ms

===========================================================================*/
/*!
  @brief
    Request from RAT to get minimum duration required for a measurement (in ms)

  @details
    This function will return the minimum duration required to measure one
    frequency belonging to the "meas_rat"

  @note
    iratman_init() should have been previously called

  @return
    uint32: minimum duration required to complete one measurement in ms units

*/
/*=========================================================================*/
extern uint32 iratman_get_min_meas_dur_ms
(
  iratman_rat_e_type        home_rat,       //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat        //!< RAT to be measured
);


/*===========================================================================

  FUNCTION:  iratman_get_meas_list

===========================================================================*/
/*!
  @brief
    Request from HOME_RAT to retrieve MEAS_RAT node list ready for measurement

  @details
    This function will scan through the known lists of nodes to determine if an
    IRAT measurement of RAT type "meas_rat" is needed.
    A node will be added to the list if the following conditions are met:
      - The node has been added by the "home_rat"
      - The node is of RAT "meas_rat"
      - At least one of the node's timers has expired or it's a new node
      - Other conditions based on the "req_data" and spec are satisfied

  @note
    iratman_init() should have been previously called

  @return
    iratman_meas_node_list_type: List of MEAS_RAT nodes ready for measurement

*/
/*=========================================================================*/
extern iratman_meas_node_list_type iratman_get_meas_list
(
  iratman_rat_e_type        home_rat,       //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat,       //!< RAT to be measured
  iratman_meas_req_type*    req_data        //!< Data for meas request processing
);


/*===========================================================================

  FUNCTION:  iratman_get_meas

===========================================================================*/
/*!
  @brief
    Request from HOME_RAT to retrieve MEAS_RAT node to be measured

  @details
    This function will scan through the known lists of nodes to determine if an
    IRAT measurement of RAT type "meas_rat" is needed.
    A node will be returned if the following conditions are met:
      - The node has been added by the "home_rat"
      - The node is of RAT "meas_rat"
      - At least one of the node's timers has expired or it's a new node
      - The time it takes to do a measurement on this "meas_rat" is less than
        the "meas_dur" value passed in
      - Other conditions based on the "req_data" and spec are satisfied

  @note
    iratman_init() should have been previously called

    If a valid node is returned then that node will be locked for the home_rat
    and it's home_rat's responsibility to report measurement results thereby
    unlocking the node for further measurements.

  @return
    iratman_meas_node_type: RAT node to be measured

*/
/*=========================================================================*/
extern iratman_meas_node_type iratman_get_meas
(
  iratman_rat_e_type        home_rat,       //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat,       //!< RAT to be measured
  uint32                    available_time, //!< Meas time available [in ms]
  iratman_meas_req_type*    req_data        //!< Data for meas request processing
);


/*===========================================================================

  FUNCTION:  iratman_report_meas

===========================================================================*/
/*!
  @brief
    "home_rat" reports the measurement results for the last IRAT measurement

  @details
    This function takes the measurement report from the "home_rat" and inserts
    the information into the IRAT Lists. The node is then compared against
    thresholds using comparison criteria. Timers/timeouts are then recalculated.
    A structure containing information related to reselection is returned if the
    new measurement meets the comparison criteria.

  @note
    iratman_init() should have been previously called.
    In case "home_rat" is unable to complete the meas or if meas fails,
    use iratman_abort_meas() to report the failure back to IRATMan.

  @return
    iratman_meas_rat_reselect_type:
      RAT reselection structure containing relevant information.
      The "home_rat" should initiate the reselection using this info.

*/
/*=========================================================================*/
extern iratman_meas_rat_reselect_type iratman_report_meas
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat, //!< RAT to be measured
  iratman_meas_node_type*   node,     //!< RAT node that was being measured
  iratman_meas_rpt_type*    report      //!< Measurement report
);

/*===========================================================================

  FUNCTION:  iratman_abort_meas

===========================================================================*/
/*!
  @brief
    Aborts an outstanding IRAT measurement

  @details
    This function informs IRATMan that measurements on a recently queried node
    has been aborted. Timers/timeouts will remain as is (which should be in an
    expired state). Then a check is done to see whether there is a node which
    has qualified for reselection.

  @note
    iratman_init() should have been previously called

  @return
    iratman_meas_rat_reselect_type:
      RAT reselection structure containing relevant information.
      The "home_rat" should initiate the reselection using this info.

*/
/*=========================================================================*/
extern iratman_meas_rat_reselect_type iratman_abort_meas
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat, //!< RAT to be measured
  iratman_meas_node_type*   node      //!< RAT node that was being measured
);


/*===========================================================================

  FUNCTION:  iratman_del_node

===========================================================================*/
/*!
  @brief
    Deletes a previously added node

  @details
    This function will delete a specific node from IRAT Lists. If the node
    is added by more than one "home_rat"s, then the node will not be deleted,
    instead the mask for the "home_rat" will be removed and the "home_rat"
    specific is cleared from node data structure.

  @note
    iratman_init() should have been previously called

  @return
    boolean: TRUE if the node is successfully deleted

*/
/*=========================================================================*/
extern boolean iratman_del_node
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat, //!< RAT to be measured
  iratman_meas_node_type*   node      //!< RAT node that was being measured
);


/*===========================================================================

  FUNCTION:  iratman_trim_nodes

===========================================================================*/
/*!
  @brief
    Trim/delete future nodes of type "meas_rat" and known to "home_rat"

  @details
    The decision to remove or retain nodes is specific to "meas_rat".
    This function deletes future nodes of type "meas_rat" and known to
    "home_rat". The "buffer_time" is used as a minimum buffer allowed
    for nodes to try to reselect even after receiving an updated list.
    The function goes ahead and returns whether there are nodes known to
    "home_rat" that require measurement after deleting the future nodes.

  @note
    iratman_init() should have been previously called
    Inputs are assumed to be pre-validated

  @return
    boolean: TRUE if there are still nodes which require measurement.

*/
/*=========================================================================*/
extern boolean iratman_trim_nodes
(
  iratman_rat_e_type    home_rat,     //!< Home/Originating RAT
  iratman_rat_e_type    meas_rat,     //!< RAT to have timers cleared
  uint32                buffer_time   //!< Buffer time for reselection [in ms]
);


/*===========================================================================

  FUNCTION:  iratman_del_all_nodes

===========================================================================*/
/*!
  @brief
    Deletes all previously added nodes of type "meas_rat"

  @details
    This function will delete all nodes from IRAT Lists matching type
    "meas_rat" and known to "home_rat". If the node is added by more than one
    RAT, then node will not be deleted, instead the mask for the "home_rat"
    will be removed.

  @note
    iratman_init() should have been previously called

  @return
    boolean: TRUE if all nodes related to "home_rat" were deleted successfully

*/
/*=========================================================================*/
extern boolean iratman_del_all_nodes
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat  //!< RAT of which all nodes to be deleted
);


/*===========================================================================

  FUNCTION:  iratman_flush_persistent_data

===========================================================================*/
/*!
  @brief
    Flush IRATMan module data to file

  @details
    This function flushes the IRAT List Manager persistent data to a file

  @note
    Flushing should be done by all RATs using IRATMan before powering
    down.

  @return
    void

*/
/*=========================================================================*/
extern void iratman_flush_persistent_data
(
   iratman_rat_e_type home_rat //!< Home/Originating RAT
);

/*===========================================================================
  FUNCTION:  iratman_is_meas_feasible
===========================================================================*/
/*!
  @brief
    Checks for restrictions for IRAT MEAS from home_rat to meas_rat

  @details


  @note
    

  @return
    boolean: TRUE if it passes all restrictions for home_rat->meas_rat IRAT
*/
/*=========================================================================*/
extern boolean iratman_is_meas_feasible
(
  iratman_rat_e_type        home_rat, //!< Home/Originating RAT
  iratman_rat_e_type        meas_rat  //!< RAT for which meas is to be done
);
#endif /* IRATMAN_H */

