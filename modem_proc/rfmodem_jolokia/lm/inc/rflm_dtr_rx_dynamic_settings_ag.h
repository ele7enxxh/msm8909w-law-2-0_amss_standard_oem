
#ifndef RFLM_DTR_RX_DYNAMIC_SETTINGS_AG_H
#define RFLM_DTR_RX_DYNAMIC_SETTINGS_AG_H


#ifdef __cplusplus
extern "C" {
#endif

/*
WARNING: This file is auto-generated.

Generated at:    Fri Jan 29 12:18:26 2016
Generated using: lm_autogen.exe v5.1.36
Generated from:  v6.4.19 of Jolokia_RxFE_Register_Settings.xlsx
*/

/*=============================================================================

           R X    A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the modem register settings 
  configured by FW, provided by the rflm_dtr_rx.

Copyright (c) 2009, 2010, 2011, 2012, 2013, 2014, 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

$DateTime: 2016/12/13 07:59:25 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/lm/inc/rflm_dtr_rx_dynamic_settings_ag.h#1 $

=============================================================================*/

/*=============================================================================
                           REVISION HISTORY
Version    Author   Date   
         Comments   IR Number   Other POC (Comma Delimited)   Release Validation Notes   Dependencies   AG File Location   
Jolokia 6.4.19; Tabasco 7.3.11   gatesj   1/14/2016   
         1. ADC settings updated for Jolokia derivatives. Lykan/Feero chipset-specific settings added to the JOLOKIA_bbrx_config tab. New updates for GSM/LTE20/FBRX on 1/14/16      wangyan@qti.qualcomm.com            
Jolokia 6.4.18; Tabasco 7.3.11   gatesj   1/13/2016   
         1. ADC settings updated for Jolokia derivatives. Lykan/Feero chipset-specific settings added to the JOLOKIA_bbrx_config tab.      wangyan@qti.qualcomm.com            
Jolokia 6.4.17; Tabasco 7.3.11   gatesj   10/22/2015   
         1. ADC settings updated for Eldarion ADC Vcm cal - bypassed AAF      yusong@qti.qualcomm.com            
Jolokia 6.4.17; Tabasco 7.3.10   gatesj   9/21/2015   
         "1. New SW Dynamic tab to hold ADC hibernation settings for each BBRx.                  
2. New ADC_HIBERNATION_MODE SW varible "      djoffe@qti.qualcomm.com   
                           
Jolokia 6.4.16; Tabasco 7.3.9   gatesj   9/15/2015   
         1. Eldarion BBRx settings updated per BBRx team recommendation       yusong@qti.qualcomm.com, dalladi@qti.qualcomm.com            
Jolokia 6.4.16; Tabasco 7.3.8   gatesj   8/17/2015   
         1. LTE_CA modes set to use sinc3      mmenon@qti.qualcomm.com         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.16/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.8/"                           
Jolokia 6.4.15; Tabasco 7.3.7   gatesj   8/12/2015   
         1. Modified IQMC settings for xPT states. Enabled by default and CFG values set to unity.       ankitm@qti.qualcomm.com         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.15/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.7/"                           
Jolokia 6.4.14; Tabasco 7.3.6   gatesj   7/27/2015   
         1. ADC settings updated for 28LP pairing (Atlas SE) for FBRx and LTE40 modes per BBRx team's recommendation    CR880082   wangyan@qti.qualcomm.com         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.14/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.6/"                           
Jolokia 6.4.13; Tabasco 7.3.5   gatesj   7/8/2015   
         1. ADC insertion loss deltas updated for LTE40 modes, corrected for some single-rx modes      wangyan@qti.qualcomm.com         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.13/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.5/"                           
Jolokia 6.4.13; Tabasco 7.3.4   gatesj   6/24/2015   
         1. WTR2955 ICIFIR updated for TDS, LTE15, LTE20 on ICIFIR_SW_Dynamic tab based on lates WTR2955 objective spec.      "tzuhanc@qti.qualcomm.com            
mlane@qti.qualcomm.com                           
mmenon@qti.qualcomm.com"         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.13/                  
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.4/"                           
Jolokia 6.4.12; Tabasco 7.3.3   gatesj   6/16/2015   
         "1. Included type definition for modem specific tabs - corrects RxLM vars types                  
2. Renamed Jolokia BBRx settings to align with Tabasco naming"      lmiao@qti.qualcomm.com   
               "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.12/            
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.3/"                           
Jolokia 6.4.11; Tabasco 7.3.2   gatesj   6/5/2015   
         "1. Added support for Eldarion/Jacala ADC registers using overrides                  
2. Added modem specific support to WB2/Writer4 debug writes for Lykan support"      djoffe@qti.qualcomm.com   
               "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.11/            
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.2/"                           
Jolokia 6.4.10; Tabasco 7.3.1   gatesj   5/11/2015   
         1.TDS WBDC K=4 for TRK and IDLE modes      huanx@qti.qualcomm.com, ccmercad@qti.qualcomm.com         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.10/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.1/"                           
Jolokia 6.4.9; Tabasco 7.3.0   gatesj   4/28/2015   
         "1. FTM mode variables LTE_K_NORM and LTE_NBEE renamed for tabasco    to include single Rx support.                
2. Single Rx SW dynamic tab updated with FTM mode variables                           
3. Single Rx CA information added to the NBPWR SW dynamic tab                           
4. Dec8 blocks now runtime dynamic                           
5. NB2 added to NBPWR SW dynamic tab                           
6. NB2 NBPWR registers now SW dynamic"      vboda@qti.qualcomm.com   
               "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.9/            
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.0/"                           
Jolokia 6.4.9; Tabasco 7.2.1   gatesj   4/21/2015   
         1. Updated common Dec8 for Tabasco. Block groups split into DEC8_FILT0 and DEC8_FILT1. Block valid flags adjusted.      djoffe@qti.qualcomm.com         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.9/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.2.1/"                           
Jolokia 6.4.9; Tabasco 7.2.0   gatesj   4/17/2015   
         "1. NB3 buffer included for Tabasco pairings only. Single-Rx CA tab updated.                  
2. Always Armed bit set to 0 for WBPWR    NBPWR    DVGA before WB and NB clears   
                           
3. Tabasco HPM (Eldarion) BBRx settings populated"      ccmercad@qti.qualcomm.com, yusong@qti.qualcomm.com, vboda@qti.qualcomm.com   
               "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.9/            
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.2.0/"                           
Jolokia 6.4.8; Tabasco 7.1.8   gatesj   4/6/2015   
         "1. New LTE_ADC_SW_Dynamic tab added to handle dynamic ADC rates    previous states removed.               
2. ICIFIR_SW_Dynamic tab added for WTR based ICIFIR selection    currently only for TDS    LTE15   
          and LTE20                  
3. VSRC and ADC CFG registers are now SW dynamic"      mlane@qti.qualcomm.com   
               "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.8/            
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.8/"                           
Jolokia 6.4.7; Tabasco 7.1.7   gatesj   4/1/2015   
         "1. BBRx change in release 6.4.6 for LTE20 removed.                   
2. New Tabasco RxLM state for B7/B41/B40 to adjust ADC rate. "      wangyan@qti.qualcomm.com, mlane@qti.qualcomm.com   
               "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.7/            
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.7/"                           
Jolokia 6.4.6; Tabasco 7.1.6   gatesj   3/30/2015   
         1. BBRx settings updated for GSM, WCDMA, TDS, and C2K/DO.      wangyan@qti.qualcomm.com         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.6/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.6/"                           
Jolokia 6.4.5; Tabasco 7.1.5   gatesj   3/4/2015   
         1. Corrected ICIFIR length for GSM IP2 state      tslocomb@qti.qualcomm.com, ppachett@qti.qualcomm.com         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.5/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.5/"                           
Jolokia 6.4.4; Tabasco 7.1.4   gatesj   3/3/2015   
         1. Updated ICIFIR coefficients for GSM IP2 cal state to match those in DPM      tslocomb@qti.qualcomm.com, ppachett@qti.qualcomm.com         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.4/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.4/"                           
Jolokia 6.4.3; Tabasco 7.1.3   gatesj   2/27/2015   
         "1. Single Rx LTE CA hooks added in CA mode tab.                   
2. Single Rx LTE CA dynamic ICIFIR tab included.                           
3. ADC Vcm cal state included                           
4. NB_DEC groups are now SW dynamic"      kbaskara@qti.qualcomm.com, djoffe@qti.qualcomm.com   
               "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.3/            
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.3/"                           
Jolokia 6.4.2; Tabasco 7.1.2   gatesj   12/11/2014   
         1. Update WCDMA fast raw scan NB index      harishv@qti.qualcomm.com, vishnun@qti.qualcomm.com         "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.2/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.2/"                           
Jolokia 6.4.1; Tabasco 7.1.1   gatesj   12/4/2014   
         "1. New state for WCDMA fast raw scan                  
2. ICIFIR updated for 1x/DO    provided by 1x team"      harishv@qti.qualcomm.com, vishnun@qti.qualcomm.com, ndhar@qti.qualcomm.com, ruhuah@qti.qualcomm.com, sekin@qti.qualcomm.com   "1. Verified in CSIM               
2. Verified by 1x team on target and in CSIM"      "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.1/   
                           
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.1/"                           
Jolokia 6.4.0; Tabasco 7.1.0   gatesj   11/10/2014   
         "1. Included the NBPWR configuration for LTE states in the nbpwr_SW_Dynamic tab                  
2. Included two new RxLM Variables for LTE_K_NORM_FACTOR and LTE_K_NBEE_TO_DBV. All scale factors now int32.                           
3. Renamed COMMON_NOTCH_02 block name to COMMON_NOTCH_23"      bhiremat@qti.qualcomm.com, itkachov@qti.qualcomm.com   
         1 & 2. Verified on DPM      "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.0/            
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.0/"                           
Jolokia 6.3.3; Tabasco 7.0.2   gatesj   10/28/2014   
         1. Updated ideal CSR values for Tabasco states: UMTS3C, LTECA               "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.3/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.0.1/"                           
Jolokia 6.3.3; Tabasco 7.0.1   gatesj   10/28/2014   
         1. Correction to the Tabasco notch CMD registers.                "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.3/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.0.1/"                           
Jolokia 6.3.3; Tabasco 7.0.0   gatesj   10/27/2014   
         1. Initial updates for Tabasco checkin. Includes all new states and registers. Placeholders for HPm BBRx settings.                "https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.3/   
https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.0.0/"                           
6.3.3   gatesj   10/21/2014   
         1. UMTS DSR FET NB has imm trigger removed      aojha@qti.qualcomm.com, carstenp@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.3/   
6.3.2   gatesj   10/20/2014   
         1. UMTS DSR state now has an extra NB for FET      aojha@qti.qualcomm.com, carstenp@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.2/   
6.3.1   gatesj   10/20/2014   
         1. WTR2955 added to the TRX RxLM parameter for NBR state      vsundar@qti.qualcomm.com, aojha@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.1/   
6.3.0   gatesj   9/17/2014   
         "1. Updated WBDC scale factors for LTE states                  
2. Updated UMTS NBR state to include expected CSR values                           
3. ADC_CFG block now runtime dynamic - requires interface change                           
4. New RxLM Variable NOTCH_SAMPLING_RATE - requires interface change                           
5. IQMC fields now set to DIRECT_WRITE                           
6. LTE20 and LTE15 states modified to increase VSRC output rate to 122.88MHz to avoid DC images falling in band                           
    a) Dec0 disabled                           
    b) Dec2 enabled                           
    c) Scaling factors updated                           
    d) Delays updated                           
    e) ICIFIR updated                           
    f) VSRC regulator bypassed                           
7. 1x NBPWR mask set to 4095    128 cx1 samples    to match the Bolt code used in Jolokia"   
         IR-028568   mhoffman@qti.qualcomm.com, sekin@qti.qualcomm.com, jpaul@qti.qualcomm.com   "1. Verified on DPM             
2. Verified on DPM                           
3. Verified on DPM                           
4. Verified on DPM                           
6. Verified on DPM                           
7. Verified on RUMI by C2K team"   7. CR726173   https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.0/   
                           
6.2.4   gatesj   8/11/2014   
         "1. BBRx register settings updated. 1605/2605 references removed from bbrx_config tab. LTE40 mode added.                   
2. WBDC LSHIFT_UPDT_CMD set for all LTE modes.                            
3. GSM state now uses Dec7 in mode 1. ICIFIR updated. "   IR-021714   wangyan@qti.qualcomm.com   
         2. Verified on DPM      https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.2.4/            
6.2.3   gatesj   7/28/2014   
         1. CSR_IDEAL_VAL_C0 and CSR_IDEAL_VAL_C1 updated for UMTS2C state to now contain the 2.5MHz and -2.5MHz rotator values.    IR-019381   rllynch@qti.qualcomm.com, carstenp@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.2.3/   
6.2.2   gatesj   7/27/2014   
         "1. GSM IP2 cal state changes (rotator location/frequency    ICIFIR)               
2. STMR_MHZ_FS modifed for 1x/HDR states correcting the STMR_FCW RFLM values.                            
3. Modified the triggering of the FET NB    followup to change 2 in 5.3.5. Now uses imm_trig instead of enable/start_stop"   IR-019089   
         vijayr@qti.qualcomm.com   1. Verified by the G team on DimePM      https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.2.2/         
6.2.1   gatesj   7/10/2014   
         "1. GSM VSRC SCTR wrap around doubled                  
2. TDS TRK IRAT WBDC parameter K2 set to 7 (previously 6).                           
3. IQ Swap set to 1 for TDS states.                            
4. UMTS 2C ICIFIR updated.                           
5. UMTS SC state has the FET NB disabled by default."   IR-016445   bcanpola@qti.qualcomm.com, yclin@qti.qualcomm.com, yingxuel@qti.qualcomm.com, carstenp@qti.qualcomm.com   
         "1. Validated by the G team on Bolt                  
2. Validated by the T team on DimePM                           
4. Validated by the W team on DimePM                           
5. Validated by the W team on DimePM"      https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.2.1/   
                           
6.2.0   gatesj   6/27/2014   
         "1. IQ_SWAP values set to 1 for LTE demod chain output    1x/DO outputs    UMTS outputs.            
2. Updated BBRx configuration registers to new recommended values.                            
3. bbrx_fuse_SW_Dynamic tab included for BBRx RSB compensation.                           
4. NB blocks are now runtime dynamic for CA - requires interface change. "   IR-014276   mhoffman@qti.qualcomm.com, sekin@qti.qualcomm.com, carstenp@qti.qualcomm.com, wangyan@qti.qualcomm.com, cdeshmuk@qti.qualcomm.com, bow@qti.qualcomm.com, djoffe@qti.qualcomm.com   
               https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.2.0/            
6.1.1   gatesj   6/5/2014   
         "1. Separate states on 4905 for LTE1.4   3   5 to account for WTR pole differences and ICIFIR changes. No SW changes required.             
2. VSRC_WA updated to FW Export dynamicity. VSRC subgroup reordering to allow for this.                            
3. UMTS_1C state now has 2 NB    to enable FET NB mirroring.                         
4. New UMTS_ACQ state - copy of the 1C state for acquisition with only 1 NB."                           
6.1.0   gatesj   5/7/2014   
         "1. TDS ACQ output format changed to 16SC15 through SROUND2    PBS_CFG updated               
2. WBDC/WBPWR/NBPWR scale factors now have csim tags.                            
3. Included WBDC_OFFSET variable    requires interface change.                         
4. All WBPWR/WBDC/NBPWR offset/scale factors are now Q24 format.                           
5. GSM output format now defaults to 16SC15 through SSAT2 instead of SROUND2.                            
6. The TDS ACQ  state now has PBS_SAMPX_SEL updated to 5 to account for the decimation in the writer.                           
7. BBRx configuration registers updated.                           
8. ADC insertion loss deltas updated."      yingxuel@qti.qualcomm.com   
                           
6.0.1   gatesj   4/16/2014   
         "1. Corrected UMTS2C ICIFIR truncbits calculation                   
2. Corrected TDS ACQ output format.                            
3. Addition of the UMTS NBR state.                           
4. Addition of the nbpwr_SW_Dynamic tab containing UMTS NBPWR Cal information.                            
5. Modified the ADC_INSERTION_LOSS_DELTA values set to zero    but will use Q24 format in the future if needed.                         
6. All LTE searcher NB modified p2=4    to reduce the wraparound to 12288 samples    6.4ms.   
                           
7. ROUND_ENABLE register renamed to ROUND_SAT_ENABLE. Bit widths corrected. "      lopezm@qti.qualcomm.com   
                           
6.0.0   gatesj   3/27/2014   
         "1. Branched from DimePM RxLM 5.2.5                  
2. Removed 1605/2605 specific states. Added 2955 TRX strings to 4905 states for compatibility.                            
3. Added PBS_WIDE32_WORD_EN register. Set to 1 for states with 16SC15 output.                            
4. Added RXFE_NB_FINAL_SWAP_NBn to each NB. None currently set.                            
5. Replaced DimePM modem strings with Jolokia."                           

=============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rflm_dtr_rx_struct_ag.h" 
#include "rflm_dtr_rx_typedef_ag.h" 




extern rflm_dtr_rx_nb_pwr_nb_group_struct rx_nbpwr_nb_pwr_nb[RFLM_DTR_RX_NBPWR_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_nbpwr_nb_pwr_nb (
 			rflm_dtr_rx_nb_pwr_nb_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_nb_pwr_nb_group_struct rx_nbpwr_nb_pwr_nb2[RFLM_DTR_RX_NBPWR_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_nbpwr_nb_pwr_nb2 (
 			rflm_dtr_rx_nb_pwr_nb_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_icifir_nb_group_struct rx_icifir_icifir_nb[RFLM_DTR_RX_ICIFIR_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_icifir_icifir_nb (
 			rflm_dtr_rx_icifir_nb_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_vsrc_xo_vars_group_struct rx_lte_adc_vsrc_xo_vars[RFLM_DTR_RX_LTE_ADC_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_lte_adc_vsrc_xo_vars (
 			rflm_dtr_rx_vsrc_xo_vars_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_adc_clk_cfg_group_struct rx_lte_adc_adc_clk_cfg[RFLM_DTR_RX_LTE_ADC_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_lte_adc_adc_clk_cfg (
 			rflm_dtr_rx_adc_clk_cfg_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_adc_config_group_struct rx_lte_adc_adc_config[RFLM_DTR_RX_LTE_ADC_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_lte_adc_adc_config (
 			rflm_dtr_rx_adc_config_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_vsrc_group_struct rx_lte_adc_vsrc[RFLM_DTR_RX_LTE_ADC_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_lte_adc_vsrc (
 			rflm_dtr_rx_vsrc_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_icifir_nb_group_struct rx_single_rx_ca_icifir_nb[RFLM_DTR_RX_SINGLE_RX_CA_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_single_rx_ca_icifir_nb (
 			rflm_dtr_rx_icifir_nb_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_nb_dec_nb_group_struct rx_single_rx_ca_nb_dec_nb[RFLM_DTR_RX_SINGLE_RX_CA_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_single_rx_ca_nb_dec_nb (
 			rflm_dtr_rx_nb_dec_nb_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_icifir_nb_group_struct rx_single_rx_ca_icifir_nb1[RFLM_DTR_RX_SINGLE_RX_CA_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_single_rx_ca_icifir_nb1 (
 			rflm_dtr_rx_icifir_nb_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_icifir_nb_group_struct rx_single_rx_ca_icifir_nb2[RFLM_DTR_RX_SINGLE_RX_CA_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_single_rx_ca_icifir_nb2 (
 			rflm_dtr_rx_icifir_nb_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_nb_dec_nb_group_struct rx_single_rx_ca_nb_dec_nb2[RFLM_DTR_RX_SINGLE_RX_CA_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_single_rx_ca_nb_dec_nb2 (
 			rflm_dtr_rx_nb_dec_nb_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_csr_xo_vars_group_struct rx_single_rx_ca_csr_xo_vars[RFLM_DTR_RX_SINGLE_RX_CA_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_single_rx_ca_csr_xo_vars (
 			rflm_dtr_rx_csr_xo_vars_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_scale_vars_group_struct rx_single_rx_ca_scale_vars[RFLM_DTR_RX_SINGLE_RX_CA_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_single_rx_ca_scale_vars (
 			rflm_dtr_rx_scale_vars_group_struct *target_struct ,
 			uint32 table_index 
			);
extern rflm_dtr_rx_adc_config_group_struct rx_adc_hibernation_adc_config[RFLM_DTR_RX_ADC_HIBERNATION_SW_DYNAMIC_SETTINGS_NUM];

extern void update_rx_adc_hibernation_adc_config (
 			rflm_dtr_rx_adc_config_group_struct *target_struct ,
 			uint32 table_index 
			);
#ifdef __cplusplus
}
#endif



#endif


