#include <stream.h>
#include <sink.h>
#include <source.h>
#include <string.h>
#include <panic.h>
#include <message.h>
#include <app/uart/uart_if.h>
#include <connection.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "uart.h"
#include "utils.h"

#ifdef UART

typedef struct
{
    TaskData task;
} UARTStreamTask_t;

#define UART_RECV_BUF_SIZE 64

static UARTStreamTask_t theUARTStreamTask;
static uint8 uartRecvBufLen = 0;
static uint8 uartRecvBuf[UART_RECV_BUF_SIZE];

#ifdef ENABLE_UART_CMD
static void cmd_parser(uint8 *buf)
{
    if (strstr((char *)buf, (char *)"scan"))
    {
        UartPrintf("Scan start...\n");
        ConnectionInquire(&app.task, 0x9E8B33, 0, 8, 0 /*listen all type*/);
    }
    else if (strstr((char *)buf, (char *)"name"))
    {
        ConnectionReadLocalName(&app.task);
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
        HidConnect(app.hid_lib, &app.task, addr, &hid_conn_cfg);
        free(addr);
    }
}
#endif

void UartStreamInit(void)
{
    /* Assign task message handler */
    theUARTStreamTask.task.handler = uart_stream_handler;
    StreamConfigure(VM_STREAM_UART_CONFIG, VM_STREAM_UART_THROUGHPUT);
    /* Configure uart settings */
    StreamUartConfigure(VM_UART_RATE_38K4, VM_UART_STOP_ONE, VM_UART_PARITY_NONE);
    /* Register uart source with task */
    MessageSinkTask(StreamSinkFromSource(StreamUartSource()), &theUARTStreamTask.task);
}

void UartSendData(uint8 *buf, uint16 len)
{
    uint16 offset;
    uint8 *dest;
    /*get the sink for the uart, panic if not available*/
    Sink sink = StreamUartSink();
    PanicNull(sink);
    /*claim space in the sink, getting the offset to it*/
    offset = SinkClaim(sink, len);
    if (offset == 0xFFFF)
        Panic(); /*space not available*/
    /*Map the sink into memory space*/
    dest = SinkMap(sink);
    (void)PanicNull(dest);
    /*copy the string into the claimed space*/
    memcpy(dest + offset, buf, len);
    /*Flush the data out to the uart*/
    PanicZero(SinkFlush(sink, len));
}

/*static uint8 uartRecvData(uint8* dataBuff,uint8 len)*/
static void uartRecvData(void)
{
    Source src = StreamUartSource();
    uint8 recvLen = SourceSize(src);
    uint8 *recvPtr = (uint8 *)SourceMap(src);
    if (recvLen > UART_RECV_BUF_SIZE - uartRecvBufLen)
        recvLen = UART_RECV_BUF_SIZE - uartRecvBufLen;
    if (recvLen > 0)
    {
        memcpy(uartRecvBuf + uartRecvBufLen, recvPtr, recvLen);
        uartRecvBufLen += recvLen;
    }
    SourceDrop(src, recvLen);
#ifdef ENABLE_UART_CMD
ScanCmd:
    for (tmp_u8 = 0; tmp_u8 < uartRecvBufLen; tmp_u8++)
    { /* find the new line */
        if (uartRecvBuf[tmp_u8] == '\n')
        {
            tmp_u8+=1;
            uartRecvBuf[tmp_u8] = 0;
            cmd_parser(uartRecvBuf);
            if (uartRecvBufLen != tmp_u8)
            {
                memcpy(uartRecvBuf, uartRecvBuf + tmp_u8, uartRecvBufLen - tmp_u8);
            }
            uartRecvBufLen -= tmp_u8;
            goto ScanCmd;
        }
    }
#endif

    if (uartRecvBufLen >= UART_RECV_BUF_SIZE)
    { /* clean buffer */
        uartRecvBufLen = 0;
    }
}

void uart_stream_handler(Task t, MessageId id, Message payload)
{
    switch (id)
    {
    case MESSAGE_MORE_DATA:
    {
        uartRecvData();
    }
    break;
    default:
        break;
    }
}

/*Max size is 64*/
void UartPrintf(const char * format, ...) {
    char buf[64];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    UartSendData((uint8 *)buf,strlen(buf));
}
#endif
