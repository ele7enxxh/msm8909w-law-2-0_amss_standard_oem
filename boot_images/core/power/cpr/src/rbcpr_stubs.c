// cpr.h - interface description of cpr driver
//
// Copyright 2013 by Qualcomm Technologies, Inc.
// All Rights Reserved
// Confidental and Proprietary
//
// $ Header$
// $ Author$
// $ DateTime$

#include "rbcpr.h"

uint32 rbcpr_cx_mx_settings_checksum(void) { return 0xDEADBEEF; }

void rbcpr_init(void) { }
void rbcpr_externalize_state(void) {}
