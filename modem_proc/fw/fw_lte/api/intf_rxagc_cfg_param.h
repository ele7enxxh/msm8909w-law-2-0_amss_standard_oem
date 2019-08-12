/*!
  @file
  intf_rxagc_cfg_param.h

  @brief
  This file contains LTE LL search config parameters definitions/enumaration

  @detail
  description of functions in this file
 
*/

/*===========================================================================

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_rxagc_cfg_param.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef INTF_RXAGC_CFG_PARAM_H
#define INTF_RXAGC_CFG_PARAM_H

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/

/*! @brief List of RX AGC configuration parameters consisting of 
    parameter names and default values.  enum names is parameter name
    preceded by LTE_LL1_RXAGC_CFG_ prefix.  The default values
    are used to initialize configuration parameter array.

    To add more parameters, simply add entries to 
    LTE_LL1_RXAGC_CFG_PARAM_LIST macro.  Adding to end
    of list does not change enumeration values (though this is
    not required). Parameter data structure and default value
    initialization is handled automatically.
    XXX - Reference for these paramerters (system HLD, ODYSIM code)!
*/

#define LTE_LL1_RXAGC_CFG_PARAM_LIST( entry )                 \
  /* parameter name */               /* default val */        \
  /* RX AGC parameter during ONLINE condition */              \
  entry( INNER_LP_FILT_COEF,               0x028F )       \
     /*!<Range:(0 to 0x7fff,ffff), linear scale frac value,
         Qformat = 32S31, default value = 0x028F5C28 (0.02)*/ \
  entry( DVGA_SET_POINT_DBFS,              0xDC000000 )     /*DC000000 -36dB, DF000000 -33dB  E2000000 -30dB E5000000 -27dB E8000000 -24dB*/  \
     /*!<Range:-128 to +127 dB, dB scale, Qformat = 32S24
                             default value= -30*2^24 */       \
  entry( HYB_EE_FFT_COEF,                  0xffffffff )       \
     /*!<Range:(0 to 0xffff,ffff), linear scale frac value,
         Qformat =32U32, default value =
                            (no FFT usage for now)*/          \
  entry( HYB_EE_CRS_COEF,                  0xffffffff )       \
     /*!<Range:(0 to 0xffff,ffff), linear scale frac value,
         Qformat =32U32, default value = 1 in 32U32 */        \
  entry( ERR_ACC_LIMIT_MIN,                0xD0000000 )       \
     /*!<Range:-128 to 127 dB, dB scale, Qformat=32S24,
        dafault value=-48<<24 */                              \
  entry( ERR_ACC_LIMIT_MAX,                0x30000000 )       \
     /*!<Range:-128 to 127 dB, dB scale, Qformat=32S24,
        dafault value= 48<<24        */                       \
  /* RX AGC parameter for FTM mode   */                       \
  entry( IS_FTM_MODE,                               0 )       \
     /*!<Range:(0=NON_FTM /1=FTM), boolean value,
         default value = 0              */                    \
  entry( FFT_EE_FILT_COEF,          0xA6666666 )              \
     /*!<Range:0 to .99999, Qformat = 32U32
                             default value= .65 */            \
  entry(IS_FFT_EE_FILT_RST_MODE,                    1 )       \
     /*!<Range:(0=No reset of FFT EE filtered value at SF end/
        1=reset FFT EE filtered value at SF end), 
        boolean value,
        default value = 1              */                     \
  entry( TOT_PWR_FILT_COEF,         0x28F5C28 )               \
     /*!<Range:0 to .99999, Qformat = 32S31
                             default value= .02 */            \
  entry(IS_BB_EE_IIR_MODE,                    0 )             \
     /*!<Range:(0=BB EE is in INTEGRATE & DUMP mode/
        1=BB EE is in IIR mode), 
        boolean value,
        default value = 0              */                     \
  entry( INNER_LP_FILT_COEF_FAST,          0x4CCC )       \
     /*!<Range:(0 to 0x7fff,ffff), linear scale frac value,
         Qformat = 32S31, default value = 0x0FFFFFFF (0.60)*/ \
  entry( DVGA_SET_POINT_OFF_MBSFN_DBFS,    0xFA000000 )       \
     /*!<Range:-128 to +127 dB, dB scale, Qformat = 32S24
         default value= -6*2^24, added to DVGA_SET_POINT_DBFS
         in MBSFN  */                                         \

/*lint --e{123} Macros are used to select one of two parameters */  
#define LTE_LL1_RXAGC_CFG_ENUM( name, default_val )          LTE_LL1_RXAGC_CFG_##name,
#define LTE_LL1_RXAGC_CFG_DEFAULT_VAL( name, default_val )   (uint32)default_val,
#define LTE_LL1_RXAGC_CFG_STRUCT_MEMBER( name, default_val ) uint32 name;


/*===========================================================================

      Typedefs

===========================================================================*/


/*! @brief Enum of RXAGC configuration parameters that can be specified
    by lte_LL1_RXAGC_CFG_param req message.  The enums are also used 
    as index into array in rxagc data structure.
*/
typedef enum
{
  LTE_LL1_RXAGC_CFG_PARAM_LIST( LTE_LL1_RXAGC_CFG_ENUM )
  LTE_LL1_RXAGC_CFG_PARAM_COUNT,
  LTE_LL1_RXAGC_CFG_MAKE_PARAM_INT32=0x7fffffff ///<last enum element to make it int32
} lte_LL1_rxagc_cfg_param_enum;


/*! @brief RXAGC Configuration element structure. Req message can contain an array of 
    these structures for configuration LL1 modules.
    Note: Currently all element are int32
*/
typedef struct
{
  lte_LL1_rxagc_cfg_param_enum index;     ///< index for specifying configuration parameter
  int32   value;                          ///< new value of configuration parameter
} lte_LL1_rxagc_cfg_element_struct_t;


/*! @brief RXAGC Configuration element structure has to have the same size as of the general
    lte_LL1_config_element_struct. Also assumed that this structure include an enum typedef
    and a value.
*/

/*! @brief RXAGC Configuration element structure. Req message can contain an array of 
    these structures for configuration LL1 modules.
    Note: Currently all element are int32
*/
typedef struct
{
  lte_LL1_rxagc_cfg_param_enum index;     ///< index for specifying configuration parameter
  int32   value;                          ///< new value of configuration parameter
} lte_LL1_rxagc_cfg_param_element_struct_t;



/*! @brief Structure of RXAGC config parameters. Note this structure
           is not used in ML-LL interface.  It is used for LL1 to access 
           configuration parameters.  Structure member are synchronized
           with array of parameters indexed by config parameter enum.
*/
typedef union
{
  struct
  {
    LTE_LL1_RXAGC_CFG_PARAM_LIST( LTE_LL1_RXAGC_CFG_STRUCT_MEMBER )
  };
  struct
  {
    LTE_LL1_RXAGC_CFG_PARAM_LIST( LTE_LL1_RXAGC_CFG_STRUCT_MEMBER )
  } param_struct;
  uint32 array[ LTE_LL1_RXAGC_CFG_PARAM_COUNT ];
} lte_LL1_rxagc_cfg_param_t;


#endif /* INTF_RXAGC_CFG_PARAM_H */
