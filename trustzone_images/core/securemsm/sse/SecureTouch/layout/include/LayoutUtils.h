/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include "SecureTouchLayout.h"
#include "InputStatus.h"

/**
 * Verify the layout contained in a page.
 */
uint32_t verifyLayout(
  LayoutPage_t *page,
  SecureTouch_InputStatus_t *inputStatus,
  const uint32_t width, 
  const uint32_t height);

/**
 * Find which object in a page is located at a specified position.
 */
uint32_t findObject(const uint32_t x, const uint32_t y, LayoutPage_t *page, LayoutObject_t **obj);

uint32_t updatePage(
  const uint32_t x,
  const uint32_t y,
  LayoutPage_t *page,
  SecureTouch_InputStatus_t *inputStatus,
  SecureTouchResponse_t *response,
  SecureTouch_Event_t *event);
