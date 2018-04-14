// HIDtoUART.cpp : Defines the entry point for the DLL application.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "SLABCP2110.h"
#include "SLABCP2114.h"
#include "HIDtoUARTVersion.h"
#include "HIDtoUART.h"
#include "UsbReports.h"
#include "DeviceList.h"
#include <string.h>
#include <unistd.h>

/////////////////////////////////////////////////////////////////////////////
// Definitions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////

CDeviceList<CHIDtoUART> DeviceList;

/////////////////////////////////////////////////////////////////////////////
// Static Prototypes
/////////////////////////////////////////////////////////////////////////////

static DWORD GetTickCount();
static BYTE QueueReadData(CHIDtoUART* device, BYTE* buffer, DWORD size, DWORD numReports, DWORD readTimeout);
static HID_UART_STATUS SetCustomShortString(CHIDtoUART* device, BYTE rID, char* ascii, BYTE strlen);
static HID_UART_STATUS GetCustomShortString(CHIDtoUART* device, BYTE rID, char* ascii, BYTE* strlen);
static HID_UART_STATUS SetCustomLongString(CHIDtoUART* device, BYTE rID_1, BYTE rID_2, char* ascii, BYTE strlen);
static HID_UART_STATUS GetCustomLongString(CHIDtoUART* device, BYTE rID_1, BYTE rID_2, char* ascii, BYTE* strlen);

/////////////////////////////////////////////////////////////////////////////
// Feature Report Prototypes
/////////////////////////////////////////////////////////////////////////////

static HID_UART_STATUS SetUartEnable(CHIDtoUART* device, BOOL enable);
static HID_UART_STATUS GetUartEnable(CHIDtoUART* device, BOOL* enable);

static HID_UART_STATUS SetFlushBuffers(CHIDtoUART* device, BOOL flushTransmit, BOOL flushReceive);

static HID_UART_STATUS GetUartStatus(CHIDtoUART* device, WORD* transmitFifoSize, WORD* receiveFifoSize, BYTE* errorStatus, BYTE* lineBreakStatus);

static HID_UART_STATUS SetUartConfig(CHIDtoUART* device, DWORD baudRate, BYTE dataBits, BYTE parity, BYTE stopBits, BYTE flowControl);
static HID_UART_STATUS GetUartConfig(CHIDtoUART* device, DWORD* baudRate, BYTE* dataBits, BYTE* parity, BYTE* stopBits, BYTE* flowControl);

static HID_UART_STATUS SetStartBreak(CHIDtoUART* device, BYTE duration);
static HID_UART_STATUS SetStopBreak(CHIDtoUART* device);

static HID_UART_STATUS SetReset(CHIDtoUART* device);

static HID_UART_STATUS GetReadLatch(CHIDtoUART* device, WORD* latchValue);
static HID_UART_STATUS SetWriteLatch(CHIDtoUART* device, WORD latchValue, WORD latchMask);

static HID_UART_STATUS GetPartNumber(CHIDtoUART* device, BYTE* partNumber, BYTE* version);

static HID_UART_STATUS SetLock(CHIDtoUART* device, WORD lock);
static HID_UART_STATUS GetLock(CHIDtoUART* device, WORD* lock);

static HID_UART_STATUS SetUsbConfig(CHIDtoUART* device, WORD vid, WORD pid, BYTE power, BYTE powerMode, WORD releaseVersion, BYTE flushBuffers, BYTE mask);
static HID_UART_STATUS GetUsbConfig(CHIDtoUART* device, WORD* vid, WORD* pid, BYTE* power, BYTE* powerMode, WORD* releaseVersion, BYTE* flushBuffers);

static HID_UART_STATUS SetManufacturingString(CHIDtoUART* device, char* manufacturingString, BYTE strlen);
static HID_UART_STATUS GetManufacturingString(CHIDtoUART* device, char* manufacturingString, BYTE* strlen);

static HID_UART_STATUS SetProductString(CHIDtoUART* device, char* productString, BYTE strlen);
static HID_UART_STATUS GetProductString(CHIDtoUART* device, char* productString, BYTE* strlen);

static HID_UART_STATUS SetSerialString(CHIDtoUART* device, char* serialString, BYTE strlen);
static HID_UART_STATUS GetSerialString(CHIDtoUART* device, char* serialString, BYTE* strlen);

/////////////////////////////////////////////////////////////////////////////
// Static Functions
/////////////////////////////////////////////////////////////////////////////

// Get system tick count in milliseconds
DWORD GetTickCount()
{
    DWORD count;
    timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts))
    {
        return 0;
    }

    count = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

    return count;
}

BYTE QueueReadData(CHIDtoUART* device, BYTE* buffer, DWORD size, DWORD numReports, DWORD readTimeout)
{
    DWORD bytesRead = 0;
    DWORD reportSize = HidDevice_GetInputReportBufferLength(device->hid);
	BYTE	hidStatus	= HidDevice_GetInputReport_Interrupt_WithTimeout(device->hid, buffer, size, (WORD)numReports, &bytesRead, readTimeout);

    // Successfully read some data
    if (hidStatus == HID_DEVICE_SUCCESS || hidStatus == HID_DEVICE_TRANSFER_TIMEOUT)
    {
		// Successful return - check for data received...
        // Iterate through each report
        for (DWORD i = 0; i < bytesRead; i += reportSize)
        {
            // The first byte in the report is the data length
            DWORD dataLen = buffer[i];

            // Don't read past the end of the buffer
            // (if dataLen is erroneous)
            if ((i + dataLen) < bytesRead)
            {
                // Enqueue the raw UART data into the device object queue
                device->queue.Enqueue(&buffer[i + 1], dataLen);
            }
        }
    }

    return hidStatus;
}

// Set feature report for single report string customization

HID_UART_STATUS SetCustomShortString(CHIDtoUART* device, BYTE rID, char* ascii, BYTE strlen)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    report[0] = rID; // Report ID
    report[1] = (strlen * 2) + 2; // String Desc Length
    report[2] = 0x03; // String Desc ID

    // Convert ASCII to Unicode string
    for (int i = 0; i < strlen; i++)
    {
        report[i * 2 + 3] = ascii[i];
    }

    // Set feature report to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Get feature report for single report string customization

HID_UART_STATUS GetCustomShortString(CHIDtoUART* device, BYTE rID, char* ascii, BYTE* strlen)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = rID;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        // Get string length from report excluding
        // the report ID and 0x03 string descriptor
        // constant
        *strlen = (report[1] - 2) / 2;

        // Allocate a blank full-size unicode string buffer
        BYTE unicode[SHORT_STRING_LEN];
        memset(unicode, 0x00, sizeof (unicode));

        // Retrieve Unicode serial string from the device
        memcpy(unicode, &report[3], SHORT_STRING_LEN);

        // Convert Unicode to ASCII
        for (int i = 0; i < (SHORT_STRING_LEN / 2); i++)
        {
            ascii[i] = unicode[i * 2];
        }

        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for two-report string customization

HID_UART_STATUS SetCustomLongString(CHIDtoUART* device, BYTE rID_1, BYTE rID_2, char* ascii, BYTE strlen)
{
    HID_UART_STATUS status = HID_UART_SUCCESS;

    // Allocate a blank full-size unicode string buffer
    BYTE unicode[LONG_STRING_LEN_1 + LONG_STRING_LEN_2];
    memset(unicode, 0x00, sizeof (unicode));

    // Convert ASCII to Unicode
    for (int i = 0; i < strlen; i++)
    {
        unicode[i * 2] = ascii[i];
    }

    // Break the string into two reports
    BYTE report1[FEATURE_REPORT_LENGTH];
    BYTE report2[FEATURE_REPORT_LENGTH];

    report1[0] = rID_1; // Report ID
    report1[1] = (strlen * 2) + 2; // String Desc Length
    report1[2] = 0x03; // String Desc ID 
    memcpy(&report1[3], &unicode[0], LONG_STRING_LEN_1); // String Part 1

    report2[0] = rID_2; // Report ID
    memcpy(&report2[1], &unicode[LONG_STRING_LEN_1], LONG_STRING_LEN_2); // String Part 2

    // Set feature report1 to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report1, FEATURE_REPORT_LENGTH) != HID_DEVICE_SUCCESS)
    {
        status = HID_UART_DEVICE_IO_FAILED;
    }

    // Set feature report2 to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report2, FEATURE_REPORT_LENGTH) != HID_DEVICE_SUCCESS)
    {
        status = HID_UART_DEVICE_IO_FAILED;
    }

    return status;
}

// Get feature report for two-report string customization

HID_UART_STATUS GetCustomLongString(CHIDtoUART* device, BYTE rID_1, BYTE rID_2, char* ascii, BYTE* strlen)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report1[FEATURE_REPORT_LENGTH];
    memset(report1, 0x00, sizeof (report1));

    BYTE report2[FEATURE_REPORT_LENGTH];
    memset(report2, 0x00, sizeof (report2));

    // Report ID
    report1[0] = rID_1;
    report2[0] = rID_2;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report1, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS &&
            HidDevice_GetFeatureReport_Control(device->hid, report2, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        // Get string length from first report
        // excluding the report ID and 0x03 string descriptor
        // constant
        *strlen = (report1[1] - 2) / 2;

        // Allocate a blank full-size unicode string buffer
        BYTE unicode[LONG_STRING_LEN_1 + LONG_STRING_LEN_2];
        memset(unicode, 0x00, sizeof (unicode));

        // Retrieve Unicode string from the device
        memcpy(&((BYTE*) unicode)[0], &report1[3], LONG_STRING_LEN_1);
        memcpy(&((BYTE*) unicode)[LONG_STRING_LEN_1], &report2[1], LONG_STRING_LEN_2);

        // Convert Unicode to ASCII
        for (int i = 0; i < ((LONG_STRING_LEN_1 + LONG_STRING_LEN_2) / 2); i++)
        {
            ascii[i] = unicode[i * 2];
        }

        status = HID_UART_SUCCESS;
    }
    else
    {
        // Failed return empty string
        *strlen = 0;
    }

    return status;
}

/////////////////////////////////////////////////////////////////////////////
// Feature Report Functions
/////////////////////////////////////////////////////////////////////////////

// Set feature report for HidUart_SetUartEnable()

HID_UART_STATUS SetUartEnable(CHIDtoUART* device, BOOL enable)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE enableMask;

    if (enable)
    {
        enableMask = 0x01;
    }
    else
    {
        enableMask = 0x00;
    }

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Prepare feature report
    report[0] = GETSET_UART_ENABLE; // Report ID
    report[1] = enableMask; // Enable option

    // Send feature report to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Get feature report for HidUart_GetUartEnable()

HID_UART_STATUS GetUartEnable(CHIDtoUART* device, BOOL* enable)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = GETSET_UART_ENABLE;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        // Extract the UART enable status from the report
        *enable = report[1];

        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for HidUart_FlushBuffers()

HID_UART_STATUS SetFlushBuffers(CHIDtoUART* device, BOOL flushTransmit, BOOL flushReceive)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE flushMask = 0x00;

    if (flushTransmit)
        flushMask |= PURGE_TRANSMIT_MASK;
    if (flushReceive)
        flushMask |= PURGE_RECEIVE_MASK;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Prepare feature report
    report[0] = PURGE_FIFOS; // Report ID
    report[1] = flushMask; // Flush options

    // Send feature report to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Get feature report for HidUart_GetUartStatus()

HID_UART_STATUS GetUartStatus(CHIDtoUART* device, WORD* transmitFifoSize, WORD* receiveFifoSize, BYTE* errorStatus, BYTE* lineBreakStatus)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = GET_UART_STATUS;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        // Extract the UART settings from the report
        *transmitFifoSize = MAKEWORD(report[2], report[1]);
        *receiveFifoSize = MAKEWORD(report[4], report[3]);
        *errorStatus = report[5];
        *lineBreakStatus = report[6];

        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for HidUart_SetUartConfig()

HID_UART_STATUS SetUartConfig(CHIDtoUART* device, DWORD baudRate, BYTE dataBits, BYTE parity, BYTE stopBits, BYTE flowControl)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Prepare feature report
    report[0] = GETSET_UART_CONFIG; // Report ID
    report[1] = (BYTE) (baudRate >> 24); // Baud rate (big-endian)
    report[2] = (BYTE) (baudRate >> 16);
    report[3] = (BYTE) (baudRate >> 8);
    report[4] = (BYTE) (baudRate);
    report[5] = parity; // Parity
    report[6] = flowControl; // Flow Control
    report[7] = dataBits; // Data Bits
    report[8] = stopBits; // Stop Bits

    // Send feature report to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Get feature report for HidUart_GetUartConfig()

HID_UART_STATUS GetUartConfig(CHIDtoUART* device, DWORD* baudRate, BYTE* dataBits, BYTE* parity, BYTE* stopBits, BYTE* flowControl)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = GETSET_UART_CONFIG;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        *baudRate = ((DWORD) report[1] << 24) | ((DWORD) report[2] << 16) | ((DWORD) report[3] << 8) | ((DWORD) report[4]);
        *parity = report[5];
        *flowControl = report[6];
        *dataBits = report[7];
        *stopBits = report[8];

        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for HidUart_StartBreak()

HID_UART_STATUS SetStartBreak(CHIDtoUART* device, BYTE duration)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Prepare feature report
    report[0] = TRANS_LINE_BREAK; // Report ID
    report[1] = duration; // Line break duration

    // Send feature report to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for HidUart_StopBreak()

HID_UART_STATUS SetStopBreak(CHIDtoUART* device)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Prepare feature report
    report[0] = STOP_LINE_BREAK; // Report ID
    report[1] = 0x00; // Unused byte

    // Send feature report to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for HidUart_Reset()

HID_UART_STATUS SetReset(CHIDtoUART* device)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = RESET_DEVICE;
    report[1] = 0x00; // Currently unused (for re-enumeration)

    // Send feature report to the device
    BYTE hidStatus = HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH);

    // The device may reset too fast to respond successfully
    //if (hidStatus == HID_DEVICE_SUCCESS || hidStatus == HID_DEVICE_TRANSFER_FAILED)
    if (hidStatus == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Get feature report for HidUart_ReadLatch()

HID_UART_STATUS GetReadLatch(CHIDtoUART* device, WORD* latchValue)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = GET_GPIO_VALUES;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        // Extract the GPIO latch values
        *latchValue = MAKEWORD(report[2], report[1]);

        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for HidUart_WriteLatch()

HID_UART_STATUS SetWriteLatch(CHIDtoUART* device, WORD latchValue, WORD latchMask)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = SET_GPIO_VALUES;
    report[1] = HIBYTE(latchValue);
    report[2] = LOBYTE(latchValue);
    report[3] = HIBYTE(latchMask);
    report[4] = LOBYTE(latchMask);

    // Set feature report to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Get feature report for HidUart_GetPartNumber()

HID_UART_STATUS GetPartNumber(CHIDtoUART* device, BYTE* partNumber, BYTE* version)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = GET_VER_INFO;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        // Retrieve version information from the device
        *partNumber = report[1];
        *version = report[2];

        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for HidUart_SetLock()

HID_UART_STATUS SetLock(CHIDtoUART* device, WORD lock)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = GETSET_LOCK_BYTE; // Report ID
    report[1] = HIBYTE(lock); // Lock bytes
    report[2] = LOBYTE(lock);

    // Set feature report to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Get feature report for HidUart_GetLock()

HID_UART_STATUS GetLock(CHIDtoUART* device, WORD* lock)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = GETSET_LOCK_BYTE;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        // Retrieve lock bytes from the device
        *lock = MAKEWORD(report[2], report[1]);

        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for HidUart_SetUsbConfig()

HID_UART_STATUS SetUsbConfig(CHIDtoUART* device, WORD vid, WORD pid, BYTE power, BYTE powerMode, WORD releaseVersion, BYTE flushBuffers, BYTE mask)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = GETSET_USB_CONFIG; // Report ID
    report[1] = LOBYTE(vid); // VID (Little-endian)
    report[2] = HIBYTE(vid);
    report[3] = LOBYTE(pid); // PID (Little-endian)
    report[4] = HIBYTE(pid);
    report[5] = power; // Power (mA/2)
    report[6] = powerMode; // Power Mode (0: bus, 1: self (VREG off), 2: self (VREG on))
    report[7] = HIBYTE(releaseVersion); // Release Version (major.minor)
    report[8] = LOBYTE(releaseVersion);
    report[9] = flushBuffers; // Flush buffers mask
    report[10] = mask; // Field mask

    // Set feature report to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Get feature report for HidUart_GetUsbConfig()

HID_UART_STATUS GetUsbConfig(CHIDtoUART* device, WORD* vid, WORD* pid, BYTE* power, BYTE* powerMode, WORD* releaseVersion, BYTE* flushBuffers)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof (report));

    // Report ID
    report[0] = GETSET_USB_CONFIG;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        // Retrieve USB customization from the device
        *vid = MAKEWORD(report[1], report[2]); // VID (Little-endian)
        *pid = MAKEWORD(report[3], report[4]); // PID (Little-endian)
        *power = report[5]; // Power (mA/2)
        *powerMode = report[6]; // Power Mode (0: bus, 1: self (VREG off), 2: self (VREG on))
        *releaseVersion = MAKEWORD(report[8], report[7]); // Release Version (major.minor)
        *flushBuffers = report[9]; // Flush buffers mask

        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for HidUart_SetManufacturingString()

HID_UART_STATUS SetManufacturingString(CHIDtoUART* device, char* manufacturingString, BYTE strlen)
{
    HID_UART_STATUS status = SetCustomLongString(device, GETSET_MANU1, GETSET_MANU2, manufacturingString, strlen);
    return status;
}

// Get feature report for HidUart_GetManufacturingString()

HID_UART_STATUS GetManufacturingString(CHIDtoUART* device, char* manufacturingString, BYTE* strlen)
{
    HID_UART_STATUS status = GetCustomLongString(device, GETSET_MANU1, GETSET_MANU2, manufacturingString, strlen);
    return status;
}

// Set feature report for HidUart_SetProductString()

HID_UART_STATUS SetProductString(CHIDtoUART* device, char* productString, BYTE strlen)
{
    HID_UART_STATUS status = SetCustomLongString(device, GETSET_PROD1, GETSET_PROD2, productString, strlen);
    return status;
}

// Get feature report for HidUart_GetProductString()

HID_UART_STATUS GetProductString(CHIDtoUART* device, char* productString, BYTE* strlen)
{
    HID_UART_STATUS status = GetCustomLongString(device, GETSET_PROD1, GETSET_PROD2, productString, strlen);
    return status;
}

// Set feature report for HidUart_SetSerialString()

HID_UART_STATUS SetSerialString(CHIDtoUART* device, char* serialString, BYTE strlen)
{
    HID_UART_STATUS status = SetCustomShortString(device, GETSET_SERSTR, serialString, strlen);
    return status;
}

// Get feature report for HidUart_GetSerialString()

HID_UART_STATUS GetSerialString(CHIDtoUART* device, char* serialString, BYTE* strlen)
{
    HID_UART_STATUS status = GetCustomShortString(device, GETSET_SERSTR, serialString, strlen);
    return status;
}

/////////////////////////////////////////////////////////////////////////////
// Exported Library Functions
/////////////////////////////////////////////////////////////////////////////

// HidUart_GetNumDevices
HID_UART_STATUS HidUart_GetNumDevices(DWORD* numDevices, WORD vid, WORD pid)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;

    // Check parameters
    if (numDevices)
    {
        // Get the number of devices with matching VID/PID
        *numDevices = HidDevice_GetNumHidDevices(vid, pid);

        status = HID_UART_SUCCESS;
    }
    else
    {
        status = HID_UART_INVALID_PARAMETER;
    }

    return status;
}

// HidUart_GetString
HID_UART_STATUS HidUart_GetString(DWORD deviceNum, WORD vid, WORD pid, char* deviceString, DWORD options)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;

    // Check parameters
    if (deviceString &&
            options >= HID_UART_GET_VID_STR && options <= HID_UART_GET_PRODUCT_STR)
    {
        // Get the product string
        BYTE hidStatus = HidDevice_GetHidString(deviceNum, vid, pid, (BYTE) options, deviceString, HID_UART_DEVICE_STRLEN);

        // Translate HID status to HID_UART status
        switch (hidStatus)
        {
        case HID_DEVICE_SUCCESS: status = HID_UART_SUCCESS;
            break;
        case HID_DEVICE_NOT_FOUND: status = HID_UART_DEVICE_NOT_FOUND;
            break;
        default: status = HID_UART_DEVICE_ACCESS_ERROR;
        }
    }
    else
    {
        status = HID_UART_INVALID_PARAMETER;
    }

    return status;
}

// HidUart_GetOpenedString
HID_UART_STATUS HidUart_GetOpenedString(HID_UART_DEVICE device, char* deviceString, DWORD options)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check parameters
        if (deviceString &&
                options >= HID_UART_GET_VID_STR && options <= HID_UART_GET_PRODUCT_STR)
        {
            // Get the product string
            BYTE hidStatus = HidDevice_GetString(hidUart->hid, (BYTE) options, deviceString, HID_UART_DEVICE_STRLEN);

            // Translate HID status to HID_UART status
            switch (hidStatus)
            {
            case HID_DEVICE_SUCCESS: status = HID_UART_SUCCESS;
                break;
            default: status = HID_UART_DEVICE_ACCESS_ERROR;
            }
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetIndexedString
HID_UART_STATUS HidUart_GetIndexedString(DWORD deviceNum, WORD vid, WORD pid, DWORD stringIndex, char* deviceString)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;

    // Check parameters
    if (deviceString)
    {
        // Get the indexed string
        BYTE hidStatus = HidDevice_GetHidIndexedString(deviceNum, vid, pid, stringIndex, deviceString, HID_UART_DEVICE_STRLEN);

        // Translate HID status to HID_UART status
        switch (hidStatus)
        {
        case HID_DEVICE_SUCCESS: status = HID_UART_SUCCESS;
            break;
        case HID_DEVICE_NOT_FOUND: status = HID_UART_DEVICE_NOT_FOUND;
            break;
        default: status = HID_UART_DEVICE_ACCESS_ERROR;
        }
    }
    else
    {
        status = HID_UART_INVALID_PARAMETER;
    }

    return status;
}

// HidUart_GetOpenedIndexedString
HID_UART_STATUS HidUart_GetOpenedIndexedString(HID_UART_DEVICE device, DWORD stringIndex, char* deviceString)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check parameters
        if (deviceString)
        {
            // Get the indexed string
            BYTE hidStatus = HidDevice_GetIndexedString(hidUart->hid, stringIndex, deviceString, HID_UART_DEVICE_STRLEN);

            // Translate HID status to HID_UART status
            switch (hidStatus)
            {
            case HID_DEVICE_SUCCESS: status = HID_UART_SUCCESS;
                break;
            default: status = HID_UART_DEVICE_ACCESS_ERROR;
            }
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetAttributes
HID_UART_STATUS HidUart_GetAttributes(DWORD deviceNum, WORD vid, WORD pid, WORD* deviceVid, WORD* devicePid, WORD* deviceReleaseNumber)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;

    // Check parameters
    if (deviceVid && devicePid && deviceReleaseNumber)
    {
        // Get the attributes
        BYTE hidStatus = HidDevice_GetHidAttributes(deviceNum, vid, pid, deviceVid, devicePid, deviceReleaseNumber);

        // Translate HID status to HID_UART status
        switch (hidStatus)
        {
        case HID_DEVICE_SUCCESS: status = HID_UART_SUCCESS;
            break;
        case HID_DEVICE_NOT_FOUND: status = HID_UART_DEVICE_NOT_FOUND;
            break;
        default: status = HID_UART_DEVICE_ACCESS_ERROR;
        }
    }
    else
    {
        status = HID_UART_INVALID_PARAMETER;
    }

    return status;
}

// HidUart_GetOpenedAttributes
HID_UART_STATUS HidUart_GetOpenedAttributes(HID_UART_DEVICE device, WORD* deviceVid, WORD* devicePid, WORD* deviceReleaseNumber)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check parameters
        if (deviceVid && devicePid && deviceReleaseNumber)
        {
            // Get the attributes
            BYTE hidStatus = HidDevice_GetAttributes(hidUart->hid, deviceVid, devicePid, deviceReleaseNumber);

            // Translate HID status to HID_UART status
            switch (hidStatus)
            {
            case HID_DEVICE_SUCCESS: status = HID_UART_SUCCESS;
                break;
            default: status = HID_UART_DEVICE_ACCESS_ERROR;
            }
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_Open
HID_UART_STATUS HidUart_Open(HID_UART_DEVICE* device, DWORD deviceNum, WORD vid, WORD pid)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;

    // Check parameters
    if (device)
    {
        // Create a new device object and add it to the device list
        CHIDtoUART* hidUart = DeviceList.Construct();

        // Check device object
        if (DeviceList.Validate(hidUart))
        {
            BYTE partNumber;
            BYTE version;

            // Open the device by index (use max input report buffer)
            BYTE hidStatus = HidDevice_Open(&hidUart->hid, deviceNum, vid, pid, MAX_REPORT_REQUEST_XP);

            // Openned successfully
            if (hidStatus == HID_DEVICE_SUCCESS)
            {
                // Get part number for future use
                status = GetPartNumber(hidUart, &partNumber, &version);

                if (status == HID_UART_SUCCESS)
                {
                    // Only support CP2110 and CP2114
                    if (partNumber == HID_UART_PART_CP2110 ||
                            partNumber == HID_UART_PART_CP2114)
                    {
                        // Save part number
                        // Used to determine device functionality
                        hidUart->partNumber = partNumber;

                        // Automatically enable the UART on open
                        status = SetUartEnable(hidUart, TRUE);

						// For CP2114, also issue the GET_DEVICE_VERSIONS report to get firmware and config format versions
						if ((status == HID_UART_SUCCESS) && (partNumber == HID_UART_PART_CP2114))
						{
							BYTE apiVersion, firmwareVersion, configFormat;
							status = CP2114_GetVersions(hidUart, &apiVersion, &firmwareVersion, &configFormat);
							if (status == HID_UART_SUCCESS)
							{
								hidUart->cp2114DeviceApiVersion = apiVersion;
								hidUart->cp2114FirmwareVersion = firmwareVersion;
								hidUart->cp2114ConfigFormat = configFormat;
							}
						}
                    }
                    else
                    {
                        status = HID_UART_DEVICE_NOT_SUPPORTED;
                    }
                }
            }
                // Failed to open device because the device was not found
            else if (hidStatus == HID_DEVICE_NOT_FOUND)
            {
                status = HID_UART_DEVICE_NOT_FOUND;
            }
                // Could not access device (i.e. already opened)
            else
            {
                status = HID_UART_DEVICE_ACCESS_ERROR;
            }

            // Device opened and initialized successfully
            if (status == HID_UART_SUCCESS)
            {
				// Read timeout doesn't matter since we specify it explicitly on every read.
				// Set interrupt write timeouts to the default write timeout (1000 ms)
				HidDevice_SetTimeouts(hidUart->hid, 0 /* read TO */, hidUart->writeTimeout);

                // Return the device object pointer to the user
                *device = hidUart;
            }
                // Failure occurred
                // Clean up
            else
            {
                // Close the handle
                HidDevice_Close(hidUart->hid);

                // Delete the device object and
                // remove the device reference from the device list
                DeviceList.Destruct(hidUart);
            }
        }
        else
        {
            status = HID_UART_INVALID_DEVICE_OBJECT;
        }
    }
    else
    {
        status = HID_UART_INVALID_PARAMETER;
    }

    return status;
}

// HidUart_Close
HID_UART_STATUS HidUart_Close(HID_UART_DEVICE device)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Close the device
        BYTE hidStatus = HidDevice_Close(hidUart->hid);

        switch (hidStatus)
        {
        case HID_DEVICE_SUCCESS: status = HID_UART_SUCCESS;
            break;
        case HID_DEVICE_HANDLE_ERROR: status = HID_UART_INVALID_HANDLE;
            break;
        default: status = HID_UART_DEVICE_ACCESS_ERROR;
        }

        // Deallocate the device object, remove the device reference
        // from the device list
        DeviceList.Destruct(hidUart);
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_IsOpened
HID_UART_STATUS HidUart_IsOpened(HID_UART_DEVICE device, BOOL* opened)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (opened)
        {
            // Check if the device is opened
            *opened = HidDevice_IsOpened(hidUart->hid);

            status = HID_UART_SUCCESS;
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_SetUartEnable
HID_UART_STATUS HidUart_SetUartEnable(HID_UART_DEVICE device, BOOL enable)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        status = SetUartEnable(hidUart, enable);
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetUartEnable
HID_UART_STATUS HidUart_GetUartEnable(HID_UART_DEVICE device, BOOL* enable)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointer
        if (enable)
        {
            status = GetUartEnable(hidUart, enable);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_Read
HID_UART_STATUS HidUart_Read(HID_UART_DEVICE device, BYTE* buffer, DWORD numBytesToRead, DWORD* numBytesRead)
{
	HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
	CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

	// Check device object
	if (DeviceList.Validate(hidUart))
	{
		// Check pointers
		if (buffer && numBytesRead)
		{
			*numBytesRead = 0;

			// Check parameter value
			if (numBytesToRead >= HID_UART_MIN_READ_SIZE &&
				numBytesToRead <= HID_UART_MAX_READ_SIZE)
			{
				DWORD	numReports	= HidDevice_GetMaxReportRequest(hidUart->hid);
				DWORD	requestSize	= numReports * HidDevice_GetInputReportBufferLength(hidUart->hid);
				BYTE*	rxBuffer	= new BYTE[requestSize];
				
				// Get the start time to implement read timeouts
				DWORD startTime   = GetTickCount();
				DWORD elapsTime   = 0;
				DWORD readTimeout = hidUart->readTimeout;

				// Keep calling QueueReadData until enough data received or time is up.
				do
				{
					// Check if there is enough data to satisfy the read
					if (hidUart->queue.Size() >= numBytesToRead)
					{
						break;
					}

					// Read the max number of interrupt reports (HID read timeout set to 1 ms),
					// Parse data out of each report
					// Add raw UART data to the device object queue
					BYTE hidStatus = QueueReadData(hidUart, rxBuffer, requestSize, numReports, readTimeout - elapsTime);

					// Trap any device object errors
					if (hidStatus != HID_DEVICE_SUCCESS && hidStatus != HID_DEVICE_TRANSFER_TIMEOUT)
					{
						status = HID_UART_READ_ERROR;
						break;
					}

					// GetTickCount rolls over every few days, but it's not a problem - the unsigned
					// subtraction will still produce the correct value. Unless it rolls over twice,
					// which can't happen because timeouts themselves are not large enough.
					elapsTime = GetTickCount() - startTime;

				} while (readTimeout > elapsTime);

				if (status != HID_UART_READ_ERROR)
				{
					// Copy requested data to user buffer
					hidUart->queue.Dequeue((BYTE*)buffer, numBytesToRead, numBytesRead);

					if (*numBytesRead < numBytesToRead)
					{
						// The loop ended because enough time passed.
						// Still the caller will get whatever data was received.
						status = HID_UART_READ_TIMED_OUT;
					}
					else
					{
						// The loop ended because enough data was received
						status = HID_UART_SUCCESS;
					}
				}

				delete [] rxBuffer;
			}
			else
			{
				status = HID_UART_INVALID_REQUEST_LENGTH;
			}
		}
		else
		{
			status = HID_UART_INVALID_PARAMETER;
		}
	}
	else
	{
		status = HID_UART_INVALID_DEVICE_OBJECT;
	}

	return status;
}

// HidUart_Write
HID_UART_STATUS HidUart_Write(HID_UART_DEVICE device, BYTE* buffer, DWORD numBytesToWrite, DWORD* numBytesWritten)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (buffer && numBytesWritten)
        {
            *numBytesWritten = 0;

            // Check parameter value
            if (numBytesToWrite >= HID_UART_MIN_WRITE_SIZE &&
                    numBytesToWrite <= HID_UART_MAX_WRITE_SIZE)
            {
                // Interrupt report buffer
                BYTE report[INTERRUPT_REPORT_LENGTH];

                // Keep track of the number of bytes written so far
                DWORD bytesWritten = 0;

                // Get the starting time for write timeout
                DWORD startTime = GetTickCount();

                // Break the write up into 63 byte interrupt reports
                // while-loop can exit with status:
                // HID_UART_UNKNOWN_ERROR
                // HID_UART_WRITE_ERROR
                while ((GetTickCount() - startTime) < hidUart->writeTimeout)
                {
                    // Check if all bytes have been written
                    if (bytesWritten == numBytesToWrite)
                    {
                        break;
                    }

                    // Prepare the interrupt report buffer for the next write
                    DWORD reportLen = MIN(numBytesToWrite - bytesWritten, 63);
                    memcpy(report + 1, (BYTE*) buffer + bytesWritten, reportLen);
                    report[0] = (BYTE) reportLen;

                    // Send the interrupt report (HID write timeout set to writeTimeout)
                    BYTE hidStatus = HidDevice_SetOutputReport_Interrupt(hidUart->hid, report, INTERRUPT_REPORT_LENGTH);

                    if (hidStatus == HID_DEVICE_SUCCESS)
                    {
                        // Update number of bytes written
                        bytesWritten += reportLen;
                    }
                        // Check for write error (a timeout in this case is
                        // also considered a write error)
                    else
                    {
                        status = HID_UART_WRITE_ERROR;
                        break;
                    }
                }

                // Return the number of bytes written
                *numBytesWritten = bytesWritten;

                if (status != HID_UART_WRITE_ERROR)
                {
                    // Check if the write operation succeeded or timed out
                    if (bytesWritten < numBytesToWrite)
                    {
                        status = HID_UART_WRITE_TIMED_OUT;
                    }
                    else
                    {
                        status = HID_UART_SUCCESS;
                    }
                }
            }
            else
            {
                status = HID_UART_INVALID_REQUEST_LENGTH;
            }
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_FlushBuffers
HID_UART_STATUS HidUart_FlushBuffers(HID_UART_DEVICE device, BOOL flushTransmit, BOOL flushReceive)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Send flush buffers feature report
        if (SetFlushBuffers(hidUart, flushTransmit, flushReceive) == HID_UART_SUCCESS)
        {
            if (flushReceive)
            {
                // Flush the HID input report buffer
                HidDevice_FlushBuffers(hidUart->hid);

                // Flush the internal UART read queue
                hidUart->queue.Clear();
            }

            status = HID_UART_SUCCESS;
        }
        else
        {
            status = HID_UART_DEVICE_IO_FAILED;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_CancelIo
HID_UART_STATUS HidUart_CancelIo(HID_UART_DEVICE device)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Cancel pending reads/writes issued in this thread
        if (HidDevice_CancelIo(hidUart->hid))
        {
            status = HID_UART_SUCCESS;
        }
        else
        {
            status = HID_UART_DEVICE_IO_FAILED;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_SetTimeouts
HID_UART_STATUS HidUart_SetTimeouts(HID_UART_DEVICE device, DWORD readTimeout, DWORD writeTimeout)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Set HidUart_Read() and HidUart_Write() timeouts
        // These are the overall read and write timeouts
        hidUart->readTimeout = readTimeout;
        hidUart->writeTimeout = writeTimeout;

		// Read timeout doesn't matter since we specify it explicitly on every read.
		// Set the HID get/set interrupt report timeouts
		HidDevice_SetTimeouts(hidUart->hid, 0 /* read TO */, writeTimeout);

        status = HID_UART_SUCCESS;
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetTimeouts
HID_UART_STATUS HidUart_GetTimeouts(HID_UART_DEVICE device, DWORD* readTimeout, DWORD* writeTimeout)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (readTimeout && writeTimeout)
        {
            // Retrieve the HidUart_Read() and HidUart_Write() timeouts
            *readTimeout = hidUart->readTimeout;
            *writeTimeout = hidUart->writeTimeout;

            status = HID_UART_SUCCESS;
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetUARTStatus
HID_UART_STATUS HidUart_GetUartStatus(HID_UART_DEVICE device, WORD* transmitFifoSize, WORD* receiveFifoSize, BYTE* errorStatus, BYTE* lineBreakStatus)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (transmitFifoSize && receiveFifoSize && errorStatus && lineBreakStatus)
        {
            // Retrieve the UART status from the device
            status = GetUartStatus(hidUart, transmitFifoSize, receiveFifoSize, errorStatus, lineBreakStatus);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_SetUARTConfig
HID_UART_STATUS HidUart_SetUartConfig(HID_UART_DEVICE device, DWORD baudRate, BYTE dataBits, BYTE parity, BYTE stopBits, BYTE flowControl)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check parameters
        if (dataBits >= HID_UART_FIVE_DATA_BITS && dataBits <= HID_UART_EIGHT_DATA_BITS &&
                parity >= HID_UART_NO_PARITY && parity <= HID_UART_SPACE_PARITY &&
                stopBits >= HID_UART_SHORT_STOP_BIT && stopBits <= HID_UART_LONG_STOP_BIT &&
                flowControl >= HID_UART_NO_FLOW_CONTROL && flowControl <= HID_UART_RTS_CTS_FLOW_CONTROL)
        {
            status = SetUartConfig(hidUart, baudRate, dataBits, parity, stopBits, flowControl);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetUARTConfig
HID_UART_STATUS HidUart_GetUartConfig(HID_UART_DEVICE device, DWORD* baudRate, BYTE* dataBits, BYTE* parity, BYTE* stopBits, BYTE* flowControl)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (baudRate && dataBits && parity && stopBits && flowControl)
        {
            status = GetUartConfig(hidUart, baudRate, dataBits, parity, stopBits, flowControl);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_StartBreak
HID_UART_STATUS HidUart_StartBreak(HID_UART_DEVICE device, BYTE duration)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check parameter
        if (duration >= 0 && duration <= 125)
        {
            status = SetStartBreak(hidUart, duration);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_StopBreak
HID_UART_STATUS HidUart_StopBreak(HID_UART_DEVICE device)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        status = SetStopBreak(hidUart);
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_Reset
HID_UART_STATUS HidUart_Reset(HID_UART_DEVICE device)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        status = SetReset(hidUart);
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_ReadLatch
HID_UART_STATUS HidUart_ReadLatch(HID_UART_DEVICE device, WORD* latchValue)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointer
        if (latchValue)
        {
            status = GetReadLatch(hidUart, latchValue);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_WriteLatch
HID_UART_STATUS HidUart_WriteLatch(HID_UART_DEVICE device, WORD latchValue, WORD latchMask)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        status = SetWriteLatch(hidUart, latchValue, latchMask);
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetPartNumber
HID_UART_STATUS HidUart_GetPartNumber(HID_UART_DEVICE device, BYTE* partNumber, BYTE* version)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (partNumber && version)
        {
            status = GetPartNumber(hidUart, partNumber, version);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetLibraryVersion
HID_UART_STATUS HidUart_GetLibraryVersion(BYTE* major, BYTE* minor, BOOL* release)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;

    // Check pointers
    if (major && minor && release)
    {
        *major = HID_UART_VERSION_MAJOR;
        *minor = HID_UART_VERSION_MINOR;
        *release = HID_UART_VERSION_RELEASE;

        status = HID_UART_SUCCESS;
    }
    else
    {
        status = HID_UART_INVALID_PARAMETER;
    }

    return status;
}

// HidUart_GetHidLibraryVersion
HID_UART_STATUS HidUart_GetHidLibraryVersion(BYTE* major, BYTE* minor, BOOL* release)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;

    // Check pointers
    if (major && minor && release)
    {
        // Get HID DLL library version
        BYTE hidStatus = HidDevice_GetHidLibraryVersion(major, minor, release);

        // Translate HID status to HID_UART status
        switch (hidStatus)
        {
        case HID_DEVICE_SUCCESS: status = HID_UART_SUCCESS;
            break;
        case HID_DEVICE_INVALID_BUFFER_SIZE: status = HID_UART_INVALID_PARAMETER;
            break;
        default: status = HID_UART_DEVICE_ACCESS_ERROR;
        }
    }
    else
    {
        status = HID_UART_INVALID_PARAMETER;
    }

    return status;
}

// HidUart_GetHidGuid
HID_UART_STATUS HidUart_GetHidGuid(void* guid)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;

    // Check pointers
    if (guid)
    {
        // Return the HID GUID
        HidDevice_GetHidGuid(guid);

        status = HID_UART_SUCCESS;
    }
    else
    {
        status = HID_UART_INVALID_PARAMETER;
    }

    return status;
}

// HidUart_SetLock
HID_UART_STATUS HidUart_SetLock(HID_UART_DEVICE device, WORD lock)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        status = SetLock(hidUart, lock);
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetLock
HID_UART_STATUS HidUart_GetLock(HID_UART_DEVICE device, WORD* lock)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (lock)
        {
            status = GetLock(hidUart, lock);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_SetUsbConfig
HID_UART_STATUS HidUart_SetUsbConfig(HID_UART_DEVICE device, WORD vid, WORD pid, BYTE power, BYTE powerMode, WORD releaseVersion, BYTE flushBuffers, BYTE mask)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check parameters
        if (powerMode >= HID_UART_BUS_POWER && powerMode <= HID_UART_SELF_POWER_VREG_EN)
        {
            // Check power parameter if bus powered
            if (powerMode == HID_UART_BUS_POWER && power > HID_UART_BUS_POWER_MAX)
            {
                status = HID_UART_INVALID_PARAMETER;
            }
            else
            {
                status = SetUsbConfig(hidUart, vid, pid, power, powerMode, releaseVersion, flushBuffers, mask);
            }
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetUsbConfig
HID_UART_STATUS HidUart_GetUsbConfig(HID_UART_DEVICE device, WORD* vid, WORD* pid, BYTE* power, BYTE* powerMode, WORD* releaseVersion, BYTE* flushBuffers)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (vid && pid && power && powerMode && releaseVersion && flushBuffers)
        {
            status = GetUsbConfig(hidUart, vid, pid, power, powerMode, releaseVersion, flushBuffers);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_SetManufacturingString
HID_UART_STATUS HidUart_SetManufacturingString(HID_UART_DEVICE device, char* manufacturingString, BYTE strlen)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (manufacturingString)
        {
            // Check parameters
            if (strlen <= HID_UART_MFG_STRLEN)
            {
                status = SetManufacturingString(hidUart, manufacturingString, strlen);
            }
            else
            {
                status = HID_UART_INVALID_PARAMETER;
            }
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetManufacturingString
HID_UART_STATUS HidUart_GetManufacturingString(HID_UART_DEVICE device, char* manufacturingString, BYTE* strlen)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (manufacturingString && strlen)
        {
            status = GetManufacturingString(hidUart, manufacturingString, strlen);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_SetProductString
HID_UART_STATUS HidUart_SetProductString(HID_UART_DEVICE device, char* productString, BYTE strlen)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (productString)
        {
            // Check parameters
            if (strlen <= HID_UART_PRODUCT_STRLEN)
            {
                status = SetProductString(hidUart, productString, strlen);
            }
            else
            {
                status = HID_UART_INVALID_PARAMETER;
            }
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetProductString
HID_UART_STATUS HidUart_GetProductString(HID_UART_DEVICE device, char* productString, BYTE* strlen)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (productString && strlen)
        {
            status = GetProductString(hidUart, productString, strlen);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_SetSerialString
HID_UART_STATUS HidUart_SetSerialString(HID_UART_DEVICE device, char* serialString, BYTE strlen)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (serialString)
        {
            // Check parameters
            if (strlen <= HID_UART_SERIAL_STRLEN)
            {
                status = SetSerialString(hidUart, serialString, strlen);
            }
            else
            {
                status = HID_UART_INVALID_PARAMETER;
            }
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// HidUart_GetSerialString
HID_UART_STATUS HidUart_GetSerialString(HID_UART_DEVICE device, char* serialString, BYTE* strlen)
{
    HID_UART_STATUS status = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART* hidUart = ((CHIDtoUART*) device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (serialString && strlen)
        {
            status = GetSerialString(hidUart, serialString, strlen);
        }
        else
        {
            status = HID_UART_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}
