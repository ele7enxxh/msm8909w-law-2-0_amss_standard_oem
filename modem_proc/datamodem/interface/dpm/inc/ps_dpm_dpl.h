#ifndef PS_DPM_DPL_H
#define PS_DPM_DPL_H
/*===========================================================================

                     PS_DPM_DPL . H
DESCRIPTION
Header file describing global DPM config definitions.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/inc/ps_dpm_dpl.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/13    bvd     Initial Version
===========================================================================*/


/*===========================================================================

                         EXTERNAL FUNCTIONS

===========================================================================*/
/**
  @brief  This function initialize IPA DPL module. It needs to be called at power up.

  @return  N/A
  
  @code
  @endcode
*/
void ps_dpm_dpl_init
(
  void
);

/**
  @brief  This function flushes all packets from the IPA DPL watermark..

  @return  N/A

  @code
  @endcode
*/
void ps_dpm_dpl_deinit
(
  void
);

/**
  @brief  This function is used to return pointer to DPM DPL WM

  @return  dsm_watermark_type* - Pointer to ps_dpm_dpl_wmk

  @code
  @endcode
*/
dsm_watermark_type *ps_dpmi_dpl_get_wmk
(
  void
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /*TEST_FRAMEWORK*/
#endif /* PS_DPM_DPL_H */