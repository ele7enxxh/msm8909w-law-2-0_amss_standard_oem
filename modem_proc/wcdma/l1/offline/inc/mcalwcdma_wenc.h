#ifndef __MCALWCDMA_WENC_H__
#define __MCALWCDMA_WENC_H__
/*============================================================================
                        M C A L W C D M A _ W E N C. H
DESCRIPTION
  This module has definition and declaration related to MCAL (modem core
  abstraction layer) W Encoder APIs.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_wenc.h#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
02/02/15   cjz    SW changes to bringup DC-HSUPA on Tabasco RUMI
11/10/14   tsk    Mainline code under feature FEATURE_WCDMA_JOLOKIA_MODEM on Jolokia branch.
06/11/14   tsk    Fixes for Jolokia first CRM 1.0.
06/08/14   ar     Added FTM support for EUL
06/03/14   tsk    Compilation fixes for JOLOKIA 1.0
05/22/14   tsk    Featurization for JOLOKIA 1.0
03/31/14   ar     Modified ERAM/RMRAM readback code and added functionality to output to diag
02/14/14   gp     Enhanced HW readback logging routines
02/14/14   ar     Porting Bolt specific uplink changes on to Bolt 2.0
01/23/14   ash    SW workaround for Resetting TXC0 and TXR0
07/10/13   ar     Initial version : Modem core abstraction layer changes 

==============================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "wenc.h"
#include "ulstates.h"
#include "l1rrcif.h"
#include "ul.h"
#include "l1macdata.h"
#include "wenci.h"
#include "l1utils.h"
#include "eulenc.h"

#include "comdef.h"
#include "msmhwio.h"
#include "msmhwioreg.h"

#ifdef FEATURE_WCDMA_L1_TM_CIPHER_SUPPORTED
#include "msm.h"
#endif

/*=========================================================================*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*=========================================================================*/

#define WRITE_ACTIVE_AND_ERAMA_TRCH_TO_HW(num_actve_trch, num_erama_trch) HWIO_OUT(TX_UNIFIED_ENC_NUM_ACTIVE_TRCHS, \
             (num_actve_trch) << HWIO_TX_UNIFIED_ENC_NUM_ACTIVE_TRCHS_NUM_ACTIVE_TRCHS_CCTRCH1_SHFT | \
              (num_erama_trch) << HWIO_TX_UNIFIED_ENC_NUM_ACTIVE_TRCHS_NUM_ERAMA_TRCHS_SHFT \
            )

/* On MSM6280 SF are from 2 to 256 with its register value 0 to 7,
   however in software SF enum definition is from SF4 to SF256
   therefore SF doesn't map directly to H/W register value. Need
   adjustment of 1 to match it */

#define WRITE_DPDCH_CTL_TO_HW(dpdch_enable, sf, ovsf) HWIO_OUT(TX_UMTS_DPDCH_CTL,	\
              (dpdch_enable) << HWIO_TX_UMTS_DPDCH_CTL_EN_SHFT | \
              ((uint8)sf + 1)  << HWIO_TX_UMTS_DPDCH_CTL_SPR_FACTOR_SHFT | \
              (ovsf) << HWIO_TX_UMTS_DPDCH_CTL_OVSF_SHFT \
            )

#define EULENC_ERAM_MEMORY_BANK_TTI_2MS_BMSK   0x00
#define EULENC_ERAM_MEMORY_BANK_TTI_10MS_BMSK  0x08

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* Transport Channel Config 0 RMRAM rate matching params struct  */
typedef struct
{
  uint8 trch_idx;
  uint16 tb_size;
  uint8 num_tb;
  uint16 eramb_start_addr;
  uint8 crc;
} mcalwcdma_wenc_write_trch_cfg0_ram_struct_type;

/* Transport Channel Config 1 RMRAM rate matching params struct */
typedef struct {
    uint8 trch_idx;
    ul_rm_parms_struct_type ul_rm_parms;
    ul_trch_coding_struct_type ul_trch_coding_parms;
    ul_trch_turbo_ileav_parms_type ul_trch_turbo_ileav_parms;
    uint16 eramb_start_addr;
    uint8 tti_enc_val;
    l1_ul_coding_enum_type cctrch_coding_type;
} mcalwcdma_wenc_write_trch_cfg1_ram_struct_type;

/*===========================================================================
FUNCTION        mcalwcdma_wenc_dpch_state_setup

DESCRIPTION     This function writes the Scrambling code, channel type 
                and endianness during DPCH setup. 

DEPENDENCIES    Inputs : Scrambling code, scrambling code type, phy chan type

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_dpch_state_setup(uint32 scrambling_code, 
   l1_ul_dpch_sc_enum_type  scrambling_code_type, uint8 chan);

/*===========================================================================
FUNCTION        mcalwcdma_wenc_dpch_tx_init

DESCRIPTION     This function is used to initialize DPCH parameters. 
                We program  TX_MODE_COMMON with 0x7D to keep tx HW in UMTS
                mode, and to enable all the clocks for each Tx Blocks,
                namely
 
                1. Tx Mem
                2. Tx Control
                3. Tx Encoder
                4. Tx Modulator
                5. Tx Filter

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_dpch_tx_init( void );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_erama_write_init

DESCRIPTION     In this function we initialize the parameters for writing to 
                ERAMA every TTI.
                1. Choose the Memory mapping (Using HW Default values, by prog 0)
                2. Program the Phychan type
                3. Program number of Active trchs and ERAMA trchs
                4. Program filler polarity
                5. Program endianness of data when written to ERAMA (Little End)

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean mcalwcdma_wenc_erama_write_init(
   uint8 chan,
   uint8 num_active_trchs,
   uint8 num_erama_trchs,
   uint8 filler_val);


/*===========================================================================
FUNCTION        mcalwcdma_wenc_calc_last_row_col_for_interleaving

DESCRIPTION     This function calculates the last row and column in RMRAM 
                of the bits written to RMRAM.
                Last Row = (Number of Bits - 1) / 30
                Last Column = (Number of Bits - 1) % 30
 
                Number of bits can change based on Spreading Factor and whether
                or not Compressed Mode is active

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
uint32 mcalwcdma_wenc_calc_last_row_col_for_interleaving(
   uint8 num_slots_txed,
   l1_sf_enum_type sf
   );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_phychan_start_encoding

DESCRIPTION     This function is used to trigger R99 encoding. 
                It programs which frame in the TTI is being programmed
                and the last row and column of data in RMRAM, before trigerring
                encoding.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_phychan_start_encoding(
   ul_state_enum_type wl1_ul_state,
   l1_tti_enum_type prach_tti,
   uint8 nrf,
   uint32 dch_rach_last_row_col_req_val,
   l1_tti_enum_type longest_active_tti
 );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_copy_rmram0_to_rmram1

DESCRIPTION     This function will copy the rate matched data from RMRAM0 to 
                RMRAM1 

DEPENDENCIES    Spreading factor is required to determine number of bytes to 
                transfer

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_copy_rmram0_to_rmram1(uint8 spreading_factor, uint8 num_cm_slots_txed);

/*===========================================================================
FUNCTION        mcalwcdma_wenc_debug_log_registers

DESCRIPTION     This function will use the HW SW register debug interface 
                to log changes to the list of registers provided. 
 
DEPENDENCIES    None

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_debug_log_registers_start(uint32 reg_addr[8], uint8 list_len);

/*===========================================================================
FUNCTION        mcalwcdma_wenc_debug_log_registers_end

DESCRIPTION     This function will use the HW SW register debug interface 
                to log changes to the list of registers provided. 
 
DEPENDENCIES    None

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_debug_log_registers_end(void);

/*===========================================================================
FUNCTION        mcalwcdma_wenc_phychan_tx_init

DESCRIPTION     This function programs phychan parameters, namely 
                1. Enable DPDCH or not
                2. Spreading Factor
                3. OVSF
                4. Scrambling code and Scrambling code type

DEPENDENCIES    Input : OVSF, Spreading factor, scrambling code, 
                        scrambling code type, phychan type

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_phychan_tx_init(
   uint16 dpch_ovsf,
   l1_sf_enum_type sf,
   uint32 prach_sc,
   l1_ul_dpch_sc_enum_type  dpch_scrambling_code_type,
   uint32 dpch_scrambling_code,
   l1_ul_phychan_enum_type ul_phychan
   );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_prach_signal_tx

DESCRIPTION     This function basically sets phychan type to PRACH and sets 
                endianness. 

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_prach_signal_tx( void );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_set_endianness

DESCRIPTION     This function sets endianness for the read back format of data 
                in ERAM. This needs to be set to 1 when accessing other TxMEM
                regions.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_set_endianness( void );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_tx_abort

DESCRIPTION     This function programs phychan type to '0' to abort any tx 
                operation. 

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_tx_abort( void );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_wl1_enc_init

DESCRIPTION     This function is called during wl1_enc_init. 
                It initializes ERAM parameters and HS-DPCCH OVSF. 

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_wl1_enc_init(void);

/*===========================================================================
FUNCTION        mcalwcdma_wenc_write_erama_data

DESCRIPTION     This function performs actual data writes to HW. 
                It first write Ciphering control words
                Second writes mac header data
                Finally writes the transport block data

DEPENDENCIES    Inputs : Transport channel index, transport channel data, 
                         transport block size, ciphering mode, total bits to be
                         ciphered

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean mcalwcdma_wenc_write_erama_data
(
   uint8 trch_idx, l1_ul_tb_data_type *trch_data, uint16 tb_size, uint32 ciphering_mode, uint16 total_bits_to_cipher
   );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_write_scrambling_code

DESCRIPTION     This function writes scrambling code to HW by calling API 
                provided by HW team. 

DEPENDENCIES    Inputs : Scrambling code, scrambling code type

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_write_scrambling_code(
   uint32 scrambling_code,
   l1_ul_dpch_sc_enum_type  scrambling_code_type);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================
FUNCTION        mcalwcdma_wenc_write_scrambling_code

DESCRIPTION     This function writes scrambling code to HW for 2nd carrier by calling API
                provided by HW team.

DEPENDENCIES    Inputs : Scrambling code, scrambling code type

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void mcalwcdma_wenc_write_scrambling_code_sec(
   uint32 scrambling_code,
   l1_ul_dpch_sc_enum_type  scrambling_code_type);
#endif /* FEATURE_WCDMA_DC_HSUPA */

/*===========================================================================
FUNCTION        mcalwcdma_wenc_write_trch_cfg0_ram_rach_dup

DESCRIPTION     This function writes the transport channel CRC parameters to 
                config ram for the 2nd frame of 20 ms TTI RACH message.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_write_trch_cfg0_ram_rach_dup( mcalwcdma_wenc_write_trch_cfg0_ram_struct_type *trch_cfg0_ram_struct );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_write_trch_cfg1_ram

DESCRIPTION     This function writes rate matching, encoding and interleaving 
                parameters to  config ram for DPDCH and PRACH message. 

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_write_trch_cfg1_ram
(
   mcalwcdma_wenc_write_trch_cfg1_ram_struct_type *mcalwcdma_wenc_write_trch_cfg1_ram_val
   );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_write_trch_cfg1_ram_rach_dup

DESCRIPTION     This function writes rate matching, encoding and interleaving 
                parameters to  config ram for 2nd frame of 20ms TTI RACH  message

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_write_trch_cfg1_ram_rach_dup (
  mcalwcdma_wenc_write_trch_cfg1_ram_struct_type *trch_cfg1_ram_struct
);

/*===========================================================================
FUNCTION        mcalwcdma_wenc_process_phychan_cfg_hsdpcch

DESCRIPTION     This function writes HS-DPCCH OVSF code to HW

DEPENDENCIES    Inputs : Number of DPDCH active 

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_process_phychan_cfg_hsdpcch(uint8 num_dpdch);

/*===========================================================================
FUNCTION        mcalwcdma_wenc_hsrach_process_tx_data_init

DESCRIPTION     This function initializes some HW parameters for HS-RACH, 
                such as phychan type, endianness and Tx Mode

DEPENDENCIES     

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_hsrach_process_tx_data_init(void);

/*===========================================================================
FUNCTION        mcalwcdma_wenc_hsrach_process_tx_data

DESCRIPTION     This function programs basic parameters while HS_RACH 
                is in progress, such as ERAMA write pointer reinitialization,
                endianness control and phychan type configuration

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_hsrach_process_tx_data(void);

/*===========================================================================
FUNCTION        mcalwcdma_wenc_wl1ulhsdpchmgr_activate_init

DESCRIPTION     This function performs HW register writes during the activation 
                of DPCH channels during FACH state as part of HS RACH 

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_wl1ulhsdpchmgr_activate_init(void);

/*===========================================================================

FUNCTION    tx_umts_hsdpcch_ovsf_cfg

DESCRIPTION 

===========================================================================*/

void mcalwcdma_wenc_write_hsdpcch_ovsf
     (
       uint8  ovsf
     );

/*===========================================================================

FUNCTION    tx_umts_trch_mux_order_cfg

DESCRIPTION transport channel order for R99 encoding

===========================================================================*/

void mcalwcdma_wenc_write_mux_order
     (
      uint8			trch_index,          //trch processing order
      uint16			eramb_rd_start_addr  //eramb read for the corresponding trch index
     );

/*===========================================================================

FUNCTION    tx_umts_scr_cfg

DESCRIPTION 

===========================================================================*/

void mcalwcdma_wenc_scrambling_config
     (
        uint32  scr_code_init,
        l1_ul_dpch_sc_enum_type scr_code_type,
        uint32     pn_load
     );

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================
FUNCTION    tx_umts_scr_cfg for 2nd carrier

DESCRIPTION

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS
===========================================================================*/
void mcalwcdma_wenc_scrambling_config_sec
     (
        uint32  scr_code_init,
        l1_ul_dpch_sc_enum_type scr_code_type,
        uint32     pn_load
     );
#endif /*FEATURE_WCDMA_DC_HSUPA*/

/*===========================================================================

FUNCTION    tx_umts_phy_ch_type_cfg

DESCRIPTION SW can program this register when required

===========================================================================*/

void mcalwcdma_wenc_write_phychan_type
     (
      l1_ul_dpch_sc_enum_type  ch_type
     );

/*===========================================================================

FUNCTION    tx_umts_sw_trch_params_cfg

DESCRIPTION turbo interleaver parameters for R99

===========================================================================*/
void mcalwcdma_wenc_write_crc_params
     (
      uint8   trch_idx,                  //transport ch index
      uint16   crc_trblk_size,
      uint8     crc_size,
      uint8     crc_num_trblks,
      uint16     crc_eramb_wr_start_addr
     );

void mcalwcdma_wenc_write_trch_coding_params
     (
        uint8   trch_idx,
        uint32  enc_num_pre_rm_bits,   
        uint8     enc_trch_tti,          
        uint8     enc_num_code_segs,     
        l1_ul_coding_enum_type     enc_coding_rate_type,  
        boolean     enc_puncture_flag,     
        uint32     enc_num_filler_bits,   
        uint32     enc_num_bits_code_seg 
     );

/*===========================================================================

FUNCTION    tx_umts_sw_rm_params_cfg

DESCRIPTION programming rate matching params for R99

===========================================================================*/
void mcalwcdma_wenc_write_rm_parms
     (
       uint8   trch_idx,   //transport ch index
       uint32  einit2,  
       uint32  einit1,  
       uint32  eminus2, 
       uint32  eminus1, 
       uint32  eplus2, 
       uint32  eplus1
     );

/*===========================================================================

FUNCTION    tx_umts_sw_tintlv_params_cfg

DESCRIPTION turbo interleaver parameters for R99

===========================================================================*/
void mcalwcdma_wenc_write_turbo_intlv_params
     (
      uint8   trch_idx,                  //transport ch index
      uint16  enc_num_columns_minus1,  
      uint16  enc_last_column,         
      uint16  enc_num_row_index,       
      uint16  enc_last_row_bit_exch,   
      uint16  enc_last_row,            
      uint16  enc_prime_num_index, 
      uint16  enc_prime_number
     );

/*===========================================================================

FUNCTION    tx_umts_sw_cipher_keys_cfg

DESCRIPTION programming the ciphering keys required for R99

===========================================================================*/
void mcalwcdma_wenc_write_ciphering_keys
     (
       uint8   cipher_key_num,   //specifies the cipher key# from 0-5
       uint32  cipher_key[4]     //128 bit cipher key
     );

/*===========================================================================
FUNCTION        mcalwcdma_wenc_write_eul_tti_bank_idx

DESCRIPTION     This function writes the EUL ERAM bank index and TTI value to 
                HW and reinitializes the EUL INCRACC write location in EUL ERAM

DEPENDENCIES    Inputs : Carrier Index, Harq bank index, EUL TTI

RETURN VALUE    returns the value written to TX_EUL_ERAM_MEMORY_BANK HW register

SIDE EFFECTS    1) Updates HW register for HARQ Memory Bank 
                2) Reinitializes EUL INCRACC write address
===========================================================================*/
uint32 mcalwcdma_wenc_write_eul_tti_bank_idx(wl1_ul_carr_id_enum_type carr_idx,
                                             uint32 eram_bank_idx, e_tti_enum_type tti);

#if defined(DEBUG_ERAMA_RMRAM_DATA_DUMP) && defined(FEATURE_WCDMA_16_QAM)
/*===========================================================================
FUNCTION        mcalwcdma_wenc_eram_iram_data_dump

DESCRIPTION     This function reads Tx mem and stores the eram and iram 
                contents in  'mcalwcdma_wenc_data_dump'
                The TX Mem is organized into 15 pages of size 1024 words each.
                To read from Tx Mem we simply cycle through all the pages
                based on bitmask provided.

DEPENDENCIES    None

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void mcalwcdma_wenc_eram_iram_data_dump(uint16 bitmask, uint8 new_or_retx);

#endif /* DEBUG_ERAMA_RMRAM_DATA_DUMP */

#endif /* __MCALWCDMA_WENC_H__ */
