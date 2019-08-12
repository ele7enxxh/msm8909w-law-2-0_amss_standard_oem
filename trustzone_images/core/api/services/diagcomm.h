#ifndef DIAGCOMM_H 
#define DIAGCOMM_H
/*==========================================================================

              Diagnostic Communications Layer Header

General Description
  API declarations for the diagnostic protocol transport layer.
  
Copyright (c) 2000-2009 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           Edit History
                           
 $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/diagcomm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/09   JV      Moved prototypes for diag_change_default_sio_bitrate and
                   diag_record_dm_baudrate here
07/31/09    JV     Merged Q6 diag code back to mainline
07/17/09    mad    Featured out for API header commonality
07/06/09    as     Removed depedency from FEATURE_DATA_SERIALIZER
06/11/09    JV     Featurized inclusion of customer.h to ease merging of the 
                   Q6 branch
05/07/09    vk      removed dependencies from diagbuf_v.h
12/12/08    vg     CMI splip into diagcomm_v.h and diagcomm.h
12/16/07    pc     Added support for diag_dtr_enable and diag_dtr_changed_cb.
12/15/06    as     Fixed compiler warnings.
05/15/05    as     Added support for FEATURE_DATA_SERIALIZER
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag
12/14/04    sl     Added support for DIAG to hold multiple SIO ports
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
10/24/01   jal     Added ability to flush transmit channel
04/06/01   lad     Introduces types for callback function pointers.
                   Moved ASYNC char definitions from diagi.h.
02/23/01   lad     Created file.

===========================================================================*/
#include "comdef.h"
#include "sio.h"
#include "rdevmap.h"

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
    extern "C" {
#endif

/*===========================================================================
  
FUNCTION DIAGCOMM_SIO_QCDMG
FUNCTION DIAGCOMM_SIO_QCDMG_PORT


DESCRIPTION
  Execute AT$QCDMG command. Diag will close its open port and grab DS's
  port.

  It returns TRUE if RDM accepts the port mapping; FALSE if RDM can't 
  change the port mapping.

===========================================================================*/
boolean diagcomm_sio_qcdmg (void); 
boolean diagcomm_sio_qcdmg_port(rdm_device_enum_type port_id); 

/*===========================================================================

FUNCTION DIAG_RECORD_DM_BAUDRATE

DESCRIPTION
  This procedure is called when a bad SPC is processed.  The Diag task will
  time out for several seconds.
 
===========================================================================*/
  void diag_record_dm_baudrate (sio_bitrate_type bitrate);


/*===========================================================================

FUNCTION DIAG_CHANGE_DEFAULT_SIO_BITRATE

DESCRIPTION
  This function changed the default DIAG bitrate as well as change the 
  current operating bitrate.

===========================================================================*/
  void diag_change_default_sio_bitrate (sio_bitrate_type new_baud);


#ifdef __cplusplus
    }
#endif
#endif /* DIAGCOMM_H  */
