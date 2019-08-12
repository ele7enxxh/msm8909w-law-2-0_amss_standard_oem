#ifndef WCDMA_COMMON_API_H
#define WCDMA_COMMON_API_H
/*===========================================================================
   FILE: wcdma_common_api.h

GENERAL DESCRIPTION:
      This header file contains WCDMA parameter definitions needed by modules 
      outside of WCDMA Access Stratum.
      
      
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c)2011 by Qualcomm Technologies, Inc. All Rights Reserved.
      
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/wcdma_common_api.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/23/14   bj      Put the prototype for wcdma_get_rat_meas_info.
06/12/14   tsk     Added Jolokia featurization for FTM related changes.
06/08/14   ar      Added FTM support for EUL
02/13/14   gp      Added FTM Support for Uplink
02/22/13   ash     Changed comment for tx_pwr to indicate the unit in dBm
06/06/12   amj     Added new API to start and stop scanning RF data.
05/31/12   at      Added new CM API support for PA gain state and correct TX power support
04/20/11   sv      Fixed error in featurization name. 
04/19/11   sv      Added API definition to get WCDMA serving cell and neighbor info. 
03/14/11    vsr     Added API to disable/enable RxD for data calls
03/18/11   sv      Fixed a compiler warning.
03/17/11   sv      First cut of WCDMA definitions needed by non W AS modules.
 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "cmapi.h"

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/
/* The invalid signal info value */
#define WCDMA_SIGNAL_INFO_INVALID_VALUE   0x80000000

/* The invalid band and channel value */
#define WCDMA_BAND_INVALID_VALUE    0xFFFFFFFF
#define WCDMA_CHANNEL_INVALID_VALUE 0xFFFFFFFF

/* Minimum WCDMA RX Power Level in db10 units */
#define WCDMA_MIN_AGC_VAL         -512
/* Minimum WCDMA RX Power Level based on UE dynamic range in dbm */
#define WCDMA_MIN_RX_POWER_DBM    -106
/* Minimum WCDMA TX Power Level based on UE dynamic range in dbm */
#define WCDMA_MIN_TX_POWER_DBM    -70

/* Macro for creating a bitmask by shifting '1' left by a  
    Specified number indicated by 'val'. 
*/
#define WCDMA_BM( val ) ( 1<< ((int)(val)) )

/* Structure of RX signal info parameters per each RX chain */
typedef struct wcdma_rx_chain_info_s
{
  /* Whether RX is tuned to a channel. If radio is tuned,
     instantenuous values will be set for below signal info  
     fields.If radio is not tuned, delayed or invalid values 
     will be set depending on each technology.
  */
  boolean is_radio_tuned;

  /* RX power value in 1/10 dbm resolution */
  int32   rx_pwr;

  /* Ec/Io in negtive 1/10 dbm. */
  int32   ecio;

  /* Received Signal Code Power in 1/10 dbm */
  int32   rscp;
 
} wcdma_rx_chain_info_struct_type;

/* Enumeration of RX chains supported */
typedef enum
{
  WCDMA_PRIM_CHAIN,
  WCDMA_DIV_CHAIN,
  WCDMA_RX_CHAIN_MAX
} wcdma_rx_chain_enum_type;

/* Enumeration of RX chain diversity info */
typedef enum 
{
  /* Signal info are not available on all RX chains */
  WCDMA_RX_CHAIN_UNAVAIL = 0,

  /* Signal info on WCDMA_PRIM_CHAIN is available */
  WCDMA_PRIM_CHAIN_AVAIL = WCDMA_BM(WCDMA_PRIM_CHAIN),

  /* Signal info on WCDMA_DIV_CHAIN is available */
  WCDMA_DIV_CHAIN_AVAIL = WCDMA_BM(WCDMA_DIV_CHAIN),

  /* Signal info on all RX chains are available */
  WCDMA_RX_CHAIN_ALL_AVAIL = (WCDMA_BM(WCDMA_PRIM_CHAIN) | WCDMA_BM(WCDMA_DIV_CHAIN)),

  WCDMA_RX_CHAIN_INVALID = WCDMA_SIGNAL_INFO_INVALID_VALUE

} wcdma_rx_diversity_bit_mask;

/* Structure of RX power related info */
typedef struct wcdma_rx_pwr_info_s
{
  /* Diversity indicator to show which RX chain has valid 
     signal info. Used to indicate RX power available info
     of below 2 fields
  */
  wcdma_rx_diversity_bit_mask rx_diversity_indicator;

  /* Rx signal info */
  wcdma_rx_chain_info_struct_type rx_pwr_chain[WCDMA_RX_CHAIN_MAX];
} wcdma_rx_pwr_info_struct_type;

/* Structure of TX power related info */
typedef struct wcdma_tx_pwr_info_s{

  /* Whether device is in traffic. tx_pwr is only meanful 
     when in traffic. If no traffic tx_pwr is invalid
  */
  boolean is_in_traffic;

  /* TX power value in dbm */
  int32   tx_pwr;
} wcdma_tx_pwr_info_struct_type;

typedef struct
{
  /* Active band of current system */
  int32                band;

  /* Active channel of current system */
  int32                channel;

  /* Rx Power info of the specified system mode */
  wcdma_rx_pwr_info_struct_type   rx_power;

 /* Tx Power info of the specified system mode */
  wcdma_tx_pwr_info_struct_type   tx_power;

  /* RF should define the PA state meaning, especially PA off state */
  uint8 pa_state;

} wcdma_signal_info_struct_type;

/*===========================================================================
                        FUNCTION DECLARATIONS
===========================================================================*/
extern wcdma_signal_info_struct_type* wcdma_get_signal_info(void);

/*===========================================================================
FUNCTION rxd_wcdma_oride_rxdiversity_for_ps_data
         
DESCRIPTION
  This API is used to disable/enable RxD in a data call. Input param is 
  boolean disable. Set it to TRUE to vote for rxd to be disabled for ps/data 
  calls. If same API is called with argument FALSE, it resets the previous vote

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rxd_wcdma_oride_rxdiversity_for_ps_data(boolean disable);

/*===========================================================================

FUNCTION      WCDMA_START_CAPTURE_DATA

DESCRIPTION
  This function sets a flag that denotes whether to start storing RF scan results
  in our local array.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean wcdma_start_capture_scan_data
(
  void   *wcdma_scan_data, /* Pointer to memory that will store WCDMA scan data */
  uint16  wcdma_scan_data_mem_size /* Size allocated for WCDMA scan data */
);

/*===========================================================================

FUNCTION      WCDMA_STOP_CAPTURE_SCAN_DATA

DESCRIPTION
  This function sets a flag to false in order to determine when to stop storing
  RF scan data into our local structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean wcdma_stop_capture_scan_data( void );

/*===========================================================================

FUNCTION      mcalwcdma_wenc_ftm_write_async_mod_parms

DESCRIPTION   This function sets up the modulator parameters for FTM mode 
              These are hard-coded parameters to allow RF Non signalling testing
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  FW - SW Interface will be updated with modulator paramteters
===========================================================================*/
extern void mcalwcdma_wenc_ftm_write_async_mod_parms(void);

/*===========================================================================

FUNCTION      mcalwcdma_wenc_ftm_configure_eul_waveform

DESCRIPTION   This function sets up the modulator parameters for FTM mode 
              for EUL testing
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  FW - SW Interface and ukernel will be updated with 
              encoder and modulator paramteters
===========================================================================*/
extern void mcalwcdma_wenc_ftm_configure_eul_waveform(
  /* DPCCH Gain Factor */
  uint8 bc, 
  /* DPDCH Gain Factor */
  uint8 bd, 
  /* HS-DPCCH Gain Factor index */
  uint8 dhs, 
  /* E-DPCCH Gain Factor index */
  uint8 dec, 
  /* E-DPDCH Gain Factor index */
  uint8 ded, 
  /* E-DPDCH SF2 Gain Factor */
  uint8 bed2, 
  /* Mu
     1 : SF >= SF4
     2 : SF = 2SF4
     4 : SF = 2SF2
     6 : SF = 2SF2 + 2SF4 QPSK
     12: SF = 2SF2 + 2SF4 4PAM
   */
  uint8 mu,
  /* Number of DPDCH (0/1) */
  uint8 num_dpdch, 
  /* HS-DPCCH Enabled or not */
  boolean hsdpcch_en, 
  /* Modulation Scheme : 0 : QPSK, 1 : 4PAM */
  uint8 modulation_scheme,
  /* EUL TTI 0 : 2ms, 1 : 10ms*/
  uint8 e_tti,
  /* Number of EUL carriers,
     1 : SC-HSUPA, 2 : DC-HSUPA
   */
  uint8 num_carriers,
  /* EUL Action :
     0 : stop, 1 : start
   */
  uint8 action
);

/*===========================================================================

FUNCTION      wcdma_get_rat_meas_info

DESCRIPTION
This is the API function that CM that will call to get WCDMA serving and neighbor cell info

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void wcdma_get_rat_meas_info(cmapi_wcdma_signal_info_struct_type* wcdma_signal_info);

/*===========================================================================

FUNCTION      wcdma_is_rat_meas_info_ready

DESCRIPTION
This function returns the status of "wcdma_rat_meas_info_ready" variable

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
boolean wcdma_is_rat_meas_info_ready(void);

#endif /* WCDMA_COMMON_API_H */
