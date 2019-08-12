How to enable and disable AUDIO feature :

By default, AUDIO is ENABLED.

First create a folder named voice in EFS Explorer under /nv/item_files/

1. Enable AUDIO feature :
Drag and drop the voicecfg.bin from :
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_config\voice_support\audio_support_on
to target EFS Explorer /nv/item_files/voice/
Reboot target.

2. Disable AUDIO feature
Drag and drop the voicecfg.bin from :
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_config\voice_support\audio_support_off
to target EFS Explorer /nv/item_files/voice/
Reboot target.

After rebooting the target, double check if the voicecfg.bin is present on target at : /nv/item_files/voice/