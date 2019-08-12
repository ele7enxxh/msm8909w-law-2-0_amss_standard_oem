#ifndef __FTM_HWTC_CMD_H
#define __FTM_HWTC_CMD_H

/*===========================================================================

                  FACTORY TEST HWTC COMMAND INTERFACE
DESCRIPTION

  This file contains prototypes, #defines, and enums which constitute
  the public interface for FTM HWTC functions intended to be called via
  the diag mechanism.

  Copyright (c) 2003, 2004, 2006, 2007, 2009
                  by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/hwtc/inc/ftm_hwtc_cmd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/11   rsr/sr  Fix for env gain length in HWTC.  uint8 ->uint16
05/13/11   sar     Added ftmicap.h.
10/28/09   aro     Featurized Tech Specifc Code
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
09/16/09   bmg     Added target.h hack after comdef.h because SCMM comdef.h
                   is broken for off-target builds
09/01/09   tnt     Move INO code from hwtc to ftm
08/20/09   tnt     Add code for common ino
07/27/09   rsr     Fixes to add HWTC command from Linear. Debug tools 
06/26/09   bmg     Fixed MSVC packing pragma to restore the previous value
                   before leaving the header file.
10/31/08   bmg     Fixed off-target packing pragmas
10/18/07   dw      Added ftm_hwtc_c0_c1_delay_type.
10/02/06   ka      Added ftm_hwtc_filter_coeff_type.
09/02/04   lcl     Added ftm_mix_type to the union.
01/18/03   ra     Initial revision.Baseline from FTM CMD header

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "target.h"
#include "ftmicap.h"
#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_HWTC
#include "ftmdiag.h"
#include "ftm_hwtc_dispatch.h"
#include "ftm.h"
#ifdef FEATURE_GSM
#include "rfgsm_core_types.h"
#endif /* FTM_HAS_GSM */

#ifdef _MSVC_VER
#pragma pack(push, 1)
#endif /* _MSVC_VER */
       
#if !defined(SWIG) && !defined(PERL_OBJECT)
#include "qw.h"
#else
/* SWIG has problems with arrays, so define an equivalent struct.  Because this
   is an array on the target, the semantics of "passed by value" can be used 
   and it works OK, unlike things that are structs on the target - jcw
*/
   
typedef struct 
{
  dword lo;
  dword hi;
} qword;
#endif 

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef T_WINNT
#error code not present
#endif
       
/* This struct holds the Rx Front End Filter Coefficients */
typedef PACK(struct)
{
  uint8  chain;
  uint8  band;
  uint16 rxf_equ_b0_coeff;
  uint16 rxf_equ_b1_coeff;
  uint16 rxf_equ_b2_coeff;
  uint16 rxf_equ_b3_coeff;
  uint16 rxf_equ_b4_coeff;
  uint16 rxf_equ_b5_coeff;
  uint16 rxf_equ_b6_coeff;
  uint16 rxf_equ_b7_coeff;
  uint16 rxf_equ_b8_coeff;
  uint16 rxf_equ_b9_coeff;
  uint16 rxf_equ_b10_coeff;
} ftm_hwtc_filter_coeff_type;

/* TX gain settings for linear TX */
typedef PACK(struct) {
 uint16 edge_env_gain; /* Beta scaling factor to set gain in EDGE/GSM mode */
 uint8 r0; /* PA range R0 */
 uint8 r1; /* PA range R1 */
 uint8 predistortion;  /* Predistortion flag (on/off) */
 uint16 pa_scale;  /* scaling factor applied to PA ramp */
 uint8 amam_tbl_bank;  /* Which AMAM bank is used to predistort (0/1) */
 uint8 slot_mode_ctl1_reg_val;
 uint8 slot_mode_ctl2_reg_val;
 uint8 slot0_reg1Val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
 uint8 slot0_reg2val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
 uint8 slot1_reg1Val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
 uint8 slot1_reg2val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
 uint8 slot2_reg1Val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
 uint8 slot2_reg2val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
 uint8 slot3_reg1Val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
 uint8 slot3_reg2val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
 uint8 slot4_reg1Val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
 uint8 slot4_reg2val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
 uint8 slotnum;
}ftm_hwtc_lin_gain_buf_type;

#ifdef FEATURE_GSM
typedef PACK(union) {
  ftm_hwtc_lin_gain_buf_type gain_buf;
}ftm_hwtc_gsm_params_type;
#endif /* FTM_HAS_GSM */

/* This struct holds the second chain diversity path delay for the Rx Front Misc control register*/
typedef PACK(struct)
{
  uint8  band;
  uint8 delay;
} ftm_hwtc_c0_c1_delay_type;

#ifdef FEATURE_GSM
typedef PACK(struct)
{
   uint8 rgi_index;
   rfcom_gsm_band_type band; 
   rfgsm_modulation_type mod_type; 
   uint8 slot_num;
   uint8 slot_mode_ctrl1;
   uint8 slot_mode_ctrl2;
   uint8 slot0_reg1Val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
   uint8 slot0_reg2val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
   uint8 slot1_reg1Val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
   uint8 slot1_reg2val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
   uint8 slot2_reg1Val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
   uint8 slot2_reg2val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
   uint8 slot3_reg1Val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
   uint8 slot3_reg2val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
   uint8 slot4_reg1Val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
   uint8 slot4_reg2val; /* SBI settings that affect gain (These are the settings affected by RF in TX AGC.)*/
} ftm_hwtc_reg_from_rgi_type;
#endif /* FTM_HAS_GSM */
typedef PACK(struct)
{
  uint16 mod_type_is_8psk;
  uint16 band_id;
  uint16 tx_lut[32];
}ftm_hwtc_set_tx_char_lut_type;

/******************************* HWTC DATA STRUCTURE **************************/
typedef PACK(union)
{   /* union of RF commands data types */
  word                    gen_w;      /* Generic word                      */
  byte                    gen_b;      /* Generic byte                      */
  dword                   gen_d;      /* Generic dword                     */
  word                    chan;       /* For tuning to a specific channel  */
  boolean                 on_off;     /* For anything with an on_off state */
  ftm_mode_id_type        mode;       /* For Setting Phone operation mode  */
  ftm_pa_range_type       range;      /* Set the LNA/PA range              */
  ftm_id_val_type        id_val;     /* For all indexed values            */
  ftm_byte_type          byte_struct;
  ftm_mix_type           mix_struct;
  ftm_hwtc_filter_coeff_type coeff_struct;
  ftm_hwtc_c0_c1_delay_type c0_c1_delay_struct;
  #ifdef FEATURE_GSM
  ftm_hwtc_gsm_params_type gsm_params;
  ftm_hwtc_reg_from_rgi_type rgi_params;
  #endif /* FTM_HAS_GSM */
  ftm_hwtc_set_tx_char_lut_type txlin_char_lut;
} ftm_hwtc_parms;


/************************* FTM HWTC DATA STRUCTURE **************************/

/* hwtc request/response type */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header;
  ftm_cmd_header_type ftm_cmd_header;
  ftm_hwtc_parms hwtc_params;
} ftm_hwtc_pkt_type;

#ifdef _MSVC_VER
#pragma pack(pop)
#endif /* _MSVC_VER */
       
#endif /* FEATURE_FTM_HWTC */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* __FTM_HWTC_CMD_H */
