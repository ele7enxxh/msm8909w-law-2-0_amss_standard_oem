
ifeq ($(TARGET),modemSWv4)
export T32_SYS_CPU := HexagonV4C
export T32_MCD_ARCH := v4c
endif

ifeq ($(TARGET),modemFWv4)
export T32_SYS_CPU := HexagonV4C
export T32_MCD_ARCH := v4c
endif

ifeq ($(TARGET),MMv4)
export T32_SYS_CPU := HexagonV4M
export T32_MCD_ARCH := v4m
endif

ifeq ($(TARGET),MMv5)
export T32_SYS_CPU := HexagonV5L
export T32_MCD_ARCH := v5l
endif

ifeq ($(TARGET),MMv5)
export T32_SYS_CPU := HexagonV5C
export T32_MCD_ARCH := v5c
endif

ifeq ($(TARGET),simv5)
export T32_SYS_CPU := HexagonV5C
export T32_MCD_ARCH := v5c
endif

ifeq ($(TARGET),modemv5)
export T32_SYS_CPU := HexagonV5C
export T32_MCD_ARCH := v5c
endif

