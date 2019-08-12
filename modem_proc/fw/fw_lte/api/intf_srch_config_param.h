/*!
  @file
  intf_srch_config_param.h

  @brief
  This file contains LTE LL search config parameters definitions/enumaration

  @detail
  description of functions in this file
 
*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_srch_config_param.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef INTF_SRCH_CONFIG_PARAM_H
#define INTF_SRCH_CONFIG_PARAM_H

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/
/* --------------------------------------------------------------------------
*
* Request, confirmation and indication message ID enums
*
* --------------------------------------------------------------------------*/

#define LTE_LL1_SRCH_CONFIG_INIT_ACQ_HW_PKS_MAX  16
#define LTE_LL1_SRCH_CONFIG_NCELL_HW_PKS_MAX     16

#define LTE_LL1_SRCH_CONFIG_MAX_NUM_HW_PKS_INIT_ACQ_NCELL (LTE_LL1_SRCH_CONFIG_INIT_ACQ_HW_PKS_MAX)

/*! @brief List of SRCH configuration parameters consisting of 
    parameter names and default values.  enum names is parameter name
    preceded by LTE_LL1_SRCH_CONFIG_ prefix.  The default values
    are used to initialize configuration parameter array.

    To add more parameters, simply add entries to 
    LTE_LL1_SRCH_CONFIG_PARAM_LIST macro.  Adding to end
    of list does not change enumeration values (though this is
    not required). Parameter data structure and default value
    initialization is handled automatically.

    For more details refer to Odyssey Physical layer Control HLD, 
    document number: 80-VM320-1 (in Agile) for initial acquisition
    and its flowchart
*/
// XXX: need to get default values
#define LTE_LL1_SRCH_CONFIG_PARAM_LIST( entry ) \
      /* parameter name */               /* default val */      \
  /* Odyssey only Parameters      */                            \
                                                                \
  /* Init Acquisition parameters */                             \
  entry( INIT_ACQ_PSS_TH,                         0 )  \
          /* XXX - check on this with Shiva */                  \
     /*!<Range:0 to 0xffff , linear scale, Threshold used on PSS 
       correlation values in HW PSS correlation engine */       \
  entry( INIT_ACQ_SSS_TH,                        72 )  \
     /*!<Range:(0 to 0xffff) , linear scale, Init Acq SSS
         threshold to be used in DSP SSS detection */           \
  entry( INIT_ACQ_MAX_SSS_SNR_TH,               810 )  \
     /*!<Range:(256 to 65535, corrspond to 0 to 24 dB), linear
         scale, Init Acq MAX SSS SNR threshold to be used in
         DSP SSS detection, 810 correspond to 5 dB) */          \
  entry( INIT_ACQ_PSS_CH_EST_MAG_ABS_TH,          0 )  \
              /* XXX - check with Shiva on these 2 below */     \
     /*!<Range:-6 to +6 dB, linear scale, default value=0 (all taps are used)  
         Init Acq Magnitude threshold applied to the PSS based
         channel estimate in time domain */                     \
  entry( INIT_ACQ_PSS_CH_EST_MAG_RELATIVE_TH,     0 )  \
     /*!<Range:-6 to +6 dB, linear scale, default value=0 (all taps are used)
         Init Acq Magnitude threshold applied to the PSS based
         channel estimate in time domain */                     \
  entry( INIT_ACQ_PSS_CH_EST_CIR_NTAPS_TH,        8 )  \
     /*!<Range: 1 to 16 (CIR time domain), Init Acq CIR number
         of taps threshold applied to the PSS based channel
         estimate in time domain. */                            \
  entry( INIT_ACQ_FREQ_BIN_D,                  3000 )  \
     /*!<Range:0 to 3000Hz, Frequency bin d Hz,
         initial Max freq error [-F to +F]Hz divided into 
         equally spaced bins of width FREQ_BIN_d Hz (FREQ_BIN_d>=0). */  \
  entry( INIT_ACQ_PSS_HW_PKS,                    16 )  \
     /*!<Range (1 to 16), Init acq search number of PSS peaks
         that HW has to send to FW */                           \
  entry( INIT_ACQ_MAX_PSS_FW_PKS,                 5 )  \
    /*!<Range (0 to 32), default value= 5. Max number of 
        PSS peaks to be used for SSS detection */               \
  entry( INIT_ACQ_PRUNING_X_DB,                   1 )  \
     /*!<Range: , linear scale, Initial Acq x dB threshold
         to be used for pruning the PSS correlation sorted peaks */  \
  entry( INIT_ACQ_PRUNING_DELTA,                  1 )  \
    /*!<Range:x ,  Initial Acq Number of samples between
        adjacent sorted peaks to be used for pruning. */        \
                                                                \
  /* Common only Parameters      */                             \
                                                                \
  /* Neighbor search parameters */                              \
  entry( NEIGH_PSS_TH,                            0 )  \
          /* XXX - check on this with Shiva */                  \
    /*!<Range:0 to 0xffff , linear scale, Threshold used on PSS
        correlation values in HW PSS correlation engine */      \
  entry( NEIGH_SSS_TH,                           72 )  \
     /*!<Range:(0 to 0xffff) , linear scale, Neighbor search SSS
         threshold to be used in DSP SSS detection  */          \
  entry( NEIGH_MAX_SSS_SNR_TH,                 65535)  \
     /*!<Range:(256 to 65535, corrspond to 0 to 24 dB), linear
         scale, ncell search MAX SSS SNR threshold to be used in
         DSP SSS detection */                                   \
           /* XXX - check with Shiva on these 2 below */        \
  entry( NEIGH_PSS_CH_EST_MAG_ABS_TH,             0 )  \
     /*!<Range:-6 to +6 dB, linear scale, default value=0 (all taps are used)
         Neighbor search Magnitude threshold applied to
         the PSS based channel estimate in time domain */       \
  entry( NEIGH_PSS_CH_EST_MAG_RELATIVE_TH,        0 )  \
     /*!<Range:-6 to +6 dB, linear scale, default value=0 (all taps are used)
         Neighbor search Magnitude threshold applied to
         the PSS based channel estimate in time domain */       \
  entry( NEIGH_PSS_CH_EST_CIR_NTAPS_TH,           8 )  \
     /*!<Range: 1 to 16 (CIR time domain), Neighbor CIR number of taps
         threshold applied to the PSS based channel estimate in time domain. */ \
  entry( NEIGH_PSS_HW_PKS,                       16 )  \
     /*!<Range (0 to 32), Neighbor search number of PSS peaks
         that HW has to send to FW  */                          \
  entry( NEIGH_MAX_PSS_FW_PKS,                    8 )  \
     /*!<Range (0 to 32), default value=8, Neighbor search Max
         num of PSS peaks to be used for SSS detection */       \
  entry( NEIGH_PRUNING_X_DB,                      2 )  \
     /*!<Range: x, linear scale, Neighbor search x dB threshold
         to be used for pruning the PSS correlation sorted peaks */  \
  entry( NEIGH_PRUNING_DELTA,                     1 )  \
     /*!<Range: x, Neighbor search Number of samples between
         adjacent sorted peaks to be used for pruning. */       \
  entry( NEIGH_NUM_PEAK_SORTS_TIME_OUT,           1 )  \
     /*!<Range: (1 to 5), Max number of times to repeat peak sort on 
         existing non-coherent buffer contents based on number of detected
         peaks (if there are not enough peaks, start the next peak sorting
         using the smallest peak as the new NCAS peak threshold), search always
         starts with NCAS peak threshold set to its max value */  \
                                                                \
  /* Esprsesso Specific parameters */                           \
  entry( INIT_ACQ_CORR_WIN_LEFT,                  4 )  \
    /*!<Range: x, default value= x, Boundary (to the left) for the window
        of PSS correlation peaks to be used for frequency offset estimation,
        Number of samples at 1.92 MHz sampling rate  */         \
  entry( INIT_ACQ_CORR_WIN_RIGHT,                 4 )  \
    /*!<Range: x, default value= x,Boundary to the right
        Number of samples at 1.92 MHz sampling rate */          \
  entry( INIT_ACQ_COH_ACC_PSC,                    4 )
     /*!<Range: <default value=x, Coherent accumulation length
         used for PSC detection correlation engine in HW.  */


#define LTE_LL1_SRCH_CONFIG_ENUM( name, default_val )          LTE_LL1_SRCH_CONFIG_##name,
#define LTE_LL1_SRCH_CONFIG_DEFAULT_VAL( name, default_val )   (uint16)default_val,
#define LTE_LL1_SRCH_CONFIG_STRUCT_MEMBER( name, default_val ) uint16 name;


/*===========================================================================

      Typedefs

===========================================================================*/


/*! @brief Enum of SRCH configuration parameters that can be specified
    by lte_LL1_srch_config_param req message.  The enums are also used 
    as index into array in srch data structure.
*/
typedef enum
{
  LTE_LL1_SRCH_CONFIG_PARAM_LIST( LTE_LL1_SRCH_CONFIG_ENUM )  /*lint !e123 */
  LTE_LL1_SRCH_CONFIG_PARAM_COUNT,
  LTE_LL1_SRCH_CONFIG_MAKE_PARAM_INT32=0x7fffffff ///<last enum element to make it int32
} lte_LL1_srch_config_param_enum;


/*! @brief SRCH Configuration element structure. Req message can contain an array of 
    these structures for configuration LL1 modules.
    Note: Currently all element are int32
*/
typedef struct
{
  lte_LL1_srch_config_param_enum index;   ///< index for specifying configuration parameter
  int32   value;                          ///< new value of configuration parameter
} lte_LL1_srch_config_element_struct_t;


/*! @brief SRCH Configuration element structure has to have the same size as of the general
    lte_LL1_config_element_struct. Also assumed that this structure include an enum typedef
    and a value.
*/
//COMPILE_ASSERT( sizeof(lte_LL1_srch_config_element_struct_t) == sizeof(lte_LL1_config_element_struct) );

/*! @brief Structure of SRCH config parameters. Note this structure
           is not used in ML-LL interface.  It is used for LL1 to access 
           configuration parameters.  Structure member are synchronized
           with array of parameters indexed by config parameter enum.
*/
typedef union
{
  struct
  {
    LTE_LL1_SRCH_CONFIG_PARAM_LIST( LTE_LL1_SRCH_CONFIG_STRUCT_MEMBER )  /*lint !e123 */
  };
  struct
  {
    LTE_LL1_SRCH_CONFIG_PARAM_LIST( LTE_LL1_SRCH_CONFIG_STRUCT_MEMBER )  /*lint !e123 */
  } param_struct;
  uint16 array[ LTE_LL1_SRCH_CONFIG_PARAM_COUNT ];
} lte_LL1_srch_config_param_t;


#endif /* INTF_SRCH_CONFIG_PARAM_H */
