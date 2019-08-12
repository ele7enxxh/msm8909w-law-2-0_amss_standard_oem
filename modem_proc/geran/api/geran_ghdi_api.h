#ifndef GERAN_GHDI_API_H
#define GERAN_GHDI_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    GERAN GHDI API FUNCTIONS

GENERAL DESCRIPTION

  This module provides Functions's for GHDI NAS module to support GSM VOICE

Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/geran_ghdi_api.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
02-08-13   pjr       CR519061: added function gl1_vs_ghdi_control_subs to include as_id
01-12-11   ws        Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "mvs.h"
#include "sys.h"

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/
#define GL1_VS_EOK          ( 0x00000000 ) /* Success / completed / no errors. */
#define GL1_VS_EFAILED      ( 0x00000001 ) /* General failure. */

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/
typedef enum
{
  GL1_GHDI_VS_ENABLE,
  GL1_GHDI_VS_DISABLE
} gl1_vs_enable_t;

/*******************************************************************************
 *
 *  Function name: gl1_msg_tch_dl_processing ()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Copies GL1 packets received on downlink into MVS structures for decoding by vocoder
 *
 *  Parameters:
 *  -----------
 *     vocoder packets are copied from gl1_rx_pkt.packet to dl_pkt
 *     gl1_rx_pkt.frame_info is copied at frame_info 
 *     status is set to of the values of mvs_pkt_status_type based on the values of variable 
 *     gl1_tx_pkt.packet_semaphore for DL

 *  Returns:
 *  --------
 *   None
 ******************************************************************************/
void gl1_msg_tch_dl_processing(uint8 *dl_pkt,
                               mvs_frame_info_type  *frame_info,
                               mvs_pkt_status_type  *status);


 /*******************************************************************************
 *
 *  Function name: gl1_msg_tch_ul_processing ()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Copies Vocoder packets from MVS into GL1 structures for transmitting on the uplink
 *
 *
 *  Parameters:
 *  -----------
 *    pkt_len number of bytes(octets) from ul_pkt should be copied to gl1_tx_pkt.packet 
 *    tx_frame_info  should be copied into  gl1_tx_pkt.frame_info 
 *    status  should be set to of the values of mvs_pkt_status_type based on the values
 *    of variable gl1_tx_pkt.packet_semaphore for UL
 *  Returns:
 *  --------
 *    None
 ******************************************************************************/
void gl1_msg_tch_ul_processing(uint8 *ul_pkt,
                                mvs_frame_info_type *tx_frame_info,
                                uint16 pkt_len,
                                mvs_pkt_status_type  *status);


/*******************************************************************************
 *
 *  Function name: gl1_ghdi_vs_control ()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Enable/disable the single mode voice system, the vs should have 
 *    been acquired before enabling
 *
 *
 *  Parameters:
 *  -----------
 *    enable GL1_GHDI_VS_DISABLE/GL1_GHDI_VS_ENABLE to enable or disable the 
 *    single mode voice system. mvs_mode determines the vocoder to use.
 *    
 *    
 *  Returns:
 *  --------
 *    GL1_VS_EOK, indicates the call to VS enable succeded. Use function
 *    gl1_vs_is_ready to check if the vs is ready to exchange vocoder packets
 *    VS_EFAILED, failed to send enable command to the VS
 ******************************************************************************/
uint32 gl1_vs_ghdi_control(gl1_vs_enable_t enable, mvs_mode_type mvs_mode);

/*******************************************************************************
 *
 *  Function name: gl1_ghdi_vs_control_subs ()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Enable/disable the single mode voice system, the vs should have
 *    been acquired before enabling
 *
 *
 *  Parameters:
 *  -----------
 *    enable GL1_GHDI_VS_DISABLE/GL1_GHDI_VS_ENABLE to enable or disable the
 *    single mode voice system. mvs_mode determines the vocoder to use.
 *    NAS ID to avoid issue on TSTS config CR519061
 *    
 *  Returns:
 *  --------
 *    GL1_VS_EOK, indicates the call to VS enable succeded. Use function
 *    gl1_vs_is_ready to check if the vs is ready to exchange vocoder packets
 *    VS_EFAILED, failed to send enable command to the VS
 ******************************************************************************/
uint32 gl1_vs_ghdi_control_subs(gl1_vs_enable_t enable, mvs_mode_type mvs_mode, 
                                sys_modem_as_id_e_type asubs_id);

#endif /* GERAN_GHDI_API_H */
