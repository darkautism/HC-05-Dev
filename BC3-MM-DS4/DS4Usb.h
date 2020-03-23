#ifndef __DS4USB_H
#define __DS4USB_H
#include <hid.h>
/*
#include <transform_.h>
*/
extern void UsbInit(void);
extern void UsbMagInit(void);
extern UsbInterface ui_dongle;

typedef struct
{
    TaskData task;
    Source usb_source;
    Sink usb_sink;
    Sink ep_sink;
} UsbTask;

#define DS4ENDPOINT end_point_int_out

extern UsbTask usb;

#endif
