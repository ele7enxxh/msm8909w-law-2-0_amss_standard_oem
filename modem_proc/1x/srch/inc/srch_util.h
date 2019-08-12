#ifndef SRCH_UTIL_H
#define SRCH_UTIL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E A R C H E R    U T I L I T Y    H E A D E R   F I L E



GENERAL DESCRIPTION
   This file is part of the breakup of the former file srchdrv.c. This file
   consists of all functions that don't fit into any of the other new
   searcher files (srch_hw.c, srch_8xhw.c, srch_16xhw.c, srch_Xxhw.c,
   srch_fing.c, srch_comb.c, srch_util.c, srch_PN.c).

  Copyright (c) 2001 - 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srch_util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/14   pap     Adding diag command to refresh NV items.
07/30/13   fg      Added declaration of function srch_sig_mapper
09/02/11   sk      Rebased the code to the NikeL modem package by mainlining
                   FEATURE_MODEM_1X_NIKEL.
02/04/11   adw     Added func to initialize TXC registers.
02/02/11   vks     Remove obsolte api: srch_util_slam_time_to_hdr().
12/09/10   mca     Created srch_util_calc_thresh()
12/09/10   mca     Added support to save individual div srch energies
                   Added srch_util_calc_lecio_thresh().
11/01/10   jtm     Modem 1x SU API changes.
12/03/09   vks     Moved srch_sect_parm_type and srch_sect_result_type from
                   srch_hw_t.h to srch_srch4.h. Include srch_srch4.h
11/09/09   bb      Define the srch_util_gen_epoch() method
10/02/09   sst     Added SVDO support
04/01/09   adw     Categorized included header files and standardized format.
03/30/09   adw     Eliminate implicit includes.
03/25/09   adw     Added comdef.h include.
02/23/07   sst     Create position comparison macros
12/15/06   tjc     Modified traffic to use the sector module
09/20/06   pa      Export srch_util_rf_reset().
06/08/06   awj     Remove max_inx and max_eng from sector structures
05/31/06   awj     Further sect and srch4 updates
09/23/05   ejv     Delete srch_util_config_msm.
10/28/04   kwo     Exported srch_util_win_thresh_lecio()
09/03/04   kwo     Added srch_util_calc_lecio()
07/26/04   ejv     Remove SRCH_MSM_NO_VERSION.
07/23/04   ejv     Remove srch_cdma_search_active and srch_util_gen_epoch.
01/28/04   ejv     Mainline T_MSM6500.
01/16/04   ejv     Remove FEATURE_SRCH_SCHEDULER.
07/30/02   gs      Initial merge into perforce for cougar.
05/30/02   ejv     Mainlined T_MSM5000.
02/27/02   ejv     Modified date format in change history comments.
01/14/02   SST     Merged into tip to replace srchdrv.c
01/11/02   SST     Completed function name changes.
                        srch_config_msm -> srch_util_config_msm
                        srch_dmod_reset -> srch_util_dmod_reset
                        srch_get_dmod_id -> srch_util_get_dmod_id
                        srch_bypass_interface -> srch_util_bypass_interface
                        srch_demod_enable_bypass -> srch_util_dmod_ena_bypass
                        srch_demod_disable_bypass -> srch_util_dmod_dis_bypass
                        srch_generate_epoch -> srch_util_gen_epoch
                        srch_hw_err_recovery -> srch_util_hw_err_recovery
01/10/02   SST     finished move for srchdrv.c breakup
01/01/02   SST     Created Module - refer to srch_hw.c for earlier edit
                      history.

==========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"

/* Srch */
#include "srch_hw.h"
#include "srch_hw_t.h"
#include "srch_hw_t_i.h"
#include "srch_srch4.h"
#include "srchi.h"
#include "srchi_i.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
  Macros
---------------------------------------------------------------------------*/

/* macro to check if a postion (pos) is within a give range defined by
   the min and max of the range (all values shall be in ChipX8) */
#define POS_IN_RANGE( pos, min, max )                                         \
  ( ( ( ( (pos) - (min) ) & PN_CIRCLE_MASK ) <=                               \
      ( ( (max) - (min) ) & PN_CIRCLE_MASK )    ) ? TRUE : FALSE )

/* macro to compare positions "a" and "b".
   note: a<b if "a" is between "b - half the pn circle" and "b"
         a>b if "a" is between "b" and "b + half the pn circle"
   (all values shall be in ChipX8) */
#define  HALF_PN_CIRCLE    (SHORT_CODE_HALF_LEN * (int4) CHIPX8_PER_CHIP)
#define POS_COMPARE_LT( a, b )                                                \
  ( POS_IN_RANGE( (a), (b) - HALF_PN_CIRCLE-1, (b) ) )
#define POS_COMPARE_GT( a, b )                                                \
  ( POS_IN_RANGE( (a), (b), (b) + HALF_PN_CIRCLE-1 ) )


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*=============================================================================

FUNCTION       SRCH_UTIL_WIN_THRESH_LECIO

DESCRIPTION    This function looks up the energy to threshold peaks as a
               function of window size.

DEPENDENCIES   None

RETURN VALUE   Threshold in linear ecio

SIDE EFFECTS   None

=============================================================================*/
extern uint32 srch_util_win_thresh_lecio
(
  uint32 win_size /* window size in cx8 */
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_LECIO

DESCRIPTION    This function converts the peak energies to linear ecio and
               does the total linero ecio calc with filtering.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_util_calc_lecio
(
  srch_sect_result_type *results, /* sector to calc linear ecio */
  srch_sect_parm_type   *parms    /* sector to calc linear ecio */
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_LECIO_THRESH

DESCRIPTION    This calculates the lecio threshold for the sector.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_util_calc_lecio_thresh
(
  srch_sect_result_type *results, /* sector to calc linear ecio */
  srch_sect_parm_type   *parms    /* sector to calc linear ecio */
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_INX_FROM_POS

DESCRIPTION    This function takes sector structure and index and computes the
               peak inx value.

DEPENDENCIES   None

RETURN VALUE   Peak index from search results.

SIDE EFFECTS   None

=============================================================================*/
extern int32 srch_util_calc_inx_from_pos
(
  srch_sect_struct_type *sect,
  int32                  pos_index
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_SRCH_FILTER

DESCRIPTION    This function grows the IIR Filter for the search results.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_util_calc_srch_filter
(
  srch_sect_struct_type               *sector
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_SRCH_OFFSET

DESCRIPTION    This function determines the offset needed for neighbor searches
               as specified in OTA Neighbor List Messages and CFS Requests.

DEPENDENCIES   None

RETURN VALUE   Calculated search offset

SIDE EFFECTS   None

=============================================================================*/
extern int32 srch_util_calc_srch_offset
(
  srch_sect_struct_type *sector
);

/*===========================================================================

FUNCTION       SRCH_UTIL_PILOT_TRPT

DESCRIPTION    This function checks the pilots list in a trpt struct
               for a given pilot.  If the pilot is present, it returns its index
               in the list.  If the pilot is not present, it is added and
               NO_PILOT is returned.

               This is used to keep track of what has been reported to the base
               station.  For each sector, fake pilot value PILOT_SELF is put on
               this list to mark the sector itself as having been reported to
               the base.  Where this sector is being compared to other sectors
               for certain conditions, a report of this sector relative to
               another sector is marked by putting that other sector's pilot on
               this list.

DEPENDENCIES   None.

RETURN VALUE   The index in the trpt.pilots list if there is a match, or
               NO_PILOT if not.

SIDE EFFECTS   The pilot may be added to the trpt.pilots list.

===========================================================================*/
extern uint16 srch_util_pilot_trpt
(
  srch_trpt_struct_type  *trpt_ptr,
  int16  pilot
);

/*===========================================================================

FUNCTION       SRCH_UTIL_GEN_EPOCH

DESCRIPTION    This function generates an EPOCH, which establishes an
               arbitrary zero-offset time reference.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   PN generates, fingers, searcher, PDM's etc initialized.

===========================================================================*/
extern void srch_util_gen_epoch( void );

/*===========================================================================

FUNCTION       SRCH_SIG_MAPPER

DESCRIPTION    This function maps signals to state machine inputs

DEPENDENCIES   None

RETURN VALUE   TRUE if mapped, FALSE otherwise

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_sig_mapper
(
  rex_tcb_type           *tcb,     /* tcb of the process with the signal */
  rex_sigs_type           sig,     /* signal which is set */
  stm_sig_cmd_type       *sig_cmd, /* ptr to the sig_cmd structure to populate */
  stm_state_machine_type *SM_type  /* ptr to a state machine to handle sig */
);

/*===========================================================================

FUNCTION       SRCH_READ_ALL_1X_NV_ITEMS

DESCRIPTION    This function reads all the NV items for 1x.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_read_all_1x_nv_items(void);

#endif /* SRCH_UTIL_H */
