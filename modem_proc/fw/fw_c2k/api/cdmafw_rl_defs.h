/**
  @file cdmafw_rl_defs.h

  External interface definitions for CDMA Reverse Link Functionality.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/
#ifndef CDMAFW_RL_DEFS_H
#define CDMAFW_RL_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} // extern "C"
#endif

/** 
    \defgroup cdma1x_rl_msg Reverse Link L1 Message Definitions.
    \defgroup cdma1x_rl_aux Auxillary Definitions for Reverse Link L1 Messages.
    \defgroup cdma1x_rl_dep Deprecated Msg and Aux Definitions for CDMA Reverse Link.
*/

/**
    \addtogroup cdma1x_rl_aux
    @{
*/   

/** Number of RL physical channels in 1X */
#define CDMAFW_NUM_RL_PHY_CHANNELS          5

/** Number of encoder channels (CH1, CH2, CH3) in 1X */
#define CDMAFW_NUM_ENCODER_CHANNELS         3

/** The BTF adjustment factor to account for the group delay of WAV - 6Cx1
 * (predictive clipping) + 6.75Cx1 (filter ramp up). This should be added to
 * the total BTF calculated by L1 and thus FW's timeline is advanced
 * appropriately */
#define CDMAFW_WAV_GRP_DELAY_BTF_CX8        102

/** RC Configuration Types. */
typedef enum
{
  /** RCs other than 1, 2, 8 */
  CDMAFW_RC_CONFIG__OTHER,
  /** RC8 or 1xAdvanced RL */
  CDMAFW_RC_CONFIG__RC8,
  /** RC1/2 or IS-95 RL */
  CDMAFW_RC_CONFIG__RC12,
  /** IS-95A ACH */
  CDMAFW_RC_CONFIG__R_ACH,

} cdmafw_rc_config_type;

/** Reverse-Link Voice Data Rate Specification  */
typedef enum {

  /** Eighth Rate */
  CDMAFW_DATA_RATE__EGTH,
  /** Quarter Rate */
  CDMAFW_DATA_RATE__QRTR,
  /** Half Rate */
  CDMAFW_DATA_RATE__HALF,
  /** Full Rate */
  CDMAFW_DATA_RATE__FULL

} cdmafw_data_rate_enum_t;

#define MSGR_MODULE_FW              0x00
#define CDMA_MSGR_RL_FW_BASE        0x80
#define MSGR_CDMA_FW                MSGR_TECH_MODULE( MSGR_TECH_CDMA, MSGR_MODULE_FW)

/** UMID definitions for CMD FW messages. */
enum {
  /* The first message in the enum */
  CDMA_FW_RL_CMD_FIRST = (MSGR_UMID_BASE(MSGR_CDMA_FW, MSGR_TYPE_CMD) | CDMA_MSGR_RL_FW_BASE),
  
  /* L1 Reverse Link Configuration Commands */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_SIGNALING,                  0x80, cdmafw_rl_signaling_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_LEGACY_RC_FRAME_CONFIG,     0x81, cdmafw_rl_legacy_rc_frame_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_RC8_FRAME_CONFIG,           0x82, cdmafw_rl_rc8_frame_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_MPP_CONFIG,                 0x83, cdmafw_rl_mpp_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_PWR_FILTER_CONFIG,          0x84, cfw_rl_pwr_est_filter_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_ACK_TEST_MODE_CONFIG,       0x85, cdmafw_rl_ack_test_mode_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_FRAME_CONFIG,               0x86, cdmafw_rl_frame_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_TXFE_TRIGGER,               0x87, cdmafw_rl_txfe_trigger_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_TXFE_STOP,                  0x88, cdmafw_rl_txfe_stop_msg_t),

  /* Debug Commands */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_FPC_OVERRIDE,               0x89, cdmafw_rl_fpc_override_msg_t),

  /* To be deprecated */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_START,                      0x8A, cfw_tx_start_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_STOP,                       0x8B, cfw_tx_stop_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_DAC_START,                  0x8C, cfw_tx_dac_start_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_DAC_STOP,                   0x8D, cfw_tx_dac_stop_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RL_ENC_TRIGGER,                0x8E, cdmafw_rl_enc_trigger_msg_t),
  
  /*! \brief The last message in the enum */
  CDMA_FW_RL_CMD_LAST = CDMA_FW_CMD_RL_ENC_TRIGGER
};

/** Message definitions for RSP messages. */
enum {
  MSGR_DEFINE_UMID(CDMA, FW, RSP, RL_TXFE_TRIGGER,               0x80, cdmafw_rl_txfe_trigger_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, RL_TXFE_STOP,                  0x81, cdmafw_rl_txfe_stop_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, RL_ENC_TRIGGER,                0x82, cdmafw_rl_enc_trigger_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, TX_DAC_START,                  0x8C, cdmafw_tx_dac_start_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, TX_DAC_STOP,                   0x8D, cdmafw_tx_dac_stop_rsp_msg_t),
};
#undef MSGR_MODULE_FW
#undef MSGR_MODULE_RL_FW
#undef MSGR_CDMA_FW

/** Short PN State Configuration.
    
    This structure defines Short PN I/Q state configuration used for initial load 
    at TX trigger time.

    @sa cdmafw_rl_txfe_trigger_msg_t
*/
typedef union 
{ 
  struct {
     
    /** I-channel pilot PN state 
      
        This parameter replaces the corresponding HW regsiter on pre-Bolt targets.
        
    */
    uint32 I_PN_STATE_01: 15; 

    /** Reserved */
    uint32 reserved0 : 17;

    /** Q-channel pilot PN state 
    
        This parameter replaces the corresponding HW register on pre-Bolt targets.
    */
    uint32 Q_PN_STATE_01 : 15; 

    /** Reserved */
    uint32 reserved1 : 17; 

  } ; 

  /** Word access to the I/Q PN states */
  uint32 word[2];

} cdmafw_rl_spn_state_cfg_t;

/** Modulator configuration for a given Physical Channel.

    This structure allows the specification of modulator parameters for a given
    physical channel (that requires encoding).

    These fields are direct passthrough to the WAV VPE and replaces 
    MOD_WCOVER_SEL on pre-Bolt targets. 

*/
typedef struct {

  /** OVSF Walsh Number.

      Please set to the following based on Channel Type.
    
      @verbatim   
        0 : IS95a FCH, IS95a ACH
        1 : CH1, CH3_SF2_SCH, CH3_SF4_SCH
        2 : CH2_IS95C_FCH, CH3_SF8_EACH, CH3_SF8_SCH
        3 : N/A
        4 : CH3_SF16_SCH
      @endverbatim
    
   */
  uint16 ovsfNum;

  /** SPRD Factor.

      @verbatim
        1   => SF8
        2   => SF16
        3   => SF32
        4   => SF64
        5   => SF128
        6   => SF256
        8   => SF1
        9   => SF2 
        10  => SF4
     others => Invalid
      @endverbatim

  */
  uint16 sprdFactor;

  /** Modulation.

      Modulation type for each channel.
      
      @verbatim
        31 => MULTI_BPSK_ON_I  (CH1)
        33 => MULTI_BPSK_ON_IQ (CH2_IS95a)
        32 => MULTI_BPSK_ON_Q  (CH2_IS95c, CH3)
      @endverbatim

  */
  uint16 modulation;

  /** Reserved Padding */
  uint16 rsvd0;

} cdmafw_mod_channel_cfg_t;

/** Legacy RC Params latched at the upcoming frame boundary.
  
    This structure defines the digital gain and rate adjust parameters for
    legacy RCs (RC1-4).

    The digital gains must be specified directly in HW units for the given
    target.
  
*/
typedef struct {

  /** Pilot Channel MOD gain (HW units) */
  uint16 pich; 
  
  /** DCCH MOD gain (HW units) */
  uint16 dcch;
  
  /** FCH MOD gain (HW units) */
  uint16 fch;
  
  /** SCH MOD gain (HW units) */
  uint16 sch;
  
  /** TX Rate Adjustment in -1/640 dB units */
  uint16 txRateAdj;

  /** Reserved Padding */
  uint16 rsvd0;

} cdmafw_legacy_mod_gains_t;

/** T2P digital gains for 1xAdvanced (RC8).

    This structure defines the T2P ratios for RC8 reverse link physical
    channels.
   
*/
typedef struct {

  /** R-FCH to R-PICH linear gain ratio in Q11.
  */
  uint16 fch;  

  /** R-FCHACKCH to R-PICH linear gain ratio in Q11. 
  */
  uint16 fchackch;

  /** R-SCHACKCH to R-PICH linear gain ratio in Q11.

      This parameter is not presently used and is a placeholder for future use.
  */
  uint16 schackch;

  /** R-SCH to R-PICH linear gain ratio in Q11. 
  */
  uint16 sch;

  /** R-DCCH to R-PICH linear gain ratio in Q11.
  */
  uint16 dcch; 

  /** Reserved Padding */
  uint16 rsvd0;

} cdmafw_rc8_t2p_gains_t;

/** Reverse Link Long Code Mask Configuration.
   
   This definition is used in the RL_FRAME_MOD_CONFIG message and specifies
   whether a new LC mask needs to be loaded @ the upcoming frame boundary
   or not, and also allows specification of the 42-bit LC mask to be loaded.

   # of words = 6.
  
*/
typedef struct {
  
  /** Long code state latched @ the upcoming frame boundary.

      This field specifies bits 0-31 of LC state.
  */
  uint32 lcState0;

  /** Long code state latched @ the upcoming frame boundary.

      This field specifies bits 32-41 of LC state.
  */
  uint32 lcState1;

  /** Long Code Mask (0-31).
    
      This field represents the LC mask (bits 0-31) that will be loaded into HW 
      at the upcoming frame boundary.

      Bit(i) of this register corresponds to M(i) of the long code PN mask (please
      refer to section 2.1.3.1.11 of the CDMA1x standard (3GPP2 C.S0002-A)  
      for the definition of M(i)).
     
      This parameter is a direct passthrough to the VPE replaces U_PN_MASK_03
      and U_PN_MASK_45 on pre-Bolt targets.
     
  */
  uint32 lcMask0;

  /** Long Code Mask (32-41)
    
      This field represents the LC mask (bits 32-41) that will be loaded into HW
      at the upcoming frame boundary.

      Bit(i) of this register corresponds to M(i) of the long code PN mask (please
      refer to section 2.1.3.1.11 of the CDMA1x standard (3GPP2 C.S0002-A)  
      for the definition of M(i)).
     
      This parameter is a direct passthrough to the VPE replaces U_PN_MASK_03
      and U_PN_MASK_45 on pre-Bolt targets.
     
  */
  uint32 lcMask1;
  
  /** Short PN state latched into VPE at the upcoming frame boundary. */
  cdmafw_rl_spn_state_cfg_t spnState;

} cdmafw_rl_pn_params_cfg_t;

/** CDMA Modulator Configuration Parameters.
    
    This structure defines the modulator parameters that can be configured
    by L1 SW at each frame boundary. These definitions are also used to 
    initialize the modulator to a known state prior to TX MOD trigger.

    # of words = 16.

    @sa cdmafw_rl_frame_config_msg_t
    @sa cdmafw_rl_txfe_trigger_msg_t
*/
typedef struct {

  union {
    /** Legacy Digital Gains */
    cdmafw_legacy_mod_gains_t legacyGains;
    /** RC8 Traffic to Pilot Ratios */
    cdmafw_rc8_t2p_gains_t rc8T2P;
  };

  /** SPN/LPN params loaded every frame. */
  cdmafw_rl_pn_params_cfg_t pnParams;

  /** Physical Channel Modulator Configuration.

      One set of config parameters for each channel that requires encoding.

      Index 0, 1, 2 correspond to CH1, CH2 and CH3.

      Only channels that are enabled in chEnableMask will be enabled. For disabled
      channels, the channel configuration shall be ignored.

      @sa chEnableMask

   */
  cdmafw_mod_channel_cfg_t channelCfg[CDMAFW_NUM_ENCODER_CHANNELS];

  /** Ping-Pong Index.
 
      Read buffer index for modulator (to implement ping-pong between modulator
      and encoder). Either 0 or 1.
      
      A single index covers all encoder channels.

  */
  uint32 iramIndex;
  
} cdmafw_rl_frame_mod_cfg_params_t;

/** CH1 Encoder/Interleaver configuration.
    
    This structure defines the encoder parameters for CH1 configured at the
    frame boundary.

    Replaces legacy registers MOD_CH1_*.

    # of words = 3.

    @sa cdmafw_rl_frame_config_msg_t
    @sa cdmafw_rl_txfe_trigger_msg_t
*/
typedef struct {

  /* Word #0 */

  /** Frame Size for the DCCH.

      @verbatim
      Register Val  :  Frame Size(ms)
          00        :    5
          01        :    10
          10        :    20
      @endverbatim
   */
  uint32 ch1FrameSize:2;

  /** Encoder Rate for the DCCH.

      @verbatim
      Register Val  : Enc Rate
          0         :  9.6
          1         : 14.4
      @endverbatim
   */
  uint32 ch1EncRate:1;

  /** DCCH radio configuration.

      @verbatim
      Register Val    : Radio Configuration
          0x3         :   RC3 ((cdma2000 1x))
          0x4         :   RC4 ((cdma2000 1x))
          0x5         :   RC5 ((cdma2000 3x))
          0x6         :   RC6 ((cdma2000 3x)
      0x0,0x1,0x2,0x7 :   Undefined
      @endverbatim
  */
  uint32 ch1RadioCfg:3;

  /** DCCH code rate.

      @verbatim
      Register Val    : Code Rate
           00         : 1/2
           01         : 1/3
           10         : 1/4
           11         : Undefined
      @endverbatim
  */
  uint32 ch1CodeRate:2;

  /** CRC length of DCCH.

      These bits are binary representations of associated CRC polynomial length.
      Valid polynomial lengths range from 0 to 12.
      To disable CRC write '00000'
  */
  uint32 ch1CrcLen:5;
 
  /** CRC polynomial for DCCH.

      Polynomials of less than 16 bits have the LSB zero filled to form a
      16bit expression polynomial. This should be re-written with each rate change.
      @verbatim
      Register Val  : CRC Polynomial Length
         0xC867     : 16bit
         0xF130     : 12bit
         0xF640     : 10bit
         0x9B00     : 8bit
         0x1C00     : 6bit (RC2)
         0x9C00     : 6bit (RC3 to RC6)
      @endverbatim
   */
  uint32 ch1CrcPolynomial:16;

  /** Reserved Padding */
  uint32 rsvd0:3;

  /* Word #1 */
 
  /** DCCH Puncture Pattern - bits 16 to 23.

      The value in this field is combined with dcchPunctPattern15to0 and is
      written with either 0xFFFFFF (for no puncturing) or 0xEBBAEA (for 8/24
      puncturing) 
      @verbatim
      Register Val  :   Puncturing Pattern
         0xFF       :   No Puncturing (also for IS-95A)
         0xEB       :   8/24
      @endverbatim
   */
  uint32 dcchPunctPattern23to16:8;

  /** DCCH Puncture Pattern - bits 0 to 15.

      The value in this field is combined with dcchPunctPattern23to16 and is
      written with either 0xFFFFFF (for no puncturing) or 0xEBBAEA (for 8/24
      puncturing) 
      @verbatim
      Register Val  :   Puncturing Pattern
         0xFFFF     :   No Puncturing (also for IS-95A)
         0xBAEA     :   8/24
      @endverbatim
  */
  uint32 dcchPunctPattern15to0:16;

  /** Reserved Padding */
  uint32 rsvd1:8;

  /* Word #2 */
 
  /** Number of Symbols @ Interleaver Input */
  uint32 numIntlvSymbols:16;

  /** Reserved Padding */
  uint32 rsvd2:16;

} cdmafw_rl_enc_ch1_cfg_t;

/** CH2 Encoder/Interleaver Configuration.

    This structure defines the encoder parameters for CH2 configured at the
    frame boundary.

    Replaces legacy registers MOD_CH2_*.

    # of words = 3.

    @sa cdmafw_rl_frame_config_msg_t
    @sa cdmafw_rl_txfe_trigger_msg_t

*/
typedef struct {

  /* Word #0 */

  /** FCH (IS-95A & cdma2000) Encoder Rate.

      These bits specify the data rate for processing of the next frame of
      reverse link data.  
      @verbatim
      Register Val  : Enc Rate
         00         : 1/8
         01         : 1/4
         10         : 1/2
         11         : full (9.6 kbps)
      @endverbatim
  */
  uint32 fchEncRate:2;

  /** FCH channel code rate.

      @verbatim
      Register Val    : Code Rate
           00         : 1/2
           01         : 1/3
           10         : 1/4
           11         : Undefined
      @endverbatim
   */
  uint32 fchCodeRate:2;

  /** CRC length of FCH (for IS-95-A and cdma2000).

      These bits are binary representations of the associated CRC polynomial
      length.  Valid polynomial lengths range from 0 to 16. To disable CRC, 
      write 0000 to these bits. 

      Note: 1 more bit is added to support CRC length of 16 bits for FCH=5
      ms.
  */
  uint32 fchCrcLength:5;
 
  /** Frame size for FCH.

      @verbatim
      Register Val    : Frame Size (ms)
          00          : 5 
          01          : 10 
          10          : 20 
          11          : Undefined
      @endverbatim
  */
  uint32 ch2FrameSize:2;

  /** Configure FCH Radio.

      @verbatim
      Register Val    : Radio Configuration
        0x1           : RC1 ((IS95A))
        0x2           : RC2 ((IS95A))
        0x3           : RC3 ((cdma2000 1x))
        0x4           : RC4 ((cdma2000 1x))
        0x5           : RC5 ((cdma2000 1x))
        0x6           : RC6 ((cmda2000 1x))
        0x7           : RC8 ((cdma2000 1x))
      @endverbatim
  */
  uint32 ch2RadioCfg:3;
 
  /** CRC polynomial for FCH.

      Polynomials of less than 16 bits should have the LS bits zero filled so
      that they form a 16-bit expression polynomial. 
      This register must be re-written with each rate change. 

      @verbatim
      Register Val   : CRC Polynomial Length
        0xC867       : 16bit
        0xF130       : 12bit
        0xF640       : 10bit
        0x9B00       : 8bit
        0x1C00       : 6bit RC2
        0x9C00       : 6bit RC3 to RC6
      @endverbatim
  */
  uint32 ch2CrcPoly:16;

  /** Reserved Padding */
  uint32 rsvd0:2;

  /* Word #1 */
 
  /** Puncture Pattern for FCH (24:16).

      The value in this field is combined with fchPunctPattern15to0 and is
      written with following values for various puncture rates
      @verbatim
      Register Val   : Puncturing Rate
        0x1FF        : No Puncture (for IS95A) 
        0x1E0        : 1/5 
        0x1FE        : 1/9
        0x1D7        : 8/24 
        0x1FF        : 1/25 
      @endverbatim
  */
  uint32 fchPunctPattern24to16:9;
 
  /** Puncture Pattern for FCH (15:0).

      The value in this field is combined with fchPunctPattern24to16 and is
      written with following values for various puncture rates
      @verbatim
      Register Val   : Puncture Rate
        0xFFFF       : No Puncture (for IS95A) 
        0x0000       : 1/5 
        0x0000       : 1/9
        0x75D4       : 8/24 
        0xFFFE       : 1/25 
      @endverbatim
  */
  uint32 fchPunctPattern15to0:16;

  /** ACH Enable. */
  uint32 achEn:1;

  /** FCH (IS-95c) Enable. */
  uint32 fchIs95cEn:1;

  /** FCH Rate (for interleave). 
    
      @verbatim
        0:  1/8
        1:  1/4
        2:  1/2
        3:  Full
      @endverbatim
  */
  uint32 fchRate:2;

  /** Reserved Padding */
  uint32 rsvd1:3;

  /* Word #2 */

  /** Number of Symbols @ Interleaver Input */
  uint16 numIntlvSymbols:16;

  /** Reserved Padding */
  uint16 rsvd2:16;

} cdmafw_rl_enc_ch2_cfg_t;

/** CH3 Encoder/Interleaver Configuration.

    This structure defines the encoder parameters for CH3 configured at the
    frame boundary.

    Replaces legacy registers MOD_CH3_*

    # of words = 3

    @sa cdmafw_rl_frame_config_msg_t
    @sa cdmafw_rl_txfe_trigger_msg_t

*/
typedef struct {

  /* Word #0 */

  /** SCH (IS-95A & cdma2000) Encoder Rate.

      These bits specify the data rate for processing of the next frame of
      reverse link data.  

      Note: When multiframe interleaving by a factor N(=1,2 or 4) the
      data rate to be programmed is given by = actual_data_rate x N.

      @verbatim
      Register Val   : Data Rate
         00000       : 1.5 Kbps
         00001       : 2.7 Kbps
         00010       : 4.8 Kbps
         00011       : 9.6 Kbps
         00100       : 19.2 Kbps
         00101       : 38.4 Kbps
         00110       : 76.8 Kbps
         00111       : 153.6 Kbps
         01000       : 307.2 Kbps
      01001 to 01111 : Undefined
         10000       : 1.8 Kbps
         10001       : 3.6 Kbps
         10010       : 7.2 Kbps
         10011       : 14.4 Kbps
         10100       : 28.8 Kbps
         10101       : 57.6 Kbps
         10110       : 115.2 Kbps
         10111       : 230.4 Kbps
      10111 to 11111 : Undefined
      @endverbatim
  */
  uint32 schEncRate:5;
 
  /** SCH channel code rate.

      @verbatim
      Register Val   : Code Rate
          00         : 1/2
          01         : 1/3
          10         : 1/4
      @endverbatim
  */
  uint32 schCodeRate:2;
 
  /** CRC length of SCH.

      These bits are binary representations of the associated CRC polynomial
      length. Valid polynomial lengths range from 0 to 16.  To disable CRC, 
      write 0x0 To these bits. 

      Note: 1 more bit is added to support CRC length of 16 bits for FCH=5
      ms.
  */
  uint32 schCrcLength:5;

  /** Encoder Type.

      Select between convolutional encoder and the turbo encoder.

      @verbatim
      Register Val   : Encoder Type
           0         : CONVOLUTIONAL_ENCODER
           1         : TURBO_ENCODER
      @endverbatim

  */
  uint32 turboEncode:1;

  /** Frame Size for SCH.

      @verbatim
      Register Val   : Frame Size (ms)
           00        : 5  (only for CCCH, EACH)
           01        : 10 (only for CCCH, EACH, REQCH)
           10        : 20
           11        : Undefined
      @endverbatim
  */
  uint32 ch3FrameSize:2;

  /** Supplemental Channel Enable/Disable.

      These bits enable or disable the supplemental channel
      @verbatim
      Register Val   : CH En/Disable
         000         : Disabled (No SCH or CCH or EACH or REQCH)
         010         : CCH
         011         : EACH
         100         : REQCH
      001, 101, 111  : Undefined
      @endverbatim
  */
  uint32 ch3Sel:3;
 
  /** Additional Encoder control bits for SCH.

      @verbatim
      Register Val   : Encoder Control Bits
         000         : M = 1
         001         : M = 2
         010         : M = 4
         011         : M = 8
         100         : M = 16
         101         : M = 32
         110         : M = 6 (REQCH)
         111         : Undefined
      @endverbatim
  */
  uint32 lowRepeat:3;

  /** Reserved Padding */
  uint32 rsvd0:11;

  /* Word #1 */
 
  /** CRC polynomial for SCH.

      Polynomials of less than 16 bits should have the LS bits zero filled so
      that they form a 16-bit expression polynomial. 
      
      This register must be re-written with each rate change. 

      @verbatim
      Register Value : CRC Polynomial Length
          0xC867     : 16bit
          0xF130     : 12bit
          0xF640     : 10bit
          0x9B00     : 8bit
          0x9C00     : 6bit
      @endverbatim
  */
  uint32 ch3CrcPolynomial:16;
 
  /** Number of Symbols @ Interleaver Input */
  uint32 numIntlvSymbols:16;

  /* Word #2 */

  /** Puncture Pattern for SCH.

      The value in this field is combined with schPunctPattern15to0 and is
      written with following values for various puncture rates.

      @verbatim
      Register Val   : Puncturing Rate
          0xFF       : No Puncture (for IS95A) 
          0xF0       : 1/5 
          0xFF       : 1/9
          0xEB       : 8/24 
          0xD9       : 4/12 (for Conv Enc)
          0xDD       : 4/12 (for Turbo Enc)
          0xEF       : 2/18 (for Conv Enc)
          0xFA       : 2/18 (for Turbo Enc)
      @endverbatim
  */
  uint32 schPunctPattern23to16:8;
 
  /** Puncture Pattern for SCH.

      The value in this field is combined with schPunctPattern23to0 and is
      written with following values for various puncture rates.

      @verbatim
      Register Val   : Puncturing Rate
          0xFFFF     : No Puncture (for IS95A) 
          0x0000     : 1/5 
          0x0000     : 1/9
          0xBAEA     : 8/24 
          0xB000     : 4/12 (for Conv Enc)
          0xA000     : 4/12 (for Turbo Enc)
          0xFF80     : 2/18 (for Conv Enc)
          0xFFC0     : 2/18 (for Turbo Enc)
      @endverbatim
  */
  uint32 schPunctPattern15to0:16;
 
  /** Enable EACH for CH3 */
  uint32 eachEn:1;

  /** SCH Interleaver Size for CH3.
      
      @verbatim 
        0: 1536
        1: 3072
        2: 6144
        3: 12288
      @endverbatim        
  */
  uint32 schIntlvSize:2;

  /** Reserved Padding */
  uint32 rsvd1:5;

} cdmafw_rl_enc_ch3_cfg_t;

/** Encoder/Interleaver Params for CH1, 2 and 3.

    Encoding configuration data is valid only for the channels that are enabled using
    chEnableMask. For disabled channels, the configuration data is ignored.

    Encoding is started @ PCG 14 of the current frame so that it completes in time
    for the next transmission frame.

    # of words = 13.

    @sa chEnableMask

*/    
typedef struct {

  /** Encoder Input Length in 32-bit words.
      
      Indexed as: 0 => CH1, 1 => CH2, 2 => CH3.

  */
  uint32 encInWords[CDMAFW_NUM_ENCODER_CHANNELS];

  /** CH1 enc/intlv configuration */
  cdmafw_rl_enc_ch1_cfg_t ch1;
  
  /** CH2 enc/intlv configuration */
  cdmafw_rl_enc_ch2_cfg_t ch2;
  
  /** CH3 enc/intlv configuration */
  cdmafw_rl_enc_ch3_cfg_t ch3;

  /** IRAM index. 

      Buffer index to write encoder output data. This is used to implement ping-pong action
      between modulator and encoder. Either 0 or 1.

      A single index covers all encoder channels.
  */
  uint32 iramIndex;

} cdmafw_rl_frame_enc_cfg_params_t;

/** Payload Definition for RL_FRAME_CONFIG message.

    This structure defines the payload for the Reverse-Link Frame Config message. 
    This structure is instantiated in the CDMA FW shared memory interface that is 
    write-accessible to SW. 

    The frame configuration parameters convered by this definition are:
      - Encoder parameters.
      - Modulator parameters.
        + Digital Gains and Rate Adjust.
        + LC Mask Updates.
        + Modulator Frame Parameters (spread factor etc.,)
      - DTX Parameters.
      - Power Control Parameters (200/400 Hz for RC8).

    SW must have written all the parameters into SMEM by the time it sends either
    the CDMAFW_CMD_RL_FRAME_CONFIG or CDMAFW_CMD_RL_TXFE_TRIGGER messages to FW.

    # of words = 33.

    @sa cdmafw_rl_frame_config_msg_t
    @sa cdmafw_smem_write_intf_t

*/
typedef struct {

  /** Power Control Pattern.

     This applies to RC8 modes only.
     
     @verbatim
       - 200Hz: 0x4444 
       - 400Hz: 0x5555
       - DTX: per systems recommendation 
     @endverbatim

   */
  uint32 rc8PcPattern;

  /** Transmit PCG Mask for the next frame.
    
      FW will use this value to implement PCG masking for the next
      frame. 

      @verbatim
        BIT(15-i) = 1 => PCG(i) *may* be transmitted.
        BIT(15-i) = 0 => PCG(i) is DTXed.
      @endverbatim

      - For non-DTX'ed and non-SB'ed frames, please set this to 0xFFFF.
      - For SB'ed frames, please set this to 0x9999.
      - For DTX'ed frames, please set this as per Systems Recommendation.

      The transmitPCGMask setting is ANDed with internal FW state to arrive
      at the final transmission state for each PCG.
   
  */
  uint32 transmitPCGMask;

  union {
    
    /** Data Rate (for RC 1/2 configs).
        
        At this time, this parameter needs to be programmed for legacy (RC 1/2)
        modes only. The only purpose of this field is to aid in the determination 
        of the DBR mask for generating blanking patterns for RC1/2 gated mode of 
        operation.

    */
    cdmafw_data_rate_enum_t dataRate;

    /** Padding */
    uint32 word;

  };

  /** Channel Enable Mask.
      
      Specifies which Encoder Channels are enabled. For Channels that are disabled,
      the corresponding modulator channels will also be disabled.

      @verbatim
        BIT(0) => CH1
        BIT(1) => CH2
        BIT(2) => CH3
      @endverbatim

  */
  uint32 chEnableMask;

  /** Encoder Configuration Params. */
  cdmafw_rl_frame_enc_cfg_params_t encParams;
  
  /** Modulator Configuration Parameters.  */
  cdmafw_rl_frame_mod_cfg_params_t modParams;

} cdmafw_rl_frame_config_params_t;

/**@}*/

/**
    \addtogroup cdma1x_rl_msg
    @{
*/

/** Message definition for RL_MPP_CONFIG command.
  
    This message configures the RC8 Max Power Protection (MPP) Algorithm. This
    command is applied immediately.
    
 */
typedef struct {

  /** MSGR header */
  msgr_hdr_struct_type hdr;

  /** MPP Enable flag
    @verbatim
      0: MPP disabled.
      1: MPP enabled.
    @endverbatim
  */
  boolean rliEnableMPP;

  /** Minimum R-FCHACKCH to R-PICH linear power ratio.

      This is the minimum FCHACK to PICH linear power ratio below which the FCHACKCH
      is DTXed. The resolution is 1/(2048*2048) or Q22. 

  */
  uint32 rliFchAckchT2PMin;

  /** Minimum R-SCH to R-PICH linear power ratio.

      This is the minimum SCH to PICH linear power ratio below which the SCH 
      channel is DTXed. The resolution is 1/(2048*2048) or Q22.

  */
  uint32 rliSchT2PMin;
  
} cdmafw_rl_mpp_cfg_msg_t;

/** Message definition for RL_ACK_TEST_MODE_CONFIG command.
  
    This message configures the RC8 ACK test mode which forces the transmission
    of the specified value on the ACK channel.

 */
typedef struct {

  /** MSGR header. */
  msgr_hdr_struct_type hdr;

  /** ACK Test Mode Flag.
      
      @verbatim
        0 => Regular Operation for ACK channel.
        1 => ACK Test Mode Operation.
      @endverbatim

  */
  uint16 rliAckTestMode;
  
  /** ACK Test Mode Value. 

      This parameter replaces the FCH rate info that FW normally determines by 
      trigging early decodes.

      Any non-zero value forces ACK channel to be transmitted (FW assumes that
      early decodes retun a non-erasure result).

  */
  uint16 rliAckTestModeValue;

} cdmafw_rl_ack_test_mode_cfg_msg_t;
    
/** Message definition for RL_SIGNALING command.
  
    This message allows L1 SW to configure static (and/or infrequent) reverse 
    link parameters (i.e., parameters that need not be updated on a 
    frame-by-frame basis). For frame-based dynamic updates, use RL_FRAME_CONFIG 
    command instead.

    Each parameter in the message is accompanied by a _Update flag that allows 
    SW to selectively update parameters (i.e., parameters that have _Update = 0 
    shall be ignored).

    Refer to the definitions inside the structure for more details.

 */
typedef struct {

  /** MSGR header */
  msgr_hdr_struct_type hdr;

  /** Set to 1 to indicate an update of fundFrameOffset.
  */
  boolean fundFrameOffset_Update;

  /** Frame offset (0~15).

      This paramter is latched and applied immediately.
  */
  uint16 fundFrameOffset;

  /** Set to 1 to indicate an update of cTxGainAdjStepSize.
  */
  boolean cTxGainAdjStepSize_Update;

  /** TX Closed Loop Adjust Step Size Parameter.

      This parameter is latched immediately and is in Q9 units.

      @verbatim
        0.25dB: 0x80
        0.5dB : 0x100
        1dB   : 0x200
        2dB   : 0x400
      @endverbatim
  */
  uint16 cTxGainAdjStepSize;

  /** Set to 1 to indicate an update of cTxMaskDelayWr.
  */
  boolean cTxMaskDelayWr_Update;

  /** TX Mask Delay Value.
      
      FW uses this parameter to adjust its RPC interpretation for PCGs that are DTXed
      for any reason. FW stores the transmit status of the last few PCGs in a buffer,
      and uses this delay to determine whether or not to ignore a RPC command from 
      the base-station for a previously DTXed PCG.

      This parameter will be latched at the upcoming frame boundary.
    
  */
  uint16 cTxMaskDelayWr;

  /** Set to 1 to indicate an update of rliRCConfig.
  */
  boolean rliRCConfig_Update;

  /** RL RC Configuration.

    Configures the RC type for the RL. This parameter is latched immediately.

    @verbatim
      0: Other RC Config
      1: RC8 Config
      2: RC1/2 Config (IS-9x)
    @endverbatim

    @sa cdmafw_rc_config_type
  
  */
  cdmafw_rc_config_type rliRCConfig;

  /** Set to 1 to indicate an update of both rliFchAckMask and rliSchAckMask.
  */
  boolean rliAckMask_Update;
  
  /** RL FCH ACK mask.
    
    This parameter specifies the reverse-link FCH ACK mask. The format is as follows:
    
    @verbatim
      Bit(n) is for PCG (15-n), n=0..15.
      Bit(n)=0: R-ACKCH for F-FCH is not allowed to be transmitted in PCG (15-n).
      Bit(n)=1: R-ACKCH for F-FCH is allowed to be transmitted in PCG (15-n).
    @endverbatim

    FW latches the configuration from SW at the frame boundary. The configuration of 
    this variable should be from FOR_FCH_ACK_MASK_RL_BLANKING field or 
    FOR_FCH_ACK_MASK_NO_RL_BLANKING field of Radio Configuration Parameter Message 
    (RCPM). Mobile Station (MS) should not send ACK to Base station on PCG0 and 
    PCG15, so bit 15 and bit 0 should always be 0.

  */
  uint16 rliFchAckMask;

  /** RL SCH ACK mask
    
      This parameter is not yet supported.

  */
  uint16 rliSchAckMask;
  
  /** Set to 1 to indicate an update of rliFPCMask.
  */
  boolean rliFPCMask_Update;

  /** Reverse Link FPC Mask 
    
      This parameter informs FW the PCGs where FPC bit can be transmitted.
      
      Bit(n) is for PCG (15-n), n=0..15.
  */ 
  uint16 rliFPCMask;

  /** Set to 1 to indicate an update of rliDtxRFGatingEn.
  */
  boolean rliDtxRfGatingEn_Update;

  /** Allows 1xL1 to control PA/TX ON gating during DTX.

      This feature is intended to be a debug feature (hopefully controlled using NV).

      @verbatim
         0 => PA/TX ON gating disabled (only digital gains gating used).
         1 => PA/TX ON gating enabled.
      @endverbatim

  */
  boolean rliDtxRfGatingEn;

} cdmafw_rl_signaling_msg_t;

/** Message definition for RL_ENC_TRIGGER command.
   
    This message is sent when SW wants the first frame to be a valid encoded
    frame. We need to have received the cdmafw_rl_frame_config_msg_t prior to
    this trigger to make sure we populate valid params into ENC LMEM from SMEM.
    A response is sent once ENC is done - TxFE trigger can be sent only after
    that. The message can be sent only when the VPE is in sleep state.

    @sa cdmafw_rl_enc_trigger_msg_t

 */
typedef struct {

  /** MSGR Header. */
  msgr_hdr_struct_type hdr;

} cdmafw_rl_enc_trigger_msg_t;

/** Message definition for RL_TXFE_TRIGGER command.

    This message sets up the trigger to start TX transmission. This message has some
    payload parameters that setup the TX trigger timing, in addition to SMEM parameters
    defined in rlFrameConfigParams (cdmafw_rl_frame_config_params_t) that are immediately 
    applied prior to TX start (to allow TX to start in a known state). For example, 
    SW can initialize digital gains to zero and configure the initial LC mask in
    rlFrameConfigParams prior to sending this message.
    
    It is expected that the SMEM parameters in rlFrameConfigParams are settled prior to
    sending this command to FW.

    Please read the description of txTriggerRTC to understand the timing requirements for
    this message.

    @sa cdmafw_rl_txfe_trigger_rsp_msg_t
    @sa txTriggerRTC
    @sa cdmafw_rl_frame_config_params_t

*/    
typedef struct {

  /** MSGR Header. */
  msgr_hdr_struct_type hdr;

  /** RTC count at TX trigger.

      This parameter specifies the "RTC" time at which TX is triggered. This time
      need not correspond to a frame boundary, but it needs to meet the following
      conditions:
        - It must correspond to atleast a 1/2 PCG boundary (in resolution).
        - It must be atleast 2 PCGs ahead of the present RTC to avoid problems
          with missing the TX trigger time which can result in PN mismatch and timing
          errors.

      The initial PN state and LC state specifications in this message must match 
      this trigger time.

      @sa frameBndryRTC

  */
  uint32 txTriggerRTC;

  /** Initial Short PN state loaded at TX trigger.
      
  */
  cdmafw_rl_spn_state_cfg_t initPNState;

  /** Initial LC state (0-31).

      Represents bits 0-31 of LC state.
  */
  uint32 initLCState0;

  /** Initial LC state (32-41).

      Represents bits 32-41 of LC state.
  */
  uint32 initLCState1;

  /** RTC count at a frame boundary.

      This parameter specifies the "RTC" time that corresponds to any frame boundary
      (an example: TX SYNC80 = RX SYNC80 - BTF). This parameter is only used to 
      initialize the TX VSTMRs and doesn't impact the modulator configuration. SW can set 
      this to whichever RTC that should reset frame, PCG and symbol counters.
      
      This parameter along with txTriggerRTC establishes all TX timing. A mismatch 
      between these two RTCs can result in FW and SW being out-of-sync with the 
      modulator with respect to frame timing.

      If txTriggerRTC == frameBndryRTC == TX SYNC 80 => we revert to legacy (pre-Bolt) 
      behavior.
      
      @sa txTriggerRTC
      
  */
  uint32 frameBndryRTC;

} cdmafw_rl_txfe_trigger_msg_t;

/** Message definition for RL_ENC_TRIGGER response.
    
    Response message for RL_ENC_TRIGGER command that indicates encoder is done.

    Currently this is sent only for the special case of ENC being triggered
    via SW CMD in the beginning of time to transmit a valid encoded frame

    @sa cdmafw_rl_enc_trigger_rsp_msg_t

*/ 
typedef struct {

  /** MSGR Header */
  msgr_hdr_struct_type hdr;

} cdmafw_rl_enc_trigger_rsp_msg_t;

/** Message definition for RL_TXFE_TRIGGER response.
    
    Response message for RL_TXFE_TRIGGER command that indicates trigger complete.

    The message may contain useful logging information in the future.

    @sa cdmafw_rl_txfe_trigger_msg_t

*/  
typedef struct {

  /** MSGR Header */
  msgr_hdr_struct_type hdr;

} cdmafw_rl_txfe_trigger_rsp_msg_t;


/** Message definition for RL_TXFE_STOP command.

    This message tells FW to stop TX. This command will execute in the upcoming
    frame boundary which means that FW will complete sending out the current 
    frame in which this command is received.

    Once the command completes, FW will send a response back to SW to indicate
    that all TX front-end operations have ceased.

    @sa cdmafw_rl_txfe_stop_rsp_msg_t

*/  
typedef struct {
  
  /** MSGR Header. */
  msgr_hdr_struct_type hdr;

} cdmafw_rl_txfe_stop_msg_t;

/** Message definition for RL_TXFE_STOP response.

    This is the response message for RL_TXFE_STOP command and informs SW that all 
    TX front-end operations have ceased. SW is free to completely shut down TX
    at this point (i.e., send RF stopTX command).

    @sa cdmafw_rl_txfe_stop_msg_t

*/    
typedef struct {

  /** MSGR Header */
  msgr_hdr_struct_type hdr;

} cdmafw_rl_txfe_stop_rsp_msg_t;

/** Message definition for TX_DAC_START response.

    This response is sent on completion of a TX_DAC_START trigger.
*/
typedef struct {

  /** MSGR Header */
  msgr_hdr_struct_type hdr;

} cdmafw_tx_dac_start_rsp_msg_t;

/** Message definition for TX_DAC_STOP response.

    This response is sent on completion of a TX_DAC_STOP trigger.
*/
typedef struct {

  /** MSGR Header */
  msgr_hdr_struct_type hdr;

} cdmafw_tx_dac_stop_rsp_msg_t;

/** Message definition for RL_FRAME_CONFIG command.
 
    This message allows SW to specify reverse link dynamic parameters that change
    on a frame-by-frame basis (static parameters are covered by the RL_SIGNALING
    command). 
    
    This message is just an indication for FW to latch SW parameters that have 
    been already setup in SMEM under cdmafw_rl_frame_config_params_t, and hence, 
    it has no payload.
    
    The frame configuration parameters setup by this message include:
      - Encoder parameters.
      - Modulator parameters.
        + Digital Gains and Rate Adjust.
        + LC Mask Updates.
        + Modulator Frame Parameters (spread factor etc.,)
      - DTX Parameters.
      - Power Control Parameters (200/400 Hz for RC8).
        
    The parameters in this message shall be applied to the upcoming frame (offset-
    SYNC20). SW must send this message atleast 1.5 PCGs ahead of the upcoming frame
    boundary to ensure correct operation. FW *may* crash if this message were 
    received at an incorrect time.

    @sa cdmafw_rl_frame_config_params_t
   
*/
typedef struct {

  /** MSGR Header. */
  msgr_hdr_struct_type hdr;

} cdmafw_rl_frame_config_msg_t;

/** Message definition for TX FPC override (ONLY intended for standalone TX debug/bit-exact).

    This message is intended only for debug/RUMI bit-exact matching purposes, and should
    not be relied on for mission-mode operation. It will override the FPC commands sent
    to BS based on a provided frame pattern (for freezing FPC, it can set to 0x5555, for 
    example).

    For mission-use, use the cdmafw_fpc_override_msg_t message instead.

    @sa cdmafw_fpc_override_msg_t

*/
typedef struct {

  /** MSGR Header */
  msgr_hdr_struct_type hdr;

  /** FPC Override Pattern to use when override flag is set. 
        
      PCG(i) shall use bit (15-i) for FPC input. Based on convention, 1=> DOWN, 0 => UP.
  
  */
  uint32 fpcOvrPattern;

  /** FPC Override flag (when set, will use fpcOvrPattern for FPC commands). */
  boolean fpcOverride;

} cdmafw_rl_fpc_override_msg_t;


/**@}*/

/**
    \addtogroup cdma1x_rl_msg
    @{
*/    
/** Message definition for RL_LEGACY_RC_FRAME_CONFIG command.
 
*/
typedef struct {

  /** MSGR header */
  msgr_hdr_struct_type hdr;

  /** R-PICH digital channel gain.
      FW latches the configuration from SW at the frame boundary.
  */
  uint16 MOD_PCH_GAIN; 

  /** R-DCCH digital channel gain.
      FW latches the configuration from SW at the frame boundary.
  */
  uint16 MOD_DCCH_GAIN;

  /** R-FCH digital channel gain.
      FW latches the configuration from SW at the frame boundary.
  */
  uint16 MOD_FCH_GAIN;  

  /** R-SCH digital channel gain.
      FW latches the configuration from SW at the frame boundary.
  */
  uint16 MOD_SCH_GAIN;

  /** TX_RATE_ADJ.
      FW latches the configuration from SW at the frame boundary. \n
      The unit is -1/640 dB.
  */
  uint16 cTxRateAdjWr;

  /** Frame Valid Indicator 
      FW uses this indicator to appropriately turn ON or OFF the 
      TX_VALID and PA_VALID signals. This signal will be latched by FW
      at PCG15, symbol10 of a frame and will be used to control PA/TX
      valid signals from that point onwards.

      When TX/PA have to be turned OFF, SW must ensure that the digital
      gains that are sent in this message are also zeroed out.
  */
  boolean cTxFrameValid;

  /** Transmit PCG Mask for the next frame.
    
      FW will use this value to implement PCG masking for the next
      frame. 
      
      @verbatim
        BIT(15-i) = 1 => PCG(i) is transmitted.
        BIT(15-i) = 0 => PCG(i) is DTXed.
      @endverbatim

      - For non-DTX'ed frames, please set this to 0xFFFF.
      - For DTX'ed frames, please set this as per Systems Recommendation.

      This parameter will be latched by FW @ symbol-16 of the last PCG of 
      the current frame, and shall be applied to the next frame.

      Please note cTxFrameValid = 0 setting *overrides* this parameter. 
      Basically, when SW wishes to turn TX completely off (usually happens
      when tearing down TX), SW can set cTxFrameValid = 0, and FW will 
      ignore any rliTransmitPCGMask setting and turns off TX.
    
   */
  uint16 rliTransmitPCGMask;


  /** Flag to indicate whether to latch transmitPCGMask and frameValid
      value from L1 in nearest symbol 16 or at frame boundary. Set only in 
      case of 1x->X LTA Gaps for timeline optimizations. 
  */
  boolean immediateLatch;

} cdmafw_rl_legacy_rc_frame_config_msg_t;

/** Message definition for RL_RC8_FRAME_CONFIG command.

*/
typedef struct {

  /** MSGR header */
  msgr_hdr_struct_type hdr;

  /** R-ACKCH to R-PICH linear gain ratio in Q11. 
      
      FW latches the configuration from SW at the upcoming frame boundary. 
  */
  uint16 rliAckchToPich;

  /** R-FCH to R-PICH linear gain ratio in Q11.
    
      FW latches the configuration from SW at the upcoming frame boundary.
  */
  uint16 rliFchToPich;  

  /** R-SCH to R-PICH linear gain ratio in Q11. 
    
      FW latches the configuration from SW at the upcoming frame boundary. 
  */
  uint16 rliSchToPich; 

  /** R-DCCH to R-PICH linear gain ratio in Q11.
    
      FW latches the configuration from SW at the upcoming frame boundary.
  */
  uint16 rliDcchToPich; 

  /** Power control pattern for the next transmitted frame.

      FW latches the configuration from SW at the upcoming frame boundary.

      @verbatim
        - 200Hz: 0x4444 
        - 400Hz: 0x5555
        - DTX: per systems recommendation.
      @verbatim

  */
  uint16 rliPCPattern;

  /** Blanking flag for the next transmitted frame.
    
      @verbatim
        0: not blanked
        1: blanked
      @endverbatim
      
      FW latches the configuration from SW at the frame boundary.
      
      @deprecated This parameter is deprecated.
  */
  uint32 rliBlankedFrame:1 __attribute__((deprecated));

  /** Frame Valid Indicator 
   
      FW uses this indicator to appropriately turn ON or OFF the 
      TX_VALID and PA_VALID signals. This signal will be latched by FW
      at PCG15, symbol10 of a frame and will be used to control PA/TX
      valid signals from that point onwards.

      When TX/PA have to be turned OFF, SW must ensure that the digital
      gains that are sent in this message are also zeroed out.

      When set to 0, this parameter overrides rliTransmitPCGMask.
  */
  boolean cTxFrameValid;

  /** Transmit PCG Mask for the next frame.
    
      FW will use this value to implement PCG masking for the next
      frame. 

      @verbatim
        BIT(15-i) = 1 => PCG(i) is transmitted.
        BIT(15-i) = 0 => PCG(i) is DTXed.
      @endverbatim

      - For non-DTX'ed and non-SB'ed frames, please set this to 0xFFFF.
      - For SB'ed frames, please set this to 0x9999.
      - For DTX'ed frames, please set this as per Systems Recommendation.

      This parameter will be latched by FW @ symbol-16 of the last PCG of 
      the current frame, and shall be applied to the next frame.

      Please note cTxFrameValid = 0 setting *overrides* this parameter. 
      Basically, when SW wishes to turn TX completely off (usually happens
      when tearing down TX), SW can set cTxFrameValid = 0, and FW will 
      ignore any rliTransmitPCGMask setting and turns off TX.
    
   */
  uint16 rliTransmitPCGMask;

  /** Flag to indicate whether to latch transmitPCGMask and frameValid
      value from L1 in nearest symbol 16 or at frame boundary. Set only in 
      case of 1x->X LTA Gaps for timeline optimizations.
  */
  boolean immediateLatch;

} cdmafw_rl_rc8_frame_config_msg_t;
/**@}*/

#endif
