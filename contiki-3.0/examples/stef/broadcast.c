#include "contiki.h"
#include "net/rime.h"
 
#include "dev/button-sensor.h"
 
#include "dev/leds.h"
#include "dev/cc2420.h"
#include <stdio.h>
#include "rimeaddr.h"
#define DESTINATION 32
#define CHANNEL 26
 
/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/


static void
recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
{
  printf("unicast message received from %d.%d\n",
     from->u8[0], from->u8[1]);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
     
  PROCESS_BEGIN();
  cc2420_set_channel(CHANNEL);
  unicast_open(&uc, 146, &unicast_callbacks);
 
  while(1) {
    static struct etimer et;
    rimeaddr_t addr;
     
    etimer_set(&et, CLOCK_SECOND);
     
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
 
    packetbuf_copyfrom("Helloooo", 8);
    addr.u8[0] = DESTINATION;
    addr.u8[1] = 0;
    if(!rimeaddr_cmp(&addr, &rimeaddr_node_addr)) {
      unicast_send(&uc, &addr);
    }
 
  }
 
  PROCESS_END();
}
