#ifndef __PM_NPA_IREP_H__
#define __PM_NPA_IREP_H__

/*! \file pm_npa_irep.h
 *  
 *  \brief This file contains PMIC resources internal rep struct types. It is
 *  created so that PMIC can expose internal structures to the external clients
 *  such as railways that need to use the same struct definition as PMIC driver.
 *  
 *    &copy; Copyright(c) 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/14    aks     Created
===========================================================================*/


/*This structure represents the values that are used for setting 
  LDO regulator. These values are derived from the xlate function()
  which is called by RPM server to xlate the KVP's to int rep.
  */
typedef struct
{
   unsigned sw_en:1; // [Disable (default), Enable] ? max aggregation (left to right)
   unsigned ldo_sw_mode:4; // [IPEAK (default), NPM] - max aggregation (left to right)
   unsigned pc_en:4; // [NONE, EN1, EN2, EN3, EN4] - ORed value of list
   unsigned pc_mode:5; // [NONE, EN1, EN2, EN3, EN4, SLEEPB] - ORed value of list
   unsigned resource_id:8; // This is needed for the LDO bypass call back from SMPS
   unsigned device_index:2; // This is needed for the LDO bypass call back from SMPS
   unsigned is_en_transition:1; // If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false
   unsigned byp_allowed:1; // [Allowed, Disallowed] - max aggregation (left to right)
   unsigned reserve1:6;
   // 32 bit boundary
   unsigned input_uvol :32; // requested uV + headroom
   // 32 bit boundary
   unsigned ip:13; // mA > 8 Amps
   unsigned noise_hr:19; // uV
   // 32 bit boundary
   unsigned output_uvol  :32; // requested voltage from the client
}pm_npa_ldo_int_rep;


/*This structure represents the values that are used for setting 
  SMPS regulator. These values are derived from the xlate function()
  which is called by RPM server to xlate the KVP's to int rep.
  */
typedef struct
{
   unsigned sw_en:1; // [Disable (default), Enable] ? max aggregation (left to right)
   unsigned smps_sw_mode:4; // [AUTO(default), IPEAK, NPM] - max aggregation (left to right)
   unsigned pc_en:4; // [NONE, EN1, EN2, EN3, EN4] - ORed value of list
   unsigned pc_mode:5; // [NONE, EN1, EN2, EN3, EN4, SLEEPB] - ORed value of list
   unsigned global_byp_en:1; // Keeps whether the child LDOs are allowed to go into bypass
   unsigned hr:17; // uV
   // 32 bit boundary
   unsigned uvol:32; // uV
   // 32 bit boundary
   unsigned ip:13; // mA > 8 Amps
   unsigned freq:5; // Frequency
   unsigned freq_reason:4; // ORed technologies from pm_npa_freq_reason_type
   unsigned quiet_mode:2; // [None, Quiet, Super Quiet] ? max aggregation (left to right)
   unsigned byp_allowed:1; // [Allowed, Disallowed] - max aggregation (left to right)
   unsigned reserve1:7;
   // 32 bit boundary
}pm_npa_smps_int_rep;

#endif //__PM_NPA_IREP_H__
