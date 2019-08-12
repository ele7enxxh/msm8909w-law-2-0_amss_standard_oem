#ifndef __WWAN_COEX_H__
#define __WWAN_COEX_H__
/*!
  @file wwan_coex.h

  @brief
   EFS Item support for the WWAN COEX module
 
*/

/*=============================================================================

  Copyright (c) 2013-2015 Qualcomm Technologies Incorporated.
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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/inc/wwan_coex.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/09/15   sg      EFS based spur simulation support
01/08/15   rj      Increase IM Blanking table length to 12 from 5
11/19/14   jm      Include blocking as non-IM desense type
09/09/14   rj      Merged non-IM BackOff and Blanking EFS
08/13/14   rj      Disable SVLTE+G Idle Mode Hopping
07/22/14   rj      Cleanup FilterPath related BOLT feature in Lmtsmgr
06/23/14   sg      desense indication mechanism
05/02/14   rj      Filter Path support to send GSM Tx specific flag info to RF
04/28/14   jm      L+G Band Avoidance support
03/17/14   jm      WWAN coex based off of RF device assignment
02/05/14   jm      Adding enhance action type for slot level backoff support
12/12/13   jm      GPS support for DDR coex
12/09/13   jm      DDR management phase 2 support (diversity power)
11/21/13   jm      Adding DDR management support
11/16/13   rj      Adding macro prio callback support
06/13/13   rj      SVLTE/SVDO Coex for Triton/DI2.0
04/06/13   ag      Support for DSDA WWAN Coexistence
08/24/12   ag      EFS Restructuring
05/06/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "wwcoex_action_iface.h"
#include "lmtsmgr_i.h"

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/
#define MAX_DESENSE_LUT_ROWS 5
#define MAX_IM_DESENSE_LUT_ROWS 12

#define NUM_NONIM_LUT_COLUMNS   4
#define NUM_IM_LUT_COLUMNS      4
#define NUM_SEM_LUT_COLUMNS     2

#define IS_NON_IM_DESENSE(type) ((type == NOISE_DESENSE) || (type == HARMONIC_DESENSE) || (type == BLOCKING_DESENSE))
#define IS_IM_DESENSE(type) ((type == IM_TYPE1_DESENSE) || (type == IM_TYPE2_DESENSE) || (type == IM_DESENSE_GPS))

#define IS_WWAN_MITIGATION_DISABLED(mask) ((mask)&(0x1))
#define IS_DDR_MITIGATION_DISABLED(mask) ((mask)&(0x2))
#define IS_WWAN_BAND_AVOIDANCE_ENABLED(mask) ((mask)&(0x4))
#define IS_SPUR_MITIGATION_ENABLED(mask) ((mask)&(0x8))


#define WWCOEX_BA_DEFAULT_MIN_PRCT   100  /* Minimum percentage of serving frequency pairs
                                             required to declare serving frequency must be avoided */
/*=============================================================================

                             TYPEDEFS

=============================================================================*/
typedef enum
{
  /* No update to priority table */
  PRIO_TABLE_NO_CHANGE, 

  /* Set the table to MIN except the ASDIV tier */
  PRIO_TABLE_SET_MIN,   

  /* Set all tiers lower than super critical tier to MIN value */
  PRIO_TABLE_SET_MIN_EXCLUDE_SUPER_CRIT,

  /* Set the table based on macro prio index and offset */
  PRIO_TABLE_MACRO_OFFSET_BASED, 

  /* Reset the table to previous state */
  PRIO_TABLE_RESET_PREV   
}wwcoex_prio_table_upd_type;

typedef enum
{
   /*! Utilized in coexistence channel combination table to 
       determine if enhanced actions are required. */
   ENH_CXM_NONE,
   ENH_CXM_SLOT_LEVEL, /* Perform slot level backoff */
   ENH_CXM_MAX
}wwcoex_enh_action_type;


typedef struct
{
  int16 aggr_txpwr_thresh;
  int16 victim_txpwr_thresh;
  int16 victim_rxpwr_imtype1_thresh;
  int16 victim_rxpwr_imtype2_thresh;
}wwcoex_im_lut_entry_type;

typedef struct
{
  uint8 num_valid_rows;
  wwcoex_im_lut_entry_type tbl[MAX_IM_DESENSE_LUT_ROWS];
}wwcoex_im_lut_type;

typedef struct
{
  uint8   num_valid_lut_rows;

  /* Tx Power limit table for aggressor */
  int16   aggressor_limits[MAX_DESENSE_LUT_ROWS][2];
}wwan_coex_desense_lut_type;

typedef struct
{
  int16    rxpower_threshold;
  uint32   upper_threshold;   /* Time */
  uint32   lower_threshold;   /* Time */
  uint32   ddr_freq;          /* Enum */
  boolean  is_valid;
}wwcoex_ddr_threshold_table_lut_type;

typedef enum
{
   /* Chain Identifier */
   COEX_DDR_TABLE_ENTRY_PRIMARY,
   COEX_DDR_TABLE_ENTRY_DIV,
   COEX_DDR_TABLE_ENTRY_MAX
}wwcoex_ddr_threshold_table_val;

typedef enum
{
   /* Carrier Identifier */
   DDR_LUT_ENTRY_PRIMARY,
   DDR_LUT_ENTRY_SECONDARY,
   DDR_LUT_ENTRY_MAX
}wwan_ddr_coex_table_entry;

typedef struct
{
  uint32   band;
  uint32   channel_min;
  uint32   channel_max;
  wwcoex_ddr_threshold_table_lut_type ddr_threshold_table[COEX_DDR_TABLE_ENTRY_MAX];
}wwcoex_ddr_desense_lut_type;

/* All types of LUTs applicable to tech-band combination */
typedef struct
{
  boolean   is_valid;
  uint32    ddr_freq;
  /* GPS default DDR plan assumed to be 0. */
}wwcoex_ddr_desense_gps_record_type;

/* All types of LUTs applicable to tech-band combination */
typedef struct
{
  /* IM DESENSE LUT when victim is high priority */
  wwan_coex_desense_lut_type*      im_desense_hp_victim_lut;

  /* IM DESENSE LUT when victim is low priority*/
  wwan_coex_desense_lut_type*      im_desense_lp_victim_lut;

  /* GPS IM DESENSE LUT */
  wwan_coex_desense_lut_type*      gps_im_desense_lut;

  /* LUTs with IM, NON-IM and SEM blanking thresholds */
  wwcoex_im_lut_type*              im_lut;
  wwan_coex_desense_lut_type*      harmonics_lut;
  wwan_coex_desense_lut_type*      rxbn_lut;
  wwan_coex_desense_lut_type*      blocking_lut;
  wwan_coex_desense_lut_type*      sem_lut;
}wwan_coex_lut_type;

typedef struct 
{
  const char *im_backoff_fname;
  const char *gps_desense_fname;
  const char *desense_fname;
  const char *sem_fname;
} wwan_coex_efs_fname_type;

typedef struct
{
  wwan_coex_efs_fname_type     files;
  /* List of all LUTs for this band combination */
  void*                        luts;
  /* Max bands for this combination */
  uint32                       num_tech1_bands;
  uint32                       num_tech2_bands;
  /* This flag is used to avoid reading the same file more than once */
  boolean                      updated;
}wwan_coex_tech_comb_lut_type;

typedef struct
{
  const char *ddr_desense_fname;
  uint32      default_ddr_freq;

  /* List of all LUTs for this band combination */
  void*                        luts;
  /* Max bands for this tech */
  uint32                       num_tech_bands;
  wwan_coex_desense_type       desense_type;
}wwan_ddr_coex_tech_lut_type;

typedef PACK (struct)
{
  uint8    victim_priority;
  uint32   victim_band;
  uint8    aggressor_priority;
  uint32   aggressor_band;
  uint8    num_valid_lut_rows;
  int16    aggressor_limit_lut[MAX_DESENSE_LUT_ROWS][2];
}wwan_coex_efs_record_type;

typedef PACK(struct)
{
  uint32   hptech_aggr_band;
  uint32   lptech_aggr_band;
  uint8    num_valid_lut_rows;
  int16    aggressor_limit_lut[MAX_DESENSE_LUT_ROWS][2];
}wwan_coex_efs_gps_record_type;

typedef PACK(struct)
{
  uint8 num_valid_rows;
  int16 blanking_threshs[MAX_DESENSE_LUT_ROWS][NUM_NONIM_LUT_COLUMNS];
}wwan_coex_efs_nonim_lut_type;

typedef PACK(struct)
{
  uint8 num_valid_rows;
  int16 blanking_threshs[MAX_IM_DESENSE_LUT_ROWS][NUM_IM_LUT_COLUMNS];
}wwan_coex_efs_im_lut_type;

typedef PACK(struct)
{
  uint8 num_valid_rows;
  int16 blanking_threshs[MAX_DESENSE_LUT_ROWS][NUM_SEM_LUT_COLUMNS];
}wwan_coex_efs_sem_lut_type;

typedef PACK (struct)
{
  uint32   victim_band;
  uint32   aggressor_band;
  wwan_coex_efs_nonim_lut_type nonim_lut;
  wwan_coex_efs_im_lut_type    im_lut;
}wwan_coex_efs_blank_fdd_victim_type;

typedef PACK (struct)
{
  uint32   victim_band;
  uint32   aggressor_band;
  wwan_coex_efs_nonim_lut_type nonim_lut;
}wwan_coex_efs_blank_tdd_victim_type;

typedef PACK (struct)
{
  uint32   victim_band;
  uint32   aggressor_band;
  wwan_coex_efs_sem_lut_type  sem_lut;
}wwan_coex_efs_sem_record_type;

typedef PACK (struct)
{
  uint32   tech_ddr_freq[CXM_TECH_MAX];
}wwan_coex_default_ddr_freq_plan_type;


typedef PACK (struct)
{
  uint32   band;
  uint32   channel_min;
  uint32   channel_max;
  /* Primary Chain */
  int16    primary_rxpower_threshold;
  uint32   primary_upper_threshold;   /* Max Time */
  uint32   primary_lower_threshold;   /* Min Time */
  uint32   primary_ddr_freq;          /* Freq Plan Enum */
  /* Secondary Chain */
  int16    div_rxpower_threshold;
  uint32   div_upper_threshold;   /* Max Time */
  uint32   div_lower_threshold;   /* Min Time */
  uint32   div_ddr_freq;          /* Freq Plan Enum */
}wwan_coex_ddr_band_spec_freq_plan_type;

typedef PACK (struct)
{
  uint32    version;
  uint32    disable_mask;
  uint32    reserved[4]; /* reserved for future use */
}wwan_coex_efs_mitigation_mask_type;

typedef PACK (struct)
{
  uint32    version;
  uint32    enable_mask;
  uint32    reserved[4]; /* reserved for future use */
}wwan_coex_efs_mitigation_en_mask_type;

typedef PACK(struct)
{
  uint32     severe_coex_thresh; /* threshold used when coex is severe             */
  uint32     mild_coex_thresh;   /* threhsold used when coex is mild               */
  uint32     delta_t_msec;       /* Expected time between power updates in traffic */
  uint64     mild_coex_time;     /* time used when coex is mild                    */
  uint64     severe_coex_time;   /* time used when coex is severe                  */
}wwan_coex_efs_desense_ind_thresh_type;

typedef PACK(struct)
{
  int32   spur_freq_offset;      /* Spur offset from the victim LO frequency. Units: Hz      */
  int16   spur_level;            /* Spur level. Units: dbm*10                                */
  uint8   notch_depth;           /* Notch deptch from RFC spread sheet                       */
  uint8   reserved;              /* Not used */
}wwan_coex_efs_spur_sim_info_type;

typedef struct
{
  cxm_tech_type vtech;
  uint32        vband;
  cxm_tech_type atech;
  uint32        aband;
  int16         input_power;
}wwan_coex_efs_lookup_in;

typedef struct
{
  int16         aplimit;
  boolean       aplimit_valid;
}wwan_coex_efs_lookup_out;

typedef struct
{
  cxm_tech_type tech;
  uint32        band;
  wwan_ddr_coex_table_entry carrier;
  wwcoex_ddr_threshold_table_val chain;
}wwan_ddr_efs_lookup_in;

typedef struct
{
  boolean  band_spec_plan_valid;
  uint32   channel_min;
  uint32   channel_max;
  int16    rxpower_threshold;
  uint32   upper_threshold;   /* Time */
  uint32   lower_threshold;   /* Time */
  uint32   ddr_freq;          /* Enum */
  uint32   default_ddr_freq;
}wwan_ddr_efs_lookup_out;

typedef struct
{
  uint8    num_rows;
  int16    lut[MAX_DESENSE_LUT_ROWS][2];
}wwan_coex_efs_lut_out;

typedef struct
{
  cxm_tech_type vtech;
  uint32        vband;
  cxm_tech_type atech;
  uint32        aband;
  cxm_tech_type hprio_tech;
  int16         victim_rxpower;
  int16         victim_txpower;
  int16         aggr_txpower;
  wwan_coex_desense_type desense;
  cxm_action_type   action;
}wwcoex_blank_efs_lookup_in;

typedef struct
{
  int16           aplimit;
  boolean         aplimit_valid;
  cxm_action_type action;
}wwan_desense_efs_lookup_out;


typedef struct
{
  cxm_tech_type vtech;
  cxm_tech_type atech;
  coex_freqid_link_s  atulfreqinfo;
  coex_freqid_link_s  vtdlfreqinfo;
}wwcoex_nonim_freqcompare_in;

typedef struct
{
  wwan_coex_desense_type  desense;
  cxm_action_type         action;  
  uint32                  special_ind_mask;
  uint32                  enh_action_mask;
  uint32                  desense_mask; /* Bitwise-OR of all desense types */
}wwcoex_nonim_freqcompare_out;

typedef struct
{
  cxm_tech_type vtech;
  cxm_tech_type atech;
  coex_freqid_link_s  vtulfreqinfo;
  coex_freqid_link_s  atulfreqinfo;
  coex_freqid_link_s  vtdlfreqinfo;
}wwcoex_im_freqcompare_in;

typedef struct 
{
  wwan_coex_desense_type  desense;
  cxm_action_type         action; 
  uint32                  desense_mask; /* Bitwise-OR of all desense types */
}wwcoex_im_freqcompare_out;

typedef struct 
{
  cxm_tech_type tech1;
  cxm_tech_type tech2;
  uint32        t1band;
  uint32        t2band;
  int16         t1_txpwr;
  int16         t2_txpwr;
}wwcoex_sem_lookup_in;

typedef struct
{
  uint32                 bl_band;      /* Blacklisted Band */
  wwan_coex_desense_mode mode;         /* Blacklisted band is Agressor or Victim */
  uint32                 desense_mask; /* Mask of all types of desense expected
                                          for this blacklisted band */
}wwcoex_comb_info_entry;

#define WWCOEX_MAX_BL_BANDS 24
typedef struct
{
  uint32                  num_entries;                    /* Number of valid entries in list */
  wwcoex_comb_info_entry  band_list[WWCOEX_MAX_BL_BANDS]; /* List of blacklisted bands */
}wwcoex_comb_lookup_out;

typedef struct
{
  cxm_tech_type       vtech;
  cxm_tech_type       atech;
  uint8               vtech_vco;
  uint8               atech_vco;
  coex_freqid_link_s  atfreqinfo;
  coex_freqid_link_s  vtfreqinfo;
}wwcoex_spur_freqcompare_in;

typedef struct
{
  cxm_action_type         action;
  uint16                  handle;
}wwcoex_spur_freqcompare_out;

typedef struct
{
  int16  spur_level;   /* Spur level in DBM          */
  uint8  spur_type;    /* is Victim CA1 or CA2       */
  uint8  notch_depth;
  int8   a_coeff;
  int8   v_coeff;
}wwcoex_int_spur_entry_type;

typedef struct
{
  wwcoex_int_spur_entry_type  rf_entry[WWCOEX_MAX_CONCURRENT_SPURS];
  uint32  count;
}wwcoex_spur_entry_type;

typedef enum
{
  WWCOEX_TX_M,
  WWCOEX_CA1_N,
  WWCOEX_CA2_P,
  WWCOEX_GPS_Q,
  WWCOEX_MAX_COEFF 
}wwcoex_spur_coeff;

#define WWCOEX_MAX_RF_SPUR_ENTRIES 243
typedef struct
{
  uint64 band_mask;    /* Mask will be filled by MCS */
  uint16 tx_band;      /* Tx band. Use 0 if m is 0   */
  uint16 ca1_band;     /* CA1 band. Use 0 if n is 0  */
  uint16 ca2_band;     /* CA2 band. Use 0 if p is 0  */
  int16  spur_level;   /* Spur level in DBM          */
  uint8  spur_type;    /* is Victim CA1 or CA2       */
  uint8  notch_depth;  /* Notch depth                */
  int8   coeff[WWCOEX_MAX_COEFF]; /* List of coeffecients       */
}wwcoex_rf_spur_entry_type;


/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  wwan_coex_efs_init

=============================================================================*/
/*!
    @brief
    Initializes WWAN COEX EFS
 
    @return
    None
*/
/*===========================================================================*/
extern void wwan_coex_efs_init (void);


/*=============================================================================

  FUNCTION:  wwcoex_blank_efs_lookup_get_thresholds

=============================================================================*/
/*!
    @brief
    Checks the efs lookup tables for blanking thresholds.
 
    @details
    [in] input - Tx/Rx tech, band, & power info
    [in] entry_type - read Tx or Rx Thresholds
    [out] txpwr_thresh - Threshold to be updated
    [out] rxpwr_thresh - Rx Threshold to be updated
 
    @return
      TRUE - Tables exist for tech/band combination
      FALSE - Tables do not exist for tech/band combination
      txpwr_thresh & rxpwr_thresh will be updated with associated LUT
      (CXM_UNKNOWN_POWER if table does not exist or power does not meet
      thresholds)
 
*/
/*===========================================================================*/
boolean wwcoex_blank_efs_lookup_get_thresholds
(
  wwcoex_blank_efs_lookup_in*  input,       /* Aggr/Victim Tx/Rx Band & Power Info */
  wwan_coex_desense_mode       entry_type,  /* Assumes AGGR or VICTIM */
  int16                     *txpwr_thresh,  /* Tx Power read from LUT */
  int16                     *rxpwr_thresh   /* Rx Power read from LUT */
);


/*=============================================================================

  FUNCTION:  wwcoex_nonim_compare_freqs

=============================================================================*/
/*!
    @brief
    Compare frequencies for any non-IM issue like Blocking, RxBN, Harmonics
 
    @return
    TRUE
*/
/*===========================================================================*/
extern boolean wwcoex_nonim_compare_freqs
(
  wwcoex_nonim_freqcompare_in*   compare_in,
  wwcoex_nonim_freqcompare_out*  compare_out
);

/*=============================================================================

  FUNCTION:  wwcoex_im_compare_freqs

=============================================================================*/
/*!
    @brief
    Compare frequencies for checking any IM issue
 
    @return
    TRUE
*/
/*===========================================================================*/
extern boolean wwcoex_im_compare_freqs
(
  wwcoex_im_freqcompare_in*   compare_in,
  wwcoex_im_freqcompare_out*  compare_out
);

/*=============================================================================

  FUNCTION:  wwcoex_desense_efs_lookup

=============================================================================*/
/*!
    @brief
    Checks the efs lookup tables for Desense thresholds.
 
    @return
    NONE - Victim is not desensed
    UNKNOWN - Power is unknown so accurate assesment cannot be made
    BLANK - Victim is desensed, so blanking needs to be applied
*/
/*===========================================================================*/
extern boolean wwcoex_desense_efs_lookup
(
  wwcoex_blank_efs_lookup_in*  input,
  wwan_desense_efs_lookup_out* output
);

/*=============================================================================

  FUNCTION:  wwcoex_spur_compare_freqs

=============================================================================*/
/*!
    @brief
    Compare frequencies to check for spur issues between the frequencies
 
    @return
    TRUE
*/
/*===========================================================================*/
extern boolean wwcoex_spur_compare_freqs
(
  wwcoex_spur_freqcompare_in*   compare_in,
  wwcoex_spur_freqcompare_out*  compare_out
);

/*=============================================================================

  FUNCTION:  wwcoex_emission_efs_lookup

=============================================================================*/
/*!
    @brief
    Checks the efs lookup tables for emission thresholds.
 
    @return
    NO_DESENSE - No emissions issue
    SPURIOUS_EMISSIONS - emission problem

*/
/*===========================================================================*/
extern cxm_action_type wwcoex_emission_efs_lookup
(
  wwcoex_sem_lookup_in       sem_in
);

/*=============================================================================

  FUNCTION:  wwcoex_mitigation_mask_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the mitigation disable mask.
 
    @return
    None
*/
/*===========================================================================*/
extern uint32 wwcoex_mitigation_mask_efs_lookup(void);

/*=============================================================================

  FUNCTION:  wwcoex_mitigation_en_mask_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the mitigation enable mask.
 
    @details
    This is currently used by Band Avoidance.
 
    @return
    None
*/
/*===========================================================================*/
extern uint32 wwcoex_mitigation_en_mask_efs_lookup(void);

/*=============================================================================

  FUNCTION:  wwcoex_idle_mode_rxpwr_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the GSM Rx Power Limit to enable Idle Mode Hopping.
 
    @details
    This is currently used to check whether Rx Pwr Limits is met for disabling Idle Mode
    Hopping, and based on that we indicate TRM
 
    @return
    Rx Power Limit
*/
/*===========================================================================*/
int16 wwcoex_idle_mode_rxpwr_efs_lookup(void);

/*=============================================================================

  FUNCTION:  wwcoex_efs_deinit

=============================================================================*/
/*!
    @brief
    Frees all the memory associated with LUTs
 
    @return
    None
*/
/*===========================================================================*/
extern void wwcoex_efs_deinit( void );

/*=============================================================================

  FUNCTION:  wwan_coex_list_init

=============================================================================*/
/*!
    @brief
    Initialize the coex freq check list
 
    @return
    TRUE
*/
/*===========================================================================*/
extern void wwan_coex_list_init( void );

/*=============================================================================

  FUNCTION:  wwan_coex_spur_list_init

=============================================================================*/
/*!
    @brief
    Initialize the spur related entries
 
    @return
    TRUE
*/
/*===========================================================================*/
extern void wwan_coex_spur_list_init( void );

/*=============================================================================

  FUNCTION:  wwan_ddr_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the DDR plan for the band.
 
    @return
    boolean - TRUE if read was successful. FALSE otherwise.
*/
/*===========================================================================*/
boolean wwan_ddr_efs_lookup
(
  wwan_ddr_efs_lookup_in* input,
  wwan_ddr_efs_lookup_out* output
);

/*=============================================================================

  FUNCTION:  wwan_ddr_efs_lookup_gps

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the DDR plan specific to GPS
 
    @return
    TRUE - EFS entry for GPS 
    FALSE - 
*/
/*===========================================================================*/
boolean wwan_ddr_efs_lookup_gps
(
  uint32* ddr_freq_plan
);

/*=============================================================================

  FUNCTION:  wwcoex_check_tech_pair_im

=============================================================================*/
/*!
    @brief
    Checks if IM desense exists for tech pair
 
    @return
    TRUE
 
*/
/*===========================================================================*/
boolean wwcoex_check_tech_pair_im
(
  cxm_tech_type tech1,
  cxm_tech_type tech2
);

/*=============================================================================

  FUNCTION:  cxm_update_prio_table

=============================================================================*/
/*!
    @brief
    update client's prio_table to respective clients.
 
    @return
    None
*/
/*===========================================================================*/
void cxm_update_prio_table
(
  cxm_tech_type       tech_id,
  uint8  prio,
  uint8  curr_offset,
  wwcoex_prio_table_upd_type prio_upd_action
);


/*=============================================================================

  FUNCTION:  cxm_update_prio_table_init

=============================================================================*/
/*!
    @brief
    Init client's prio table info.
 
    @return
    None
*/
/*===========================================================================*/
void cxm_update_prio_table_init( void );


/*=============================================================================

  FUNCTION:  wwcoex_get_mitigation_action

=============================================================================*/
/*!
    @brief
    Find Mitigation action based on the mode of operation of device DSDA/SGLTE+G/SGTDS+G
 
    @return
      desense type
*/
/*===========================================================================*/
cxm_action_type wwcoex_get_mitigation_action
( 
  cxm_tech_type a_tech,
  cxm_tech_type v_tech
);

/*=============================================================================

  FUNCTION:  wwcoex_is_filtered_path_combo

=============================================================================*/
/*!
    @brief
    Checks if the current frequency reuiqres a filter path.
 
    @return
    TRUE
 
*/
/*===========================================================================*/
uint32 wwcoex_is_filtered_path_combo
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vdl_info
);

/*=============================================================================

  FUNCTION:  wwcoex_get_all_desense_combs

=============================================================================*/
/*!
    @brief
    Generates a list of bands (and their associated desense issues)
    which have desense issues with the non-blacklisted tech
 
    @param
      bl_tech - Blacklisted Tech
      nb_tech - Non-Blacklisted Tech
      nb_band - Non-Blacklisted Tech's Band
      output  - Pointer to blacklisted band list
    @details

    @return
     output will be filled with the list of blacklisted bands
     and their potential desense issues

*/
/*===========================================================================*/
void wwcoex_get_all_desense_combs
(
  cxm_tech_type bl_tech,
  cxm_tech_type nb_tech,
  uint32 nb_band,
  wwcoex_comb_lookup_out *output
);

/*=============================================================================

  FUNCTION:  wwcoex_gen_freq_range

=============================================================================*/
/*!
    @brief
    Generates the victim's frequency range that has any desense issue
    based on frequency information
 
    @details
      - Desense types Blocking & PA Noise will provide full frequency range
      - Other desense types will be calculated based on respective equations
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_gen_freq_range
(
   cxm_tech_type a_tech,                    /* Agressor Tech */
   cxm_tech_type v_tech,                    /* Victim Tech */
   lmtsmgr_ba_serv_info_type *aul_freqInfo, /* Agresssor Tx Freq Info */
   lmtsmgr_ba_serv_info_type *vul_freqInfo, /* Victim Tx Freq Info */
   lmtsmgr_ba_serv_info_type *vdl_freqInfo, /* Victim Rx Freq Info */
   wwan_coex_desense_type type,             /* Desense Type */
   wwan_coex_desense_mode mode,             /* Calculate for Aggr (Tx) or Victim (Rx)*/
   uint32 *freq_lower,                      /* Frequency Range (kHz) */
   uint32 *freq_upper
);

/*=============================================================================

  FUNCTION:  wwcoex_init_desense_stat

=============================================================================*/
/*!
    @brief
    Initialize the desense statistics
 
    @return
    TRUE
*/
/*===========================================================================*/
void wwcoex_init_desense_stat(void);

/*=============================================================================

  FUNCTION:  wwcoex_process_desense_timeout

=============================================================================*/
/*!
    @brief
    Proces the desense timer expiry
 
    @return
    TRUE
*/
/*===========================================================================*/
void wwcoex_process_desense_timeout(void);


/*=============================================================================

  FUNCTION:  wwan_coex_read_prio_update_efs

=============================================================================*/
/*!
    @brief
    Reads the priority update EFS file and return the value stored in it.
 
    @details
    This EFS file tells coex manager whether the super critical tiers need
    to be retained or not while updating priority tables
 
    @return
    None
*/
/*===========================================================================*/
uint32 wwan_coex_read_prio_update_efs(void);

/*=============================================================================

  FUNCTION:  wwan_coex_read_desense_ind_efs

=============================================================================*/
/*!
    @brief
    Reads the desense indication EFS and stores the thresholds 
 
    @details
    This EFS file tells coex manager about the thresholds for triggering
    desense indication and timers values to be used for desense indication
 
    @return
    None
*/
/*===========================================================================*/
void wwan_coex_read_desense_ind_thresh_efs(void);

#endif /* __WWAN_COEX_H__ */
