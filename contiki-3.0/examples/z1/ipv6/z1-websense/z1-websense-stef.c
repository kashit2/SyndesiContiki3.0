/*
 * Copyright (c) 2011, Zolertia(TM) is a trademark by Advancare,SL
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 *         Battery and Temperature IPv6 Demo for Zolertia Z1
 * \author
 *         Niclas Finne    <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         Joel Hoglund    <joel@sics.se>
 *         Enric M. Calvo  <ecalvo@zolertia.com>
 */

#include "contiki.h"
#include "httpd-simple.h"
#include "webserver-nogui.h"
#include "dev/button-sensor.h"
#include "dev/temperature-sensor.h"
#include "dev/battery-sensor.h"
#include "cc2420.h"
#include "dev/leds.h"
#include <stdio.h>
#include "dev/z1-phidgets.h"


float floor(float x){ 
  if(x>=0.0f) return (float) ((int)x);
  else        return (float) ((int)x-1);
}

PROCESS(web_sense_process, "Sense Web Demo");

AUTOSTART_PROCESSES(&web_sense_process);

#define HISTORY 16
static int temperature[HISTORY];
static int battery1[HISTORY];
static int sensors_pos;

static int get_phidget(void) 
{
  return phidgets.value(PHIDGET3V_2);
}


/*---------------------------------------------------------------------------*/
static int
get_battery(void)
{
  return battery_sensor.value(0);
}
/*---------------------------------------------------------------------------*/
static int
get_temp(void)
{
  return temperature_sensor.value(0);
}

static float get_mybatt(void){ return (float) ((get_battery()*2.500*2)/4096);}
static float get_mytemp(void){ return (float) (((get_temp()*2.500)/4096)-0.986)*282;}

/*---------------------------------------------------------------------------*/
static const char *TOP = "<html><head><title>Contiki Web Sense</title></head><body>\n";
static const char *BOTTOM = "</body></html>\n";
/*---------------------------------------------------------------------------*/
/* Only one single request at time */
static char buf[256];
static int blen;
#define ADD(...) do {                                                   \
    blen += snprintf(&buf[blen], sizeof(buf) - blen, __VA_ARGS__);      \
  } while(0)
static void
generate_chart(const char *title, const char *unit, int min, int max, int *values)
{
  int i;
  blen = 0;
  ADD("<h1>%s</h1>\n"
      "<img src=\"http://chart.apis.google.com/chart?"
      "cht=lc&chs=400x300&chxt=x,x,y,y&chxp=1,50|3,50&"
      "chxr=2,%d,%d|0,0,30&chds=%d,%d&chxl=1:|Time|3:|%s&chd=t:",
      title, min, max, min, max, unit);
  for(i = 0; i < HISTORY; i++) {
    ADD("%s%d", i > 0 ? "," : "", values[(sensors_pos + i) % HISTORY]);
  }
  ADD("\">");
}
static
PT_THREAD(send_values(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);



    /* Default page: show latest sensor values as text (does not
       require Internet connection to Google for charts). */
    blen = 0;
    float mybatt = get_mybatt();
    float mytemp = get_mytemp();
    int myphidg = get_phidget();
    ADD("<h1>Current readings</h1>\n"
        "Battery: %ld.%03d V"
        "Temperature: %ld.%03d &deg; C\n" "Rotation: %d units<br>",
        (long) mybatt, (unsigned) ((mybatt-floor(mybatt))*1000), 
        (long) mytemp, (unsigned) ((mytemp-floor(mytemp))*1000), (int) myphidg); 
    SEND_STRING(&s->sout, buf);


  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
httpd_simple_script_t
httpd_simple_get_script(const char *name)
{
  return send_values;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(web_sense_process, ev, data)
{
  static struct etimer timer;
  PROCESS_BEGIN();
  cc2420_set_txpower(31);

  sensors_pos = 0;
  process_start(&webserver_nogui_process, NULL);

  etimer_set(&timer, CLOCK_SECOND);
  SENSORS_ACTIVATE(phidgets);
  SENSORS_ACTIVATE(battery_sensor);
  SENSORS_ACTIVATE(temperature_sensor);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);

    battery1[sensors_pos] = get_mybatt()*1000;
    temperature[sensors_pos] = get_mytemp();
    printf("Phidget 5V 1:%d\n", phidgets.value(PHIDGET5V_1));
    printf("Phidget 5V 2:%d\n", phidgets.value(PHIDGET5V_2));
    printf("Phidget 3V 1:%d\n", phidgets.value(PHIDGET3V_1));
    printf("Phidget 3V 2:%d\n", phidgets.value(PHIDGET3V_2));
    sensors_pos = (sensors_pos + 1) % HISTORY;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
