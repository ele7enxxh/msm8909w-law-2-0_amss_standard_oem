/* sound_trigger_platform.c
 *
 * Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#define LOG_TAG "sound_trigger_platform"
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

#include <cutils/log.h>
#include <dlfcn.h>
#include <expat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sound/msmcal-hwdep.h>
#include "sound_trigger_platform.h"

/* macros used to verify all param id tags defined for vendor uuid
   in platform file */
#define PARAM_LOAD_SOUND_MODEL_BIT (1 << 0)
#define PARAM_UNLOAD_SOUND_MODEL_BIT (1 << 1)
#define PARAM_CONFIDENCE_LEVELS_BIT (1 << 2)
#define PARAM_OPERATION_MODE_BIT (1 << 3)
#define PARAM_ID_MANDATORY_BITS \
    (PARAM_LOAD_SOUND_MODEL_BIT | PARAM_UNLOAD_SOUND_MODEL_BIT | \
     PARAM_OPERATION_MODE_BIT)

typedef int  (*acdb_loader_init_v2_t)(const char *, const char *, int);
typedef void (*acdb_loader_deallocate_t)(void);
typedef void (*acdb_loader_send_listen_afe_cal_t)(int, int);
typedef int  (*acdb_loader_send_listen_lsm_cal_t)(int, int, int, int);
typedef int  (*acdb_loader_get_calibration_t)(char *, int, void *);

struct st_device_index
st_device_name_idx[ST_DEVICE_EXEC_MODE_MAX][ST_DEVICE_MAX] = {
   {
       {"DEVICE_HANDSET_APE_ACDB_ID", ST_DEVICE_HANDSET_MIC},
   },
   {
       {"DEVICE_HANDSET_CPE_ACDB_ID", ST_DEVICE_HANDSET_MIC},
   },
};

static const char * const
st_device_table[ST_DEVICE_EXEC_MODE_MAX][ST_DEVICE_MAX] = {
    {
        /* ADSP SVA devices */
        [ST_DEVICE_NONE] = "none",
        [ST_DEVICE_HANDSET_MIC] = "listen-ape-handset-mic",
    },
    {
        /* CPE SVA devices */
        [ST_DEVICE_NONE] = "none",
        [ST_DEVICE_HANDSET_MIC] = "listen-cpe-handset-mic",
    },
};

/* ACDB IDs for each device for both CDSP and ADSP */
static int acdb_device_table[ST_DEVICE_EXEC_MODE_MAX][ST_DEVICE_MAX] = {
    {
      [ST_DEVICE_NONE] = -1,
      [ST_DEVICE_HANDSET_MIC] = DEVICE_HANDSET_APE_ACDB_ID,
    },
    {
      [ST_DEVICE_NONE] = -1,
      [ST_DEVICE_HANDSET_MIC] = DEVICE_HANDSET_CPE_ACDB_ID,
    }
};

/* Qualcomm vendorUuid for SVA soundmodel */
static const sound_trigger_uuid_t qc_uuid =
    { 0x68ab2d40, 0xe860, 0x11e3, 0x95ef, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };

struct platform_data {
    int hwdep_fd;
    int prev_acdb_id;
    sound_trigger_device_t *stdev;
    void *acdb_handle;
    acdb_loader_send_listen_afe_cal_t acdb_send_afe_cal;
    acdb_loader_send_listen_lsm_cal_t acdb_send_lsm_cal;
    acdb_loader_get_calibration_t acdb_get_cal;
    acdb_loader_deallocate_t acdb_deinit;
    struct st_vendor_info *vendor_uuid_info;
    int param_tag_tracker;

    char codec_version[15];
};

static int platform_stdev_set_acdb_id(void *userdata __unused, const char* device, int acdb_id)
{
    int i, j;
    int ret = 0;
    int dev_idx = ST_DEVICE_NONE;

    if (device == NULL) {
       ALOGE("%s: device name is NULL", __func__);
       ret = -ENODEV;
       goto done;
    }

    for (i = 0; i < ST_DEVICE_EXEC_MODE_MAX; i++) {
        for (j = 0; j < ST_DEVICE_MAX; i++) {
           if(strcmp(st_device_name_idx[i][j].name, device) == 0)
               dev_idx = st_device_name_idx[i][j].index;
               break;
        }
        if (dev_idx != ST_DEVICE_NONE)
            break;
    }
    if (dev_idx == ST_DEVICE_NONE) {
       ALOGE("%s: Could not find index for device name = %s",
               __func__, device);
       ret = -ENODEV;
       goto done;
    }

    acdb_device_table[i][dev_idx] = acdb_id;

done:
    return ret;

}

static void platform_stdev_set_default_config(struct platform_data *platform)
{
    sound_trigger_device_t *stdev = platform->stdev;

    stdev->run_on_ape =  true;
    stdev->max_ape_sessions = 1;
    stdev->avail_ape_phrases = 1;
    stdev->avail_ape_users = 1;
    stdev->max_cpe_sessions = 1;
    stdev->avail_cpe_phrases  = 1;
    stdev->avail_cpe_users = 1;
    stdev->rx_conc_max_st_ses = UINT_MAX;
}

static void platform_stdev_set_config(void *userdata, const char* param, const char* value)
{
    struct platform_data *platform = userdata;
    sound_trigger_device_t *stdev;

    if (!platform) {
        ALOGE("%s: platform data NULL", __func__);
        return;
    }
    if (!platform->stdev) {
        ALOGE("%s: platform stdev data NULL", __func__);
        return;
    }

    stdev = platform->stdev;
    if (!strcmp(param, "execution_type")) {
        if(!strcmp(value, "CPE"))
            stdev->run_on_ape =  false;
        else if(!strcmp(value, "CPE_AND_APE"))
            stdev->transition_enabled = true;
    }
    else if (!strcmp(param, "max_ape_sessions")) {
        stdev->max_ape_sessions = atoi(value);
    }
    else if (!strcmp(param, "max_cpe_sessions")) {
        stdev->max_cpe_sessions = atoi(value);
    }
    else if (!strcmp(param, "max_cpe_phrases")) {
        stdev->avail_cpe_phrases = atoi(value);
    }
    else if (!strcmp(param, "max_cpe_users")) {
        stdev->avail_cpe_users = atoi(value);
    }
    else if (!strcmp(param, "max_ape_phrases")) {
        stdev->avail_ape_phrases = atoi(value);
    }
    else if (!strcmp(param, "max_ape_users")) {
        stdev->avail_ape_users = atoi(value);
    }
    else if (!strcmp(param, "rx_concurrency_disabled")) {
        stdev->rx_concurrency_disabled =
           (0 == strncasecmp(value, "true", 4))? true:false;
        ALOGD("%s:rx_concurrency_disabled = %d",
                 __func__, stdev->rx_concurrency_disabled);
    }
    else if (!strcmp(param, "rx_conc_max_st_ses")) {
        stdev->rx_conc_max_st_ses = atoi(value);
        ALOGD("%s:rx_conc_max_st_ses = %d",
                 __func__, stdev->rx_conc_max_st_ses);
    }
    else if (!strcmp(param, "enable_failure_detection")) {
        stdev->detect_failure =
           (0 == strncasecmp(value, "true", 4))? true:false;
    }
    else
        ALOGD("%s: unknown config param, ignoring..", __func__);
}

static int string_to_uuid(const char *str, sound_trigger_uuid_t *uuid)
{
    int tmp[10];

    if (str == NULL || uuid == NULL) {
        return -EINVAL;
    }

    if (sscanf(str, "%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x",
            tmp, tmp + 1, tmp + 2, tmp + 3, tmp + 4, tmp + 5, tmp + 6,
            tmp + 7, tmp+ 8, tmp+ 9) < 10) {
        return -EINVAL;
    }
    uuid->timeLow = (uint32_t)tmp[0];
    uuid->timeMid = (uint16_t)tmp[1];
    uuid->timeHiAndVersion = (uint16_t)tmp[2];
    uuid->clockSeq = (uint16_t)tmp[3];
    uuid->node[0] = (uint8_t)tmp[4];
    uuid->node[1] = (uint8_t)tmp[5];
    uuid->node[2] = (uint8_t)tmp[6];
    uuid->node[3] = (uint8_t)tmp[7];
    uuid->node[4] = (uint8_t)tmp[8];
    uuid->node[5] = (uint8_t)tmp[9];

    return 0;
}
static void platform_stdev_set_vendor_config
(
   void *userdata,
   const char *tag_name,
   const char** attr,
   bool is_new_uuid
)
{
    struct platform_data *platform = userdata;
    sound_trigger_device_t *stdev;
    struct st_vendor_info *vendor_uuid_info = NULL;

    if (!platform || !attr) {
        ALOGE("%s: NULL attr or platform ", __func__);
        return;
    }
    if (!platform->stdev) {
        ALOGE("%s: platform stdev data NULL", __func__);
        return;
    }
    stdev = platform->stdev;

    if (is_new_uuid) {
        if (strcmp(attr[0], "value")) {
            ALOGE("%s: 'value' not found! for vendor_uuid tag",
                  __func__);
            goto cleanup;
        }

        vendor_uuid_info = calloc(1, sizeof(struct st_vendor_info));
        if(!vendor_uuid_info) {
            ALOGE("%s: can't allocate vendor_uuid_info", __func__);
            goto cleanup;
        }
        if(string_to_uuid(attr[1], &vendor_uuid_info->uuid) < 0) {
            ALOGE("%s: string_to_uuid failed", __func__);
            goto cleanup;
        }
        if(attr[2] && !strcmp(attr[2], "app_type"))
           vendor_uuid_info->app_type = strtoul(attr[3], NULL, 16);

        /* if soundmodel library for ISV vendor uuid is mentioned, use it. If not
           ignore and continue sending the opaque data from HAL to DSP */
        if(attr[4] && !strcmp(attr[4], "library") && strcmp(attr[5], "none")) {
            ALOGV("%s: vendor library present %s", __func__, attr[5]);
            vendor_uuid_info->smlib_handle = dlopen(attr[5], RTLD_NOW);
            if (!vendor_uuid_info->smlib_handle) {
                ALOGE("%s: ERROR. dlopen failed for %s", __func__, attr[5]);
                goto cleanup;
            }
            vendor_uuid_info->generate_st_phrase_recognition_event =
                (smlib_generate_sound_trigger_phrase_recognition_event_t)
                dlsym(vendor_uuid_info->smlib_handle,
                      "generate_sound_trigger_phrase_recognition_event");

            if (!vendor_uuid_info->generate_st_phrase_recognition_event) {
                ALOGE("%s: dlsym error %s for generate_sound_trigger_phrase_recognition_event",
                      __func__, dlerror());
                goto cleanup;
            }

            vendor_uuid_info->generate_st_recognition_config_payload =
                (smlib_generate_sound_trigger_recognition_config_payload_t)
                dlsym(vendor_uuid_info->smlib_handle,
                      "generate_sound_trigger_recognition_config_payload");

            if (!vendor_uuid_info->generate_st_recognition_config_payload) {
                ALOGE("%s: dlsym error %s for generate_sound_trigger_recognition_config_payload",
                      __func__, dlerror());
                goto cleanup;
            }
        }
        vendor_uuid_info->kw_transfer_mode = RT_TRANSFER_MODE;
        vendor_uuid_info->kw_capture_format = PCM_RAW;
        vendor_uuid_info->kw_duration = DEFAULT_MAX_KEYWORD_DURATION_MS;
        vendor_uuid_info->client_capture_read_delay = DEFAULT_MAX_CLIENT_LAB_READ_DELAY_MS;
        platform->vendor_uuid_info = vendor_uuid_info;
        return;
    }

    if(!platform->vendor_uuid_info)
        return;

    vendor_uuid_info = platform->vendor_uuid_info;

    /* process the vendor_uuid elements */
    if (!strcmp(tag_name, "firmware_image") && !stdev->run_on_ape) {
        if (strcmp(attr[0], "name")) {
            ALOGE("%s: firmware_image not found! for vendor_uuid tag",
                  __func__);
            goto cleanup;
        }
        strlcpy(vendor_uuid_info->cpe_firmware_image,
                attr[1], sizeof(vendor_uuid_info->cpe_firmware_image));
        ALOGV("%s: firmware_image name is %s",
               __func__, vendor_uuid_info->cpe_firmware_image);
    } else if (!strcmp(tag_name, "capture_keyword")) {
        if (strcmp(attr[0], "format") || strcmp(attr[2], "transfer_mode")) {
            ALOGE("%s: capture_keyword info not found! for vendor_uuid tag",
                  __func__);
            goto cleanup;
        }
        if (!strcmp(attr[1], "ADPCM_packet")) {
            vendor_uuid_info->kw_capture_format |= ADPCM_CUSTOM_PACKET;
        }
        else if (!strcmp(attr[1], "ADPCM_raw")) {
            vendor_uuid_info->kw_capture_format |= ADPCM_RAW;
        }
        else if (!strcmp(attr[1], "PCM_packet")) {
            vendor_uuid_info->kw_capture_format |= PCM_CUSTOM_PACKET;
        }
        else if (!strcmp(attr[1], "PCM_raw")) {
            vendor_uuid_info->kw_capture_format |= PCM_RAW;
        }
        else {
            ALOGE("%s: Unknown catpure format for vendor_uuid tag", __func__);
            goto cleanup;
        }

        if (!strcmp(attr[3], "FTRT")) {
            vendor_uuid_info->kw_transfer_mode = FTRT_TRANSFER_MODE;
        }
        else if (!strcmp(attr[3], "RT")) {
            vendor_uuid_info->kw_transfer_mode = RT_TRANSFER_MODE;
        }
        else {
            ALOGE("%s: Unknown catpure mode for vendor_uuid tag", __func__);
            goto cleanup;
        }

        if (attr[4] && !strcmp(attr[4], "kw_duration")) {
            vendor_uuid_info->kw_duration = atoi(attr[5]);
        }
    } else if (!strcmp(tag_name, "param_type")) {
        if (strcmp(attr[0], "name") || strcmp(attr[2], "module_id") ||
            strcmp(attr[4], "param_id")) {
            ALOGE("%s: param_type info not found! for vendor_uuid tag",
                  __func__);
            goto cleanup;
        }
        if (!strcmp(attr[1], "load_sound_model")) {
            vendor_uuid_info->params[LOAD_SOUND_MODEL].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[LOAD_SOUND_MODEL].param_id =
                                                    strtoul(attr[5], NULL, 16);
            platform->param_tag_tracker |= PARAM_LOAD_SOUND_MODEL_BIT;
        } else if (!strcmp(attr[1], "unload_sound_model")) {
            vendor_uuid_info->params[UNLOAD_SOUND_MODEL].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[UNLOAD_SOUND_MODEL].param_id =
                                                    strtoul(attr[5], NULL, 16);
            platform->param_tag_tracker |= PARAM_UNLOAD_SOUND_MODEL_BIT;
        } else if (!strcmp(attr[1], "confidence_levels")) {
            vendor_uuid_info->params[CONFIDENCE_LEVELS].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[CONFIDENCE_LEVELS].param_id =
                                                    strtoul(attr[5], NULL, 16);
            platform->param_tag_tracker |= PARAM_CONFIDENCE_LEVELS_BIT;
        } else if (!strcmp(attr[1], "operation_mode")) {
            vendor_uuid_info->params[OPERATION_MODE].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[OPERATION_MODE].param_id =
                                                    strtoul(attr[5], NULL, 16);
            platform->param_tag_tracker |= PARAM_OPERATION_MODE_BIT;
        } else {
            ALOGD("%s: unknown config param, ignoring..", __func__);
        }
    } else if (!strcmp(tag_name, "ctrl")) {
        if (strcmp(attr[0], "name") || strcmp(attr[2], "value")) {
           ALOGE("%s: 'name' or 'value' not found! for ctrl tag", __func__);
           goto cleanup;
        }
        if (!strcmp(attr[1], "max_cpe_phrases")) {
            vendor_uuid_info->avail_cpe_phrases = atoi(attr[3]);
        } else if (!strcmp(attr[1], "max_cpe_users")) {
            vendor_uuid_info->avail_cpe_users = atoi(attr[3]);
        } else if (!strcmp(attr[1], "max_ape_phrases")) {
            vendor_uuid_info->avail_ape_phrases = atoi(attr[3]);
        } else if (!strcmp(attr[1], "max_ape_users")) {
            vendor_uuid_info->avail_ape_users = atoi(attr[3]);
        } else if (!strcmp(attr[1], "client_capture_read_delay")) {
            vendor_uuid_info->client_capture_read_delay = atoi(attr[3]);
        } else {
            ALOGD("%s: unknown config param, ignoring..", __func__);
        }
    } else {
        ALOGD("%s: unknown tag, ignoring..", __func__);
    }

    return;

cleanup:
    if(vendor_uuid_info) {
        if(vendor_uuid_info->smlib_handle)
            dlclose(vendor_uuid_info->smlib_handle);
        free(vendor_uuid_info);
        platform->vendor_uuid_info = NULL;
    }
}
static void start_tag(void *userdata, const XML_Char *tag_name,
                      const XML_Char **attr)
{
    int ret;
    struct platform_data *platform = (void *)userdata;

    if (!platform || !tag_name || !attr) {
        ALOGE("%s: NULL platform/tag_name/attr", __func__);
        return;
    }

    if (!strcmp(tag_name, "device")) {
        if (strcmp(attr[0], "name") || strcmp(attr[2], "value")) {
            ALOGE("%s: 'name' or 'value' not found! for device tag", __func__);
            goto done;
        }
        ret = platform_stdev_set_acdb_id(userdata, (const char *)attr[1], atoi((const char *)attr[3]));
        if (ret < 0) {
            ALOGE("%s: Device %s in platform xml not found, no ACDB ID set!",
                  __func__, attr[1]);
            goto done;
        }
    } else if (!strcmp(tag_name, "ctrl") && !platform->vendor_uuid_info) {
        if (strcmp(attr[0], "name") || strcmp(attr[2], "value")) {
            ALOGE("%s: 'name' or 'value' not found! for ctrl tag", __func__);
            goto done;
        }
        platform_stdev_set_config(userdata, (const char *)attr[1],
                                  (const char *)attr[3]);
    } else if (!strcmp(tag_name, "vendor_uuid") || platform->vendor_uuid_info) {
        platform_stdev_set_vendor_config(userdata, (const char *)tag_name,
                                         (const char **)attr, !platform->vendor_uuid_info);
    }

done:
    return;
}

static void end_tag(void *userdata, const XML_Char *tag_name)
{
    struct platform_data *platform = userdata;

    if (!platform || !tag_name) {
        ALOGE("%s: NULL tag or platform", __func__);
        return;
    }

    if (!strcmp(tag_name, "vendor_uuid")) {
        if (platform->vendor_uuid_info) {
            if (platform->param_tag_tracker & PARAM_ID_MANDATORY_BITS) {
                list_add_tail(&platform->stdev->vendor_uuid_list,
                              &platform->vendor_uuid_info->list_node);
            } else {
                if(platform->vendor_uuid_info->smlib_handle)
                    dlclose(platform->vendor_uuid_info->smlib_handle);
                free(platform->vendor_uuid_info);
                ALOGE("%s: param_type missing for vendor_uuid tag. Bits 0x%x",
                      __func__, platform->param_tag_tracker);
            }
        }
        platform->param_tag_tracker = 0;
        platform->vendor_uuid_info = NULL;
    }
}

static int platform_parse_info(struct platform_data *platform, const char *filename)
{
    XML_Parser      parser;
    FILE            *file;
    int             ret = 0;
    int             bytes_read;
    void            *buf;

    file = fopen(filename, "r");
    if (!file) {
        ALOGD("%s: Failed to open %s, using defaults", __func__, filename);
        ret = -ENODEV;
        goto done;
    }

    parser = XML_ParserCreate(NULL);
    if (!parser) {
        ALOGE("%s: Failed to create XML parser!", __func__);
        ret = -ENODEV;
        goto err_close_file;
    }

    XML_SetUserData(parser, platform);

    XML_SetElementHandler(parser, start_tag, end_tag);

    while (1) {
        buf = XML_GetBuffer(parser, BUF_SIZE);
        if (buf == NULL) {
            ALOGE("%s: XML_GetBuffer failed", __func__);
            ret = -ENOMEM;
            goto err_free_parser;
        }

        bytes_read = fread(buf, 1, BUF_SIZE, file);
        if (bytes_read < 0) {
            ALOGE("%s: fread failed, bytes read = %d", __func__, bytes_read);
             ret = bytes_read;
            goto err_free_parser;
        }

        if (XML_ParseBuffer(parser, bytes_read,
                            bytes_read == 0) == XML_STATUS_ERROR) {
            ALOGE("%s: XML_ParseBuffer failed, for %s",
                __func__, filename);
            ret = -EINVAL;
            goto err_free_parser;
        }

        if (bytes_read == 0)
            break;
    }

err_free_parser:
    XML_ParserFree(parser);
err_close_file:
    fclose(file);
done:
    return ret;
}

static int get_codec_version(struct platform_data *my_data,
                            const char *snd_card_name,
                            int card_num)
{
    char procfs_path[50];
    FILE *fp;

    /* query the codec type to handle FTRT based on codec capability
       currently applicable to tasha codec versions */
    if (strstr(snd_card_name, "tasha")) {
        snprintf(procfs_path, sizeof(procfs_path),
                 "/proc/asound/card%d/codecs/tasha/version", card_num);
        if ((fp = fopen(procfs_path, "r")) != NULL) {
            fgets(my_data->codec_version, sizeof(my_data->codec_version), fp);
            fclose(fp);
        } else {
            ALOGE("%s: ERROR. cannot open %s", __func__, procfs_path);
            return -ENOENT;
        }
    }
    ALOGD("%s: codec version %s", __func__, my_data->codec_version);

    return 0;
}

static void query_stdev_platform(sound_trigger_device_t *stdev,
                                 const char *snd_card_name,
                                 char *mixer_path_xml)
{
    if (strstr(snd_card_name, "msm8939-tapan")) {
        strlcpy(mixer_path_xml, MIXER_PATH_XML_WCD9306,
                        sizeof(MIXER_PATH_XML_WCD9306));
    } else if (strstr(snd_card_name, "msm8952-tomtom")||
                 strstr(snd_card_name, "msm8996-tomtom")) {
        strlcpy(mixer_path_xml, MIXER_PATH_XML_WCD9330,
                        sizeof(MIXER_PATH_XML_WCD9330));
    } else if (strstr(snd_card_name, "msm8976-tasha")||
                 strstr(snd_card_name, "msm8952-tasha") ||
                 strstr(snd_card_name, "msm8953-tasha") ||
                 strstr(snd_card_name, "msm8937-tasha")) {
        strlcpy(mixer_path_xml, MIXER_PATH_XML_WCD9335,
                        sizeof(MIXER_PATH_XML_WCD9335));
    } else {
        strlcpy(mixer_path_xml, MIXER_PATH_XML,
                         sizeof(MIXER_PATH_XML));
    }

    if ((strstr(snd_card_name, "msm8939") ||
        strstr(snd_card_name, "msm8909") ||
        strstr(snd_card_name, "msm8952") ||
        strstr(snd_card_name, "msm8x16")) &&
        !strstr(snd_card_name, "msm8976-tasha") &&
        !strstr(snd_card_name, "msm8952-tasha") &&
        !strstr(snd_card_name, "msm8953-tasha") &&
        !strstr(snd_card_name, "msm8937-tasha") &&
        !strstr(snd_card_name, "msm8952-tomtom") &&
        !strstr(snd_card_name, "msm8939-tomtom")) {
        stdev->sw_mad = true;
    }
}

void *platform_stdev_init(sound_trigger_device_t *stdev)
{
    int ret = 0, retry_num = 0;
    struct platform_data *my_data = NULL;
    const char *snd_card_name = NULL;
    acdb_loader_init_v2_t acdb_init;
    char mixer_path_xml[100];
    struct listnode *v_node;
    struct st_vendor_info* v_info;
    char dev_name[256];

    ALOGI("%s: Enter", __func__);
    my_data = calloc(1, sizeof(struct platform_data));

    if (!my_data || !stdev) {
        ALOGE("%s: ERROR. NULL param", __func__);
        if(my_data)
            free(my_data);
        return NULL;
    }
    my_data->stdev = stdev;
    list_init(&stdev->vendor_uuid_list);

    stdev->mixer = mixer_open(SOUND_CARD);
    while (!stdev->mixer && retry_num < MIXER_OPEN_MAX_NUM_RETRY) {
        usleep(RETRY_US);
        stdev->mixer = mixer_open(SOUND_CARD);
        retry_num++;
    }

    if (!stdev->mixer) {
        ALOGE("%s: ERROR. Unable to open the mixer, aborting", __func__);
        goto cleanup;
    }

    snd_card_name = mixer_get_name(stdev->mixer);

    query_stdev_platform(stdev, snd_card_name, mixer_path_xml);
    stdev->audio_route = audio_route_init(SOUND_CARD, mixer_path_xml);
    if (!stdev->audio_route) {
        ALOGE("%s: ERROR. Failed to init audio route controls, aborting.",
                __func__);
        goto cleanup;
    }

    my_data->acdb_handle = dlopen(LIB_ACDB_LOADER, RTLD_NOW);
    if (my_data->acdb_handle == NULL) {
        ALOGE("%s: ERROR. dlopen failed for %s", __func__, LIB_ACDB_LOADER);
        goto cleanup;
    }

    acdb_init = (acdb_loader_init_v2_t)dlsym(my_data->acdb_handle,
                                              "acdb_loader_init_v2");
    if (acdb_init == NULL) {
        ALOGE("%s: dlsym error %s for acdb_loader_init_v2", __func__, dlerror());
        goto cleanup;
    }

    my_data->acdb_deinit = (acdb_loader_deallocate_t)dlsym(my_data->acdb_handle,
                                           "acdb_loader_deallocate_ACDB");
    if (my_data->acdb_deinit == NULL) {
        ALOGE("%s: dlsym error %s for acdb_loader_deallocate_ACDB", __func__, dlerror());
        goto cleanup;
    }

    my_data->acdb_send_afe_cal = (acdb_loader_send_listen_afe_cal_t)
              dlsym(my_data->acdb_handle, "acdb_loader_send_listen_afe_cal");

    if (my_data->acdb_send_afe_cal == NULL) {
        ALOGE("%s: ERROR. dlsym Error:%s acdb_loader_send_listen_afe_cal", __func__,
               dlerror());
        goto cleanup;
    }

    my_data->acdb_send_lsm_cal = (acdb_loader_send_listen_lsm_cal_t)
              dlsym(my_data->acdb_handle, "acdb_loader_send_listen_lsm_cal");

    if (my_data->acdb_send_lsm_cal == NULL) {
        ALOGE("%s: ERROR. dlsym Error:%s acdb_loader_send_listen_lsm_cal", __func__,
               dlerror());
        goto cleanup;
    }

    my_data->acdb_get_cal = (acdb_loader_get_calibration_t)
              dlsym(my_data->acdb_handle, "acdb_loader_get_calibration");
    if (my_data->acdb_get_cal == NULL) {
        ALOGE("%s: ERROR. dlsym Error:%s acdb_loader_get_calibration", __func__,
               dlerror());
        goto cleanup;
    }

    platform_stdev_set_default_config(my_data);

    platform_parse_info(my_data, PLATFORM_PATH_XML);

    /* Using non topology solution still need QC smlib wrapper APIs */
    stdev->smlib_handle = dlopen(LIB_SM_WRAPPER, RTLD_NOW);
    if (stdev->smlib_handle) {
        stdev->generate_st_phrase_recognition_event =
            (smlib_generate_sound_trigger_phrase_recognition_event_t)dlsym(stdev->smlib_handle,
                                                  "generate_sound_trigger_phrase_recognition_event");
        if (!stdev->generate_st_phrase_recognition_event) {
            ALOGE("%s: dlsym error %s for generate_sound_trigger_phrase_recognition_event", __func__,
                  dlerror());
            goto cleanup;
        }

        stdev->generate_st_recognition_config_payload =
            (smlib_generate_sound_trigger_recognition_config_payload_t)dlsym(stdev->smlib_handle,
                                                  "generate_sound_trigger_recognition_config_payload");
        if (!stdev->generate_st_recognition_config_payload) {
            ALOGE("%s: dlsym error %s for generate_sound_trigger_recognition_config_payload",
                  __func__, dlerror());
            goto cleanup;
        }
    } else {
        ALOGW("%s: dlopen failed for %s", __func__, LIB_SM_WRAPPER);
    }

    /* Check if ISV vendor_uuid is present and force disable transitions */
    list_for_each(v_node, &stdev->vendor_uuid_list) {
        v_info = node_to_item(v_node, struct st_vendor_info, list_node);
        if(!memcmp(&v_info->uuid, &qc_uuid, sizeof(sound_trigger_uuid_t))) {
            v_info->is_qc_uuid = true;
        } else {
            ALOGV("%s: ISV uuid present, force disable transitions",
              __func__);
            stdev->transition_enabled = false;
        }
        if (!stdev->adpcm_dec_lib_handle &&
             (v_info->kw_capture_format & ADPCM)) {
            /* Load ADPCM decoder library */
            stdev->adpcm_dec_lib_handle = dlopen(LIB_ADPCM_DECODER, RTLD_NOW);
            if (!stdev->adpcm_dec_lib_handle) {
                ALOGE("%s: ERROR. dlopen failed for %s", __func__, LIB_ADPCM_DECODER);
                goto cleanup;
            }
            stdev->adpcm_dec_init =
                (g722_init_decoder_t)dlsym(stdev->adpcm_dec_lib_handle,
                                                      "g722_init_decoder");
            if (!stdev->adpcm_dec_init) {
                ALOGE("%s: dlsym error %s for g722_init_decoder", __func__,
                      dlerror());
                goto cleanup;
            }

            stdev->adpcm_dec_get_scratch_size =
                (g722_dec_get_total_byte_size_t)dlsym(stdev->adpcm_dec_lib_handle,
                                                      "g722_dec_get_total_byte_size");
            if (!stdev->adpcm_dec_get_scratch_size) {
                ALOGE("%s: dlsym error %s for g722_dec_get_total_byte_size", __func__,
                      dlerror());
                goto cleanup;
            }

            stdev->adpcm_dec_process =
                (g722_dec_process_t)dlsym(stdev->adpcm_dec_lib_handle,
                                                      "g722_dec_process");
            if (!stdev->adpcm_dec_process) {
                ALOGE("%s: dlsym error %s for g722_dec_process", __func__,
                      dlerror());
                goto cleanup;
            }
        }
        ALOGV("%s: vendor config: kcf=%d, ktm=%d, ckw=%d, cu=%d, akw=%d, au=%d",
              __func__, v_info->kw_capture_format, v_info->kw_transfer_mode,
              v_info->avail_cpe_phrases, v_info->avail_cpe_users,
              v_info->avail_ape_phrases, v_info->avail_ape_users);
    }

    ALOGI("%s: acdb_init: %s", __func__, snd_card_name);
    ret = acdb_init(snd_card_name, NULL, 0);
    if (ret) {
        ALOGE("%s: ERROR. acdb_loader_init_v2 failed status %d", __func__, ret);
        goto cleanup;
    }

    snprintf(dev_name, sizeof(dev_name), "/dev/snd/hwC%uD%u",
                               SOUND_CARD, WCD9XXX_CODEC_HWDEP_NODE);
    ALOGD("%s Opening device %s", __func__, dev_name);
    my_data->hwdep_fd = open(dev_name, O_WRONLY);
    if (my_data->hwdep_fd < 0) {
        ALOGE("%s: cannot open device '%s'", __func__, dev_name);
        my_data->acdb_deinit();
        goto cleanup;
    }
    my_data->prev_acdb_id = -1;

    ret = get_codec_version(my_data, snd_card_name, SOUND_CARD);
    if (ret)
        goto cleanup;

    return my_data;

cleanup:
    if(stdev->adpcm_dec_lib_handle)
        dlclose(stdev->adpcm_dec_lib_handle);

    if (my_data->acdb_handle)
        dlclose(my_data->acdb_handle);

    if (stdev->smlib_handle)
        dlclose(stdev->smlib_handle);

    if (stdev->audio_route)
        audio_route_free(stdev->audio_route);

    if (stdev->mixer)
        mixer_close(stdev->mixer);

    list_for_each(v_node, &stdev->vendor_uuid_list) {
        list_remove(v_node);
        v_info = node_to_item(v_node, struct st_vendor_info, list_node);
        if (v_info->smlib_handle)
            dlclose(v_info->smlib_handle);
        free(v_info);
    }
    if (my_data->hwdep_fd >= 0) {
        close(my_data->hwdep_fd);
        my_data->hwdep_fd = -1;
    }

    free(my_data);
    return NULL;
}

void platform_stdev_deinit(void *platform)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    struct listnode *v_node;
    struct st_vendor_info* v_info;

    ALOGI("%s: Enter", __func__);
    if (my_data) {
        list_for_each(v_node, &my_data->stdev->vendor_uuid_list) {
            list_remove(v_node);
            v_info = node_to_item(v_node, struct st_vendor_info, list_node);
            if (v_info->smlib_handle)
                dlclose(v_info->smlib_handle);
            free(v_info);
        }
        my_data->acdb_deinit();
        dlclose(my_data->acdb_handle);
        if (my_data->stdev->smlib_handle)
            dlclose(my_data->stdev->smlib_handle);
        if (my_data->stdev->adpcm_dec_lib_handle)
            dlclose(my_data->stdev->adpcm_dec_lib_handle);
        audio_route_free(my_data->stdev->audio_route);
        mixer_close(my_data->stdev->mixer);
        if (my_data->hwdep_fd >= 0) {
            close(my_data->hwdep_fd);
            my_data->hwdep_fd = -1;
        }
        free(my_data);
    }
}

struct st_vendor_info* platform_stdev_get_vendor_info
(
   void *platform,
   sound_trigger_uuid_t *uuid,
   bool *is_qc_uuid
)
{
    struct listnode *v_node;
    struct st_vendor_info* v_info;
    struct platform_data *my_data;
    sound_trigger_device_t *stdev;

    if (!platform || !is_qc_uuid || !uuid) {
        ALOGE("%s: NULL inputs", __func__);
        return NULL;
    }
    my_data = (struct platform_data *)platform;
    if (!my_data->stdev) {
        ALOGE("%s: platform stdev data is NULL", __func__);
        return NULL;
    }
    stdev = my_data->stdev;
    *is_qc_uuid = false;

    list_for_each(v_node, &stdev->vendor_uuid_list) {
        v_info = node_to_item(v_node, struct st_vendor_info, list_node);
        if(!memcmp(&v_info->uuid, uuid, sizeof(sound_trigger_uuid_t))) {
            ALOGV("%s: Matched uuid", __func__);
            return v_info;
        }
    }

    if (!memcmp(&qc_uuid, uuid, sizeof(sound_trigger_uuid_t)))
        *is_qc_uuid = true;

    return NULL;
}

int platform_stdev_get_device_name
(
   void *platform,
   st_exec_mode_t exec_mode,
   st_device_t st_device,
   char *device_name
)
{
    struct platform_data *my_data = (struct platform_data *)platform;

    if ((st_device >= ST_DEVICE_MIN && st_device < ST_DEVICE_MAX) &&
        (exec_mode > ST_DEVICE_EXEC_MODE_NONE && exec_mode < ST_DEVICE_EXEC_MODE_MAX)) {
        strlcpy(device_name, st_device_table[exec_mode][st_device], DEVICE_NAME_MAX_SIZE);

        if (strstr(my_data->codec_version, "WCD9335_1_0") ||
            strstr(my_data->codec_version, "WCD9335_1_1")) {
            strlcat(device_name, " low-speed-intf", DEVICE_NAME_MAX_SIZE);
        }
    } else {
        strlcpy(device_name, "", DEVICE_NAME_MAX_SIZE);
        return -EINVAL;
    }
    return 0;
}

int platform_stdev_get_device
(
   void *platform,
   audio_devices_t device
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    st_device_t st_device = ST_DEVICE_NONE;

    if (device == AUDIO_DEVICE_NONE)
        ALOGV("%s: device none",__func__);

    st_device = ST_DEVICE_HANDSET_MIC;//TODO

    return st_device;
}

int platform_stdev_send_hwmad_cal
(
    void *platform,
    int acdb_id
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    int ret = 0;
    struct wcdcal_ioctl_buffer codec_buffer;
    struct hwdep_cal_param_data calib;
    sound_trigger_device_t *stdev = my_data->stdev;
    int hwdep_fd = my_data->hwdep_fd;

    if ((stdev->sw_mad) || (my_data->prev_acdb_id == acdb_id)) {
        ALOGD("%s: previous acdb_id %d new acdb_id %d, sw_mad %d return",
              __func__, my_data->prev_acdb_id, acdb_id, stdev->sw_mad);
        return 0;
    }

    calib.acdb_id = acdb_id;
    calib.get_size = 1;
    ret = my_data->acdb_get_cal("mad_cal", sizeof(struct hwdep_cal_param_data),
                                                            &calib);
    if (ret < 0) {
        ALOGE("%s: get_calibration to get size failed", __func__);
        return ret;
    }

    calib.get_size = 0;
    calib.buff = malloc(calib.buff_size);
    if (!calib.buff) {
        ALOGE("%s: malloc calib of buff size %d failed",
                  __func__, calib.buff_size);
        return -ENOMEM;
    }

    ret = my_data->acdb_get_cal("mad_cal", sizeof(struct hwdep_cal_param_data),
                                                            &calib);
    if (ret < 0) {
        ALOGE("%s: get_calibration to get size failed", __func__);
        free(calib.buff);
        return ret;
    }

    codec_buffer.buffer = calib.buff;
    codec_buffer.size = calib.data_size;
    codec_buffer.cal_type = WCD9XXX_MAD_CAL;
    ret = ioctl(hwdep_fd, SNDRV_CTL_IOCTL_HWDEP_CAL_TYPE, &codec_buffer);
    if (ret < 0) {
        ALOGE("%s: failed to call ioctl err=%d",__func__, errno);
    } else {
        my_data->prev_acdb_id = acdb_id;
        ALOGD("%s hwmad cal sent for acdb_id (%d)", __func__, acdb_id);
    }

    free(calib.buff);
    return ret;
}

int platform_stdev_send_calibration
(
    void *platform,
    audio_devices_t device,
    st_exec_mode_t exec_mode,
    int app_id,
    bool use_topology,
    st_cal_type type
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    int acdb_id;
    st_device_t st_device;
    int status = 0;
    int hw_type;

    st_device = platform_stdev_get_device(platform, device);
    if (st_device == ST_DEVICE_NONE) {
        ALOGE("%s: Could not find valid device",__func__);
        return -EINVAL;
    }

    acdb_id = acdb_device_table[exec_mode][st_device];
    if (acdb_id < 0) {
        ALOGE("%s: Could not find acdb id for device(%d)",
              __func__, st_device);
        return -EINVAL;
    }
    hw_type = (exec_mode == ST_DEVICE_EXEC_MODE_CPE) ? 0 : 1;

    switch (type) {
    case ST_SESSION_CAL:
        if (my_data->acdb_send_lsm_cal) {
            ALOGD("%s: sending lsm calibration for device(%d) acdb_id(%d)",
                                             __func__, st_device, acdb_id);
            /* ACDB modes: topology=1, non-topology=0 */
            status = my_data->acdb_send_lsm_cal(acdb_id, app_id,
                                                (use_topology == true),
                                                hw_type);
        }
        break;

    case ST_DEVICE_CAL:
        if (platform_stdev_send_hwmad_cal(platform, acdb_id) < 0)
            return -EINVAL;

        if (my_data->acdb_send_afe_cal) {
            ALOGD("%s: sending afe calibration for device(%d) acdb_id(%d)",
                                             __func__, st_device, acdb_id);
            my_data->acdb_send_afe_cal(acdb_id, hw_type);
        }
        break;

    default:
        ALOGE("%s: invalid cal type %d",__func__,type);
        return -EINVAL;
    }

    return status;
}

bool platform_stdev_check_and_update_concurrency
(
   void *platform,
   audio_event_type_t event_type,
   unsigned int num_sessions
)
{
    struct platform_data *my_data;
    sound_trigger_device_t *stdev;
    bool concurrency_ses_allowed = true;

    if (!platform) {
        ALOGE("%s: NULL platform", __func__);
        return false;
    }
    my_data = (struct platform_data *)platform;
    if (!my_data->stdev) {
        ALOGE("%s: platform stdev data is NULL", __func__);
        return false;
    }
    stdev = my_data->stdev;

    switch (event_type) {
    case AUDIO_EVENT_CAPTURE_DEVICE_ACTIVE:
        stdev->tx_concurrency_active++;
        break;
    case AUDIO_EVENT_CAPTURE_DEVICE_INACTIVE:
        if (stdev->tx_concurrency_active > 0)
            stdev->tx_concurrency_active--;
        break;
    case AUDIO_EVENT_PLAYBACK_STREAM_ACTIVE:
            stdev->rx_concurrency_active++;
        break;
    case AUDIO_EVENT_PLAYBACK_STREAM_INACTIVE:
        if (stdev->rx_concurrency_active > 0)
            stdev->rx_concurrency_active--;
        break;
    default:
        break;
    }
    if ((stdev->tx_concurrency_active > 0) ||
        (stdev->rx_concurrency_disabled &&
          stdev->rx_concurrency_active > 0 &&
          num_sessions > stdev->rx_conc_max_st_ses)) {
        concurrency_ses_allowed = false;
    }

    ALOGD("%s: concurrency active %d, tx %d, rx %d, concurrency session_allowed %d",
          __func__, stdev->audio_concurrency_active, stdev->tx_concurrency_active,
          stdev->rx_concurrency_active, concurrency_ses_allowed);
    return concurrency_ses_allowed;
}

bool platform_stdev_is_session_allowed
(
    void *platform,
    unsigned int num_sessions,
    bool sound_model_loaded
)
{
    struct platform_data *my_data;
    sound_trigger_device_t *stdev;
    bool session_allowed = true;

    if (!platform) {
        ALOGE("%s: NULL platform", __func__);
        return false;
    }
    my_data = (struct platform_data *)platform;
    if (!my_data->stdev) {
        ALOGE("%s: platform stdev data is NULL", __func__);
        return false;
    }
    stdev = my_data->stdev;

    if (!stdev->sw_mad) {
        /* hw_mad case only applicable only when sound_model is loaded */
        if (sound_model_loaded) {
            if (stdev->tx_concurrency_active > 0)
                session_allowed = false;
            stdev->audio_concurrency_active = session_allowed ? false: true;
        }
    } else {
        /* sw_mad case applicable only before sound_model is loaded */
        /* num_sessions does not reflect current session yet */
        if (!sound_model_loaded) {
            if ((stdev->tx_concurrency_active > 0) ||
                (stdev->rx_concurrency_disabled &&
                stdev->rx_concurrency_active > 0 &&
                (num_sessions + 1) > stdev->rx_conc_max_st_ses)) {
                session_allowed = false;
            } else {
                stdev->audio_concurrency_active = false;
            }
        }
    }

    ALOGD("%s: stdev->audio_concurrency_active %d session_allowed %d", __func__,
          stdev->audio_concurrency_active, session_allowed);
    return session_allowed;
}

void platform_stdev_check_and_update_pcm_config
(
   void *platform,
   struct pcm_config *config
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    const char *snd_card_name;

    snd_card_name = mixer_get_name(my_data->stdev->mixer);
    if (strstr(snd_card_name, "tomtom") ||
        strstr(my_data->codec_version, "WCD9335_1_0") ||
        strstr(my_data->codec_version, "WCD9335_1_1")) {
        /* tomtom, Tasha1.0, Tasha1.1 support max 16KHz/24bit bandwidth */
        config->rate = SOUND_TRIGGER_SAMPLING_RATE;
        if (config->format != PCM_FORMAT_S16_LE)
            config->format = PCM_FORMAT_S24_LE;
    } else {
        /* Other codecs configured at ftrt 384Khz/32bit */
        config->rate = SOUND_TRIGGER_SAMPLING_RATE_384000;
        config->format = PCM_FORMAT_S32_LE;
    }
}

int platform_stdev_connect_mad
(
   void *platform,
   bool is_ape
)
{
    /* This mixer control is only valid for CPE supported codec */
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    int status = 0;
    struct mixer_ctl *ctl = NULL;
    const char *mixer_ctl_name = "MAD_SEL MUX";

    if (stdev->sw_mad)
        return 0;

    ctl = mixer_get_ctl_by_name(stdev->mixer, mixer_ctl_name);
    if (!ctl) {
        ALOGE("%s: ERROR. Could not get ctl for mixer cmd - %s",
        __func__, mixer_ctl_name);
        return -EINVAL;
    }
    if(is_ape)
        status = mixer_ctl_set_enum_by_string(ctl, "MSM");
    else
        status = mixer_ctl_set_enum_by_string(ctl, "SPE");

    if (status)
        ALOGE("%s: ERROR. Mixer ctl set failed", __func__);

    return status;
}

