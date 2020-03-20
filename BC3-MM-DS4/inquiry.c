#include <connection.h>
#include <bdaddr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <panic.h>
#include "inquiry.h"
#include "log.h"
#include "main.h"
#include "uart.h"
#include "utils.h"


#define EIR_UUID16_PART         0x02    /* More 16-bit UUIDs available */
#define EIR_UUID16              0x03    /* Complete list of 16-bit UUIDs */
#define EIR_UUID32_PART         0x04    /* More 32-bit UUIDs available */
#define EIR_UUID32              0x05    /* Complete list of 32-bit UUIDs */
#define EIR_UUID128_PART        0x06    /* More 128-bit UUIDs available */
#define EIR_UUID128             0x07    /* Complete list of 128-bit UUIDs */
#define EIR_NAME_PART           0x08    /* Shortened local name */
#define EIR_NAME                0x09    /* Complete local name */
#define EIR_TXPOWER             0x0a    /* TX Power level */
#define EIR_OOB_COD             0x0d    /* SSP OOB Class of Device */
#define EIR_OOB_C               0x0e    /* SSP OOB Hash C */
#define EIR_OOB_R               0x0f    /* SSP OOB Randomizer R */
#define EIR_ID                  0x10    /* Device ID */
#define EIR_MANUFACTURER        0xff    /* Manufacturer Specific Data */

#define MAX_DEVICE 8

static uint8 sz_dev=0;
static bdaddr* dev=0;

void inquiry_result(TaskData *task ,CL_DM_INQUIRE_RESULT_T *ir)
{
    if (BdaddrIsZero(&ir->bd_addr))
        return;

    if (!dev)
        dev=PanicUnlessMalloc(MAX_DEVICE * sizeof(bdaddr));

    for (tmp_u8 = 0; tmp_u8 < sz_dev; tmp_u8++)
    {
        if (BdaddrIsSame(&dev[tmp_u8], &ir->bd_addr))
            return;
    }
    if (sz_dev < MAX_DEVICE)
    {
        memcpy(&dev[sz_dev], &ir->bd_addr, sizeof(bdaddr));
        sz_dev++;
    }

    ConnectionReadRemoteName(task, &ir->bd_addr);
}

void inquiry_complete(TaskData *task ,CL_DM_INQUIRE_RESULT_T *ir)
{   
    free(dev);
    dev=0;
    sz_dev = 0;
}

