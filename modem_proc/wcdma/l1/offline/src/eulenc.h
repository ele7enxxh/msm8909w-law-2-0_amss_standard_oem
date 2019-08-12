#ifndef EULENC_H
#define EULENC_H

/*==========================================================================
             WCDMA L1 HSUPA UL ENC driver related header file

DESCRIPTION
  This file contains definition, declaration required for HSUPA UL ENC driver
  
  Copyright (c) 2006 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/eulenc.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/15   ssv     De-initializeing A2 only if it was initialized.
02/02/15   cjz     SW changes to bringup DC-HSUPA on Tabasco RUMI
11/10/14   tsk     Mainline code under feature FEATURE_WCDMA_JOLOKIA_MODEM on Jolokia branch.
07/28/14   pkg     Code Check-in for 16QAM feature.
06/03/14   tsk     Compilation fixes for JOLOKIA 1.0
05/31/14   vr      Converting large local to dynamically allocated variable to avoid
                   stack overflow.
03/31/14   ar      Removed Dime ERAM readback functions from Bolt
02/14/14   ar      Porting Bolt specific uplink changes on to Bolt 2.0
12/17/13   ash     Removed A2 Done ISR as well as A2 Done Sem Logic
07/01/13   at      Remove HARQ buffer_avail flag and related error fatal.
06/12/13   at      Set SW HARQ memory DSM pointers to NULL during EUL init and after dsm free.
05/22/13   oh      Adding DC-HSUPA framework support
05/06/13   at      SW Workaround for TXHW-A2 issue and cleanup FEATURE_WCDMA_NIKEL_ERAMA_ACCESS_FIX
01/08/13   at      Changes for HARQ memory readback with A2 using DSM item (which
                   is cache aligned). Removed temporary hack for HAQR_MEM crash. 
08/21/12   at      Changes for Dime Rumi bring-up for EUL ERAM HARQ memory reduction
08/10/12   at      EUL ERAM HARQ memory reduction
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/19/12   ksr     L2 - MAC-I and HS-RACH code changes
02/01/12   gnk     Nikel feature cleanup
01/31/12   raj     feature cleanup
01/31/12   raj     Nikel feature cleanup.
02/23/11   asm     Cleanup of unused, mainlined or irrelevant features in offline UL code
11/23/10   ksr     UL Ciphering Keys for A2 is updated
09/15/10   ksr     Enhanced A2 logics with ERAM Readback, L1-L2 interace contents
                   dumping, Rd back profiling etc
08/12/10   ksr     Initialize A2 during EUL Cfg and de-initialize during EUL release
07/16/10   ksr     A2 EUL Enhancements
06/21/10   ksr     Compiler warning fixes
02/10/10   asm     Code-merge to mainline
11/16/09   asm     Added EUL Datapath support with A2 and Removed P2-related changes
08/28/09   gv      Checkin EUL PP changes protected within the feature
                   DEBUG_FEATURE_WCDMA_PP_HW_UL
06/26/08   av      l1-l2 interface cleanup 
04/22/09   gnk     Fixed compiler warnings
10/03/08   hk      Bringing in 7k Mailine fixes
08/14/06   yiz     Mainline HSUPA code
02/28/06   yiz     Initial version

===========================================================================*/


/* ---------------------------- */
/* Define/unde local features   */
/* ---------------------------- */

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "eulcfg.h"
#include "mdspasync.h"
#include "eulcfg.h"
#include "l1rrcif.h"

/* ---------------------------- */
/* Check for necessary features */
/* ---------------------------- */


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* parameters for HW, HW interface document for EUL coding */
typedef struct
{
  /* num_code_blocks at bits 17~20 and pre RM bits or Ne,j at bits 0~16 */
  uint32  blocks_pre_rm_bits;

  /* num_filler_bits at bits 13~18 and bits_per_block at bits 0~12 */
  uint32  filler_block_bits;
} eul_pre_rm_param_struct_type;

/* struct type for pre RM HW parameters of one E-TFC table */
typedef struct
{
  /* Number of E-TFCI in this table, up to L1_UPA_UL_MAX_NUM_ETFC_PER_TAB */
  uint32  num_etfc;

  /* pre RM HW parameters */
  eul_pre_rm_param_struct_type  pre_rm_params[L1_UPA_UL_MAX_NUM_ETFC_PER_TAB];

} eul_pre_rm_tab_struct_type;

#ifdef FEATURE_WCDMA_L1_ZI_OPT
typedef enum
{
  EUL_ERAM_0 = 0,
  EUL_ERAM_1,
  EUL_ERAM_MAX
} eulenc_eram_enum_type;

/* HARQ level ERAM Read back data structure - 
 * harq_eram_mem - Memory is allocated from Dynamic memory pool
 * buff_avail - indicates whether the data is already flushed to loggin or not
 * subfn_cfn - Notes the value of CFN/SubFN info for log packet purpose
 * num_bits - valid number of bits in this harq_eram_mem based on etfci
 * etfci - selected etfci
 * new_tx - Indicates new or retransmission in this TTI
 */
#define EULENC_ERAM_COPY_ERAM_HARQ_MEM_BYTES   (1500)  /* DSM Item allocation max size 1500 bytes */
#define EULENC_ERAM_COPY_ERAM_HARQ_MEM_BITS    (12000) /* 1500*8 =12000 bits */
typedef struct
{
  dsm_item_type *dsm_item_ptr1;  /* DSM Item for HARQ memory [0:1499] bytes */
  dsm_item_type *dsm_item_ptr2;  /* DSM Item for HARQ memory [1500:2999] bytes */
  uint8 subfn_cfn;
  uint8 etfci;
  uint16 num_bits;
  boolean new_tx;
} eul_copy_harq_eram_mem_type;
#endif /* FEATURE_WCDMA_L1_ZI_OPT */

#if defined(DEBUG_ERAMA_RMRAM_DATA_DUMP) && defined(FEATURE_WCDMA_16_QAM)
/* HARQ Data back up memory. After writing the data to EUL ERAM, data 
 * is copied back from EUL ERAM to this SW memory. The data that is read 
 * back from EUL ERAM is already ciphered and CRC'd */
typedef struct {
  uint8  en;
  uint8  etfci;
  uint8  rsn;
  uint8  hq_id;
} debug_harq_buf_struct_type;

extern debug_harq_buf_struct_type debug_harq_buf;
#endif /* DEBUG_ERAMA_RMRAM_DATA_DUMP && FEATURE_WCDMA_16_QAM */

/* ==========================================================================
** Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
extern uint32  curr_hdr_read_ptr_word_idx;
extern uint32  curr_data_read_ptr_word_idx;
extern uint32  curr_data_read_ptr_bit_idx;
extern uint32  curr_write_ptr_word_idx;
extern uint32  curr_write_ptr_bit_idx;
extern uint32 l1_p2_ul_ctrl_wrd[EULENC_MAX_NUM_PDU_BLD];
extern uint32 l1_p2_ul_ctrl_wrd_idx;
extern eul_copy_harq_eram_mem_type eulenc_sw_harq_mem[WL1_MAX_UL_CARR_IDX][EULSG_MAX_NUM_HARQ];
extern uint32 debug_run_eul_pp_emulator;
extern uint32 *hdr_length_bits_array;
extern uint32 *data_length_bits_array;
extern uint32 *padding_length_bits_array;
extern a2_iovec_t *dma_iovec;

#ifdef FEATURE_WCDMA_A2_POWER_COLLAPSE   
extern boolean eul_a2_enabled;
#endif

#define L1_CIPH_BUFFER_IDX_INIT_VAL  0xFFFFFFFF
#define L1_COPY_ENG_BUFFER_IDX_INIT_VAL  0xFFFFFFFF

/* Index at which Ciphering keys should be written to and read from in A2 UL SEC memory */
#define EULENC_A2_UL_CIPH_INDEX  4

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION  EULENC_GET_PRE_RM_PARAM_STRUCT_FOR_ETFCI

DESCRIPTION
  This function returns a pointer to pre RM params struct for an ETFCI.

DEPENDENCIES
  Input parameters must be validated already.

RETURN VALUE
  const eul_pre_rm_param_struct_type*

SIDE EFFECTS
  None
===========================================================================*/
const eul_pre_rm_param_struct_type* eulenc_get_pre_rm_param_struct_for_etfci
(
  e_tti_enum_type etti,
  uint32 etfc_tab_idx, /* 0 or 1 */
  uint32 etfci  /* 0 ~ 127 */
);

/*===========================================================================
FUNCTION  EULENC_GET_PRE_RM_BITS_FOR_ETFCI

DESCRIPTION
  This function returns the pre_rm_bits or Ne,j for an ETFCI

DEPENDENCIES
  Input parameters must be validated already.

RETURN VALUE
  uint32 num_pre_rm_bits

SIDE EFFECTS
  None
===========================================================================*/
uint32 eulenc_get_pre_rm_bits_for_etfci
(
  e_tti_enum_type etti,
  uint32 etfc_tab_idx, /* 0 or 1 */
  uint32 etfci  /* 0 ~ 127 */
);

/*===========================================================================
FUNCTION  EULENC_SET_EDPDCH_CFG_FOR_FW

DESCRIPTION
  This function sets EDPDCH config parameters for FW

DEPENDENCIES
  HW TX_EUL_CONFIG_EDPDCH register definition

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void eulenc_set_edpdch_cfg_for_fw
(
  /* SF's for E-TFC */
  l1_e_max_ch_codes_enum_type etfc_coding_sf,

  /* pointer to EDPCH parameters to write to FW */
  mdspasync_edpch_param_db_struct_type*  next_tti_fw_param_ptr
);

/*===========================================================================
FUNCTION  EULENC_SET_SF_OVSF_FOR_FW

DESCRIPTION
  This function sets the following FW parameters for each EDPDCH channel
  SF, OVSF, and dirty bit

DEPENDENCIES
  HW interafce and 25.213 V6.4.0 4.3.1.2.3 Table 1E

RETURN VALUE
  None 

SIDE EFFECTS
  None
===========================================================================*/
void eulenc_set_sf_ovsf_for_fw
(
  /* SF's for E-TFC */
  l1_e_max_ch_codes_enum_type etfc_coding_sf,

  /* pointer to EDPCH parameters to write to FW */
  mdspasync_edpch_param_db_struct_type*  next_tti_fw_param_ptr
);

/*===========================================================================
FUNCTION  EULENC_INIT_EUL_HW

DESCRIPTION
  This function initializes the HW for next TTI EUL

DEPENDENCIES
  HW interafce

RETURN VALUE
  None 

SIDE EFFECTS
  HW ready for next TTI EUL data
===========================================================================*/
void eulenc_init_eul_hw
(
  uint32  eram_bank_idx,
  uint32  etfci,
  wl1_ul_carr_id_enum_type   carr_idx
);

/*===========================================================================
FUNCTION  EULENC_SET_ERAM_MEM_BANK

DESCRIPTION
  This function initializes the ERAM memory bank register, which is used
  by hardware to select ERAM bank for a particular HARQ.

DEPENDENCIES
  HW interafce

RETURN VALUE
  Value written to the register

SIDE EFFECTS
  HW selects the eram memory bank
===========================================================================*/
uint32 eulenc_set_eram_mem_bank
(
  uint32  eram_bank_idx,
  uint32  etfci,
  wl1_ul_carr_id_enum_type   carr_idx
);

/*=========================================================
FUNCTION      EULENC_RESET_BUILD_FRAME_IFC_CONTENTS

DESCRIPTION   Reset all the interface contents between L1 and L2
              to initial values

DEPENDENCIES  NONE

RETURN VALUE  VOID

SIDE EFFECTS  NONE
============================================================*/
void eulenc_reset_build_frame_ifc_contents(void);

/*=========================================================
FUNCTION      EULENC_VALIDATE_BUILD_FRAME_PARAMETERS

DESCRIPTION   Validate all the interface contents passed from 
              L2 to build the TBS under consideration

DEPENDENCIES  NONE

RETURN VALUE  VOID

SIDE EFFECTS  NONE
============================================================*/
void eulenc_validate_build_frame_parameters(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================
FUNCTION      EULENC_A2_UL_INIT

DESCRIPTION   This function will set the following - 
              - A2 UL Technology to HSPA
              - Register the callback, based on Interrupt needed
              - Configure the Gen2AXI bus
              - Initialize the interface parameters

DEPENDENCIES  VOID

RETURN VALUE  VOID

SIDE EFFECTS  VOID
============================================================*/
void eulenc_a2_ul_init(void);

/*=========================================================
FUNCTION      EULENC_A2_UL_DEINIT

DESCRIPTION   This function will set the A2 UL Technology to 
              Invalid. This will release the A2 UL resources
              
DEPENDENCIES  VOID

RETURN VALUE  VOID

SIDE EFFECTS  VOID
============================================================*/
void eulenc_a2_ul_deinit(void);

/*=========================================================
FUNCTION      EULENC_BUILD_A2_TASKS

DESCRIPTION   This function is called after data is formed by L2 and 
              given to L1, in build frame indication.It has CTRL Info
              and CIPH words

DEPENDENCIES  RLC should have defined and prepared the data and header info
              blocks, as well as PDU-info.

RETURN VALUE  VOID

SIDE EFFECTS  NONE
============================================================*/
void eulenc_build_a2_tasks(wl1_ul_carr_id_enum_type carr_idx);

#ifdef FEATURE_WCDMA_L1_ZI_OPT

/*===========================================================================
FUNCTION  EULENC_ERAM_DUMP_PROCESS_LOCAL_CMD

DESCRIPTION
  Process local command to log the HARQ data in Task context
  Currently printing using MSG_ERROR
  Later move to log packet model
  
DEPENDENCIES  None
RETURN VALUE  None
SIDE EFFECTS  None
===========================================================================*/
void eulenc_eram_dump_process_local_cmd(uint8 harq_id);

/*===========================================================================
FUNCTION  EULENC_A2_ERAM_DUMP_DEALLOC_MEMORY

DESCRIPTION
  This function enables eram memory dump logics on QXDM command

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void eulenc_a2_enable_qxdm_dump_eram_mem(void);

/*===========================================================================
FUNCTION  EULENC_A2_DISABLE_QXDM_DUMP_ERAM_MEM

DESCRIPTION
  This function disables eram memory dump logics on QXDM command

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void eulenc_a2_disable_qxdm_dump_eram_mem(void);

/*===========================================================================
FUNCTION  EULENC_A2_DISABLE_QXDM_DUMP_ERAM_MEM_REQ

DESCRIPTION
  This function requests to Disable eram memory dump logics on QXDM command
  To avoid any Task/ISR race conditions, QXDM will set the following flag.
  In the A2 DONE ISR CB, if this flag is TRUE 
     - Stops the QXDM Dumping logics and de registers the CB, 
       releases memory, clears the flags.

DEPENDENCIES  None
RETURN VALUE  None
SIDE EFFECTS  None
===========================================================================*/
void eulenc_a2_disable_qxdm_dump_eram_mem_req(void);

/*===========================================================================
FUNCTION  EULENC_A2_CONFIG_QXDM_DUMP_ERAM_PROFILING_STATS_REQ

DESCRIPTION
  This function configures the ERAM profiling results printing.

DEPENDENCIES  None
RETURN VALUE  None
SIDE EFFECTS  None
===========================================================================*/
void eulenc_a2_config_qxdm_dump_eram_profiling_stats_req(uint32 val);
#endif

void l1_l2_build_copy_engine_ctrl_info_with_index
(
  uint32 hdr_len_bits,
  uint32 data_len_bits,  
  uint32 offset_bits,
  uint32 ctrl_wrd_idx
);

/*=========================================================
FUNCTION      E_UL_ERAM_HARQ_MEM_REDUCTION_INIT

DESCRIPTION   This function performs initialization for handling EUL ERAM Bank reduction.

DEPENDENCIES

RETURN VALUE  VOID

SIDE EFFECTS  NONE
============================================================*/
void e_ul_eram_harq_mem_reduction_init(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================
FUNCTION      E_UL_ERAM_HARQ_MEM_REDUCTION_DEINIT

DESCRIPTION   This function performs de-initialization related to EUL ERAM Bank reduction.

DEPENDENCIES

RETURN VALUE  VOID

SIDE EFFECTS  NONE
============================================================*/
void e_ul_eram_harq_mem_reduction_deinit(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================
FUNCTION      EULENC_INIT_HARQ_DSM_PTR

DESCRIPTION   This function initializes SW HARQ DSM pointers to NULL
              which are used for EUL ERAM Bank reduction logics.

DEPENDENCIES

RETURN VALUE  VOID

SIDE EFFECTS  NONE
============================================================*/
void eulenc_init_harq_dsm_ptr(void);

/*=========================================================
FUNCTION      EULENC_INIT_SW_HARQ_MEM

DESCRIPTION   This function initializes SW data structure for handling EUL ERAM Bank reduction.

DEPENDENCIES

RETURN VALUE  VOID

SIDE EFFECTS  NONE
============================================================*/
void eulenc_init_sw_harq_mem(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================
FUNCTION      EULENC_DEINIT_SW_HARQ_MEM

DESCRIPTION   This function de-initializes SW data structure related to EUL ERAM Bank reduction.

DEPENDENCIES

RETURN VALUE  VOID

SIDE EFFECTS  NONE
============================================================*/
void eulenc_deinit_sw_harq_mem(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================
FUNCTION      EULENC_ERAM_A2_RB_DATA

DESCRIPTION   This function is called by L1 SW to readback data from ERAMA Bank using
                        A2 Driver.

DEPENDENCIES  VOID

RETURN VALUE  VOID

SIDE EFFECTS  VOID
============================================================*/
void eulenc_eram_a2_rb_data(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================
FUNCTION      EULENC_A2_DUMP_RETX_INFO_TO_EULERAM

DESCRIPTION   This function dumps the data from SW memory to EUL ERAM for retx.
              The SW memory is populated during eulenc_eram_a2_rb_data.
              The data in the SW memory is already ciphered and CRC'd. So this function simply
              dumps the data to EUL ERAM using A2. The first write to A2  writes only 32 bits due 
              to constraint from A2 side.

DEPENDENCIES  VOID

RETURN VALUE  VOID

SIDE EFFECTS  VOID
============================================================*/
void eulenc_a2_dump_retx_info_to_euleram(uint8 harq_id, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION  EULENC_A2_RDBK_MEM_RED_RETX_BUFFER_MGMT

DESCRIPTION   This function updates the ERAM Bank Index using Ping-pong buffer. If new Tx, it ensures the SW HARQ 
                        dumping Memory is available to read back after A2 is completed writing from L2 DSM to EUL ERAM.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void eulenc_a2_rdbk_mem_red_retx_buffer_mgmt(boolean next_tti_dtx,
                                             mdspasync_edpch_param_db_struct_type*  next_tti_fw_param_ptr,
                                             eul_edch_harq_sg_info_struct_type *harq_sg_info_ptr,
                                             wl1_ul_carr_id_enum_type carr_idx);
#endif /* EULENC_H */

