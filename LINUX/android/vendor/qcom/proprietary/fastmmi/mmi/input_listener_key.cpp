/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "input_listener.h"
#include "mmi.h"

bool input_listener_key::dispatch_event(input_event ev) {

    layout *lay = this->get_lay();
    char btn_name[64] = { 0 };

    __u16 type = ev.type;
    __u16 code = ev.code;
    __u32 value = ev.value;
    mod_ev_t modev;

    modev.mod = this->get_module();

    int down = ! !value;

    if(type == EV_KEY) {
        switch (code) {
        case KEY_BACK:
            strlcpy(btn_name, KEY_STR_BACK, sizeof(btn_name));
            break;

        case KEY_HOMEPAGE:
            ev.code = KEY_HOME; //change the code to KEY_HOMEPAGE
        case KEY_HOME:
            strlcpy(btn_name, KEY_STR_HOME, sizeof(btn_name));
            break;

        case KEY_MENU:
            strlcpy(btn_name, KEY_STR_MENU, sizeof(btn_name));
            break;

        case KEY_VOLUMEUP:
            strlcpy(btn_name, KEY_STR_VOLUMEUP, sizeof(btn_name));
            break;

        case KEY_VOLUMEDOWN:
            strlcpy(btn_name, KEY_STR_VOLUMEDOWN, sizeof(btn_name));
            break;

        case KEY_CAMERA_SNAPSHOT:
            strlcpy(btn_name, KEY_STR_SNAPSHOT, sizeof(btn_name));
            break;

        case KEY_POWER:
            strlcpy(btn_name, KEY_STR_POWER, sizeof(btn_name));
            break;

        case BTN_1:
            ev.code = KEY_MEDIA;    //change the code to KEY_MEDIA
        case KEY_MEDIA:        //Hangup key
            strlcpy(btn_name, KEY_STR_HANGUP, sizeof(btn_name));
            break;

        default:
            break;
        }
    }
    if(type == EV_SW) {
        switch (code) {
        case SW_HEADPHONE_INSERT:
            strlcpy(btn_name, KEY_STR_HEADPHONE, sizeof(btn_name));
            break;
        case SW_MICROPHONE_INSERT:
            strlcpy(btn_name, KEY_STR_MICROPHONE, sizeof(btn_name));
            break;
        default:
            break;
        }
    }
    button *btn = lay->find_button_by_name(btn_name);

    if(btn != NULL) {
        if(down) {
            MMI_ALOGI("button(%s) press down, code=%d", btn->get_name(), code);
            btn->set_color(255, 0, 0, 125);
            cb_t cb = this->get_cb();

            modev.ev = &ev;
            if(cb != NULL)
                cb(&modev);
        } else {
            MMI_ALOGI("button(%s) release, code=%d", btn->get_name(), code);
            btn->set_color(255, 0, 0, 255);
        }
        invalidate();
    } else {
        MMI_ALOGW("Not find button(%s) in layout(%s)", btn_name, lay->get_layout_path());
    }

    if(code == KEY_BACK)
        return false;

    return true;
}
