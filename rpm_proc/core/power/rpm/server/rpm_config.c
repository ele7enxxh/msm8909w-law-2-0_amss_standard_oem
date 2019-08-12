/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "rpm_config.h"

static const unsigned SUPPORTED_CLASSES   = 20;
static const unsigned SUPPORTED_RESOURCES = 128;

//
// BEGIN config data; should migrate to the system enumeration data method
//
static SystemData temp_config_data =
{
    .num_ees = 5, // 5 EE's, [apps, modem, pronto, tz, qdsp]

    .ees    = (EEData[] ) {
        [0] = {
            .edge      = SMD_APPS_RPM,
            .smd_fifo_sz = 1024,
            .ee_buflen = 256,
            .priority  = 4,
            .wakeupInt = (1 << 5) | (1 << 7),

#if ((DAL_CONFIG_TARGET_ID == 0x9645) || (DAL_CONFIG_TARGET_ID == 0x9607))
            .spm       = {
                             .numCores = 1,
                             .bringupInts  = (unsigned[]) {  1 },
                             .bringupAcks  = (unsigned[]) { 16 },
                             .shutdownInts = (unsigned[]) {  0 },
                             .shutdownAcks = (unsigned[]) {  0 },
                         },
#else
            .spm       = {
                             .numCores = 1,
                             .bringupInts  = (unsigned[]) {  15 },
                             .bringupAcks  = (unsigned[]) {  20 },
                             .shutdownInts = (unsigned[]) {  14 },
                             .shutdownAcks = (unsigned[]) {   4 },
                         },
#endif
        },
        [1] = {
            .edge      = SMD_MODEM_RPM,
            .smd_fifo_sz = 1024,
            .ee_buflen = 1024,
            .priority  = 2,
            .wakeupInt = (1 << 13) | (1 << 15),
            .spm       = {
                             .numCores = 1,
                             .bringupInts  = (unsigned[]) { 25 },
                             .bringupAcks  = (unsigned[]) { 22 },
                             .shutdownInts = (unsigned[]) { 24 },
                             .shutdownAcks = (unsigned[]) {  6 },
                         },
        },
        [2] = {
            .edge      = SMD_RIVA_RPM,
            .smd_fifo_sz = 1024,
            .ee_buflen = 256,
            .priority  = 1,
            .wakeupInt = (1 << 17) | (1 << 19),
            .spm       = {
                             .numCores = 1,
                             .bringupInts  = (unsigned[]) { 31 },
                             .bringupAcks  = (unsigned[]) { 23 },
                             .shutdownInts = (unsigned[]) { 30 },
                             .shutdownAcks = (unsigned[]) {  7 },
                         },
        },
        [3] = {
            .edge      = SMD_RPM_TZ,
            .smd_fifo_sz = 1024,
            .ee_buflen = 256,
            .priority  = 5,
            .wakeupInt = 0,
            .spm       = {
                             .numCores = 0,
                             .bringupInts  = (unsigned[]) { 31 },
                             .bringupAcks  = (unsigned[]) { 23 },
                             .shutdownInts = (unsigned[]) { 30 },
                             .shutdownAcks = (unsigned[]) {  7 },
                         },
        },
        [4] = {
            .edge      = SMD_QDSP_RPM,
            .smd_fifo_sz = 1024,
            .ee_buflen = 256,
            .priority  = 3,
            .wakeupInt = 0,
            .spm       = {
                             .numCores = 1,
                             .bringupInts  = (unsigned[]) { 55 },
                             .bringupAcks  = (unsigned[]) { 21 },
                             .shutdownInts = (unsigned[]) { 54 },
                             .shutdownAcks = (unsigned[]) {  5 },
                         },
        },
    },

    .supported_classes   = SUPPORTED_CLASSES,
    .supported_resources = SUPPORTED_RESOURCES,
    .classes             = (ResourceClassData[SUPPORTED_CLASSES]) { 0 },
    .resources           = (ResourceData[SUPPORTED_RESOURCES])    { 0 },
    .resource_seeds      = (int16_t[SUPPORTED_RESOURCES])         { 0 },
};
//
// END config data
//

SystemData * const rpm = &temp_config_data;

