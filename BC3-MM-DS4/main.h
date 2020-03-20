#ifndef __MAIN_H
#define __MAIN_H
#include <connection.h>
#include <hid.h>
#include <sink.h>

#define STATUS_HID_CONNECTED 0x01

typedef struct
{
    TaskData task;
    uint8 status;
    HID_LIB *hid_lib;
    Sink hid_sink;
    Source hid_source;
    Sink hid_inturrupt_sink;
    HID *hid;
} APP_DATA_T;

extern APP_DATA_T app;
extern const hid_connection_config hid_conn_cfg;
#endif
