How to enable and disable eAMR for testing. 

eAMR can be enabled or disabled for GSM, WCDMA and TDSCDMA independently. 

By default, eAMR is disabled for  GSM, WCDMA and TDSCDMA.

To configure eAMR, first create a folder named voice in EFS Explorer under /nv/item_files/

1. Enable GSM only
Drag and drop the voicenv.bin 
from modem build
  \modem_proc\avs\vsd\vocsvc\eAMR_config\enable_g_disable_w_disable_tds 
to target EFS Explorer 
  /nv/item_files/voice/
Reboot target.

2. Enable WCDMA only
Drag and drop the voicenv.bin 
from modem build
  \modem_proc\avs\vsd\vocsvc\eAMR_config\disable_g_enable_w_disable_tds
to target EFS Explorer 
  /nv/item_files/voice/
Reboot target.

3. Enable TDSCDMA only
Drag and drop the voicenv.bin 
from modem build
  \modem_proc\avs\vsd\vocsvc\eAMR_config\disable_g_disable_w_enable_tds
to target EFS Explorer 
  /nv/item_files/voice/
Reboot target.

4. Enable GSM and WCDMA only
Drag and drop the voicenv.bin 
from modem build
  \modem_proc\avs\vsd\vocsvc\eAMR_config\enable_g_enable_w_disable_tds
to target EFS Explorer 
  /nv/item_files/voice/
Reboot target.

5. Enable GSM and TDSCDMA only 
Drag and drop the voicenv.bin 
from modem build
  \modem_proc\avs\vsd\vocsvc\eAMR_config\enable_g_disable_w_enable_tds
to target EFS Explorer 
  /nv/item_files/voice/
Reboot target.

6. Enable WCDMA and TDSCDMA only 
Drag and drop the voicenv.bin 
from modem build
  \modem_proc\avs\vsd\vocsvc\eAMR_config\disable_g_enable_w_enable_tds
to target EFS Explorer 
  /nv/item_files/voice/
Reboot target.

7. Enable GSM, WCDMA and TDSCDMA
Drag and drop the voicenv.bin 
from modem build
  \modem_proc\avs\vsd\vocsvc\eAMR_config\enable_g_enable_w_enable_tds
to target EFS Explorer 
  /nv/item_files/voice/
Reboot target.

8. Disable GSM, WCDMA and TDSCDMA
Drag and drop the voicenv.bin 
from modem build
  \modem_proc\avs\vsd\vocsvc\eAMR_config\disable_g_disable_w_disable_tds
to target EFS Explorer 
  /nv/item_files/voice/
Reboot target.






