#include "lpasshwio_devcfg.h"

HwdLpassPropertyType lpass_prop ={0xFE037000};
HwdAudioIfPropertyType audioif_prop = {0xFE0C0000, (128*1024), 0, 
   0, 4, 5, 4, 6, {0,1,2,3,4,5},{4,4,4,4,4,2}, {3,3,3,3,3,3}, 19, 2, 0x30020000, 0};
HwdAvtimerPropertyType avtimer_prop = {{0xFE053000, 0, 0}, {0x1000, 0, 0}, {19200000, 0, 0}, 50, 0x30000001};
HwdDmlitePropertyType dmlite_prop = {0xFE04A000, 0x1000, 23, 0x30020000};
HwdResamplerPropertyType resampler_prop = {0, 0, 0, 0};
lpasshwio_vfr_prop_t vfr_prop = {0xFE033000, 0x1000, 0x30020000, 0x1, {{36, 0x0, 0x1}, {40, 0x7, 0x1}, {0, 0x0, 0x0}, {0, 0x0, 0x0}}};
lpasshwio_prop_riva_struct_t afe_riva_prop =  {1, 0xFB206008, 0x20, 17, 10, 16, 15};
lpasshwio_prop_slimbus_struct_t afe_slimbus_prop = {0};
HwdMidiPropertyType midi_prop = {0, 0, 0, 0, 0, 0};lpasshwio_prop_spdif_struct_t spdiftx_prop = {0, 0, 0, 0, 0, 0, 0};
lpasshwio_prop_hdmi_input_struct_t hdmi_input_prop = {0, 0, 0, 0};
