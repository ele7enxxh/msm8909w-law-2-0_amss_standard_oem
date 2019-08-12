#ifndef __VSS_ISOURCETRACK_PUBLIC_IF_H__
#define __VSS_ISOURCETRACK_PUBLIC_IF_H__

/*===========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "apr_comdef.h"

/****************************************************************************
 * SOURCE TRACK DEFINES                                                     *
 ****************************************************************************/

/** Indicates that the voice activity detector output is not available. */
#define VSS_ISOURCETRACK_VOICE_ACTIVITY_NOT_AVAILABLE ( 0xFF )

/** Indicates that the direction of arrival for the desired
    talker or interferer is not available. */
#define VSS_ISOURCETRACK_DOA_NOT_AVAILABLE ( 0xFFFF )

/****************************************************************************
 * SOURCE TRACK APIS                                                        *
 ****************************************************************************/

/** 
  Gets the sound activity data.
 
  @apr_msgpayload{vss_isourcetrack_get_activity_t}
 
  @detdesc 
  Sound activity data include the voice activity detector output,
  direction of arrival for desired talker, direction of arrival for interferer, 
  and sound strengths at each degree on 360 degree circle.
  It is recommended to get sound activity data every 50ms.
  Client shall only read the data from shared memory after receiving #APR_EOK as
  the command response.
  The layout of sound activity data in the shared memory is defined by the
  #vss_isourcetrack_activity_data_t structure.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  Before using this command, the client must have mapped memory with CVD
  successfully.
  For CVD memory mapping information, see Section @xref{hdr:ShMemGuidelines}.
*/

#define VSS_ISOURCETRACK_CMD_GET_ACTIVITY ( 0x00013136 )

/* Type definition for vss_isourcetrack_cmd_get_activity_t. */
typedef struct vss_isourcetrack_cmd_get_activity_t vss_isourcetrack_cmd_get_activity_t;

#include "apr_pack_begin.h"

/* Payload structure for the VSS_ISOURCETRACK_CMD_GET_ACTIVITY command.
 */
struct vss_isourcetrack_cmd_get_activity_t
{
  uint32_t mem_handle;
  /**< Handle to the shared memory where the sound activity data 
       is returned.
       For more information, see Section @xref{hdr:ShMemGuidelines}. */

  uint64_t mem_address;
  /**< Location where the sound activity data is returned.
       For information on how the address is intrepreted, see
       Section @xref{hdr:ShMemGuidelines}. */

  uint32_t mem_size;
  /**< Size of the memory in bytes where the sound activity data is returned.
       The size must meet the requirements specified in
       Section @xref{hdr:ShMemGuidelines}.
       The size must also be large enough to accommodate the sound activity
       data as defined via the #vss_isourcetrack_activity_data_t structure. */
}
#include "apr_pack_end.h"
;

/* Type definition for vss_isourcetrack_activity_data_t. */
typedef struct vss_isourcetrack_activity_data_t vss_isourcetrack_activity_data_t;

#include "apr_pack_begin.h"

/* Structure for the sound activity data. */
struct vss_isourcetrack_activity_data_t
{
  uint8_t voice_active[ 8 ];
  /**< An array of voice activity detector outputs corresponding to each sector. 
       For sector definition, refer to #VSS_ISOUNDFOCUS_CMD_SET_SECTORS.

       @values 
       - TRUE -- The corresponding sector has active voice. 
       - FALSE -- The corresponding sector has no active voice. 
       - #VSS_ISOURCETRACK_VOICE_ACTIVITY_NOT_AVAILABLE -- The information 
       is not available for the corresponding sector. */

  uint16_t talker_doa;
  /**< Direction (in degrees) of arrival for desired talker. 
 
       @values 
       If direction of arrival is available, the value will be an integer 
       between 0 and 359. Otherwise, the value will be
       #VSS_ISOURCETRACK_DOA_NOT_AVAILABLE. */

  uint16_t interferer_doa[ 3 ];
  /**< An array of directions (in degrees) of arrival for interferers. 
       The maximum number of interferers can be tracked is three.
 
       @values 
       If direction of arrival is available, the value will be an integer 
       between 0 and 359. Otherwise, the value will be
       #VSS_ISOURCETRACK_DOA_NOT_AVAILABLE. */

  uint8_t sound_strength[ 360 ];
  /**< An array of sound strength at each degree on a 360 degree plane. 
 
       @values 
       Integer between 0 and 255. */
}
#include "apr_pack_end.h"
;

#endif /* __VSS_ISOURCETRACK_PUBLIC_IF_H__ */
