#ifndef PS_IPDC_DECOM_CUSTOMER_CODE_H
#define PS_IPDC_DECOM_CUSTOMER_CODE_H
/*==========================================================================

                 PS_IPDC_DECOM_CUSTOMER_CODE. H

GENERAL DESCRIPTION
  This file contains the API for enabling customer's decompress code. The
  code will not be active in our TBS

EXTERNALIZED FUNCTIONS
udc_proc_pmcr_header_wrapper()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2015 QUALCOMM Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================

                      EDIT HISTORY FOR FILE

					  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/15   cx      Initial version

===========================================================================*/

#include "ps_ipdci_ex.h"

/*-------------------------------------------------------------------------
  Define below types in case customer does not define them
--------------------------------------------------------------------------*/
#ifndef uint8
  typedef  unsigned char uint8;
#endif

#ifndef byte
  typedef  unsigned char byte;
#endif

#ifndef uint16
  typedef  unsigned short uint16;
#endif

#ifndef uint32
  typedef  unsigned int uint32;
#endif
/*===========================================================================
FUNCTION UDC_PROC_PMCR_HEADER_WRAPPER

DESCRIPTION
  PROCESS THE PMCR header

DEPENDENCIES
  None

PARAMETERS
  instPtr[In] - pointer point to decompressor instance 
  compPkt[In] - compressed packet
  decompSizePtr[Out] - decompressed packet size
  checksum[In] - the checksum value in the compressed packet
  isCpcrHdrPresent[Out] - flag for CPCR header is present or not
  daccHdrPtrPtr[Out] - the pointer points to the CPCR(DACC) header
  daccPayloadPtrPtr[Out] -  the pointer points to the CPCR(dacc) payload    

RETURN VALUE
   none

SIDE EFFECTS
   none
===========================================================================*/
boolean udc_proc_pmcr_header_wrapper
(
  ipdc_instance_s_type           * instPtr, 
  byte                           * compPkt,   
  uint32                         * decompSizePtr,  
  uint8                            checksum,
  boolean                        * isCpcrHdrPresent,
  byte                          ** daccHdrPtrPtr,
  byte                          ** daccPayloadPtrPtr
);

/*===========================================================================
FUNCTION UDCHDRMEM_PUSH_UDC_HDR

DESCRIPTION
  Push the meta header into UDC header memory

DEPENDENCIES
  The passed in compressed packet has to be full compression packet or header
  only compression packet

PARAMETERS
  compPkt - compressed packet
  
RETURN VALUE
   none

SIDE EFFECTS
   none
===========================================================================*/
boolean udcHdrMem_push_udc_hdr(uint8 * compPkt);

#endif /*PS_IPDC_DECOM_CUSTOMER_CODE_H*/
