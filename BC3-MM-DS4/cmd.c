#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <panic.h>
#include <app/bluestack/types.h>
#include <app/bluestack/bluetooth.h>
#include <app/uart/uart_if.h>
#include <stream.h>

#include "main.h"
#include "cmd.h"
#include "uart.h"
#include "log.h"
#include "utils.h"

static void wait_reg_psm ( Task t, MessageId id, Message payload ) {
    uint16 channel = id;
    bdaddr *addr = (bdaddr *)payload;
    if ( (theApp.reg_in == channel && theApp.state & STATUS_CL_L2CAP_REGISTER_CFM_IN) ||
        (theApp.reg_out == channel && theApp.state & STATUS_CL_L2CAP_REGISTER_CFM_OUT) ) {
        #ifdef BLUESTACK_VERSION_MAJOR
        ConnectionL2capConnectRequest(&theApp.task, addr, channel, channel, 0, 0);
        #else /*Old version SDK*/
        ConnectionL2capConnectRequest(&theApp.task, addr, channel, channel, 0);
        #endif
        free(addr);
    } else {
        MessageSendLater( t, 0, 0, 100 );
    }
}

static TaskData waitRegPSM = { wait_reg_psm };

void cmd_parser(uint8 *buf)
{
    if (strstr((char *)buf, (char *)"S"))
    {
        ConnectionInquire(&theApp.task, 0x9E8B33, 0, 8, 0 /*listen all type*/);
    }
    else if (strstr((char *)buf, (char *)"N"))
    {
        ConnectionReadLocalName(&theApp.task);
    }
    else if (strstr((char *)buf, (char *)"L2CAP"))
    {   /* L2CAP,ADDRESS,CHANNEL,I/O */
        uint8 channel;
        bdaddr *addr = PanicUnlessMalloc(sizeof(bdaddr));
        buf[10]=0;
        buf[13]=0;
        buf[20]=0;
        addr->nap=hexadecimalToDecimal((char *)buf+6);
        addr->uap=hexadecimalToDecimal((char *)buf+11);
        addr->lap=hexadecimalToDecimal((char *)buf+14);
        buf[23]=0;
        channel = hexadecimalToDecimal((char *)buf+21);
        if( buf[24] == 'i' ) {
            if (theApp.reg_in != 0) {
                UartPrintf("EEXIST\n");
                free(addr);
                return;
            }
            theApp.reg_in = channel;
        }
        else if( buf[24] == 'o' ) {
            if (theApp.reg_out != 0) {
                UartPrintf("EEXIST\n");
                free(addr);
                return;
            }
            theApp.reg_out = channel;
        }
        else {
            if (theApp.reg_in != 0 && theApp.reg_out != 0) {
                UartPrintf("EEXIST\n");
                free(addr);
                return;
            }
            theApp.reg_in = channel;
            theApp.reg_out = channel;
        }

        UartPrintf("%X:%X:%lX,%X\n",
            addr->nap,
            addr->uap,
            addr->lap,
            channel);

        ConnectionL2capRegisterRequest(&theApp.task, channel);
        MessageSendLater( &waitRegPSM, channel, (void *)addr, 100 ); /* wait reg psm */
    } else if (strstr((char *)buf, (char *)"J")) {
        UartPrintf("EEXIST\n");
        return;
        PanicZero( StreamConnect( StreamSourceFromSink(theApp.sink_in) , StreamUartSink() ));
        PanicZero( StreamConnect( StreamUartSource(), theApp.sink_out ));
    }
}
