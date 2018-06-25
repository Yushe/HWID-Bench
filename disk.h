#ifndef HWID_DISK_H
#define HWID_DISK_H

#include <stdint.h>

/**
 * A response from querying a miniport for identification.
 */
typedef struct _IdentifyMiniportResponse {
    uint16_t pad_0;
    uint16_t pad_1;
    uint16_t pad_2;
    uint16_t pad_3;
    uint16_t pad_4;
    uint16_t pad_5;
    uint16_t pad_6;
    uint16_t pad_7[3];
    char serial[20];
} IdentifyMiniportResponse;

#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI << 16) + 0x0501)

#endif
