#include "lpasshwio_devcfg.h"

HwdLpassPropertyType lpass_prop ={0xFE04F000};
HwdAudioIfPropertyType audioif_prop = {0xFE140000, (128*1024), 0xFD922000, 
   (4*1024), 4, 5, 4, 9, {0,1,2,3,4,5,6,7,8}, {4,4,4,4,4,8,2,8,4}, {3,3,3,3,3,3,1,1,2}, 19, 2, 0x30020001, 1};
HwdAvtimerPropertyType avtimer_prop = {{0xFE0C9000, 0, 0}, {0x1000, 0, 0}, {19200000, 0, 0}, 50, 0x30040000};
HwdDmlitePropertyType dmlite_prop = {0xFE0B2000, 0x1000, 23, 0x30000001};
HwdResamplerPropertyType resampler_prop = {0xFE0D0000, 0x4000, 53, 0x30000001};
lpasshwio_vfr_prop_t vfr_prop = {0xFE03C000, 0x1000, 0x30000001, 0x1, {{36, 0x0, 0x1}, {40, 0x7, 0x1}, {0, 0x0, 0x0}, {0, 0x0, 0x0}}};
lpasshwio_prop_riva_struct_t afe_riva_prop = {1, 0xFB206008, 0x20, 17, 10, 16, 15};
lpasshwio_prop_slimbus_struct_t afe_slimbus_prop = {1};
HwdMidiPropertyType midi_prop = {0,0, 0, 0, 0,0 };
lpasshwio_prop_spdif_struct_t spdiftx_prop = {0, 0, 0, 0, 0, 0, 0};
lpasshwio_prop_hdmi_input_struct_t hdmi_input_prop = {0, 0, 0, 0};