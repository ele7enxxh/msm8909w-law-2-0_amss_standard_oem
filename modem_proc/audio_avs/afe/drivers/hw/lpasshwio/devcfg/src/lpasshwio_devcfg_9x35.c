#include "lpasshwio_devcfg.h"

HwdLpassPropertyType lpass_prop ={0xFE04F000};
HwdAudioIfPropertyType audioif_prop = {0xFE140000, (128*1024), 0, 
   0, 4, 2, 2, 4, {0,1,2,3}, {4,4,4,4}, {3,3,3,3}, 19, 2, 0x30010000, 1};
HwdAvtimerPropertyType avtimer_prop = {{0xFE0C9000, 0, 0}, {0x1000, 0, 0}, {19200000, 0, 0}, 50, 0x30040000};
HwdDmlitePropertyType dmlite_prop = {0xFE0B2000, 0x1000, 23, 0x30010000};
HwdResamplerPropertyType resampler_prop = {0, 0, 0, 0};
lpasshwio_vfr_prop_t vfr_prop = {0xFE03C000, 0x1000, 0x30010000, 0x1, {{36, 0x0, 0x1}, {40, 0x7, 0x1}, {0, 0x0, 0x0}, {0, 0x0, 0x0}}};
lpasshwio_prop_riva_struct_t afe_riva_prop = {0, 0, 0, 0, 0, 0, 0};
lpasshwio_prop_slimbus_struct_t afe_slimbus_prop = {1};
HwdMidiPropertyType midi_prop = {0, 0, 0, 0, 0, 0};
lpasshwio_prop_spdif_struct_t spdiftx_prop = {0, 0, 0, 0, 0, 0, 0};
lpasshwio_prop_hdmi_input_struct_t hdmi_input_prop = {0, 0, 0, 0};