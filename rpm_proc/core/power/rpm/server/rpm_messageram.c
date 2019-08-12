#include <stdint.h>
#include <string.h>
#include "CoreVerify.h"
#include "err.h"
#include "rpm.h"
#include "railway.h"
#include "rpm_messageram.h"
#include "time_service.h"

#define ARRAY_SIZE(a)                   (sizeof(a)/sizeof((a)[0]))

//message ram master mapping
typedef struct
{
  //0 = apps, 1 = modem, 2 = riva, 3 = tz, 4 = qdsp,
  message_ram_master_t master[5];
} message_ram_s;

#define message_ram (*((message_ram_s*)RPM_MSG_RAM_BASE))

//data regarding message ram block allocation
static struct message_ram_data_s
{
  //0 = apps, 1 = modem,  2 = riva, 3 = tz, 4 = qdsp
  message_ram_master_data_t master_data[RPM_MASTER_MAX_COUNT];
} message_ram_data;

typedef struct
{
  unsigned mx_microvolts;
  unsigned cx_microvolts;
} message_ram_rail_info_s;

#define RPM_MSG_RAM_RAIL_INFO_SIZE      0x400   // 1KB
static message_ram_rail_info_s *message_ram_rail_info = ((message_ram_rail_info_s*)(RPM_MSG_RAM_BASE + RPM_MSG_RAM_BASE_SIZE - RPM_MSG_RAM_RAIL_INFO_SIZE));
static int mx_rail_id, cx_rail_id;

void * message_ram_malloc(smem_host_type master_id, uint32_t size)
{
  char * message_ram_address = NULL;
  uint8_t master_num;
  message_ram_master_t *space;
  message_ram_master_data_t *used;

  switch(master_id)
  {
    case SMEM_APPS:
      master_num = 0;
      break;
    case SMEM_MODEM:
      master_num = 1;
      break;
    case SMEM_RIVA:
      master_num = 2;
      break;
    case SMEM_TZ:
      master_num = 3;
      break;
    case SMEM_ADSP:
      master_num = 4;
      break;
    default:
      /* invalid input */
      CORE_VERIFY(0);
      break;
  }
/* During Smem Init page select will be configured to access Shared memory region.
 * reconfugre page select to acess messagram location
 * */
  space = &message_ram.master[master_num];
  used = &message_ram_data.master_data[master_num];

  // 4-byte align the size
  size = (size + 3) & ~3;

  if(size <= (sizeof(space->small_alloc_space) - used->small_alloc_used))
  {
    if (used->small_alloc_used == 0)
    {
      memset(&(space->vmpm), 0, sizeof(vmpm_data_t));
      memset (&space->master_stats, 0, sizeof (space->master_stats));
    }
    // we can allocate from small size
    message_ram_address = ((char *)space->small_alloc_space) + used->small_alloc_used;
    used->small_alloc_used += size;
    memset((void *)message_ram_address, 0, size);
    return message_ram_address;
  }
  else if((!used->large_alloc_used) && (size <= sizeof(space->large_alloc_space)))
  {
    used->large_alloc_used = 1;
    return space->large_alloc_space;
  }

  ERR_FATAL("Out of message RAM for master %d; cannot allocate %d bytes.", master_id, size, 0);
}

vmpm_data_t *message_ram_vmpm(vmpm_masters master)
{
  CORE_VERIFY(master <= ARRAY_SIZE(message_ram.master));
  return &message_ram.master[master].vmpm;
}

#define MBA_DONE_COOKIE 0x52414A55
boolean message_ram_vmpm_isMBADone(void)
{
  vmpm_data_t *vmpm = message_ram_vmpm(VMPM_MODEM);
  if ((vmpm != NULL) && (vmpm->mba_done == MBA_DONE_COOKIE))
  {
    return TRUE;
  }

  return FALSE;
}

rpm_master_stats_t *
message_ram_master_stats (unsigned ee)
{
  CORE_VERIFY(ee < ARRAY_SIZE(message_ram.master));
  return &message_ram.master[ee].master_stats;
}

void message_ram_update_master_xo_stats(unsigned ee, int vote)
{
  uint64 now = time_service_now();
  rpm_master_stats_t *stats = message_ram_master_stats(ee);

  if(vote)
  {
      uint64 last_entered_at = stats->xo_last_entered_at;

      if(last_entered_at)
      {
          stats->xo_last_exited_at        = now;
          stats->xo_accumulated_duration += (now - last_entered_at);
      }
  }
  else
  {
      stats->xo_last_entered_at = now;
      stats->xo_count++;
  }
}


void message_ram_update_rail_info (const railway_settings *settings, void *cookie)
{
  int rail_id = (int)cookie;

  if (rail_id == mx_rail_id)
    message_ram_rail_info->mx_microvolts = settings->microvolts;
  else if (rail_id == cx_rail_id)
    message_ram_rail_info->cx_microvolts = settings->microvolts;
  else
    CORE_VERIFY(0);
}

void message_ram_init_rail_info(void)
{
  mx_rail_id = rail_id("vddmx");
  cx_rail_id = rail_id("vddcx");

  CORE_VERIFY(RAIL_NOT_SUPPORTED_BY_RAILWAY != mx_rail_id);
  CORE_VERIFY(RAIL_NOT_SUPPORTED_BY_RAILWAY != cx_rail_id);

  railway_set_callback(mx_rail_id, RAILWAY_POSTCHANGE_CB, message_ram_update_rail_info, (void *)mx_rail_id);
  railway_set_callback(cx_rail_id, RAILWAY_POSTCHANGE_CB, message_ram_update_rail_info, (void *)cx_rail_id);
}

#define member_sizeof(type, field) (sizeof(((type *)0)->field))
#define STATIC_CORE_VERIFY(tag, cond) extern int COMPILE_TIME_ASSERT__##tag[(cond)?1:-1]
STATIC_CORE_VERIFY(small_alloc_is_multiple_of_4, member_sizeof(message_ram_master_t, small_alloc_space) % 4 == 0);
STATIC_CORE_VERIFY(vmpm_is_multiple_of_4,        member_sizeof(message_ram_master_t, vmpm) % 4 == 0);
STATIC_CORE_VERIFY(four_kB_per_master, sizeof(message_ram_master_t) == 4096);

