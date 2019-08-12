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
  printf("app1 main started\n");

  cbdrv_init();

  cbdrv_register_callback(mycallback, "app1");

  cbdrv_notify_callbacks(10);

  qurt_thread_stop();

  return 0;
}

