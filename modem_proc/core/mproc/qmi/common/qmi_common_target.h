#ifndef QMI_COMMON_TARGET_H
#define QMI_COMMON_TARGET_H
/******************************************************************************
  @file    qmi_common_target.h
  @brief   The QMI Common OS specific definition

  DESCRIPTION
  QMI common OS specific defines
   
  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <string.h>
#include "stringl.h"

/*=============================================================================
  MACRO  QMI_MEM_COPY
=============================================================================*/
/*!
@brief
  Copy over from source buffer to destination buffer.

@param[in]  dst     Pointer to the destination buffer
@param[in]  d_len   Size in bytes of the destination buffer
@param[in]  src     Pointer to the source buffer
@param[in]  c_len   Size in bytes to copy from source to destination

@retval   Total number of bytes which were copied from source to destination
*/
/*=========================================================================*/
#define QMI_MEM_COPY(dst, d_len, src, c_len) memscpy(dst, d_len, src, c_len)

#endif
