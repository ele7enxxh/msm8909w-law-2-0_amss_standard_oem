/*****************************************************************************
***
*** TITLE
***
***  GMM & GSMS PROXY CLIENTS OF GPRS LL SAP SERVICE LAYER
***
***
*** DESCRIPTION
***
***  Provides an interface through which GMM and GSMS can connect to GPRS LLC
***  as clients of the LL SAP service layer. This proxy client adaptation
***  functionality adapts the messaging and data representation differences
***  between GMM/GSMS and GPRS LLC, and also registers for service at the
***  correct LL SAP addresses on behalf of GMM/GSMS.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gllclient.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GLLCLIENT_H
#define INC_GLLCLIENT_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gmmllcif.h"
#include "gsmsllcif.h"

#include "queue.h"

#include "geran_dual_sim.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* Union of all LL SAP messages that can be sent to LLC from GMM via message
   adaptation. */

typedef union
{
  gmmllc_ll_prim_e_type          prim;

  gmmllc_ll_unitdata_req_type    ll_unitdata_req;
  gmmllc_ll_xid_req_type         ll_xid_req;
  gmmllc_ll_xid_res_type         ll_xid_res;

} gmmllc_ll_mes_u_type;


typedef struct
{
  q_link_type            link;
  gmmllc_ll_mes_u_type   mes;

} gmmllc_ll_mes_type;

/* ======================================================================== */

/* Union of all LL SAP messages that can be sent to LLC from GSMS via message
   adaptation. */

typedef union
{
  gsmsllc_ll_prim_e_type         prim;

  gsmsllc_ll_unitdata_req_type   ll_unitdata_req;
  gsmsllc_ll_xid_req_type        ll_xid_req;
  gsmsllc_ll_xid_res_type        ll_xid_res;

} gsmsllc_ll_mes_u_type;


typedef struct
{
  q_link_type            link;
  gsmsllc_ll_mes_u_type  mes;

} gsmsllc_ll_mes_type;


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
===  FUNCTION      GMMLLC_LL_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Dynamically allocate sufficient memory from heap to accommodate a
===    message of type gmmllc_ll_mes_type.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to allocated gmmllc_ll_mes_type message buffer if resource
===    is available, otherwise NULL.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern gmmllc_ll_mes_type *gmmllc_ll_get_mes_buf( void );


/*===========================================================================
===
===  FUNCTION      GMMLLC_LL_PUT_MES()
===
===  DESCRIPTION
===
===    Perform appropriate message adaptation and post the resultant message
===    to the appropriate LL SAP message queue of LLC.
===
===  DEPENDENCIES
===
===    Message ptr must have been allocated by gmmllc_ll_get_mes_buf().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    May cause a context switch.
===
===========================================================================*/

extern void gmmllc_ll_put_mes( gmmllc_ll_mes_type *mes_ptr );


/*===========================================================================
===
===  FUNCTION      GSMSLLC_LL_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Dynamically allocate sufficient memory from heap to accommodate a
===    message of type gsmsllc_ll_mes_type.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to allocated gsmsllc_ll_mes_type message buffer if resource
===    is available, otherwise NULL.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern gsmsllc_ll_mes_type *gsmsllc_ll_get_mes_buf( void );


/*===========================================================================
===
===  FUNCTION      GSMSLLC_LL_PUT_MES()
===
===  DESCRIPTION
===
===    Perform appropriate message adaptation and post the resultant message
===    to the appropriate LL SAP message queue of LLC.
===
===  DEPENDENCIES
===
===    Message ptr must have been allocated by gsmsllc_ll_get_mes_buf().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    May cause a context switch.
===
===========================================================================*/

extern void gsmsllc_ll_put_mes( gsmsllc_ll_mes_type *mes_ptr );



/*===========================================================================
===
===  FUNCTION      GLLCLIENT_GEA_CLASSMARK_ENQUIRY()
===
===  DESCRIPTION
===
===    For use by A classmark enquiring client to determine whether the
===    indicated GPRS Encryption Algorithm (GEA) is supported.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE if given GEA is supported, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    This function should not be called by NAS in DSDA targets. This is
===    provided purely for compilation purposes as NAS have single branch
===    supporting many PLs.
===
===========================================================================*/

extern boolean gllclient_gea_classmark_enquiry
(
  gmmllc_gea_e_type       gea
);


/*===========================================================================
===
===  FUNCTION      GLLCLIENT_GEA_CLASSMARK_ENQUIRY_PER_SUB()
===
===  DESCRIPTION
===
===    For use by A classmark enquiring client to determine whether the
===    indicated GPRS Encryption Algorithm (GEA) is supported by the
===    requested AS_ID.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE if given GEA is supported, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern boolean gllclient_gea_classmark_enquiry_per_subs
(
  sys_modem_as_id_e_type as_id,
  gmmllc_gea_e_type      gea
);

#endif /* INC_GLLCLIENT_H */

/*** EOF: don't remove! ***/
