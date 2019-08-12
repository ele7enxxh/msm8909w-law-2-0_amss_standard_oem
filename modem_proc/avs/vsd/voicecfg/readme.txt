How to enable and disable eAMR mode change detection and mvs event sequence. 

By default, eAMR mode change detection is disabled.

By default, mvs event sequence is off i.e the sequence is command cb -> MODE INIT -> MODE READY
and when mvs event is on the sequence is MODE INIT -> MODE READY->command cb

First create a folder named voice in EFS Explorer under /nv/item_files/

1. Enable eAMR mode change detection and enable mvs event sequence 
Drag and drop the voicenvcfg.bin from modem build
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_config\eamr_notify_on\mvs_event_sequence_on
to target EFS Explorer /nv/item_files/voice/
Reboot target.

2. Enable eAMR mode change detection and disable mvs event sequence
Drag and drop the voicenv.bin from modem build
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_config\eamr_notify_on\mvs_event_sequence_off
to target EFS Explorer /nv/item_files/voice/
Reboot target.

3. Disable eAMR mode change detection and enable mvs event sequence 
Drag and drop the voicenvcfg.bin from modem build
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_config\eamr_notify_off\mvs_event_sequence_on
to target EFS Explorer /nv/item_files/voice/
Reboot target.

4. Disable eAMR mode change detection and disable mvs event sequence
Drag and drop the voicenv.bin from modem build
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_config\eamr_notify_off\mvs_event_sequence_off
to target EFS Explorer /nv/item_files/voice/
Reboot target.

