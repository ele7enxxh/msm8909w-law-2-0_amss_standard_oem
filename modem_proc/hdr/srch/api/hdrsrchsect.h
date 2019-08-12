#ifndef HDRSRCHSECT_H
#define HDRSRCHSECT_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H D R    S R C H   S E C T O R

                          Search Sector Header File

GENERAL DESCRIPTION

   This file contains the databases, typedefs, enums and defines used in
   the HDR Search Sector operations.

EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/api/hdrsrchsect.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------- 
05/02/2013   ljl     Saved scc offset and rate info.
08/07/2012   wsh     Fixed compile error
04/16/2012   vlc     Removed HDR_EMPTYSET_RETURN enum value. 
03/19/2012   vlc     Fixed pilot purging to work with gRICE. 
02/06/2012   arm     Supported diversity Ec/Io.
09/21/2011   vlc/lyl Added MC gRice /cset promotion.
06/03/2009   lyl     Updated for firmware MSGR interface.
08/22/2008   lyl     Supported primary chain ASET search.
08/19/2008   smd     Added idle handoff improvement.
06/11/2008   cc      Added aux_drc_cover.
10/31/2007   ljl     Merged from Rev B branch.
04/10/2007   mt      Enhanced OFS and ATOM changes.
02/07/2007   rmg     Added support for TxT2P adjust (DeltaT2P).
10/27/2006   mt      Added timeconstant field to OFS pilot filter.
09/27/2006   jyw     Added support for using interpolated srch results.
06/07/2006   ljl     Added SBHO optimization
10/04/2005   ljl     Changed ra_channel_gain_lin type to uint16
09/28/2005   ljl     Added support of RAChannelGain
09/28/2005   ljl     Added instantaneous pilot energy
06/03/2005   sq      Added a new pilot strength filter in sector structure
06/01/2005   ljl     Added DSC support
11/03/2004   mpa     Removed dependency on std.h (merged with sys.h)
04/05/2004   aaj     Added typedef for color code type
02/27/2004   aaj     Added color_code to the hdr sector structures
10/06/2003   aaj     Mainlined HDRSRCH_MULTIPATH_WIN_CENTER
03/11/2003   ajn     Updated Copyright notice
12/19/2002   ajn     Added C_ABOVE_A_AND_ADD report type.
09/18/2002   aaj     Support for multipath window centering for HDR
03/06/2002   sst     Added peak_pos to hdrsrch_sect_struct_type
01/17/2002   aaj     support for RF band to the hdr sector structure type
04/10/2001   ajn     Added link_id.  Rename freq_idx -> chan_num.
11/30/2000   ajn     Sector structure reorg
11/22/2000   ajn     Code review changes
10/23/2000   ajn     Created this file

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrsrchtypes.h"
#include "hdrsrchi.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

typedef uint16 hdrsrch_color_code_type;
/* HDR Protocol uses a 8 bit color code type to be compliant with the OTA
   message types. However since 0-255 all are valid color codes, HDRSRCH
   uses a 16 bit color code type with 0xffff as an escape value to indicate
   invalid or NO color code */


/*----------------------------------------------------------------------*
                     hdrsrch_set_enum_type
 *----------------------------------------------------------------------*/

/* enum type for search sets */

typedef enum
{
  HDR_EMPTYSET,             /* HDR Sector NULL type */
  HDR_RSET,                 /* HDR Remaining Set sector type */
  HDR_NSET,                 /* HDR Neighbor Set sector type */
  HDR_CSET,                 /* HDR Candidate Set sector type */
  HDR_ASET,                 /* HDR Active Set sector type */
  HDR_RUPSET,               /* HDR RUP Search Set sector type */  
#ifdef FEATURE_HDR_GRICE
  HDR_GHOST_GSET,           /* HDR Ghost gRICE CSET sector type */
#endif /* FEATURE_HDR_GRICE */
  HDR_MAX_NUM_SETS          /* Maximum number of set types */
}
hdrsrch_set_enum_type;


/*----------------------------------------------------------------------*
                     hdrsrch_pilot_rpt_enum_type
 *----------------------------------------------------------------------*/

typedef enum
{

  HDRSET_PILOT_RPT_NONE,
    /* No pilot report pending */

  HDRSET_PILOT_RPT_DROP_A,
    /* ASET pilot drop timer expired */

  HDRSET_PILOT_RPT_R_TO_C,
    /* Pilot moved from RSET to CSET */

  HDRSET_PILOT_RPT_N_TO_C,
    /* Pilot moved from NSET to CSET */

  HDRSET_PILOT_RPT_C_ABOVE_A,
    /* CSET pilot is PILOT_COMPARE above an ASET pilot  */

  HDRSET_PILOT_RPT_C_ABOVE_ADD,
    /* CSET pilot is above add threshold */

  HDRSET_PILOT_RPT_C_ABOVE_A_AND_ADD
    /* CSET pilot above both ASET pilot and ADD threshold */

}
hdrsrch_pilot_rpt_enum_type;

#ifdef FEATURE_HDR_GRICE 

/*----------------------------------------------------------------------*
                     hdrsrchset_grice_cset_enum_type
 *----------------------------------------------------------------------*/

typedef enum
{
  HDR_GRICE_CSET_NONE,               /* Not a gRICE CSET pilot */
  HDR_GRICE_GHOST_CSET,              /* Pilot group candidate of gRICE CSET 
                                        is promoted to gRICE CSET status 
                                        with finger assignement */
  HDR_GRICE_CSET                     /* True CSET pilot is promoted to gRICE
                                        CSET status with finger assignment */
}
hdrsrchset_grice_cset_enum_type;
  /* Type of gRICE CSET pilot */
  
#endif /* FEATURE_HDR_GRICE */

/*----------------------------------------------------------------------*
                     hdrsrch_nset_sect_struct_type
 *----------------------------------------------------------------------*/

typedef struct
{
  hdrsrch_winsize_enum_type       win_size;
    /* Window Size when in Neighbour Set */

  hdrsrch_winoffset_enum_type     win_offset;
    /* Window Offset when in Neighbour Set */

  uint8                           age;
   /* Age of the sector. Used only in the NSET. It is incremented
      each time the SectorParametersMessage (neighbor list update message)
      is received and which contains different set of pilots in it */
}
hdrsrch_nset_sect_struct_type;


/*----------------------------------------------------------------------*
                     hdrsrch_aset_sect_struct_type
 *----------------------------------------------------------------------*/

typedef struct
{
  hdrmac_mac_index_type           mac_idx;
   /* RPC walsh channel Index */

  hdrmac_drc_cover_type           drc_cover;
   /* Data rate control channel cover */

  hdrmac_drc_cover_type           aux_drc_cover;
   /* Aux DRC Cover for data rate control channel for RevB rates */

  uint8                           asp_idx;
   /* active set pilot map. This indicates the active cell sectors that are
      combined in Ior/Nt to get total SINR */

  uint8                           rpc_cell_idx;
   /* Rev Power Control combiner cell mapping is stored in this element */

#ifdef FEATURE_HDR_REVA_L1
  uint16                          ra_channel_gain_lin;
    /* Linear value of RAChannelGain */

  int8                            delta_t2p;
    /* TxT2P adjust value in 1/4 dB */

  uint8                           dsc;
    /* DSC for the cell */
#endif /* FEATURE_HDR_REVA_L1 */

#ifdef FEATURE_HDR_REVB
  hdrmac_mac_index_type           rl_mac_idx;
    /* RL MAC Index is 9 bits for indicating the index used to 
       transmit power control commands. Used if using RUP subtype 1. */

  hdrmac_mac_index_type           rab_mac_idx;
    /* RL MAC Index is 7 bits for indicating the index used to 
       transmit RAB commands. Used if using RUP subtype 1. */

  hdrmac_rab_len_type             rab_length;
    /* Reverse Activity Bit Length */

  uint8                           rab_offset;
    /* Reverse Activity Bit Offset */

  uint8                           subaset_idx;
    /* SubASET idx to which the sector belongs to with valid the range of
     * [ 0, HDRSRCH_MAX_NUM_SUBASET-1 ]; please be aware this is the indx
     * to the software aset structure not of the fw structure */

  uint8                           scheduler_tag; 
    /* Scheduler tag assigned by the current TCA */

  boolean                         aset_newbie;
    /* To indicate if the sector is newly added into ASET and this flag
     * is used to facilitate the scheuler group management */

  boolean                         asp_idx_assigned;
    /* To indicate if the asp index has assigned; the flag is used to 
     * facilitate the set management which requests the sectors accross
     * demod carriers to be aligned accroding to the drc cover */

  boolean                         new_sect;
    /* To indicate if the sector is newly added into ASET to facilitate
       FRABSoft/QRABSoft initialization control */
#endif /* FEATURE_HDR_REVB */

#ifdef FEATURE_HDR_REVC
  uint8                           loadadj;
    /* LoadAdj in SP msg, one per sector */
#endif
}
hdrsrch_aset_sect_struct_type;


/*----------------------------------------------------------------------*
                     hdrsrch_drop_timer_struct_type
 *----------------------------------------------------------------------*/

typedef struct
{
  boolean                         active;
   /* indicates whether or not the drop timer is active on this pilot */

  boolean                         expired;
   /* Drop Timer Expired flag is used for the active set management.
      If the Expired flag is TRUE then this pilot may be demoted from
      the active/candidate set */

#ifdef FEATURE_HDR_REVB
  boolean                         reportable;
  /*  The flag decides whether the pilot report is generated from this 
      sector. In revB, only one sector/pilot from one pilot group in ASET
      is set to reportable to avoid reports generated by all pilots in 
      the same group. */
#endif /* FEATURE_HDR_REVB */

  int16                           trigger_x26ms;
   /* this variable stores the drop time trigger point in PN roll counts
      If the PN roll count exceeds the trigger count value then the timer
      is considered to be expired */

}
hdrsrch_drop_timer_struct_type;


/*----------------------------------------------------------------------*
                     hdrsrch_pil_filter_struct_type
 *----------------------------------------------------------------------*/

typedef struct
{
  uint16                          total_eng;
    /* Filtered energy for evaluating OFS condition               */
  
  uint16                          length;
    /* length of the IIR filter dedicated for OFS evaluation      */

  uint16                          time_const;
    /* IIR filter time constant                                   */

  boolean                         first_reacq_srch;
    /* whether this filtering is for the first reacq search       */

  int32                           internal_value;
    /* IIR filter internal memory                                 */

} hdrsrch_pil_filter_struct_type;


typedef struct
{
  int32                           accum;
    /* IIR filter internal memory                                 */

  uint16                          filtered_eng;
    /* Filtered energy for evaluating OFS condition               */
  
  uint16                          num_samples;
    /* number of samples used in averaging                        */
    
} hdrsrch_ofs_filter_struct_type;

/*----------------------------------------------------------------------*
                     hdrsrch_sbho_filter_struct_type
 *----------------------------------------------------------------------*/

typedef struct
{
  uint16                          total_eng;
   /* Total energy obtained by adding the best multipaths received from
      this sector */

  uint16                          length;
   /* The length of the IIR filter used for filtering the pilot energy */

}  hdrsrch_sbho_filter_struct_type;

/*----------------------------------------------------------------------*
                     hdrsrch_idho_filter_struct_type
 *----------------------------------------------------------------------*/

typedef struct
{
  uint16                          total_eng;
   /* Total energy calculated with filters */

  uint16                          length;
   /* The length of the IIR filter used for filtering the pilot energy */

  timetick_type                   measure_time_sclks;
   /* Time of last pilot energy mesurement and calculation */

}  hdrsrch_idho_filter_struct_type;

/*----------------------------------------------------------------------*
                     hdrsrch_sect_struct_type
 *----------------------------------------------------------------------*/

typedef struct
{
  hdrsrch_system_enum_type        sys_type;
    /* this enum indicates the system type (1X/DO) for this sector */
    
  hdrsrch_set_enum_type           set;
   /* This enum indicates the set type to which this particular sector
      belongs to. */

  uint8                           set_idx;
   /* Index of this sector in the set given above. */
   /* For revB, this is the sector index inside the SubASET */

#ifdef FEATURE_HDR_REVB
  hdrsrch_pilot_gid_type          pilot_gid;
   /* Pilot group idx assigned from SP, TCA, NL; If not assigned, use
    * zero as default */
#endif /* FEATURE_HDR_REVB */

  sys_pilot_pn_type               pn_offset;
   /* Pilot PN offset for this sector */

  int16                           peak_pos;
   /* Position of earliest arriving peak with sufficient energy */

  uint32                          peak_posx8_int;
   /* Interpolated earliest peak with sufficient energy */

  sys_channel_type                chan;
   /* Channel on which this sector is being transmitted */

  hdrsrch_link_id_type            link_id;
   /* Link ID for this pilot */

  hdrsrch_nset_sect_struct_type   nset;
   /* NSet parameters - when the sector is part of the Neighbour Set */

  hdrsrch_aset_sect_struct_type   aset;
   /* ASet parameters - when sector is part of the Active Set */
   /* The structure is modified for HDR revB */

  uint16                          win_sizex1;
   /* Search window size to be used for searching this pilot. This is in
      chipx1 units */

  uint16                          total_eng;
   /* Total energy obtained by adding the best multipaths received from
      this sector */

  uint16                          inst_eng;
   /* Instantaneous pilot energy */

  uint16                          inst_eng_int;
   /* Interpolated instantaneous pilot energy */

  uint16                          filt_length;
   /* length of the IIR filter used for filtering the pilot energy */

  uint16                          pri_chain_inst_eng;
   /* Instantaneous pilot energy of primary chain */

  uint16                          pri_chain_filt_eng;
   /* Filtered pilot energy of primary chain */

  uint16                          pri_chain_eng_filt_length;
   /* length of the IIR filter used for filtering the primary chain energy */

  uint16                          div_chain_inst_eng;
   /* Instantaneous pilot energy of diversity chain */

  hdrsrch_pil_filter_struct_type  pil_filter;
   /* filter for evaluating OFS conditions  */

  hdrsrch_sbho_filter_struct_type sbho_filter;
    /* Filter for Subnet handoff */

  hdrsrch_ofs_filter_struct_type  ofs_filter;
   /* filter for OFS results */

  hdrsrch_idho_filter_struct_type idho_filter;
    /* Filter for idle handoff */

  hdrsrch_drop_timer_struct_type  drop_timer;
   /* Drop timer parameters - when pilot is part of ASet or CSet */

  hdrsrch_pilot_rpt_enum_type     pilot_rpt;
   /* this enum type defines the kind of pilot report that needs to be
      sent to the upper layer as a result of set evaluation */

  boolean                         pilot_rpt_pending;
   /* this flag indicates whether the above report has yet to be sent to
      the network */

  int16                           win_cenx2;
    /* Search window center around earliest arriving multipath for ASET/CSET
       pilots */

  hdrsrch_color_code_type         color_code;
    /* Color code for a sector, if available. This is updated by the HDRRUP
       through CC update command. Used to minimize subnet handoffs*/

#ifdef FEATURE_HDR_GRICE 
  hdrsrchset_grice_cset_enum_type gcset_pilot;
    /* gRICE CSET pilot state */

  uint8                           gcset_idx;
    /* gRICE CSET index (in subaset) */
#endif /* FEATURE_HDR_GRICE */

  uint16                          scc_offset;
    /* SCC offset */

  uint8                           scc_rate;
    /* SCC rate */
}
hdrsrch_sect_struct_type;


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

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

#endif /* HDRSRCHSECT_H */

