/*
* This application demonstrates how to make a simple connection between two
 * Bluetooth devices. It will pair the devices using the 2.1 Simple Pairing
 * 'Just works' model, and send "Hello World" from Device A to Device B.
 *
 * Device A is the master, initiates the connection and sends the "Hello World"
 * String.
 *
 * Device B is the slave, it scans for the connection from Device A and then
 * receives and outputs the string to the xIDE 'Print Channel 0' tab.
 *
 * This is the code for Device B - The Slave.
 */

#include <connection.h>
#include <app/bluestack/types.h>
#include <app/bluestack/bluetooth.h>
#include <ps.h>
#include <panic.h>
#include <stdio.h>
#include <source.h>
#include <sink.h>
#include <pio.h>
#include <source.h>
#include <string.h>
#include <stream.h>
#include <bdaddr.h>
#include <transform.h>
#ifdef USB
#include <hid.h>
#endif

#include "led.h"
#include "uart.h"
#include "main.h"
#include "inquiry.h"
#include "log.h"
#include "utils.h"

#ifdef USB
#include "DS4Usb.h"
#endif

#define loopConnect 4000

APP_DATA_T app;

const hid_connection_config hid_conn_cfg =
    {
        11250, /* Latency (11.25ms) */
        TRUE,
};

static void app_handler(Task task, MessageId id, Message message)
{
    switch (id)
    {
    case CL_INIT_CFM:
    {
        CL_INIT_CFM_T *cic = (CL_INIT_CFM_T *)message;
        HidInit(task, 0);
        if (success == cic->status)
        {
            LOG_DEBUG(("Bluetooth version: "));
            switch (cic->version)
            {
            case bluetooth2_0:
                LOG_DEBUG(("v2.0\n"));
                ConnectionWriteInquiryMode(task, inquiry_mode_rssi);
                break;
            case bluetooth2_1:
                LOG_DEBUG(("v2.1\n"));
                ConnectionWriteInquiryMode(task, inquiry_mode_eir);
                break;
            default:
                LOG_DEBUG(("unknow\n"));
                ConnectionWriteInquiryMode(task, inquiry_mode_eir);
            }

            /* Page scan for 100 slots out of every 150 */
            ConnectionWritePagescanActivity(150, 100);
            /* Make us discoverable for inquiries and pages */
            ConnectionWriteScanEnable(hci_scan_enable_inq_and_page);

            /* enable pin pair */
            ConnectionSmSetSecurityMode(task, sec_mode0_off, hci_enc_mode_off);
            ConnectionSmSetSecurityLevel(protocol_l2cap, 0x0001, ssp_secl4_l0, TRUE, FALSE, FALSE);
            ConnectionSmSetSecurityLevel(protocol_rfcomm, 0x0001, ssp_secl4_l0, TRUE, FALSE, FALSE);
            /* start scan */
            ConnectionInquire(task, 0x9E8B33, 0, 8, 0 /*listen all type*/);
        }
        else
        {
            LOG_DEBUG(("Init failure: %d\n", cic->status));
        }
    }
    break;

    case CL_DM_WRITE_INQUIRY_MODE_CFM:
    case CL_DM_READ_INQUIRY_TX_CFM:
    case CL_DM_LOCAL_NAME_COMPLETE:
    case CL_SM_SECURITY_LEVEL_CFM:
    case CL_SDP_SERVICE_SEARCH_ATTRIBUTE_CFM:
    case CL_DM_ACL_OPENED_IND:
    case CL_DM_ACL_CLOSED_IND:
    case CL_SM_AUTHENTICATE_CFM:
        break;

    case CL_L2CAP_CONNECT_IND:
        printf("CL_L2CAP_CONNECT_IND\n");
        break;

    case CL_SM_PIN_CODE_IND:
        ConnectionSmPinCodeResponse(&((CL_SM_PIN_CODE_IND_T *)message)->bd_addr, 4, (uint8 *)"0000");
        break;

    case CL_DM_REMOTE_NAME_COMPLETE:
#define _RNAME ((CL_DM_REMOTE_NAME_COMPLETE_T *)message)
        if (_RNAME->status == hci_success)
        {
            if (strstr((char *)_RNAME->remote_name, "Wireless Controller"))
            {
                HidConnect(app.hid_lib, task, &_RNAME->bd_addr, &hid_conn_cfg);
            }
        }
        break;

    case CL_DM_INQUIRE_RESULT:
    {
        switch (((CL_DM_INQUIRE_RESULT_T *)message)->status)
        {
        case inquiry_status_ready:
            LedSpeed(1000, 100);
            inquiry_complete(task, (CL_DM_INQUIRE_RESULT_T *)message);
            MessageSendLater(task, loopConnect, 0, 10000);
        case inquiry_status_result:
            inquiry_result(task, (CL_DM_INQUIRE_RESULT_T *)message);
        }
        break;
    }

    case HID_INIT_CFM:
        app.hid_lib = ((HID_INIT_CFM_T *)message)->hid_lib;
        break;

    case HID_CONNECT_CFM:
        LedSpeed(100, 100);
        app.status |= STATUS_HID_CONNECTED;
        app.hid = ((HID_CONNECT_CFM_T *)message)->hid;
        app.hid_sink = ((HID_CONNECT_CFM_T *)message)->interrupt_sink;
        app.hid_source = StreamSourceFromSink(app.hid_sink);
        app.hid_inturrupt_sink = ((Sink)((uint8*)app.hid + 32)); /* Just force use offset to set struct*/
#ifdef UART
        PanicZero(StreamConnect(StreamSourceFromSink(app.hid_sink), StreamUartSink()));
        PanicZero(StreamConnect(StreamUartSource(), app.hid_inturrupt_sink));
#endif
#ifdef USB
        MessageSinkTask(app.hid_sink, task);
        /* TODO Set Report */
#endif
        break;

    case HID_DISCONNECT_IND:
        LedSpeed(1000, 100);
        app.status &= ~STATUS_HID_CONNECTED;
        MessageSendLater(task, loopConnect, 0, 10000);
#ifdef UART
        StreamDisconnect(0, StreamUartSink());
        StreamDisconnect(StreamUartSource(), 0);
#endif
        break;

    case HID_SET_REPORT_CFM:
        printf("HID_SET_REPORT_CFM %d\n", ((HID_SET_REPORT_CFM_T *)message)->status);
        break;

#ifdef USB
    case MESSAGE_MORE_DATA: {        
        uint8 i = 0;
        uint8 *dest;
        Sink sink;
        tmp_u8 = SourceSize(app.hid_source);
        tmp_ucp = (uint8 *)SourceMap(app.hid_source);
        printf("s %d\n", tmp_u8);
        for (i = 0; i + 10 < tmp_u8; i++)
        {
            if (tmp_ucp[i] != 0xa1)
                continue; /* drop this */
            sink = StreamUsbEndPointSink(DS4ENDPOINT);
            if (SinkClaim(sink, 64) != 0)
                break; /* claim fail so skip this round */
            dest = SinkMap(sink);
            if (tmp_ucp[++i] == 0x01)
            { /* report 0x01 */
                memcpy(dest, &tmp_ucp[i], 10);
                memset(dest + 10, 0, 54);
            }
            else if (tmp_ucp[i] == 0x11 || tmp_ucp[i] == 0x05)
            { /* report 0x11 */
                memcpy(dest, &tmp_ucp[i + 2] /*Skip first and second bytes*/, 64);
            }
            else
            {
                continue; /* drop this */
            }
            SinkFlush(sink, 64);
            break; /* Because BC3 chip is too slow so we can not handle all of DS4 packet, just skip other packet */
        }
        /* drop all because this chip cannot handle more data */
        SourceDrop(app.hid_source, tmp_u8);
        break;
    }
#endif

    case loopConnect:
        LOG_DEBUG(("LOOPCONNECT\n"));
        if ((app.status & STATUS_HID_CONNECTED) == 0)
        {
            LedSpeed(500, 100);
            ConnectionInquire(task, 0x9E8B33, 0, 8, 0 /*listen all type*/);
        }
        break;

    default:
        LOG_DEBUG(("UMsg: 0x%0X\n", id));
        printf("UMsg: 0x%0X\n", id);
        break;
    }
}

#ifdef USB
extern void _init(void);
void _init(void)
{
    UsbInit();
}
#endif

int main(void)
{
    app.task.handler = app_handler;
    /*UartStreamInit();*/
    LedInit();
#ifdef USB
    UsbMagInit();
#endif
    ConnectionInit(&app.task);
    MessageLoop();
    return 0;
}
