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
#include <pio.h>
#include <source.h>
#include <string.h>
#include <stream.h>
#include <bdaddr.h>

#include "led.h"
#include "uart.h"
#include "main.h"
#include "inquiry.h"
#include "log.h"
#include "utils.h"

#define loopConnect 4000

APP_DATA_T theApp;

static void app_handler(Task task, MessageId id, Message message)
{
    switch (id)
    {
    case CL_INIT_CFM:
    {
        CL_INIT_CFM_T *cic = (CL_INIT_CFM_T *)message;
        theApp.state |= STATUS_CL_INIT_CFM;
        if (success == cic->status)
        {
            LOG_DEBUG(("Bluetooth version: "  ));
            switch(cic->version) {
                case bluetooth2_0:
                    LOG_DEBUG(("v2.0\n"));
                    ConnectionWriteInquiryMode(&theApp.task, inquiry_mode_rssi);
                    break;
                case bluetooth2_1:
                    LOG_DEBUG(("v2.1\n"));
                    ConnectionWriteInquiryMode(&theApp.task, inquiry_mode_eir);
                    break;
                default:
                    LOG_DEBUG(("unknow\n"));
                    ConnectionWriteInquiryMode(&theApp.task, inquiry_mode_eir);
            }
            

            /* Page scan for 100 slots out of every 150 */
            ConnectionWritePagescanActivity(150, 100);
            /* Make us discoverable for inquiries and pages */
            ConnectionWriteScanEnable(hci_scan_enable_inq_and_page);

            /* enable pin pair */
            ConnectionSmSetSecurityMode(&theApp.task, sec_mode0_off, hci_enc_mode_off);
            ConnectionSmSetSecurityLevel(protocol_l2cap, 0x0001, ssp_secl4_l0, TRUE, FALSE, FALSE);
            ConnectionSmSetSecurityLevel(protocol_rfcomm, 0x0001, ssp_secl4_l0, TRUE, FALSE, FALSE);
            
#ifdef BLUESTACK_VERSION_MAJOR
            ConnectionSmRegisterIncomingService(protocol_l2cap, 0x0001, sec_in_none);
            ConnectionSmRegisterIncomingService(protocol_rfcomm, 0x0001, sec_in_none);
            
            /* register psm for ds4 connection */
            ConnectionL2capRegisterRequest(&theApp.task, 0x11, 0);
            ConnectionL2capRegisterRequest(&theApp.task, 0x13, 0);
#else
            ConnectionSmRegisterIncomingService(protocol_l2cap, 0x0001, secl_none);
            ConnectionSmRegisterIncomingService(protocol_rfcomm, 0x0001, secl_none);
            
            /* register psm for ds4 connection */
            ConnectionL2capRegisterRequest(&theApp.task, 0x11);
            ConnectionL2capRegisterRequest(&theApp.task, 0x13);
#endif


            /* start scan */
            ConnectionInquire(&theApp.task, 0x9E8B33, 0, 8, 0 /*listen all type*/);
        }
        else
        {
            LOG_DEBUG(("Init failure: %d\n", cic->status));
        }
    }
    break;

    case CL_DM_WRITE_INQUIRY_MODE_CFM:
        theApp.state |= STATUS_CL_DM_WRITE_INQUIRY_MODE_CFM;
        break;

    case CL_DM_READ_INQUIRY_TX_CFM:
        theApp.state |= STATUS_CL_DM_READ_INQUIRY_TX_CFM;
        break;

    case CL_DM_LOCAL_NAME_COMPLETE:
    /*
        UartPrintf("%.*s\n", ((CL_DM_LOCAL_NAME_COMPLETE_T*)message)->size_local_name, ((CL_DM_LOCAL_NAME_COMPLETE_T*)message)->local_name);
        */
        break;    

    case CL_SM_SECURITY_LEVEL_CFM:
        LOG_DEBUG(("CL_SM_SECURITY_LEVEL_CFM\n" ));
        break;

    case CL_SDP_SERVICE_SEARCH_ATTRIBUTE_CFM:
        LOG_DEBUG(("CL_SDP_SERVICE_SEARCH_ATTRIBUTE_CFM\n" ));
        break;
        
    case CL_DM_ACL_OPENED_IND:
        LOG_DEBUG(("CL_DM_ACL_OPENED_IND\n" ));
        break;

    case CL_DM_ACL_CLOSED_IND:
        LOG_DEBUG(("CL_DM_ACL_CLOSED_IND\n" ));
        break;
    case CL_L2CAP_CONNECT_IND:
        LOG_DEBUG(("CL_L2CAP_CONNECT_IND\n" ));
        break;

        

    case CL_SM_PIN_CODE_IND:
        ConnectionSmPinCodeResponse(&((CL_SM_PIN_CODE_IND_T*)message)->bd_addr, 4, (uint8 *)"0000");
        LOG_DEBUG(("CL_SM_PIN_CODE_IND\n" ));
        break;

    case CL_L2CAP_UNREGISTER_CFM:
        LOG_DEBUG(("CL_L2CAP_UNREGISTER_CFM\n" ));
        break;
        
    case CL_L2CAP_REGISTER_CFM:
        LOG_DEBUG(("CL_L2CAP_REGISTER_CFM\n" ));
        break;

    case CL_SM_AUTHENTICATE_CFM:
        LOG_DEBUG(("CL_SM_AUTHENTICATE_CFM %d,%d\n", ((CL_SM_AUTHENTICATE_CFM_T*)message)->status, ((CL_SM_AUTHENTICATE_CFM_T*)message)->key_type ));
        break;

    case CL_L2CAP_DISCONNECT_IND:
        LedSpeed(1000,100);
        theApp.state &= ~STATUS_CL_L2CAP_CONNECT_CFM;
        /* start scan */
        ConnectionInquire(&theApp.task, 0x9E8B33, 0, 8, 0 /*listen all type*/);
        /*
        UartPrintf("Disconnect %d\n", ((CL_L2CAP_DISCONNECT_IND_T*)message)->status);
        */
        break;        

    case CL_L2CAP_CONNECT_CFM:
        LOG_DEBUG(("CL_L2CAP_CONNECT_CFM\n" ));  
        #define _L2CAP_C ((CL_L2CAP_CONNECT_CFM_T*)message)
        switch (_L2CAP_C->status) {    
            case l2cap_connect_success:
                LedSpeed(100,100);
                theApp.state |= STATUS_CL_L2CAP_CONNECT_CFM;
                if (_L2CAP_C->psm_local == 0x13)
                    PanicZero( StreamConnect( StreamSourceFromSink(_L2CAP_C->sink) , StreamUartSink() ));
                else if (_L2CAP_C->psm_local == 0x11)
                    PanicZero( StreamConnect( StreamUartSource(), _L2CAP_C->sink ));
                break;
            case l2cap_connect_failed:
                LOG_DEBUG(("l2cap_connect_failed\n" ));
                break;
            case l2cap_connect_failed_internal_error:
                LOG_DEBUG(("l2cap_connect_failed_internal_error\n" ));
                break;
            case l2cap_connect_failed_remote_reject:
                LOG_DEBUG(("Remote reject conn\n" ));
                break;
            default:
                LOG_DEBUG(("l2cap unknow status %d\n", _L2CAP_C->status ));
        }
        break;

    case CL_DM_REMOTE_NAME_COMPLETE:
        #define _RNAME ((CL_DM_REMOTE_NAME_COMPLETE_T*)message)
        if (_RNAME->status == hci_success) {
            /* 
            UartPrintf("%X:%X:%lX %.*s\n",
               _RNAME->bd_addr.nap,
               _RNAME->bd_addr.uap,
               _RNAME->bd_addr.lap,
               _RNAME->size_remote_name, _RNAME->remote_name);
               tmp_ucp = ((unsigned char *)&_RNAME->bd_addr);
            */
            if(strstr((char *)_RNAME->remote_name, "Wireless Controller")) {
#ifdef BLUESTACK_VERSION_MAJOR
                ConnectionL2capConnectRequest(&theApp.task, &_RNAME->bd_addr, 0x11, 0x11, 0, 0);
                ConnectionL2capConnectRequest(&theApp.task, &_RNAME->bd_addr, 0x13, 0x13, 0, 0);
#else /*Old version SDK*/
                ConnectionL2capConnectRequest(&theApp.task, &_RNAME->bd_addr, 0x11, 0x11, 0);
                ConnectionL2capConnectRequest(&theApp.task, &_RNAME->bd_addr, 0x13, 0x13, 0);
#endif
            }
        }        
        break;

    case CL_DM_INQUIRE_RESULT:
    {
        switch (((CL_DM_INQUIRE_RESULT_T *)message)->status)
        {
        case inquiry_status_ready:
            inquiry_complete(&theApp.task, (CL_DM_INQUIRE_RESULT_T *)message);
            /* UartPrintf("Scan complete\n");*/
            MessageSendLater( &theApp.task, loopConnect, 0, 10000 );
        case inquiry_status_result:
            inquiry_result(&theApp.task, (CL_DM_INQUIRE_RESULT_T *)message);
        }
        break;
    }

    case loopConnect:
        LOG_DEBUG(("LOOPCONNECT\n"));        
        if ((theApp.state & STATUS_CL_L2CAP_CONNECT_CFM) == 0)
            ConnectionInquire(&theApp.task, 0x9E8B33, 0, 8, 0 /*listen all type*/);
        break;

    default:
        LOG_DEBUG(("UMsg: 0x%0X\n", id));
        break;
    }
}

int main(void)
{
    theApp.task.handler = app_handler;
    theApp.state = state_initialised;
    /*UartStreamInit();*/
    LedInit();
    ConnectionInit(&theApp.task);
    MessageLoop();
    return 0;
}
