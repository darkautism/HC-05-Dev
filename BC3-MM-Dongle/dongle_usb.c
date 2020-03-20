#include <usb.h>
#include <panic.h>
#include <message.h>
#include <sink.h>
#include <source.h>
#include <stream.h>
#include <stdio.h>


#include "dongle_usb.h"


#define HID_DESCRIPTOR_TYPE 0x21
#define HID_DESCRIPTOR_LENGTH 0x12

static const UsbCodes codes_dongle = {0xE0, 0x01, 0x01};


/* USB HID class descriptor for the keyboard. */
static const uint8 hid_descriptor_dongle[HID_DESCRIPTOR_LENGTH] =
{
    HID_DESCRIPTOR_LENGTH,                  /* bLength */
    0x01,                                   /* bDescriptorType */
    0x01,0x10,                              /* bcdHID */
    0xE0,0x01,0x01,
    0x64,                                   /* Max packet */
    0x0a,0x12,                              /* Vender */
    0x00,0x01,                              /* Product */
    0x88,0x91,                              /* bcdDevice */
    0x00,
    0x02,                                   /* pid */
    0x00,                                   /* sn */
    0x01,                                   /* cn_conf */
};

/*
static const uint8 zz[] = { 0x07, 0x05, 0x03, 0x01,
0x00, 0x00, 0x01, 0x07, 0x05, 0x83, 0x01, 0x00, 0x00, 0x01, 0x09, 0x04, 0x01, 0x01, 0x02, 0xe0,
0x01, 0x01, 0x00, 0x07, 0x05, 0x03, 0x01, 0x09, 0x00, 0x01, 0x07, 0x05, 0x83, 0x01, 0x09, 0x00,
0x01, 0x09, 0x04, 0x01, 0x02, 0x02, 0xe0, 0x01, 0x01, 0x00, 0x07, 0x05, 0x03, 0x01, 0x11, 0x00,
0x01, 0x07, 0x05, 0x83, 0x01, 0x11, 0x00, 0x01, 0x09, 0x04, 0x01, 0x03, 0x02, 0xe0, 0x01, 0x01,
0x00, 0x07, 0x05, 0x03, 0x01, 0x19, 0x00, 0x01, 0x07, 0x05, 0x83, 0x01, 0x19, 0x00, 0x01, 0x09,
0x04, 0x01, 0x04, 0x02, 0xe0, 0x01, 0x01, 0x00, 0x07, 0x05, 0x03, 0x01, 0x21, 0x00, 0x01, 0x07,
0x05, 0x83, 0x01, 0x21, 0x00, 0x01, 0x09, 0x04, 0x01, 0x05, 0x02, 0xe0, 0x01, 0x01, 0x00, 0x07,
0x05, 0x03, 0x01, 0x3f, 0x00, 0x01, 0x07, 0x05, 0x83, 0x01, 0x3f, 0x00, 0x01
};*/


static const EndPointInfo ep_doungle[] = 
{
    {
        0, 
        0,
		end_point_int_out, 
		end_point_attr_int, 
		16,
        TRUE
    },
    {
        0, 
        0,
		end_point_bulk_in, 
		end_point_attr_bulk, 
		64,
        TRUE
    },
    {
        0, 
        0,
		end_point_bulk_out, 
		end_point_attr_bulk, 
		64,
        TRUE
    },
};

/*
static const EndPointInfo ep_doungle_1[] = 
{
    {
        0, 
        0,
		end_point_iso_in, 
		end_point_attr_iso, 
		0,
        TRUE
    },
    {
        0, 
        0,
		end_point_iso_out, 
		end_point_attr_iso, 
		0,
        TRUE
    },
};

static const EndPointInfo ep_doungle_2[] = 
{
    {
        0, 
        0,
		end_point_iso_in, 
		end_point_attr_iso, 
		9,
        TRUE
    },
    {
        0, 
        0,
		end_point_iso_out, 
		end_point_attr_iso, 
		9,
        TRUE
    },
};

static const EndPointInfo ep_doungle_3[] = 
{
    {
        0, 
        0,
		end_point_iso_in, 
		end_point_attr_iso, 
		17,
        TRUE
    },
    {
        0, 
        0,
		end_point_iso_out, 
		end_point_attr_iso, 
		17,
        TRUE
    },
};

static const EndPointInfo ep_doungle_4[] = 
{
    {
        0, 
        0,
		end_point_iso_in, 
		end_point_attr_iso, 
		25,
        TRUE
    },
    {
        0, 
        0,
		end_point_iso_out, 
		end_point_attr_iso, 
		25,
        TRUE
    },
};

static const EndPointInfo ep_doungle_5[] = 
{
    {
        0, 
        0,
		end_point_iso_in, 
		end_point_attr_iso, 
		33,
        TRUE
    },
    {
        0, 
        0,
		end_point_iso_out, 
		end_point_attr_iso, 
		33,
        TRUE
    },
};
static const EndPointInfo ep_doungle_6[] = 
{
    {
        0, 
        0,
		end_point_iso_in, 
		end_point_attr_iso, 
		63,
        TRUE
    },
    {
        0, 
        0,
		end_point_iso_out, 
		end_point_attr_iso, 
		63,
        TRUE
    },
};
*/

typedef struct
{
    TaskData    				task;
    Source						usb_source;
    Sink						usb_sink;
    Sink                        hid_sink;
} UsbTask;
UsbInterface ui_dongle;
/*
static UsbInterface ui_dongle1;
static UsbInterface ui_dongle2;
static UsbInterface ui_dongle3;
static UsbInterface ui_dongle4;
static UsbInterface ui_dongle5;
static UsbInterface ui_dongle6;
*/

static UsbTask ut_default;
/*
static UsbTask ut_dongle1;
static UsbTask ut_dongle2;
static UsbTask ut_dongle3;
static UsbTask ut_dongle4;
static UsbTask ut_dongle5;
static UsbTask ut_dongle6;
*/

static void devHandler(Task task, MessageId id, Message message)
{
	switch (id)
    {
        default:
            printf("hhUMsg: 0x%0X\n", id);
            break;
    }
}

void UsbInit(void) {
    ui_dongle = UsbAddInterface(&codes_dongle, 0, 0, 0);
    if (ui_dongle == usb_interface_error)
        Panic();

	PanicFalse(UsbAddEndPoints(ui_dongle, 3, ep_doungle));
    
    ut_default.usb_sink = StreamUsbClassSink(ui_dongle);
    ut_default.usb_source = StreamUsbClassSource(ui_dongle);
    
/*
    ui_dongle1 = UsbAddInterface(&codes_dongle, 0x02, zz, sizeof(zz));
    if (ui_dongle1 == usb_interface_error)
        Panic();
	PanicFalse(UsbAddEndPoints(ui_dongle1, 2, ep_doungle_1));
    
    ut_dongle1.usb_sink = StreamUsbClassSink(ui_dongle1);
    ut_dongle1.usb_source = StreamUsbClassSource(ui_dongle1);

    ui_dongle2 = UsbAddInterface(&codes_dongle, 0, 0, 0);
    if (ui_dongle2 == usb_interface_error)
        Panic();
        
	PanicFalse(UsbAddEndPoints(ui_dongle2, 2, ep_doungle_1));
    ut_dongle2.task.handler = devHandler;
    ut_dongle2.usb_sink = StreamUsbClassSink(ui_dongle2);
    ut_dongle2.usb_source = StreamUsbClassSource(ui_dongle2);
    
    ui_dongle3 = UsbAddInterface(&codes_dongle, 0, 0, 0);
    if (ui_dongle3 == usb_interface_error)
        Panic();
        
	PanicFalse(UsbAddEndPoints(ui_dongle3, 2, ep_doungle_1));
    ut_dongle3.task.handler = devHandler;
    ut_dongle3.usb_sink = StreamUsbClassSink(ui_dongle3);
    ut_dongle3.usb_source = StreamUsbClassSource(ui_dongle3);

    ui_dongle4 = UsbAddInterface(&codes_dongle, 0, 0, 0);
    if (ui_dongle4 == usb_interface_error)
        Panic();
        
	PanicFalse(UsbAddEndPoints(ui_dongle4, 2, ep_doungle_1));
    ut_dongle4.task.handler = devHandler;
    ut_dongle4.usb_sink = StreamUsbClassSink(ui_dongle4);
    ut_dongle4.usb_source = StreamUsbClassSource(ui_dongle4);
    
    ui_dongle5 = UsbAddInterface(&codes_dongle, 0, 0, 0);
    if (ui_dongle5 == usb_interface_error)
        Panic();
        
	PanicFalse(UsbAddEndPoints(ui_dongle5, 2, ep_doungle_1));
    ut_dongle5.task.handler = devHandler;
    ut_dongle5.usb_sink = StreamUsbClassSink(ui_dongle5);
    ut_dongle5.usb_source = StreamUsbClassSource(ui_dongle5);
    
    ui_dongle6 = UsbAddInterface(&codes_dongle, 0, 0, 0);
    if (ui_dongle6 == usb_interface_error)
        Panic();
        
	PanicFalse(UsbAddEndPoints(ui_dongle6, 2, ep_doungle_1));
    ut_dongle6.task.handler = devHandler;
    ut_dongle6.usb_sink = StreamUsbClassSink(ui_dongle6);
    ut_dongle6.usb_source = StreamUsbClassSource(ui_dongle6);
    */
}

void UsbMagInit(void) {
    ut_default.task.handler = devHandler;
    MessageSinkTask(ut_default.usb_sink, &ut_default.task);
    /*
    ut_dongle1.task.handler = devHandler;
    MessageSinkTask(ut_dongle1.usb_sink, &ut_dongle1.task);
    MessageSinkTask(ut_dongle2.usb_sink, &ut_dongle2.task);
    MessageSinkTask(ut_dongle3.usb_sink, &ut_dongle3.task);
    MessageSinkTask(ut_dongle4.usb_sink, &ut_dongle4.task);
    MessageSinkTask(ut_dongle5.usb_sink, &ut_dongle5.task);
    MessageSinkTask(ut_dongle6.usb_sink, &ut_dongle6.task);
    */
}

