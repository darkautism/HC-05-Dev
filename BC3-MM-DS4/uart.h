#ifndef __SINK_UART_H_
#define __SINK_UART_H_

#define UART_UI_ENABLE 1
#define UART_UI_DISABLE 0

extern void UartStreamInit(void);
extern void UartSendData(uint8 *buf, uint16 len);
extern void uart_stream_handler(Task t, MessageId id, Message payload);
extern void UartPrintf(const char * format, ...);
extern void UartUserInterface(uint8 enable);
#endif
