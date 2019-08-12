#! /bin/sh
#
#Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
#Qualcomm Technologies Proprietary and Confidential.

# qmi_ip   init.d script to start the Data Software's qmi_ip daemon

set -e

case "$1" in
  start)
        echo -n "Starting qmi_ip: "
        start-stop-daemon -S -b --exec /usr/bin/qmi_ip_multiclient /etc/qmi_ip_cfg.xml
        echo "done"
        ;;
  stop)
        echo -n "Stopping qmi_ip: "
        start-stop-daemon -K -n qmi_ip_multiclient
        echo "done"
        ;;
  restart)
        $0 stop
        $0 start
        ;;
  *)
        echo "Usage qmi_ip_multiclient { start | stop | restart}" >&2
        exit 1
        ;;
esac

exit 0
