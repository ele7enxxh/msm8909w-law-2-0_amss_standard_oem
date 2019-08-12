#ifndef __VSS_DELETED_PUBLIC_IF_H__
#define __VSS_DELETED_PUBLIC_IF_H__

/*===========================================================================
NOTE: This file contains the APR API definitions that are deleted and
      unsupported by the Core Voice Driver (CVD). The only purpose of this file
      is to allow CVD's code compilation.
=============================================================================*/
/*===========================================================================
Copyright (C) 2014 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_deleted_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/

/****************************************************************************
 * DELETED CALIBRATION FRAMEWORK DEFINITIONS                                *
 ****************************************************************************/

/*--------------------------------------------------------------------------*
 * DELETED CALIBRATION COLUMN IDS                                           *
 *--------------------------------------------------------------------------*/

/** Column vocoder class. */
#define VSS_ICOMMON_CAL_COLUMN_VOC_CLASS ( 0x00011359 )

/*--------------------------------------------------------------------------*
 * DELETED CALIBRATION VOCODER CLASS COLUMN VALUES                          *
 *--------------------------------------------------------------------------*/

/** NONE vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_NONE ( 0x00011364 )

/** CDMA vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_CDMA ( 0x00011365 )

/** GSM vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_GSM ( 0x00011366 )

/** WCDMA vocoder class */
#define VSS_ICOMMON_CAL_VOC_CLASS_WCDMA ( 0x00011367 )

#endif /* __VSS_DELETED_PUBLIC_IF_H__ */

