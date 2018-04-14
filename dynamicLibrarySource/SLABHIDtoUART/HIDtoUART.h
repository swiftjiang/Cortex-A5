#pragma once

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "SLABHIDDevice.h"
#include "ReportQueue.h"
#include "CriticalSectionLock.h"

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CHIDtoUART Class
/////////////////////////////////////////////////////////////////////////////

class CHIDtoUART
{
public:
    HID_DEVICE hid;
    CReportQueue queue;
    DWORD readTimeout;
    DWORD writeTimeout;
    BYTE partNumber;
    BYTE deviceVersion;
    BYTE cp2114DeviceApiVersion;
    BYTE cp2114FirmwareVersion;
    BYTE cp2114ConfigFormat;

    CHIDtoUART()
    {
        hid = NULL;
        readTimeout = 1000;
        writeTimeout = 1000;
        partNumber = 0x00;
        deviceVersion = 0x00;
        cp2114DeviceApiVersion = 0x00;
        cp2114FirmwareVersion = 0x00;
        cp2114ConfigFormat = 0x00;
    }
};
