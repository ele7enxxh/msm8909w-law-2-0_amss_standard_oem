/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include "SecureTouchLayout.h"
#include "SecureTouchError.h"
#include "LayoutUtils.h"
#include <stdbool.h>
#include <string.h>

uint32_t findObject(const uint32_t x, const uint32_t y, LayoutPage_t *page, LayoutObject_t **obj) {
  uint32_t rv = SSE_OK;
  ENTER;
  do {
    if ((obj == NULL) || (*obj != NULL)) {
      LOG_E("obj NULL or already pointing to an object!");
      rv = E_SSE_INVALID_PARAMETER;
      break;
    }
    if (!page) {
      LOG_E("NULL page");
      rv = E_SSE_NULL_POINTER;
      break;
    }
    LOG_D("Looking for object at location (%u,%u)", x, y);
    for (uint32_t i = 0; i < page->number; i++) {
      Rect_t *r = &page->objects[i].rect;
      LOG_D("Checking object %u: [(%u,%u),(%u,%u)]",
        i,
        r->x1, r->y1,
        r->x2, r->y2
        );
      if ((x >= r->x1) &&
          (x <= r->x2) &&
          (y >= r->y1) &&
          (y <= r->y2)) {
        *obj = &page->objects[i];
        break;
      }
    }
    if (*obj == NULL) {
      LOG_E("No object found at the requested location: (%d,%d)", x, y);
      rv = E_SSE_INVALID_RANGE;
    }
  } while (0);
  EXITV(rv);
}

static bool isOrdered(const Rect_t *rect) {
  if ((rect->x1 <= rect->x2) && (rect->y1 <= rect->y2)) {
    return true;
  }
  return false;
}

static bool isContained(const Rect_t *rect, const uint32_t w, const uint32_t l) {
  if ((rect->x1 < w) &&
      (rect->x2 < w) &&
      (rect->y1 < l) &&
      (rect->y2 < l)) {
    return true;
  }
  return false;
}

static bool areOverlapped(const Rect_t *r1, const Rect_t *r2) {
  if ((r2->x2 < r1->x1) || (r2->x1 > r1->x2)) {
    // they do not overlap on X, so they do not overlap
    return false;
  }
  // we know they overlap on X, do they on Y?
  if ((r2->y2 < r1->y1) || (r2->y1 > r1->y2)) {
    // they do not overlap on Y, so they do not overlap
    return false;
  }
  return true;
}


/** Verify that fields in the page do not overlap
 */
uint32_t verifyLayout(
  LayoutPage_t *page,
  SecureTouch_InputStatus_t *inputStatus,
  const uint32_t width,
  const uint32_t height)
{
  uint32_t rv = SSE_OK;
  uint32_t index = 0; // used to assign an index number to input fields
  uint32_t i = 0;
  uint32_t j = 0;
  ENTER;
  do {
    if (!page) {
      LOG_E("NULL page");
      rv = E_SSE_NULL_POINTER;
      break;
    }
    if ((page->number == 0) ||
        (page->number > MAX_LAYOUT_OBJECTS_IN_PAGE)) {
      LOG_E("Invalid number of objects");
      rv = E_SSE_INVALID_PARAMETER;
      break;
    }
    LOG_D("Page layout: number of objects: %d", page->number);
    // go through all objects in page, that they are ordered and within the screen
    // meanwhile also count and check input fields and check buttons' content
    for (i = 0; i < page->number; i++) {
      LayoutObject_t *obj = &page->objects[i];
      if (obj->type == LAYOUT_OBJECT_TYPE_INPUT) {
        if (index >= MAX_INPUT_FIELDS) {
          LOG_E("Too many input fields in the layout: >%d vs %d max",
                index, MAX_INPUT_FIELDS);
          rv = E_SSE_INVALID_PARAMETER;
          break;
        }
        obj->input.index = index++;
        LOG_D("Object %u: INPUT, idx=%u", i, obj->input.index);
      } else if (obj->type == LAYOUT_OBJECT_TYPE_BUTTON) {
        LayoutObjectButton_t *button = &obj->button;
        if (button->type == LAYOUT_BUTTON_TYPE_DATA) {
          LayoutButtonData_t *data = &button->data;
          if ((data->length > UTF8_MAX_CHAR_LEN) || (data->length == 0)) {
            LOG_E("Unsupported data length (%d) for object %d",data->length, i);
            rv = E_SSE_INVALID_PARAMETER;
            break;
          }
          LOG_D("Object %u: DATA, len=%u", i, data->length);
          // TODO: check that it is a valid UTF8 character, import the function from TLP11
        } else if (button->type == LAYOUT_BUTTON_TYPE_FUNCTION) {
          LOG_D("Object %u: DATA, function=%u", i, button->function);
          if ((button->function != LAYOUT_BUTTON_FUNCTION_ENTER) &&
              (button->function != LAYOUT_BUTTON_FUNCTION_CANCEL) &&
              (button->function != LAYOUT_BUTTON_FUNCTION_BACKSPACE) &&
              (button->function != LAYOUT_BUTTON_FUNCTION_CLEAR)) {
            LOG_E("Unsupported function (0x%08X) for object %d", button->function, i);
            rv = E_SSE_INVALID_PARAMETER;
            break;
          }
        } else {
          LOG_E("Unsupported button type: [%d] 0x%08X", i, button->type);
          rv = E_SSE_INVALID_PARAMETER;
          break;
        }
      } else if (obj->type == LAYOUT_OBJECT_TYPE_OTHER) {
        // happy to ignore it
      } else {
        LOG_E("Unsupported object type: [%d] 0x%08X", i, obj->type);
        rv = E_SSE_INVALID_PARAMETER;
        break;
      }
      if (!isOrdered(&obj->rect)) {
        LOG_E("Object %d is not ordered: (%d,%d),(%d,%d)",
              i,
              obj->rect.x1, obj->rect.y1,
              obj->rect.x2, obj->rect.y2);
        rv = E_SSE_INVALID_PARAMETER;
        break;
      }
      if (!isContained(&obj->rect, width, height)) {
        LOG_E("Object %d is not contained on the screen: (%d,%d),(%d,%d) vs (%d,%d)",
              i,
              obj->rect.x1, obj->rect.y1,
              obj->rect.x2, obj->rect.y2,
              width, height);
        rv = E_SSE_INVALID_PARAMETER;
        break;
      }

    }
    inputStatus->num_input = index;
    if (rv != SSE_OK) {
      break;
    }
    // now check if they overlap
    for (i = 0; i < page->number; i++) {
      // check if it overlaps with any of the remaining objects
      for (j = i + 1; j < page->number; j++) {
        if (areOverlapped(&page->objects[i].rect, &page->objects[j].rect)) {
          LOG_E("Objects %d and %d overlaps: {(%d,%d),(%d,%d)} and {(%d,%d),(%d,%d)}",
                i, j,
                page->objects[i].rect.x1, page->objects[i].rect.y1,
                page->objects[i].rect.x2, page->objects[i].rect.y2,
                page->objects[j].rect.x1, page->objects[j].rect.y1,
                page->objects[j].rect.x2, page->objects[j].rect.y2);
          rv = E_SSE_INVALID_PARAMETER;
          break;
        }
      }
    }
  } while (0);
  EXITV(rv);
}

uint32_t updatePage(
  const uint32_t x,
  const uint32_t y,
  LayoutPage_t *page,
  SecureTouch_InputStatus_t *inputStatus,
  SecureTouchResponse_t *response,
  SecureTouch_Event_t *event)
{
  uint32_t rv = SSE_OK;
  LayoutObject_t *obj = NULL;
  ENTER;
  do {
    if (!page || !inputStatus || !response || !event) {
      LOG_E("NULL parameter");
      rv = E_SSE_NULL_POINTER;
      break;
    }
    if (inputStatus->status != SECURETOUCH_COMPLETION_STATUS_INPROGRESS) {
      LOG_E("No input in progress, ignore");
      rv = E_ST_INPUT_NOT_IN_PROGRESS;
      break;
    }
    // valid touch, map it to a layout object
    rv = findObject(x, y, page, &obj);
    if (rv != SSE_OK) {
      LOG_W("Failed to map the touch to an object: 0x%08X", rv);
      rv = SSE_OK;
      break;
    }
    if (obj->type == LAYOUT_OBJECT_TYPE_INPUT) {
      // we might have changed focus
      LOG_I("Set focus to input element %u", obj->input.index);
      inputStatus->focus = obj->input.index;
      break;
    }
    if ((obj->type == LAYOUT_OBJECT_TYPE_BUTTON)) {
      LOG_D("Button pressed");
      // it's a button
      if (obj->button.type == LAYOUT_BUTTON_TYPE_FUNCTION) {
        if (obj->button.function == LAYOUT_BUTTON_FUNCTION_CLEAR) {
          LOG_D("Clear button pressed");
          // clear the input with focus
          response->length[inputStatus->focus] = 0;
          memset(response->input_fields[inputStatus->focus], 0, MAX_INPUT_FIELD_LEN);
          inputStatus->num_chars[inputStatus->focus] = 0;
          memset(inputStatus->num_bytes_in_char[inputStatus->focus], 0, MAX_INPUT_FIELD_LEN);
        } else if (obj->button.function == LAYOUT_BUTTON_FUNCTION_BACKSPACE) {
          LOG_D("Backspace button pressed");
          if (inputStatus->num_chars[inputStatus->focus] > 0) {
            uint32_t num_bytes_in_char = inputStatus->num_bytes_in_char[inputStatus->focus][inputStatus->num_chars[inputStatus->focus]-1];
            LOG_D("Removing last char, %u bytes", num_bytes_in_char);
            // there is a char to remove
            // remove the number of bytes
            response->length[inputStatus->focus] -= num_bytes_in_char;
            LOG_D("New length of the input field %u: %u", inputStatus->focus, response->length[inputStatus->focus]);
            // zero them, since we're paranoid
            for (uint32_t i = 0; i < num_bytes_in_char; i++) {
              response->input_fields[inputStatus->focus][response->length[inputStatus->focus] + i] = 0;
            }
            inputStatus->num_bytes_in_char[inputStatus->focus][inputStatus->num_chars[inputStatus->focus] - 1] = 0;
            inputStatus->num_chars[inputStatus->focus] -= 1;
          }
        } else if (obj->button.function == LAYOUT_BUTTON_FUNCTION_CANCEL) {
          LOG_D("Cancel button pressed");
          // nothing will be returned
          inputStatus->status = SECURETOUCH_COMPLETION_STATUS_CANCELLED;
          memset(response->length, 0, sizeof(response->length));
          memset(response->input_fields, 0, sizeof(response->input_fields));
          *event = SECURETOUCH_EVENT_CMD_CANCELLED;
        } else if (obj->button.function == LAYOUT_BUTTON_FUNCTION_ENTER) {
          LOG_D("Complete button pressed");
          inputStatus->status = SECURETOUCH_COMPLETION_STATUS_COMPLETED;
          *event = SECURETOUCH_EVENT_CMD_COMPLETED;
        }
      } else if (obj->button.type == LAYOUT_BUTTON_TYPE_DATA) {
        // add associated data to current input, if there is enough room
        LayoutButtonData_t *data = &obj->button.data;
        LOG_D("Data button pressed. Adding %u bytes to input %u", data->length, inputStatus->focus);
        if (response->length[inputStatus->focus] + data->length < MAX_INPUT_FIELD_LEN) {
          for (uint32_t i = 0; i < data->length; i++) {
            response->input_fields[inputStatus->focus][response->length[inputStatus->focus]] = data->data[i];
            response->length[inputStatus->focus]++;
          }
          inputStatus->num_bytes_in_char[inputStatus->focus][inputStatus->num_chars[inputStatus->focus]] = data->length;
          inputStatus->num_chars[inputStatus->focus] += 1;
        }
      }
    }
  } while (0);
  EXITV(rv);
}
