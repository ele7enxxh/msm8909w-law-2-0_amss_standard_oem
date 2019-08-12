/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "logfile.h"
#include "QvopService.h"

#include <stdio.h>
#include <sys/stat.h>
#include <cutils/properties.h>

static char const* const QVOP_DIRECTORY = "/data/misc/qvop";
static char const* const QVOP_LOG_FILE = "/data/misc/qvop/qvop_log.txt";


using namespace android;


#include <android/log.h>

/**
 * @brief daemon for starting the voiceprint service
 * @details VoicePrint daemon that starts the voice print service at startup.
 *
 * @param argc
 * @param argv
 *
 * @return
 */
int main(int argc, char* argv[]) {

  logfile_init(true);
  logfile_print("%s: App build date: %s %s\n", QVOP_FN, __DATE__, __TIME__);

  char buf[PROPERTY_VALUE_MAX] = { };
  bool enable = property_get("persist.qvop", buf, "false") >= 0 &&
                strncmp(buf, "true", sizeof (buf)) == 0;
  if (!enable) {
    // voiceprint is not enabled, suspend
    logfile_print("%s: voiceprint not enabled, suspending\n", QVOP_FN);
    pause();
    return 0;
  }

  logfile_print("%s: voiceprint enabled, starting service\n", QVOP_FN);

  signal(SIGPIPE, SIG_IGN);

  // clear the log file
  FILE* fp = fopen(QVOP_LOG_FILE, "wb");
  if (fp) {
    fclose(fp);
  }


  QvopService::instantiate();
  ProcessState::self()->startThreadPool();
  IPCThreadState::self()->joinThreadPool();
  ALOGI("%s: exit\n", QVOP_FN);
  return 0;
}

