#ifndef DIAGCOMM_IO_SIO_H 
#define DIAGCOMM_IO_SIO_H
/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description
  
Copyright (c) 2011-2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_io_sio.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/03/13  sg      Command Response Channel separation
03/04/12  sg      Update functions to support control channel
06/15/12  sa      Removed function prototype diagcomm_ctrl_open_cb
03/04/12  sg      Update functions to support control channel
03/04/11  is      Support for Diag over SMD-Lite

===========================================================================*/
#include "core_variation.h"
#ifndef FEATURE_WINCE
#endif

#ifdef DIAG_SIO_SUPPORT

#include "comdef.h"
#include "diagcomm_io.h"   /* For diagcomm_io_conn_type */
#include "sio.h"           /* For sio_bitrate_type */


/* For smd channel */
void diagcomm_smd_sio_set_port( diagcomm_io_conn_type * conn );

/* For sio channel */
void diag_dtr_enable( diagcomm_io_conn_type * conn );
void diag_dtr_changed_cb( void * cb_data );
void diagcomm_sio_open_cb( sio_port_id_type port_id );
void diagcomm_sio_set_port( diagcomm_io_conn_type * conn );

/* For ctrl channel */
void diagcomm_ctrl_sio_set_port( diagcomm_io_conn_type * conn );
void diagcomm_ctrl_open_cb( sio_port_id_type port_id );

#endif /* DIAG_SIO_SUPPORT */

#endif /* DIAGCOMM_IO_SIO_H */
