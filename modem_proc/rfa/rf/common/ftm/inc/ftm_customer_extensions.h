#ifndef FTM_CUSTEXT_H
#define FTM_CUSTEXT_H
/*===========================================================================

   F T M    C U S T O M E R    E X T E N S I O N S    H E A D E R    F I L E

DESCRIPTION
   This file contains declarations associated with the FTM Services module.

REFERENCES
   None

Copyright (c) 2002, 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                       EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/05   jf      Initial check in with sample code
===========================================================================*/
#include "rfa_variation.h"
#include "target.h"                // Target specific definitions            //
#include "comdef.h"                // Common stuff                           //
#include "ftmicap.h"               // Add for FTM feature definition         //


#ifdef FEATURE_FTM_CUSTOMER_EXTENSIONS

#include "ftm.h"
#include "ftmdiag.h"
/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* ftm customer extension Enumuration */
typedef enum
{
  FTM_CUSTEXT_SAMPLE = 0,
  FTM_CUSTEXT_MAX
}ftm_custext_cmd_type;

/*===========================================================================
* Packet Definitions
===========================================================================*/
/* ftm customer extension request type */ 
typedef PACK(struct) 
{
   ftm_diagpkt_subsys_header_type  diag_hdr;
   ftm_cmd_header_type             ftm_hdr;
   void                            *data;
} ftm_custext_pkt_type;

/*===========================================================================

FUNCTION FTM_CUSTOMER_EXTENSIONS_DISPATCH

DESCRIPTION
    This function handles Customer Extension FTM requests to run tests and
    other primitives by dispatching the appropriate functions.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.

===========================================================================*/
ftm_rsp_pkt_type ftm_customer_extensions_dispatch(ftm_custext_pkt_type *req_pkt);

#endif /* FTM_HAS_CUSTOMER_EXTENSIONS */
#endif //FTM_CUSTEXT_H


