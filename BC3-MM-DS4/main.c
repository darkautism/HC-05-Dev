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
        UartPrintf("%.*s\n", ((CL_DM_LOCAL_NAME_COMPLETE_T*)message)->size_local_name, ((CL_DM_LOCAL_NAME_COMPLETE_T*)message)->local_name);
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
        
    case CL_L2CAP_REGISTER_CFM:
        #define _L2CAP_REG ((CL_L2CAP_REGISTER_CFM_T*)message)
        if ( _L2CAP_REG->psm == theApp.reg_in) 
            theApp.state |= STATUS_CL_L2CAP_REGISTER_CFM_IN;
        if ( _L2CAP_REG->psm == theApp.reg_out) 
            theApp.state |= STATUS_CL_L2CAP_REGISTER_CFM_OUT;
        LOG_DEBUG(("CL_L2CAP_REGISTER_CFM\n" ));
        break;

    case CL_L2CAP_UNREGISTER_CFM:
        #define _L2CAP_UREG ((CL_L2CAP_UNREGISTER_CFM_T*)message)
        if ( _L2CAP_UREG->psm == theApp.reg_in) {
            theApp.state &= ~STATUS_CL_L2CAP_REGISTER_CFM_IN;
            theApp.reg_in = 0;
        }
        if ( _L2CAP_UREG->psm == theApp.reg_out) {
            theApp.state &= ~STATUS_CL_L2CAP_REGISTER_CFM_OUT;
            theApp.reg_out = 0;
        }
        LOG_DEBUG(("CL_L2CAP_UNREGISTER_CFM\n" ));
        break;
        

    case CL_SM_AUTHENTICATE_CFM:
        LOG_DEBUG(("CL_SM_AUTHENTICATE_CFM %d,%d\n", ((CL_SM_AUTHENTICATE_CFM_T*)message)->status, ((CL_SM_AUTHENTICATE_CFM_T*)message)->key_type ));
        break;

    case CL_L2CAP_DISCONNECT_IND:
        #define _L2CAP_DC ((CL_L2CAP_DISCONNECT_IND_T*)message)
        LedSpeed(1000,100);
        theApp.state &= ~STATUS_CL_L2CAP_CONNECT_CFM;
        if (theApp.reg_in != 0)
            ConnectionL2capUnregisterRequest(&theApp.task, theApp.reg_in);
        if (theApp.reg_out != 0)
            ConnectionL2capUnregisterRequest(&theApp.task, theApp.reg_out);
        theApp.reg_in = theApp.reg_out = 0;
        StreamDisconnect(StreamSourceFromSink(theApp.sink_in) , StreamUartSink());
        StreamDisconnect(StreamUartSource(), theApp.sink_out);
        theApp.sink_in = theApp.sink_out = 0;
        UartPrintf("DC\n");
        break;        

    case CL_L2CAP_CONNECT_CFM:
        LOG_DEBUG(("CL_L2CAP_CONNECT_CFM\n" ));  
        #define _L2CAP_C ((CL_L2CAP_CONNECT_CFM_T*)message)
        switch (_L2CAP_C->status) {    
            case l2cap_connect_success:
                LedSpeed(100,100);
                theApp.state |= STATUS_CL_L2CAP_CONNECT_CFM;
                if (_L2CAP_C->psm_local == theApp.reg_in) {
                    theApp.sink_in = _L2CAP_C->sink;
                    UartPrintf("IOK\n");
                }
                if (_L2CAP_C->psm_local == theApp.reg_out){
                    theApp.sink_out = _L2CAP_C->sink;
                    UartPrintf("OOK\n");
                }
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
            UartPrintf("%X:%X:%lX, %.*s\n",
                _RNAME->bd_addr.nap,
                _RNAME->bd_addr.uap,
                _RNAME->bd_addr.lap,
                _RNAME->size_remote_name,
                _RNAME->remote_name);
        }        
        break;

    case CL_DM_INQUIRE_RESULT:
    {
        switch (((CL_DM_INQUIRE_RESULT_T *)message)->status)
        {
        case inquiry_status_ready:
            inquiry_complete(&theApp.task, (CL_DM_INQUIRE_RESULT_T *)message);
            UartPrintf("SOK\n");
        case inquiry_status_result:
            inquiry_result(&theApp.task, (CL_DM_INQUIRE_RESULT_T *)message);
        }
        break;
    }

    default:
        LOG_DEBUG(("UMsg: 0x%0X\n", id));
        break;
    }
}

int main(void)
{
    theApp.task.handler = app_handler;
    theApp.state = state_initialised;
    UartStreamInit();
    LedInit();
    ConnectionInit(&theApp.task);
    MessageLoop();
    return 0;
}
