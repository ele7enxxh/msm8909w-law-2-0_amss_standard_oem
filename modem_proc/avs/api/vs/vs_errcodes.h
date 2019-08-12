#ifndef __VS_ERRCODES_H__
#define __VS_ERRCODES_H__

/**
  @file vs_errcodes.h
  @brief
      This file defines the error codes for the Voice Services

*/

/*
  ============================================================================

   Copyright (C) 2012, 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_errcodes.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  11/16/12  sud   Initial revision

  ===========================================================================*/

/** @addtogroup vs_errcodes
@{ */

#define VS_EOK          ( 0x00000000 ) /**< Success; completed with no errors. */
#define VS_EFAILED      ( 0x00000001 ) /**< General failure. */
#define VS_EBADPARAM    ( 0x00000002 ) /**< Bad operation parameter(s). */
#define VS_EUNSUPPORTED ( 0x00000003 ) /**< Unsupported routine/operation. */
#define VS_EVERSION     ( 0x00000004 ) /**< Unsupported version. */
#define VS_EUNEXPECTED  ( 0x00000005 ) /**< Unexpected problem encountered. */
#define VS_EPANIC       ( 0x00000006 ) /**< Unhandled problem occurred. */

#define VS_ENORESOURCE  ( 0x00000007 ) /**< Unable to allocate resource(s). */
#define VS_EHANDLE      ( 0x00000008 ) /**< Invalid handle. */
#define VS_EALREADY     ( 0x00000009 ) /**< Operation is already processed. */
#define VS_ENOTREADY    ( 0x0000000A ) /**< Operation is not ready to be processed. */
#define VS_EPENDING     ( 0x0000000B ) /**< Operation is pending completion. */
#define VS_EBUSY        ( 0x0000000C ) /**< Operation could not be accepted or processed. */
#define VS_EABORTED     ( 0x0000000D ) /**< Operation aborted due to an error. */
#define VS_EPREEMPTED   ( 0x0000000E ) /**< Operation preempted by a higher priority. */
#define VS_ECONTINUE    ( 0x0000000F ) /**< Operation requests intervention to complete. */
#define VS_EIMMEDIATE   ( 0x00000010 ) /**< Operation requests immediate intervention to complete. */
#define VS_ENOTIMPL     ( 0x00000011 ) /**< Operation is not implemented. */
#define VS_ENEEDMORE    ( 0x00000012 ) /**< Operation needs more data or resources. */
#define VS_ELPC         ( 0x00000013 ) /**< Operation is a local procedure call. */
#define VS_ENOMEMORY    ( 0x00000014 ) /**< Unable to allocate enough memory. */
#define VS_ENOTEXIST    ( 0x00000015 ) /**< Item does not exist. */

/** @} */ /* end_addtogroup vs_errcodes */

#endif  /* __VS_ERRCODES_H__ */

