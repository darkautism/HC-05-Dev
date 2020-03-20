#ifndef __SINK_UART_H_
#define __SINK_UART_H_
#ifdef UART
extern void UartStreamInit(void);
extern void UartSendData(uint8 *buf, uint16 len);
extern void uart_stream_handler(Task t, MessageId id, Message payload);
extern void UartPrintf(const char * format, ...);
#endif
#endif
