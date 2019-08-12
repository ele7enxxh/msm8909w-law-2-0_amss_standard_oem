#ifndef __VSD_ERRCODES_H__
#define __VSD_ERRCODES_H__

/**
  @file vss_errcodes.h
  @brief This file contains the VSD (Voice Services and Drivers) error codes.
*/

/*===========================================================================
Copyright (C) 2013 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/common/vsd_errcodes.h#1 $
  $Author: mplcsds1 $

===========================================================================*/

/** @addtogroup vsd_error_codes
@{ */

/* 0x00000000 to 0x00000FFF are GUIDs pre-allocated for error codes. */

#define VSD_EOK          ( 0x00000000 ) /**< Success / completed / no errors. */
#define VSD_EFAILED      ( 0x00000001 ) /**< General failure. */
#define VSD_EBADPARAM    ( 0x00000002 ) /**< Bad operation parameter(s). */
#define VSD_EUNSUPPORTED ( 0x00000003 ) /**< Unsupported routine/operation. */
#define VSD_EVERSION     ( 0x00000004 ) /**< Unsupported version. */
#define VSD_EUNEXPECTED  ( 0x00000005 ) /**< Unexpected problem encountered. */
#define VSD_EPANIC       ( 0x00000006 ) /**< Unhandled problem occurred. */

#define VSD_ENORESOURCE  ( 0x00000007 ) /**< Unable to allocate resource(s). */
#define VSD_EHANDLE      ( 0x00000008 ) /**< Invalid handle. */
#define VSD_EALREADY     ( 0x00000009 ) /**< Operation is already processed. */
#define VSD_ENOTREADY    ( 0x0000000A ) /**< Operation is not ready to be processed. */
#define VSD_EPENDING     ( 0x0000000B ) /**< Operation is pending completion. */
#define VSD_EBUSY        ( 0x0000000C ) /**< Operation could not be accepted or processed. */
#define VSD_EABORTED     ( 0x0000000D ) /**< Operation aborted due to an error. */
#define VSD_EPREEMPTED   ( 0x0000000E ) /**< Operation preempted by a higher priority. */
#define VSD_ECONTINUE    ( 0x0000000F ) /**< Operation requests intervention to complete. */
#define VSD_EIMMEDIATE   ( 0x00000010 ) /**< Operation requests immediate intervention to complete. */
#define VSD_ENOTIMPL     ( 0x00000011 ) /**< Operation is not implemented. */
#define VSD_ENEEDMORE    ( 0x00000012 ) /**< Operation needs more data or resources. */
#define VSD_ELPC         ( 0x00000013 ) /**< Operation is a local procedure call. */
#define VSD_ENOMEMORY    ( 0x00000014 ) /**< Unable to allocate enough memory. */
#define VSD_ENOTEXIST    ( 0x00000015 ) /**< Item does not exist. */
#define VSD_ENOSERVICE   ( 0x00000016 ) /**< Operation sent to an unregistered service. */
#define VSD_EDENIED      ( 0x00000017 ) /**< Access denied. */

/** @} */ /* end_addtogroup vsd_error_codes */

#endif /* __VSD_ERRCODES_H__ */
