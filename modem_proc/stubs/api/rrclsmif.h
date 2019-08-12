#ifndef RRCLS_H
#define RRCLS_H


/*============================================================================

FILE:      rrclsmif.h

DESCRIPTION:
  This file stubs two wcdma api structs for PE playback.

PUBLIC CLASSES:
  <none>

============================================================================
        Copyright © 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/


typedef struct
{
  uint16 cell_id;
} rrcmeas_plmn_cell_info_struct_type;


typedef struct
{
  uint16 cell_id;
}rrc_cell_id_status_type  ;

typedef enum
{
  WCDMA_CLASSMARK_UE_CAPABILITY_NETWORK_BASED = 0,
  WCDMA_CLASSMARK_UE_CAPABILITY_UE_BASED,
  WCDMA_CLASSMARK_UE_CAPABILITY_BOTH,
  WCDMA_CLASSMARK_UE_CAPABILITY_NONE
} network_assisted_gps_support_enum_type;

typedef struct
{
  boolean standalone_location_method_supported;
  boolean ue_based_otdoa_supported;
  network_assisted_gps_support_enum_type  network_assisted_gps_support;
  boolean gps_timing_of_cell_frames_supported;
  boolean ipdl_supported;
  boolean rx_tx_type2_supported;
  boolean up_meas_validity_cell_pch_and_ura_pch;
  boolean lcs_value_added_lcs_supported;
} tm_umts_cp_wcdma_ue_pos_capability_struct_type;


#endif /* RRCLS_H */