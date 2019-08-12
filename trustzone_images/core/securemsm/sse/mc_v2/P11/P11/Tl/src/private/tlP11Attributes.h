/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_ATTRIBUTES_H
#define TLP11_ATTRIBUTES_H

#include "tlP11Objects.h"

/** GetAttributeValue
 */
CK_RV getULongAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    const tlp11_container_object_t *object,
    CK_ULONG *value);

/** GetAttributeValueArray
 */
CK_RV getArrayAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    const tlp11_container_object_t *object,
    uint8_t *value,
    uint32_t *value_len);

/** SetAttributeValue
 */
CK_RV setAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    tlp11_container_object_t *object,
    const uint32_t value);

/** SetArrayAttributeValue
 */
CK_RV setArrayAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    tlp11_container_object_t *object,
    const uint8_t *value,
    const uint32_t value_len);

/**
 * GetArrayAttribute, ONLY for attributes relevant for CKA_WRAP_TEMPLATE
 */
CK_RV getTemplateArrayAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    tlp11_container_object_t *object,
    uint8_t *value,
    uint32_t *value_len
    );

/**
 * GetBoolAttribute, ONLY for attributes relevant for CKA_WRAP_TEMPLATE
 */
CK_RV getTemplateBoolAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    const tlp11_container_object_t *object,
    CK_BBOOL *value);

/**
 * GetULongAttribute, ONLY for attributes relevant for CKA_WRAP_TEMPLATE
 */
CK_RV getTemplateULongAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    const tlp11_container_object_t *object,
    CK_ULONG *value);

#endif /* TLP11_ATTRIBUTES_H */
