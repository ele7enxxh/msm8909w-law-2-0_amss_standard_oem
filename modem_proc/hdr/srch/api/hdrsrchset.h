#ifndef HDRSRCHSET_H
#define HDRSRCHSET_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           H D R    S R C H    S E T

                          Search Set types Header File

GENERAL DESCRIPTION

   This file contains the databases, typedefs, enums and defines used in
   the HDR Pilot Search operations. See more header comments in hdrsrchset.c

EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/api/hdrsrchset.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------- 
02/13/2014   ljl     Supported handoff override after tuneaway.
05/05/2014   ljl     Added HDRSRCH_CHANNEL_CHANGE_AND_NSET_UPDATE_CMD.
07/31/2013   sat     Mainlining Modem Statistics Feature
04/22/2013   arm     Added changes for DSDA coexistence manager.
05/02/2013   ljl     Added hdrsrchset_set_scc_info().
02/26/2013   mbs     Fixed featurization for IRAT code
03/19/2012   vlc     Fixed pilot purging to work with gRICE. 
02/15/2012   rkc     Added structure to save at least one ASET in hdrsrchset.
03/08/2012   wsh     Merged NLB support
03/08/2012   rkc     Added structure to save at least one ASET in hdrsrchset.
01/25/2012   grl     Update gRice rank for all ASP changes.
01/10/2012   vlc     Removed hdrsrchset_deactivate_pilot_with_wait(), not needed.
12/22/2011   vlc     Renamed to hdrsrchset_grice_deactivate_pilot_with_wait() 
                     for consistency. 
11/30/2011   vlc/lyl Added hdrsrchset_deactivate_pilot_with_wait().
09/09/2011   vlc/lyl Added support for MC gRICE /cset promotion. 
09/07/2011   lyl     Added hdrsrchset_set_asp_snrrank().
08/11/2010   smd     Added hdrsrchset_get_pilot_meas_rup_sec().
06/21/2010   cnx     Fixed compiler warnings.
04/21/2010   smd     Increased HDR_MAX_RUP_SET_SIZE to 32.
                     Supported feature of reverse time transfer.
04/16/2010   smd     Added hdrsrchset_create_aset_sect().
03/08/2010   lyl     Added hdrsrchset_sinr_linear_to_db().
03/05/2010   smd     Updated code based on code review comments.
02/09/2010   smd     Added feature of LTE to DO pilot measurement.
08/18/2009   etv     Changed hdrsrchset_get_demod_carrier_info to 
                     hdrsrchset_get_fl_carrier_info and added 
                     hdrsrchset_get_subaset_index.
05/18/2009   lyl     Updated for firmware MSGR interface.
04/07/2009   smd     Used 2 bytes to store RSSI value.
03/30/2009   rkc     Renamed purge function to hdrsrchset_pilot_group_purge_mc.
03/20/2009   rkc     Added HDRSRCHSET_PILOT_GROUP_CMP and
                     subaset_reportable_idx to hdrsrchset_struct_type
03/09/2009   smd     Added support for pilot measurement velcro solution.
02/10/2009   ljl     Added hdrsrchset_reset_ofs_filter().
03/06/2009   ljl     Added hdrsrchset_get_asp_index.
08/29/2008   lyl     Added hdrsrchset_filt_pri_chain_eng().
07/25/2008   ljl     Changed hdrsrchset_check_if_ofreq_chan() to
                     hdrsrchset_is_ofreq_chan().
06/06/2008   rkc     Added hdrsrchset_get_cc_monitoring_chan()
05/05/2008   ljl     Added hdrsrchset_get_basp_fl_sect_ptr().
03/27/2008   ljl     Added hdrsrchset_remove_sectors_on_chan().
12/17/2007   jyw     Changed carrier_idx to csr_idx to avoid confusion.
01/02/2008   ljl     Added hdrsrchset_get_demod_carrier_index().
12/17/2007   jyw     Added hdrsrchset_get_demod_asp_sect_ptr.
10/17/2007   jyw     Merged Rev B changes from branch.
09/20/2007   mt      Added hdrsrchset_adaptive_div_filter_sinr().
07/12/2007   grl     Added hdrsrchset_delete_set_sects function.
04/10/2007   mt      Defined and added RUP search set.
01/02/2006   ljl     Added hdrsrchset_drop_from_aset_to_nset().
10/31/2006   mt      Renamed function hdrsrchset_reacq_reset_filters().
04/06/2006   jyw     Feature ROBN.
07/27/2005   ljl     Kept NSET after system loss
07/28/2005   ds      Added support for Same PN on multiple frequencies (SPMF)
07/25/2005   ljl     Added nset broadcast and directed update
06/30/2005   sq      Updated copyright info
11/03/2004   mpa     Removed dependency on std.h (merged with sys.h)
05/27/2003   ajn     Replaced _get_sector_report with _get_pilot_report
03/11/2003   ajn     Updated Copyright notice
02/06/2003   aaj     Fixed set size definitions to avoid duplication
11/25/2002   aaj     Increased NSET set size from 20 to 40 
09/09/2002   aaj     Added function to return drop timer in msec for DDR#3
03/06/2002   sst     Updated parameter in hdrsrchset_get_sector_report()
                     Added hdrsrchset_identify_earliest_peak()
02/22/2002   sst     Added ref_pn input to hdrsrchset_get_sector_report()
02/05/2002   aaj     Made ecio_to_eng() function accessible to other files
01/30/2002   ajn     Added force_unique_pn function
01/17/2002   aaj     Changed sector struct to contain band+chan number
12/07/2001   ajn     Added function to drop pilot from ASET.
12/06/2001   ajn     Implemented NSet pilot aging [6.6.5.3.6]
08/21/2001   aaj     Moved hdrsrchutil_get_sector_report here
05/02/2001   aaj     Remaining Set Filtering Support added
04/11/2001   ajn     Added hdrsrchset_find_link_id( ).
03/30/2001   ajn     Added filter reset function.
11/22/2000   ajn     Code review changes
09/26/2000   aaj     Tested set management code in sync state
06/27/2000   aaj     Compiled the code successfully
06/21/2000   aaj     Incorporated code review comments
06/07/2000   aaj     Added search programming support
05/24/2000   aaj     Created this file

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "hdrsrchsect.h"
#include "hdrsrchdrv.h"
#include "hdrsrchtypes.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

/*==========================================================================

                     MACROS USED IN THE MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

/*==========================================================================

                        EXTERNAL STRUCTURES, VARIABLES

This section contains the declaration of the external variables, structs
which are used by other files of HDRSRCH module

==========================================================================*/

/*----------------------------*
 * Setsize specific constants *
 *----------------------------*/
#define HDR_MAX_ASET_SIZE        HDRSRCH_ASET_MAX_SIZE
  /* active set max size */

#define HDR_MAX_CSET_SIZE        HDRSRCH_ASET_MAX_SIZE
  /* candidate set max size */

#define HDR_MAX_NSET_SIZE        40
  /* neighbor set max size */

#define HDR_MAX_RSET_SIZE        1   
  /* remaining set max size */

#define HDR_MAX_RUP_SET_SIZE     32
  /* RUP search set max size */

#define HDR_MAX_WAIT_ASP_UPDATE_RESP_USEC         1000
  /* Max wait for FW's ASP Update Response Message in micro seconds */

#ifdef FEATURE_HDR_GRICE
#define HDR_GRICE_MAX_NUM_ASP    3
  /* Max number of ASP for GRICE tracking */

#define HDR_MAX_GHOST_GSET_SIZE ( HDR_GRICE_MAX_NUM_ASP - 1 )
  /* Ghost Pilot Group CSET max size is set to maximum GSET size minus
     1 ASET pilot.  There will always be at least 1 ASET pilot in GSET. */

#define HDR_GRICE_PERIODIC_SNR_ORDER_UPDATE_TIMER 14
  /* gRICE ASP SINR Order update timer is set to 14 search cycles
     or 256 slots for regular periodic updates */

#define HDR_GRICE_SCHEDULED_RANKING_TIMER 2
  /* Scheduled ranking update happens in 2 search cycles
     or 60 ms for regular periodic updates */

#define HDR_GRICE_SNR_ORDER_DEFAULT_VAL           7
  /* Default SNR order value, this setting means the ASP is not active */
#else
#define HDR_MAX_GHOST_GSET_SIZE 0
#endif /* FEATURE_HDR_GRICE */

/* HDRSRCHSET_PILOT_GROUP_CMP returns TRUE if 2 given sectors
   belong to the same Pilot Group.
   From the RevB Spec:
   Two pilots are defined to belong to the same Pilot Group if both the
   PN offset and the PilotGroupID associated with the two pilots are the
   same.
   Note: If not RevB, then do not use Pilot Group ID to compare.
*/
#define HDRSRCHSET_PILOT_GROUP_CMP( sector1, sector2 )                  \
         ( ( ( sector1 ).pn_offset == ( sector2 ).pn_offset ) &&              \
           ( hdrsrchstate_is_revb_mode() ?                                    \
             ( ( sector1 ).pilot_gid == ( sector2 ).pilot_gid ) : TRUE ) )

/* Action to take after returning from pilot purge function */
typedef enum
{
  HDRSRCHSET_PILOTPURGE_ACTION_NO_ACTION,
  HDRSRCHSET_PILOTPURGE_ACTION_ADD_CSET,
  HDRSRCHSET_PILOTPURGE_ACTION_ADD_NSET
} 
hdrsrchset_pilotpurge_action_enum_type;

/* Contains input ptr and default action type to be passed to the pilot purge
   function.  The pilot purge function will return the ptr to be added and 
   action type */
         
typedef struct
{
  hdrsrchset_pilotpurge_action_enum_type    action;
    /* Action to take after returning from pilot purge function */

  hdrsrch_sect_struct_type                  *sect_ptr;
    /* Pointer to sector to add */
}
hdrsrchset_pilotpurge_struct_type;

/*---------------------*
 * HDR SRCH Set Tables *
 *---------------------*/

typedef struct
{
  boolean                     pn_update;
    /* flag which tells weather or not to update the remaining set pilot */

  uint8                       cross_cnt;
    /* rset threshold cross count */
}
hdrsrchset_rset_params_struct_type;


typedef struct
{

  hdrsrch_nset_pilot_type    pilot_list[ HDR_MAX_NSET_SIZE ];   
    /* The pilot list */

  uint8                      count;
    /* The number of pilots in the update list */

}
hdrsrchset_nset_update_list_type;

typedef struct
{

  uint8 nset_srch_idx;
    /* As the nset sectors are searched in round robin fashion, these indexes
       point to the start and end of the nset indices which were searched last
       these numbers should be set to zero when nset is shuffled */

  hdrsrchset_nset_update_list_type   broadcast;
    /* The broadcast nset update list */

  hdrsrchset_nset_update_list_type   directed;
    /* The directed nset update list */

} 
hdrsrchset_nset_params_struct_type;

#ifdef FEATURE_HDR_GRICE
typedef struct
{

  uint16                             filt_sinr;
    /* Filtered SINR values of ASPs, read from mDSP */

  uint8                              asp_idx;
    /* Index of ASPs */
} 
hdrsrchset_asp_snr_struct_type;
#endif /* FEATURE_HDR_GRICE */

/*---------------------*
 * HDR SRCH SubASET    *
 *---------------------*/
#ifdef FEATURE_HDR_REVB
typedef struct
{
  hdrsrch_sect_struct_type *  aset[ HDRSRCH_MAX_SUBASET_SIZE+1];
    /* sectors belong to this SubASET */

  uint8                       aset_cnt;
    /* number of sectors for this SubASET */

#ifdef FEATURE_HDR_GRICE
  uint8                       gcset_cnt;
    /* number of sectors for this SubASET which were promoted
       from CSET to gRICE CSET to improve gRICE performance.
       Unlike normal CSET sectors, these sectors are committed
       to mDSP similar to ASET sectors, and they also have finger
       assignments. */
#endif /* FEATURE_HDR_GRICE */

  boolean                     non_reportable;
    /* if this subASET is none-reportable per TCA */

  boolean                     cc_carrier;
    /* if this SubASET carries the CC channel */

  hdrsrch_carrier_idx_type    csr_idx;
    /* The firmware CSR/Sample RAM index. SW dictate it to the 
       RF interface and write it again to the fw ASP structure. */

  hdrsrch_demod_idx_type      demod_idx;
    /* The firmware demod carrier index. It is in the firmware
       ASP interface. */

  hdrsrch_tca_idx_type        tca_idx;
    /* The TCA index of the subactive set in the latest TCA message */

  sys_channel_type            channel;
   /* Channel on which this sector resides */

  uint16                       hdr_rssi;
    /* RSSI storage */

  boolean                     tune_away_ho_override;
    /* Tune away ho override flag */

  boolean                     tune_away_basp_index;
    /* Best ASP index at tune away */

  boolean                     tc_ho_override;
    /* Whether handoff override at traffic state is pending */

  uint16                      basp_change_cnt;
    /* best ASP change count */

  boolean                     tca_disjoint;
    /* Flag to indicates the subaset is being updated with a disjoint set if 
       - TCA subaset has a new demod index (not persistent with old subasets);
       - TCA subaset has an existing demod index but the best asp in the old 
         subaset can not be found in that TCA subaset */

#ifdef FEATURE_HDR_GRICE
  hdrsrchset_asp_snr_struct_type     asp_snr_arr[ HDR_MAX_ASET_SIZE ];
    /* Array of ASP filtered SINR values used to create ASP SNR Order list */

  uint16                      asp_snr_sorted_arr[  HDR_MAX_ASET_SIZE ];
    /* Array of ASP filtered SINR values after sorting, as it is given to
       mDSP */

  uint16                      asp_snr_arr_valid;
    /* Valid flag as it is given to mDSP */
#endif /* FEATURE_HDR_GRICE */

#ifdef FEATURE_HDR_REVC
  uint8                       NLB_K;
#endif /* FEATURE_HDR_REVC */
}
hdrsrch_subaset_struct_type;
#endif /* FEATURE_HDR_REVB */

/*---------------------*
 * HDR SRCH Set Tables *
 *---------------------*/

typedef struct
{
  /* Active Set */

#ifndef FEATURE_HDR_REVB
  hdrsrch_sect_struct_type *  aset[ HDR_MAX_ASET_SIZE+1];
  uint8                       aset_cnt;
#else
  hdrsrch_subaset_struct_type subaset[ HDRSRCH_MAX_NUM_SUBASETS ]; 
  uint8                       subaset_cnt;
  int8                        subaset_reportable_idx;
    /* Index of reportable subaset with CC carrier prefered. */
#endif /* FEATURE_HDR_REVB */

  /* Candidate Set */

  hdrsrch_sect_struct_type *  cset[ HDR_MAX_CSET_SIZE+1];
  uint8                       cset_cnt;


  /* Neighbour Set */

  hdrsrch_sect_struct_type *  nset[ HDR_MAX_NSET_SIZE+1];
  uint8                       nset_cnt;


  /* Remaining Set */

  hdrsrch_sect_struct_type *  rset;
    /* Note that we do not maintain the whole remaining set. We rather have
       only the PN offset of the last searched remaining set. Next sector
       in the rset is the next (+pilot_inc) PN offset to rset_pn which is
       not present in any aset, cset or nset */

  /* RUP search Set */

  hdrsrch_sect_struct_type *  rupset[ HDR_MAX_RUP_SET_SIZE ];
  uint8                       rupset_cnt;
  uint32                      rupset_strongest_peak_posx2;

  hdrsrchset_rset_params_struct_type rset_params;
    /* remaining set sector related parameters used in evaluating the pilot */

  hdrsrchset_nset_params_struct_type nset_params;
    /* Neighbor set sector related parameters */

  hdrsrch_sect_struct_type    last_subaset0_aset0;
    /* Last hdrsrchset.subaset[0].aset[0] saved during transient case where
       all ASET's are deleted before adding new ASETS, e.g., TCA cmd
       processing.
       Note: This is a temporary fix until at least one ASET is kept during
       ASET updates. */

#ifdef FEATURE_HDR_GRICE 
  boolean                     asp_snr_order_timer_enabled;
    /* Delay timer to rank ASPs committed to mDSP using their SINR value 
       When enabled, SW will update ranking array to FW every time the
       ASP changes and also periodically. */

  uint8                       asp_snr_order_timer_cnt;
    /* Delay timer count in units of search cycles (30ms) to rank ASPs committed
       to mDSP using their SINR value */

  hdrsrch_sect_struct_type *  sorted_cset[ HDR_MAX_CSET_SIZE+1];
  uint8                       sorted_cset_cnt;
    /* Strongest CSET pilots are selected to be part of gRICE set in FW */

#endif /* FEATURE_HDR_GRICE */
  
}
hdrsrchset_struct_type;


extern hdrsrchset_struct_type hdrsrchset;



/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*=========================================================================

FUNCTION     : HDRSRCHSET_INIT

DESCRIPTION  : This function initializes the search database. This should
               be called only one time before using the search sectors.
               
DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_init( void );


/*=========================================================================

FUNCTION     : HDRSRCHSET_CREATE_ASET

DESCRIPTION  : This function initializes the active set with the pilot that
               was demodulated during acquisition and synchronization. It
               does reverse engineers the locked finger date to create a
               equivalent active set pilot.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_create_aset( void );


/*=========================================================================

FUNCTION     : HDRSRCHSET_MANAGE_SETS

DESCRIPTION  : This function manages A,C,N and R set transitions as per the
               HAI standard.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : May result in switching sectors between different sets. Changes
               total_eng field in sector table.

=========================================================================*/

int8 hdrsrchset_manage_sets
(
  hdrsrchdrv_srch_list_struct_type *srch_list,
    /* search sweep list */

  uint8                             srch_cnt
    /* search count in the list */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_INIT_SETS

DESCRIPTION  : This function initializes the search database and keep the 
               nset.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_init_sets( void );



/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_REACQ_RESET_FILTERS

DESCRIPTION  : This function resets the filter lengths to zero, since after
               sleep energies can be significantly different.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_reacq_reset_filters( void );


/*=========================================================================

FUNCTION     : HDRSRCHSET_RESET_OFS_FILTER

DESCRIPTION  : This function resets the OFS filter length.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_reset_ofs_filter
(
  hdrsrch_sect_struct_type   *sect
    /* The sector whose OFS filter is to be reset */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_CALC_TOT_ENG

DESCRIPTION  : This function calculates the total sector energy for all the
               sectors in the search list.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : It changes the total_eng field in the sector table

=========================================================================*/

void hdrsrchset_calc_tot_eng
(
  hdrsrchdrv_srch_list_struct_type *srch_list,
    /* search sweep list */

  uint8                             srch_cnt
    /* search count in the list */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_IDENTIFY_EARLIEST_PEAK

DESCRIPTION  : This function scans the search list to find each sector's 
                earliest arriving peak whose signal is of sufficient 
                strength

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : It changes the peak_pos field in the sector table

=========================================================================*/

void hdrsrchset_identify_earliest_peak
(

  hdrsrchdrv_srch_list_struct_type *srch_list,
    /* search sweep list */

  uint8                             srch_cnt
    /* search count in the list */

);

/*=========================================================================

FUNCTION     : HDRSRCHSET_IDENTIFY_STRONGEST_PEAK_FOR_RUPSET

DESCRIPTION  : This function finds the strongest peak in strongest sector.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : It changes the peak_pos field in the sector table

=========================================================================*/

void hdrsrchset_identify_strongest_peak_for_rupset
(

  hdrsrchdrv_srch_list_struct_type *srch_list,
    /* search sweep list */

  uint8                             srch_cnt
    /* search count in the list */

);

/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_IDENTIFY_1X_PEAK

DESCRIPTION  : This function scans the search list to find the peak location
               for 1x neighbor search.
               
DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : It changes the peak_pos field in the sector table

=========================================================================*/

void hdrsrchset_identify_1x_peak
(

  hdrsrchdrv_srch_list_struct_type *srch_list,
    /* search sweep list */

  uint8                             srch_cnt
    /* search count in the list */
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHSET_GET_1X_ECIO_EQUVAILENT_ENERGY

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void hdrsrchset_get_1x_ecio_equivalent_energy
(
  hdrsrchdrv_srch_list_struct_type *srch_list,
    /* search list */

  uint8                             srch_cnt
    /* search count in the list */ 
);

/*=========================================================================

FUNCTION     : HDRSRCHSET_FILT_ENG

DESCRIPTION  : This function filters searched sector energies

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_filt_eng
(
  hdrsrchdrv_srch_list_struct_type *srch_list,
    /* search sweep list */

  uint8                             srch_cnt
    /* search count in the list */    
);

/*=========================================================================

FUNCTION     : HDRSRCHSET_FILT_PRI_CHAIN_ENG

DESCRIPTION  : This function filters the primary chain energies in the
               search list

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_filt_pri_chain_eng
(
  hdrsrchdrv_srch_list_struct_type *srch_list,
    /* search sweep list */

  uint8                             srch_cnt
    /* search count in the list */    
);

/*=========================================================================

FUNCTION     : HDRSRCHSET_FILT_OFS_ENG

DESCRIPTION  : This function filters the OFS sector energies

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_filt_ofs_eng
(
  hdrsrchdrv_srch_list_struct_type *srch_list,
    /* search sweep list */

  uint8                             srch_cnt
    /* search count in the list */    
);

#ifdef FEATURE_ATOM

/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_OPMODE_FILT_ENG

DESCRIPTION  : This function filters the serving sector energy.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_opmode_filt_eng
(
  hdrsrchdrv_srch_list_struct_type *srch_list,
    /* search sweep list */

  uint8                             srch_cnt
    /* search count in the list */    
);

#endif /* FEATURE_ATOM */

/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_FIND_LINK_ID

DESCRIPTION  : This function searches all the sectors in the mobile's
               set tables to find the sector entry with the given link_id.

               For revB, the function searches across all the subasets.

DEPENDENCIES : None

INPUT        : link_id

RETURN VALUE : sector pointer, or NULL.

SIDE EFFECTS : None

=========================================================================*/

hdrsrch_sect_struct_type* hdrsrchset_find_link_id
(
  hdrsrch_link_id_type              link_id
    /* Link id of of required sector */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_GET_SECT

DESCRIPTION  : This function returns a pointer to the sector table entry
               which matches the given PN offset and the frequency. If
               there is no matching entry then it creates a new sector and
               returns the pointer to it.

DEPENDENCIES : None

RETURN VALUE : Pointer to the sector table entry for the new active sector

SIDE EFFECTS : None

=========================================================================*/

hdrsrch_sect_struct_type* hdrsrchset_get_sect
(
  sys_pilot_pn_type               pn_offset,
    /* PN offset of the sector */

  sys_channel_type                chan
    /* Channel number of the sector */
);


/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_CREATE_RUP_SECT

DESCRIPTION  : This function creates a sector for RUP search and
               returns a pointer to it.

DEPENDENCIES : None

RETURN VALUE : Pointer to the sector table entry for the new rup sector

SIDE EFFECTS : None

=========================================================================*/

hdrsrch_sect_struct_type* hdrsrchset_get_rup_sect
(
  sys_pilot_pn_type               pn_offset,
    /* PN offset of the sector */

  sys_channel_type                chan,
    /* Channel of the sector */

  hdrsrch_system_enum_type        system_type
    /* system type for the current channel */
);

/*=========================================================================

FUNCTION     : HDRSRCHSET_GET_PILOT_MEAS_RUP_SECT

DESCRIPTION  : This function creates sector for pilot measurement search and
               returns a pointer to it.

DEPENDENCIES : None

RETURN VALUE : Pointer to the sector table entry for the new rup sector

SIDE EFFECTS : None

=========================================================================*/

hdrsrch_sect_struct_type* hdrsrchset_get_pilot_meas_rup_sect
(
  sys_pilot_pn_type               pn_offset,
    /* PN offset of the sector */

  sys_channel_type                chan,
    /* Channel of the sector */
    
  hdrsrch_system_enum_type        systm
    /* system type for the current channel */
    
);
/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_ADD_RUP_SECT

DESCRIPTION  : This function adds an existing sector to RUP search set.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_add_rup_sect
(
  hdrsrch_sect_struct_type        *sect
    /* Sector pointer */
);

/*=========================================================================

FUNCTION     : HDRSRCHSET_REMOVE_SAME_PN_ON_DIFF_CHANNEL

DESCRIPTION  : This function deletes pilots (other than ASET pilots) with 
               the same PN offset as the given sector, but with a different 
               channel that is listed on the hash channel list of the Sector 
               Parameters message. This is the case after NSET update is 
               performed after channel hashing. 

               If the hash channel check is FALSE, then each pilot's channel
               is not checked against the hash channel. This is the case 
               immediately after idle channel change is performed due to
               channel hashing and before the next NSET update is received. 

               This pilot deletion is to prevent ping-pong between channel 
               hashing and handoff to OFS pilots on the same sector.

INPUTS       : serving_sect       - Current serving sector
               check_hash_channel - Flag to indicate whether each pilot's 
                                    channel has to be checked against hash 
                                    channel.
                                    (TRUE - check, FALSE - don't check)             
               channel_ptr        - Pointer to a hash channel in SP chan list

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_remove_same_pn_on_diff_channel
(
  hdrsrch_sect_struct_type        *serving_sect,      
    /* Pointer to the current serving sector */

  boolean                         check_hash_channel,
    /* Flag to indicate whether hash channel has to be checked */

  const sys_channel_type          *channel_ptr
    /* Pointer to a hash channel in the SP message channel list */
);

/*=========================================================================

FUNCTION     : HDRSRCHSET_REMOVE_SECTORS_ON_CHAN

DESCRIPTION  : This function deletes the sectors on a channel.

INPUTS       : channel_ptr        - Pointer to a channel
               

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_remove_sectors_on_chan
(
  const sys_channel_type          *channel_ptr
    /* Pointer to a channel */
);


#ifdef FEATURE_HDR_REVB
/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_PILOT_GROUP_PURGE_MC

DESCRIPTION  : Only purge if multi-carrier mode, otherwise skip purging.
               This function enforces only one member is in CSET/NSET within
               the same pilot group. The NSET/CSET pilots on cc carrier are 
               preferred. If there is no cc carrier, the new pilot is 
               preferred.

INPUTS       : serving_sect       - Current serving sector

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_pilot_group_purge_mc
(
  hdrsrchset_pilotpurge_struct_type         *pilot_purge,
    /* Contains input ptr and default action type to be passed to the pilot purge
       function. */ 
  const sys_channel_type                    *pref_c
    /* Prefered carrier */
);
#endif /* FEATURE_HDR_REVB */

#ifdef FEATURE_HDR_REVB
/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_PILOT_GROUP_PURGE_CNSET_MC

DESCRIPTION  : Only purge if multi-carrier mode, otherwise skip purging.
               This function enforces only one member is in CSET/NSET within
               the same pilot group. The NSET/CSET pilots on cc carrier are 
               preferred. If there is no cc carrier, the new pilot is 
               preferred.

               Unlike hdrsrchset_pilot_group_purge_mc(), this function
               acts on the entire C/NSET instead of a single new NSET pilot.
               This function is called when srch changes from Rev0A to RevB
               personality.

INPUTS       : 

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_pilot_group_purge_cnset_mc( void );
#endif /* FEATURE_HDR_REVB */

/*=========================================================================

FUNCTION     : HDRSRCHSET_GET_NEW_RPC_IDX

DESCRIPTION  : This function returns a new RPC index that is not used by
               any of the Active Set Pilots when a new pilot from a new
               cell is added to the active set.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
#ifdef FEATURE_HDR_REVB
uint8 hdrsrchset_get_new_rpc_idx( uint8 subaset_idx );
#else
uint8 hdrsrchset_get_new_rpc_idx( void );
#endif /* FEATURE_HDR_REVB */


#ifdef FEATURE_HDR_REVB
/*=========================================================================

FUNCTION     : HDRSRCHSET_GET_NEXT_SCHEDULER_TAG

DESCRIPTION  : The function is used when a TCA does not include secheduler
               tags for aset pilots. The function returns next scheduler 
               tag which has not been used by the current ASET.

DEPENDENCIES : TBD

RETURN VALUE : next availabe scheduler tag

SIDE EFFECTS : TBD

=========================================================================*/

uint8 hdrsrchset_get_next_scheduler_tag( void );
#endif /* FEATURE_HDR_REVB */


/*=========================================================================

FUNCTION     : HDRSRCHSET_ADD_TO_ASET

DESCRIPTION  : Add the sector to the Active set list. If the active
               set list has already reached its limit then do something
               TBD

DEPENDENCIES : TBD

RETURN VALUE : None

SIDE EFFECTS : TBD

=========================================================================*/

void hdrsrchset_add_to_aset
(
#ifdef FEATURE_HDR_REVB
  uint8                   subaset_idx,
    /* which subaset to add */
#endif /* FEATURE_HDR_REVB */

  hdrsrch_sect_struct_type *  sect_ptr
    /* pilot to be added to aset */
);


#ifdef FEATURE_HDR_REVB
/*=========================================================================

FUNCTION     : HDRSRCHSET_ADD_TO_RSET

DESCRIPTION  : Adds the given sector to RSET. As we do not maintain any
               RSET sector list, it just gets dropped on the floor and the
               sector table slot is returned to the empty sector list for
               reuse.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_add_to_rset
(
  hdrsrch_sect_struct_type* sect_ptr
    /* pilot to be added to nset */
);
#endif /* FEATURE_HDR_REVB */


/*=========================================================================

FUNCTION     : HDRSRCHSET_ADD_TO_CSET

DESCRIPTION  : Add the sector to the candidate set list. If the candidate
               set list has already reached its limit then transfer the
               oldest and/or weakest candidate sector to neighbor set
               so as to limit the set size. The criterion used for deleting
               a CSET sector is as follows

               1. Delete the cset sector with pilot drop timer active

               2. If there are more than one cset sectors with active pilot
                  drop timers, then delete the one whose drop timer is
                  closest to expiry.

               3. If there are no cset sectors with pilot drop timer active
                  then delete the sector with least pilot strength.

DEPENDENCIES : TBD

RETURN VALUE : None

SIDE EFFECTS : TBD

=========================================================================*/

void hdrsrchset_add_to_cset
(
  hdrsrch_sect_struct_type *  sect_ptr
    /* pilot to be added to cset */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_ADD_TO_NSET

DESCRIPTION  : Add the sector to the Neighbor set list. If the neighbor
               set list has already reached its limit then transfer the
               oldest nset sector to remaining set so as to limit the set
               size. The criterion used for deleting a NSET sector is as
               follows

               1. Delete the nset sector with max AGE

DEPENDENCIES : TBD

RETURN VALUE : None

SIDE EFFECTS : TBD

=========================================================================*/

void hdrsrchset_add_to_nset
(
  hdrsrch_sect_struct_type *  sect_ptr
    /* pilot to be added to nset */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_NEXT_RSET_SECT

DESCRIPTION  : This function updates the rset pointer with the next
               remaining set PN. In the rare event of no available valid
               PN offset for RSET sector, this function will return NULL
               pointer in hdrsrchset.rset.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : TBD

=========================================================================*/
#ifdef FEATURE_HDR_REVB
void hdrsrchset_next_rset_sect
( 
  sys_channel_type    chan
    /* Generate the next rset sect on this channel */
);
#else
void hdrsrchset_next_rset_sect( void );
#endif /* FEATURE_HDR_REVB */


/*=========================================================================

FUNCTION     : HDRSRCHSET_DROP_FROM_ASET

DESCRIPTION  : This function drops the given pilot from the aset to either
               the NSET or the CSET, depending on whether or not the drop
               timer has expired.

DEPENDENCIES : This function should only be with a sector that is in the
               ASET

RETURN VALUE : None

SIDE EFFECTS : Deassigns fingers for the pilot.

=========================================================================*/


#ifdef FEATURE_HDR_REVB
/* EJECT */
/*=========================================================================

FUNCTION HDRSRCHSET_GET_REF_SUBASET_IDX

DESCRIPTION
  Returns subaset index for the reference subactive set.

DEPENDENCIES

RETURN VALUE
  Channel Management Parameters structure pointer.

SIDE EFFECTS
  None

=========================================================================*/

int8 hdrsrchset_get_ref_subaset_idx( void );
#endif /* FEATURE_HDR_REVB */


void hdrsrchset_drop_from_aset
(
  hdrsrch_sect_struct_type *      sect
    /* Pilot to be dropped from the ASET */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_DROP_FROM_ASET_TO_NSET

DESCRIPTION  : This function drops the given pilot from the ASET to 
               the NSET.

DEPENDENCIES : This function should only be called with a sector that 
               is in the ASET.

RETURN VALUE : None

SIDE EFFECTS : Deassigns fingers for the pilot.

=========================================================================*/

void hdrsrchset_drop_from_aset_to_nset
(
  hdrsrch_sect_struct_type             *sect
    /* Pilot to be dropped from the ASET */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_DROP_FROM_ASET_TO_RSET

DESCRIPTION  : This function drops the given pilot from the ASET to 
               the RSET.

DEPENDENCIES : This function should only be called with a sector that 
               is in the ASET.

RETURN VALUE : None

SIDE EFFECTS : Deassigns fingers for the pilot.

=========================================================================*/

void hdrsrchset_drop_from_aset_to_rset
(
  hdrsrch_sect_struct_type             *sect
    /* Pilot to be dropped from the ASET */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_AGE_NSET_PILOTS

DESCRIPTION  : This function ages all pilots in the NSET by one.
               [6.6.5.3.6]

DEPENDENCIES : This function should only be called when the AT receives a
               sector parameter list which is different from the previous
               one.  However, since the protocol layer caches the list,
               it only calls hdrsrch_nset_update when a change occurs.

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_age_nset_pilots( void );


/*=========================================================================

FUNCTION     : HDRSRCHSET_LOG_PILOT_SET_RECORD

DESCRIPTION  : Logs the pilot set record. It gets a log record from the diag
               log buffer, fills it with the latest pilot set information
               and commits the log record to the diag for delivery.

DEPENDENCIES : TBD

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/

void hdrsrchset_log_pilot_set_record( void );


/*=========================================================================

FUNCTION     : HDRSRCHSET_GET_ASP_SECT_PTR

DESCRIPTION  : This function returns the sector pointer for the active set
               sector which has the matching ASP_INDEX

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

hdrsrch_sect_struct_type* hdrsrchset_get_asp_sect_ptr 
( 
#ifdef FEATURE_HDR_REVB
  uint8 subaset_index, 
#endif /* FEATURE_HDR_REVB */
  uint8 asp_index 
);


#ifdef FEATURE_HDR_REVB
/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHSET_GET_DEMOD_ASP_SECT_PTR

DESCRIPTION  : This function returns the sector pointer for the active set
               sector which has the matching ASP_INDEX and DEMOD_CARRIER_INDEX

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

hdrsrch_sect_struct_type* hdrsrchset_get_demod_asp_sect_ptr 
( 
  uint8 demod_index, 
    /* Demod carrier index of the sector */

  uint8 asp_index 
    /* ASP index */
);
#endif /* FEATURE_HDR_REVB */



/*=========================================================================

FUNCTION     : HDRSRCHSET_GET_BASP_FL_SECT_PTR

DESCRIPTION  : This function returns the sector pointer for the best 
               active set sector for the specified subset.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

hdrsrch_sect_struct_type* hdrsrchset_get_basp_fl_sect_ptr
( 
  uint8                      sa
    /* Subaset index */
);


/* <EJECT> */
/*==========================================================================

FUNCTION HDRSRCHSET_GET_PILOT_REPORT

DESCRIPTION
  Retrieves a snapshot of pilot information.

DEPENDENCIES
  None

PARAMETERS
  max_pilots    - Maximum number of pilots which will be retrieved.
  pilots        - Array of length max_pilots, in which the pilot report
                  information will be stored.
  sect          - Reference pilot for report

RETURN VALUE
  None

SIDE EFFECTS
  Clears the "ReportPending" status for the reported sectors.

==========================================================================*/

int hdrsrchset_get_pilot_report
(
  int                                   max_pilots,
    /* Maximum # of pilot entries which may be placed in "pilots" array */

  hdrsrch_pilot_rpt_type *              pilots,
    /* Array to store pilot information in */

  hdrsrch_sect_struct_type *            sect
    /* Reference pilot for pilot report */
);

/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_FIND_SECT

DESCRIPTION  : This function searches all the sectors in the mobile's
               set tables to find the sector entry matching a particular
               PN offset and frequency parameters. It returns the pointer
               to the sector table entry for the matching pilot.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

hdrsrch_sect_struct_type* hdrsrchset_find_sect
(
  sys_pilot_pn_type               pn_offset,
    /* find sector with this PN offset */

  sys_channel_type                chan
    /* and this cdma channel */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_SCALE_ENG

DESCRIPTION  : This function scales a linear energy value up or down by
               a given dB factor (in 0.5dB)

DEPENDENCIES : None

RETURN VALUE : Scaled energy value

SIDE EFFECTS : None

=========================================================================*/

uint16 hdrsrchset_scale_eng
(
  uint16                          eng,
    /* Energy value to be scaled */

  int                             scale
    /* Scale factor, in 0.5dB units */
);

/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_ENG_TO_ECIO

DESCRIPTION  : This function converts an energy value in to Ec/Io in units
               of -0.5dB

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

int hdrsrchset_eng_to_ecio
(
  int                             eng
    /* Energy to be converted to -0.5dB units */
);

/*=========================================================================

FUNCTION     : HDRSRCHSET_ECIO_TO_ENG

DESCRIPTION  : This function converts an energy value in Ec/Io (-0.5dB) to
               a linear scale value

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

uint16 hdrsrchset_ecio_to_eng
(
  int                             eng
    /* Energy in -0.5dB units */
);

/*=========================================================================

FUNCTION     : HDRSRCHSET_ENG_TO_ECIO_CEILING_CONVERT

DESCRIPTION  : This function converts an energy value in to Ec/Io in units
               of -0.5dB

DEPENDENCIES : None

RETURN VALUE : Energy value in unit of -0.5 DB

SIDE EFFECTS : None

=========================================================================*/

int hdrsrchset_eng_to_ecio_ceiling_convert
(
  int                             eng
    /* Energy to be converted to -0.5dB units */
);

/*=========================================================================

FUNCTION     : HDRSRCHSET_SINR_LINEAR_TO_DB

DESCRIPTION  : This function converts linear value of SINR to dB scale value

DEPENDENCIES : None

RETURN VALUE : SINR in dB

SIDE EFFECTS : None

=========================================================================*/

int16 hdrsrchset_sinr_linear_to_db
( 
  uint16 linear_val 
    /* sinr in linear scale */
);

/* EJECT */
/*=========================================================================

FUNCTION HDRSRCHSET_GET_DROP_TIMER_MS

DESCRIPTION
  Returns drop timer value in msec

DEPENDENCIES

RETURN VALUE
  Drop timer value in msec

SIDE EFFECTS
  None

=========================================================================*/

int hdrsrchset_get_drop_timer_ms
(
  hdrsrch_sect_struct_type *            sect
    /* Sector pointer */
);

/*=========================================================================

FUNCTION     : HDRSRCHSET_MOVE_ACSET_TO_NSET

DESCRIPTION  : This function moves the pilot in ASET or CSET to NSET.
               It is called during system lost.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_move_acset_to_nset( void );

/*=========================================================================

FUNCTION     : HDRSRCHSET_FORCE_UNIQUE_ASET_PILOT

DESCRIPTION  : This function deletes sectors with the same chan and 
               PN as ASET pilot.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_force_unique_aset_pilot
(
  hdrsrch_sect_struct_type        *sect
    /* Sector to have a unique PN number */
);

/*==========================================================================

FUNCTION HDRSRCHSET_UPDATE_NSET_SEARCH_WINDOW_PARAMS

DESCRIPTION
  This function updates NSET pilot search window parameters (window size 
  and offset) using the broadcast or directed nset update list.

DEPENDENCIES
  None

PARAMETERS
  update_type    - Broadcast or directed

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/

void hdrsrchset_update_nset_search_window_params
(
  hdrsrch_nset_update_enum_type update_type
    /* Broadcast or directed */

);

/*===========================================================================

FUNCTION HDRSRCHSET_ADD_NSET_PILOTS

DESCRIPTION
  Updates the neighbour list with new pilots.
  Pilots already in the Active Set or the Candidate Set are not demoted
  to the neighbour set.

DEPENDENCIES
  None

PARAMETERS
  nset_pilots     - Array of pilots to put into NSET
  num_nset_pilots - Number of pilots in the array
  update_type     - Broadcast or directed
  
RETURN VALUE
  TRUE  - Set transitions performed.
  FALSE - No set transitions occurred.

SIDE EFFECTS
  NSet pilots are aged.

===========================================================================*/

boolean hdrsrchset_add_nset_pilots
(
  const hdrsrch_nset_pilot_type *nset_pilots,
    /* Array of pilots to put in neighbour set */

  int                            num_nset_pilots,
    /* Number of pilots to put into neighbour set */

  hdrsrch_nset_update_enum_type  update_type,
    /* Broadcast or directed update */

  const sys_channel_type         *pref_chan
    /* The prefered channel for nset update */

);

/*===========================================================================

FUNCTION HDRSRCHSET_REMOVE_OFF_BAND_NEIGHBORS

DESCRIPTION
  This function removes the off band sectors from the current CSET and 
  NSET whose band is not listed in the nset_pilots.

DEPENDENCIES
  None

PARAMETERS
  nset_pilots     - Array of pilots to put into NSET
  num_nset_pilots - Number of pilots in the array
  update_type     - Broadcast or directed
  
RETURN VALUE
  none

SIDE EFFECTS
  Offband neighbors are removed from CSET and NSET

===========================================================================*/

void hdrsrchset_remove_off_band_neighbors
(
  const hdrsrch_nset_pilot_type *nset_pilots,
    /* Array of pilots to put in neighbour set */

  int                            num_nset_pilots,
    /* Number of pilots to put into neighbour set */

  hdrsrch_nset_update_enum_type  update_type
    /* Broadcast or directed update */

);

/*==========================================================================

FUNCTION HDRSRCHSET_INIT_NSET_UPDATE_LIST

DESCRIPTION
  This function initializes nset update list.

DEPENDENCIES
  None

PARAMETERS
  update_type    - Broadcast or directed

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/

void hdrsrchset_init_nset_update_list
( 
  hdrsrch_nset_update_enum_type update_type
    /* Broadcast or directed */

);

/*==========================================================================

FUNCTION HDRSRCHSET_RESET_NSET_SEARCH_WINDOW_PARAMS

DESCRIPTION
  This function sets NSET pilot search window parameters (window size 
  and offset) to the default.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/

void hdrsrchset_reset_nset_search_window_params( void );


#ifdef FEATURE_HDR_REVB
/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_UPDATE_PILOT_GID

DESCRIPTION  : This function updates pilot group id to a given sector.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrsrchset_update_pilot_gid
( 
  hdrsrch_sect_struct_type        *sect_ptr,      
    /* The sector pointor of the sector to updated */

  hdrsrch_pilot_gid_type          gid
    /* Pilot group id */
);


/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_IS_OFS_CHAN

DESCRIPTION  : This function check if the sector is an off frequency sector.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : TURE  - an OFS channel
               FALSE - not an OFS channel

SIDE EFFECTS : None

=========================================================================*/

boolean hdrsrchset_is_ofreq_chan
( 
  sys_channel_type                chan
);


/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_TOTAL_ASET_CNT

DESCRIPTION  : This function returns the total number of ASET sectors across
               all subasets.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : Total ASET count

SIDE EFFECTS : None

=========================================================================*/

uint8 hdrsrchset_total_aset_cnt( void );


/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSET_SWAP_SUBASETS

DESCRIPTION  : This function swap the subaset[i] and subaset[j].

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_swap_subasets
(
  uint8 idx_i,
  uint8 idx_j 
); 


/* EJECT */
/*=========================================================================

FUNCTION HDRSRCHSET_MAP_DEMOD_IDX_TO_CHAN

DESCRIPTION
  The function returns the channel information based on the current demod
  index.

DEPENDENCIES

RETURN VALUE
  Channel information

SIDE EFFECTS
  None

=========================================================================*/

sys_channel_type hdrsrchset_map_demod_idx_to_chan
(
  hdrsrch_demod_idx_type   demod_idx
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHSET_GET_FL_CARRIER_INFO

DESCRIPTION
  The function returns the information about the forward link carrier.

DEPENDENCIES
  None

PARAMETERS
  subaset_idx - the sub aset idx associated with the carrier requested
  fl_carrier_info - The pointer to the info structure to fill.

RETURN VALUE
  TRUE if the carrier identified Sub ASET index is in use and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchset_get_fl_carrier_info
(
  hdrsrch_subaset_idx_type           subaset_idx,
    /* The sub active set index */

  hdrsrch_fl_carrier_info_type*       fl_carrier_info
    /* The pointer to the table to fill */
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHSET_GET_SUB_ASET_INDEX

DESCRIPTION
  The function returns the subaset index of the given demod carrier.

DEPENDENCIES
  None

PARAMETERS
  demod_idx - Index of the demod carrier

RETURN VALUE
  Sub Active Set index

SIDE EFFECTS
  None

===========================================================================*/

hdrsrch_subaset_idx_type hdrsrchset_get_subaset_index
(
  uint8                      demod_idx
    /* Demod index */
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHSET_GET_DEMOD_CARRIER_INDEX

DESCRIPTION
  The function returns the demod carrier index of the subaset.

DEPENDENCIES
  None

PARAMETERS
  sa - subaset index

RETURN VALUE
  return value of demod carrier index

SIDE EFFECTS
  None

===========================================================================*/

hdrsrch_demod_idx_type hdrsrchset_get_demod_carrier_index
(
  uint8                      sa
    /* Subaset index */
);


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHSET_GET_CC_MONITORING_CHAN

DESCRIPTION  : This function returns the channel pointer for the first active
               set channel that is a Control Channel Monitoring carrier.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : Pointer to CC Carrier channel if found, otherwise NULL

SIDE EFFECTS : None

=========================================================================*/

sys_channel_type* hdrsrchset_get_cc_monitoring_chan( void );

#endif /* FEATURE_HDR_REVB */

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHSET_DELETE_SET_SECTS

DESCRIPTION  : This function deletes all sectors in the specified set moving
               them to the EMPTY_SET.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchset_delete_set_sects
(
  hdrsrch_set_enum_type           set_type
    /* Set to be deleted */
);
/*==========================================================================

FUNCTION HDRSRCHSET_GET_PILOT_MEAS_REPORT

DESCRIPTION
  Retrieves pilot measurement information .

DEPENDENCIES
  None

PARAMETERS
  max_pilots    - Maximum number of pilots which will be retrieved.
  pilots        - Array of length max_pilots, in which the pilot report
                  information will be stored.
  sect          - Reference pilot for report

RETURN VALUE
  None

SIDE EFFECTS
  Clears the "ReportPending" status for the reported sectors.

==========================================================================*/

int hdrsrchset_get_pilot_meas_report
(
  int                                   max_pilots,
    /* Maximum # of pilot entries which may be placed in "pilots" array */

  hdrsrch_pilot_meas_rpt_type *         pilots,
    /* Array to store pilot information in */

  hdrsrch_sect_struct_type *            ref_sect
    /* Reference pilot for pilot report */
);

#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
/*==========================================================================

FUNCTION HDRSRCHSET_GET_LTE_PILOT_MEAS_REPORT

DESCRIPTION
  Retrieves pilot measurement information .

DEPENDENCIES
  None

PARAMETERS
  p_pilot_meas_rsp    - structure to store pilot measurement result

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchset_get_lte_pilot_meas_report
(
  hdrsrch_lte_pilot_meas_rsp_msg_t      * p_pilot_meas_rsp
    /* structure to store pilot information in */
);
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHSET_GET_ASP_INDEX

DESCRIPTION
  The function returns the asp index that matches the drc cover.

DEPENDENCIES
  None

PARAMETERS
  demod_idx - demod carrier index
  drc_cover - drc cover

RETURN VALUE
  return asp index that matches drc cover or invalid asp index if no match.

SIDE EFFECTS
  None

===========================================================================*/

hdrsrch_demod_idx_type hdrsrchset_get_asp_index
(
  uint8                      demod_idx,
    /* Demod carrier index */

  hdrmac_drc_cover_type      drc_cover
    /* drc cover */
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHSET_PROCESS_ASP_CHANGE_IND

DESCRIPTION
  The function increases the best ASP change count of the subaset when it
  gets the ASP change indication message

DEPENDENCIES
  None

PARAMETERS
  basp_change_mask - best ASP change mask

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchset_process_asp_change_ind
( 
  uint16              basp_change_mask
    /* best ASP change mask */        
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHSET_RESET_ASP_CHANGE_CNT

DESCRIPTION
  The function reset the subaset best ASP change count.

DEPENDENCIES
  None

PARAMETERS
  subaset_idx - subaset index

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchset_reset_asp_change_cnt
(
  uint8         subaset_idx
    /* SubActiveSet Index to clear */
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHSET_GET_ASP_CHANGE_CNT

DESCRIPTION
  The function returns the subaset best ASP change count.

DEPENDENCIES
  None

PARAMETERS
  subaset_idx - subaset index

RETURN VALUE
  best ASP change count

SIDE EFFECTS
  None

===========================================================================*/

uint16 hdrsrchset_get_asp_change_cnt
(
  uint8         subaset_idx
    /* SubactiveSet index */
);

/*===========================================================================

FUNCTION HDRSRCHSET_RESET_TCA_DISJOINT_FLAG

DESCRIPTION
  This function resets the tca_disjoint flag for all subasets.

DEPENDENCIES
  Connected State

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchset_reset_tca_disjoint_flag( void );

/*=========================================================================

FUNCTION     : HDRSRCHSET_CREATE_ASET_SECT

DESCRIPTION  : This function initializes the ASET sector structure

DEPENDENCIES : None

RETURN VALUE : 
  Pointer to aset sector just created

SIDE EFFECTS : None

=========================================================================*/

hdrsrch_sect_struct_type * hdrsrchset_create_aset_sect
(
  uint16       pn_offset,
    /* PN offset */ 

  byte         rpc_cell_idx,
    /* cell map */

  byte         mac_idx
    /* mac index */
);


#ifdef FEATURE_HDR_GRICE 
/*==========================================================================

FUNCTION HDRSRCHSET_GRICE_BUILD_ASP_SNR_ORDER_LIST
 
DESCRIPTION 
  This function builds the ASP SNR Order list using filtered SINR values
  read from mDSP.
 
  This function executes every 256 slots.
 
  If a new ASP list has just been committed, the ASP SNR Order list is also
  updated.  Still, a short delay of 2 search cycles is imposed to make sure
  SINR values read from mDSP are valid.
 
  When the timer expires, this function ranks all ASPs (aset ASPs and gcset
  ASPs) that were committed to mDSP.
  Order index of ASPs ranked by filtered SINR. 0 for strongest to
  HDR_GRICE_MAX_NUM_ASP - 1 for weakest; 7 for inactive ASP.
  mDSP also checks Config word in ASP structure to determine ASP is active
  before checking the corresponding entry in this array.
 
DEPENDENCIES
  Only runs during traffic state

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchset_grice_build_asp_snr_order_list( void );

/*==========================================================================

FUNCTION HDRSRCHSET_GRICE_MONITOR_ASP_SNR_ORDER_UPDATES
 
DESCRIPTION 
  This function monitors the ASP SNR Order timer, which is set to expire
  every 256 slots.  
  When the timer expires, this function ranks all ASPs (aset ASPs and gcset
  ASPs) that were committed to mDSP.
  Order index of ASPs ranked by filtered SINR. 0 for strongest to
  HDR_GRICE_MAX_NUM_ASP - 1 for weakest; 7 for inactive ASP.
  mDSP also checks Config word in ASP structure to determine ASP is active
  before checking the corresponding entry in this array.
 
DEPENDENCIES
  Only runs during traffic state

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchset_grice_monitor_asp_snr_order_updates( void );

/*==========================================================================

FUNCTION HDRSRCHSET_GRICE_ACTIVATE_ASP_SNR_ORDER_UPDATES
 
DESCRIPTION 
  This function activates/deactivates the ASP SNR Order timer, which is set
  to expire every 256 slots.
  If a new ASP list has just been committed, the ASP SNR Order list is also
  updated.  Still, a short delay of 2 search cycles is imposed to make sure
  SINR values read from mDSP are valid.
 
  When the timer expires, this function ranks all ASPs (aset ASPs and gcset
  ASPs) that were committed to mDSP.
  Order index of ASPs ranked by filtered SINR. 0 for strongest to
  HDR_GRICE_MAX_NUM_ASP - 1 for weakest; 7 for inactive ASP.
  mDSP also checks Config word in ASP structure to determine ASP is active
  before checking the corresponding entry in this array.
 
DEPENDENCIES
  Only runs during traffic state

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchset_grice_activate_asp_snr_order_updates
( 
  boolean                       activate_timer
    /* Activate/Deactivate ASP SNR Order timer */
);

/*==========================================================================

FUNCTION HDRSRCHSET_SCHED_ASP_RANKING
 
DESCRIPTION 
  Make sure the gRice ranking is scheduled soon once we have real SINRs from
  the FW.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchset_sched_asp_ranking( void );

/*==========================================================================

FUNCTION HDRSRCHSET_GRICE_UPDATE_GSET 

DESCRIPTION
  This function prepares to promote potential CSET pilots to gRICE CSET.
  gRICE CSET pilots have assigned fingers to improve gRICE performance.  The
  promotion criterion include:
  - The strongest CSET pilots get promoted
  - Total pilots committed to mDSP <= HDR_GRICE_MAX_NUM_ASP
 
DEPENDENCIES
  Only runs during traffic state

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchset_grice_update_gcset( void );

/*=========================================================================

FUNCTION HDRSRCHSET_GRICE_DROP_GCSET

DESCRIPTION 
  Remove pilots from gRICE CSET.

DEPENDENCIES 
  Only runs during traffic state

RETURN VALUE 
  TURE  - gcset is dropped
  FALSE - otherwise

SIDE EFFECTS 
  None

=========================================================================*/

boolean hdrsrchset_grice_drop_gcset( void );

/*=========================================================================

FUNCTION HDRSRCHSET_GRICE_DROP_SECT

DESCRIPTION 
  If a pilot is dropped from the CSET, check to see if it has been promoted
  to gRICE CSET.  If so, remove from gRICE CSET.

DEPENDENCIES 
  Only runs during traffic state

RETURN VALUE 
  None

SIDE EFFECTS 
  None

=========================================================================*/

void hdrsrchset_grice_drop_sect
(
  hdrsrch_sect_struct_type      *sect_ptr
    /* pilot to check for gRICE CSET */
);


/*=========================================================================

FUNCTION     : HDRSRCHSET_INIT_ASP_SNRRANK

DESCRIPTION  : This function init the ASP SNR rank in FW.

DEPENDENCIES : None

RETURN VALUE : None
  
SIDE EFFECTS : None

=========================================================================*/
void hdrsrchset_init_asp_snrrank( void );

#endif /* FEATURE_HDR_GRICE */

#ifdef FEATURE_HDR_REVC
/*==========================================================================

FUNCTION HDRSRCHSET_PILOT_IS_IN_ASET

DESCRIPTION
  Checks if given pilot is part of aset

DEPENDENCIES
  None

PARAMETERS 
  chan - Channel to search for
  pn   - Pilot PN to search for

RETURN VALUE
  True   - if pilot is part of ASET
  False  - otherwise

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrsrchset_pilot_is_in_aset
(
  const sys_channel_type* chan,
  const sys_pilot_pn_type pn
);

/*==========================================================================

FUNCTION HDRSRCHSET_IS_LINKID_ON_CC_MONITORING_CHAN

DESCRIPTION
  Checks if given link_id is on CC monitoring chan. This API is created
  as a faster alternative to calling hdrsrch_get_link_info() then compare
  the channel with hdrsrchset_get_cc_monitoring_chan(). 
  
  If link_info is already known for a link_id, then it is more efficient
  to compare its channel with hdrsrchset_get_cc_monitoring_chan()

DEPENDENCIES
  None

PARAMETERS 
  link_id - link_id to check 

RETURN VALUE
  True   - if link_id is on CC monitoring chan
  False  - otherwise

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrsrchset_is_linkid_on_cc_monitoring_chan
(
  hdrsrch_link_id_type link_id
);
#endif /* FEATURE_HDR_REVC */


/*==========================================================================

FUNCTION HDRSRCHSET_SET_SCC_INFO 

DESCRIPTION
  This fucntion sets scc offset and scc rate.

DEPENDENCIES
  None

PARAMETERS 
  scc_offset - SCC offset
  scc_rate   - SCC rate
  link_id - link_id to check 

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void  hdrsrchset_set_scc_info( 
  uint16 scc_offset,
    /* SCC offset */

  uint8  scc_rate,
    /* SCC rate */

  uint8  link_id
    /* Link Id */
);

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
/*==========================================================================

FUNCTION        HDRSRCHSET_GET_PHYSICAL_CARRIER_ID

DESCRIPTION
  This function return the demod id for the corresponding channel.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  demod index for the corresponding channel

SIDE EFFECTS
  None

==========================================================================*/
hdrsrch_demod_idx_type hdrsrchset_get_carrier_demod_id
( 
   sys_channel_type channel
);
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */
#endif /* HDRSRCHSET_H */

