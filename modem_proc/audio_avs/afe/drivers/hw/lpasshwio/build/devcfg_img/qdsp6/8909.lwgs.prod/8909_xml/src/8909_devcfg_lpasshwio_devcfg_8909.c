#include "../config/devcfg_lpasshwio.h"
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int int16;
typedef unsigned short int uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int int64;
typedef unsigned long long int uint64;
typedef unsigned char boolean;
enum QCSR_REG_TYPE {QCSR_REG_RO, QCSR_REG_WO, QCSR_REG_RW, QCSR_REG_COMMAND};
enum
{
   TIMESTAMP_LATCH_TYPE_SOFTWARE = 0,
   TIMESTAMP_LATCH_TYPE_HARDWARE
};
typedef struct
{
   uint32 hw_ver_reg_base;
}HwdLpassPropertyType;
typedef struct
{
   uint32 audHwRegBase;
   uint32 audHwRegSize;
   uint32 hdmiAudHwRegBase;
   uint32 hdmiAudHwRegSize;
   uint32 dmaWordSize;
   uint32 outgoingDmaChCnt;
   uint32 incomingDmaChCnt;
   uint32 infTypeCnt;
   uint32 infTypeList[9];
   uint32 infTypeDataLineCaps[9];
   uint32 infTypeDirection[9];
   uint32 isrHookPinNum;
   uint32 intrOutputIndex;
   uint32 hw_revision;
   uint32 avtimer_latch_type;
} HwdAudioIfPropertyType;
typedef struct
{
   uint32 baseRegAddr[3];
   uint32 regSize[3];
   uint32 rootClk[3];
   uint32 isrHookPinNum;
   uint32 hw_revision;
} HwdAvtimerPropertyType;
typedef struct
{
   uint32 axi_bw[8];
   uint32 mcps_scaling[8];
} HwdMmpmPropertyType;
typedef struct
{
   uint32 baseRegAddr;
   uint32 regSize;
   uint32 intPinNo;
   uint32 hw_revision;
} HwdDmlitePropertyType;
typedef struct
{
   uint32 baseRegAddr;
   uint32 regSize;
   uint32 isrHookPinNum;
   uint32 hw_revision;
} HwdResamplerPropertyType;
typedef struct vfr_src_prop
{
   uint32 l2vic_num;
   uint32 irq_mux_ctl_sel;
   uint32 is_supported;
} vfr_src_prop_t;
typedef struct
{
   uint32 reg_base_addr;
   uint32 reg_size;
   uint32 hw_revision;
   uint32 hw_latch_ver;
   vfr_src_prop_t vfr_src_prop[4];
}lpasshwio_vfr_prop_t;
 typedef struct {
    uint32 hw_revision;
   uint32 ccu_mb0_ctrl_reg_phy_addr;
   uint32 q6ss_ipc_reg_phy_addr;
   uint32 q6ss_ipc_reg_ack_bit_pos;
    uint16 ipc_send_mail_box_offset;
    uint16 internal_bt_tx_int;
 uint16 internal_bt_rx_int;
 uint16 internal_fm_tx_rx_int;
 uint16 internal_bt_fm_ipc_int;
}lpasshwio_prop_riva_struct_t;
 typedef struct {
    uint32 hw_revision;
}lpasshwio_prop_slimbus_struct_t;
typedef struct
{
   uint32 baseRegAddr;
   uint32 baseRegVirtAddr;
   uint32 virtOffset;
   uint32 regSize;
   uint32 intPinNo;
   uint32 hw_revision;
} HwdMidiPropertyType;
typedef struct
{
   uint32 base_reg_phy_addr;
   uint32 reg_size;
   uint32 reset_reg_phy_addr;
   uint32 reset_reg_size;
   uint32 int_irq_no;
   uint32 num_hw_buf;
   uint32 hw_revision;
} lpasshwio_prop_spdif_struct_t;
typedef struct
{
   uint32 reg_base_addr;
   uint32 reg_size;
   uint32 int_irq_no;
   uint32 hw_revision;
} lpasshwio_prop_hdmi_input_struct_t;
HwdLpassPropertyType lpass_prop ={0x7700000};
HwdAudioIfPropertyType audioif_prop =
{
   0x7708000,
   (0x1000*16),
   0,
   0,
   4,
   2,
   2,
   5,
   {0, 1, 3, 4, 5},
   {4, 4, 2, 4, 4},
   {1, 3, 1, 2, 3},
   46,
   1,
   (0x10040000),
   0
};
HwdAvtimerPropertyType avtimer_prop =
{
   {0x7706000, 0, 0},
   {0x1000*2, 0, 0},
   {19200000, 0, 0},
   45,
   0x30040000
};
lpasshwio_vfr_prop_t vfr_prop =
{
   0x7701000,
   0x1000,
   0x10000000,
   0x1,
   {{59, 0x0, 0x1},
   {60, 0x7, 0x1},
   {0, 0x0, 0x0},
   {0, 0x0, 0x0}}
};
lpasshwio_prop_riva_struct_t afe_riva_prop =
{
   1,
   0xA206008,
   0x4105000,
   8,
   0x20,
   47,
   44,
   56,
   40
};
HwdMmpmPropertyType mmpm_prop =
{
   {100, 200, 300, 400, 500, 600, 700, 800},
   {100, 100, 100, 100, 100, 100, 100, 100},
};
lpasshwio_prop_slimbus_struct_t afe_slimbus_prop =
{
   0
};
HwdDmlitePropertyType dmlite_prop =
{
   0,
   0,
   0,
   0
};
HwdResamplerPropertyType resampler_prop =
{
   0,
   0,
   0,
   0
};
HwdMidiPropertyType midi_prop =
{
   0,
   0,
   0,
   0,
   0,
   0
};
lpasshwio_prop_spdif_struct_t spdiftx_prop =
{
   0,
   0,
   0,
   0,
   0,
   0,
   0
};
lpasshwio_prop_hdmi_input_struct_t hdmi_input_prop =
{
   0,
   0,
   0,
   0
};
