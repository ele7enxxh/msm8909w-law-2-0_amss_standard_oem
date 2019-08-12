#ifndef RFLTE_MC_EXT_H
#define RFLTE_MC_EXT_H

/*
   @file
   rflte_ext_mc.h

   @brief
   RF LTE Driver's external interface file.

   @details

*/

/*===========================================================================
Copyright (c) 2002 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/lte/rflte_ext_mc.h#1 $
$DateTime: 2016/12/13 07:59:52 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/22/16   cv      Added support for 256 lte band mask check
11/26/15   lhb     FR 31436: API to provide the HW band limitation to LTE RRC
11/19/15   as      Reduce max cell supported to 3 for Jolokia
10/20/14   st      RRC interface for Gapless Support BandMask
09/15/14   kab     Modify RF API for RRC integration
09/15/14   kab     Inform RRC about LTE CA Band Combo Support 
08/12/14    vb     NLIC clock mode support
08/12/14   jf      MCL LTE support
07/31/14   ndb     Added support for extended EARFCN for LTE
07/18/14   jf      Return TX power in linear region to ML1
07/08/14   gvn     API for RRC to query split band channel raster
05/01/14   bsh     NLIC status to ML1
03/28/14   svi     Expose Intra Band CA capabilities to RRC
02/28/14   php     Add APIs for Light Sleep/Wakeup 
10/28/13   npi     Define LTE cell type enum
05/07/13   cri     Export temp comp scaling API
01/17/13   gvn     Expose some rflte_core_txpl APIs which are used by MCS team 
01/15/13   gvn     Fix compiler error 
01/04/13   gvn     Move interface to query RF LTE CA bc config from rfm to rflte_mc 
12/28/12   gvn     Initial Check-in

============================================================================*/

#include "rfcom.h"
#include "sys.h"
#include "rfm_device_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define rflte_mc_rrc_max_partial_band  10
/*!
   @brief
   Enumeration for LTE Cells.

*/
typedef enum {
  RFLTE_MC_PCELL_IDX   = 0x00000000,
  RFLTE_MC_SCELL1_IDX  = 1,
  RFLTE_MC_SCELL2_IDX  = 2,
  RFLTE_MC_MAX_CELL_IDX_SUPPORTED
} rflte_mc_cell_idx_type;

/*!
   @brief
   Enumeration for NLIC clock modes
*/
 typedef enum { 
     /* mapped to 285300 khz */
     RFLTE_MC_NLIC_CLOCK_MODE_MCVS_SVS2  = 0,

     /* mapped to 499200 khz */
     RFLTE_MC_NLIC_CLOCK_MODE_MCVS_SVS   = 1,

     /* mapped to 665600 khz */
     RFLTE_MC_NLIC_CLOCK_MODE_MCVS_NOM   = 2,

     /* mapped to 768000 khz */
     RFLTE_MC_NLIC_CLOCK_MODE_MCVS_TURBO = 3,

     RFLTE_MC_NLIC_CLOCK_MODE_MAX_MODE, 

     RFLTE_MC_NLIC_CLOCK_MODE_INVALID 
 }rflte_mc_nlic_clock_mode_type;

typedef enum
{
  RFLTE_MC_EXT_CARR_TYPE_PRX = 0,
  RFLTE_MC_EXT_CARR_TYPE_DRX,
  RFLTE_MC_EXT_CARR_TYPE_INVALID = 0xFF
} rflte_mc_ext_carrier_type;

typedef struct 
{
    rfcom_lte_earfcn_type              pcc_tx_chan;
    rfcom_lte_earfcn_type              scc_rx_chan;
    rfcom_lte_bw_type   pcc_ul_bw;          /*PCELL UL BW*/ 
    rfcom_lte_bw_type   scc_dl_bw;          /*SCELL Rx BW*/ 
} nlic_status_input_params;

typedef struct 
{
    boolean                 nlic_possible;
    uint8                   victim_id;
} nlic_status_return_params;

typedef struct
{
   uint8 sf_num;
} rflte_mc_linear_tx_power_input_s;

typedef struct
{
   uint32 tx_power;
   uint32 tx_power_srs;
} rflte_mc_linear_tx_power_output_s;

/*!
   @brief
   CA Info per LTE CA Band combo - shared with RRC.

*/
typedef struct 
{ 
  /* Number of bands in a given CA Combo.*/
   uint8 num_bands_per_combo; 
   
   /*Band Info - Bit[0]Pcell + Bit[1]SCELL1 + Bit[2]SCELL2 + Bit[3]SCELL3 + Bit[4]SCELL4. */
   uint8 band[5];             

   /*Per Cell bw support.Bit[0] -> InterBand CA,Bit[1] -> IntraBand CA(20MHz),Bit[2] -> IntraBand CA(40MHz)*/
   uint8 bw_class[5];         
   
   /*UL Support - Bit[0]Pcell + Bit[1]SCELL1 + Bit[2]SCELL2 + Bit[3]SCELL3 + Bit[4]SCELL4.*/
   uint8 ul_supported; 

} rflte_mc_rrc_ca_info_per_band_combo_s; 

/*!
   @brief
   Structure to hold LTEE CA band combos - shared with RRC.

*/
typedef struct 
{ 
  /* Number of Supported CA band combos*/ 
   uint8 num_combos;  
                               
   /* As per ASNI, 36331 spec at max we can only support 128 combos */
   rflte_mc_rrc_ca_info_per_band_combo_s band_combo[128]; 

} rflte_mc_rrc_ca_band_combos_s;

 typedef struct 
 {
   uint64 pcell_band;
   uint64 gapless_support_permitted_mask;
 }rflte_rrc_gapless_support_cfg_t;


/*
@brief
  Structure to store band range information for partial band ,shared with RRC
*/

typedef struct 
{ 
  uint32  band;
  uint32  start_dl_earfcn;  
  uint32  stop_dl_earfcn;
}rflte_mc_rrc_band_range_info_s;


/*
@brief
 Struture to store all partial band informaton got from RFC, share with RRC

*/

typedef struct 
{ 
  /* Number of partial band*/ 
   uint16 num_partial_bands;  
                               
   /* partial band information*/
   rflte_mc_rrc_band_range_info_s  lte_band_range[rflte_mc_rrc_max_partial_band];

}rflte_mc_rrc_partial_band_info_s;
 /*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

boolean rflte_mc_tx_coex_wlan_set_txplim(int16 coex_limit_db10) ;

#ifdef LTE_BAND_NUM
sys_lte_band_mask_e_type rflte_mc_get_lte_ca_band_mask
(
  rfcom_lte_band_type pcell_band
); 
#else
uint64 rflte_mc_get_lte_ca_band_mask(rfcom_lte_band_type pcell_band);
#endif 

void rflte_mc_get_lte_ca_gapless_support_band_mask(rflte_rrc_gapless_support_cfg_t *gapless_support_mask);


uint32 rflte_core_get_tx_freq_from_uarfcn(rfcom_lte_earfcn_type tx_chan, rfcom_lte_band_type band);

uint32 rflte_core_get_rx_freq_from_uarfcn(rfcom_lte_earfcn_type rx_chan, rfcom_lte_band_type band);

uint16 rflte_core_get_bw_in_khz(rfcom_lte_bw_type bw);

uint16 rflte_core_temp_comp_raw_to_scaled(uint16 therm_raw_value);

void rflte_mc_light_sleep(void);

void rflte_mc_light_wakeup(void);

uint8 rflte_mc_single_rx_ca_get_intra_band_ca_support(rfcom_lte_band_type band);

void rflte_mc_get_nlic_status(nlic_status_input_params *nlic_input_params, nlic_status_return_params *nlic_return_params);

void rflte_mc_get_split_rx_band_channel_raster(rfcom_lte_band_type split_band, 
                                               rfcom_lte_earfcn_type *low_earfcn,
                                               rfcom_lte_earfcn_type *high_earfcn,
                                               uint8 *num_of_splits);

void rflte_mc_fed_get_linear_tx_power(rflte_mc_linear_tx_power_input_s* tx_power_input,
                                      rflte_mc_linear_tx_power_output_s* tx_power_output);

boolean rflte_mc_get_tuner_scenario_list(rfm_device_enum_type device,
                                         rflte_mc_ext_carrier_type carrier_type,
                                         uint32 channel,
                                         rfm_antenna_tuner_scenario_data *scenario_data,
                                         uint8 *free_space_index,
                                         rfm_tuner_handle_type *tuner_handle);

boolean rflte_mc_tuner_stop_mcl(rfm_device_enum_type device,
                                rflte_mc_ext_carrier_type carrier_type,
                                uint32 channel,
                                rfm_tuner_handle_type tuner_handle);

void rflte_mc_get_lte_ca_band_combo_support(rflte_mc_rrc_ca_band_combos_s *ca_band_combo);

void rfm_lte_get_partial_band_range_info(rflte_mc_rrc_partial_band_info_s* partial_band_info);
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RFLTE_MC_EXT_H */

