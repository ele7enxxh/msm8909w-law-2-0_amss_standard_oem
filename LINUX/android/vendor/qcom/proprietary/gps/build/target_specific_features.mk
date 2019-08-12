
#List of supported board platform targets
LOC_BOARD_PLATFORM_LIST := msm8960
LOC_BOARD_PLATFORM_LIST += msm8974
LOC_BOARD_PLATFORM_LIST += msm8226
LOC_BOARD_PLATFORM_LIST += msm8610
LOC_BOARD_PLATFORM_LIST += apq8084
LOC_BOARD_PLATFORM_LIST += msm8916
LOC_BOARD_PLATFORM_LIST += msm8994
LOC_BOARD_PLATFORM_LIST += msm8909
LOC_BOARD_PLATFORM_LIST += msm8952
LOC_BOARD_PLATFORM_LIST += msm8992
LOC_BOARD_PLATFORM_LIST += msm8996
LOC_BOARD_PLATFORM_LIST += msm8937
LOC_BOARD_PLATFORM_LIST += msm8953

ifneq (,$(filter $(LOC_BOARD_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))
#default defines for all targets
GPS_LOC_API_V02_ENABLED := true
FEATURE_GNSS_BIT_API := true
GPS_FEATURES := \
        -D FEATURE_COARSE_POS_TIMESTAMP_AGE \
        -D FEATURE_LOC_MW_EFS_ACCESS \
        -D FEATURE_RPC_CHAR_ARRAY \
        -D FEATURE_GNSS_BIT_API
ifeq ($(TARGET_BOARD_PLATFORM),msm8960) # MSM8960 specific defines
NO_GSIFF_DSPS := true
else ifeq ($(TARGET_BOARD_PLATFORM),msm8974) # MSM8974 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8226) # MSM8226 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8610) # MSM8610 specific defines
NO_GSIFF_DSPS := true
else ifeq ($(TARGET_BOARD_PLATFORM),apq8084) # APQ8084 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8916) # MSM8916 specific defines
NO_GSIFF_DSPS := true
else ifeq ($(TARGET_BOARD_PLATFORM),msm8994) # MSM8994 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8909) # MSM8909 specific defines
NO_GSIFF_DSPS := true
else ifeq ($(TARGET_BOARD_PLATFORM),msm8952) # MSM8952 specific defines
NO_GSIFF_DSPS := true
else ifeq ($(TARGET_BOARD_PLATFORM),msm8992) # MSM8992 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8996) # MSM8996 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8937) # msm8937 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8953) # MSM8953 specific defines
endif
else #no supported target found
LOC_UNSUPPORTED_TARGET := true
endif #is-board-platform-in-list

ifeq ($(TARGET_DEVICE),apq8026_lw)
LW_FEATURE_SET := true
endif

ifeq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
LW_FEATURE_SET := true
endif
