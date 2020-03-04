#ifndef __MAIN_H
#define __MAIN_H
#include <connection.h>

typedef enum enum_state
{
    state_initialised,
    state_connecting,
    state_connected
} e_state;

#define STATUS_CL_INIT_CFM                  0x01
#define STATUS_CL_L2CAP_CONNECT_CFM         0x02
#define STATUS_CL_DM_WRITE_INQUIRY_MODE_CFM 0x04
#define STATUS_CL_DM_READ_INQUIRY_TX_CFM    0x08

typedef struct
{
    TaskData task;
    uint8 status;
    e_state state;

} APP_DATA_T;

extern APP_DATA_T theApp;

#endif
