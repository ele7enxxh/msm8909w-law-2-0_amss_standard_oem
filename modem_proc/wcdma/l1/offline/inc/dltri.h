#ifndef DLTRI_H
#define DLTRI_H

/*===========================================================================
                           D L _ T R I . H

GENERAL DESCRIPTION
  This file contains variables and functions to support triage.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dltri.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
04/14/09    ks      Removed redundant prototypes
12/22/08    ks      Mainlining FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
01/25/07    vb      featurized to bring in support for MCAL/dl enhanced
10/30/07    mmr     Added prototype for function to clear the finger count db.
09/06/07    gnk     Removed temporary Tx sys time debugging through QXDM
08/08/07    gnk     Enabled Tx sys time debugging through QXDM  
05/14/07    mmr     Added support for Dynamic fing granularity under 
                    FEATURE_WCDMA_DYNAMIC_FING_GRAN. 
04/12/07    bbd     Corrected naming convention of deassign fingers per RL.
03/16/07    bbd     Removed unnecessary dl_tri_get_ref_fing_info() interface.
03/09/07    kps     Support for CGPS Aries interface change.
03/05/07    bbd     Corrected R99 table declaration.
01/31/07    bbd     Added feature checks for function table struct definition.
12/21/06    bbd     Re-organized per RL triage interfaces.
12/07/06    bbd     Added finger ena bitmask.
08/30/06    rgn     Added log code for wcdma rxd ant stats log packet
08/14/06    ub      Added support for HSUPA.
07/18/06    bbd     Added prototype for function to remove path from path db.
07/06/06    bbd     Added function to return strongest finger of a pair.
06/22/06    bbd     Added macro for finger TPC index.
                    Modified the RL sets structure.
05/31/06    bbd     Modified get weakest finger routine to use mode parameter.
05/30/06    ms      Removed dl_tri_serv_cell_ref_finger_change_handler
                    and dl_tri_nghbr_cell_ref_finger_change_handler from under 
                    FEATURE_PER_PSC_TRIAGE
05/18/06    ms      In dl_tri_per_rl_triage_info_struct_type, removed callback
                    function information and put in assoc_services, that tells
                    which services are using the specified RL. Defined ref finger
                    change handlers for serving cell and neighboring cell.
04/06/06    mc      Corrected compilation warning
03/20/06    bbd     Support for paired finger assignment and RxD.
03/16/06    mc      Moved dl_eq_frame_window_update to l1eqctrl.c. Renamed
                    the EQ callback functions.
03/16/06    mc      Added support for Equalizer under the feature flag
                    FEATURE_WCDMA_EQ_CONTROLLER
03/03/06    cpe     Added a prototype for dl_tri_find_strongest_fing
11/08/05    gv      Added function prototype for dl_tri_set_fing_to_strongest_path(),
                    defined in dltri.c for BPLMN feature.
11/01/05    bbd     Added dl_tri_pch_init().
10/14/05    bbd     Update for new dl_tri_select_nghbr_ref_fing() API.
10/11/05    bbd     Added support for finger manager sub-module.
09/20/05    bbd     Added support for cell path biasing.
09/19/05    cpe     Mainlined FEATURE_WCDMA_RX_TX feature
09/01/05    gv      Modified the function prototype for 
                    dl_ngbhr_fing_status_modify().
08/19/05    mc      Exported neighbor path database structure and variable 
                    storing number of neighbor paths
08/12/05    gv      Added prototype for  dl_ngbhr_fing_status_modify_cell_trans(). 
07/28/05    bbd     Changed return type for dl_tri_fing_assigned_to_strongest_path()
                    from uint16 to uint8.
07/20/05    scm     Support freq err running average for GPS.
04/28/05    bbd     Added prototype for dl_tri_get_weakest_finger().
04/08/05    bbd     Corrected feature conflict. Use FEATURE_WCDMA_RX_TX instead of
                    FEATURE_RX_TX_TIME_DIFFERENCE.
02/26/05    gs      Updated definition of function
                    DL_TRI_REF_FING_CHANGE_UPDATE_CB_FUNC_TYPE to add old
                    ref finger as new argument.
02/18/05    bbd     Added prototype for dl_tri_select_nghbr_ref_fing()
12/01/04    cpe     Added prototype for dltri_deassign_fing_for_rl()
10/11/04    gs      Added declaration for per PSC triage feature
07/27/04    bbd     Added virtual_path to dl_tri_path_struct_type.
07/21/04    bbd     Relocated MAX_DELAY_TRIAGE_CYCLES to dltriparm.h
07/21/04    bbd     Added dl_tri_find_earliest_finger() to support
                    FEATURE_RX_TX_TIME_DIFFERENCE
06/28/04    src     Added declaration for the dl_tri_disable_all_nghbr_fingers
                    function.
05/17/04    bbd     Added "triage watchdog" to declare error condition if triage
                    is delayed longer than MAX_DELAY_TRIAGE_CYCLES
05/17/04    yus     Add prototype of dl_nghbr_triage().
04/05/04    yus     Add prototype of dl_tri_eng_sort_comp_fn()
                    and dl_tri_get_idle_finger().
04/23/03    yus     Replace function dl_tri_find_assigned_fing with
                    dl_tri_fing_assigned_to_strongest_path()
04/23/03    yus     Add function dl_tri_save_fing_infor(void) and 
                    dl_tri_restore_fingers(void). They are used when MDSP clk
                    changes.
04/18/03    yus     Add function dl_tri_find_assigned_fing(void). It's mainly
                    for sleep re-acq.
02/04/03    yus     Add support for new TPC threshold algorithm.
                    It's defined under FEATURE_SHO_TPC_THRESH.
01/23/03    yus     Lint clean up. 
12/10/02    scm     Add extern to dl_tri_rl_db_init() for compiler warning.
11/14/02    asr     Declared function dl_triage_without_status_dump()
08/12/02    yus     Merged with FEATURE_FINGER_REUSE code. A function called
                    dl_tri_get_idle_finger_for_nbch(.) is created. It will
                    returns a finger which can be used to decode a neighbor
                    BCH.
06/12/02    yus     Changed eng of dl_tri_path_struct_type to uint32.
06/06/02    src     Added extern declaration corresponding to the new triage
                    function dl_tri_get_fing_for_phychan_setup ().
05/19/02    src     Merged-in changes associated with the new method of OFF/Tm
                    computation and the accompanying re-arrangement of data
                    declarations and definitions.
05/18/02    ma      Added dl_tri_get_weakest_finger_for_nbch_decode() for
                    cell reselection.
05/08/02    src     Merged-in the total of changes and fixes for the following.
                    1. Temporary OFF computation problem. This fix is still on
                       the old method of SFN-CFN difference computation, which
                       relies on a prirori knowledge of the reference finger.
                       The change includes a major change in the signature of
                       the function dl_tri_get_ref_fing_info (.), which now
                       takes a scrambling code as its only argument. Function
                       dl_tri_set_ref_fing_info (.) is deleted.
                    Note that the above changes will again undergo overhaul,
                    if not elimination, when the re-design of the method to
                    compute OFF/Tm is implemented.
03/13/2002  src     Merged-in fix for D-OFF computation during soft hand-over.
                    Too many code changes and re-organisations to individually
                    list. In general terms, the changes disambiguate Tau-DPCH
                    and CFN based on the exact unnormalised time-lines. Method
                    dl_tri_wrapup(.) has been seriously modified. New methods
                    dl_tri_get_ref_fing_info() and dl_tri_set_ref_fing_info(.)
                    have been added to cleanly retrieve and store information
                    about the reference finger, as needed in disambiguating a
                    neighbour-cell DPCH offset.
02/22/2002  src     Added some extern declarations after shifting them out of
                    the C source file dldem.c
01/28/2002  yus     Added a macro for position difference calculation. 
11/02/2001  cpe     Added dltriparm.h to the included header files
10/18/2001  cpe     Added min_distance and max_distance fields to
                    dl_tri_parms_struct_type structure
04/27/2001  mdl     added use of radio link index into path structure and
                    various triage routines.  Added dl_tri_ta_log().

03/08/2001  mdl     externed dl_max_aset_fingers and dl_num_aset_fingers

01/24/2001  mdl     removed externs of demod_nghbr_cell_ref_finger and
                    demod_serv_cell_ref_finger to demoddrv.h

01/17/2001  mdl     added soft_lock_thresh to dl_tri_parms_struct_type
                    externed dl_tri_parms
                    added macro RL_SET_PTR_TO_IDX() to generate the
                    index into the radio link set array from the address
                    of the given entry
                    added macro PATH_PTR_TO_IDX to do the same for a path
                    array entry address
                    added prototypes for a dl_tri_path_init(),
                    dl_tri_rl_set_init(), dl_triage_parms_init(), and
                    dl_tri_init()

12/21/2000  mdl     changed return type of dl_triage from uint8 to void
10/30/2000  mdl     first cut

===========================================================================*/


/*--------------------------------------------------------------------------
                      Included Header Files
--------------------------------------------------------------------------*/
#include "wl1tri.h"

#endif


