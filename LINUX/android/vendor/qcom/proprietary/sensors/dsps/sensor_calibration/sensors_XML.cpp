/*--------------------------------------------------------------------------
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------*/

#include <sensors_extension.h>
#include "sensors_XML.h"
#include <cutils/log.h>
#include "unistd.h"

#define SENSOR_XML_ROOT_ELEMENT "sensors"

ANDROID_SINGLETON_STATIC_INSTANCE(sensors_XML);
const static char *filepath[] = {
    "/persist/sensors_calibration_params.xml",
    "/data/sensors_calibration_params.xml"
};

const char *sensor_param[] = {"offset_x", "offset_y", "offset_z", "threshold_h", "threshold_l", "bias"};
const char *cal_state[] = {"static","dynamic"};
sensors_XML :: sensors_XML()
    : mdoc(NULL)
{
}

int sensors_XML :: write_sensors_params(struct sensor_t *sensor, struct cal_result_t *cal_result, int state)
{
    xmlNodePtr rootNode, curNode, newNode;
    xmlAttrPtr value;
    bool newcreate = false;
    char string[33];
    int fnum = 0;
    int i = 0, j, MAX = 0;

    if (state < CAL_STATIC || state > CAL_DYNAMIC) {
        ALOGE("state error\n");
        return -1;
    }
    if (cal_result == NULL) {
        ALOGE("Null pointer parameter\n");
        return -1;
    }

    if (state == 1)
        fnum = 1;

    if (!access(filepath[fnum], F_OK)) {
        if (!access(filepath[fnum], W_OK)) {
            mdoc = xmlReadFile(filepath[fnum], "UTF-8" , XML_PARSE_NOBLANKS);
            if (mdoc == NULL) {
                ALOGE("read calibration file error\n");
                return -EINVAL;
            }
        } else {
            ALOGE("No permission write file %s\n", filepath[fnum]);
            return -1;
        }
    } else {
        mdoc = xmlNewDoc(BAD_CAST "1.0");
        if (mdoc == NULL) {
            ALOGE("create sensor calibration file error\n");
            return -EINVAL;
        }
        newcreate = true;
    }

    if(newcreate) {
        rootNode = xmlNewNode(NULL, BAD_CAST SENSOR_XML_ROOT_ELEMENT);
        xmlDocSetRootElement(mdoc, rootNode);
        curNode = xmlNewNode(NULL, BAD_CAST "sensor");
        xmlAddChild(rootNode, curNode);
        xmlNewProp(curNode, BAD_CAST "name", BAD_CAST sensor->name);
        xmlNewProp(curNode, BAD_CAST "state", BAD_CAST cal_state[state]);
    } else {
        rootNode = xmlDocGetRootElement(mdoc);
        if (rootNode == NULL) {
            ALOGE("empty document\n");
            xmlFreeDoc(mdoc);
        }

        if (xmlStrcmp(rootNode->name, BAD_CAST SENSOR_XML_ROOT_ELEMENT)) {
            ALOGE("root node != sensors\n");
            xmlFreeDoc(mdoc);
        }
        curNode = rootNode->xmlChildrenNode;
        while(curNode != NULL) {
            if (!xmlStrcmp(xmlGetProp(curNode, BAD_CAST "name"), BAD_CAST sensor->name) &&
                !xmlStrcmp(xmlGetProp(curNode, BAD_CAST "state"), BAD_CAST cal_state[state]))
                break;
            curNode = curNode->next;
        }
    }
    switch(sensor->type) {
        case SENSOR_TYPE_ACCELEROMETER:
        case SENSOR_TYPE_GYROSCOPE:
            i = 0;
            MAX = 3;
            break;
        case SENSOR_TYPE_PROXIMITY:
            i = 3;
            MAX = 6;
            break;
        case SENSOR_TYPE_LIGHT:
        case SENSOR_TYPE_MAGNETIC_FIELD:
        case SENSOR_TYPE_PRESSURE:
        case SENSOR_TYPE_TEMPERATURE:
        default:
            break;
    }
    if (newcreate) {
        for(j = 0; i < MAX; i++, j++) {
            snprintf(string, sizeof(string), "%d", cal_result->offset[j]);
            xmlNewProp(curNode, BAD_CAST sensor_param[i], BAD_CAST string);
        }
    } else {
        if(curNode == NULL) {
            curNode = xmlNewNode(NULL, BAD_CAST "sensor");
            xmlAddChild(rootNode, curNode);
            value = xmlNewProp(curNode, BAD_CAST "name", BAD_CAST sensor->name);
            if (value == NULL) {
                ALOGE("\nname is NULL\n");
            }
            value = xmlNewProp(curNode, BAD_CAST "state", BAD_CAST cal_state[state]);
            if (value == NULL) {
                ALOGE("state is NULL\n");
                xmlFreeDoc(mdoc);
                return -1;
            }
            for(j = 0; i < MAX; i++, j++) {
                snprintf(string, sizeof(string), "%d", cal_result->offset[j]);
                xmlNewProp(curNode, BAD_CAST sensor_param[i], BAD_CAST string);
            }
        } else {
            for(j = 0; i < MAX; i++, j++) {
                snprintf(string, sizeof(string), "%d", cal_result->offset[j]);
                xmlSetProp(curNode, BAD_CAST sensor_param[i], BAD_CAST string);
            }
        }
    }
    if (xmlSaveFormatFileEnc(filepath[fnum], mdoc, "UTF-8", 1) == -1) {
        ALOGE("save %s failed %s\n", filepath[fnum], strerror(errno));
        xmlFreeDoc(mdoc);
        return -1;
    }
    xmlFreeDoc(mdoc);
    return 0;
}

int sensors_XML :: read_sensors_params(struct sensor_t *sensor, struct cal_result_t *cal_result, int state)
{
    xmlNodePtr rootNode, curNode;
    int i = 0, j, MAX = 0;

    if (state < CAL_STATIC || state > CAL_DYNAMIC) {
        ALOGE("state error\n");
        return -1;
    }
    if (cal_result == NULL) {
        ALOGE("Null pointer parameter\n");
        return -1;
    }
    if(!access(filepath[1], R_OK)) {
        mdoc = xmlReadFile(filepath[1], "UTF-8" , XML_PARSE_RECOVER);
    } else if (!access(filepath[0], F_OK)){
        char buf[200];
        snprintf(buf, sizeof(buf), "cp %s %s", filepath[0], filepath[1]);
        system(buf);
        if (!access(filepath[1], R_OK)) {
            mdoc = xmlReadFile(filepath[1], "UTF-8" , XML_PARSE_RECOVER);
        } else {
            ALOGE("file can't read\n");
            return -1;
        }
    } else {
        ALOGE("file can't read\n");
        return -1;
    }
    rootNode = xmlDocGetRootElement(mdoc);
    if (rootNode == NULL) {
        ALOGE("empty document\n");
        xmlFreeDoc(mdoc);
        return -1;
    }

    if (xmlStrcmp(rootNode->name, BAD_CAST SENSOR_XML_ROOT_ELEMENT)) {
        ALOGE("root node != sensors\n");
        xmlFreeDoc(mdoc);
        return -1;
    }
    curNode = rootNode->xmlChildrenNode;
    while(curNode != NULL) {
        if (!xmlStrcmp(xmlGetProp(curNode, BAD_CAST "name"), BAD_CAST sensor->name) &&
            !xmlStrcmp(xmlGetProp(curNode, BAD_CAST "state"), BAD_CAST cal_state[state]))
            break;
            curNode = curNode->next;
    }
    switch(sensor->type) {
        case SENSOR_TYPE_ACCELEROMETER:
        case SENSOR_TYPE_GYROSCOPE:
            i = 0;
            MAX = 3;
            break;
        case SENSOR_TYPE_PROXIMITY:
            i = 3;
            MAX = 6;
            break;
        case SENSOR_TYPE_LIGHT:
        case SENSOR_TYPE_MAGNETIC_FIELD:
        case SENSOR_TYPE_PRESSURE:
        case SENSOR_TYPE_TEMPERATURE:
        default:
            break;
    }
    if (curNode != NULL) {
        xmlChar* value;
        for(j = 0; i < MAX; ++i, ++j) {
            value = xmlGetProp(curNode, BAD_CAST sensor_param[i]);
            cal_result->offset[j] = atoi((char*)value);
        }
    } else {
        for(j = 0; j < 3; ++j) {
            cal_result->offset[j] = 0;
        }
        ALOGE("The sensor calibrate parameters is not found\n");
    }
    xmlFreeDoc(mdoc);
    return 0;
}

int sensors_XML :: sensors_calibrate_reset()
{
    int i;
    for(i=0; i < 2; ++i) {
        if (access(filepath[i], F_OK)) {
            ALOGE("file is not exits\n");
            return -1;
        }
        if (remove(filepath[i])) {
            ALOGE("reset calibrate error\n");
            return -1;
        }
    }
    return 0;
}

int sensors_XML :: sensors_rm_file()
{

        if (access(filepath[1], F_OK)) {
            return 0;
        }
        if (remove(filepath[1])) {
            ALOGE("reset calibrate error\n");
            return -1;
        }
        return 0;
}
