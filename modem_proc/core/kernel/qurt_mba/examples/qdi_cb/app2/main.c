#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qurt_mutex.h"
#include "qurt_thread.h"
#include "cbdrv.h"

void mycallback(void *arg, int value)
{
   printf("Got callback -- %s, value %d\n",
          (char *)arg, value);
}

int main(int argc, char **argv)
{
  printf("app2 main started\n");

  cbdrv_init();

  cbdrv_register_callback(mycallback, "app2");

  cbdrv_notify_callbacks(20);

  qurt_thread_stop();

  return 0;
}

