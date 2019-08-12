/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          TM-JGPS Message translation unit

GENERAL DESCRIPTION
  This module contains the services which translate an KDDI message between
  and internal format.
  
EXTERNALIZED FUNCTIONS
 

DESCRIPTION
  This function translates a Position Determination Data Message header from 
  internal to external format.

  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/jgps/src/tm_jgps_msg.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/06   rw      Initial Release
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_JCDMA_GPSONE

#include "msg.h"
#include "qw.h"
#include "bit.h"

#include "tm_jgps_msg.h"

/*===========================================================================

FUNCTION TM_JGPS_XLATE_INT_MS_SCU_HDR

DESCRIPTION
  This function converst internal SCU header to external format.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_xlate_int_ms_scu_hdr
(
  byte *buf_ptr,
    /* Pointer to where translated message should be placed */
  tm_jgps_i_hdr_s_type *hdr_ptr
    /* Pointer to internal message to be transmitted */
)
{
  word buffer_pos=0;
    /* Position to pack bits in message */

  /*--------------------------------------------------------------------
     Pack the Header to external format
  --------------------------------------------------------------------*/

  /* Service ID low byte */  
  b_packb( hdr_ptr->service_id[0], buf_ptr, buffer_pos, FSIZ( tm_jgps_ext_hdr_s_type, service_id_lo ));
  buffer_pos += FSIZ( tm_jgps_ext_hdr_s_type, service_id_lo );
  
  /* Service ID high byte */
  b_packb( hdr_ptr->service_id[1], buf_ptr, buffer_pos, FSIZ( tm_jgps_ext_hdr_s_type, service_id_hi ));
  buffer_pos += FSIZ( tm_jgps_ext_hdr_s_type, service_id_hi );
  
  /* Protocol version */
  b_packb( hdr_ptr->prot_version, buf_ptr, buffer_pos, FSIZ( tm_jgps_ext_hdr_s_type, prot_version ));
  buffer_pos += FSIZ( tm_jgps_ext_hdr_s_type, prot_version );
  
  /* Data length */
  b_packw( hdr_ptr->length, buf_ptr, buffer_pos, FSIZ( tm_jgps_ext_hdr_s_type, length ));
  buffer_pos += FSIZ( tm_jgps_ext_hdr_s_type, length );

  /* Command ID */
  b_packb( hdr_ptr->cmd_id, buf_ptr, buffer_pos, FSIZ( tm_jgps_ext_hdr_s_type, cmd_id ));

  return;
} /* tm_jgps_xlate_int_ms_scu_hdr */


/*===========================================================================

FUNCTION TM_JGPS_XLATE_EXT_MS_SCU_HDR

DESCRIPTION
  This function converst external SCU header to internal format.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_xlate_ext_ms_scu_hdr
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  tm_jgps_i_hdr_s_type *hdr_ptr
    /* Pointer to place where translated message should be placed */
)
{
    word msg_pos = 0;
      /* Position to unpack bits in message */
    
    /*--------------------------------------------------------------------
        Unpack the Header 
    --------------------------------------------------------------------*/
    
    /* Service ID low byte */
    hdr_ptr->service_id[0] = b_unpackb( buf_ptr, msg_pos, FSIZ( tm_jgps_ext_hdr_s_type, service_id_lo ));
    msg_pos += FSIZ( tm_jgps_ext_hdr_s_type, service_id_lo );
    
    /* Service ID high byte */
    hdr_ptr->service_id[1] = b_unpackb( buf_ptr, msg_pos, FSIZ( tm_jgps_ext_hdr_s_type, service_id_hi ));
    msg_pos += FSIZ( tm_jgps_ext_hdr_s_type, service_id_hi );

    /* Protocol version */
    hdr_ptr->prot_version = b_unpackb( buf_ptr, msg_pos, FSIZ( tm_jgps_ext_hdr_s_type, prot_version ));
    msg_pos += FSIZ( tm_jgps_ext_hdr_s_type, prot_version );

    /* Data length */
    hdr_ptr->length = b_unpackw( buf_ptr, msg_pos, FSIZ( tm_jgps_ext_hdr_s_type, length ));
    msg_pos += FSIZ( tm_jgps_ext_hdr_s_type, length );

    /* Command ID */
    hdr_ptr->cmd_id = b_unpackb( buf_ptr, msg_pos, FSIZ( tm_jgps_ext_hdr_s_type, cmd_id ));

    return;
} /* tm_jgps_xlate_ext_ms_scu_hdr */


/*===========================================================================

FUNCTION TM_JGPS_XLATE_INT_MS_SCU_REQ

DESCRIPTION
  This function converst internal SCU RF information to external format.
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_xlate_int_ms_scu_req
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  tm_jgps_i_ms_req_s_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
    int i;
      /* Counter for loop */
    word buffer_pos=0;
      /* Position to pack bits in message */

    /*--------------------------------------------------------------------
       Pack the Header to external format
    --------------------------------------------------------------------*/

    /* MS Phone Number */
    for ( i = 0; i < TM_JGPS_MAX_MSNUM; i++ )
    {
      b_packb( int_ptr->msnum[i*2+1], buf_ptr, buffer_pos, FSIZ( tm_jgps_msnum_s_type, msnum ));
      buffer_pos += FSIZ( tm_jgps_msnum_s_type, msnum );
      
      b_packb( int_ptr->msnum[i*2],   buf_ptr, buffer_pos, FSIZ( tm_jgps_msnum_s_type, msnum ));
      buffer_pos += FSIZ( tm_jgps_msnum_s_type, msnum );
    }
    
    /* MS IP address */
    b_packd( int_ptr->ipaddr, buf_ptr, buffer_pos, FSIZ( tm_jgps_ext_req_s_type, ipaddr ) );
    buffer_pos += FSIZ(tm_jgps_ext_req_s_type, ipaddr);

    /* MS RF information */
    b_packd( int_ptr->esn, buf_ptr, buffer_pos, FSIZ(tm_jgps_rf_fix_s_type, esn) );
    buffer_pos += FSIZ(tm_jgps_rf_fix_s_type, esn);
  
    b_packb( int_ptr->tx, buf_ptr, buffer_pos, FSIZ(tm_jgps_rf_fix_s_type, tx) );
    buffer_pos += FSIZ(tm_jgps_rf_fix_s_type, tx);
  
    b_packb( int_ptr->fer, buf_ptr, buffer_pos, FSIZ(tm_jgps_rf_fix_s_type, fer) );
    buffer_pos += FSIZ(tm_jgps_rf_fix_s_type, fer);
  
    b_packb( int_ptr->rssi, buf_ptr, buffer_pos, FSIZ(tm_jgps_rf_fix_s_type, rssi) );
    buffer_pos += FSIZ(tm_jgps_rf_fix_s_type, rssi);
  
    for ( i = 0; i < TM_JGPS_MAX_ASET; i++ )
    {
      b_packw( int_ptr->pn[i].pn, buf_ptr, buffer_pos, FSIZ(tm_jgps_rf_var_s_type, pn) );
      buffer_pos += FSIZ(tm_jgps_rf_var_s_type, pn);
  
      b_packb( int_ptr->pn[i].ecio, buf_ptr, buffer_pos, FSIZ(tm_jgps_rf_var_s_type, ecio) );
      buffer_pos += FSIZ(tm_jgps_rf_var_s_type, ecio);
    }

    b_packw( int_ptr->ch, buf_ptr, buffer_pos, FSIZ(tm_jgps_rf_info_s_type, ch) );
    buffer_pos += FSIZ(tm_jgps_rf_info_s_type, ch);
    
    /* URL length */
    b_packb( int_ptr->url_length, buf_ptr, buffer_pos, FSIZ(tm_jgps_ext_req_s_type, url_length) );
    buffer_pos += FSIZ(tm_jgps_ext_req_s_type, url_length);

    /* URL */
    for ( i = 0; i < int_ptr->url_length; i++ )
    {
      b_packb( int_ptr->url[i], buf_ptr, buffer_pos, FSIZ(tm_jgps_url_s_type, url) );
      buffer_pos += FSIZ(tm_jgps_url_s_type, url);
    }

    return;
} /* tm_jgps_xlate_int_ms_scu_req */


/*===========================================================================

FUNCTION TM_JPGS_XLATE_EXT_RESP_POS_RESP

DESCRIPTION
  This function converst the external Position response from external to
  internal format.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_xlate_ext_pos_resp
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  tm_jgps_i_pos_resp_s_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
    /* Status returned to calling procedure */
    word msg_pos = 0;

    /*--------------------------------------------------------------------
       Unpack the position response
    --------------------------------------------------------------------*/
    
    /* for pos response, simply translate the result and cause code.
    */
    int_ptr->result = b_unpackb( buf_ptr, msg_pos, FSIZ( tm_jgps_ext_pos_resp_s_type, result ));
    msg_pos += FSIZ( tm_jgps_ext_pos_resp_s_type, result );

    int_ptr->cause = b_unpackb( buf_ptr, msg_pos, FSIZ( tm_jgps_ext_pos_resp_s_type, cause ));

    return;
} /* tm_jgps_xlate_ext_pos_resp */


/*===========================================================================

FUNCTION TM_JGPS_XLATE_EXT_RESP_POS_COMP

DESCRIPTION
  This function translates external Position completion response from 
  external to internal format.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_xlate_ext_pos_comp
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  tm_jgps_i_pos_comp_s_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
    word msg_pos = 0;

    /*--------------------------------------------------------------------
       Unpack the position complete
    --------------------------------------------------------------------*/
    /* for pos response, simply translate the result and cause code.
    */
    int_ptr->result = b_unpackb( buf_ptr, msg_pos, FSIZ( tm_jgps_ext_pos_comp_s_type, result ));
    msg_pos += FSIZ( tm_jgps_ext_pos_comp_s_type, result );

    int_ptr->cause = b_unpackb( buf_ptr, msg_pos, FSIZ( tm_jgps_ext_pos_comp_s_type, cause ));
    msg_pos += FSIZ( tm_jgps_ext_pos_comp_s_type, cause );

    int_ptr->flength = b_unpackb( buf_ptr, msg_pos, FSIZ( tm_jgps_ext_pos_comp_s_type, flength ));
    
    return;
} /* tm_jgps_xlate_ext_pos_comp */


/*===========================================================================

FUNCTION TM_JGPS_XLATE_INT_CANCEL_REQ

DESCRIPTION
  This function converst external SCU Cancel Request message to internal format 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_xlate_int_cancel_req
( 
  byte *buf_ptr, 
    /* Pointer to where translated message should be placed */
  tm_jgps_i_cancel_req_s_type *int_ptr
    /* Pointer to internal message to be transmitted */ 
)
{
  word msg_pos = 0;

  /*--------------------------------------------------------------------
     Pack the cancellation request data
  --------------------------------------------------------------------*/
  b_packb( int_ptr->cause, buf_ptr, msg_pos, FSIZ(tm_jgps_ext_cancel_req_s_type, cause) );

  return;
} /* tm_jgps_xlate_int_cancel_req */


/*===========================================================================

FUNCTION TM_JGPS_XLATE_EXT_STOP_REQ

DESCRIPTION
  This function converst external SCU stop Request message to internal format 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_xlate_ext_stop_req
( 
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  tm_jgps_i_stop_req_s_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  /* Status returned to calling procedure */
  word msg_pos = 0;

  /*--------------------------------------------------------------------
     Pack the suspension request data
  --------------------------------------------------------------------*/

  int_ptr->cause = b_unpackb( buf_ptr, msg_pos, FSIZ(tm_jgps_ext_stop_req_s_type, cause) );

  return;
} /* tm_jgps_xlate_ext_stop_req */

#endif /* FEATURE_JCDMA_GPSONE */
