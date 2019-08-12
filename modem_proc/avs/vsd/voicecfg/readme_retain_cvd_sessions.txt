How to enable and disable retaining CVD sessions and eAMR mode change detection. 

By default, the CVD sessions are destroyed after the end of each call.

By default, eAMR mode change detection is disabled.

First create a folder named voice in EFS Explorer under /nv/item_files/

1. Enable retaining CVD sessions and enable eAMR mode change detection
Drag and drop the voicecfg.bin from modem build
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_config\cvd_sessions_retained_on\eamr_notify_on
to target EFS Explorer /nv/item_files/voice/
Reboot target.

2. Enable retaining CVD sessions and disable eAMR mode change detection
Drag and drop the voicecfg.bin from modem build
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_config\cvd_sessions_retained_on\eamr_notify_off
to target EFS Explorer /nv/item_files/voice/
Reboot target.

3. Disable retaining CVD sessions and enable eAMR mode change detection
Drag and drop the voicecfg.bin from modem build
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_config\cvd_sessions_retained_off\eamr_notify_on
to target EFS Explorer /nv/item_files/voice/
Reboot target.

4. Disable retaining CVD sessions and disable eAMR mode change detection 
Drag and drop the voicecfg.bin from modem build
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_config\cvd_sessions_retained_off\eamr_notify_off
to target EFS Explorer /nv/item_files/voice/
Reboot target.

