/*****************************************************************************
***
*** TITLE
***
***  SNDCP UTILITY HEADER FILE
***
***
*** DESCRIPTION
***
***  Header file for the SNDCP utility module.
***
*** 
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsn_util.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz	   changes
***
*****************************************************************************/

#ifndef INC_GSN_UTIL_H
#define INC_GSN_UTIL_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "gsni.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/




/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      GSN_DECODE_DATA_PDU_HDR()
===
===  DESCRIPTION
===
===   Decodes SN-DATA PDU header from external format into 
===   internal format ( gsn_data_pdu_hdr_t ).
===
===   *src:       array of bytes representing Down-link LLC data block 
===   *sn_data:   structure representing SNDCP SN-DATA PDU format
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===    TRUE if valid SN-DATA PDU header format, otherwise FALSE 
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/

boolean gsn_decode_data_pdu_hdr( gas_id_t gas_id, uint8 *src, gsn_data_pdu_hdr_t *sn_data);


/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_DATA_PDU_HDR()
===
===  DESCRIPTION
===
===   Encodes SN-DATA PDU header from internal format to external format
===
===   *sn_data:   SN-DATA PDU header struct in internal format
===   *dst:       SN-DATA PDU header external format as an array of bytes
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/

void gsn_encode_data_pdu_hdr( const gsn_data_pdu_hdr_t *sn_data, uint8 *dst);


/*===========================================================================
===
===  FUNCTION      GSN_DECODE_UNITDATA_PDU_HDR()
===
===  DESCRIPTION
===
===   Decodes SN-UNITDATA PDU header from external format into 
===   internal format ( gsn_unitdata_pdu_hdr_t ).
===
===   *src:       array of bytes representing Down-link LLC data block 
===   *sn_data:   structure representing SNDCP SN-UNITDATA PDU format
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===   TRUE if valid SN-UNITDATA PDU header format, otherwise FALSE 
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/

boolean gsn_decode_unitdata_pdu_hdr( gas_id_t gas_id, uint8 *src, gsn_unitdata_pdu_hdr_t *sn_udata);


/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_UNITDATA_PDU_HDR()
===
===  DESCRIPTION
===
===   Encodes SN-UNITDATA PDU header from internal format to external format
===
===   *sn_unitdata:   SN-UNITDATA PDU header struct in internal format
===   *dst:           SN-UNITDATA PDU header external format as an array of bytes
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/

void gsn_encode_unitdata_pdu_hdr( gas_id_t gas_id, const gsn_unitdata_pdu_hdr_t *sn_unitdata, uint8 *dst);

/*===========================================================================
===
===  FUNCTION      GSN_DECODE_NEG_XID()
===
===  DESCRIPTION
===
===   Decodes requested (by SGSN) XID block from external format into internal format
===   ( gsn_xid_block_t ) and checks if negotiated XID differ from the requested XID.   
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===   TRUE if negotiated XID do not differ from the requested XID
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/

boolean gsn_decode_neg_xid( gas_id_t gas_id, dsm_item_type **l3_xid, gsn_sapi_e_t *sapi_ptr );


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_REQ_XID()
===
===  DESCRIPTION
===
===   Processes XID block requested (by SGSN) and forms the response 
===   (negotiated) XID block. 
===
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===    Pointer to the XID block that will form the response to the 
==     XID block requested
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
dsm_item_type* gsn_process_req_xid
( 
  gas_id_t,
  gsn_sapi_e_t*,
  dsm_item_type**, 
  boolean, 
  boolean
);


/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_XID()
===
===  DESCRIPTION
===
===   Encodes XID block from internal format ( gsn_xid_block_t ) into external format 
===   ( array of bytes as a pdu payload )
===
===   *xid:     XID block in internal format
===   *xid_dst: Points to an array of bytes of an XID pdu block
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===    Length of buffer that holds the XID block in byte format
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/

uint8 gsn_encode_xid( const gsn_xid_block_t *xid_src, uint8 *xid_dst  );


/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_REQ_XID()
===
===  DESCRIPTION
===
===   Builds XID block and calls gsn_encode_xid() to create the XID request
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===    Pointer to the XID block 
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/

dsm_item_type* gsn_encode_req_xid( gas_id_t gas_id, const gsn_nsapi_e_t *nsapi_ptr );

/*===========================================================================
===
===  FUNCTION      GSN_PRINT_XID_PARAMS()
===
===  DESCRIPTION
===
===   Dumps the XID PDU in hex format in a F3 message up to 24 octets
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   Pointer to the xid bock
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/
void gsn_print_xid_params ( gas_id_t gas_id, dsm_item_type  *xid_pdu, boolean ul );


#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_RESET_DCOMP()
===
===  DESCRIPTION
===
===   
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/
void gsn_reset_dcomp( gsn_sapi_e_t *sapi_ptr );

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
/*===========================================================================
===
===  FUNCTION      GSN_RESET_PCOMP()
===
===  DESCRIPTION
===
===   
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/
void gsn_reset_pcomp( gsn_sapi_e_t * );

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

/*===========================================================================
===
===  FUNCTION      GSN_DEL_ALL_SELECTED_COMP_E()
===
===  DESCRIPTION
===
===   Deletes all SELECTED compression entities on the SAPI.
===   This function is called after the XID negotiation has failed and
===   the SELECTED entities have not been ASSIGNED. 
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
void gsn_del_all_selected_comp_e( gas_id_t gas_id, gsn_sapi_e_t *);

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */
   


#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_DEL_DCOMP_E()
===
===  DESCRIPTION
===
===   Deletes the Data compession entity
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
void gsn_del_dcomp_e( gas_id_t gas_id, uint8, gsn_sapi_e_t *);

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */


#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
/*===========================================================================
===
===  FUNCTION      GSN_DEL_PCOMP_E()
===
===  DESCRIPTION
===
===   Deletes the Data compession entity
===
===  DEPENDENCIES
===
===   None
===  
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
void gsn_del_pcomp_e( gas_id_t, uint8, gsn_sapi_e_t *);

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP   */

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) ) 
/*===========================================================================
===
===  FUNCTION      GSN_RESET_COMP_ENT()
===
===  DESCRIPTION
===
===   Resets all assigned compression entities on the SAPI.
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/
void gsn_reset_comp_ent( gas_id_t, gsn_sapi_e_t* );

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */ 

/*===========================================================================
===
===  FUNCTION      GSN_BLD_REJ_T1_XID()
===
===  DESCRIPTION
===
===   Builds up a T1 (Data compression) XID parameter field with Applicable NSAPIs
===   set to zero, to be used as a response to an invalid/unsupported XID request
===
===  DEPENDENCIES
===
===   The calling function has to fill in the "Entity Number" and the 
===   "Type length" field in xid_i.
===  
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
void  gsn_bld_rej_t1_xid
( 
  gas_id_t, 
  gsn_xid_block_t *, 
  uint8 *,                           
  dsm_item_type **
);

/*===========================================================================
===
===  FUNCTION      GSN_BLD_REJ_T2_XID()
===
===  DESCRIPTION
===
===   Builds up a T2  XID parameter field with Applicable NSAPIs
===   set to zero, to be used as a response to an invalid/unsupported XID request
===
===  DEPENDENCIES
===
===   The calling function has to fill in the "Entity Number" and the 
===   "Type length" field in xid_i.
===  
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
void  gsn_bld_rej_t2_xid
( 
  gas_id_t, 
  gsn_xid_block_t *, 
  uint8 *,                           
  dsm_item_type **
);

/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_RMV_NSAPI_XID()
===
===  DESCRIPTION
===
===   If a compression entity is used by the NSAPI, encode an XID block  
===   to indicate to the SGSN SNDCP that the NSAPI shall be removed from 
===   the "Applicable NSAPIs" of the compression entity.
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   Pointer to the xid block
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/
dsm_item_type * gsn_encode_rmv_nsapi_xid( gas_id_t, const gsn_nsapi_e_t * );

/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_DEF_XID()
===
===  DESCRIPTION
===
===   Encodes T0, T1 and T2 XID parameters with default values
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   Pointer to the xid bock
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/
dsm_item_type *gsn_encode_def_xid ( void );

#endif /* INC_GSN_UTIL_H */

/*** EOF: don't remove! ***/
