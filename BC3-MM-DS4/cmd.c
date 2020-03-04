#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <panic.h>
#include <app/bluestack/types.h>
#include <app/bluestack/bluetooth.h>
#include "main.h"
#include "cmd.h"
#include "uart.h"
#include "log.h"
#include "utils.h"

void cmd_parser(uint8 *buf)
{
    if (strstr((char *)buf, (char *)"scan"))
    {
        UartPrintf("Scan start...\n");
        ConnectionInquire(&theApp.task, 0x9E8B33, 0, 8, 0 /*listen all type*/);
    }
    else if (strstr((char *)buf, (char *)"name"))
    {
        ConnectionReadLocalName(&theApp.task);
    }
    else if (strstr((char *)buf, (char *)"conn"))
    {
        bdaddr *addr = PanicUnlessMalloc(sizeof(bdaddr));
        buf[9]=0;
        buf[12]=0;
        buf[19]=0;
        addr->nap=hexadecimalToDecimal((char *)buf+5);
        addr->uap=hexadecimalToDecimal((char *)buf+10);
        addr->lap=hexadecimalToDecimal((char *)buf+13);        
        UartPrintf("%X:%X:%lX\n",
            addr->nap,
            addr->uap,
            addr->lap);
        #ifdef BLUESTACK_VERSION_MAJOR
        ConnectionL2capConnectRequest(&theApp.task, addr, 0x11, 0x11, 0, 0);
        ConnectionL2capConnectRequest(&theApp.task, addr, 0x13, 0x13, 0, 0);
        #else /*Old version SDK*/
        ConnectionL2capConnectRequest(&theApp.task, addr, 0x11, 0x11, 0);
        ConnectionL2capConnectRequest(&theApp.task, addr, 0x13, 0x13, 0);
        #endif
        free(addr);
    }
}
