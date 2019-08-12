#ifndef DIAGCOMM_SMD_H 
#define DIAGCOMM_SMD_H
/*==========================================================================

              Diagnostic Communications Layer Header

General Description
  API declarations for the diagnostic protocol transport layer.
  
Copyright (c) 2000-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                           Edit History
                           
 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_smd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
03/04/11   is      Support for Diag over SMD-Lite
09/14/10   is      New control channel and central routing features 
08/10/10   sg      Close SMD Port when the SIO port on apps is closed
05/13/10   JV      Merge functionality in diagcomm_smd.c and diagcomm_smd2.c
                   in a single file
10/01/09   JV      Removed inclusion of diagcomm.h
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/17/09   mad     Featurized includion of customer.h and target.h for WM.
05/07/09   vk      added internal header files includes for CMI cleanup
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
10/24/01   jal     Added ability to flush transmit channel
04/06/01   lad     Introduces types for callback function pointers.
                   Moved ASYNC char definitions from diagi.h.
02/23/01   lad     Created file.

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#ifndef FEATURE_WINCE /* Don't see any features in this file, prob. these are not needed for AMSS either */
#endif

#include "diagcomm_v.h"
#include "diagcomm_io.h"


/* ------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
	extern "C" {
#endif

/* Generic - SMD specific */
void diagcomm_smd_init( diagcomm_enum_port_type port_num );
boolean diagcomm_smd_open( diagcomm_enum_port_type port_num );
void diagcomm_smd_close( diagcomm_enum_port_type port_num );
boolean diagcomm_smd_status( diagcomm_enum_port_type port_num );
void diagcomm_smd_close_done_cb( void );
void diagcomm_smd_send( void *req_pkt, uint16 pkt_len, diagcomm_enum_port_type port_num );
void diagcomm_smd_conn_changed( diagcomm_io_conn_type * conn, boolean connected );

/* SIO/SMD specific */
#ifdef DIAG_SIO_SUPPORT
void diagcomm_smd_open_init( diagcomm_io_conn_type * conn );
#endif /* DIAG_SIO_SUPPORT */
extern uint32 diagcomm_smd_get_rx_wmq_cnt( diagcomm_enum_port_type port_num );
extern void diagcomm_smd_process_slave_tx( diagcomm_enum_port_type port_num );

#ifdef __cplusplus
	}
#endif

#endif /* DIAGCOMM_SMD_H  */
