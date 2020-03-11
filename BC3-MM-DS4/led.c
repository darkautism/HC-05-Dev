#include <message.h>
#include <pio.h>
#include <stdio.h>

#include "led.h"
#include "uart.h"

#define LED 	 0x08    	/* HC-05's led pin is 0x80 */

static int led_dark_delay=1000;
static int led_light_delay=100;

static void led_light( Task t, MessageId id, Message payload )
{
    static uint8 light = 0;
    if ( light ) {
        PioSet( LED, (PioGet() & ~LED) );
        MessageSendLater( t, 0, 0, led_dark_delay );
        light=0;
    } else {
        PioSet( LED, (PioGet() | LED) );
        MessageSendLater( t, 0, 0, led_light_delay );
        light=1;
    }
}

static TaskData led_task = { led_light };

void LedInit(void) {    
	PioSetDir(LED, 0xFF&LED);
	PioSet(LED, 0);
	MessageSend( &led_task, 0 , 0 );
}

void LedSpeed(int d,int l) {
    led_dark_delay = d;
    led_light_delay = l;
}
