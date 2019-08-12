#ifndef PDCPRLCIF_H
#define PDCPRLCIF_H
/*===========================================================================

              
                           H E A D E R   F I L E


DESCRIPTION
This file has APIs relating to interactions between the PDCP layer and RLC.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 - 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/pdcp/inc/pdcprlcif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/09   ss      SU level CMI modifications.
03/04/09   sa      AU level CMI modifications.
03/09/07   rr      Changed back return type of pdcp_get_traffic_class to 
                   ds_umts_qos_tclass_enum_type.
02/01/07   vk      Watermark level changes for HSUPA.
02/01/07   rsl     Changes relating to CPU based flow control.
08/31/06   rsl     Featurized PDCP
08/18/06   rsl     Added support for traffic class under 
                   FEATURE_CPU_BASED_FLOW_CONTROL.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dsm.h"

#ifdef FEATURE_DATA_WCDMA_PS

#include "comptask_v.h"

#include "dsumtsps.h"
#include "ds_3gpp_bearer_context.h"
#include "dsumtspdpreg.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  WCDMA PDCP<->RLC Watermark Levels
---------------------------------------------------------------------------*/

#ifdef FEATURE_WCDMA_HSUPA
  #define PDCP_RLC_UL_WM_LO     DSWPSD_UL_WM_DEFAULT_EUL_LO
  #define PDCP_RLC_UL_WM_HI     DSWPSD_UL_WM_DEFAULT_EUL_HI
  #define PDCP_RLC_UL_WM_DNE    DSWPSD_UL_WM_DEFAULT_EUL_DNE
  #define PDCP_RLC_UL_WM_DNE_QCNT    DSWPSD_UL_WM_DEFAULT_EUL_DNE_QCNT
#else
  #define PDCP_RLC_UL_WM_LO     DSUMTSPS_UL_WM_DEFAULT_LO
  #define PDCP_RLC_UL_WM_HI     DSUMTSPS_UL_WM_DEFAULT_HI
  #define PDCP_RLC_UL_WM_DNE    DSUMTSPS_UL_WM_DEFAULT_DNE
  #define PDCP_RLC_UL_WM_DNE_QCNT    DSUMTSPS_UL_WM_DEFAULT_DNE_QCNT
#endif

#if defined(FEATURE_6280_HSDPA_PHASE2_CAPABILITY)
  #define PDCP_RLC_DL_WM_LO     DSWPSD_DL_WM_DEFAULT_HS_P2_LO
  #define PDCP_RLC_DL_WM_HI     DSWPSD_DL_WM_DEFAULT_HS_P2_HI
  #define PDCP_RLC_DL_WM_DNE    DSWPSD_DL_WM_DEFAULT_HS_P2_DNE
  #define PDCP_RLC_DL_WM_DNE_QCNT    DSWPSD_DL_WM_DEFAULT_HS_P2_DNE_QCNT
#elif defined(FEATURE_6280_HSDPA_PHASE1_CAPABILITY)
  #define PDCP_RLC_DL_WM_LO     DSWPSD_DL_WM_DEFAULT_HS_P1_LO
  #define PDCP_RLC_DL_WM_HI     DSWPSD_DL_WM_DEFAULT_HS_P1_HI
  #define PDCP_RLC_DL_WM_DNE    DSWPSD_DL_WM_DEFAULT_HS_P1_DNE
  #define PDCP_RLC_DL_WM_DNE_QCNT    DSWPSD_DL_WM_DEFAULT_HS_P1_DNE_QCNT
#elif defined(FEATURE_HSDPA)
  #define PDCP_RLC_DL_WM_LO     DSWPSD_DL_WM_DEFAULT_HS_LO
  #define PDCP_RLC_DL_WM_HI     DSWPSD_DL_WM_DEFAULT_HS_HI
  #define PDCP_RLC_DL_WM_DNE    DSWPSD_DL_WM_DEFAULT_HS_DNE
  #define PDCP_RLC_DL_WM_DNE_QCNT    DSWPSD_DL_WM_DEFAULT_HS_DNE_QCNT
#else
  #define PDCP_RLC_DL_WM_LO     DSUMTSPS_DL_WM_DEFAULT_LO
  #define PDCP_RLC_DL_WM_HI     DSUMTSPS_DL_WM_DEFAULT_HI
  #define PDCP_RLC_DL_WM_DNE    DSUMTSPS_DL_WM_DEFAULT_DNE
  #define PDCP_RLC_DL_WM_DNE_QCNT    DSUMTSPS_DL_WM_DEFAULT_DNE_QCNT
#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION pdcp_RESET_SENT_CLOSE_TO_RLC()

DESCRIPTION
  This function is called to reset the flag which tracks if we have sent 
  a RLC window close request to trigger RLC peer-to-peer flow control.

PARAMETERS   
  pdcp_inst - Index into call table for call

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void pdcp_reset_sent_close_to_rlc
(
  uint8 pdcp_inst
);




/*===========================================================================
FUNCTION pdcp_PDCP_TO_RLC_LO_WATER_CB

DESCRIPTION
  This function is called when the PDCP to RLC watermark reaches the low
  watermark. 
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
void pdcp_pdcp_to_rlc_lo_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
);


/*===========================================================================
FUNCTION pdcp_PDCP_TO_RLC_HI_WATER_CB

DESCRIPTION
  This function is called when the PS to RLC watermark reaches the high
  watermark. 
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
void pdcp_pdcp_to_rlc_hi_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
);


/*===========================================================================
FUNCTION pdcp_RLC_TO_PDCP_HI_WATER_CB

DESCRIPTION
  This function is called when the RLC to PS watermark reaches the high
  watermark. We send the RLC_CLOSE_RX_WIN primitive to RLC so it will in turn
  close the RLC RX window of the UE.
  
PARAMETERS 
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Causes RLC to close the UE's RX window to one PDU (for this RLC ID)  

===========================================================================*/
void pdcp_rlc_to_pdcp_hi_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
);


/*===========================================================================
FUNCTION pdcp_RLC_TO_PDCP_lo_water_CB

DESCRIPTION
  This function is called when the RLC to PS watermark reaches the low
  watermark. We send the RLC_OPEN_RX_WIN primitive to RLC so it will in turn
  open the RLC RX window of the UE.
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Causes RLC to request the UE's RX window be opened back up for this RLC ID
  
===========================================================================*/
void pdcp_rlc_to_pdcp_lo_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
);


/*===========================================================================
FUNCTION      pdcp_CLEANUP_RLC_WATERMARKS

DESCRIPTION   
  This function cleans up the watermarks that are owned by data services.

PARAMETERS   
  pdcp_instance - index into the pdcp control block.
  
DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
===========================================================================*/
void pdcp_cleanup_rlc_watermarks
(
  uint32 pdcp_instance
);

/*===========================================================================
FUNCTION pdcp_SETUP_PDCP_TO_RLC_WM()

DESCRIPTION
  This function initializes the PDCP_RLC_WM.

PARAMETERS   
  pdcp_inst - Index into the pdcp control block.

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void pdcp_setup_pdcp_to_rlc_wm
(
  uint32 pdcp_inst
);


/*===========================================================================
FUNCTION pdcp_SETUP_RLC_TO_PDCP_WM()

DESCRIPTION
  This function initializes the RLC_PDCP_WM.

PARAMETERS   
  pdcp_inst - Index into pdcp control block.

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void pdcp_setup_rlc_to_pdcp_wm
(
  uint32 pdcp_inst
);


/*===========================================================================
FUNCTION pdcp_REGISTER_WM_WITH_RLC

DESCRIPTION
  This function registers the watermarks with RLC for uplink & downlink data
  
PARAMETERS 
  rlc_dl_id - RLC DL ID.
  rlc_ul_id - RLC UL ID.
  tx_wm_ptr - TX_WM between PDCP and RLC.
  rx_wm_ptr - RX_WM between PDCP and RLC.
  post_rx_Func_ptr - function to be invoked when data is received on the RX
  WM between PDCP and RLC.
  rx_func_para - parameter to be passed into the post_rx_func_ptr.
                   
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None.

===========================================================================*/
void pdcp_register_wm_with_rlc
(
  sys_modem_as_id_e_type       subs_id,  
  rlc_lc_id_type               rlc_dl_id,
  rlc_lc_id_type               rlc_ul_id,
  dsm_watermark_type          *tx_wm_ptr,
  dsm_watermark_type          *rx_wm_ptr,
  void                        (*post_rx_func_ptr)(byte, byte, void *),
  void                        *rx_func_para
);


/*===========================================================================
FUNCTION pdcp_DEREGISTER_WM_WITH_RLC

DESCRIPTION
  This function deregisters the watermarks with RLC for uplink & downlink data

PARAMETERS
  pdcp_instance - Index into pdcp control block.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void pdcp_deregister_wm_with_rlc
(
  uint8 pdcp_instance
);

/*===========================================================================
FUNCTION PDCP_FROM_RLC_POST_RX_FUNC_PTR

DESCRIPTION
  When data is enqueued onto RLC_TO_PDCP_WM, RLC invokes this callback.
  
PARAMETERS   
  pdcp_inst - only parameter used in this function which is the index
  into the pdcp control block.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
     
SIDE EFFECTS 
  None.
  
===========================================================================*/
extern void pdcp_from_rlc_post_rx_func_ptr
(
  byte x,
  byte y,
  void *pdcp_inst
);

/*===========================================================================
FUNCTION      PDCP_CLEANUP_TDRLC_WATERMARKS

DESCRIPTION   
  This function cleans up the watermarks that are owned by data services.

PARAMETERS   
  call_instance - Index into call table for call.
  
DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
===========================================================================*/
void pdcp_cleanup_tdrlc_watermarks
(
  uint32 pdcp_instance
);

/*===========================================================================
FUNCTION pdcp_TDRLC_TO_PDCP_LO_WATER_CB

DESCRIPTION
  This function is called when the TDRLC to PDCP watermark reaches the low
  watermark. We send the RLC_OPEN_RX_WIN primitive to RLC so it will in turn
  open the RLC RX window of the UE.
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Causes RLC to request the UE's RX window be opened back up for this RLC ID
  
===========================================================================*/
void pdcp_tdrlc_to_pdcp_lo_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
);

/*===========================================================================
FUNCTION pdcp_DEREGISTER_WM_WITH_TDRLC

DESCRIPTION
  This function deregisters the watermarks with TDRLC for UL & DL data

PARAMETERS
  pdcp_instance - Index into pdcp_ctl_blk.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void pdcp_deregister_wm_with_tdrlc
(
  uint8 pdcp_instance
);

/*===========================================================================
FUNCTION pdcp_REGISTER_WM_WITH_TDRLC

DESCRIPTION
  This function registers the watermarks with RLC for uplink & downlink data
  
PARAMETERS 
  rlc_dl_id: RLC downlink id to register WM with.
  rlc_ul_id: RLC uplink id to register WM with.
  tx_wm_ptr: tx_wm that will be regd with RLC.
  rx_wm_ptr: rx_wm that will be regd with RLC.
  post_rx_func_ptr: rx func that will be called when there is data on RX path.
  rx_func_para: value passed in while calling post_rx_func.
                     
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None.

===========================================================================*/
void pdcp_register_wm_with_tdrlc
(
  sys_modem_as_id_e_type      subs_id,
  uint8                       rlc_dl_id,
  uint8                       rlc_ul_id,
  dsm_watermark_type          *tx_wm_ptr,
  dsm_watermark_type          *rx_wm_ptr,
  void                        (*post_rx_func_ptr)(byte, byte, void *),
  void                        *rx_func_para
);

#endif /* FEATURE_DATA_WCDMA_PS */



#endif /*PDCPRLCIF_H*/
