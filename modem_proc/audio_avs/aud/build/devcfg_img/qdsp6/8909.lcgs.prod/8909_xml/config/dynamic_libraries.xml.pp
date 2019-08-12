# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/aud/common/config/dynamic_libraries.xml"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/aud/common/config/dynamic_libraries.xml" 1
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 136 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/aud/common/config/dynamic_libraries.xml" 2
<!-- Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved. DESCRIPTION:
 Configuration of the dynamic modules to be loaded by the DSP. $Header: $ -->

<!-- NULL Driver does not require Dal Driver Interface APIs, since none of
 AVS HWD drivers uses DAL Device framework -->
 <!--
 Explanation of the fields:
 NumModules: Number of dynamic modules.

 All subsequent fields must end with _ followed by a sequence number.
 The sequence numbers should not have any gaps and must start with 1.

 URI: The full path of the shared object.
 TYPE: 0 = CAPI, 1 = APPI
 PRELOAD: 0 = FALSE, 1 = TRUE
 ID: Unique identifier used to create the module

 For CAPIs:
 GETSIZE: name of the getsize function
 CTOR: name of the ctor function

 For APPIs:
 GETSIZE: name of the getsize function
 INIT: name of the init function

 All strings should be declared in the global section and the variable names
 should be mentioned in the props sections. Please refer to the example data
 below.

  -->
<driver name="NULL">
 <!-- Example data
 <global_def>
  <var_seq name="uri_1" type=DALPROP_DATA_TYPE_STRING>
   /path/capi_mp3.so
  </var_seq>
  <var_seq name="getsize_1" type=DALPROP_DATA_TYPE_STRING>
   capi_mp3_getsize
  </var_seq>
  <var_seq name="ctor_1" type=DALPROP_DATA_TYPE_STRING>
   capi_mp3_ctor
  </var_seq>

  <var_seq name="uri_2" type=DALPROP_DATA_TYPE_STRING>
   /path/appi_gain.so
  </var_seq>
  <var_seq name="getsize_2" type=DALPROP_DATA_TYPE_STRING>
   appi_gain_getsize
  </var_seq>
  <var_seq name="init_2" type=DALPROP_DATA_TYPE_STRING>
   appi_gain_init
  </var_seq>

 </global_def>
 <device id="dynamic_modules">
  <props name="NumModules" type=DALPROP_ATTR_TYPE_UINT32>
   2
  </props>

  <props name="URI_1" type=DALPROP_ATTR_TYPE_STRING_PTR>
   uri_1
  </props>
  <props name="TYPE_1" type=DALPROP_ATTR_TYPE_UINT32>
   0
  </props>
  <props name="PRELOAD_1" type=DALPROP_ATTR_TYPE_UINT32>
   0
  </props>
  <props name="ID_1" type=DALPROP_ATTR_TYPE_UINT32>
   0x10BE9
  </props>
  <props name="GETSIZE_1" type=DALPROP_ATTR_TYPE_STRING_PTR>
   getsize_1
  </props>
  <props name="CTOR_1" type=DALPROP_ATTR_TYPE_STRING_PTR>
   ctor_1
  </props>

  <props name="URI_2" type=DALPROP_ATTR_TYPE_STRING_PTR>
   uri_2
  </props>
  <props name="TYPE_2" type=DALPROP_ATTR_TYPE_UINT32>
   1
  </props>
  <props name="PRELOAD_2" type=DALPROP_ATTR_TYPE_UINT32>
   1
  </props>
  <props name="ID_2" type=DALPROP_ATTR_TYPE_UINT32>
   0x10BFE
  </props>
  <props name="GETSIZE_2" type=DALPROP_ATTR_TYPE_STRING_PTR>
   getsize_2
  </props>
  <props name="INIT_2" type=DALPROP_ATTR_TYPE_STRING_PTR>
   init_2
  </props>
 </device>
 -->
 <global_def>
  <var_seq name="capi_v2_shared_object" type=DALPROP_DATA_TYPE_STRING>capi_v2_voice_wakeup.so</var_seq>
  <var_seq name="capi_v2_static_properties" type=DALPROP_DATA_TYPE_STRING>capi_v2_voicewakeup_v2_get_static_properties</var_seq>
  <var_seq name="capi_v2_init" type=DALPROP_DATA_TYPE_STRING>capi_v2_voicewakeup_v2_init</var_seq>
 </global_def>
 <device id="dynamic_modules">
  <props name="NumModules" type=DALPROP_ATTR_TYPE_UINT32>
   1
  </props>
  <props name="URI_1" type=DALPROP_ATTR_TYPE_STRING_PTR>
   capi_v2_shared_object
  </props>
  <props name="TYPE_1" type=DALPROP_ATTR_TYPE_UINT32>
   2
  </props>
  <props name="PRELOAD_1" type=DALPROP_ATTR_TYPE_UINT32>
   1
  </props>
  <props name="CAPIV2TYPE_1" type=DALPROP_ATTR_TYPE_UINT32>
   0
  </props>
  <props name="ID1_1" type=DALPROP_ATTR_TYPE_UINT32>
   0x12c16
  </props>
  <props name="ID2_1" type=DALPROP_ATTR_TYPE_UINT32>
   0
  </props>
  <props name="GETSTATICPROPERTIES_1" type=DALPROP_ATTR_TYPE_STRING_PTR>
   capi_v2_static_properties
  </props>
  <props name="INIT_1" type=DALPROP_ATTR_TYPE_STRING_PTR>
   capi_v2_init
  </props>
 </device>

</driver>
