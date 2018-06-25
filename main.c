#include <stdio.h>
#include <windows.h>
#include <ntddndis.h>
#include <ntddscsi.h>
#include <stdint.h>
#include "disk.h"

/**
 * Retrieves all HDD serials.
 */
void retrieve_hdd_serials() {
    // TODO
}

/**
 * Retrieves all SSD serials.
 */
void retrieve_ssd_serials() {
    uint32_t serials_found = 0;
    for (uint32_t disk = 0; disk < 16; disk++) {
        char device_name[256];
        sprintf(device_name, "\\\\.\\Scsi%d:", disk);

        HANDLE handle = CreateFile(device_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (handle) {
            for (uint8_t drv = 0; drv < 2; drv++) {
                char req[1024];
                memset(req, 0, 1024);

                SRB_IO_CONTROL *req_header = (SRB_IO_CONTROL *) req;
                req_header->HeaderLength = sizeof(SRB_IO_CONTROL);
                req_header->Timeout = 10000;
                req_header->Length = 512;
                req_header->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
                strncpy((char *) req_header->Signature, "SCSIDISK", 8);

                SENDCMDINPARAMS *req_params = (SENDCMDINPARAMS *) (req + sizeof(SRB_IO_CONTROL));
                req_params->irDriveRegs.bCommandReg = 0xEC;
                req_params->bDriveNumber = drv;

                uint32_t request_output_len = 0;
                NTSTATUS status = DeviceIoControl(handle, IOCTL_SCSI_MINIPORT, req, 1024, req, 1024, &request_output_len, NULL);
                if (!status) {
                    continue;
                }

                SENDCMDOUTPARAMS *resp = (SENDCMDOUTPARAMS *) (req + sizeof(SRB_IO_CONTROL));
                IdentifyMiniportResponse *resp_buffer = (IdentifyMiniportResponse *) (resp->bBuffer);

                serials_found += 1;
                printf("Miniport serial (disk #%d drv #%d): %s\n", disk, drv, resp_buffer->serial);
            }
        }
    }


    printf("Found %d SSD serials\n\n", serials_found);
}

/**
 * Retrieves all mac addresses.
 */
void retrieve_mac_addresses() {
    HKEY key_handle;
    NTSTATUS status = RegOpenKey(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Adapters", &key_handle);
    if (status != ERROR_SUCCESS) {
        printf("Failed to open adapters registry key\n");
        return;
    }

    char adapter_key_name[256];
    uint32_t reg_key_buf_len = 39;
    uint32_t macs_found = 0;
    for (uint32_t i = 0; (status = RegEnumKeyEx(key_handle, i, adapter_key_name, &reg_key_buf_len, NULL, NULL, NULL, NULL)) != ERROR_NO_MORE_ITEMS; reg_key_buf_len = 39, i++) {
        if (status == ERROR_MORE_DATA || reg_key_buf_len != 38) {
            continue;
        }

        if (status != ERROR_SUCCESS) {
            break;
        }

        char device_name[256];
        sprintf(device_name, "\\\\.\\%s", adapter_key_name);
        HANDLE handle = CreateFile(device_name, FILE_GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (!handle) {
            continue;
        }

        NDIS_STATISTICS_VALUE req;
        req.Oid = OID_802_3_PERMANENT_ADDRESS;
        req.DataLength = 10;

        char resp_buffer[1024];
        uint32_t resp_buffer_len = 0;
        status = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &req, sizeof(NDIS_STATISTICS_VALUE), resp_buffer, 256, &resp_buffer_len, NULL);
        if (!status || resp_buffer_len != 6) {
            continue;
        }

        /* null mac */
        if (resp_buffer[0] == 0 && resp_buffer[1] == 0 && resp_buffer[2] == 0 && resp_buffer[3] == 0 && resp_buffer[4] == 0 && resp_buffer[5] == 0) {
            continue;
        }

        macs_found += 1;
        printf("Permanent mac: %02x%02x%02x%02x%02x%02x\n",
               resp_buffer[0] & 0xFF, resp_buffer[1] & 0xFF, resp_buffer[2] & 0xFF,
               resp_buffer[3] & 0xFF, resp_buffer[4] & 0xFF, resp_buffer[5] & 0xFF);
    }

    printf("Found %d permanent mac addresses\n\n", macs_found);
}

int main() {
    printf("== Copyright clrlslwi 2018\n");
    printf("== Retrieving all possible HWIDs\n\n");

    printf("= Retrieving SSD serials\n");
    retrieve_ssd_serials();

    printf("= Retrieving permanent mac addresses\n");
    retrieve_mac_addresses();

    getchar();
    return 0;
}