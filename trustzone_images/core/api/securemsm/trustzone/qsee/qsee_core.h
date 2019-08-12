#ifndef QSEE_CORE_H
#define QSEE_CORE_H

/**
@file qsee_core.h
@brief Provide Core functionality
*/

/*===========================================================================
   Copyright (c) 2014 by QUALCOMM, Technology Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_core.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
09/19/14    cz      Added a new api qsee_read_hw_version()
01/25/14    hw      add new api qsee_get_secure_state
08/09/11    rv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum
{
  QSEE_MEM_TAG_USECASE_CP  = 0x01,
  QSEE_MEM_TAG_USECASE_AUDIO_CP  = 0x02,
  QSEE_MEM_TAG_USECASE_VIDEO_PIXEL_CP  = 0x03,
  QSEE_MEM_TAG_USECASE_MAX = 0x7FFFFFFF
}qsee_mem_usecase_tag_id_t;

typedef struct
{
  uint32 value[2];
} qsee_secctrl_secure_status_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/**
 * @brief     Reads the JTAG ID
 * @return    JTAG ID value
 */
int qsee_read_jtag_id(void);

/**
 * @brief     Reads the serial number from PTE chain
 * @return    Serial number
 */
int qsee_read_serial_num(void);

/**
 * Tests whether all of the range [\c start, \c start + \c len] is in
 * non-secure memory. This is a convenience function to access \c
 * tzbsp_is_ns_area. \c NULL is a valid value for \c start, because
 * physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] len Length of the memory range in bytes.
 *
 * @return \c TRUE if the entire area is in non-secure memory. \c
 * FALSE if the area contains secure memory.
 */
boolean qsee_is_ns_range(const void* start, uint32 len);

/**
 * Tests whether all of the range [\c start, \c end] is in secure
 * memory and also tagged for a particular use case.
 *  
 * @param [in] tag  Tag ID of the memory it should be tagged with
 *  
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end End of the memory range, physical address,
 * included  in the range.
 *
 * @return \c TRUE if the entire area is in secure memory. \c FALSE
 *  if the area contains non-secure memory.
 */
boolean qsee_is_s_tag_area
( 
  uint32                    tag,
  uint32                    start,
  uint32                    end
);


/**
* Reads the status of the HDMI link and hardware HDCP
* @param [out] hdmi_enable  HDMI output enabled
* @param [out] hdmi_sense HDMI sense
* @param [out] hdcp_auth HDCP authentication success.
* @return 0 on success
*/
int qsee_hdmi_status_read
(
  uint32* hdmi_enable,
  uint32* hdmi_sense,
  uint32* hdcp_auth
);

/**
 * check security status on the device
 *
 * @param [out] status   the security status (struct qsee_secctrl_secure_status_t)
 *                       - Bit field definition of the status struct,
 *                        (value 0 means pass and value 1 means fail)
 *                        Bit 0: secboot enabling check 
 *                        Bit 1: check Sec HW key is programmed
 *                        Bit 2: debug disable check
 *                        Bit 3: Anti-rollback check
 *                        Bit 4: fuse config check
 *                        Bit 5: rpmb check (check rpmb production key is provisioned)
 *
 * @return 0 on call success
 */
int qsee_get_secure_state(qsee_secctrl_secure_status_t* status);

/**
 * @brief     Reads the hardware version from TCSR_TCSR_REGS
 * @return    Serial number
 */
uint32 qsee_read_hw_version(void);


/**
 * @brief     return the application ID that is stored in qsee app certificate.
 * @return    64 bits long application ID
 */
int qsee_get_tz_app_id(uint8 * tz_app_id, uint32 id_buf_len);

#endif /* QSEE_CORE_H */

