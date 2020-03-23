#include <usb.h>
#include <panic.h>
#include <message.h>
#include <sink.h>
#include <source.h>
#include <stream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
#include <hid.h>
*/

#include "main.h"
#include "utils.h"
#include "log.h"
#include "DS4Usb.h"

#ifdef USB


#define VER122
#ifdef VER122
#define REPORT_SIZE 122
#else
#define REPORT_SIZE 483
#endif

const UsbCodes codes_dongle = {0x03, 0x00, 0x00};

/* USB HID class descriptor for the keyboard. */
static const uint8 hid_descriptor_dongle[9] = {
    0x09,       /* bLength */
    0x21,       /* bDescriptorType */
    0x11, 0x01, /* bcdHID */
    0,          /* bCountryCode */
    1,          /* bNumDescriptors */
    0x22,       /* bDescriptorType */
#ifdef VER122
    0x7a, 0x00, /* wDescriptorLength = 122 */
#else
    0xe3, 0x01, /* wDescriptorLength = 483 */
#endif
};

static const uint8 usb_hid_report[REPORT_SIZE] = {
#ifdef VER122
    0x05, 0x01,        /* Usage Page (Generic Desktop Ctrls) */
    0x09, 0x05,        /* Usage (Game Pad) */
    0xA1, 0x01,        /* Collection (Application) */
    0x85, 0x01,        /*   Report ID (1) */
    0x09, 0x30,        /*   Usage (X) */
    0x09, 0x31,        /*   Usage (Y) */
    0x09, 0x32,        /*   Usage (Z) */
    0x09, 0x35,        /*   Usage (Rz) */
    0x15, 0x00,        /*   Logical Minimum (0) */
    0x26, 0xFF, 0x00,  /*   Logical Maximum (255) */
    0x75, 0x08,        /*   Report Size (8) */
    0x95, 0x04,        /*   Report Count (4) */
    0x81, 0x02,        /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */
    0x09, 0x39,        /*   Usage (Hat switch) */
    0x15, 0x00,        /*   Logical Minimum (0) */
    0x25, 0x07,        /*   Logical Maximum (7) */
    0x35, 0x00,        /*   Physical Minimum (0) */
    0x46, 0x3B, 0x01,  /*   Physical Maximum (315) */
    0x65, 0x14,        /*   Unit (System: English Rotation, Length: Centimeter) */
    0x75, 0x04,        /*   Report Size (4) */
    0x95, 0x01,        /*   Report Count (1) */
    0x81, 0x42,        /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State) */
    0x65, 0x00,        /*   Unit (None) */
    0x05, 0x09,        /*   Usage Page (Button) */
    0x19, 0x01,        /*   Usage Minimum (0x01) */
    0x29, 0x0E,        /*   Usage Maximum (0x0E) */
    0x15, 0x00,        /*   Logical Minimum (0) */
    0x25, 0x01,        /*   Logical Maximum (1) */
    0x75, 0x01,        /*   Report Size (1) */
    0x95, 0x0E,        /*   Report Count (14) */
    0x81, 0x02,        /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */
    0x06, 0x00, 0xFF,  /*   Usage Page (Vendor Defined 0xFF00) */
    0x09, 0x20,        /*   Usage (0x20) */
    0x75, 0x06,        /*   Report Size (6) */
    0x95, 0x01,        /*   Report Count (1) */
    0x15, 0x00,        /*   Logical Minimum (0) */
    0x25, 0x7F,        /*   Logical Maximum (127) */
    0x81, 0x02,        /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */
    0x05, 0x01,        /*   Usage Page (Generic Desktop Ctrls) */
    0x09, 0x33,        /*   Usage (Rx) */
    0x09, 0x34,        /*   Usage (Ry) */
    0x15, 0x00,        /*   Logical Minimum (0) */
    0x26, 0xFF, 0x00,  /*   Logical Maximum (255) */
    0x75, 0x08,        /*   Report Size (8) */
    0x95, 0x02,        /*   Report Count (2) */
    0x81, 0x02,        /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */
    0x06, 0x00, 0xFF,  /*   Usage Page (Vendor Defined 0xFF00) */
    0x09, 0x21,        /*   Usage (0x21) */
    0x95, 0x36,        /*   Report Count (54) */
    0x81, 0x02,        /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */
    0x85, 0x05,        /*   Report ID (5) */
    0x09, 0x22,        /*   Usage (0x22) */
    0x95, 0x1F,        /*   Report Count (31) */
    0x91, 0x02,        /*   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x11,        /*   Report ID (17) */
    0x09, 0x27,        /*   Usage (0x27) */
    0x95, 0x02,        /*   Report Count (2) */
    0xB1, 0x02,        /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0xC0,              /* End Collection */

    /* 122 bytes */
#else
    0x05, 0x01,       /* Usage Page (Generic Desktop Ctrls) */
    0x09, 0x05,       /* Usage (Game Pad) */
    0xA1, 0x01,       /* Collection (Application) */
    0x85, 0x01,       /*   Report ID (1) */
    0x09, 0x30,       /*   Usage (X) */
    0x09, 0x31,       /*   Usage (Y) */
    0x09, 0x32,       /*   Usage (Z) */
    0x09, 0x35,       /*   Usage (Rz) */
    0x15, 0x00,       /*   Logical Minimum (0) */
    0x26, 0xFF, 0x00, /*   Logical Maximum (255) */
    0x75, 0x08,       /*   Report Size (8) */
    0x95, 0x04,       /*   Report Count (4) */
    0x81, 0x02,       /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */
    0x09, 0x39,       /*   Usage (Hat switch) */
    0x15, 0x00,       /*   Logical Minimum (0) */
    0x25, 0x07,       /*   Logical Maximum (7) */
    0x35, 0x00,       /*   Physical Minimum (0) */
    0x46, 0x3B, 0x01, /*   Physical Maximum (315) */
    0x65, 0x14,       /*   Unit (System: English Rotation, Length: Centimeter) */
    0x75, 0x04,       /*   Report Size (4) */
    0x95, 0x01,       /*   Report Count (1) */
    0x81, 0x42,       /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State) */
    0x65, 0x00,       /*   Unit (None) */
    0x05, 0x09,       /*   Usage Page (Button) */
    0x19, 0x01,       /*   Usage Minimum (0x01) */
    0x29, 0x0E,       /*   Usage Maximum (0x0E) */
    0x15, 0x00,       /*   Logical Minimum (0) */
    0x25, 0x01,       /*   Logical Maximum (1) */
    0x75, 0x01,       /*   Report Size (1) */
    0x95, 0x0E,       /*   Report Count (14) */
    0x81, 0x02,       /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */
    0x06, 0x00, 0xFF, /*   Usage Page (Vendor Defined 0xFF00) */
    0x09, 0x20,       /*   Usage (0x20) */
    0x75, 0x06,       /*   Report Size (6) */
    0x95, 0x01,       /*   Report Count (1) */
    0x15, 0x00,       /*   Logical Minimum (0) */
    0x25, 0x7F,       /*   Logical Maximum (127) */
    0x81, 0x02,       /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */
    0x05, 0x01,       /*   Usage Page (Generic Desktop Ctrls) */
    0x09, 0x33,       /*   Usage (Rx) */
    0x09, 0x34,       /*   Usage (Ry) */
    0x15, 0x00,       /*   Logical Minimum (0) */
    0x26, 0xFF, 0x00, /*   Logical Maximum (255) */
    0x75, 0x08,       /*   Report Size (8) */
    0x95, 0x02,       /*   Report Count (2) */
    0x81, 0x02,       /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */
    0x06, 0x00, 0xFF, /*   Usage Page (Vendor Defined 0xFF00) */
    0x09, 0x21,       /*   Usage (0x21) */
    0x95, 0x36,       /*   Report Count (54) */
    0x81, 0x02,       /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */
    0x85, 0x05,       /*   Report ID (5) */
    0x09, 0x22,       /*   Usage (0x22) */
    0x95, 0x1F,       /*   Report Count (31) */
    0x91, 0x02,       /*   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x04,       /*   Report ID (4) */
    0x09, 0x23,       /*   Usage (0x23) */
    0x95, 0x24,       /*   Report Count (36) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x02,       /*   Report ID (2) */
    0x09, 0x24,       /*   Usage (0x24) */
    0x95, 0x24,       /*   Report Count (36) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x08,       /*   Report ID (8) */
    0x09, 0x25,       /*   Usage (0x25) */
    0x95, 0x03,       /*   Report Count (3) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x10,       /*   Report ID (16) */
    0x09, 0x26,       /*   Usage (0x26) */
    0x95, 0x04,       /*   Report Count (4) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x11,       /*   Report ID (17) */
    0x09, 0x27,       /*   Usage (0x27) */
    0x95, 0x02,       /*   Report Count (2) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x12,       /*   Report ID (18) */
    0x06, 0x02, 0xFF, /*   Usage Page (Vendor Defined 0xFF02) */
    0x09, 0x21,       /*   Usage (0x21) */
    0x95, 0x0F,       /*   Report Count (15) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x13,       /*   Report ID (19) */
    0x09, 0x22,       /*   Usage (0x22) */
    0x95, 0x16,       /*   Report Count (22) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x14,       /*   Report ID (20) */
    0x06, 0x05, 0xFF, /*   Usage Page (Vendor Defined 0xFF05) */
    0x09, 0x20,       /*   Usage (0x20) */
    0x95, 0x10,       /*   Report Count (16) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x15,       /*   Report ID (21) */
    0x09, 0x21,       /*   Usage (0x21) */
    0x95, 0x2C,       /*   Report Count (44) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x06, 0x80, 0xFF, /*   Usage Page (Vendor Defined 0xFF80) */
    0x85, 0x80,       /*   Report ID (-128) */
    0x09, 0x20,       /*   Usage (0x20) */
    0x95, 0x06,       /*   Report Count (6) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x81,       /*   Report ID (-127) */
    0x09, 0x21,       /*   Usage (0x21) */
    0x95, 0x06,       /*   Report Count (6) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x82,       /*   Report ID (-126) */
    0x09, 0x22,       /*   Usage (0x22) */
    0x95, 0x05,       /*   Report Count (5) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x83,       /*   Report ID (-125) */
    0x09, 0x23,       /*   Usage (0x23) */
    0x95, 0x01,       /*   Report Count (1) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x84,       /*   Report ID (-124) */
    0x09, 0x24,       /*   Usage (0x24) */
    0x95, 0x04,       /*   Report Count (4) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x85,       /*   Report ID (-123) */
    0x09, 0x25,       /*   Usage (0x25) */
    0x95, 0x06,       /*   Report Count (6) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x86,       /*   Report ID (-122) */
    0x09, 0x26,       /*   Usage (0x26) */
    0x95, 0x06,       /*   Report Count (6) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x87,       /*   Report ID (-121) */
    0x09, 0x27,       /*   Usage (0x27) */
    0x95, 0x23,       /*   Report Count (35) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x88,       /*   Report ID (-120) */
    0x09, 0x28,       /*   Usage (0x28) */
    0x95, 0x22,       /*   Report Count (34) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x89,       /*   Report ID (-119) */
    0x09, 0x29,       /*   Usage (0x29) */
    0x95, 0x02,       /*   Report Count (2) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x90,       /*   Report ID (-112) */
    0x09, 0x30,       /*   Usage (0x30) */
    0x95, 0x05,       /*   Report Count (5) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x91,       /*   Report ID (-111) */
    0x09, 0x31,       /*   Usage (0x31) */
    0x95, 0x03,       /*   Report Count (3) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x92,       /*   Report ID (-110) */
    0x09, 0x32,       /*   Usage (0x32) */
    0x95, 0x03,       /*   Report Count (3) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0x93,       /*   Report ID (-109) */
    0x09, 0x33,       /*   Usage (0x33) */
    0x95, 0x0C,       /*   Report Count (12) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xA0,       /*   Report ID (-96) */
    0x09, 0x40,       /*   Usage (0x40) */
    0x95, 0x06,       /*   Report Count (6) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xA1,       /*   Report ID (-95) */
    0x09, 0x41,       /*   Usage (0x41) */
    0x95, 0x01,       /*   Report Count (1) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xA2,       /*   Report ID (-94) */
    0x09, 0x42,       /*   Usage (0x42) */
    0x95, 0x01,       /*   Report Count (1) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xA3,       /*   Report ID (-93) */
    0x09, 0x43,       /*   Usage (0x43) */
    0x95, 0x30,       /*   Report Count (48) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xA4,       /*   Report ID (-92) */
    0x09, 0x44,       /*   Usage (0x44) */
    0x95, 0x0D,       /*   Report Count (13) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xA5,       /*   Report ID (-91) */
    0x09, 0x45,       /*   Usage (0x45) */
    0x95, 0x15,       /*   Report Count (21) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xA6,       /*   Report ID (-90) */
    0x09, 0x46,       /*   Usage (0x46) */
    0x95, 0x15,       /*   Report Count (21) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xF0,       /*   Report ID (-16) */
    0x09, 0x47,       /*   Usage (0x47) */
    0x95, 0x3F,       /*   Report Count (63) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xF1,       /*   Report ID (-15) */
    0x09, 0x48,       /*   Usage (0x48) */
    0x95, 0x3F,       /*   Report Count (63) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xF2,       /*   Report ID (-14) */
    0x09, 0x49,       /*   Usage (0x49) */
    0x95, 0x0F,       /*   Report Count (15) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xA7,       /*   Report ID (-89) */
    0x09, 0x4A,       /*   Usage (0x4A) */
    0x95, 0x01,       /*   Report Count (1) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xA8,       /*   Report ID (-88) */
    0x09, 0x4B,       /*   Usage (0x4B) */
    0x95, 0x01,       /*   Report Count (1) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xA9,       /*   Report ID (-87) */
    0x09, 0x4C,       /*   Usage (0x4C) */
    0x95, 0x08,       /*   Report Count (8) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xAA,       /*   Report ID (-86) */
    0x09, 0x4E,       /*   Usage (0x4E) */
    0x95, 0x01,       /*   Report Count (1) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xAB,       /*   Report ID (-85) */
    0x09, 0x4F,       /*   Usage (0x4F) */
    0x95, 0x39,       /*   Report Count (57) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xAC,       /*   Report ID (-84) */
    0x09, 0x50,       /*   Usage (0x50) */
    0x95, 0x39,       /*   Report Count (57) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xAD,       /*   Report ID (-83) */
    0x09, 0x51,       /*   Usage (0x51) */
    0x95, 0x0B,       /*   Report Count (11) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xAE,       /*   Report ID (-82) */
    0x09, 0x52,       /*   Usage (0x52) */
    0x95, 0x01,       /*   Report Count (1) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xAF,       /*   Report ID (-81) */
    0x09, 0x53,       /*   Usage (0x53) */
    0x95, 0x02,       /*   Report Count (2) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xB0,       /*   Report ID (-80) */
    0x09, 0x54,       /*   Usage (0x54) */
    0x95, 0x3F,       /*   Report Count (63) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xB1,       /*   Report ID (-79) */
    0x09, 0x55,       /*   Usage (0x55) */
    0x95, 0x02,       /*   Report Count (2) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0x85, 0xB2,       /*   Report ID (-78) */
    0x09, 0x56,       /*   Usage (0x56) */
    0x95, 0x02,       /*   Report Count (2) */
    0xB1, 0x02,       /*   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */
    0xC0              /* End Collection */
    /* 483 bytes */
#endif
};

static const EndPointInfo ep_dongle[] = {
    {0,
     0,
     DS4ENDPOINT,
     end_point_attr_int,
     64,
     TRUE},
};

UsbTask usb;
UsbInterface ui_dongle;

static void UsbHandleClassRequest(void) {
    uint16 packet_size;
	/* Check for outstanding class requests */
	while ((packet_size = SourceBoundary(usb.usb_source)) != 0)
	{
        UsbResponse usb_response;	
        /* Build the response, it must contain the original request, so copy from
		   the source header. */
		memcpy(&usb_response.original_request, SourceMapHeader(usb.usb_source), sizeof(UsbRequest));
		/* Set the response fields to default values to make the code below simpler */
		usb_response.success = TRUE;
		usb_response.data_length = 0;
		/* Process the request */
		switch (usb_response.original_request.bRequest)
		{
			/* GET_REPORT */
        	case 0x01:
                usb_response.success = FALSE;
				break;

			/* GET_IDLE */
            case 0x02: {
                tmp_ucp = SinkMap(usb.usb_sink) + SinkClaim(usb.usb_sink, 1);
                tmp_ucp[0] = app.idle;
                usb_response.data_length = 1;
				break;
            }

			/* GET_PROTOCOL */
            case 0x03:
                tmp_ucp = SinkMap(usb.usb_sink) + SinkClaim(usb.usb_sink, 1);
                tmp_ucp[0] = app.protocol;
                usb_response.data_length = 1;
				break;

			/* SET_REPORT */
            case 0x09: 
                usb_response.success = TRUE;
                {
                    uint8 i;
                    uint8 size = SourceSize(usb.usb_source);
                    if ((i = SinkClaim(app.hid_inturrupt_sink, 79)) != 0xFFFF) {
                        tmp_ucp = SinkMap(app.hid_inturrupt_sink)+i;
                        tmp_ucp[0] = 0x52;
                        tmp_ucp[1] = 0x11;
                        tmp_ucp[2] = 0x3E|0x80;
                        tmp_ucp[4] = 0x01|0x02|0x04;
                        memcpy(tmp_ucp+3+2, (uint8 *)SourceMap(usb.usb_source)+2, size-2); /* Skip 2 byte we dont know */
                        memset(tmp_ucp+3+size,0, 76-size);
                        SinkFlush(app.hid_inturrupt_sink, 79);
                    }
                }
				break;

			/* SET IDLE */
            case 0x0a:
                if (app.status & STATUS_HID_CONNECTED) {
                    app.idle = usb_response.original_request.wValue >> 8;
                    HidSetIdle(app.hid, app.idle);
                } else {
                    usb_response.success = FALSE;
                }
				break;

			/* SET_PROTOCOL */
            case 0x0b:
                app.protocol = usb_response.original_request.wValue & 1;
				break;

			/* Unknown command, report failure */
            default:
                LOG_DEBUG(("WTF?\n"));
				break;
		}

		/* Send response, handle 0 length responses */
		if (usb_response.data_length == 0)
		{
           	/* Set length to minumum allowed */
            usb_response.data_length = 1;

			/* Sink packets can never be zero-length, so flush a dummy byte */
			SinkClaim(usb.usb_sink, 1);
        }
		SinkFlushHeader(usb.usb_sink, usb_response.data_length, (uint16 *)&usb_response, sizeof(UsbResponse));

		/* Discard the original request */
		SourceDrop(usb.usb_source, packet_size);
	}
}

static void devHandler(Task task, MessageId id, Message message)
{
    switch (id)
    {
        case MESSAGE_MORE_DATA:
        if (((MessageMoreData *)message)->source == usb.usb_source)
            UsbHandleClassRequest();
            break;
    default:
        LOG_DEBUG(("UMsg: 0x%0X\n", id));
        break;
    }
}

void UsbInit(void)
{
    ui_dongle = UsbAddInterface(&codes_dongle, 0x21, hid_descriptor_dongle, 9);
    if (ui_dongle == usb_interface_error)
        Panic();

    (void)PanicFalse(UsbAddDescriptor(ui_dongle, 0x22, usb_hid_report, REPORT_SIZE));

    PanicFalse(UsbAddEndPoints(ui_dongle, 1, ep_dongle));
}

void UsbMagInit(void)
{
    usb.task.handler = devHandler;
    usb.usb_sink = StreamUsbClassSink(ui_dongle);
    usb.usb_source = StreamUsbClassSource(ui_dongle);
    MessageSinkTask(usb.usb_sink, &usb.task);
}
#endif
