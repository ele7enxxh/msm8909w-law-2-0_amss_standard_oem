/*
 *
 * Copyright (c) 2013, 2015 QUALCOMM Technologies, Inc. All Rights Reserved.
 * QUALCOMM Technologies Proprietary and Confidential.
 *
 */
#ifndef __ADIERTAC_H__
#define __ADIERTAC_H__

#include "acph.h"

#ifndef _ANDROID_
#define strlcat g_strlcat
#define strlcpy g_strlcpy
#endif

#define ACPH_CAL_DATA_UNIT_LENGTH   4

#define CODEC_INFO_PATH "/proc/asound/card0/codecs/"

struct chipset_id_info {
	char	*name;
	int	id;
};

static struct chipset_id_info codec_chipset[] = {
	{"MSM8X52", MSM8X52},
	{"WCD9335", WCD9335},
	{"WSA881X-ANALOG", WSA881X_ANALOG},
	{"WSA881X-SOUNDWIRE", WSA881X_SOUNDWIRE},
	{"WSA881X", WSA881X},
	{"MSM8909", MSM8909},
	{"WCD9330", WCD9330},
	{"WCD9326", WCD9326},
	{"WCD9320", WCD9320},
	{"WCD9310", WCD9310},
	{"WCD9306", WCD9306},
	{"WCD9302", WCD9302},
	{"MSM8X16", MSM8X16},
	{"MSM8X10", MSM8X10}
};

enum {
	CODEC_MSM8X52 = 0,
	CODEC_WCD9335,
	CODEC_WSA8810,
	CODEC_MSM8909,
	CODEC_MSM8994,
	MAX_CODEC_CHIPSET
};

void adie_rtac_init(void);
void adie_rtac_exit(void);
#endif
