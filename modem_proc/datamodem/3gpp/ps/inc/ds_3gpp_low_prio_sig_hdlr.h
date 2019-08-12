/*!
  @file
  ds_3gpp_low_prio_sig_hdlr.h

  @brief
  This header file defines the externalized function that interface DSEPS 
  and ESM.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/        
/*==============================================================================

  Copyright (c) 2008 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_low_prio_sig_hdlr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/09/15   vb      Low priority signaling handler

==============================================================================*/

/*===========================================================================
FUNCTION DS_3GPP_READ_LOW_PRIO_BIT
 
DESCRIPTION
  This function takes a byte stream and checks to see if Low_prio_signaling
  is present in the byte stream.
 
  It returns TRUE if Low priority bit is present in the file.
  The value of the low priority bit is returned through a ptr

PARAMETERS 
  Byte stream
  Length of the byte stream
  Output ptr to write the low prio bit.
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  Boolean 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_read_low_prio_bit
(
   uint8   *input_file_stream,
   uint8    input_file_length,
   boolean *low_prio_bit
);


/*===========================================================================
FUNCTION DS_3GPP_LOW_PRIO_GET_NAS_CONFIG_TAG_LENGTH
 
DESCRIPTION 
 The function takes NAS CONFIG read from UIM and for every TAG gives the
 tag length
 
 It also gives the offset --> Offset signifies how many bytes the length field
 occupy

PARAMETERS 
  Input File stream 
  Offset
 
DEPENDENCIES 
  The first byte must always be pointing to the stream length
  
RETURN VALUE 
  tag_length
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
uint8 ds_3gpp_low_prio_get_nas_config_tag_length
(
   uint8   *input_file_stream,
   uint8   *offset
);
/*===========================================================================
FUNCTION  DS_3GPP_GET_LOW_PRIORITY_SIGNALING_BIT

DESCRIPTION
  The function decides the low priority signaling bit's value.
  ie
  Low priority signaling bit will be set if UE is configured for
  Low priority signaling with the below exceptions
 
  Low priority signaling will not be set for
  1. Emergency calls
  2. If emergency PDN is already UP
  3. Access Class from the SIM is 11-15
  
 
PARAMETERS
  pdn_cntxt_p - PDN context pointer
 
DEPENDENCIES
  None.

RETURN VALUE 
  Boolean
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_get_low_priority_signaling_bit
(
  ds_pdn_context_s       *pdn_cntxt_p
);
