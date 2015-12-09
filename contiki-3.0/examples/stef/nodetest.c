#include "dev/leds.h"
#include "dev/watchdog.h"
#include "node-id.h"
#include "contiki.h"
#include "sys/etimer.h"
 
#include <stdio.h>
 
static struct etimer etimer;
 
PROCESS(burn_process, "Burn node id");
AUTOSTART_PROCESSES(&burn_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(burn_process, ev, data)
{
  PROCESS_BEGIN();
 
  etimer_set(&etimer, 5*CLOCK_SECOND);
  PROCESS_WAIT_UNTIL(etimer_expired(&etimer));
 
  watchdog_stop();
  leds_on(LEDS_RED);
#if NODEID
  printf("Burning node id %d\n", NODEID);
  node_id_burn(NODEID);
  leds_on(LEDS_BLUE);
  node_id_restore();
  printf("Restored node id %d\n", node_id);
 
#else
#error "burn-nodeid must be compiled with nodeid=<the ID of the node>"
  node_id_restore();
  printf("Restored node id %d\n", node_id);
#endif
  leds_off(LEDS_RED + LEDS_BLUE);
  watchdog_start();
  while(1) {
    PROCESS_WAIT_EVENT();
  }
  PROCESS_END();
}

