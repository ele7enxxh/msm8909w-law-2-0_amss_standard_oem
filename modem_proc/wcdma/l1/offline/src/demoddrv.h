#ifndef DEMODDRV_H
#define DEMODDRV_H

/*===========================================================================
                              D E M O D D R V . H


DESCRIPTION

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2000 - 2005  by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/demoddrv.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
04/06/10    ks      Added Enhanced FDPCH support
09/02/09    mg      Remove feature FEATURE_WCDMA_CALC_CFN_USING_STMR_STATUS_DUMP
12/17/08    ks      Mainline FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT 
12/08/08    rmak    Mainline FEATURE_PARALLEL_QXT_NCP
10/21/08    scm     Fix MDSP_READ_COMBINED_FREQ_EST() definition.
08/08/08    ks      Redirecting to wl1demodrv.h under new dl feature
08/04/08    scm     Adjust PICH macros for FEATURE_WFW_SW_INTERFACE.
07/31/08    oh      EUL changes for Taxis 
07/17/08    mg      Support for Cx32 TTL
01/25/07    vb      featurized to bring in support for MCAL/dl enhanced
08/07/07    vsr     Moved in finger ECIO related functions from dldem.c
07/24/07    nd      Added S-CPICH params to the assign finger struct
05/11/07    mmr     Mainlined FEATURE_WCDMA_L1_TA_INFO_LOG_CODE_4006.
03/23/07    bbd     Added support for APF and DBS.
03/21/07    bbd     Removed srch timing adjust feature.
01/31/07    vr      Added demod_stmr_status_dump() to avoid calling
                    demod_status_dump() in ISR context.
01/30/07    bbd     Changed FING_ADV_RET_NUM_CX8_VAL to be half a chip.
10/17/06    ms      Added support for FDPCH.
10/06/06    bbd     Added demod init routine.
09/19/06    bbd     Added support for MMSE.
08/28/06    vp      Added new member nbr_rl_idx in figer_status_struct
08/14/06    ub      Added support for HSUPA.
07/12/06    mc      Changed the ref fing bitmask to 8 bits.
06/22/06    bbd     Added bitmask for f?Common.
05/30/06    bbd     Updated RSSI to searcher energy conversion.
05/18/06    mc      Added support for HS+SSC. Removed HS enable bit field
                    from finger common config.
03/20/06    bbd     Added support for paired finger assignment and RxD changes.
12/16/05    bbd     Modified finger assignment to use new QXT interface.
11/07/05    bbd     Changed demod_assign_finger() to use parameter struct.
09/27/05    bbd     Mainlined HW FMP feature and removed unused finger dbase
                    elements associated with SW/FW FMP.
08/19/05    mc      Added a field in the finger status database to remember 
                    the hardware channel it is associated with. 
07/12/05    vp      Added functions demod_nbr_fing_asn_imdiate and
                    demod_xfer_nbr_fing_parms_imdiate to send the finger
                    assignment command for N-PCCPCH immediately without
                    awaiting next CM finger update event
07/07/05    bbd     Added definition for workaround DCH state TTL beta.
05/31/05    gv      Changes to remove Lint errors
05/12/05    bbd     Under FEATURE_FTL_INIT_ON_FIRST_FINGER_ONLY, changed initial
                    finger RSSI to 73 and added first_finger field to finger
                    database.  This indicates that the finger is the first
                    finger assigned to its respective radio link.
04/04/05    bbd     More modification of TTL_BETA. Now set to 1 in DCH state.
03/29/05    bbd     Added fing_adv_ret_enum_type and advance/retard values
                    for finger nudging.
03/28/05    bbd     Clean up the TTL Beta code.
03/25/05    bbd     When FEATURE_SRCH_GRANULARITY_WORKAROUND defined, set non-acq
                    TTL_BETA value to 2 instead of 3.
02/26/05    gs      Added HS related structure members hs_enabled, hs_ref_fing
                    hs_cell_idx to finger status structure.
                    Added macros to access and set the value of these elements
                    Externalized functions demod_drv_update_fing_hs_cell_idx
                    and demod_drv_update_hs_cell_idx
02/03/05    bbd     Mainlined FEATURE_FING_PERF_ENHANCE.
12/30/04    gs      Updated prototype definition of functions
                    demod_disable_fing_hs and demod_enable_fing_hs
12/27/04    src     Increased polling retrials to 6 for status-dump command and
                    reduced to 133 the upfront microseconds needed there. Also,
                    increased polling retrials to 6 for finger-config command.
11/11/04    vb      Code update to support to set use best pilot in CLTD case
                    under FEATURE_USE_BEST_PILOT
10/22/04    bbd     Modified demod_set_fing_ttl_beta() to accept a TTL Beta
                    value as a parm.
08/30/04    gs      Declared HS enable/disable related functions
                    Added offset, bit mask for HS enable and HS cell index
08/26/04    src     Merged in the latest CLTD changes with featurisation under
                    the feature flag FEATURE_TRANSMIT_DIVERSITY_SUPPORT.
06/01/04    scm     Add extern to demod_is_at_least_one_finger_in_lock().
04/19/04    bbd     Added support to set TTL_BETA values for HW CPP under
                    FEATURE_SET_FING_TTL_BETA.
04/13/04    scm     Add extern to demod_is_at_least_one_finger_assigned().
09/23/03    src     Changed several finger configuration functions to return a
                    boolean (TRUE/FALSE);
09/16/03    cpe     Added a macro to initialize the finger init bit
08/03/03    gs      Externalized following functions
                    - demod_xfer_fing_cfg_shadow_cm
                    - demod_fing_cfg_update_in_cm
                    - demod_fing_cfg_update_cm_cleanup
07/14/03    src     Created a new fing_cfg_cmd_status_enum_type enumerant type
                    and the extern declaration for demod_fing_cfg_cmd_status to
                    keep track of the progress of finger config command.
07/02/03    sh      Removed demod_serv_cell_last_known_pos.
06/17/03    src     Corrected bit-mask FINGPARM_PHYS_TTOFFSET_BMSK (to 0xFF).
06/05/03    scm     Remove unused demod_use_tcxo_freqhypo. Add () to 2 macros.
06/02/03    yus     Define macros SET_MAX_NUM_FINGERS(void) and
                    SET_NUM_FINGS_FOR_LOW_MDSP_CLK(void).
05/20/03    src     Added hash-defined symbols to support giving more time for
                    the mDSP to finish the status dump and finger config.
04/30/03    src     Added a new hash-symbol DEMOD_FINGER_CONF_MICROSECS_NEEDED
                    for the expected number of microseconds that a finger con-
                    figuration command may require in the worst case scenario.
04/28/03    src     Merged-in changes again corresponding to physical channel
                    configuration in the event context. Also, made sure that
                    there are a "..._wait" function and a "..._pause" function
                    for both finger-config and status-dump commands.
04/23/03    yus     Added support for FEATURE_FING_PERF_ENHANCE.
04/15/03    src     Added declaration for demod_send_finger_cfg_cmd_wait (at
                    least until it is removed later due to non-use).
04/14/03    src     Merged-in changes and code re-organisation from the single-
                    mode branch corresponding to elimination of inline L1 wait,
                    while keeping the old interfaces as operationally similar
                    as possible.
02/04/03    yus     Add the support for new TPC threshold algorithm.
                    It's defined under FEATURE_SHO_TPC_THRESH.
11/05/02    cpe     Updated the copyright for the file
                    Corrected the value of FINGPARM_PHYS_SSCRCODE_SHFT from 12
                    to 9 as per the finger physical channel layout
10/14/02    scm     Extern demod_use_tcxo_freqhypo for SUSPEND/RESUME.
09/12/02    src     Added FEATURE_FINGER_REUSE as an additional condition to
                    check for when hash-defining IS_FING_ASSIGNED_NBCH() macro.
07/24/02    gs      Added bit field, mask values for finger phch fields
                    for alternate scrambling code change.
07/19/02    yus     Added support for finger merge protection
06/19/02    yus     Changed INITIAL_FINGER_RSSI to 240.
06/12/2002  yus     Changed RSSI_TO_ENG to deal with 32 bits srch eng value.
05/19/2002  src     Added hash-macros to compute status dump parameters such as
                    Tx system time and to read pre-adjusted reference count.
05/18/2002  ma      Added demod_serv_cell_last_known_pos and IS_FING_ASSIGNED_NBCH()
                    for cell reselection support.
05/17/2002  yus     Change RSSI_TO_ENG formula to match the rssi bitwidth.
05/14/2002  src     Corrected a pair of hash-defined macros that had syntactic
                    errors.
05/08/2002  src     Added a new macro to read the newly assigned PN position
                    of a finger. The macro name is PN_ASSG_POS() and takes the
                    finger as an argument to index into the finger database.
01/28/2002  yus     Add SET_TT_DISABLE() macros.
11/05/2001  cpe     Corrected the MACRO definitions for  REF_FING_TTOFFSET_BMSK
                    and REF_FING_TTOFFSET_SHFT as per the change in mdsp interface
11/02/2001  cpe     Added dltriparm.h to the includes
09/19/2001  asr     removed obsolete pich-related mdsp macros
09/12/2001  mdl     fixed pich-related mdsp macros
08/24/2001  mdl     removed MDSP_WRITE_AICH_THRESH() and added
                    MDSP_WRITE_AICH_THRESHOLDS() which writes both the ACK
                    and NAK detection thresholds
07/18/2001  mdl     added MDSP_WRITE_AICH_THRESH() macro
06/13/2001  mdl     modified prototype of demod_assign_finger, added
                    FING_ROTATOR macro.
06/11/2001  mdl     added a few scrambling code conversion macros
03/19/2001  mdl     changed MSM5200*.h to msm5200*.h in #include statements
02/21/2001  mdl     Removed prototypes for demod_clr_update_bits() and
                    demod_xfer_fing_cfg_shadow().
                    Added prototype for demod_send_fing_cfg_cmd().

01/31/2001  mdl     Added macros FING_CLR_ASSIGN_UPDATE_BIT and
                    FING_CLR_PHYCHAN_CFG_UPDATE_BIT to clear the update bits
                    in the finger assignment and finger phychan config
                    portions of the finger config record shadow.

                    added prototype for demod_clr_update_bits()

01/24/2001  mdl     added externs for reference finger index variables
                    demod_nghbr_cell_ref_finger and
                    demod_serv_cell_ref_finger

                    added prototype for demod_disable_all_fingers()

01/17/2001  mdl     changed FING_SOFT_LOCK_RSSI_THRESH from a hard constant
                    to refer to the variable dl_tri_parms.soft_lock_thresh

                    added path_idx to the fing_status_struct_type which is
                    the array index of the path that the finger is assigned
                    to

                    added prototype for demod_disable_finger()

01/12/2001  mdl     changed finger lock bitmask bit definitions to use the
                    definitions in msm5200mdsp.h definitions directly

                    added FING_STATE macro, change IS_IN_LOCK to
                    IS_IN_COMB_LOCK, changed COMB_RSSI to FING_COMB_RSSI

                    added SET_PILOT_SYM_FILT_GAIN macro to write the pilot
                    symbol filter gain parameter to the mDSP

01/11/2001  mdl     added lost_status_dump field to demod_status_dump_info
                    structure.

                    changed return type of demod_status_dump from void to
                    boolean in the prototype

                    added some comments

01/09/2001  mdl     added macros SET_FING_STATE, SET_FING_NEW_ASSIGN,
                    SET_FING_NEW_CELL

12/19/2000  mdl     added field assg_pos in finger status database to indicate
                    the position that a finger is newly assigned to.  Added
                    FING_SOFT_LOCK_RSSI_THRESH

12/14/2000  mdl     included dltri.h for path structure definition
                    included srchset.h for cell struture definition
                    added bitmask contants for interpreting finger lock status
                    added cell and path struct pointers to finger status
                    records
                    added boolean in_soft_lock to finger status record
                    added a bunch of utility macros for accessing finger
                    status record parameters
                    changed phch_deskew to comb_cnt in finger status record
10/19/2000  mdl     added demod_ to the fing_status_dbase, status_dump_info
                    and fing_cfg_shadow variables
8/19/2000   mdl     first cut
===========================================================================*/

#include "wl1demoddrv.h"
#endif  /* #ifndef DEMODDRV_H        */
