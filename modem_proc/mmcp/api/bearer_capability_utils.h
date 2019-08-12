#ifndef _BEARERCAP_PACK_UNPACK_H_
#define _BEARERCAP_PACK_UNPACK_H_
/*==============================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/bearerCap_pack_unpack.h_v   1.0   10 May 2002 15:26:38   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/bearer_capability_utils.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/10/20   kwa     Initial version.
==============================================================================*/

/*==============================================================================

                     INCLUDE FILES FOR MODULE

==============================================================================*/

/*==============================================================================

                             DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION DECLARATIONS

==============================================================================*/
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  pack_bearer_capability

Arguments: 
   Input
     cm_bearer_capability         - Pointer to the Bearer Capability struct containing the data to be packed.

   Output
     data_ptr                         - Pointer to packed byte stream packed by this func(). 
                                            This will be as per the 3GPP 24.008 spec defined format(10.5.4.5) i.e TLV format
     length                            - Pointer to the number of bytes packed by this func()

Return 
     Packing is success or fail

==============================================================================*/
boolean  pack_bearer_capability(byte *data_ptr, cm_bearer_capability_T *cm_bearer_capability, int *length);

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  unpack_bearer_capability

Arguments: 
   Input
     data_ptr            - Pointer byte stream to be unpacked by this func(). 
                              This has to be as per the 3GPP 24.008 spec defined format(10.5.4.5)i.e TLV format
   Output
     cm_bearer_capability   - Pointer to the Bearer Capability struct which this func() will populate as a result of unpacking.
     length                         - Pointer to the number of bytes unpacked by this func()

Return 
     UnPacking is success or fail


==============================================================================*/
boolean  unpack_bearer_capability(byte *data_ptr, cm_bearer_capability_T *cm_bearer_capability, int *length, byte *last_octet_data_ptr);

/*==============================================================================

              
FUNCTION NAME

  init_bearear_capability

Arguments: 
   Input
  
   Output
     cm_bearer_capability      - Pointer to the Bearer Capability struct which this func() will initialise.
  

Return 
     
==============================================================================*/
void init_bearear_capability(cm_bearer_capability_T *cm_bearer_capability);
#endif /* #ifndef _BEARERCAP_PACK_UNPACK_H_ */
